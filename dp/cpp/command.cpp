#include <iostream>
#include <memory>
#include <vector>
#include <stack>
#include <string>
#include <functional>
#include <unordered_map>
#include <queue>
#include <thread>
#include <chrono>

// Command interface
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getDescription() const = 0;
    virtual bool isUndoable() const { return true; }
};

// Receiver - Text Editor
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
            std::cout << "Replaced " << actualLength << " characters with '" << newText << "' at position " << position << std::endl;
        }
    }

    void setCursorPosition(size_t position) {
        cursorPosition = std::min(position, content.length());
        std::cout << "Cursor moved to position " << cursorPosition << std::endl;
    }

    const std::string& getContent() const { return content; }
    size_t getCursorPosition() const { return cursorPosition; }

    void clear() {
        content.clear();
        cursorPosition = 0;
        std::cout << "Editor cleared" << std::endl;
    }

    void display() const {
        std::cout << "Content: \"" << content << "\"" << std::endl;
        std::cout << "Cursor at position: " << cursorPosition << std::endl;
    }
};

// Concrete Commands
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

class DeleteTextCommand : public Command {
private:
    TextEditor& editor;
    size_t position;
    size_t length;
    std::string deletedText; // Store for undo

public:
    DeleteTextCommand(TextEditor& editor, size_t position, size_t length)
        : editor(editor), position(position), length(length) {}

    void execute() override {
        // Store the text that will be deleted for undo
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

class ReplaceTextCommand : public Command {
private:
    TextEditor& editor;
    size_t position;
    size_t length;
    std::string newText;
    std::string oldText; // Store for undo

public:
    ReplaceTextCommand(TextEditor& editor, size_t position, size_t length, const std::string& newText)
        : editor(editor), position(position), length(length), newText(newText) {}

    void execute() override {
        // Store the old text for undo
        const std::string& content = editor.getContent();
        if (position < content.length()) {
            size_t actualLength = std::min(length, content.length() - position);
            oldText = content.substr(position, actualLength);
        }
        editor.replaceText(position, length, newText);
    }

    void undo() override {
        if (!oldText.empty()) {
            editor.replaceText(position, newText.length(), oldText);
        }
    }

    std::string getDescription() const override {
        return "Replace " + std::to_string(length) + " characters with '" + newText + "' at position " + std::to_string(position);
    }
};

// Macro Command (Composite Command)
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
        // Undo in reverse order
        for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
            (*it)->undo();
        }
    }

    std::string getDescription() const override {
        return "Macro: " + description + " (" + std::to_string(commands.size()) + " commands)";
    }
};

// Invoker - Command Manager with Undo/Redo
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
            // Clear redo stack when new command is executed
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

// Example 2: Remote Control (Classic Command Pattern Example)
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
        if (brightness > 0) {
            isOn = true;
        }
        std::cout << location << " light brightness set to " << brightness << "%" << std::endl;
    }

    bool getIsOn() const { return isOn; }
    int getBrightness() const { return brightness; }
    const std::string& getLocation() const { return location; }
};

class Fan {
private:
    std::string location;
    int speed; // 0-3 (0=off, 1=low, 2=medium, 3=high)

public:
    explicit Fan(const std::string& location) : location(location), speed(0) {}

    void turnOn() {
        speed = 1;
        std::cout << location << " fan is ON (speed: " << speed << ")" << std::endl;
    }

    void turnOff() {
        speed = 0;
        std::cout << location << " fan is OFF" << std::endl;
    }

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

// Remote Control Commands
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

class LightOffCommand : public Command {
private:
    Light& light;

public:
    explicit LightOffCommand(Light& light) : light(light) {}

