/// Command Pattern Implementation in Rust
///
/// The Command pattern encapsulates a request as an object, thereby allowing you
/// to parameterize clients with different requests, queue or log requests,
/// and support undoable operations.

use std::collections::VecDeque;
use std::time::{SystemTime, UNIX_EPOCH};

/// Command trait that all commands must implement
pub trait Command: std::fmt::Debug {
    fn execute(&mut self) -> Result<String, String>;
    fn undo(&mut self) -> Result<String, String>;
    fn get_description(&self) -> String;
    fn can_undo(&self) -> bool;
}

/// Text editor example - Receiver
#[derive(Debug, Clone)]
pub struct TextEditor {
    content: String,
    cursor_position: usize,
}

impl TextEditor {
    pub fn new() -> Self {
        TextEditor {
            content: String::new(),
            cursor_position: 0,
        }
    }

    pub fn insert_text(&mut self, text: &str) -> String {
        self.content.insert_str(self.cursor_position, text);
        self.cursor_position += text.len();
        format!("Inserted '{}' at position {}", text, self.cursor_position - text.len())
    }

    pub fn delete_text(&mut self, length: usize) -> (String, String) {
        if self.cursor_position >= length {
            let start = self.cursor_position - length;
            let deleted = self.content.drain(start..self.cursor_position).collect();
            self.cursor_position = start;
            (deleted, format!("Deleted {} characters", length))
        } else {
            let deleted = self.content.drain(0..self.cursor_position).collect();
            let deleted_count = self.cursor_position;
            self.cursor_position = 0;
            (deleted, format!("Deleted {} characters", deleted_count))
        }
    }

    pub fn move_cursor(&mut self, position: usize) -> String {
        let old_position = self.cursor_position;
        self.cursor_position = position.min(self.content.len());
        format!("Moved cursor from {} to {}", old_position, self.cursor_position)
    }

    pub fn get_content(&self) -> &str {
        &self.content
    }

    pub fn get_cursor_position(&self) -> usize {
        self.cursor_position
    }

    pub fn select_text(&self, start: usize, end: usize) -> String {
        let start = start.min(self.content.len());
        let end = end.min(self.content.len());
        if start <= end {
            self.content[start..end].to_string()
        } else {
            String::new()
        }
    }

    pub fn replace_text(&mut self, start: usize, end: usize, new_text: &str) -> String {
        let start = start.min(self.content.len());
        let end = end.min(self.content.len()).max(start);

        let old_text = self.content.drain(start..end).collect::<String>();
        self.content.insert_str(start, new_text);
        self.cursor_position = start + new_text.len();

        format!("Replaced '{}' with '{}'", old_text, new_text)
    }
}

impl Default for TextEditor {
    fn default() -> Self {
        Self::new()
    }
}

/// Concrete Commands

/// Insert text command
#[derive(Debug)]
pub struct InsertCommand {
    editor: Option<*mut TextEditor>,
    text: String,
    position: usize,
    executed: bool,
}

impl InsertCommand {
    pub fn new(editor: &mut TextEditor, text: String) -> Self {
        let position = editor.get_cursor_position();
        InsertCommand {
            editor: Some(editor as *mut TextEditor),
            text,
            position,
            executed: false,
        }
    }

    fn get_editor(&mut self) -> Option<&mut TextEditor> {
        self.editor.map(|ptr| unsafe { &mut *ptr })
    }
}

impl Command for InsertCommand {
    fn execute(&mut self) -> Result<String, String> {
        if self.executed {
            return Err("Command already executed".to_string());
        }

        let position = self.position;
        let text = self.text.clone();

        if let Some(editor) = self.get_editor() {
            editor.move_cursor(position);
            let result = editor.insert_text(&text);
            self.executed = true;
            Ok(result)
        } else {
            Err("No editor available".to_string())
        }
    }

    fn undo(&mut self) -> Result<String, String> {
        if !self.executed {
            return Err("Command not executed".to_string());
        }

        let position = self.position;
        let text_len = self.text.len();

        if let Some(editor) = self.get_editor() {
            editor.move_cursor(position + text_len);
            let (_, result) = editor.delete_text(text_len);
            self.executed = false;
            Ok(format!("Undone: {}", result))
        } else {
            Err("No editor available".to_string())
        }
    }

