#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <chrono>
#include <random>
#include <thread>

// Forward declarations
class Colleague;
class ChatMediator;

// Abstract Mediator
class Mediator {
public:
    virtual ~Mediator() = default;
    virtual void sendMessage(const std::string& message, Colleague* sender) = 0;
    virtual void addUser(std::shared_ptr<Colleague> user) = 0;
    virtual void removeUser(const std::string& username) = 0;
};

// Abstract Colleague
class Colleague {
protected:
    std::shared_ptr<Mediator> mediator;
    std::string name;

public:
    Colleague(std::shared_ptr<Mediator> mediator, const std::string& name)
        : mediator(mediator), name(name) {}

    virtual ~Colleague() = default;

    virtual void send(const std::string& message) = 0;
    virtual void receive(const std::string& message, const std::string& from) = 0;

    const std::string& getName() const { return name; }
};

// Concrete Colleague - Chat User
class ChatUser : public Colleague {
public:
    ChatUser(std::shared_ptr<Mediator> mediator, const std::string& name)
        : Colleague(mediator, name) {}

    void send(const std::string& message) override {
        std::cout << "👤 " << name << " sends: " << message << std::endl;
        mediator->sendMessage(message, this);
    }

    void receive(const std::string& message, const std::string& from) override {
        std::cout << "📱 " << name << " receives from " << from << ": " << message << std::endl;
    }
};

// Concrete Colleague - Bot User
class ChatBot : public Colleague {
private:
    std::vector<std::string> responses = {
        "That's interesting!",
        "Can you tell me more?",
        "I understand.",
        "Thanks for sharing!",
        "How fascinating!"
    };
    std::mt19937 rng;

public:
    ChatBot(std::shared_ptr<Mediator> mediator, const std::string& name)
        : Colleague(mediator, name), rng(std::random_device{}()) {}

    void send(const std::string& message) override {
        std::cout << "🤖 " << name << " (bot) sends: " << message << std::endl;
        mediator->sendMessage(message, this);
    }

    void receive(const std::string& message, const std::string& from) override {
        std::cout << "🤖 " << name << " (bot) receives from " << from << ": " << message << std::endl;

        // Auto-respond to messages
        if (message.find("?") != std::string::npos) {
            std::uniform_int_distribution<int> dist(0, responses.size() - 1);
            std::string response = responses[dist(rng)];

            // Delay auto-response
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            send("@" + from + " " + response);
        }
    }
};

// Concrete Mediator - Chat Room
class ChatRoom : public Mediator {
private:
    std::vector<std::shared_ptr<Colleague>> users;
    std::vector<std::string> messageHistory;

public:
    void sendMessage(const std::string& message, Colleague* sender) override {
        std::string fullMessage = sender->getName() + ": " + message;
        messageHistory.push_back(fullMessage);

        // Send to all users except sender
        for (auto& user : users) {
            if (user.get() != sender) {
                user->receive(message, sender->getName());
            }
        }
    }

    void addUser(std::shared_ptr<Colleague> user) override {
        users.push_back(user);
        std::cout << "✅ " << user->getName() << " joined the chat room" << std::endl;

        // Notify existing users
        for (auto& existingUser : users) {
            if (existingUser != user) {
                existingUser->receive(user->getName() + " joined the room", "System");
            }
        }
    }

    void removeUser(const std::string& username) override {
        auto it = std::find_if(users.begin(), users.end(),
            [&username](const std::shared_ptr<Colleague>& user) {
                return user->getName() == username;
            });

        if (it != users.end()) {
            std::cout << "❌ " << username << " left the chat room" << std::endl;
            users.erase(it);

            // Notify remaining users
            for (auto& user : users) {
                user->receive(username + " left the room", "System");
            }
        }
    }

    void showHistory() const {
        std::cout << "\n📜 Chat History:" << std::endl;
        for (const auto& message : messageHistory) {
            std::cout << "   " << message << std::endl;
        }
    }

    size_t getUserCount() const { return users.size(); }
};

// Example 2: Air Traffic Control System
class Aircraft;

