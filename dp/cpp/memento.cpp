#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <chrono>
#include <sstream>
#include <iomanip>

// Forward declarations
class TextEditor;

// Memento class
class TextMemento {
private:
    std::string content;
    size_t cursorPosition;
    std::chrono::system_clock::time_point timestamp;

    // Private constructor - only Originator can create
    friend class TextEditor;
    TextMemento(const std::string& content, size_t cursor)
        : content(content), cursorPosition(cursor),
          timestamp(std::chrono::system_clock::now()) {}

public:
    std::string getTimestamp() const {
        auto time_t = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    size_t getContentSize() const {
        return content.length();
    }

    // Only TextEditor can access the state
    friend class TextEditor;
};

// Originator - Text Editor
class TextEditor {
private:
    std::string content;
    size_t cursorPosition;

public:
    TextEditor() : cursorPosition(0) {}

    void write(const std::string& text) {
        content.insert(cursorPosition, text);
        cursorPosition += text.length();
        std::cout << "📝 Wrote: '" << text << "'" << std::endl;
    }

    void deletePrevious(size_t count = 1) {
        if (cursorPosition > 0) {
            size_t deleteCount = std::min(count, cursorPosition);
            size_t startPos = cursorPosition - deleteCount;
            std::string deleted = content.substr(startPos, deleteCount);
            content.erase(startPos, deleteCount);
            cursorPosition = startPos;
            std::cout << "🗑️ Deleted: '" << deleted << "'" << std::endl;
        }
    }

    void moveCursor(size_t position) {
        cursorPosition = std::min(position, content.length());
        std::cout << "👆 Cursor moved to position: " << cursorPosition << std::endl;
    }

    void replace(size_t start, size_t length, const std::string& newText) {
        if (start < content.length()) {
            length = std::min(length, content.length() - start);
            std::string replaced = content.substr(start, length);
            content.replace(start, length, newText);
            cursorPosition = start + newText.length();
            std::cout << "🔄 Replaced '" << replaced << "' with '" << newText << "'" << std::endl;
        }
    }

    // Create memento
    std::unique_ptr<TextMemento> createMemento() const {
        return std::unique_ptr<TextMemento>(new TextMemento(content, cursorPosition));
    }

    // Restore from memento
    void restoreFromMemento(const TextMemento& memento) {
        content = memento.content;
        cursorPosition = memento.cursorPosition;
        std::cout << "↩️ Restored state from: " << memento.getTimestamp() << std::endl;
    }

    void display() const {
        std::cout << "📄 Content: \"" << content << "\"" << std::endl;
        std::cout << "   Cursor at position: " << cursorPosition << std::endl;
        std::cout << "   Length: " << content.length() << " characters" << std::endl;
    }

    const std::string& getContent() const { return content; }
    size_t getCursorPosition() const { return cursorPosition; }
    bool isEmpty() const { return content.empty(); }
};

// Caretaker - manages mementos
class EditorHistory {
private:
    std::stack<std::unique_ptr<TextMemento>> undoStack;
    std::stack<std::unique_ptr<TextMemento>> redoStack;
    static const size_t MAX_HISTORY = 50;

public:
    void saveState(std::unique_ptr<TextMemento> memento) {
        // Clear redo stack when new state is saved
        while (!redoStack.empty()) {
            redoStack.pop();
        }

        undoStack.push(std::move(memento));

        // Limit history size
        if (undoStack.size() > MAX_HISTORY) {
            // Remove oldest state (bottom of stack)
            std::stack<std::unique_ptr<TextMemento>> temp;
            while (undoStack.size() > 1) {
                temp.push(std::move(undoStack.top()));
                undoStack.pop();
            }
            undoStack.pop(); // Remove oldest

            while (!temp.empty()) {
                undoStack.push(std::move(temp.top()));
                temp.pop();
            }
        }

        std::cout << "💾 State saved (history size: " << undoStack.size() << ")" << std::endl;
    }

    std::unique_ptr<TextMemento> undo() {
        if (undoStack.empty()) {
            std::cout << "⚠️ Nothing to undo" << std::endl;
            return nullptr;
        }

        auto memento = std::move(undoStack.top());
        undoStack.pop();
        return memento;
    }

