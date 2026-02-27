/// Mediator Pattern Implementation in Rust
///
/// The Mediator pattern defines how a set of objects interact with each other.
/// It promotes loose coupling by keeping objects from referring to each other
/// explicitly and letting you vary their interaction independently.

use std::collections::HashMap;
use std::rc::{Rc, Weak};
use std::cell::RefCell;

/// Chat room mediator example
pub trait ChatMediator {
    fn send_message(&self, message: &str, sender: &str, recipient: Option<&str>);
    fn add_user(&mut self, user: Rc<RefCell<ChatUser>>);
    fn remove_user(&mut self, username: &str);
    fn get_user_list(&self) -> Vec<String>;
}

#[derive(Debug)]
pub struct ChatUser {
    username: String,
    mediator: Option<Weak<RefCell<dyn ChatMediator>>>,
    message_history: Vec<String>,
}

impl ChatUser {
    pub fn new(username: &str) -> Self {
        ChatUser {
            username: username.to_string(),
            mediator: None,
            message_history: Vec::new(),
        }
    }

    pub fn set_mediator(&mut self, mediator: Weak<RefCell<dyn ChatMediator>>) {
        self.mediator = Some(mediator);
    }

    pub fn send_message(&self, message: &str, recipient: Option<&str>) {
        if let Some(mediator_weak) = &self.mediator {
            if let Some(mediator) = mediator_weak.upgrade() {
                mediator.borrow().send_message(message, &self.username, recipient);
            }
        }
    }

    pub fn receive_message(&mut self, message: &str, sender: &str) {
        let formatted_message = format!("{}: {}", sender, message);
        self.message_history.push(formatted_message.clone());
        println!("[{}] Received: {}", self.username, formatted_message);
    }

    pub fn get_username(&self) -> &str {
        &self.username
    }

    pub fn get_message_history(&self) -> &[String] {
        &self.message_history
    }
}

pub struct ChatRoom {
    users: HashMap<String, Rc<RefCell<ChatUser>>>,
    message_log: Vec<String>,
}

impl ChatRoom {
    pub fn new() -> Self {
        ChatRoom {
            users: HashMap::new(),
            message_log: Vec::new(),
        }
    }

    pub fn get_message_log(&self) -> &[String] {
        &self.message_log
    }
}

impl Default for ChatRoom {
    fn default() -> Self {
        Self::new()
    }
}

impl ChatMediator for ChatRoom {
    fn send_message(&self, message: &str, sender: &str, recipient: Option<&str>) {
        let log_entry = if let Some(recipient) = recipient {
            format!("[Private] {} -> {}: {}", sender, recipient, message)
        } else {
            format!("[Public] {}: {}", sender, message)
        };

        println!("{}", log_entry);

        if let Some(recipient_name) = recipient {
            // Private message
            if let Some(recipient_user) = self.users.get(recipient_name) {
                recipient_user.borrow_mut().receive_message(message, sender);
            }
        } else {
            // Broadcast to all users except sender
            for (username, user) in &self.users {
                if username != sender {
                    user.borrow_mut().receive_message(message, sender);
                }
            }
        }
    }

    fn add_user(&mut self, user: Rc<RefCell<ChatUser>>) {
        let username = user.borrow().get_username().to_string();
        self.users.insert(username.clone(), user);
        println!("{} joined the chat room", username);
    }

    fn remove_user(&mut self, username: &str) {
        if self.users.remove(username).is_some() {
            println!("{} left the chat room", username);
        }
    }

    fn get_user_list(&self) -> Vec<String> {
        self.users.keys().cloned().collect()
    }
}

/// Air Traffic Control System
pub trait AirTrafficMediator {
    fn request_landing(&self, aircraft_id: &str) -> Result<String, String>;
    fn request_takeoff(&self, aircraft_id: &str) -> Result<String, String>;
    fn register_aircraft(&mut self, aircraft: Rc<RefCell<Aircraft>>);
    fn get_runway_status(&self) -> RunwayStatus;
}

#[derive(Debug, Clone)]
pub struct Aircraft {
    id: String,
    aircraft_type: String,
    status: AircraftStatus,
    mediator: Option<Weak<RefCell<dyn AirTrafficMediator>>>,
}

