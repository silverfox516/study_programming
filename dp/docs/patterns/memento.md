# Memento Pattern

## 개요
Memento 패턴은 객체의 상태 정보를 저장하고 나중에 이 상태로 되돌릴 수 있게 하는 행위 패턴입니다. 실행 취소 기능을 구현할 때 주로 사용됩니다.

## C 언어 구현
```c
// Memento - 상태 저장 객체
typedef struct EditorMemento {
    char* content;
    int cursor_position;
} EditorMemento;

EditorMemento* create_memento(const char* content, int cursor_pos) {
    EditorMemento* memento = malloc(sizeof(EditorMemento));
    memento->content = malloc(strlen(content) + 1);
    strcpy(memento->content, content);
    memento->cursor_position = cursor_pos;
    return memento;
}

void destroy_memento(EditorMemento* memento) {
    if (memento) {
        free(memento->content);
        free(memento);
    }
}

// Originator - 상태를 가지고 있는 객체
typedef struct TextEditor {
    char* content;
    int cursor_position;
} TextEditor;

TextEditor* create_editor() {
    TextEditor* editor = malloc(sizeof(TextEditor));
    editor->content = malloc(1);
    editor->content[0] = '\0';
    editor->cursor_position = 0;
    return editor;
}

void editor_write(TextEditor* editor, const char* text) {
    int old_len = strlen(editor->content);
    int new_len = old_len + strlen(text);
    editor->content = realloc(editor->content, new_len + 1);
    strcat(editor->content, text);
    editor->cursor_position = new_len;
    printf("Content: %s (cursor at %d)\n", editor->content, editor->cursor_position);
}

EditorMemento* editor_save(TextEditor* editor) {
    printf("Saving current state\n");
    return create_memento(editor->content, editor->cursor_position);
}

void editor_restore(TextEditor* editor, EditorMemento* memento) {
    printf("Restoring previous state\n");
    free(editor->content);
    editor->content = malloc(strlen(memento->content) + 1);
    strcpy(editor->content, memento->content);
    editor->cursor_position = memento->cursor_position;
    printf("Restored content: %s (cursor at %d)\n", editor->content, editor->cursor_position);
}

// Caretaker - 메멘토를 관리하는 객체
typedef struct History {
    EditorMemento* mementos[100];
    int count;
} History;

History* create_history() {
    History* history = malloc(sizeof(History));
    history->count = 0;
    return history;
}

void history_push(History* history, EditorMemento* memento) {
    if (history->count < 100) {
        history->mementos[history->count++] = memento;
    }
}

EditorMemento* history_pop(History* history) {
    if (history->count > 0) {
        return history->mementos[--history->count];
    }
    return NULL;
}
```

## 사용 예제
```c
TextEditor* editor = create_editor();
History* history = create_history();

// 작업 및 저장
editor_write(editor, "Hello ");
history_push(history, editor_save(editor));

editor_write(editor, "World");
history_push(history, editor_save(editor));

editor_write(editor, "!!!");

// 실행 취소
printf("\n--- Undo Operations ---\n");
EditorMemento* memento = history_pop(history);
if (memento) {
    editor_restore(editor, memento);
    destroy_memento(memento);
}

memento = history_pop(history);
if (memento) {
    editor_restore(editor, memento);
    destroy_memento(memento);
}
```

## 적용 상황
- **텍스트 에디터**: 실행 취소/다시 실행 기능
- **게임**: 체크포인트나 세이브 상태
- **트랜잭션**: 데이터베이스 롤백 기능
- **GUI 애플리케이션**: 사용자 액션의 취소 기능