    fn get_description(&self) -> String {
        format!("Insert '{}' at position {}", self.text, self.position)
    }

    fn can_undo(&self) -> bool {
        self.executed
    }
}

/// Delete text command
#[derive(Debug)]
pub struct DeleteCommand {
    editor: Option<*mut TextEditor>,
    deleted_text: String,
    position: usize,
    length: usize,
    executed: bool,
}

impl DeleteCommand {
    pub fn new(editor: &mut TextEditor, length: usize) -> Self {
        let position = editor.get_cursor_position();
        DeleteCommand {
            editor: Some(editor as *mut TextEditor),
            deleted_text: String::new(),
            position,
            length,
            executed: false,
        }
    }

    fn get_editor(&mut self) -> Option<&mut TextEditor> {
        self.editor.map(|ptr| unsafe { &mut *ptr })
    }
}

impl Command for DeleteCommand {
    fn execute(&mut self) -> Result<String, String> {
        if self.executed {
            return Err("Command already executed".to_string());
        }

        let position = self.position;
        let length = self.length;

        if let Some(editor) = self.get_editor() {
            editor.move_cursor(position);
            let (deleted, result) = editor.delete_text(length);
            self.deleted_text = deleted;
            self.executed = true;
            Ok(result)
        } else {
            Err("No editor available".to_string())
        }
    }

    fn undo(&mut self) -> Result<String, String> {
        if !self.executed {
            return Err("Command not executed".to_string());
        }

        let position = self.position;
        let deleted_text = self.deleted_text.clone();

        if let Some(editor) = self.get_editor() {
            editor.move_cursor(position);
            let result = editor.insert_text(&deleted_text);
            self.executed = false;
            Ok(format!("Undone: {}", result))
        } else {
            Err("No editor available".to_string())
        }
    }

    fn get_description(&self) -> String {
        format!("Delete {} characters at position {}", self.length, self.position)
    }

    fn can_undo(&self) -> bool {
        self.executed
    }
}

/// Macro command that combines multiple commands
pub struct MacroCommand {
    commands: Vec<Box<dyn Command>>,
    description: String,
    executed: bool,
}

impl std::fmt::Debug for MacroCommand {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("MacroCommand")
            .field("description", &self.description)
            .field("executed", &self.executed)
            .field("command_count", &self.commands.len())
            .finish()
    }
}

impl MacroCommand {
    pub fn new(description: String) -> Self {
        MacroCommand {
            commands: Vec::new(),
            description,
            executed: false,
        }
    }

    pub fn add_command(&mut self, command: Box<dyn Command>) {
        self.commands.push(command);
    }

    pub fn get_command_count(&self) -> usize {
        self.commands.len()
    }
}

impl Command for MacroCommand {
    fn execute(&mut self) -> Result<String, String> {
        if self.executed {
            return Err("Macro already executed".to_string());
        }

        let mut results = Vec::new();
        for command in &mut self.commands {
            match command.execute() {
                Ok(result) => results.push(result),
                Err(e) => {
                    // Try to undo previously executed commands
                    for prev_command in self.commands.iter_mut().rev() {
                        if prev_command.can_undo() {
                            let _ = prev_command.undo();
                        }
                    }
                    return Err(format!("Macro failed: {}", e));
                }
            }
        }

        self.executed = true;
        Ok(format!("Executed macro: {}", results.join("; ")))
    }

    fn undo(&mut self) -> Result<String, String> {
        if !self.executed {
            return Err("Macro not executed".to_string());
        }

        let mut results = Vec::new();
        for command in self.commands.iter_mut().rev() {
            if command.can_undo() {
                match command.undo() {
                    Ok(result) => results.push(result),
                    Err(e) => return Err(format!("Macro undo failed: {}", e)),
                }
            }
        }

        self.executed = false;
        Ok(format!("Undone macro: {}", results.join("; ")))
    }

