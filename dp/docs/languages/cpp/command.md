# Command Pattern - C++ Implementation

## 개요

Command 패턴은 요청을 객체로 캡슐화하여 다양한 요청으로 클라이언트를 매개변수화하고, 요청을 대기열에 넣거나 로깅하며, 실행 취소 가능한 연산을 지원하는 행동적 디자인 패턴입니다.

C++에서는 스마트 포인터, STL 컨테이너, 람다 함수, `std::function`을 활용하여 강력하고 유연한 Command 시스템을 구현할 수 있습니다.

## 구조

### 핵심 구성 요소

1. **Command** (Command 인터페이스)
   - 작업 수행을 위한 인터페이스
   - execute(), undo(), getDescription() 메서드

2. **Concrete Command** (InsertTextCommand, DeleteTextCommand, LightOnCommand 등)
   - Command 인터페이스의 구체적 구현
   - Receiver 객체와 필요한 매개변수 저장

3. **Receiver** (TextEditor, Light, Fan)
   - 요청을 처리하는 방법을 알고 있는 객체
   - 실제 작업을 수행

4. **Invoker** (CommandManager, RemoteControl)
   - Command 객체를 저장하고 execute() 호출
   - 명령 기록 관리 (undo/redo)

5. **Client**
   - Command 객체를 생성하고 Receiver 설정

## C++ 구현

### 1. Command 인터페이스

```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getDescription() const = 0;
    virtual bool isUndoable() const { return true; }
};
```

### 2. Receiver 클래스 - 텍스트 에디터

```cpp
class TextEditor {
private:
    std::string content;
    size_t cursorPosition;

public:
    TextEditor() : cursorPosition(0) {}

    void insertText(const std::string& text, size_t position) {
        if (position <= content.length()) {
            content.insert(position, text);
            cursorPosition = position + text.length();
            std::cout << "Inserted '" << text << "' at position " << position << std::endl;
        }
    }

    void deleteText(size_t position, size_t length) {
        if (position < content.length()) {
            size_t actualLength = std::min(length, content.length() - position);
            content.erase(position, actualLength);
            cursorPosition = position;
            std::cout << "Deleted " << actualLength << " characters at position " << position << std::endl;
        }
    }

    void replaceText(size_t position, size_t length, const std::string& newText) {
        if (position < content.length()) {
            size_t actualLength = std::min(length, content.length() - position);
            content.erase(position, actualLength);
            content.insert(position, newText);
            cursorPosition = position + newText.length();
            std::cout << "Replaced " << actualLength << " characters with '" << newText << "'" << std::endl;
        }
    }

    const std::string& getContent() const { return content; }
    size_t getCursorPosition() const { return cursorPosition; }

    void display() const {
        std::cout << "Content: \"" << content << "\"" << std::endl;
        std::cout << "Cursor at position: " << cursorPosition << std::endl;
    }
};
```

### 3. Concrete Command 클래스들

#### 텍스트 삽입 명령
```cpp
class InsertTextCommand : public Command {
private:
    TextEditor& editor;
    std::string text;
    size_t position;

public:
    InsertTextCommand(TextEditor& editor, const std::string& text, size_t position)
        : editor(editor), text(text), position(position) {}

    void execute() override {
        editor.insertText(text, position);
    }

    void undo() override {
        editor.deleteText(position, text.length());
    }

    std::string getDescription() const override {
        return "Insert '" + text + "' at position " + std::to_string(position);
    }
};
```

#### 텍스트 삭제 명령
```cpp
class DeleteTextCommand : public Command {
private:
    TextEditor& editor;
    size_t position;
    size_t length;
    std::string deletedText; // undo를 위해 저장

public:
    DeleteTextCommand(TextEditor& editor, size_t position, size_t length)
        : editor(editor), position(position), length(length) {}

    void execute() override {
        // undo를 위해 삭제될 텍스트 저장
        const std::string& content = editor.getContent();
        if (position < content.length()) {
            size_t actualLength = std::min(length, content.length() - position);
            deletedText = content.substr(position, actualLength);
        }
        editor.deleteText(position, length);
    }

    void undo() override {
        if (!deletedText.empty()) {
            editor.insertText(deletedText, position);
        }
    }

    std::string getDescription() const override {
        return "Delete " + std::to_string(length) + " characters at position " + std::to_string(position);
    }
};
```

### 4. 매크로 명령 (Composite Command)

```cpp
class MacroCommand : public Command {
private:
    std::vector<std::unique_ptr<Command>> commands;
    std::string description;

public:
    explicit MacroCommand(const std::string& description) : description(description) {}

    void addCommand(std::unique_ptr<Command> command) {
        commands.push_back(std::move(command));
    }

    void execute() override {
        std::cout << "Executing macro: " << description << std::endl;
        for (auto& command : commands) {
            command->execute();
        }
    }

    void undo() override {
        std::cout << "Undoing macro: " << description << std::endl;
        // 역순으로 undo 실행
        for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
            (*it)->undo();
        }
    }

    std::string getDescription() const override {
        return "Macro: " + description + " (" + std::to_string(commands.size()) + " commands)";
    }
};
```

