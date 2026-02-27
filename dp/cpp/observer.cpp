#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <random>
#include <mutex>

// Traditional Observer Pattern
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& message) = 0;
    virtual std::string getName() const = 0;
};

class Subject {
private:
    std::vector<std::weak_ptr<Observer>> observers;
    std::mutex mutable mutex_; // Mutex for thread safety

public:
    virtual ~Subject() = default;

    void attach(std::shared_ptr<Observer> observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        observers.push_back(observer);
        std::cout << "Observer " << observer->getName() << " attached" << std::endl;
    }

    void detach(std::shared_ptr<Observer> observer) {
        std::lock_guard<std::mutex> lock(mutex_);
        observers.erase(
            std::remove_if(observers.begin(), observers.end(),
                [&observer](const std::weak_ptr<Observer>& weak_obs) {
                    auto obs = weak_obs.lock();
                    return !obs || obs == observer;
                }),
            observers.end()
        );
        std::cout << "Observer " << observer->getName() << " detached" << std::endl;
    }

    void notify(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "Notifying " << observers.size() << " observers..." << std::endl;

        // Clean up expired weak_ptrs and notify active observers
        observers.erase(
            std::remove_if(observers.begin(), observers.end(),
                [&message](const std::weak_ptr<Observer>& weak_obs) {
                    if (auto obs = weak_obs.lock()) {
                        obs->update(message);
                        return false; // Keep this observer
                    }
                    return true; // Remove expired observer
                }),
            observers.end()
        );
    }

    size_t getObserverCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return observers.size();
    }
};

// Concrete Observers
class EmailNotifier : public Observer {
private:
    std::string email;

public:
    explicit EmailNotifier(const std::string& email) : email(email) {}

    void update(const std::string& message) override {
        std::cout << "📧 Email to " << email << ": " << message << std::endl;
    }

    std::string getName() const override {
        return "EmailNotifier(" + email + ")";
    }
};

class SMSNotifier : public Observer {
private:
    std::string phoneNumber;

public:
    explicit SMSNotifier(const std::string& phone) : phoneNumber(phone) {}

    void update(const std::string& message) override {
        std::cout << "📱 SMS to " << phoneNumber << ": " << message << std::endl;
    }

    std::string getName() const override {
        return "SMSNotifier(" + phoneNumber + ")";
    }
};

class PushNotifier : public Observer {
private:
    std::string deviceId;

public:
    explicit PushNotifier(const std::string& device) : deviceId(device) {}

    void update(const std::string& message) override {
        std::cout << "🔔 Push to " << deviceId << ": " << message << std::endl;
    }

    std::string getName() const override {
        return "PushNotifier(" + deviceId + ")";
    }
};

// Concrete Subject
class NewsAgency : public Subject {
private:
    std::string latestNews;

public:
    void setNews(const std::string& news) {
        latestNews = news;
        std::cout << "\n📰 Breaking News: " << news << std::endl;
        notify("Breaking News: " + news);
    }

    const std::string& getLatestNews() const {
        return latestNews;
    }
};

// Example 2: Stock Market with typed events
struct StockData {
    std::string symbol;
    double price;
    double change;
    double percentChange;

    std::string toString() const {
        return symbol + ": $" + std::to_string(price) +
               " (" + (change >= 0 ? "+" : "") + std::to_string(change) +
               ", " + (change >= 0 ? "+" : "") + std::to_string(percentChange) + "%)";
    }
};

class StockObserver {
public:
    virtual ~StockObserver() = default;
    virtual void onPriceUpdate(const StockData& data) = 0;
    virtual void onVolumeAlert(const std::string& symbol, long volume) = 0;
    virtual std::string getName() const = 0;
};

class StockMarket {
private:
    std::vector<std::weak_ptr<StockObserver>> observers;
    std::unordered_map<std::string, StockData> stocks;

public:
    void subscribe(std::shared_ptr<StockObserver> observer) {
        observers.push_back(observer);
        std::cout << "Stock observer " << observer->getName() << " subscribed" << std::endl;
    }

    void unsubscribe(std::shared_ptr<StockObserver> observer) {
        observers.erase(
            std::remove_if(observers.begin(), observers.end(),
                [&observer](const std::weak_ptr<StockObserver>& weak_obs) {
                    auto obs = weak_obs.lock();
                    return !obs || obs == observer;
                }),
            observers.end()
        );
        std::cout << "Stock observer " << observer->getName() << " unsubscribed" << std::endl;
    }

    void updatePrice(const std::string& symbol, double newPrice) {
        StockData& stock = stocks[symbol];
        double oldPrice = stock.price;
        stock.symbol = symbol;
        stock.price = newPrice;
        stock.change = newPrice - oldPrice;
        stock.percentChange = oldPrice != 0 ? (stock.change / oldPrice) * 100 : 0;

        notifyPriceUpdate(stock);
    }

    void alertVolume(const std::string& symbol, long volume) {
        notifyVolumeAlert(symbol, volume);
    }

private:
    void notifyPriceUpdate(const StockData& data) {
        cleanupAndNotify([&data](auto observer) {
            observer->onPriceUpdate(data);
        });
    }

