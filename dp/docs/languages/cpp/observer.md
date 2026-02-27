# Observer Pattern - C++ Implementation

## 개요

Observer 패턴은 객체 간의 일대다 의존성을 정의하여, 한 객체의 상태가 변할 때 의존하는 모든 객체들이 자동으로 알림을 받고 업데이트되는 행동 패턴입니다. C++에서는 스마트 포인터, 함수형 프로그래밍, 템플릿 등을 활용하여 타입 안전하고 메모리 안전한 Observer 패턴을 구현할 수 있습니다.

## 구조

Observer 패턴의 주요 구성 요소:
- **Subject**: 옵저버들을 관리하고 상태 변화를 알리는 인터페이스
- **Observer**: 알림을 받을 객체들의 인터페이스
- **ConcreteSubject**: Subject의 구현체, 실제 상태를 가지고 있음
- **ConcreteObserver**: Observer의 구현체, 알림을 받아 처리함

## C++ 구현

### 1. 전통적인 Observer 패턴

#### 기본 인터페이스
```cpp
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& message) = 0;
    virtual std::string getName() const = 0;
};

class Subject {
private:
    std::vector<std::weak_ptr<Observer>> observers;

public:
    void attach(std::shared_ptr<Observer> observer) {
        observers.push_back(observer);
        std::cout << "Observer " << observer->getName() << " attached" << std::endl;
    }

    void detach(std::shared_ptr<Observer> observer) {
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
        // 자동으로 만료된 weak_ptr 정리와 함께 알림 전송
        observers.erase(
            std::remove_if(observers.begin(), observers.end(),
                [&message](const std::weak_ptr<Observer>& weak_obs) {
                    if (auto obs = weak_obs.lock()) {
                        obs->update(message);
                        return false; // 유효한 옵저버 유지
                    }
                    return true; // 만료된 옵저버 제거
                }),
            observers.end()
        );
    }
};
```

#### 구체적인 구현체들
```cpp
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
```

### 2. 타입 안전한 Observer (템플릿 활용)

```cpp
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

private:
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

    void notifyPriceUpdate(const StockData& data) {
        cleanupAndNotify([&data](auto observer) {
            observer->onPriceUpdate(data);
        });
    }
};
```

### 3. 모던 C++ Observer (std::function 활용)

```cpp
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

    void updateScore(const std::string& playerId, int newScore) {
        if (gameActive && playerScores.find(playerId) != playerScores.end()) {
            playerScores[playerId] = newScore;
            eventPublisher.publish({GameEvent::SCORE_CHANGED, playerId, newScore});
        }
    }
};
```

## 사용 예제

### 기본 사용법
```cpp
NewsAgency newsAgency;

auto emailNotifier = std::make_shared<EmailNotifier>("user@example.com");
auto smsNotifier = std::make_shared<SMSNotifier>("+1-555-0123");

newsAgency.attach(emailNotifier);
newsAgency.attach(smsNotifier);

newsAgency.setNews("Major earthquake hits California");
newsAgency.setNews("New COVID variant discovered");

// SMS 알림 해제
newsAgency.detach(smsNotifier);
newsAgency.setNews("Stock market reaches all-time high");
```

### 주식 시장 예제
```cpp
StockMarket market;

auto portfolio = std::make_shared<Portfolio>("Retirement Fund");
portfolio->addHolding("AAPL", 100);

auto tradingBot = std::make_shared<TradingBot>("Momentum", -5.0, 5.0);

market.subscribe(portfolio);
market.subscribe(tradingBot);

market.updatePrice("AAPL", 150.00);
market.updatePrice("AAPL", 142.50); // 5% 하락 - 매수 신호
```

### 람다 함수를 활용한 Observer
```cpp
GameSession gameSession;

// 람다 함수로 이벤트 구독
gameSession.subscribeToEvents([](const GameEvent& event) {
    std::cout << "🎮 Game Log: " << event.toString() << std::endl;
});

gameSession.subscribeToEvents([](const GameEvent& event) {
    if (event.type == GameEvent::SCORE_CHANGED) {
        std::cout << "🏆 Leaderboard Update: " << event.playerId
                  << " scored " << event.value << " points!" << std::endl;
    }
});

gameSession.playerJoin("Alice");
gameSession.updateScore("Alice", 100);
```