#[derive(Debug, Clone, PartialEq)]
pub enum AircraftStatus {
    InFlight,
    RequestingLanding,
    Landing,
    OnGround,
    RequestingTakeoff,
    TakingOff,
}

impl Aircraft {
    pub fn new(id: &str, aircraft_type: &str) -> Self {
        Aircraft {
            id: id.to_string(),
            aircraft_type: aircraft_type.to_string(),
            status: AircraftStatus::InFlight,
            mediator: None,
        }
    }

    pub fn set_mediator(&mut self, mediator: Weak<RefCell<dyn AirTrafficMediator>>) {
        self.mediator = Some(mediator);
    }

    pub fn request_landing(&mut self) -> Result<String, String> {
        if self.status != AircraftStatus::InFlight {
            return Err("Aircraft must be in flight to request landing".to_string());
        }

        self.status = AircraftStatus::RequestingLanding;

        if let Some(mediator_weak) = &self.mediator {
            if let Some(mediator) = mediator_weak.upgrade() {
                mediator.borrow().request_landing(&self.id)
            } else {
                Err("No air traffic control available".to_string())
            }
        } else {
            Err("No air traffic control assigned".to_string())
        }
    }

    pub fn request_takeoff(&mut self) -> Result<String, String> {
        if self.status != AircraftStatus::OnGround {
            return Err("Aircraft must be on ground to request takeoff".to_string());
        }

        self.status = AircraftStatus::RequestingTakeoff;

        if let Some(mediator_weak) = &self.mediator {
            if let Some(mediator) = mediator_weak.upgrade() {
                mediator.borrow().request_takeoff(&self.id)
            } else {
                Err("No air traffic control available".to_string())
            }
        } else {
            Err("No air traffic control assigned".to_string())
        }
    }

    pub fn get_id(&self) -> &str {
        &self.id
    }

    pub fn get_status(&self) -> &AircraftStatus {
        &self.status
    }

    pub fn set_status(&mut self, status: AircraftStatus) {
        self.status = status;
    }
}

#[derive(Debug, Clone)]
pub struct RunwayStatus {
    pub runway_available: bool,
    pub current_aircraft: Option<String>,
    pub waiting_for_landing: Vec<String>,
    pub waiting_for_takeoff: Vec<String>,
}

pub struct AirTrafficControl {
    aircraft: HashMap<String, Rc<RefCell<Aircraft>>>,
    runway_status: RunwayStatus,
    operation_log: Vec<String>,
}

impl AirTrafficControl {
    pub fn new() -> Self {
        AirTrafficControl {
            aircraft: HashMap::new(),
            runway_status: RunwayStatus {
                runway_available: true,
                current_aircraft: None,
                waiting_for_landing: Vec::new(),
                waiting_for_takeoff: Vec::new(),
            },
            operation_log: Vec::new(),
        }
    }

    fn log_operation(&mut self, operation: &str) {
        self.operation_log.push(operation.to_string());
        println!("ATC: {}", operation);
    }

    pub fn get_operation_log(&self) -> &[String] {
        &self.operation_log
    }

    fn process_next_operation(&mut self) {
        if self.runway_status.runway_available {
            // Priority: landing requests first
            if let Some(aircraft_id) = self.runway_status.waiting_for_landing.pop() {
                self.approve_landing(&aircraft_id);
            } else if let Some(aircraft_id) = self.runway_status.waiting_for_takeoff.pop() {
                self.approve_takeoff(&aircraft_id);
            }
        }
    }

    fn approve_landing(&mut self, aircraft_id: &str) {
        if let Some(aircraft) = self.aircraft.get(aircraft_id) {
            self.runway_status.runway_available = false;
            self.runway_status.current_aircraft = Some(aircraft_id.to_string());
            aircraft.borrow_mut().set_status(AircraftStatus::Landing);
            self.log_operation(&format!("Cleared {} for landing", aircraft_id));
        }
    }

