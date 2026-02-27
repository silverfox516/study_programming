/// Memento Pattern Implementation in Rust
///
/// The Memento pattern captures and externalizes an object's internal state
/// so that the object can be restored to this state later, without violating
/// encapsulation.

use std::collections::HashMap;
use std::time::{SystemTime, UNIX_EPOCH};

/// Text editor memento example
#[derive(Debug, Clone)]
pub struct TextEditorMemento {
    content: String,
    cursor_position: usize,
    selection: Option<(usize, usize)>,
    timestamp: u64,
}

impl TextEditorMemento {
    pub fn new(content: String, cursor_position: usize, selection: Option<(usize, usize)>) -> Self {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();

        TextEditorMemento {
            content,
            cursor_position,
            selection,
            timestamp,
        }
    }

    pub fn get_timestamp(&self) -> u64 {
        self.timestamp
    }
}

#[derive(Debug)]
pub struct TextEditor {
    content: String,
    cursor_position: usize,
    selection: Option<(usize, usize)>,
    modification_count: u32,
}

impl TextEditor {
    pub fn new() -> Self {
        TextEditor {
            content: String::new(),
            cursor_position: 0,
            selection: None,
            modification_count: 0,
        }
    }

    pub fn insert_text(&mut self, text: &str) {
        self.content.insert_str(self.cursor_position, text);
        self.cursor_position += text.len();
        self.selection = None;
        self.modification_count += 1;
    }

    pub fn delete_text(&mut self, length: usize) {
        if self.cursor_position >= length {
            let start = self.cursor_position - length;
            self.content.drain(start..self.cursor_position);
            self.cursor_position = start;
            self.selection = None;
            self.modification_count += 1;
        }
    }

    pub fn set_cursor_position(&mut self, position: usize) {
        self.cursor_position = position.min(self.content.len());
        self.selection = None;
    }

    pub fn select_text(&mut self, start: usize, end: usize) {
        let start = start.min(self.content.len());
        let end = end.min(self.content.len());
        if start <= end {
            self.selection = Some((start, end));
            self.cursor_position = end;
        }
    }

    pub fn get_content(&self) -> &str {
        &self.content
    }

    pub fn get_cursor_position(&self) -> usize {
        self.cursor_position
    }

    pub fn get_selection(&self) -> Option<(usize, usize)> {
        self.selection
    }

    pub fn get_modification_count(&self) -> u32 {
        self.modification_count
    }

    // Create memento
    pub fn create_memento(&self) -> TextEditorMemento {
        TextEditorMemento::new(
            self.content.clone(),
            self.cursor_position,
            self.selection,
        )
    }

    // Restore from memento
    pub fn restore_from_memento(&mut self, memento: &TextEditorMemento) {
        self.content = memento.content.clone();
        self.cursor_position = memento.cursor_position;
        self.selection = memento.selection;
        self.modification_count += 1;
    }

    pub fn get_status(&self) -> String {
        format!(
            "Content: '{}' | Cursor: {} | Selection: {:?} | Modifications: {}",
            self.content, self.cursor_position, self.selection, self.modification_count
        )
    }
}

impl Default for TextEditor {
    fn default() -> Self {
        Self::new()
    }
}

/// Caretaker for managing mementos
pub struct EditorHistory {
    mementos: Vec<TextEditorMemento>,
    current_index: Option<usize>,
    max_history: usize,
}

impl EditorHistory {
    pub fn new(max_history: usize) -> Self {
        EditorHistory {
            mementos: Vec::new(),
            current_index: None,
            max_history,
        }
    }

    pub fn save_state(&mut self, memento: TextEditorMemento) {
        // Remove any mementos after current index (when we're in the middle of history)
        if let Some(current) = self.current_index {
            self.mementos.truncate(current + 1);
        }

        // Add new memento
        self.mementos.push(memento);

        // Maintain max history size
        if self.mementos.len() > self.max_history {
            self.mementos.remove(0);
        } else {
            self.current_index = Some(self.mementos.len() - 1);
        }
    }