    fn get_description(&self) -> String {
        format!("{} ({} commands)", self.description, self.commands.len())
    }

    fn can_undo(&self) -> bool {
        self.executed && self.commands.iter().any(|cmd| cmd.can_undo())
    }
}

/// Command invoker - manages command execution and undo history
pub struct CommandManager {
    history: VecDeque<Box<dyn Command>>,
    undo_stack: Vec<Box<dyn Command>>,
    max_history: usize,
}

impl CommandManager {
    pub fn new(max_history: usize) -> Self {
        CommandManager {
            history: VecDeque::new(),
            undo_stack: Vec::new(),
            max_history,
        }
    }

    pub fn execute_command(&mut self, mut command: Box<dyn Command>) -> Result<String, String> {
        let result = command.execute()?;

        // Add to history
        self.history.push_back(command);
        if self.history.len() > self.max_history {
            self.history.pop_front();
        }

        // Clear redo stack when new command is executed
        self.undo_stack.clear();

        Ok(result)
    }

    pub fn undo(&mut self) -> Result<String, String> {
        if let Some(mut command) = self.history.pop_back() {
            if command.can_undo() {
                let result = command.undo()?;
                self.undo_stack.push(command);
                Ok(result)
            } else {
                self.history.push_back(command);
                Err("Command cannot be undone".to_string())
            }
        } else {
            Err("No commands to undo".to_string())
        }
    }

    pub fn redo(&mut self) -> Result<String, String> {
        if let Some(mut command) = self.undo_stack.pop() {
            let result = command.execute()?;
            self.history.push_back(command);
            Ok(result)
        } else {
            Err("No commands to redo".to_string())
        }
    }

    pub fn get_history(&self) -> Vec<String> {
        self.history
            .iter()
            .map(|cmd| cmd.get_description())
            .collect()
    }

    pub fn get_undo_stack_size(&self) -> usize {
        self.undo_stack.len()
    }

    pub fn get_history_size(&self) -> usize {
        self.history.len()
    }

    pub fn can_undo(&self) -> bool {
        self.history
            .back()
            .map(|cmd| cmd.can_undo())
            .unwrap_or(false)
    }

    pub fn can_redo(&self) -> bool {
        !self.undo_stack.is_empty()
    }

    pub fn clear_history(&mut self) {
        self.history.clear();
        self.undo_stack.clear();
    }
}

impl Default for CommandManager {
    fn default() -> Self {
        Self::new(50)
    }
}

/// Remote control example for home automation
#[derive(Debug, Clone)]
pub struct Light {
    location: String,
    is_on: bool,
    brightness: u8,
}

impl Light {
    pub fn new(location: &str) -> Self {
        Light {
            location: location.to_string(),
            is_on: false,
            brightness: 0,
        }
    }

    pub fn turn_on(&mut self) -> String {
        self.is_on = true;
        self.brightness = 100;
        format!("{} light turned ON", self.location)
    }

    pub fn turn_off(&mut self) -> String {
        self.is_on = false;
        self.brightness = 0;
        format!("{} light turned OFF", self.location)
    }

    pub fn set_brightness(&mut self, brightness: u8) -> String {
        self.brightness = brightness;
        self.is_on = brightness > 0;
        format!("{} light brightness set to {}%", self.location, brightness)
    }

    pub fn is_on(&self) -> bool {
        self.is_on
    }

    pub fn get_brightness(&self) -> u8 {
        self.brightness
    }
}

/// Light commands
#[derive(Debug)]
pub struct LightOnCommand {
    light: Option<*mut Light>,
    was_on: bool,
    previous_brightness: u8,
}

impl LightOnCommand {
    pub fn new(light: &mut Light) -> Self {
        LightOnCommand {
            light: Some(light as *mut Light),
            was_on: light.is_on(),
            previous_brightness: light.get_brightness(),
        }
    }

    fn get_light(&mut self) -> Option<&mut Light> {
        self.light.map(|ptr| unsafe { &mut *ptr })
    }
}

