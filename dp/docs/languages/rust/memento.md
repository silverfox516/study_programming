# Memento Pattern - Rust Implementation

## 개요

Memento 패턴은 객체의 내부 상태를 저장하고 나중에 복원할 수 있게 해줍니다. 캡슐화를 위반하지 않으면서 객체의 이전 상태로 롤백할 수 있는 기능을 제공합니다.

## 구조

```rust
// Memento - 상태를 저장하는 객체
#[derive(Debug, Clone)]
pub struct TextEditorMemento {
    content: String,
    cursor_position: usize,
    timestamp: SystemTime,
}

// Originator - 상태를 가지고 메멘토를 생성하는 객체
pub struct TextEditor {
    content: String,
    cursor_position: usize,
}

impl TextEditor {
    pub fn save_state(&self) -> TextEditorMemento {
        TextEditorMemento {
            content: self.content.clone(),
            cursor_position: self.cursor_position,
            timestamp: SystemTime::now(),
        }
    }

    pub fn restore_state(&mut self, memento: &TextEditorMemento) {
        self.content = memento.content.clone();
        self.cursor_position = memento.cursor_position;
    }

    pub fn type_text(&mut self, text: &str) {
        self.content.push_str(text);
        self.cursor_position += text.len();
    }
}

// Caretaker - 메멘토들을 관리하는 객체
pub struct EditorHistory {
    mementos: Vec<TextEditorMemento>,
    current_index: Option<usize>,
}

impl EditorHistory {
    pub fn save(&mut self, memento: TextEditorMemento) {
        // 현재 인덱스 이후의 히스토리 제거 (새로운 변경사항)
        if let Some(index) = self.current_index {
            self.mementos.truncate(index + 1);
        } else {
            self.mementos.clear();
        }

        self.mementos.push(memento);
        self.current_index = Some(self.mementos.len() - 1);
    }

    pub fn undo(&mut self) -> Option<&TextEditorMemento> {
        if let Some(index) = self.current_index {
            if index > 0 {
                self.current_index = Some(index - 1);
                return Some(&self.mementos[index - 1]);
            }
        }
        None
    }

    pub fn redo(&mut self) -> Option<&TextEditorMemento> {
        if let Some(index) = self.current_index {
            if index + 1 < self.mementos.len() {
                self.current_index = Some(index + 1);
                return Some(&self.mementos[index + 1]);
            }
        }
        None
    }
}
```

## 사용 예제

```rust
let mut editor = TextEditor::new();
let mut history = EditorHistory::new();

// 초기 상태 저장
history.save(editor.save_state());

// 텍스트 입력
editor.type_text("Hello ");
history.save(editor.save_state());

editor.type_text("World!");
history.save(editor.save_state());

// Undo 실행
if let Some(memento) = history.undo() {
    editor.restore_state(memento);
    println!("After undo: {}", editor.get_content()); // "Hello "
}

// Redo 실행
if let Some(memento) = history.redo() {
    editor.restore_state(memento);
    println!("After redo: {}", editor.get_content()); // "Hello World!"
}
```

## 적용 상황

### 1. 텍스트 에디터의 Undo/Redo
### 2. 게임의 체크포인트 시스템
### 3. 데이터베이스 트랜잭션 롤백
### 4. 설정 변경 되돌리기
### 5. 워크플로우 상태 복원