    fn approve_takeoff(&mut self, aircraft_id: &str) {
        if let Some(aircraft) = self.aircraft.get(aircraft_id) {
            self.runway_status.runway_available = false;
            self.runway_status.current_aircraft = Some(aircraft_id.to_string());
            aircraft.borrow_mut().set_status(AircraftStatus::TakingOff);
            self.log_operation(&format!("Cleared {} for takeoff", aircraft_id));
        }
    }

    pub fn complete_operation(&mut self, aircraft_id: &str) -> Result<String, String> {
        if let Some(current) = &self.runway_status.current_aircraft {
            if current == aircraft_id {
                if let Some(aircraft) = self.aircraft.get(aircraft_id) {
                    let status = aircraft.borrow().get_status().clone();
                    match status {
                        AircraftStatus::Landing => {
                            aircraft.borrow_mut().set_status(AircraftStatus::OnGround);
                            self.log_operation(&format!("{} has landed", aircraft_id));
                        }
                        AircraftStatus::TakingOff => {
                            aircraft.borrow_mut().set_status(AircraftStatus::InFlight);
                            self.log_operation(&format!("{} has taken off", aircraft_id));
                        }
                        _ => return Err("Invalid operation".to_string()),
                    }

                    self.runway_status.runway_available = true;
                    self.runway_status.current_aircraft = None;
                    self.process_next_operation();
                    Ok("Operation completed".to_string())
                } else {
                    Err("Aircraft not found".to_string())
                }
            } else {
                Err("Aircraft not currently using runway".to_string())
            }
        } else {
            Err("No current runway operation".to_string())
        }
    }
}

impl Default for AirTrafficControl {
    fn default() -> Self {
        Self::new()
    }
}

impl AirTrafficMediator for AirTrafficControl {
    fn request_landing(&self, aircraft_id: &str) -> Result<String, String> {
        if self.runway_status.runway_available {
            Ok(format!("Cleared for immediate landing: {}", aircraft_id))
        } else {
            Ok(format!("Added to landing queue: {}", aircraft_id))
        }
    }

    fn request_takeoff(&self, aircraft_id: &str) -> Result<String, String> {
        if self.runway_status.runway_available {
            Ok(format!("Cleared for immediate takeoff: {}", aircraft_id))
        } else {
            Ok(format!("Added to takeoff queue: {}", aircraft_id))
        }
    }

    fn register_aircraft(&mut self, aircraft: Rc<RefCell<Aircraft>>) {
        let aircraft_id = aircraft.borrow().get_id().to_string();
        self.aircraft.insert(aircraft_id.clone(), aircraft);
        self.log_operation(&format!("Aircraft {} registered with ATC", aircraft_id));
    }

    fn get_runway_status(&self) -> RunwayStatus {
        self.runway_status.clone()
    }
}

/// GUI Mediator Example
pub trait DialogMediator {
    fn on_button_click(&self, button_name: &str);
    fn on_checkbox_change(&self, checkbox_name: &str, checked: bool);
    fn on_text_change(&self, field_name: &str, text: &str);
}

pub struct GuiComponent {
    name: String,
    enabled: bool,
    visible: bool,
    mediator: Option<Weak<RefCell<dyn DialogMediator>>>,
}

impl GuiComponent {
    pub fn new(name: &str) -> Self {
        GuiComponent {
            name: name.to_string(),
            enabled: true,
            visible: true,
            mediator: None,
        }
    }

    pub fn set_mediator(&mut self, mediator: Weak<RefCell<dyn DialogMediator>>) {
        self.mediator = Some(mediator);
    }

    pub fn set_enabled(&mut self, enabled: bool) {
        self.enabled = enabled;
        println!("Component '{}' {}", self.name, if enabled { "enabled" } else { "disabled" });
    }

    pub fn set_visible(&mut self, visible: bool) {
        self.visible = visible;
        println!("Component '{}' {}", self.name, if visible { "shown" } else { "hidden" });
    }

    pub fn is_enabled(&self) -> bool {
        self.enabled
    }

    pub fn is_visible(&self) -> bool {
        self.visible
    }

    pub fn get_name(&self) -> &str {
        &self.name
    }
}

pub struct FormDialog {
    components: HashMap<String, GuiComponent>,
    form_data: HashMap<String, String>,
}

