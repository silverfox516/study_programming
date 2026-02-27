# Memento Pattern - C++ Implementation

## 개요

Memento 패턴은 객체의 내부 상태를 저장하여 나중에 이 상태로 복원할 수 있게 하는 행위 패턴입니다. 객체의 캡슐화를 위반하지 않으면서도 객체의 이전 상태를 저장하고 복원할 수 있습니다.

## 구조

- **Memento**: 원본 객체의 상태를 저장하는 객체
- **Originator**: 상태를 가지며 Memento를 생성하고 복원하는 객체
- **Caretaker**: Memento를 관리하는 객체

## C++ 구현

### 1. 기본 텍스트 에디터 예제

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stack>

// Memento class
class TextMemento {
private:
    std::string content;
    int cursorPosition;

public:
    TextMemento(const std::string& content, int cursorPosition)
        : content(content), cursorPosition(cursorPosition) {}

    std::string getContent() const { return content; }
    int getCursorPosition() const { return cursorPosition; }
};

// Originator class
class TextEditor {
private:
    std::string content;
    int cursorPosition;

public:
    TextEditor() : cursorPosition(0) {}

    void write(const std::string& text) {
        content.insert(cursorPosition, text);
        cursorPosition += text.length();
        std::cout << "Written: \"" << text << "\"" << std::endl;
        std::cout << "Current content: \"" << content << "\"" << std::endl;
    }

    void setCursorPosition(int position) {
        if (position >= 0 && position <= content.length()) {
            cursorPosition = position;
        }
    }

    std::unique_ptr<TextMemento> createMemento() const {
        return std::make_unique<TextMemento>(content, cursorPosition);
    }

    void restoreMemento(const TextMemento& memento) {
        content = memento.getContent();
        cursorPosition = memento.getCursorPosition();
        std::cout << "Restored to: \"" << content << "\"" << std::endl;
    }

    void show() const {
        std::cout << "Content: \"" << content << "\", Cursor: " << cursorPosition << std::endl;
    }
};

// Caretaker class
class EditorHistory {
private:
    std::stack<std::unique_ptr<TextMemento>> history;
    TextEditor* editor;

public:
    explicit EditorHistory(TextEditor* editor) : editor(editor) {}

    void save() {
        history.push(editor->createMemento());
        std::cout << "State saved" << std::endl;
    }

    void undo() {
        if (!history.empty()) {
            auto memento = std::move(history.top());
            history.pop();
            editor->restoreMemento(*memento);
            std::cout << "Undo performed" << std::endl;
        } else {
            std::cout << "No more states to undo" << std::endl;
        }
    }

    size_t getHistorySize() const {
        return history.size();
    }
};
```

### 2. 게임 상태 저장 예제

```cpp
class GameMemento {
private:
    int level;
    int score;
    int lives;
    std::string playerName;

public:
    GameMemento(int level, int score, int lives, const std::string& playerName)
        : level(level), score(score), lives(lives), playerName(playerName) {}

    int getLevel() const { return level; }
    int getScore() const { return score; }
    int getLives() const { return lives; }
    std::string getPlayerName() const { return playerName; }
};

class Game {
private:
    int level;
    int score;
    int lives;
    std::string playerName;

public:
    Game(const std::string& playerName)
        : level(1), score(0), lives(3), playerName(playerName) {}

    void play() {
        // Simulate game progress
        score += 100;
        if (score % 500 == 0) {
            level++;
            std::cout << "Level up! Now at level " << level << std::endl;
        }

        // Random chance of losing a life
        if (rand() % 10 == 0 && lives > 0) {
            lives--;
            std::cout << "Lost a life! Lives remaining: " << lives << std::endl;
        }
    }

    std::unique_ptr<GameMemento> saveGame() const {
        return std::make_unique<GameMemento>(level, score, lives, playerName);
    }

    void loadGame(const GameMemento& save) {
        level = save.getLevel();
        score = save.getScore();
        lives = save.getLives();
        playerName = save.getPlayerName();
        std::cout << "Game loaded!" << std::endl;
    }

    void showStatus() const {
        std::cout << "Player: " << playerName << ", Level: " << level
                  << ", Score: " << score << ", Lives: " << lives << std::endl;
    }

    bool isGameOver() const {
        return lives <= 0;
    }
};

class SaveManager {
private:
    std::vector<std::unique_ptr<GameMemento>> saveSlots;

public:
    void saveToSlot(int slot, std::unique_ptr<GameMemento> save) {
        if (slot >= saveSlots.size()) {
            saveSlots.resize(slot + 1);
        }
        saveSlots[slot] = std::move(save);
        std::cout << "Game saved to slot " << slot << std::endl;
    }

    std::unique_ptr<GameMemento> loadFromSlot(int slot) {
        if (slot < saveSlots.size() && saveSlots[slot]) {
            // Create a copy of the memento
            auto& original = saveSlots[slot];
            return std::make_unique<GameMemento>(
                original->getLevel(),
                original->getScore(),
                original->getLives(),
                original->getPlayerName()
            );
        }
        std::cout << "No save found in slot " << slot << std::endl;
        return nullptr;
    }
};
```

## 사용 예제

```cpp
int main() {
    // 1. 텍스트 에디터 예제
    std::cout << "=== Text Editor Example ===" << std::endl;

    TextEditor editor;
    EditorHistory history(&editor);

    editor.write("Hello ");
    history.save();

    editor.write("World");
    history.save();

    editor.write("!");
    editor.show();

    history.undo(); // Back to "Hello World"
    history.undo(); // Back to "Hello "
    history.undo(); // No more states

    // 2. 게임 상태 저장 예제
    std::cout << "\n=== Game Save Example ===" << std::endl;

    Game game("Player1");
    SaveManager saveManager;

    // Play the game and save periodically
    for (int i = 0; i < 10; ++i) {
        game.play();
        if (i == 4) {
            // Save at checkpoint
            saveManager.saveToSlot(0, game.saveGame());
            std::cout << "Checkpoint saved!" << std::endl;
        }
    }

    game.showStatus();

    // Load from checkpoint
    auto checkpoint = saveManager.loadFromSlot(0);
    if (checkpoint) {
        game.loadGame(*checkpoint);
        game.showStatus();
    }

    return 0;
}
```

## C++의 장점

1. **스마트 포인터**: 안전한 메모리 관리
2. **캡슐화**: private 멤버로 상태 보호
3. **타입 안전성**: 컴파일 타임 타입 체크
4. **이동 시맨틱**: 효율적인 객체 전달

## 적용 상황

1. **텍스트 에디터**: Undo/Redo 기능
2. **게임**: 세이브/로드 시스템
3. **데이터베이스**: 트랜잭션 롤백
4. **디자인 도구**: 작업 히스토리 관리
5. **설정 관리**: 이전 설정으로 복원

Memento 패턴은 객체의 상태 관리와 되돌리기 기능 구현에 매우 유용한 패턴입니다.