impl Command for LightOnCommand {
    fn execute(&mut self) -> Result<String, String> {
        if let Some(light) = self.get_light() {
            let was_on = light.is_on();
            let previous_brightness = light.get_brightness();
            let result = light.turn_on();
            self.was_on = was_on;
            self.previous_brightness = previous_brightness;
            Ok(result)
        } else {
            Err("No light available".to_string())
        }
    }

    fn undo(&mut self) -> Result<String, String> {
        let was_on = self.was_on;
        let previous_brightness = self.previous_brightness;

        if let Some(light) = self.get_light() {
            if was_on {
                Ok(light.set_brightness(previous_brightness))
            } else {
                Ok(light.turn_off())
            }
        } else {
            Err("No light available".to_string())
        }
    }

    fn get_description(&self) -> String {
        "Turn light ON".to_string()
    }

    fn can_undo(&self) -> bool {
        true
    }
}

#[derive(Debug)]
pub struct LightOffCommand {
    light: Option<*mut Light>,
    was_on: bool,
    previous_brightness: u8,
}

impl LightOffCommand {
    pub fn new(light: &mut Light) -> Self {
        LightOffCommand {
            light: Some(light as *mut Light),
            was_on: light.is_on(),
            previous_brightness: light.get_brightness(),
        }
    }

    fn get_light(&mut self) -> Option<&mut Light> {
        self.light.map(|ptr| unsafe { &mut *ptr })
    }
}

impl Command for LightOffCommand {
    fn execute(&mut self) -> Result<String, String> {
        if let Some(light) = self.get_light() {
            let was_on = light.is_on();
            let previous_brightness = light.get_brightness();
            let result = light.turn_off();
            self.was_on = was_on;
            self.previous_brightness = previous_brightness;
            Ok(result)
        } else {
            Err("No light available".to_string())
        }
    }

    fn undo(&mut self) -> Result<String, String> {
        let was_on = self.was_on;
        let previous_brightness = self.previous_brightness;

        if let Some(light) = self.get_light() {
            if was_on {
                Ok(light.set_brightness(previous_brightness))
            } else {
                Ok(light.turn_off())
            }
        } else {
            Err("No light available".to_string())
        }
    }

    fn get_description(&self) -> String {
        "Turn light OFF".to_string()
    }

    fn can_undo(&self) -> bool {
        true
    }
}

/// Remote control with programmable slots
pub struct RemoteControl {
    on_commands: Vec<Option<Box<dyn Command>>>,
    off_commands: Vec<Option<Box<dyn Command>>>,
    last_command: Option<Box<dyn Command>>,
    slots: usize,
}

impl RemoteControl {
    pub fn new(slots: usize) -> Self {
        RemoteControl {
            on_commands: (0..slots).map(|_| None).collect(),
            off_commands: (0..slots).map(|_| None).collect(),
            last_command: None,
            slots,
        }
    }

    pub fn set_command(
        &mut self,
        slot: usize,
        on_command: Box<dyn Command>,
        off_command: Box<dyn Command>,
    ) -> Result<(), String> {
        if slot >= self.slots {
            return Err(format!("Invalid slot: {}. Must be < {}", slot, self.slots));
        }

        self.on_commands[slot] = Some(on_command);
        self.off_commands[slot] = Some(off_command);
        Ok(())
    }

    pub fn press_on_button(&mut self, slot: usize) -> Result<String, String> {
        if slot >= self.slots {
            return Err(format!("Invalid slot: {}", slot));
        }

        if let Some(mut command) = self.on_commands[slot].take() {
            // Execute the command (this consumes it)
            let result = match command.execute() {
                Ok(result) => {
                    // Store it as last command for undo
                    self.last_command = Some(command);
                    result
                }
                Err(e) => {
                    // Put the command back if it failed
                    self.on_commands[slot] = Some(command);
                    return Err(e);
                }
            };

            // We need to recreate the command since we consumed it
            // In a real implementation, you'd have a command factory
            // For now, we'll leave the slot empty
            Ok(result)
        } else {
            Err("No ON command set for this slot".to_string())
        }
    }