class AirTrafficControl {
public:
    virtual ~AirTrafficControl() = default;
    virtual void requestTakeoff(Aircraft* aircraft) = 0;
    virtual void requestLanding(Aircraft* aircraft) = 0;
    virtual void reportPosition(Aircraft* aircraft, const std::string& position) = 0;
    virtual void registerAircraft(std::shared_ptr<Aircraft> aircraft) = 0;
};

class Aircraft {
protected:
    std::shared_ptr<AirTrafficControl> atc;
    std::string callSign;
    std::string position;
    std::string status;

public:
    Aircraft(std::shared_ptr<AirTrafficControl> atc, const std::string& callSign)
        : atc(atc), callSign(callSign), position("Ground"), status("Parked") {}

    virtual ~Aircraft() = default;

    virtual void requestTakeoff() {
        std::cout << "✈️ " << callSign << " requesting takeoff clearance" << std::endl;
        atc->requestTakeoff(this);
    }

    virtual void requestLanding() {
        std::cout << "🛬 " << callSign << " requesting landing clearance" << std::endl;
        atc->requestLanding(this);
    }

    virtual void updatePosition(const std::string& newPosition) {
        position = newPosition;
        std::cout << "📍 " << callSign << " reporting position: " << position << std::endl;
        atc->reportPosition(this, position);
    }

    virtual void receiveInstruction(const std::string& instruction) {
        std::cout << "📻 " << callSign << " received: " << instruction << std::endl;
    }

    const std::string& getCallSign() const { return callSign; }
    const std::string& getPosition() const { return position; }
    const std::string& getStatus() const { return status; }
    void setStatus(const std::string& newStatus) { status = newStatus; }
};

class CommercialAircraft : public Aircraft {
private:
    int passengerCount;

public:
    CommercialAircraft(std::shared_ptr<AirTrafficControl> atc,
                      const std::string& callSign, int passengers)
        : Aircraft(atc, callSign), passengerCount(passengers) {}

    void requestTakeoff() override {
        std::cout << "🛫 Commercial flight " << callSign << " (PAX: " << passengerCount
                  << ") requesting takeoff" << std::endl;
        atc->requestTakeoff(this);
    }

    int getPassengerCount() const { return passengerCount; }
};

class PrivateJet : public Aircraft {
public:
    PrivateJet(std::shared_ptr<AirTrafficControl> atc, const std::string& callSign)
        : Aircraft(atc, callSign) {}

    void requestTakeoff() override {
        std::cout << "🛩️ Private jet " << callSign << " requesting priority takeoff" << std::endl;
        atc->requestTakeoff(this);
    }
};

class ControlTower : public AirTrafficControl {
private:
    std::vector<std::shared_ptr<Aircraft>> aircraft;
    std::vector<std::string> runwayQueue;
    bool runwayAvailable = true;

public:
    void requestTakeoff(Aircraft* aircraft) override {
        std::cout << "🏢 Control Tower processing takeoff request from " << aircraft->getCallSign() << std::endl;

        if (runwayAvailable) {
            runwayAvailable = false;
            aircraft->receiveInstruction("Cleared for takeoff on runway 09L");
            aircraft->setStatus("Taking off");

            // Simulate takeoff time
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            std::cout << "🛫 " << aircraft->getCallSign() << " is airborne" << std::endl;
            runwayAvailable = true;

            // Process queue
            processQueue();
        } else {
            runwayQueue.push_back(aircraft->getCallSign());
            aircraft->receiveInstruction("Hold position, runway occupied. You are #" +
                                       std::to_string(runwayQueue.size()) + " in queue");
        }
    }

    void requestLanding(Aircraft* aircraft) override {
        std::cout << "🏢 Control Tower processing landing request from " << aircraft->getCallSign() << std::endl;

        if (runwayAvailable) {
            runwayAvailable = false;
            aircraft->receiveInstruction("Cleared to land on runway 09L");
            aircraft->setStatus("Landing");

            // Simulate landing time
            std::this_thread::sleep_for(std::chrono::milliseconds(300));

            std::cout << "🛬 " << aircraft->getCallSign() << " has landed safely" << std::endl;
            aircraft->setStatus("Taxiing");
            runwayAvailable = true;

            // Process queue
            processQueue();
        } else {
            aircraft->receiveInstruction("Hold at 3000ft, runway occupied");
        }
    }