    void execute() override { light.turnOff(); }
    void undo() override { light.turnOn(); }
    std::string getDescription() const override {
        return "Turn off " + light.getLocation() + " light";
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

// No Operation Command
class NoCommand : public Command {
public:
    void execute() override {
        std::cout << "No operation" << std::endl;
    }
    void undo() override {
        std::cout << "No operation to undo" << std::endl;
    }
    std::string getDescription() const override {
        return "No Command";
    }
    bool isUndoable() const override { return false; }
};

// Remote Control
class RemoteControl {
private:
    static const int SLOT_COUNT = 7;
    std::vector<std::unique_ptr<Command>> onCommands;
    std::vector<std::unique_ptr<Command>> offCommands;
    std::unique_ptr<Command> lastCommand;

public:
    RemoteControl() {
        auto noCommand = std::make_unique<NoCommand>();
        for (int i = 0; i < SLOT_COUNT; ++i) {
            onCommands.push_back(std::make_unique<NoCommand>());
            offCommands.push_back(std::make_unique<NoCommand>());
        }
        lastCommand = std::make_unique<NoCommand>();
    }

    void setCommand(int slot, std::unique_ptr<Command> onCommand, std::unique_ptr<Command> offCommand) {
        if (slot >= 0 && slot < SLOT_COUNT) {
            onCommands[slot] = std::move(onCommand);
            offCommands[slot] = std::move(offCommand);
        }
    }

    void onButtonPressed(int slot) {
        if (slot >= 0 && slot < SLOT_COUNT) {
            onCommands[slot]->execute();
            lastCommand = std::make_unique<LightOnCommand>(*static_cast<LightOnCommand*>(onCommands[slot].get()));
        }
    }

    void offButtonPressed(int slot) {
        if (slot >= 0 && slot < SLOT_COUNT) {
            offCommands[slot]->execute();
            lastCommand = std::make_unique<LightOffCommand>(*static_cast<LightOffCommand*>(offCommands[slot].get()));
        }
    }

    void undoButtonPressed() {
        lastCommand->undo();
    }

    void displayStatus() const {
        std::cout << "\n--- Remote Control Status ---" << std::endl;
        for (int i = 0; i < SLOT_COUNT; ++i) {
            std::cout << "Slot " << i << ": " << onCommands[i]->getDescription()
                      << " | " << offCommands[i]->getDescription() << std::endl;
        }
        std::cout << "Last command: " << lastCommand->getDescription() << std::endl;
        std::cout << "----------------------------" << std::endl;
    }
};

// Example 3: Modern C++ Functional Commands
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

        // Clear redo stack
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

// Example 4: Queued Commands (Command Queue)
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

            // Simulate processing time
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        processing = false;
        std::cout << "All commands processed." << std::endl;
    }

    size_t getQueueSize() const {
        return commandQueue.size();
    }