    std::unique_ptr<TextMemento> redo() {
        if (redoStack.empty()) {
            std::cout << "⚠️ Nothing to redo" << std::endl;
            return nullptr;
        }

        auto memento = std::move(redoStack.top());
        redoStack.pop();
        return memento;
    }

    void addToRedoStack(std::unique_ptr<TextMemento> memento) {
        redoStack.push(std::move(memento));
    }

    size_t getUndoStackSize() const { return undoStack.size(); }
    size_t getRedoStackSize() const { return redoStack.size(); }

    void showHistory() const {
        std::cout << "\n📚 History:" << std::endl;
        std::cout << "   Undo stack: " << undoStack.size() << " states" << std::endl;
        std::cout << "   Redo stack: " << redoStack.size() << " states" << std::endl;
    }
};

// Example 2: Game State Memento
struct GameState {
    int level;
    int score;
    int lives;
    std::string playerName;
    std::map<std::string, int> inventory;

    std::string toString() const {
        std::ostringstream oss;
        oss << "Level " << level << ", Score: " << score << ", Lives: " << lives;
        return oss.str();
    }
};

class GameMemento {
private:
    GameState state;
    std::chrono::system_clock::time_point saveTime;

    friend class Game;
    friend class GameSaveManager;

    explicit GameMemento(const GameState& state)
        : state(state), saveTime(std::chrono::system_clock::now()) {}

public:
    static std::unique_ptr<GameMemento> create(const GameState& state) {
        return std::unique_ptr<GameMemento>(new GameMemento(state));
    }

    std::string getSaveTime() const {
        auto time_t = std::chrono::system_clock::to_time_t(saveTime);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
        return ss.str();
    }

    const GameState& getState() const { return state; }
};

class Game {
private:
    GameState currentState;

public:
    Game(const std::string& playerName) {
        currentState.playerName = playerName;
        currentState.level = 1;
        currentState.score = 0;
        currentState.lives = 3;
        currentState.inventory["coins"] = 0;
        currentState.inventory["keys"] = 0;
    }

    void levelUp() {
        currentState.level++;
        currentState.score += 1000;
        std::cout << "🎮 Level up! Now at " << currentState.toString() << std::endl;
    }

    void scorePoints(int points) {
        currentState.score += points;
        std::cout << "⭐ Scored " << points << " points! Total: " << currentState.score << std::endl;
    }

    void loseLife() {
        if (currentState.lives > 0) {
            currentState.lives--;
            std::cout << "💔 Lost a life! Lives remaining: " << currentState.lives << std::endl;
        }
    }

    void collectItem(const std::string& item, int quantity = 1) {
        currentState.inventory[item] += quantity;
        std::cout << "💎 Collected " << quantity << " " << item
                  << " (total: " << currentState.inventory[item] << ")" << std::endl;
    }

    std::unique_ptr<GameMemento> saveGame() const {
        std::cout << "💾 Game saved at " << currentState.toString() << std::endl;
        return std::unique_ptr<GameMemento>(new GameMemento(currentState));
    }

    void loadGame(const GameMemento& memento) {
        currentState = memento.state;
        std::cout << "📂 Game loaded from " << memento.getSaveTime()
                  << " - " << currentState.toString() << std::endl;
    }

    void displayStatus() const {
        std::cout << "🎮 " << currentState.playerName << " - " << currentState.toString() << std::endl;
        std::cout << "   Inventory: ";
        for (const auto& [item, count] : currentState.inventory) {
            std::cout << item << ":" << count << " ";
        }
        std::cout << std::endl;
    }

    bool isGameOver() const {
        return currentState.lives <= 0;
    }

    const GameState& getCurrentState() const { return currentState; }
};

class SaveManager {
private:
    std::vector<std::unique_ptr<GameMemento>> saveSlots;
    static const size_t MAX_SAVES = 5;

public:
    void saveToSlot(size_t slot, std::unique_ptr<GameMemento> memento) {
        if (slot < MAX_SAVES) {
            if (saveSlots.size() <= slot) {
                saveSlots.resize(slot + 1);
            }
            saveSlots[slot] = std::move(memento);
            std::cout << "💾 Game saved to slot " << slot << std::endl;
        } else {
            std::cout << "❌ Invalid save slot: " << slot << std::endl;
        }
    }