## C++의 장점

### 1. 메모리 안전성
```cpp
// weak_ptr을 사용한 자동 정리
std::vector<std::weak_ptr<Observer>> observers;

// 소멸된 옵저버 자동 제거
observers.erase(
    std::remove_if(observers.begin(), observers.end(),
        [&message](const std::weak_ptr<Observer>& weak_obs) {
            if (auto obs = weak_obs.lock()) {
                obs->update(message);
                return false; // 유효한 옵저버 유지
            }
            return true; // 만료된 옵저버 제거
        }),
    observers.end()
);
```

### 2. 타입 안전성
```cpp
// 강타입 이벤트 시스템
template<typename EventType>
class EventPublisher {
    std::vector<std::function<void(const EventType&)>> callbacks;
};

// 컴파일 타임에 타입 체크
EventPublisher<GameEvent> gameEvents;
EventPublisher<StockData> stockEvents;
```

### 3. 성능 최적화
```cpp
// 이동 의미론 활용
void attach(std::shared_ptr<Observer> observer) {
    observers.push_back(std::move(observer));
}

// 범위 기반 for 루프
for (const auto& callback : callbacks) {
    callback(event);
}
```

### 4. 함수형 프로그래밍 지원
```cpp
// std::function으로 유연한 콜백
std::function<void(const EventType&)> callback = [](const auto& event) {
    // 처리 로직
};

// 람다 캡처로 컨텍스트 전달
auto userId = getCurrentUserId();
subscribe([userId](const GameEvent& event) {
    if (event.playerId == userId) {
        // 특정 사용자 이벤트 처리
    }
});
```

### 5. RAII와 예외 안전성
```cpp
class SafeObserver {
private:
    std::shared_ptr<Subject> subject_;

public:
    SafeObserver(std::shared_ptr<Subject> subject) : subject_(subject) {
        subject_->attach(shared_from_this());
    }

    ~SafeObserver() {
        if (auto subject = subject_.lock()) {
            subject->detach(shared_from_this());
        }
    }
};
```

### 6. 템플릿 기반 일반화
```cpp
template<typename T>
class TypedSubject {
private:
    std::vector<std::function<void(const T&)>> observers;

public:
    void subscribe(std::function<void(const T&)> observer) {
        observers.push_back(std::move(observer));
    }

    void notify(const T& data) {
        for (const auto& observer : observers) {
            observer(data);
        }
    }
};
```

## 적용 상황

### 1. 사용자 인터페이스 (GUI)
- 모델-뷰 아키텍처
- 이벤트 기반 프로그래밍

### 2. 실시간 시스템
- 주식 거래 시스템
- 게임 이벤트 처리
- IoT 센서 데이터 처리

### 3. 비즈니스 로직
- 주문 처리 시스템
- 워크플로우 엔진
- 알림 시스템

### 4. 시스템 모니터링
- 로그 수집기
- 성능 모니터링
- 경고 시스템

## 장점과 단점

### 장점
- **느슨한 결합**: Subject와 Observer 간의 독립성
- **동적 관계**: 런타임에 Observer 추가/제거 가능
- **브로드캐스트 통신**: 일대다 통신 지원
- **개방-폐쇄 원칙**: 새로운 Observer 타입 쉽게 추가

### 단점
- **메모리 누수 위험**: 순환 참조 가능성 (weak_ptr로 해결)
- **성능 오버헤드**: 많은 Observer가 있을 때
- **업데이트 순서**: Observer 호출 순서 보장 어려움
- **복잡성 증가**: 많은 이벤트 타입이 있을 때

## 관련 패턴

### Mediator 패턴과의 차이
- **Observer**: 일대다 통신, Subject가 중심
- **Mediator**: 다대다 통신, Mediator가 중재

### MVC 패턴과의 관계
- Model이 Subject 역할
- View가 Observer 역할
- Controller가 중재자 역할

Observer 패턴은 C++의 스마트 포인터와 함수형 프로그래밍 기능을 활용하여 안전하고 유연한 이벤트 기반 시스템을 구축하는 데 매우 유용합니다.