    bool isProcessing() const {
        return processing;
    }
};

int main() {
    std::cout << "=== Command Pattern Demo ===" << std::endl;

    // Example 1: Text Editor with Undo/Redo
    std::cout << "\n1. Text Editor with Command Pattern:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    TextEditor editor;
    CommandManager commandManager;

    // Execute some commands
    commandManager.executeCommand(std::make_unique<InsertTextCommand>(editor, "Hello", 0));
    editor.display();

    commandManager.executeCommand(std::make_unique<InsertTextCommand>(editor, " World", 5));
    editor.display();

    commandManager.executeCommand(std::make_unique<InsertTextCommand>(editor, "!", 11));
    editor.display();

    // Test undo
    std::cout << "\nTesting undo operations:" << std::endl;
    commandManager.undo();
    editor.display();

    commandManager.undo();
    editor.display();

    // Test redo
    std::cout << "\nTesting redo operations:" << std::endl;
    commandManager.redo();
    editor.display();

    // Macro command
    std::cout << "\nTesting macro command:" << std::endl;
    auto macro = std::make_unique<MacroCommand>("Format Text");
    macro->addCommand(std::make_unique<DeleteTextCommand>(editor, 0, 11)); // Clear existing
    macro->addCommand(std::make_unique<InsertTextCommand>(editor, "Welcome to", 0));
    macro->addCommand(std::make_unique<InsertTextCommand>(editor, " Command Pattern!", 10));

    commandManager.executeCommand(std::move(macro));
    editor.display();

    commandManager.undo(); // Undo entire macro
    editor.display();

    // Example 2: Remote Control
    std::cout << "\n\n2. Remote Control with Command Pattern:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    // Create devices
    Light livingRoomLight("Living Room");
    Light bedroomLight("Bedroom");
    Fan ceilingFan("Ceiling");

    RemoteControl remote;

    // Set up remote control slots
    remote.setCommand(0,
        std::make_unique<LightOnCommand>(livingRoomLight),
        std::make_unique<LightOffCommand>(livingRoomLight));

    remote.setCommand(1,
        std::make_unique<LightOnCommand>(bedroomLight),
        std::make_unique<LightOffCommand>(bedroomLight));

    remote.setCommand(2,
        std::make_unique<FanSpeedCommand>(ceilingFan, 3),
        std::make_unique<FanSpeedCommand>(ceilingFan, 0));

    remote.displayStatus();

    // Test remote control
    std::cout << "\nTesting remote control:" << std::endl;
    remote.onButtonPressed(0);  // Living room light on
    remote.onButtonPressed(1);  // Bedroom light on
    remote.onButtonPressed(2);  // Fan high speed

    std::cout << "\nTurning things off:" << std::endl;
    remote.offButtonPressed(0); // Living room light off
    remote.offButtonPressed(2); // Fan off

    std::cout << "\nTesting undo:" << std::endl;
    remote.undoButtonPressed(); // Undo last command

    // Example 3: Functional Commands
    std::cout << "\n\n3. Functional Commands (Modern C++):" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    FunctionalCommandManager funcManager;
    int counter = 0;

    // Lambda-based commands
    funcManager.executeCommand(
        [&counter]() { counter += 5; std::cout << "Counter incremented by 5, now: " << counter << std::endl; },
        [&counter]() { counter -= 5; std::cout << "Counter decremented by 5, now: " << counter << std::endl; },
        "Increment counter by 5"
    );

    funcManager.executeCommand(
        [&counter]() { counter *= 2; std::cout << "Counter doubled, now: " << counter << std::endl; },
        [&counter]() { counter /= 2; std::cout << "Counter halved, now: " << counter << std::endl; },
        "Double counter"
    );

    std::cout << "\nTesting functional undo/redo:" << std::endl;
    funcManager.undo();
    funcManager.undo();
    funcManager.redo();

    // Example 4: Command Queue
    std::cout << "\n\n4. Command Queue Processing:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    CommandQueue commandQueue;
    TextEditor queueEditor;

    // Add commands to queue
    commandQueue.addCommand(std::make_unique<InsertTextCommand>(queueEditor, "First ", 0));
    commandQueue.addCommand(std::make_unique<InsertTextCommand>(queueEditor, "Second ", 6));
    commandQueue.addCommand(std::make_unique<InsertTextCommand>(queueEditor, "Third", 13));

    std::cout << "Editor before processing: ";
    queueEditor.display();

    // Process all commands
    commandQueue.processCommands();

    std::cout << "Editor after processing: ";
    queueEditor.display();

    // Benefits summary
    std::cout << "\n\n5. Command Pattern Benefits:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "✓ Decouples invoker from receiver" << std::endl;
    std::cout << "✓ Supports undo/redo operations" << std::endl;
    std::cout << "✓ Supports macro commands (composite)" << std::endl;
    std::cout << "✓ Supports queuing and scheduling commands" << std::endl;
    std::cout << "✓ Supports logging and auditing" << std::endl;
    std::cout << "✓ Enables functional programming with lambdas" << std::endl;
    std::cout << "✓ Makes GUI actions easily testable" << std::endl;

    return 0;
}