    std::unique_ptr<GameMemento> loadFromSlot(size_t slot) {
        if (slot < saveSlots.size() && saveSlots[slot]) {
            // Create a copy of the memento
            auto originalMemento = saveSlots[slot].get();
            std::cout << "📂 Loading from slot " << slot << std::endl;
            return GameMemento::create(originalMemento->getState());
        } else {
            std::cout << "❌ No save found in slot " << slot << std::endl;
            return nullptr;
        }
    }

    void showSaveSlots() const {
        std::cout << "\n💾 Save Slots:" << std::endl;
        for (size_t i = 0; i < MAX_SAVES; ++i) {
            std::cout << "   Slot " << i << ": ";
            if (i < saveSlots.size() && saveSlots[i]) {
                std::cout << saveSlots[i]->getState().toString()
                          << " (saved at " << saveSlots[i]->getSaveTime() << ")";
            } else {
                std::cout << "Empty";
            }
            std::cout << std::endl;
        }
    }

    void deleteSlot(size_t slot) {
        if (slot < saveSlots.size() && saveSlots[slot]) {
            saveSlots[slot].reset();
            std::cout << "🗑️ Deleted save slot " << slot << std::endl;
        } else {
            std::cout << "❌ No save to delete in slot " << slot << std::endl;
        }
    }
};

// Example 3: Configuration Memento
class Configuration {
private:
    std::map<std::string, std::string> settings;

public:
    void setSetting(const std::string& key, const std::string& value) {
        settings[key] = value;
        std::cout << "⚙️ Setting " << key << " = " << value << std::endl;
    }

    std::string getSetting(const std::string& key) const {
        auto it = settings.find(key);
        return it != settings.end() ? it->second : "";
    }

    class ConfigMemento {
    private:
        std::map<std::string, std::string> savedSettings;
        std::string description;

        friend class Configuration;

        ConfigMemento(const std::map<std::string, std::string>& settings, const std::string& desc)
            : savedSettings(settings), description(desc) {}

    public:
        const std::string& getDescription() const { return description; }
        size_t getSettingCount() const { return savedSettings.size(); }
    };

    std::unique_ptr<ConfigMemento> createSnapshot(const std::string& description) const {
        std::cout << "📸 Creating configuration snapshot: " << description << std::endl;
        return std::unique_ptr<ConfigMemento>(new ConfigMemento(settings, description));
    }

    void restoreFromSnapshot(const ConfigMemento& memento) {
        settings = memento.savedSettings;
        std::cout << "🔄 Restored configuration: " << memento.description << std::endl;
    }

    void displaySettings() const {
        std::cout << "⚙️ Current Configuration:" << std::endl;
        for (const auto& [key, value] : settings) {
            std::cout << "   " << key << " = " << value << std::endl;
        }
    }

    void reset() {
        settings.clear();
        std::cout << "🔄 Configuration reset" << std::endl;
    }
};

class ConfigurationManager {
private:
    std::map<std::string, std::unique_ptr<Configuration::ConfigMemento>> presets;

public:
    void savePreset(const std::string& name, std::unique_ptr<Configuration::ConfigMemento> memento) {
        presets[name] = std::move(memento);
        std::cout << "💾 Preset '" << name << "' saved" << std::endl;
    }

    std::unique_ptr<Configuration::ConfigMemento> loadPreset(const std::string& name) {
        auto it = presets.find(name);
        if (it != presets.end()) {
            std::cout << "📂 Loading preset '" << name << "'" << std::endl;
            // Return a copy
            return std::unique_ptr<Configuration::ConfigMemento>(
                new Configuration::ConfigMemento(*it->second));
        } else {
            std::cout << "❌ Preset '" << name << "' not found" << std::endl;
            return nullptr;
        }
    }

    void listPresets() const {
        std::cout << "\n📋 Available Presets:" << std::endl;
        for (const auto& [name, memento] : presets) {
            std::cout << "   " << name << " (" << memento->getSettingCount() << " settings)" << std::endl;
        }
    }