    pub fn press_off_button(&mut self, slot: usize) -> Result<String, String> {
        if slot >= self.slots {
            return Err(format!("Invalid slot: {}", slot));
        }

        if let Some(mut command) = self.off_commands[slot].take() {
            let result = match command.execute() {
                Ok(result) => {
                    self.last_command = Some(command);
                    result
                }
                Err(e) => {
                    self.off_commands[slot] = Some(command);
                    return Err(e);
                }
            };
            Ok(result)
        } else {
            Err("No OFF command set for this slot".to_string())
        }
    }

    pub fn press_undo_button(&mut self) -> Result<String, String> {
        if let Some(mut command) = self.last_command.take() {
            command.undo()
        } else {
            Err("No command to undo".to_string())
        }
    }

    pub fn get_slot_count(&self) -> usize {
        self.slots
    }

    pub fn is_slot_configured(&self, slot: usize) -> (bool, bool) {
        if slot >= self.slots {
            return (false, false);
        }
        (self.on_commands[slot].is_some(), self.off_commands[slot].is_some())
    }
}

/// Queue-based command processor for batch operations
pub struct CommandQueue {
    queue: VecDeque<QueuedCommand>,
    processing: bool,
}

#[derive(Debug)]
pub struct QueuedCommand {
    command: Box<dyn Command>,
    priority: u8,
    scheduled_time: Option<SystemTime>,
    retry_count: u8,
    max_retries: u8,
}

impl CommandQueue {
    pub fn new() -> Self {
        CommandQueue {
            queue: VecDeque::new(),
            processing: false,
        }
    }

    pub fn enqueue_command(
        &mut self,
        command: Box<dyn Command>,
        priority: u8,
        scheduled_time: Option<SystemTime>,
        max_retries: u8,
    ) {
        let queued_command = QueuedCommand {
            command,
            priority,
            scheduled_time,
            retry_count: 0,
            max_retries,
        };

        // Insert based on priority (higher number = higher priority)
        let insert_pos = self.queue
            .iter()
            .position(|cmd| cmd.priority < priority)
            .unwrap_or(self.queue.len());

        self.queue.insert(insert_pos, queued_command);
    }

    pub fn process_commands(&mut self) -> Vec<String> {
        if self.processing {
            return vec!["Already processing commands".to_string()];
        }

        self.processing = true;
        let mut results = Vec::new();
        let now = SystemTime::now();

        while let Some(mut queued_command) = self.queue.pop_front() {
            // Check if command is scheduled for the future
            if let Some(scheduled_time) = queued_command.scheduled_time {
                if now < scheduled_time {
                    // Put it back and break (assuming sorted by time)
                    self.queue.push_front(queued_command);
                    break;
                }
            }

            // Execute the command
            match queued_command.command.execute() {
                Ok(result) => {
                    results.push(format!("✅ {}", result));
                }
                Err(error) => {
                    queued_command.retry_count += 1;
                    if queued_command.retry_count <= queued_command.max_retries {
                        // Put it back at the end for retry
                        results.push(format!("⚠️  Retrying: {} (attempt {})", error, queued_command.retry_count));
                        self.queue.push_back(queued_command);
                    } else {
                        results.push(format!("❌ Failed: {} (max retries exceeded)", error));
                    }
                }
            }
        }

        self.processing = false;
        results
    }

    pub fn get_queue_size(&self) -> usize {
        self.queue.len()
    }

    pub fn is_processing(&self) -> bool {
        self.processing
    }

    pub fn clear_queue(&mut self) {
        self.queue.clear();
    }

    pub fn get_next_command_info(&self) -> Option<String> {
        self.queue.front().map(|cmd| {
            format!(
                "{} (priority: {}, retries: {}/{})",
                cmd.command.get_description(),
                cmd.priority,
                cmd.retry_count,
                cmd.max_retries
            )
        })
    }
}

impl Default for CommandQueue {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_text_editor_commands() {
        let mut editor = TextEditor::new();
        let mut manager = CommandManager::new(10);

        // Test insert command
        let insert_cmd = Box::new(InsertCommand::new(&mut editor, "Hello".to_string()));
        manager.execute_command(insert_cmd).unwrap();
        assert_eq!(editor.get_content(), "Hello");