### 5. Invoker - Command Manager

```cpp
class CommandManager {
private:
    std::stack<std::unique_ptr<Command>> undoStack;
    std::stack<std::unique_ptr<Command>> redoStack;

public:
    void executeCommand(std::unique_ptr<Command> command) {
        std::cout << "Executing: " << command->getDescription() << std::endl;
        command->execute();

        if (command->isUndoable()) {
            undoStack.push(std::move(command));
            // 새 명령 실행 시 redo 스택 클리어
            while (!redoStack.empty()) {
                redoStack.pop();
            }
        }
    }

    bool undo() {
        if (undoStack.empty()) {
            std::cout << "Nothing to undo" << std::endl;
            return false;
        }

        auto command = std::move(undoStack.top());
        undoStack.pop();

        std::cout << "Undoing: " << command->getDescription() << std::endl;
        command->undo();

        redoStack.push(std::move(command));
        return true;
    }

    bool redo() {
        if (redoStack.empty()) {
            std::cout << "Nothing to redo" << std::endl;
            return false;
        }

        auto command = std::move(redoStack.top());
        redoStack.pop();

        std::cout << "Redoing: " << command->getDescription() << std::endl;
        command->execute();

        undoStack.push(std::move(command));
        return true;
    }

    size_t getUndoStackSize() const { return undoStack.size(); }
    size_t getRedoStackSize() const { return redoStack.size(); }

    void clearHistory() {
        while (!undoStack.empty()) undoStack.pop();
        while (!redoStack.empty()) redoStack.pop();
        std::cout << "Command history cleared" << std::endl;
    }
};
```

### 6. 리모컨 예제 - 다양한 기기 제어

#### 기기 클래스들
```cpp
class Light {
private:
    std::string location;
    bool isOn;
    int brightness; // 0-100

public:
    explicit Light(const std::string& location) : location(location), isOn(false), brightness(0) {}

    void turnOn() {
        isOn = true;
        brightness = 100;
        std::cout << location << " light is ON (brightness: " << brightness << "%)" << std::endl;
    }

    void turnOff() {
        isOn = false;
        brightness = 0;
        std::cout << location << " light is OFF" << std::endl;
    }

    void setBrightness(int level) {
        brightness = std::max(0, std::min(100, level));
        if (brightness > 0) isOn = true;
        std::cout << location << " light brightness set to " << brightness << "%" << std::endl;
    }

    const std::string& getLocation() const { return location; }
};

class Fan {
private:
    std::string location;
    int speed; // 0-3 (0=off, 1=low, 2=medium, 3=high)

public:
    explicit Fan(const std::string& location) : location(location), speed(0) {}

    void setSpeed(int newSpeed) {
        speed = std::max(0, std::min(3, newSpeed));
        std::cout << location << " fan speed set to " << speed;
        if (speed == 0) std::cout << " (OFF)";
        else if (speed == 1) std::cout << " (LOW)";
        else if (speed == 2) std::cout << " (MEDIUM)";
        else std::cout << " (HIGH)";
        std::cout << std::endl;
    }

    int getSpeed() const { return speed; }
    const std::string& getLocation() const { return location; }
};
```

#### 기기 제어 명령들
```cpp
class LightOnCommand : public Command {
private:
    Light& light;

public:
    explicit LightOnCommand(Light& light) : light(light) {}

    void execute() override { light.turnOn(); }
    void undo() override { light.turnOff(); }
    std::string getDescription() const override {
        return "Turn on " + light.getLocation() + " light";
    }
};

class FanSpeedCommand : public Command {
private:
    Fan& fan;
    int newSpeed;
    int previousSpeed;

public:
    FanSpeedCommand(Fan& fan, int speed) : fan(fan), newSpeed(speed), previousSpeed(0) {}

    void execute() override {
        previousSpeed = fan.getSpeed();
        fan.setSpeed(newSpeed);
    }

    void undo() override {
        fan.setSpeed(previousSpeed);
    }

    std::string getDescription() const override {
        return "Set " + fan.getLocation() + " fan speed to " + std::to_string(newSpeed);
    }
};
```

### 7. 함수형 Command (Modern C++)

