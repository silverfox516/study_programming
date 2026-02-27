#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <chrono>

// Subsystem 1: Audio System
class AudioSystem {
public:
    void powerOn() {
        std::cout << "[Audio] Powering on audio system..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void powerOff() {
        std::cout << "[Audio] Powering off audio system..." << std::endl;
    }

    void setVolume(int volume) {
        std::cout << "[Audio] Setting volume to " << volume << std::endl;
    }

    void setSurroundSound(bool enabled) {
        std::cout << "[Audio] Surround sound " << (enabled ? "enabled" : "disabled") << std::endl;
    }

    void playAudio(const std::string& source) {
        std::cout << "[Audio] Playing audio from " << source << std::endl;
    }
};

// Subsystem 2: Video System
class VideoSystem {
public:
    void powerOn() {
        std::cout << "[Video] Powering on video system..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
    }

    void powerOff() {
        std::cout << "[Video] Powering off video system..." << std::endl;
    }

    void setResolution(const std::string& resolution) {
        std::cout << "[Video] Setting resolution to " << resolution << std::endl;
    }

    void setHDR(bool enabled) {
        std::cout << "[Video] HDR " << (enabled ? "enabled" : "disabled") << std::endl;
    }

    void playVideo(const std::string& source) {
        std::cout << "[Video] Playing video from " << source << std::endl;
    }
};

// Subsystem 3: Lighting System
class LightingSystem {
public:
    void dimLights(int percentage) {
        std::cout << "[Lighting] Dimming lights to " << percentage << "%" << std::endl;
    }

    void setAmbientLighting(const std::string& mood) {
        std::cout << "[Lighting] Setting ambient lighting to " << mood << " mode" << std::endl;
    }

    void turnOff() {
        std::cout << "[Lighting] Turning off all lights" << std::endl;
    }
};

// Subsystem 4: Climate Control
class ClimateControl {
public:
    void setTemperature(int temperature) {
        std::cout << "[Climate] Setting temperature to " << temperature << "°F" << std::endl;
    }

    void setFanSpeed(int speed) {
        std::cout << "[Climate] Setting fan speed to " << speed << std::endl;
    }

    void turnOff() {
        std::cout << "[Climate] Turning off climate control" << std::endl;
    }
};

// Subsystem 5: Security System
class SecuritySystem {
public:
    void disarm() {
        std::cout << "[Security] Disarming security system" << std::endl;
    }

    void arm() {
        std::cout << "[Security] Arming security system" << std::endl;
    }

    void lockDoors() {
        std::cout << "[Security] Locking all doors" << std::endl;
    }

    void unlockDoors() {
        std::cout << "[Security] Unlocking doors" << std::endl;
    }
};

// Facade: Home Theater System
class HomeTheaterFacade {
private:
    std::unique_ptr<AudioSystem> audioSystem;
    std::unique_ptr<VideoSystem> videoSystem;
    std::unique_ptr<LightingSystem> lightingSystem;
    std::unique_ptr<ClimateControl> climateControl;
    std::unique_ptr<SecuritySystem> securitySystem;

public:
    HomeTheaterFacade()
        : audioSystem(std::make_unique<AudioSystem>()),
          videoSystem(std::make_unique<VideoSystem>()),
          lightingSystem(std::make_unique<LightingSystem>()),
          climateControl(std::make_unique<ClimateControl>()),
          securitySystem(std::make_unique<SecuritySystem>()) {}

    void watchMovie(const std::string& movie) {
        std::cout << "\n=== Starting Movie: " << movie << " ===" << std::endl;
        securitySystem->disarm();
        securitySystem->lockDoors();

        lightingSystem->dimLights(20);
        lightingSystem->setAmbientLighting("movie");

        climateControl->setTemperature(72);
        climateControl->setFanSpeed(2);

        audioSystem->powerOn();
        audioSystem->setVolume(8);
        audioSystem->setSurroundSound(true);

        videoSystem->powerOn();
        videoSystem->setResolution("4K");
        videoSystem->setHDR(true);

        std::cout << "\n🎬 Now playing: " << movie << std::endl;
        audioSystem->playAudio("Blu-ray");
        videoSystem->playVideo("Blu-ray");

        std::cout << "🍿 Enjoy your movie!" << std::endl;
    }

    void endMovie() {
        std::cout << "\n=== Ending Movie Session ===" << std::endl;
        audioSystem->powerOff();
        videoSystem->powerOff();
        lightingSystem->dimLights(100);
        climateControl->turnOff();
        securitySystem->unlockDoors();
        securitySystem->arm();
        std::cout << "Movie session ended. All systems reset." << std::endl;
    }

    void listenToMusic(const std::string& playlist) {
        std::cout << "\n=== Starting Music: " << playlist << " ===" << std::endl;
        audioSystem->powerOn();
        audioSystem->setVolume(6);
        audioSystem->setSurroundSound(false);

        lightingSystem->setAmbientLighting("relaxing");
        lightingSystem->dimLights(60);

        climateControl->setTemperature(70);

        std::cout << "🎵 Now playing: " << playlist << std::endl;
        audioSystem->playAudio("Streaming");
    }