    pub fn undo(&mut self) -> Option<&TextEditorMemento> {
        if let Some(current) = self.current_index {
            if current > 0 {
                self.current_index = Some(current - 1);
                return self.mementos.get(current - 1);
            }
        }
        None
    }

    pub fn redo(&mut self) -> Option<&TextEditorMemento> {
        if let Some(current) = self.current_index {
            if current + 1 < self.mementos.len() {
                self.current_index = Some(current + 1);
                return self.mementos.get(current + 1);
            }
        }
        None
    }

    pub fn can_undo(&self) -> bool {
        self.current_index.map_or(false, |i| i > 0)
    }

    pub fn can_redo(&self) -> bool {
        self.current_index.map_or(false, |i| i + 1 < self.mementos.len())
    }

    pub fn get_history_size(&self) -> usize {
        self.mementos.len()
    }

    pub fn get_current_index(&self) -> Option<usize> {
        self.current_index
    }

    pub fn get_history_summary(&self) -> Vec<String> {
        self.mementos
            .iter()
            .enumerate()
            .map(|(i, memento)| {
                let marker = if Some(i) == self.current_index { ">" } else { " " };
                format!("{} {}: '{}' ({})", marker, i, memento.content, memento.timestamp)
            })
            .collect()
    }
}

impl Default for EditorHistory {
    fn default() -> Self {
        Self::new(10)
    }
}

/// Game state memento example
#[derive(Debug, Clone)]
pub struct GameStateMemento {
    player_position: (f32, f32),
    health: u32,
    score: u32,
    level: u32,
    inventory: HashMap<String, u32>,
    timestamp: u64,
    checkpoint_name: String,
}

impl GameStateMemento {
    pub fn new(
        player_position: (f32, f32),
        health: u32,
        score: u32,
        level: u32,
        inventory: HashMap<String, u32>,
        checkpoint_name: String,
    ) -> Self {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();

        GameStateMemento {
            player_position,
            health,
            score,
            level,
            inventory,
            timestamp,
            checkpoint_name,
        }
    }

    pub fn get_checkpoint_name(&self) -> &str {
        &self.checkpoint_name
    }

    pub fn get_timestamp(&self) -> u64 {
        self.timestamp
    }
}

#[derive(Debug)]
pub struct GameState {
    player_position: (f32, f32),
    health: u32,
    max_health: u32,
    score: u32,
    level: u32,
    inventory: HashMap<String, u32>,
    game_time: f32,
}

impl GameState {
    pub fn new() -> Self {
        let mut inventory = HashMap::new();
        inventory.insert("coins".to_string(), 0);
        inventory.insert("health_potions".to_string(), 0);
        inventory.insert("keys".to_string(), 0);

        GameState {
            player_position: (0.0, 0.0),
            health: 100,
            max_health: 100,
            score: 0,
            level: 1,
            inventory,
            game_time: 0.0,
        }
    }

    pub fn move_player(&mut self, x: f32, y: f32) {
        self.player_position = (x, y);
    }

    pub fn take_damage(&mut self, damage: u32) {
        self.health = self.health.saturating_sub(damage);
    }

    pub fn heal(&mut self, amount: u32) {
        self.health = (self.health + amount).min(self.max_health);
    }

    pub fn add_score(&mut self, points: u32) {
        self.score += points;
    }

    pub fn level_up(&mut self) {
        self.level += 1;
        self.max_health += 10;
        self.health = self.max_health;
    }

    pub fn add_item(&mut self, item: &str, quantity: u32) {
        *self.inventory.entry(item.to_string()).or_insert(0) += quantity;
    }

    pub fn use_item(&mut self, item: &str, quantity: u32) -> bool {
        if let Some(current) = self.inventory.get_mut(item) {
            if *current >= quantity {
                *current -= quantity;
                return true;
            }
        }
        false
    }

    pub fn update_game_time(&mut self, delta: f32) {
        self.game_time += delta;
    }

    // Getters
    pub fn get_position(&self) -> (f32, f32) {
        self.player_position
    }

    pub fn get_health(&self) -> u32 {
        self.health
    }