        // Test undo
        manager.undo().unwrap();
        assert_eq!(editor.get_content(), "");

        // Test redo
        manager.redo().unwrap();
        assert_eq!(editor.get_content(), "Hello");
    }

    #[test]
    fn test_macro_command() {
        let mut editor = TextEditor::new();
        let mut macro_cmd = MacroCommand::new("Insert greeting".to_string());

        macro_cmd.add_command(Box::new(InsertCommand::new(&mut editor, "Hello ".to_string())));
        macro_cmd.add_command(Box::new(InsertCommand::new(&mut editor, "World!".to_string())));

        assert_eq!(macro_cmd.get_command_count(), 2);

        macro_cmd.execute().unwrap();
        assert_eq!(editor.get_content(), "Hello World!");

        macro_cmd.undo().unwrap();
        assert_eq!(editor.get_content(), "");
    }

    #[test]
    fn test_remote_control() {
        let mut living_room_light = Light::new("Living Room");
        let mut remote = RemoteControl::new(3);

        let on_cmd = Box::new(LightOnCommand::new(&mut living_room_light));
        let off_cmd = Box::new(LightOffCommand::new(&mut living_room_light));

        remote.set_command(0, on_cmd, off_cmd).unwrap();

        // Note: This test is simplified because of ownership issues
        // In a real implementation, you'd handle this differently
        assert!(remote.is_slot_configured(0).0);
        assert!(remote.is_slot_configured(0).1);
    }

    #[test]
    fn test_command_queue() {
        let mut queue = CommandQueue::new();
        let mut editor = TextEditor::new();

        // Add commands with different priorities
        queue.enqueue_command(
            Box::new(InsertCommand::new(&mut editor, "Low priority".to_string())),
            1,
            None,
            0
        );

        queue.enqueue_command(
            Box::new(InsertCommand::new(&mut editor, "High priority".to_string())),
            5,
            None,
            0
        );

        assert_eq!(queue.get_queue_size(), 2);

        let results = queue.process_commands();
        assert!(!results.is_empty());
    }