impl FormDialog {
    pub fn new() -> Self {
        let mut dialog = FormDialog {
            components: HashMap::new(),
            form_data: HashMap::new(),
        };

        // Initialize components
        dialog.components.insert("username_field".to_string(), GuiComponent::new("username_field"));
        dialog.components.insert("password_field".to_string(), GuiComponent::new("password_field"));
        dialog.components.insert("remember_me".to_string(), GuiComponent::new("remember_me"));
        dialog.components.insert("login_button".to_string(), GuiComponent::new("login_button"));
        dialog.components.insert("guest_mode".to_string(), GuiComponent::new("guest_mode"));

        dialog
    }

    pub fn get_component(&mut self, name: &str) -> Option<&mut GuiComponent> {
        self.components.get_mut(name)
    }

    pub fn get_form_data(&self) -> &HashMap<String, String> {
        &self.form_data
    }

    fn validate_form(&self) -> bool {
        let empty_string = String::new();
        let username = self.form_data.get("username_field").unwrap_or(&empty_string);
        let password = self.form_data.get("password_field").unwrap_or(&empty_string);
        !username.is_empty() && !password.is_empty()
    }
}

impl Default for FormDialog {
    fn default() -> Self {
        Self::new()
    }
}

impl DialogMediator for FormDialog {
    fn on_button_click(&self, button_name: &str) {
        match button_name {
            "login_button" => {
                if self.validate_form() {
                    println!("Login successful!");
                } else {
                    println!("Please fill in all required fields");
                }
            }
            _ => println!("Button clicked: {}", button_name),
        }
    }

    fn on_checkbox_change(&self, checkbox_name: &str, checked: bool) {
        match checkbox_name {
            "guest_mode" => {
                if checked {
                    println!("Guest mode enabled - login fields disabled");
                } else {
                    println!("Guest mode disabled - login fields enabled");
                }
            }
            "remember_me" => {
                println!("Remember me: {}", if checked { "enabled" } else { "disabled" });
            }
            _ => println!("Checkbox '{}' changed: {}", checkbox_name, checked),
        }
    }