    pub fn get_score(&self) -> u32 {
        self.score
    }

    pub fn get_level(&self) -> u32 {
        self.level
    }

    pub fn get_inventory(&self) -> &HashMap<String, u32> {
        &self.inventory
    }

    pub fn get_game_time(&self) -> f32 {
        self.game_time
    }

    // Create memento
    pub fn create_checkpoint(&self, checkpoint_name: &str) -> GameStateMemento {
        GameStateMemento::new(
            self.player_position,
            self.health,
            self.score,
            self.level,
            self.inventory.clone(),
            checkpoint_name.to_string(),
        )
    }

    // Restore from memento
    pub fn load_checkpoint(&mut self, memento: &GameStateMemento) {
        self.player_position = memento.player_position;
        self.health = memento.health;
        self.score = memento.score;
        self.level = memento.level;
        self.inventory = memento.inventory.clone();
    }

    pub fn get_status(&self) -> String {
        format!(
            "Level {} | Position: ({:.1}, {:.1}) | Health: {}/{} | Score: {} | Time: {:.1}s",
            self.level, self.player_position.0, self.player_position.1,
            self.health, self.max_health, self.score, self.game_time
        )
    }
}

impl Default for GameState {
    fn default() -> Self {
        Self::new()
    }
}

/// Game save system
pub struct SaveSystem {
    checkpoints: HashMap<String, GameStateMemento>,
    auto_saves: Vec<GameStateMemento>,
    max_auto_saves: usize,
}

impl SaveSystem {
    pub fn new(max_auto_saves: usize) -> Self {
        SaveSystem {
            checkpoints: HashMap::new(),
            auto_saves: Vec::new(),
            max_auto_saves,
        }
    }

    pub fn save_checkpoint(&mut self, name: &str, memento: GameStateMemento) {
        self.checkpoints.insert(name.to_string(), memento);
    }

    pub fn load_checkpoint(&self, name: &str) -> Option<&GameStateMemento> {
        self.checkpoints.get(name)
    }

    pub fn auto_save(&mut self, memento: GameStateMemento) {
        self.auto_saves.push(memento);
        if self.auto_saves.len() > self.max_auto_saves {
            self.auto_saves.remove(0);
        }
    }

    pub fn get_latest_auto_save(&self) -> Option<&GameStateMemento> {
        self.auto_saves.last()
    }

    pub fn get_checkpoint_names(&self) -> Vec<String> {
        self.checkpoints.keys().cloned().collect()
    }

    pub fn get_auto_save_count(&self) -> usize {
        self.auto_saves.len()
    }

    pub fn delete_checkpoint(&mut self, name: &str) -> bool {
        self.checkpoints.remove(name).is_some()
    }

    pub fn get_save_summary(&self) -> SaveSummary {
        SaveSummary {
            checkpoint_count: self.checkpoints.len(),
            auto_save_count: self.auto_saves.len(),
            checkpoint_names: self.get_checkpoint_names(),
            latest_auto_save_time: self.auto_saves.last().map(|m| m.timestamp),
        }
    }
}

impl Default for SaveSystem {
    fn default() -> Self {
        Self::new(5)
    }
}

#[derive(Debug)]
pub struct SaveSummary {
    pub checkpoint_count: usize,
    pub auto_save_count: usize,
    pub checkpoint_names: Vec<String>,
    pub latest_auto_save_time: Option<u64>,
}

/// Configuration manager with memento
#[derive(Debug, Clone)]
pub struct ConfigurationMemento {
    settings: HashMap<String, String>,
    timestamp: u64,
    description: String,
}

impl ConfigurationMemento {
    pub fn new(settings: HashMap<String, String>, description: String) -> Self {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();

        ConfigurationMemento {
            settings,
            timestamp,
            description,
        }
    }

    pub fn get_description(&self) -> &str {
        &self.description
    }

    pub fn get_timestamp(&self) -> u64 {
        self.timestamp
    }
}

pub struct ConfigurationManager {
    settings: HashMap<String, String>,
    change_count: u32,
}