```cpp
using FunctionalCommand = std::function<void()>;

class FunctionalCommandManager {
private:
    struct CommandPair {
        FunctionalCommand execute;
        FunctionalCommand undo;
        std::string description;
    };

    std::stack<CommandPair> undoStack;
    std::stack<CommandPair> redoStack;

public:
    void executeCommand(FunctionalCommand executeFunc, FunctionalCommand undoFunc, const std::string& description) {
        std::cout << "Executing: " << description << std::endl;
        executeFunc();

        undoStack.push({executeFunc, undoFunc, description});

        // redo 스택 클리어
        while (!redoStack.empty()) {
            redoStack.pop();
        }
    }

    bool undo() {
        if (undoStack.empty()) {
            std::cout << "Nothing to undo" << std::endl;
            return false;
        }

        auto command = undoStack.top();
        undoStack.pop();

        std::cout << "Undoing: " << command.description << std::endl;
        command.undo();

        redoStack.push(command);
        return true;
    }

    bool redo() {
        if (redoStack.empty()) {
            std::cout << "Nothing to redo" << std::endl;
            return false;
        }

        auto command = redoStack.top();
        redoStack.pop();

        std::cout << "Redoing: " << command.description << std::endl;
        command.execute();

        undoStack.push(command);
        return true;
    }
};
```

### 8. 명령 대기열 (Command Queue)

```cpp
class CommandQueue {
private:
    std::queue<std::unique_ptr<Command>> commandQueue;
    bool processing;

public:
    CommandQueue() : processing(false) {}

    void addCommand(std::unique_ptr<Command> command) {
        commandQueue.push(std::move(command));
        std::cout << "Command added to queue. Queue size: " << commandQueue.size() << std::endl;
    }

    void processCommands() {
        if (processing) {
            std::cout << "Already processing commands..." << std::endl;
            return;
        }

        processing = true;
        std::cout << "Processing " << commandQueue.size() << " commands..." << std::endl;

        while (!commandQueue.empty()) {
            auto command = std::move(commandQueue.front());
            commandQueue.pop();

            std::cout << "Processing: " << command->getDescription() << std::endl;
            command->execute();

            // 처리 시간 시뮬레이션
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        processing = false;
        std::cout << "All commands processed." << std::endl;
    }

    size_t getQueueSize() const { return commandQueue.size(); }
    bool isProcessing() const { return processing; }
};
```

## 사용 예제

### 1. 텍스트 에디터 명령
```cpp
TextEditor editor;
CommandManager commandManager;

// 명령 실행
commandManager.executeCommand(std::make_unique<InsertTextCommand>(editor, "Hello", 0));
commandManager.executeCommand(std::make_unique<InsertTextCommand>(editor, " World", 5));
commandManager.executeCommand(std::make_unique<InsertTextCommand>(editor, "!", 11));

editor.display(); // "Hello World!"

// Undo/Redo 테스트
commandManager.undo();  // "Hello World"
commandManager.undo();  // "Hello"
commandManager.redo();  // "Hello World"
```

### 2. 매크로 명령
```cpp
auto macro = std::make_unique<MacroCommand>("Format Text");
macro->addCommand(std::make_unique<DeleteTextCommand>(editor, 0, 11));
macro->addCommand(std::make_unique<InsertTextCommand>(editor, "Welcome to", 0));
macro->addCommand(std::make_unique<InsertTextCommand>(editor, " Command Pattern!", 10));

commandManager.executeCommand(std::move(macro));
editor.display(); // "Welcome to Command Pattern!"

commandManager.undo(); // 전체 매크로 실행 취소
```

### 3. 리모컨 사용
```cpp
Light livingRoomLight("Living Room");
Fan ceilingFan("Ceiling");

RemoteControl remote;

remote.setCommand(0,
    std::make_unique<LightOnCommand>(livingRoomLight),
    std::make_unique<LightOffCommand>(livingRoomLight));

remote.setCommand(1,
    std::make_unique<FanSpeedCommand>(ceilingFan, 3),
    std::make_unique<FanSpeedCommand>(ceilingFan, 0));

// 리모컨 사용
remote.onButtonPressed(0);  // 거실 조명 켜기
remote.onButtonPressed(1);  // 선풍기 최고 속도
remote.undoButtonPressed(); // 마지막 명령 취소
```

### 4. 함수형 명령 (람다 사용)
```cpp
FunctionalCommandManager funcManager;
int counter = 0;

funcManager.executeCommand(
    [&counter]() {
        counter += 5;
        std::cout << "Counter incremented by 5, now: " << counter << std::endl;
    },
    [&counter]() {
        counter -= 5;
        std::cout << "Counter decremented by 5, now: " << counter << std::endl;
    },
    "Increment counter by 5"
);

funcManager.executeCommand(
    [&counter]() {
        counter *= 2;
        std::cout << "Counter doubled, now: " << counter << std::endl;
    },
    [&counter]() {
        counter /= 2;
        std::cout << "Counter halved, now: " << counter << std::endl;
    },
    "Double counter"
);

funcManager.undo(); // counter /= 2
funcManager.undo(); // counter -= 5
funcManager.redo(); // counter += 5
```

