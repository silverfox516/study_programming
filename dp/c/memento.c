#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Memento - stores the state of the editor
typedef struct {
    char* content;
    int cursor_position;
    char* filename;
    time_t timestamp;
} EditorMemento;

EditorMemento* editor_memento_create(const char* content, int cursor_pos, const char* filename) {
    EditorMemento* memento = malloc(sizeof(EditorMemento));

    memento->content = malloc(strlen(content) + 1);
    strcpy(memento->content, content);

    memento->cursor_position = cursor_pos;

    memento->filename = malloc(strlen(filename) + 1);
    strcpy(memento->filename, filename);

    memento->timestamp = time(NULL);

    return memento;
}

void editor_memento_destroy(EditorMemento* memento) {
    if (memento->content) {
        free(memento->content);
    }
    if (memento->filename) {
        free(memento->filename);
    }
    free(memento);
}

// Originator - Text Editor
typedef struct {
    char* content;
    int cursor_position;
    char* filename;
    int max_content_size;
} TextEditor;

TextEditor* text_editor_create() {
    TextEditor* editor = malloc(sizeof(TextEditor));
    editor->max_content_size = 1024;
    editor->content = malloc(editor->max_content_size);
    editor->content[0] = '\0';
    editor->cursor_position = 0;
    editor->filename = malloc(strlen("untitled.txt") + 1);
    strcpy(editor->filename, "untitled.txt");
    return editor;
}

void text_editor_set_content(TextEditor* editor, const char* content) {
    int needed_size = strlen(content) + 1;
    if (needed_size > editor->max_content_size) {
        editor->max_content_size = needed_size * 2;
        editor->content = realloc(editor->content, editor->max_content_size);
    }
    strcpy(editor->content, content);
    editor->cursor_position = strlen(content);
    printf("Content set to: \"%s\"\n", content);
}

void text_editor_append_text(TextEditor* editor, const char* text) {
    int current_len = strlen(editor->content);
    int text_len = strlen(text);
    int needed_size = current_len + text_len + 1;

    if (needed_size > editor->max_content_size) {
        editor->max_content_size = needed_size * 2;
        editor->content = realloc(editor->content, editor->max_content_size);
    }

    strcat(editor->content, text);
    editor->cursor_position += text_len;
    printf("Appended: \"%s\" -> Content: \"%s\"\n", text, editor->content);
}

void text_editor_set_filename(TextEditor* editor, const char* filename) {
    free(editor->filename);
    editor->filename = malloc(strlen(filename) + 1);
    strcpy(editor->filename, filename);
    printf("Filename changed to: %s\n", filename);
}

void text_editor_set_cursor_position(TextEditor* editor, int position) {
    int content_len = strlen(editor->content);
    if (position >= 0 && position <= content_len) {
        editor->cursor_position = position;
        printf("Cursor moved to position %d\n", position);
    } else {
        printf("Invalid cursor position: %d\n", position);
    }
}

void text_editor_show_status(TextEditor* editor) {
    printf("Editor Status:\n");
    printf("  File: %s\n", editor->filename);
    printf("  Content: \"%s\"\n", editor->content);
    printf("  Length: %zu characters\n", strlen(editor->content));
    printf("  Cursor at: %d\n", editor->cursor_position);
    printf("---\n");
}

// Create memento
EditorMemento* text_editor_create_memento(TextEditor* editor) {
    printf("Creating memento (saving state)\n");
    return editor_memento_create(editor->content, editor->cursor_position, editor->filename);
}

// Restore from memento
void text_editor_restore_memento(TextEditor* editor, EditorMemento* memento) {
    printf("Restoring from memento (timestamp: %s", ctime(&memento->timestamp));

    // Update content
    int needed_size = strlen(memento->content) + 1;
    if (needed_size > editor->max_content_size) {
        editor->max_content_size = needed_size * 2;
        editor->content = realloc(editor->content, editor->max_content_size);
    }
    strcpy(editor->content, memento->content);

    // Update cursor position
    editor->cursor_position = memento->cursor_position;

    // Update filename
    free(editor->filename);
    editor->filename = malloc(strlen(memento->filename) + 1);
    strcpy(editor->filename, memento->filename);

    printf("State restored!\n");
}