impl ConfigurationManager {
    pub fn new() -> Self {
        ConfigurationManager {
            settings: HashMap::new(),
            change_count: 0,
        }
    }

    pub fn set_setting(&mut self, key: &str, value: &str) {
        self.settings.insert(key.to_string(), value.to_string());
        self.change_count += 1;
    }

    pub fn get_setting(&self, key: &str) -> Option<&String> {
        self.settings.get(key)
    }

    pub fn remove_setting(&mut self, key: &str) -> Option<String> {
        let result = self.settings.remove(key);
        if result.is_some() {
            self.change_count += 1;
        }
        result
    }

    pub fn get_all_settings(&self) -> &HashMap<String, String> {
        &self.settings
    }

    pub fn get_change_count(&self) -> u32 {
        self.change_count
    }

    // Create memento
    pub fn create_snapshot(&self, description: &str) -> ConfigurationMemento {
        ConfigurationMemento::new(self.settings.clone(), description.to_string())
    }

    // Restore from memento
    pub fn restore_snapshot(&mut self, memento: &ConfigurationMemento) {
        self.settings = memento.settings.clone();
        self.change_count += 1;
    }
}

impl Default for ConfigurationManager {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_text_editor_memento() {
        let mut editor = TextEditor::new();
        let mut history = EditorHistory::new(5);

        editor.insert_text("Hello");
        history.save_state(editor.create_memento());

        editor.insert_text(" World");
        history.save_state(editor.create_memento());

        assert_eq!(editor.get_content(), "Hello World");

        // Undo
        if let Some(memento) = history.undo() {
            editor.restore_from_memento(memento);
        }
        assert_eq!(editor.get_content(), "Hello");

        // Redo
        if let Some(memento) = history.redo() {
            editor.restore_from_memento(memento);
        }
        assert_eq!(editor.get_content(), "Hello World");
    }

    #[test]
    fn test_game_state_memento() {
        let mut game = GameState::new();
        let mut save_system = SaveSystem::new(3);

        game.move_player(10.0, 20.0);
        game.add_score(100);

        let checkpoint = game.create_checkpoint("start");
        save_system.save_checkpoint("start", checkpoint);

        game.take_damage(50);
        game.move_player(30.0, 40.0);

        // Load checkpoint
        if let Some(memento) = save_system.load_checkpoint("start") {
            game.load_checkpoint(memento);
        }

        assert_eq!(game.get_health(), 100);
        assert_eq!(game.get_position(), (10.0, 20.0));
    }

    #[test]
    fn test_configuration_memento() {
        let mut config = ConfigurationManager::new();

        config.set_setting("theme", "dark");
        config.set_setting("language", "en");

        let snapshot = config.create_snapshot("Initial setup");

        config.set_setting("theme", "light");
        config.remove_setting("language");

        assert_eq!(config.get_setting("theme"), Some(&"light".to_string()));
        assert_eq!(config.get_setting("language"), None);

        // Restore
        config.restore_snapshot(&snapshot);
        assert_eq!(config.get_setting("theme"), Some(&"dark".to_string()));
        assert_eq!(config.get_setting("language"), Some(&"en".to_string()));
    }
}