    void notifyVolumeAlert(const std::string& symbol, long volume) {
        cleanupAndNotify([&symbol, volume](auto observer) {
            observer->onVolumeAlert(symbol, volume);
        });
    }

    template<typename Func>
    void cleanupAndNotify(Func notifyFunc) {
        observers.erase(
            std::remove_if(observers.begin(), observers.end(),
                [&notifyFunc](const std::weak_ptr<StockObserver>& weak_obs) {
                    if (auto obs = weak_obs.lock()) {
                        notifyFunc(obs);
                        return false;
                    }
                    return true;
                }),
            observers.end()
        );
    }
};

class Portfolio : public StockObserver {
private:
    std::string name;
    std::unordered_map<std::string, int> holdings; // symbol -> quantity

public:
    explicit Portfolio(const std::string& name) : name(name) {}

    void addHolding(const std::string& symbol, int quantity) {
        holdings[symbol] = quantity;
    }

    void onPriceUpdate(const StockData& data) override {
        if (holdings.find(data.symbol) != holdings.end()) {
            int quantity = holdings[data.symbol];
            double value = quantity * data.price;
            std::cout << "💼 Portfolio " << name << ": " << data.symbol
                      << " (" << quantity << " shares) = $" << value
                      << " [" << data.toString() << "]" << std::endl;
        }
    }

    void onVolumeAlert(const std::string& symbol, long volume) override {
        if (holdings.find(symbol) != holdings.end()) {
            std::cout << "🔊 Portfolio " << name << ": High volume alert for "
                      << symbol << " - " << volume << " shares traded" << std::endl;
        }
    }

    std::string getName() const override {
        return "Portfolio(" + name + ")";
    }
};

class TradingBot : public StockObserver {
private:
    std::string strategy;
    double buyThreshold;
    double sellThreshold;

public:
    TradingBot(const std::string& strategy, double buyThreshold, double sellThreshold)
        : strategy(strategy), buyThreshold(buyThreshold), sellThreshold(sellThreshold) {}

    void onPriceUpdate(const StockData& data) override {
        if (data.percentChange <= buyThreshold) {
            std::cout << "🤖 TradingBot (" << strategy << "): BUY signal for "
                      << data.symbol << " at $" << data.price << std::endl;
        } else if (data.percentChange >= sellThreshold) {
            std::cout << "🤖 TradingBot (" << strategy << "): SELL signal for "
                      << data.symbol << " at $" << data.price << std::endl;
        }
    }

    void onVolumeAlert(const std::string& symbol, long volume) override {
        std::cout << "🤖 TradingBot (" << strategy << "): Analyzing volume spike for "
                  << symbol << " - " << volume << " shares" << std::endl;
    }

    std::string getName() const override {
        return "TradingBot(" + strategy + ")";
    }
};

// Example 3: Modern C++ Observer with std::function
template<typename EventType>
class EventPublisher {
private:
    std::vector<std::function<void(const EventType&)>> callbacks;

public:
    void subscribe(std::function<void(const EventType&)> callback) {
        callbacks.push_back(callback);
    }

    void publish(const EventType& event) {
        for (const auto& callback : callbacks) {
            callback(event);
        }
    }

    size_t getSubscriberCount() const {
        return callbacks.size();
    }
};

struct GameEvent {
    enum Type { PLAYER_JOINED, PLAYER_LEFT, GAME_STARTED, GAME_ENDED, SCORE_CHANGED };
    Type type;
    std::string playerId;
    int value;

    std::string toString() const {
        static const char* typeNames[] = {
            "PLAYER_JOINED", "PLAYER_LEFT", "GAME_STARTED", "GAME_ENDED", "SCORE_CHANGED"
        };
        return std::string(typeNames[type]) + " - Player: " + playerId + " - Value: " + std::to_string(value);
    }
};

class GameSession {
private:
    EventPublisher<GameEvent> eventPublisher;
    std::unordered_map<std::string, int> playerScores;
    bool gameActive;

public:
    GameSession() : gameActive(false) {}

    void subscribeToEvents(std::function<void(const GameEvent&)> callback) {
        eventPublisher.subscribe(callback);
    }

    void playerJoin(const std::string& playerId) {
        playerScores[playerId] = 0;
        eventPublisher.publish({GameEvent::PLAYER_JOINED, playerId, 0});
    }

    void playerLeave(const std::string& playerId) {
        playerScores.erase(playerId);
        eventPublisher.publish({GameEvent::PLAYER_LEFT, playerId, 0});
    }

    void startGame() {
        gameActive = true;
        eventPublisher.publish({GameEvent::GAME_STARTED, "", 0});
    }

    void endGame() {
        gameActive = false;
        eventPublisher.publish({GameEvent::GAME_ENDED, "", 0});
    }

    void updateScore(const std::string& playerId, int newScore) {
        if (gameActive && playerScores.find(playerId) != playerScores.end()) {
            playerScores[playerId] = newScore;
            eventPublisher.publish({GameEvent::SCORE_CHANGED, playerId, newScore});
        }
    }