void text_editor_destroy(TextEditor* editor) {
    if (editor->content) {
        free(editor->content);
    }
    if (editor->filename) {
        free(editor->filename);
    }
    free(editor);
}

// Caretaker - Undo/Redo Manager
typedef struct {
    EditorMemento** mementos;
    int count;
    int capacity;
    int current_index; // -1 means no current state, 0+ means index in array
} UndoRedoManager;

UndoRedoManager* undo_redo_manager_create() {
    UndoRedoManager* manager = malloc(sizeof(UndoRedoManager));
    manager->mementos = malloc(10 * sizeof(EditorMemento*));
    manager->count = 0;
    manager->capacity = 10;
    manager->current_index = -1;
    return manager;
}

void undo_redo_manager_save_state(UndoRedoManager* manager, EditorMemento* memento) {
    // If we're in the middle of undo/redo history, remove everything after current position
    if (manager->current_index < manager->count - 1) {
        for (int i = manager->current_index + 1; i < manager->count; i++) {
            editor_memento_destroy(manager->mementos[i]);
        }
        manager->count = manager->current_index + 1;
    }

    // Expand capacity if needed
    if (manager->count >= manager->capacity) {
        manager->capacity *= 2;
        manager->mementos = realloc(manager->mementos, manager->capacity * sizeof(EditorMemento*));
    }

    manager->mementos[manager->count] = memento;
    manager->count++;
    manager->current_index = manager->count - 1;

    printf("State saved (total states: %d)\n", manager->count);
}

EditorMemento* undo_redo_manager_undo(UndoRedoManager* manager) {
    if (manager->current_index > 0) {
        manager->current_index--;
        printf("Undo: Going back to state %d\n", manager->current_index);
        return manager->mementos[manager->current_index];
    } else {
        printf("Cannot undo: No previous states\n");
        return NULL;
    }
}

EditorMemento* undo_redo_manager_redo(UndoRedoManager* manager) {
    if (manager->current_index < manager->count - 1) {
        manager->current_index++;
        printf("Redo: Going forward to state %d\n", manager->current_index);
        return manager->mementos[manager->current_index];
    } else {
        printf("Cannot redo: No forward states\n");
        return NULL;
    }
}

bool undo_redo_manager_can_undo(UndoRedoManager* manager) {
    return manager->current_index > 0;
}

bool undo_redo_manager_can_redo(UndoRedoManager* manager) {
    return manager->current_index < manager->count - 1;
}

void undo_redo_manager_show_history(UndoRedoManager* manager) {
    printf("Undo/Redo History:\n");
    for (int i = 0; i < manager->count; i++) {
        char* indicator = (i == manager->current_index) ? " <-- CURRENT" : "";
        printf("  %d: \"%s\" (cursor: %d)%s\n",
               i, manager->mementos[i]->content,
               manager->mementos[i]->cursor_position, indicator);
    }
    printf("---\n");
}

void undo_redo_manager_destroy(UndoRedoManager* manager) {
    for (int i = 0; i < manager->count; i++) {
        editor_memento_destroy(manager->mementos[i]);
    }
    free(manager->mementos);
    free(manager);
}

// Game example - additional use case
typedef struct {
    int level;
    int score;
    int lives;
    int x, y; // player position
} GameMemento;

GameMemento* game_memento_create(int level, int score, int lives, int x, int y) {
    GameMemento* memento = malloc(sizeof(GameMemento));
    memento->level = level;
    memento->score = score;
    memento->lives = lives;
    memento->x = x;
    memento->y = y;
    return memento;
}

void game_memento_destroy(GameMemento* memento) {
    free(memento);
}

typedef struct {
    int level;
    int score;
    int lives;
    int x, y;
} Game;

Game* game_create() {
    Game* game = malloc(sizeof(Game));
    game->level = 1;
    game->score = 0;
    game->lives = 3;
    game->x = 0;
    game->y = 0;
    return game;
}

GameMemento* game_save(Game* game) {
    printf("Game saved: Level %d, Score %d, Lives %d, Position (%d,%d)\n",
           game->level, game->score, game->lives, game->x, game->y);
    return game_memento_create(game->level, game->score, game->lives, game->x, game->y);
}