    fn on_text_change(&self, field_name: &str, text: &str) {
        println!("Text field '{}' changed to: '{}'", field_name, text);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_chat_mediator() {
        let chat_room = Rc::new(RefCell::new(ChatRoom::new()));

        let user1 = Rc::new(RefCell::new(ChatUser::new("Alice")));
        let user2 = Rc::new(RefCell::new(ChatUser::new("Bob")));

        user1.borrow_mut().set_mediator(Rc::downgrade(&chat_room));
        user2.borrow_mut().set_mediator(Rc::downgrade(&chat_room));

        chat_room.borrow_mut().add_user(user1.clone());
        chat_room.borrow_mut().add_user(user2.clone());

        user1.borrow().send_message("Hello everyone!", None);
        user2.borrow().send_message("Hi Alice!", Some("Alice"));

        assert_eq!(chat_room.borrow().get_user_list().len(), 2);
    }

    #[test]
    fn test_atc_mediator() {
        let atc = Rc::new(RefCell::new(AirTrafficControl::new()));
        let mut aircraft = Aircraft::new("FL123", "Boeing 737");

        aircraft.set_mediator(Rc::downgrade(&atc));
        let aircraft_rc = Rc::new(RefCell::new(aircraft));

        atc.borrow_mut().register_aircraft(aircraft_rc.clone());

        let result = aircraft_rc.borrow_mut().request_landing();
        assert!(result.is_ok());
    }
}

pub fn demo() {
    println!("=== Mediator Pattern Demo ===");

    // Chat Room Example
    println!("\n1. Chat Room Mediator:");
    let chat_room = Rc::new(RefCell::new(ChatRoom::new()));

    let alice = Rc::new(RefCell::new(ChatUser::new("Alice")));
    let bob = Rc::new(RefCell::new(ChatUser::new("Bob")));
    let charlie = Rc::new(RefCell::new(ChatUser::new("Charlie")));

    // Set mediator for all users
    let chat_room_trait: Rc<RefCell<dyn ChatMediator>> = chat_room.clone();
    alice.borrow_mut().set_mediator(Rc::downgrade(&chat_room_trait));
    bob.borrow_mut().set_mediator(Rc::downgrade(&chat_room_trait));
    charlie.borrow_mut().set_mediator(Rc::downgrade(&chat_room_trait));

    // Add users to chat room
    chat_room.borrow_mut().add_user(alice.clone());
    chat_room.borrow_mut().add_user(bob.clone());
    chat_room.borrow_mut().add_user(charlie.clone());

    println!("Users in chat: {:?}", chat_room.borrow().get_user_list());

    // Send messages
    println!("\nChat conversation:");
    alice.borrow().send_message("Hello everyone!", None);
    bob.borrow().send_message("Hey Alice! How are you?", None);
    charlie.borrow().send_message("Good morning!", None);
    alice.borrow().send_message("I'm doing great, thanks!", Some("Bob"));

    chat_room.borrow_mut().remove_user("Charlie");
    bob.borrow().send_message("Charlie left", None);

    // Air Traffic Control Example
    println!("\n2. Air Traffic Control Mediator:");
    let atc = Rc::new(RefCell::new(AirTrafficControl::new()));

    let mut flight1 = Aircraft::new("UA123", "Boeing 777");
    let mut flight2 = Aircraft::new("DL456", "Airbus A320");
    let mut flight3 = Aircraft::new("AA789", "Boeing 737");

    let atc_trait: Rc<RefCell<dyn AirTrafficMediator>> = atc.clone();
    flight1.set_mediator(Rc::downgrade(&atc_trait));
    flight2.set_mediator(Rc::downgrade(&atc_trait));
    flight3.set_mediator(Rc::downgrade(&atc_trait));

    let flight1_rc = Rc::new(RefCell::new(flight1));
    let flight2_rc = Rc::new(RefCell::new(flight2));
    let flight3_rc = Rc::new(RefCell::new(flight3));

    atc.borrow_mut().register_aircraft(flight1_rc.clone());
    atc.borrow_mut().register_aircraft(flight2_rc.clone());
    atc.borrow_mut().register_aircraft(flight3_rc.clone());

    println!("\nAircraft operations:");

    // Request landings
    match flight1_rc.borrow_mut().request_landing() {
        Ok(response) => println!("UA123: {}", response),
        Err(e) => println!("UA123 Error: {}", e),
    }

    match flight2_rc.borrow_mut().request_landing() {
        Ok(response) => println!("DL456: {}", response),
        Err(e) => println!("DL456 Error: {}", e),
    }

    // Complete landing
    atc.borrow_mut().complete_operation("UA123").unwrap();

    // Set aircraft on ground and request takeoff
    flight3_rc.borrow_mut().set_status(AircraftStatus::OnGround);
    match flight3_rc.borrow_mut().request_takeoff() {
        Ok(response) => println!("AA789: {}", response),
        Err(e) => println!("AA789 Error: {}", e),
    }

    let runway_status = atc.borrow().get_runway_status();
    println!("\nRunway status:");
    println!("  Available: {}", runway_status.runway_available);
    println!("  Current aircraft: {:?}", runway_status.current_aircraft);

    // GUI Dialog Example
    println!("\n3. GUI Dialog Mediator:");
    let mut dialog = FormDialog::new();

    // Simulate user interactions
    println!("Simulating user interactions:");
    dialog.on_text_change("username_field", "john_doe");
    dialog.on_text_change("password_field", "secret123");
    dialog.on_checkbox_change("remember_me", true);
    dialog.on_button_click("login_button");

    println!("\nEnabling guest mode:");
    dialog.on_checkbox_change("guest_mode", true);

    // Update component states based on guest mode
    if let Some(username_field) = dialog.get_component("username_field") {
        username_field.set_enabled(false);
    }
    if let Some(password_field) = dialog.get_component("password_field") {
        password_field.set_enabled(false);
    }

    println!("\n4. Mediator Pattern Benefits:");
    println!("  ✅ Reduces coupling between interacting objects");
    println!("  ✅ Centralizes control logic");
    println!("  ✅ Makes object interactions easier to understand");
    println!("  ✅ Promotes reusable components");
    println!("  ✅ Simplifies many-to-many relationships");
    println!("  ✅ Enables complex coordination logic");
}

pub fn main() {
    demo();
}