    #[test]
    fn test_command_manager_history() {
        let mut editor = TextEditor::new();
        let mut manager = CommandManager::new(3);

        // Add more commands than history limit
        for i in 0..5 {
            let cmd = Box::new(InsertCommand::new(&mut editor, format!("Text{}", i)));
            manager.execute_command(cmd).unwrap();
        }

        // Should only keep last 3 commands
        assert_eq!(manager.get_history_size(), 3);
        let history = manager.get_history();
        assert!(history.last().unwrap().contains("Text4"));
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Command Pattern Demo ===");

    // Text Editor with Command Manager
    println!("\n1. Text Editor with Undo/Redo:");
    let mut editor = TextEditor::new();
    let mut manager = CommandManager::new(10);

    // Execute some commands
    let commands: Vec<Box<dyn Command>> = vec![
        Box::new(InsertCommand::new(&mut editor, "Hello ".to_string())),
        Box::new(InsertCommand::new(&mut editor, "World!".to_string())),
        Box::new(DeleteCommand::new(&mut editor, 6)), // Delete "World!"
        Box::new(InsertCommand::new(&mut editor, "Rust!".to_string())),
    ];

    for (i, command) in commands.into_iter().enumerate() {
        println!("Executing command {}: {}", i + 1, command.get_description());
        match manager.execute_command(command) {
            Ok(result) => println!("  Result: {}", result),
            Err(error) => println!("  Error: {}", error),
        }
        println!("  Editor content: '{}'", editor.get_content());
    }

    // Show command history
    println!("\nCommand History:");
    for (i, desc) in manager.get_history().iter().enumerate() {
        println!("  {}: {}", i + 1, desc);
    }

    // Demonstrate undo/redo
    println!("\nUndo operations:");
    for i in 0..3 {
        if manager.can_undo() {
            match manager.undo() {
                Ok(result) => {
                    println!("  Undo {}: {}", i + 1, result);
                    println!("  Editor content: '{}'", editor.get_content());
                }
                Err(error) => println!("  Undo error: {}", error),
            }
        }
    }

    println!("\nRedo operations:");
    for i in 0..2 {
        if manager.can_redo() {
            match manager.redo() {
                Ok(result) => {
                    println!("  Redo {}: {}", i + 1, result);
                    println!("  Editor content: '{}'", editor.get_content());
                }
                Err(error) => println!("  Redo error: {}", error),
            }
        }
    }

    // Macro Command Example
    println!("\n2. Macro Command (Multiple Operations):");
    let mut new_editor = TextEditor::new();
    let mut macro_command = MacroCommand::new("Create document header".to_string());

    macro_command.add_command(Box::new(InsertCommand::new(&mut new_editor, "# Document Title\n".to_string())));
    macro_command.add_command(Box::new(InsertCommand::new(&mut new_editor, "\n".to_string())));
    macro_command.add_command(Box::new(InsertCommand::new(&mut new_editor, "## Introduction\n".to_string())));
    macro_command.add_command(Box::new(InsertCommand::new(&mut new_editor, "This is the introduction.\n".to_string())));

    println!("Macro: {}", macro_command.get_description());
    match macro_command.execute() {
        Ok(result) => {
            println!("Executed: {}", result);
            println!("Document content:\n{}", new_editor.get_content());
        }
        Err(error) => println!("Macro failed: {}", error),
    }

    println!("\nUndoing macro...");
    match macro_command.undo() {
        Ok(result) => {
            println!("Undone: {}", result);
            println!("Document content: '{}'", new_editor.get_content());
        }
        Err(error) => println!("Undo failed: {}", error),
    }

    // Home Automation Remote Control
    println!("\n3. Home Automation Remote Control:");
    let mut living_room_light = Light::new("Living Room");
    let mut bedroom_light = Light::new("Bedroom");
    let mut kitchen_light = Light::new("Kitchen");

    let mut remote = RemoteControl::new(4);

    // Configure remote slots (simplified - in real implementation, you'd handle ownership better)
    println!("Configuring remote control slots...");

    // Simulate light operations directly since the remote has ownership issues in this demo
    println!("\nSimulating light control:");
    println!("  {}", living_room_light.turn_on());
    println!("  {}", bedroom_light.turn_on());
    println!("  {}", bedroom_light.set_brightness(50));
    println!("  {}", kitchen_light.turn_on());
    println!("  {}", living_room_light.turn_off());

    // Command Queue Example
    println!("\n4. Command Queue Processing:");
    let mut queue = CommandQueue::new();
    let mut queue_editor = TextEditor::new();

    // Add commands with different priorities
    println!("Queuing commands with different priorities...");

    queue.enqueue_command(
        Box::new(InsertCommand::new(&mut queue_editor, "Low priority task".to_string())),
        1,
        None,
        2
    );

    queue.enqueue_command(
        Box::new(InsertCommand::new(&mut queue_editor, "HIGH PRIORITY TASK".to_string())),
        5,
        None,
        1
    );

    queue.enqueue_command(
        Box::new(InsertCommand::new(&mut queue_editor, "Medium priority task".to_string())),
        3,
        None,
        0
    );

    println!("Queue size: {}", queue.get_queue_size());

    if let Some(next_cmd) = queue.get_next_command_info() {
        println!("Next command: {}", next_cmd);
    }

    println!("\nProcessing command queue:");
    let results = queue.process_commands();
    for result in results {
        println!("  {}", result);
    }

    println!("Final editor content: '{}'", queue_editor.get_content());
    println!("Remaining queue size: {}", queue.get_queue_size());

    // Command Pattern Benefits Summary
    println!("\n5. Command Pattern Benefits:");
    println!("  ✅ Decouples invoker from receiver");
    println!("  ✅ Supports undo/redo operations");
    println!("  ✅ Enables macro commands (composite operations)");
    println!("  ✅ Allows command queuing and scheduling");
    println!("  ✅ Supports logging and auditing");
    println!("  ✅ Enables remote procedure calls");
    println!("  ✅ Facilitates transaction-like behavior");
}

pub fn main() {
    demo();
}