void game_restore(Game* game, GameMemento* memento) {
    game->level = memento->level;
    game->score = memento->score;
    game->lives = memento->lives;
    game->x = memento->x;
    game->y = memento->y;
    printf("Game restored: Level %d, Score %d, Lives %d, Position (%d,%d)\n",
           game->level, game->score, game->lives, game->x, game->y);
}

void game_destroy(Game* game) {
    free(game);
}

// Client code
int main() {
    printf("=== Memento Pattern Demo - Text Editor with Undo/Redo ===\n\n");

    // Create text editor and undo/redo manager
    TextEditor* editor = text_editor_create();
    UndoRedoManager* manager = undo_redo_manager_create();

    // Initial state
    printf("=== Initial State ===\n");
    text_editor_show_status(editor);

    // Save initial state
    EditorMemento* initial_state = text_editor_create_memento(editor);
    undo_redo_manager_save_state(manager, initial_state);

    // Make some changes
    printf("=== Making Changes ===\n");
    text_editor_set_content(editor, "Hello World");
    EditorMemento* state1 = text_editor_create_memento(editor);
    undo_redo_manager_save_state(manager, state1);

    text_editor_append_text(editor, "!");
    EditorMemento* state2 = text_editor_create_memento(editor);
    undo_redo_manager_save_state(manager, state2);

    text_editor_set_filename(editor, "greeting.txt");
    EditorMemento* state3 = text_editor_create_memento(editor);
    undo_redo_manager_save_state(manager, state3);

    text_editor_append_text(editor, " How are you?");
    EditorMemento* state4 = text_editor_create_memento(editor);
    undo_redo_manager_save_state(manager, state4);

    text_editor_show_status(editor);
    undo_redo_manager_show_history(manager);

    // Test undo operations
    printf("=== Testing Undo Operations ===\n");
    EditorMemento* undo_memento = undo_redo_manager_undo(manager);
    if (undo_memento) {
        text_editor_restore_memento(editor, undo_memento);
        text_editor_show_status(editor);
    }

    undo_memento = undo_redo_manager_undo(manager);
    if (undo_memento) {
        text_editor_restore_memento(editor, undo_memento);
        text_editor_show_status(editor);
    }

    undo_redo_manager_show_history(manager);

    // Test redo operations
    printf("=== Testing Redo Operations ===\n");
    EditorMemento* redo_memento = undo_redo_manager_redo(manager);
    if (redo_memento) {
        text_editor_restore_memento(editor, redo_memento);
        text_editor_show_status(editor);
    }

    // Make a new change after undo (should clear redo history)
    printf("=== Making New Change After Undo ===\n");
    text_editor_append_text(editor, " [MODIFIED]");
    EditorMemento* new_state = text_editor_create_memento(editor);
    undo_redo_manager_save_state(manager, new_state);
    undo_redo_manager_show_history(manager);

    // Quick game example
    printf("\n=== Game Save/Load Example ===\n");
    Game* game = game_create();

    printf("Initial game state:\n");
    GameMemento* checkpoint1 = game_save(game);

    // Simulate gameplay
    game->level = 2;
    game->score = 1500;
    game->x = 10;
    game->y = 5;
    printf("\nAfter playing:\n");
    GameMemento* checkpoint2 = game_save(game);

    // Player dies, restore checkpoint
    game->lives--;
    printf("\nPlayer died! Restoring checkpoint:\n");
    game_restore(game, checkpoint1);

    // Cleanup
    text_editor_destroy(editor);
    undo_redo_manager_destroy(manager);
    game_destroy(game);
    game_memento_destroy(checkpoint1);
    game_memento_destroy(checkpoint2);

    printf("\n=== Memento Pattern Benefits ===\n");
    printf("1. Encapsulation: Internal state is not exposed\n");
    printf("2. Undo/Redo: Easy implementation of undo/redo functionality\n");
    printf("3. Snapshot: Can save and restore state at any point\n");
    printf("4. Independence: Originator and caretaker are loosely coupled\n");

    return 0;
}