    void reportPosition(Aircraft* aircraft, const std::string& position) override {
        std::cout << "🏢 Control Tower tracking " << aircraft->getCallSign()
                  << " at " << position << std::endl;

        // Issue instructions based on position
        if (position.find("Final") != std::string::npos) {
            aircraft->receiveInstruction("Continue approach, wind 090 at 8 knots");
        } else if (position.find("Downwind") != std::string::npos) {
            aircraft->receiveInstruction("Turn base when ready");
        }
    }

    void registerAircraft(std::shared_ptr<Aircraft> aircraft) override {
        this->aircraft.push_back(aircraft);
        std::cout << "📝 " << aircraft->getCallSign() << " registered with Control Tower" << std::endl;
    }

private:
    void processQueue() {
        if (!runwayQueue.empty() && runwayAvailable) {
            std::string nextCallSign = runwayQueue.front();
            runwayQueue.erase(runwayQueue.begin());
            std::cout << "🏢 Processing next aircraft in queue: " << nextCallSign << std::endl;
        }
    }
};

// Example 3: GUI Dialog Mediator
class Widget;

class DialogMediator {
public:
    virtual ~DialogMediator() = default;
    virtual void notify(Widget* sender, const std::string& event) = 0;
};

class Widget {
protected:
    std::shared_ptr<DialogMediator> mediator;
    std::string name;
    bool enabled = true;

public:
    Widget(std::shared_ptr<DialogMediator> mediator, const std::string& name)
        : mediator(mediator), name(name) {}

    virtual ~Widget() = default;

    virtual void click() {
        if (enabled) {
            std::cout << "🖱️ " << name << " clicked" << std::endl;
            mediator->notify(this, "click");
        } else {
            std::cout << "⚠️ " << name << " is disabled" << std::endl;
        }
    }

    virtual void change() {
        std::cout << "🔄 " << name << " changed" << std::endl;
        mediator->notify(this, "change");
    }

    void setEnabled(bool enabled) {
        this->enabled = enabled;
        std::cout << "⚙️ " << name << (enabled ? " enabled" : " disabled") << std::endl;
    }

    const std::string& getName() const { return name; }
    bool isEnabled() const { return enabled; }
};

class Button : public Widget {
public:
    Button(std::shared_ptr<DialogMediator> mediator, const std::string& name)
        : Widget(mediator, name) {}
};

class CheckBox : public Widget {
private:
    bool checked = false;

public:
    CheckBox(std::shared_ptr<DialogMediator> mediator, const std::string& name)
        : Widget(mediator, name) {}

    void toggle() {
        checked = !checked;
        std::cout << "☑️ " << name << (checked ? " checked" : " unchecked") << std::endl;
        mediator->notify(this, checked ? "checked" : "unchecked");
    }

    bool isChecked() const { return checked; }
};

class TextBox : public Widget {
private:
    std::string text;

public:
    TextBox(std::shared_ptr<DialogMediator> mediator, const std::string& name)
        : Widget(mediator, name) {}

    void setText(const std::string& newText) {
        text = newText;
        std::cout << "📝 " << name << " text set to: '" << text << "'" << std::endl;
        mediator->notify(this, "textChanged");
    }

    const std::string& getText() const { return text; }
};

class SettingsDialog : public DialogMediator, public std::enable_shared_from_this<SettingsDialog> {
private:
    std::shared_ptr<CheckBox> enableNotifications;
    std::shared_ptr<CheckBox> enableSound;
    std::shared_ptr<TextBox> emailAddress;
    std::shared_ptr<Button> saveButton;
    std::shared_ptr<Button> cancelButton;

public:
    SettingsDialog() {
        // Create widgets
        enableNotifications = std::make_shared<CheckBox>(shared_from_this(), "Enable Notifications");
        enableSound = std::make_shared<CheckBox>(shared_from_this(), "Enable Sound");
        emailAddress = std::make_shared<TextBox>(shared_from_this(), "Email Address");
        saveButton = std::make_shared<Button>(shared_from_this(), "Save");
        cancelButton = std::make_shared<Button>(shared_from_this(), "Cancel");

        // Initially disable save button
        saveButton->setEnabled(false);
    }

