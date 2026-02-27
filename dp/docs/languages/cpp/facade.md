# Facade Pattern - C++ Implementation

## 개요

Facade 패턴은 복잡한 서브시스템에 대한 단순한 인터페이스를 제공하는 구조적 디자인 패턴입니다. 이 패턴은 여러 개의 복잡한 서브시스템을 하나의 고수준 인터페이스로 감싸서 클라이언트가 쉽게 사용할 수 있도록 합니다. Facade는 "정면", "외관"이라는 뜻으로, 건물의 복잡한 내부 구조를 숨기고 아름다운 외관만 보여주는 것과 같은 개념입니다.

## 구조

Facade 패턴의 주요 구성 요소:
- **Facade**: 서브시스템의 복잡성을 숨기는 단순한 인터페이스
- **Subsystem Classes**: 실제 작업을 수행하는 복잡한 클래스들
- **Client**: Facade를 통해 서브시스템을 사용하는 클라이언트

## C++ 구현

### 1. 홈시어터 시스템

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

// Subsystem 1: Audio System
class AudioSystem {
public:
    void powerOn() {
        std::cout << "[Audio] Powering on audio system..." << std::endl;
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
        std::cout << "\\n=== Starting Movie: " << movie << " ===" << std::endl;
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

        std::cout << "\\n🎬 Now playing: " << movie << std::endl;
        audioSystem->playAudio("Blu-ray");
        videoSystem->playVideo("Blu-ray");
        std::cout << "🍿 Enjoy your movie!" << std::endl;
    }

    void endMovie() {
        std::cout << "\\n=== Ending Movie Session ===" << std::endl;
        audioSystem->powerOff();
        videoSystem->powerOff();
        lightingSystem->dimLights(100);
        climateControl->turnOff();
        securitySystem->unlockDoors();
        securitySystem->arm();
        std::cout << "Movie session ended. All systems reset." << std::endl;
    }

    void listenToMusic(const std::string& playlist) {
        std::cout << "\\n=== Starting Music: " << playlist << " ===" << std::endl;
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
        std::cout << "\\n=== Activating Party Mode ===" << std::endl;
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
        std::cout << "\\n=== Good Night Mode ===" << std::endl;
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
```

### 2. 컴퓨터 시스템

```cpp
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
        std::cout << "\\n=== Starting Computer ===" << std::endl;
        cpu->boot();
        memory->load("Operating System");
        hardDrive->spinUp();
        gpu->initialize();
        cpu->execute("system_startup");
        std::cout << "💻 Computer ready!" << std::endl;
    }

    void shutdownComputer() {
        std::cout << "\\n=== Shutting Down Computer ===" << std::endl;
        cpu->execute("save_state");
        memory->clear();
        gpu->shutdown();
        hardDrive->spinDown();
        cpu->shutdown();
        std::cout << "💤 Computer shut down safely." << std::endl;
    }

    void runGame(const std::string& gameName) {
        std::cout << "\\n=== Running Game: " << gameName << " ===" << std::endl;
        hardDrive->readData(gameName);
        memory->load(gameName);
        cpu->execute("launch_game");
        gpu->render("game_scene");
        std::cout << "🎮 " << gameName << " is now running!" << std::endl;
    }
};
```

### 3. 은행 시스템

```cpp
class AccountManager {
public:
    bool verifyAccount(const std::string& accountId) {
        std::cout << "[Account] Verifying account: " << accountId << std::endl;
        return true;
    }

    double getBalance(const std::string& accountId) {
        std::cout << "[Account] Retrieving balance for: " << accountId << std::endl;
        return 1000.0;
    }

    bool updateBalance(const std::string& accountId, double amount) {
        std::cout << "[Account] Updating balance for " << accountId << " by $" << amount << std::endl;
        return true;
    }
};

class SecurityManager {
public:
    bool authenticate(const std::string& userId, const std::string& password) {
        std::cout << "[Security] Authenticating user: " << userId << std::endl;
        return true;
    }

    bool authorizeTransaction(const std::string& userId, double amount) {
        std::cout << "[Security] Authorizing transaction of $" << amount << " for " << userId << std::endl;
        return amount <= 5000;
    }
};

class TransactionLogger {
public:
    void logTransaction(const std::string& type, const std::string& accountId, double amount) {
        std::cout << "[Logger] Recording " << type << " of $" << amount
                  << " for account " << accountId << std::endl;
    }
};

class BankingFacade {
private:
    std::unique_ptr<AccountManager> accountManager;
    std::unique_ptr<SecurityManager> securityManager;
    std::unique_ptr<TransactionLogger> transactionLogger;

public:
    BankingFacade()
        : accountManager(std::make_unique<AccountManager>()),
          securityManager(std::make_unique<SecurityManager>()),
          transactionLogger(std::make_unique<TransactionLogger>()) {}

    bool transferMoney(const std::string& fromAccount, const std::string& toAccount,
                      double amount, const std::string& userId, const std::string& password) {
        std::cout << "\\n=== Processing Money Transfer ===" << std::endl;

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

        std::cout << "✅ Transfer completed successfully!" << std::endl;
        return true;
    }

    double checkBalance(const std::string& accountId, const std::string& userId, const std::string& password) {
        std::cout << "\\n=== Checking Account Balance ===" << std::endl;

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
```

## 사용 예제

```cpp
int main() {
    // 1. 홈시어터 시스템 사용
    HomeTheaterFacade homeTheater;

    homeTheater.watchMovie("The Matrix");
    // 복잡한 15+ 단계의 설정이 한 번의 호출로 완료

    homeTheater.endMovie();
    homeTheater.listenToMusic("Chill Vibes Playlist");
    homeTheater.partyMode();
    homeTheater.goodNight();

    // 2. 컴퓨터 시스템 사용
    ComputerFacade computer;

    computer.startComputer();
    computer.runGame("Cyberpunk 2077");
    computer.shutdownComputer();

    // 3. 은행 시스템 사용
    BankingFacade bankingSystem;

    bankingSystem.checkBalance("12345", "john_doe", "password123");
    bankingSystem.transferMoney("12345", "67890", 250.00, "john_doe", "password123");

    return 0;
}
```

## C++의 장점

1. **스마트 포인터**: `unique_ptr`로 서브시스템의 자동 메모리 관리
2. **RAII**: 리소스 자동 해제로 안전한 시스템 관리
3. **캡슐화**: private 멤버로 서브시스템 세부사항 은닉
4. **타입 안전성**: 컴파일 타임에 인터페이스 검증
5. **성능**: 직접 메모리 관리로 높은 성능

## 적용 상황

1. **복잡한 라이브러리**: 써드파티 라이브러리의 복잡한 API 단순화
2. **레거시 시스템**: 기존 시스템에 대한 현대적인 인터페이스
3. **마이크로서비스**: 여러 서비스에 대한 통합 API
4. **시스템 통합**: 다양한 하위 시스템의 조율
5. **API 게이트웨이**: 여러 백엔드 서비스를 하나의 인터페이스로

### Facade vs Adapter 비교

- **Facade**: 여러 클래스를 감싸서 단순한 인터페이스 제공
- **Adapter**: 호환되지 않는 인터페이스를 변환

Facade 패턴은 복잡성을 숨기고 사용 편의성을 제공하여, 클라이언트 코드를 더 간단하고 유지보수하기 쉽게 만드는 강력한 패턴입니다.