    void partyMode() {
        std::cout << "\n=== Activating Party Mode ===" << std::endl;
        securitySystem->disarm();
        securitySystem->unlockDoors();

        audioSystem->powerOn();
        audioSystem->setVolume(10);
        audioSystem->setSurroundSound(true);

        lightingSystem->setAmbientLighting("party");
        lightingSystem->dimLights(80);

        climateControl->setTemperature(68);
        climateControl->setFanSpeed(3);

        std::cout << "🎉 Party mode activated! Let's dance!" << std::endl;
        audioSystem->playAudio("Streaming");
    }

    void goodNight() {
        std::cout << "\n=== Good Night Mode ===" << std::endl;
        audioSystem->powerOff();
        videoSystem->powerOff();
        lightingSystem->turnOff();
        climateControl->setTemperature(65);
        climateControl->setFanSpeed(1);
        securitySystem->lockDoors();
        securitySystem->arm();
        std::cout << "😴 Good night! All systems secured." << std::endl;
    }
};

// Example 2: Computer System Facade
class CPU {
public:
    void boot() { std::cout << "[CPU] Booting processor..." << std::endl; }
    void shutdown() { std::cout << "[CPU] Shutting down processor..." << std::endl; }
    void execute(const std::string& instruction) {
        std::cout << "[CPU] Executing: " << instruction << std::endl;
    }
};

class Memory {
public:
    void load(const std::string& program) {
        std::cout << "[Memory] Loading " << program << " into memory" << std::endl;
    }
    void clear() { std::cout << "[Memory] Clearing memory" << std::endl; }
};

class HardDrive {
public:
    void spinUp() { std::cout << "[HDD] Spinning up hard drive..." << std::endl; }
    void spinDown() { std::cout << "[HDD] Spinning down hard drive..." << std::endl; }
    void readData(const std::string& file) {
        std::cout << "[HDD] Reading " << file << " from disk" << std::endl;
    }
};

class GraphicsCard {
public:
    void initialize() { std::cout << "[GPU] Initializing graphics card..." << std::endl; }
    void shutdown() { std::cout << "[GPU] Shutting down graphics card..." << std::endl; }
    void render(const std::string& scene) {
        std::cout << "[GPU] Rendering " << scene << std::endl;
    }
};

class ComputerFacade {
private:
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<Memory> memory;
    std::unique_ptr<HardDrive> hardDrive;
    std::unique_ptr<GraphicsCard> gpu;

public:
    ComputerFacade()
        : cpu(std::make_unique<CPU>()),
          memory(std::make_unique<Memory>()),
          hardDrive(std::make_unique<HardDrive>()),
          gpu(std::make_unique<GraphicsCard>()) {}

    void startComputer() {
        std::cout << "\n=== Starting Computer ===" << std::endl;
        cpu->boot();
        memory->load("Operating System");
        hardDrive->spinUp();
        gpu->initialize();
        cpu->execute("system_startup");
        std::cout << "💻 Computer ready!" << std::endl;
    }

    void shutdownComputer() {
        std::cout << "\n=== Shutting Down Computer ===" << std::endl;
        cpu->execute("save_state");
        memory->clear();
        gpu->shutdown();
        hardDrive->spinDown();
        cpu->shutdown();
        std::cout << "💤 Computer shut down safely." << std::endl;
    }

    void runGame(const std::string& gameName) {
        std::cout << "\n=== Running Game: " << gameName << " ===" << std::endl;
        hardDrive->readData(gameName);
        memory->load(gameName);
        cpu->execute("launch_game");
        gpu->render("game_scene");
        std::cout << "🎮 " << gameName << " is now running!" << std::endl;
    }
};

// Example 3: Banking System Facade
class AccountManager {
public:
    bool verifyAccount(const std::string& accountId) {
        std::cout << "[Account] Verifying account: " << accountId << std::endl;
        return true; // Simplified
    }

    double getBalance(const std::string& accountId) {
        std::cout << "[Account] Retrieving balance for: " << accountId << std::endl;
        return 1000.0; // Simplified
    }

    bool updateBalance(const std::string& accountId, double amount) {
        std::cout << "[Account] Updating balance for " << accountId << " by $" << amount << std::endl;
        return true; // Simplified
    }
};

class SecurityManager {
public:
    bool authenticate(const std::string& userId, const std::string& password) {
        std::cout << "[Security] Authenticating user: " << userId << std::endl;
        return true; // Simplified
    }

    bool authorizeTransaction(const std::string& userId, double amount) {
        std::cout << "[Security] Authorizing transaction of $" << amount << " for " << userId << std::endl;
        return amount <= 5000; // Simplified limit
    }
};