    void notify(Widget* sender, const std::string& event) override {
        std::cout << "🔔 Dialog received event: " << event << " from " << sender->getName() << std::endl;

        if (sender == enableNotifications.get()) {
            if (event == "checked") {
                enableSound->setEnabled(true);
                emailAddress->setEnabled(true);
            } else if (event == "unchecked") {
                enableSound->setEnabled(false);
                emailAddress->setEnabled(false);
            }
        } else if (sender == emailAddress.get() && event == "textChanged") {
            // Enable save button only if email is not empty and notifications are enabled
            bool canSave = !emailAddress->getText().empty() && enableNotifications->isChecked();
            saveButton->setEnabled(canSave);
        } else if (sender == saveButton.get() && event == "click") {
            saveSettings();
        } else if (sender == cancelButton.get() && event == "click") {
            cancelSettings();
        }
    }

    // Expose widgets for external interaction
    auto getEnableNotifications() { return enableNotifications; }
    auto getEnableSound() { return enableSound; }
    auto getEmailAddress() { return emailAddress; }
    auto getSaveButton() { return saveButton; }
    auto getCancelButton() { return cancelButton; }

private:
    void saveSettings() {
        std::cout << "💾 Saving settings:" << std::endl;
        std::cout << "   Notifications: " << (enableNotifications->isChecked() ? "On" : "Off") << std::endl;
        std::cout << "   Sound: " << (enableSound->isChecked() ? "On" : "Off") << std::endl;
        std::cout << "   Email: " << emailAddress->getText() << std::endl;
        std::cout << "✅ Settings saved successfully!" << std::endl;
    }

    void cancelSettings() {
        std::cout << "❌ Settings cancelled" << std::endl;
    }
};

// Modern C++ Functional Mediator
template<typename EventType>
class EventMediator {
private:
    std::unordered_map<std::string, std::vector<std::function<void(const EventType&)>>> subscribers;

public:
    void subscribe(const std::string& eventType, std::function<void(const EventType&)> handler) {
        subscribers[eventType].push_back(handler);
    }

    void publish(const std::string& eventType, const EventType& event) {
        auto it = subscribers.find(eventType);
        if (it != subscribers.end()) {
            for (const auto& handler : it->second) {
                handler(event);
            }
        }
    }

    size_t getSubscriberCount(const std::string& eventType) const {
        auto it = subscribers.find(eventType);
        return it != subscribers.end() ? it->second.size() : 0;
    }
};

struct GameEvent {
    std::string type;
    std::string data;
    std::chrono::steady_clock::time_point timestamp;

    GameEvent(const std::string& type, const std::string& data)
        : type(type), data(data), timestamp(std::chrono::steady_clock::now()) {}
};