pub fn demo() {
    println!("=== Memento Pattern Demo ===");

    // Text Editor Example
    println!("\n1. Text Editor with Undo/Redo:");
    let mut editor = TextEditor::new();
    let mut history = EditorHistory::new(10);

    println!("Initial state: {}", editor.get_status());

    // Type some text
    editor.insert_text("Hello");
    history.save_state(editor.create_memento());
    println!("After 'Hello': {}", editor.get_status());

    editor.insert_text(" ");
    history.save_state(editor.create_memento());

    editor.insert_text("World");
    history.save_state(editor.create_memento());
    println!("After 'World': {}", editor.get_status());

    editor.set_cursor_position(5);
    editor.insert_text(" Beautiful");
    history.save_state(editor.create_memento());
    println!("After insertion: {}", editor.get_status());

    // Undo operations
    println!("\nUndo operations:");
    for i in 1..=3 {
        if history.can_undo() {
            if let Some(memento) = history.undo() {
                editor.restore_from_memento(memento);
                println!("  Undo {}: {}", i, editor.get_status());
            }
        }
    }

    // Redo operations
    println!("\nRedo operations:");
    for i in 1..=2 {
        if history.can_redo() {
            if let Some(memento) = history.redo() {
                editor.restore_from_memento(memento);
                println!("  Redo {}: {}", i, editor.get_status());
            }
        }
    }

    println!("\nHistory summary:");
    for summary in history.get_history_summary() {
        println!("  {}", summary);
    }

    // Game State Example
    println!("\n2. Game State Checkpoints:");
    let mut game = GameState::new();
    let mut save_system = SaveSystem::new(5);

    println!("Initial game state: {}", game.get_status());

    // Play the game
    game.move_player(10.0, 5.0);
    game.add_item("coins", 50);
    game.add_item("health_potions", 2);
    game.add_score(250);
    game.update_game_time(30.0);

    println!("After some gameplay: {}", game.get_status());

    // Save checkpoint
    let checkpoint1 = game.create_checkpoint("forest_entrance");
    save_system.save_checkpoint("forest_entrance", checkpoint1.clone());
    println!("Saved checkpoint: {}", checkpoint1.get_checkpoint_name());

    // Continue playing
    game.move_player(25.0, 15.0);
    game.take_damage(30);
    game.use_item("health_potions", 1);
    game.heal(20);
    game.add_score(150);
    game.level_up();
    game.update_game_time(45.0);

    println!("After more gameplay: {}", game.get_status());

    // Save another checkpoint
    let checkpoint2 = game.create_checkpoint("boss_defeated");
    save_system.save_checkpoint("boss_defeated", checkpoint2);

    // Auto-save
    save_system.auto_save(game.create_checkpoint("auto_save"));

    // Player dies, load checkpoint
    println!("\nPlayer died! Loading checkpoint...");
    if let Some(checkpoint) = save_system.load_checkpoint("forest_entrance") {
        game.load_checkpoint(checkpoint);
        println!("Loaded state: {}", game.get_status());
    }

    // Show save summary
    let summary = save_system.get_save_summary();
    println!("\nSave system summary:");
    println!("  Checkpoints: {}", summary.checkpoint_count);
    println!("  Auto-saves: {}", summary.auto_save_count);
    println!("  Available checkpoints: {:?}", summary.checkpoint_names);

    // Configuration Manager Example
    println!("\n3. Configuration Manager with Snapshots:");
    let mut config = ConfigurationManager::new();

    // Set initial configuration
    config.set_setting("theme", "dark");
    config.set_setting("language", "english");
    config.set_setting("auto_save", "true");
    config.set_setting("difficulty", "normal");

    println!("Initial config:");
    for (key, value) in config.get_all_settings() {
        println!("  {}: {}", key, value);
    }

    // Create snapshot
    let initial_snapshot = config.create_snapshot("Default settings");
    println!("\nCreated snapshot: {}", initial_snapshot.get_description());

    // Modify settings
    config.set_setting("theme", "light");
    config.set_setting("difficulty", "hard");
    config.remove_setting("auto_save");
    config.set_setting("sound_effects", "enabled");

    println!("\nModified config:");
    for (key, value) in config.get_all_settings() {
        println!("  {}: {}", key, value);
    }

    // Restore snapshot
    println!("\nRestoring to initial snapshot...");
    config.restore_snapshot(&initial_snapshot);

    println!("Restored config:");
    for (key, value) in config.get_all_settings() {
        println!("  {}: {}", key, value);
    }

    println!("Change count: {}", config.get_change_count());

    println!("\n4. Memento Pattern Benefits:");
    println!("  ✅ Preserves encapsulation boundaries");
    println!("  ✅ Simplifies originator by delegating state management");
    println!("  ✅ Enables undo/redo functionality");
    println!("  ✅ Supports checkpoint and restore operations");
    println!("  ✅ Can be used for rollback in transactions");
    println!("  ✅ Facilitates debugging and testing");
}

pub fn main() {
    demo();
}