### 5. 명령 대기열
```cpp
CommandQueue commandQueue;
TextEditor queueEditor;

// 대기열에 명령 추가
commandQueue.addCommand(std::make_unique<InsertTextCommand>(queueEditor, "First ", 0));
commandQueue.addCommand(std::make_unique<InsertTextCommand>(queueEditor, "Second ", 6));
commandQueue.addCommand(std::make_unique<InsertTextCommand>(queueEditor, "Third", 13));

// 모든 명령 처리
commandQueue.processCommands();
queueEditor.display(); // "First Second Third"
```

### 실행 결과
```
Executing: Insert 'Hello' at position 0
Inserted 'Hello' at position 0
Content: "Hello"

Executing: Insert ' World' at position 5
Inserted ' World' at position 5
Content: "Hello World"

Executing macro: Format Text
Deleted 11 characters at position 0
Inserted 'Welcome to' at position 0
Inserted ' Command Pattern!' at position 10
Content: "Welcome to Command Pattern!"

Living Room light is ON (brightness: 100%)
Ceiling fan speed set to 3 (HIGH)

Counter incremented by 5, now: 5
Counter doubled, now: 10
Undoing: Double counter
Counter halved, now: 5
```

## C++의 장점

### 1. 스마트 포인터를 통한 안전한 메모리 관리
```cpp
std::unique_ptr<Command> command;
undoStack.push(std::move(command)); // 안전한 소유권 이전
```

### 2. STL 컨테이너를 활용한 효율적 구조
```cpp
std::stack<std::unique_ptr<Command>> undoStack; // undo 스택
std::queue<std::unique_ptr<Command>> commandQueue; // 명령 대기열
std::vector<std::unique_ptr<Command>> commands; // 매크로 명령 목록
```

### 3. 람다와 std::function으로 함수형 접근
```cpp
using FunctionalCommand = std::function<void()>;
FunctionalCommand executeFunc = [&]() { /* ... */ };
```

### 4. RAII를 통한 자동 리소스 관리
```cpp
~Command() = default; // 가상 소멸자로 안전한 정리
```

### 5. 템플릿 활용 가능성
```cpp
template<typename Receiver>
class GenericCommand {
    Receiver& receiver;
    // 타입 안전한 제네릭 명령
};
```

### 6. 멀티스레딩 지원
```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 비동기 처리
```

### 7. 예외 안전성
```cpp
try {
    command->execute();
} catch (const std::exception& e) {
    // 예외 처리 및 복구
}
```

## 적용 상황

### 1. GUI 애플리케이션
- 메뉴 항목, 버튼, 키보드 단축키
- Undo/Redo 기능
- 매크로 녹화 및 재생

### 2. 텍스트 에디터 / IDE
- 편집 작업 (삽입, 삭제, 교체)
- 코드 리팩토링 작업
- 다중 커서 편집

### 3. 트랜잭션 처리
- 데이터베이스 트랜잭션
- 롤백/커밋 기능
- 배치 처리

### 4. 게임 개발
- 플레이어 액션 시스템
- AI 명령 시스템
- 게임 상태 저장/복원

### 5. 로봇 제어
- 동작 명령 시퀀스
- 프로그래밍 가능한 동작
- 안전 중단 및 복구

### 6. 워크플로우 엔진
- 비즈니스 프로세스 자동화
- 작업 스케줄링
- 병렬 처리

### 7. 리모컨/IoT 기기
- 스마트 홈 제어
- 기기 간 통신
- 상태 동기화

## 장점과 단점

### 장점
1. **분리**: Invoker와 Receiver의 분리
2. **확장성**: 새로운 명령 쉽게 추가
3. **조합성**: 매크로 명령으로 복잡한 작업 구성
4. **실행 취소**: Undo/Redo 기능 구현 용이
5. **로깅**: 명령 실행 기록 및 감사
6. **대기열**: 명령 스케줄링 및 배치 처리
7. **테스트**: 명령을 독립적으로 테스트 가능

### 단점
1. **복잡성**: 간단한 작업에는 과도한 구조
2. **메모리**: 명령 객체 저장으로 인한 메모리 사용
3. **성능**: 간접 호출로 인한 미미한 성능 저하

## 관련 패턴

- **Composite**: 매크로 명령 구현 시 사용
- **Memento**: 명령의 상태 저장과 함께 사용
- **Prototype**: 명령 복제를 위해 사용 가능
- **Chain of Responsibility**: 명령 처리 체인과 결합 가능

Command 패턴은 요청을 객체로 변환하여 매우 유연하고 확장 가능한 시스템을 만들 수 있게 해주며, 특히 실행 취소 기능과 복잡한 작업 조합이 필요한 애플리케이션에서 매우 유용합니다.