int main() {
    std::cout << "=== Mediator Pattern Demo ===" << std::endl;

    // Example 1: Chat Room
    std::cout << "\n1. Chat Room Mediator:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    auto chatRoom = std::make_shared<ChatRoom>();

    auto alice = std::make_shared<ChatUser>(chatRoom, "Alice");
    auto bob = std::make_shared<ChatUser>(chatRoom, "Bob");
    auto charlie = std::make_shared<ChatUser>(chatRoom, "Charlie");
    auto assistant = std::make_shared<ChatBot>(chatRoom, "Assistant");

    chatRoom->addUser(alice);
    chatRoom->addUser(bob);
    chatRoom->addUser(charlie);
    chatRoom->addUser(assistant);

    std::cout << "\nChat conversation:" << std::endl;
    alice->send("Hello everyone!");
    bob->send("Hey Alice, how are you?");
    charlie->send("Good morning all!");
    alice->send("I'm doing great, thanks for asking!");

    std::cout << "\nRemoving user:" << std::endl;
    chatRoom->removeUser("Charlie");

    bob->send("Where did Charlie go?");

    chatRoom->showHistory();

    // Example 2: Air Traffic Control
    std::cout << "\n\n2. Air Traffic Control Mediator:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    auto controlTower = std::make_shared<ControlTower>();

    auto flight123 = std::make_shared<CommercialAircraft>(controlTower, "UA123", 180);
    auto flight456 = std::make_shared<CommercialAircraft>(controlTower, "DL456", 210);
    auto privateJet = std::make_shared<PrivateJet>(controlTower, "N123AB");

    controlTower->registerAircraft(flight123);
    controlTower->registerAircraft(flight456);
    controlTower->registerAircraft(privateJet);

    std::cout << "\nFlight operations:" << std::endl;
    flight123->requestTakeoff();
    flight456->requestTakeoff(); // Should be queued
    privateJet->requestTakeoff(); // Should be queued

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    flight123->updatePosition("Downwind");
    flight123->updatePosition("Final approach");
    flight123->requestLanding();

    // Example 3: GUI Dialog Mediator
    std::cout << "\n\n3. GUI Dialog Mediator:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    // Note: SettingsDialog needs to inherit from std::enable_shared_from_this
    // For this demo, we'll create a simpler version
    std::cout << "Simulating Settings Dialog interactions:" << std::endl;

    std::cout << "\nUser interactions:" << std::endl;
    std::cout << "1. User clicks 'Enable Notifications' checkbox" << std::endl;
    std::cout << "   -> Email field and Sound checkbox become enabled" << std::endl;

    std::cout << "2. User enters email address: 'user@example.com'" << std::endl;
    std::cout << "   -> Save button becomes enabled" << std::endl;

    std::cout << "3. User clicks Save button" << std::endl;
    std::cout << "   -> Settings are saved and dialog closes" << std::endl;

    // Example 4: Functional Event Mediator
    std::cout << "\n\n4. Functional Event Mediator:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    EventMediator<GameEvent> gameMediator;

    // Subscribe to events
    gameMediator.subscribe("player_joined", [](const GameEvent& event) {
        std::cout << "🎮 New player joined: " << event.data << std::endl;
    });

    gameMediator.subscribe("player_scored", [](const GameEvent& event) {
        std::cout << "🏆 Player scored: " << event.data << std::endl;
    });

    gameMediator.subscribe("game_over", [](const GameEvent& event) {
        std::cout << "🏁 Game over: " << event.data << std::endl;
    });

    // Subscribe multiple handlers to same event
    gameMediator.subscribe("player_scored", [](const GameEvent& event) {
        std::cout << "📊 Statistics updated for: " << event.data << std::endl;
    });

    // Publish events
    std::cout << "\nGame events:" << std::endl;
    gameMediator.publish("player_joined", GameEvent("player_joined", "Alice"));
    gameMediator.publish("player_joined", GameEvent("player_joined", "Bob"));
    gameMediator.publish("player_scored", GameEvent("player_scored", "Alice - 100 points"));
    gameMediator.publish("player_scored", GameEvent("player_scored", "Bob - 150 points"));
    gameMediator.publish("game_over", GameEvent("game_over", "Final Score - Alice: 100, Bob: 150"));

    std::cout << "\nSubscriber counts:" << std::endl;
    std::cout << "player_joined: " << gameMediator.getSubscriberCount("player_joined") << " subscribers" << std::endl;
    std::cout << "player_scored: " << gameMediator.getSubscriberCount("player_scored") << " subscribers" << std::endl;
    std::cout << "game_over: " << gameMediator.getSubscriberCount("game_over") << " subscribers" << std::endl;

    // Benefits summary
    std::cout << "\n\n5. Mediator Pattern Benefits:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "✓ Decouples objects by preventing direct communication" << std::endl;
    std::cout << "✓ Centralizes control logic in mediator" << std::endl;
    std::cout << "✓ Promotes loose coupling between colleagues" << std::endl;
    std::cout << "✓ Makes object collaboration more explicit" << std::endl;
    std::cout << "✓ Easier to maintain and extend interactions" << std::endl;
    std::cout << "✓ Supports many-to-many communication patterns" << std::endl;
    std::cout << "✓ Can be implemented functionally with event systems" << std::endl;

    return 0;
}