    void deletePreset(const std::string& name) {
        auto it = presets.find(name);
        if (it != presets.end()) {
            presets.erase(it);
            std::cout << "🗑️ Preset '" << name << "' deleted" << std::endl;
        } else {
            std::cout << "❌ Preset '" << name << "' not found" << std::endl;
        }
    }
};

int main() {
    std::cout << "=== Memento Pattern Demo ===" << std::endl;

    // Example 1: Text Editor with Undo/Redo
    std::cout << "\n1. Text Editor with Undo/Redo:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    TextEditor editor;
    EditorHistory history;

    // Initial state
    editor.display();

    // Save initial state
    history.saveState(editor.createMemento());

    // Make some changes
    editor.write("Hello");
    editor.display();
    history.saveState(editor.createMemento());

    editor.write(" World");
    editor.display();
    history.saveState(editor.createMemento());

    editor.write("!");
    editor.display();
    history.saveState(editor.createMemento());

    // Test undo
    std::cout << "\nTesting undo operations:" << std::endl;
    if (auto memento = history.undo()) {
        history.addToRedoStack(editor.createMemento());
        editor.restoreFromMemento(*memento);
        editor.display();
    }

    if (auto memento = history.undo()) {
        history.addToRedoStack(editor.createMemento());
        editor.restoreFromMemento(*memento);
        editor.display();
    }

    // Test redo
    std::cout << "\nTesting redo operations:" << std::endl;
    if (auto memento = history.redo()) {
        history.saveState(editor.createMemento());
        editor.restoreFromMemento(*memento);
        editor.display();
    }

    history.showHistory();

    // Example 2: Game Save System
    std::cout << "\n\n2. Game Save System:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    Game game("Player1");
    SaveManager saveManager;

    game.displayStatus();

    // Play the game
    game.scorePoints(500);
    game.collectItem("coins", 10);
    game.collectItem("keys", 2);
    game.displayStatus();

    // Save to slot 0
    saveManager.saveToSlot(0, game.saveGame());

    // Continue playing
    game.levelUp();
    game.scorePoints(750);
    game.collectItem("coins", 15);
    game.displayStatus();

    // Save to slot 1
    saveManager.saveToSlot(1, game.saveGame());

    // Simulate losing lives
    game.loseLife();
    game.loseLife();
    game.displayStatus();

    // Save to slot 2
    saveManager.saveToSlot(2, game.saveGame());

    saveManager.showSaveSlots();

    // Load an earlier save
    std::cout << "\nLoading earlier save:" << std::endl;
    if (auto memento = saveManager.loadFromSlot(1)) {
        game.loadGame(*memento);
        game.displayStatus();
    }

    // Example 3: Configuration Management
    std::cout << "\n\n3. Configuration Management:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    Configuration config;
    ConfigurationManager configManager;

    // Set some initial configuration
    config.setSetting("theme", "dark");
    config.setSetting("language", "english");
    config.setSetting("notifications", "enabled");
    config.displaySettings();

    // Save as preset
    configManager.savePreset("default", config.createSnapshot("Default settings"));

    // Change configuration
    config.setSetting("theme", "light");
    config.setSetting("language", "spanish");
    config.setSetting("sound", "enabled");
    config.displaySettings();

    // Save as another preset
    configManager.savePreset("spanish_light", config.createSnapshot("Spanish light theme"));

    // Create gaming configuration
    config.reset();
    config.setSetting("theme", "dark");
    config.setSetting("language", "english");
    config.setSetting("sound", "enabled");
    config.setSetting("graphics", "high");
    config.setSetting("fps", "60");
    config.displaySettings();

    configManager.savePreset("gaming", config.createSnapshot("Gaming optimized"));

    configManager.listPresets();

    // Load a preset
    std::cout << "\nLoading default preset:" << std::endl;
    if (auto memento = configManager.loadPreset("default")) {
        config.restoreFromSnapshot(*memento);
        config.displaySettings();
    }

    // Benefits and considerations
    std::cout << "\n\n4. Memento Pattern Benefits:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "✓ Preserves encapsulation boundaries" << std::endl;
    std::cout << "✓ Simplifies originator by delegating state management" << std::endl;
    std::cout << "✓ Supports unlimited undo operations" << std::endl;
    std::cout << "✓ State snapshots are immutable" << std::endl;
    std::cout << "✓ Easy to implement save/restore functionality" << std::endl;

    std::cout << "\n5. Memento Pattern Considerations:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "⚠️ Can be memory intensive for large states" << std::endl;
    std::cout << "⚠️ Creating mementos can be expensive" << std::endl;
    std::cout << "⚠️ Caretaker must manage memento lifecycle" << std::endl;
    std::cout << "⚠️ May break encapsulation if not carefully designed" << std::endl;

    return 0;
}