class TransactionLogger {
public:
    void logTransaction(const std::string& type, const std::string& accountId, double amount) {
        std::cout << "[Logger] Recording " << type << " of $" << amount
                  << " for account " << accountId << std::endl;
    }
};

class NotificationService {
public:
    void sendNotification(const std::string& userId, const std::string& message) {
        std::cout << "[Notification] Sending to " << userId << ": " << message << std::endl;
    }
};

class BankingFacade {
private:
    std::unique_ptr<AccountManager> accountManager;
    std::unique_ptr<SecurityManager> securityManager;
    std::unique_ptr<TransactionLogger> transactionLogger;
    std::unique_ptr<NotificationService> notificationService;

public:
    BankingFacade()
        : accountManager(std::make_unique<AccountManager>()),
          securityManager(std::make_unique<SecurityManager>()),
          transactionLogger(std::make_unique<TransactionLogger>()),
          notificationService(std::make_unique<NotificationService>()) {}

    bool transferMoney(const std::string& fromAccount, const std::string& toAccount,
                      double amount, const std::string& userId, const std::string& password) {
        std::cout << "\n=== Processing Money Transfer ===" << std::endl;

        // Authenticate user
        if (!securityManager->authenticate(userId, password)) {
            std::cout << "❌ Authentication failed!" << std::endl;
            return false;
        }

        // Verify accounts
        if (!accountManager->verifyAccount(fromAccount) || !accountManager->verifyAccount(toAccount)) {
            std::cout << "❌ Invalid account!" << std::endl;
            return false;
        }

        // Check authorization
        if (!securityManager->authorizeTransaction(userId, amount)) {
            std::cout << "❌ Transaction not authorized!" << std::endl;
            return false;
        }

        // Check balance
        if (accountManager->getBalance(fromAccount) < amount) {
            std::cout << "❌ Insufficient funds!" << std::endl;
            return false;
        }

        // Perform transfer
        accountManager->updateBalance(fromAccount, -amount);
        accountManager->updateBalance(toAccount, amount);

        // Log transaction
        transactionLogger->logTransaction("TRANSFER", fromAccount, amount);

        // Send notifications
        notificationService->sendNotification(userId, "Transfer of $" + std::to_string(amount) + " completed successfully");

        std::cout << "✅ Transfer completed successfully!" << std::endl;
        return true;
    }

    double checkBalance(const std::string& accountId, const std::string& userId, const std::string& password) {
        std::cout << "\n=== Checking Account Balance ===" << std::endl;

        if (!securityManager->authenticate(userId, password)) {
            std::cout << "❌ Authentication failed!" << std::endl;
            return -1;
        }

        if (!accountManager->verifyAccount(accountId)) {
            std::cout << "❌ Invalid account!" << std::endl;
            return -1;
        }

        double balance = accountManager->getBalance(accountId);
        transactionLogger->logTransaction("BALANCE_INQUIRY", accountId, 0);

        std::cout << "💰 Current balance: $" << balance << std::endl;
        return balance;
    }
};

int main() {
    std::cout << "=== Facade Pattern Demo ===" << std::endl;

    // Example 1: Home Theater System
    std::cout << "\n1. Home Theater System Facade:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    HomeTheaterFacade homeTheater;

    homeTheater.watchMovie("The Matrix");
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    homeTheater.endMovie();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    homeTheater.listenToMusic("Chill Vibes Playlist");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    homeTheater.partyMode();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    homeTheater.goodNight();

    // Example 2: Computer System
    std::cout << "\n\n2. Computer System Facade:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    ComputerFacade computer;

    computer.startComputer();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    computer.runGame("Cyberpunk 2077");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    computer.shutdownComputer();

    // Example 3: Banking System
    std::cout << "\n\n3. Banking System Facade:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    BankingFacade bankingSystem;

    // Check balance
    bankingSystem.checkBalance("12345", "john_doe", "password123");

    // Transfer money
    bankingSystem.transferMoney("12345", "67890", 250.00, "john_doe", "password123");

    // Check balance again
    bankingSystem.checkBalance("12345", "john_doe", "password123");

    // Try unauthorized transfer
    std::cout << "\nTrying large transfer:" << std::endl;
    bankingSystem.transferMoney("12345", "67890", 10000.00, "john_doe", "password123");

    // Demonstrate the complexity hidden by facade
    std::cout << "\n\n4. Complexity Hidden by Facade:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "Without facade, a simple 'watch movie' operation would require:" << std::endl;
    std::cout << "- 15+ individual method calls across 5 different subsystems" << std::endl;
    std::cout << "- Knowledge of the correct sequence of operations" << std::endl;
    std::cout << "- Understanding of each subsystem's API" << std::endl;
    std::cout << "- Error handling for each subsystem" << std::endl;
    std::cout << "\nWith facade: homeTheater.watchMovie(\"Movie Name\") - Simple!" << std::endl;

    return 0;
}