    size_t getEventSubscriberCount() const {
        return eventPublisher.getSubscriberCount();
    }
};

int main() {
    std::cout << "=== Observer Pattern Demo ===" << std::endl;

    // Example 1: Traditional Observer Pattern - News Agency
    std::cout << "\n1. Traditional Observer Pattern - News Agency:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    NewsAgency newsAgency;

    auto emailNotifier = std::make_shared<EmailNotifier>("user@example.com");
    auto smsNotifier = std::make_shared<SMSNotifier>("+1-555-0123");
    auto pushNotifier = std::make_shared<PushNotifier>("device_12345");

    newsAgency.attach(emailNotifier);
    newsAgency.attach(smsNotifier);
    newsAgency.attach(pushNotifier);

    newsAgency.setNews("Major earthquake hits California");
    newsAgency.setNews("New COVID variant discovered");

    std::cout << "\nDetaching SMS notifier..." << std::endl;
    newsAgency.detach(smsNotifier);

    newsAgency.setNews("Stock market reaches all-time high");

    // Example 2: Stock Market Observer
    std::cout << "\n\n2. Stock Market Observer with Typed Events:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    StockMarket market;

    auto portfolio1 = std::make_shared<Portfolio>("Retirement Fund");
    portfolio1->addHolding("AAPL", 100);
    portfolio1->addHolding("GOOGL", 50);

    auto portfolio2 = std::make_shared<Portfolio>("Growth Fund");
    portfolio2->addHolding("AAPL", 200);
    portfolio2->addHolding("TSLA", 30);

    auto tradingBot = std::make_shared<TradingBot>("Momentum", -5.0, 5.0); // Buy on 5% drop, sell on 5% gain

    market.subscribe(portfolio1);
    market.subscribe(portfolio2);
    market.subscribe(tradingBot);

    std::cout << "\nSimulating stock price updates:" << std::endl;
    market.updatePrice("AAPL", 150.00);
    market.updatePrice("AAPL", 142.50); // 5% drop - should trigger buy signal
    market.updatePrice("GOOGL", 2800.00);
    market.updatePrice("TSLA", 800.00);
    market.updatePrice("TSLA", 840.00); // 5% gain - should trigger sell signal

    std::cout << "\nSimulating volume alerts:" << std::endl;
    market.alertVolume("AAPL", 10000000);
    market.alertVolume("TSLA", 5000000);

    // Example 3: Modern C++ Observer with Lambda Functions
    std::cout << "\n\n3. Modern C++ Observer with Lambda Functions:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    GameSession gameSession;

    // Subscribe with lambda functions
    gameSession.subscribeToEvents([](const GameEvent& event) {
        std::cout << "🎮 Game Log: " << event.toString() << std::endl;
    });

    gameSession.subscribeToEvents([](const GameEvent& event) {
        if (event.type == GameEvent::SCORE_CHANGED) {
            std::cout << "🏆 Leaderboard Update: " << event.playerId
                      << " scored " << event.value << " points!" << std::endl;
        }
    });

    gameSession.subscribeToEvents([](const GameEvent& event) {
        if (event.type == GameEvent::PLAYER_JOINED) {
            std::cout << "🎉 Welcome " << event.playerId << " to the game!" << std::endl;
        } else if (event.type == GameEvent::PLAYER_LEFT) {
            std::cout << "👋 " << event.playerId << " has left the game." << std::endl;
        }
    });

    std::cout << "\nGame session events:" << std::endl;
    gameSession.playerJoin("Alice");
    gameSession.playerJoin("Bob");
    gameSession.startGame();
    gameSession.updateScore("Alice", 100);
    gameSession.updateScore("Bob", 150);
    gameSession.updateScore("Alice", 200);
    gameSession.playerLeave("Bob");
    gameSession.endGame();

    // Example 4: Automatic Observer Cleanup Demo
    std::cout << "\n\n4. Automatic Observer Cleanup Demo:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    NewsAgency cleanupDemo;

    {
        auto tempObserver = std::make_shared<EmailNotifier>("temp@example.com");
        cleanupDemo.attach(tempObserver);
        std::cout << "Observer count: " << cleanupDemo.getObserverCount() << std::endl;
        cleanupDemo.setNews("Temporary observer active");
    } // tempObserver goes out of scope here

    std::cout << "\nAfter observer destruction:" << std::endl;
    cleanupDemo.setNews("Temporary observer should be cleaned up");
    std::cout << "Observer count after cleanup: " << cleanupDemo.getObserverCount() << std::endl;

    // Performance comparison
    std::cout << "\n\n5. Observer Pattern Benefits:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "✓ Loose coupling between subjects and observers" << std::endl;
    std::cout << "✓ Dynamic relationships - can add/remove observers at runtime" << std::endl;
    std::cout << "✓ Broadcast communication - one subject can notify many observers" << std::endl;
    std::cout << "✓ Automatic cleanup with weak_ptr prevents memory leaks" << std::endl;
    std::cout << "✓ Type-safe events with template-based modern approach" << std::endl;
    std::cout << "✓ Functional programming support with std::function" << std::endl;

    return 0;
}
