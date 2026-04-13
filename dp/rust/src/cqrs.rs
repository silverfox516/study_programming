// CQRS (Command Query Responsibility Segregation) in Rust
// Separate write (command) and read (query) models

use std::collections::HashMap;

// --- Commands (write side) ---
#[derive(Debug)]
enum Command {
    CreateUser { id: u32, name: String },
    UpdateEmail { id: u32, email: String },
}

// --- Events ---
#[derive(Debug, Clone)]
enum Event {
    UserCreated { id: u32, name: String },
    EmailUpdated { id: u32, email: String },
}

// --- Write model ---
struct CommandHandler {
    events: Vec<Event>,
}

impl CommandHandler {
    fn new() -> Self {
        CommandHandler { events: Vec::new() }
    }

    fn handle(&mut self, cmd: Command) {
        let event = match cmd {
            Command::CreateUser { id, name } => {
                println!("[Command] CreateUser: {} ({})", name, id);
                Event::UserCreated { id, name }
            }
            Command::UpdateEmail { id, email } => {
                println!("[Command] UpdateEmail: {} -> {}", id, email);
                Event::EmailUpdated { id, email }
            }
        };
        self.events.push(event);
    }
}

// --- Read model ---
#[derive(Debug, Clone)]
struct UserView {
    id: u32,
    name: String,
    email: Option<String>,
}

struct QueryHandler {
    users: HashMap<u32, UserView>,
}

impl QueryHandler {
    fn new() -> Self {
        QueryHandler { users: HashMap::new() }
    }

    fn apply(&mut self, event: &Event) {
        match event {
            Event::UserCreated { id, name } => {
                self.users.insert(*id, UserView {
                    id: *id,
                    name: name.clone(),
                    email: None,
                });
            }
            Event::EmailUpdated { id, email } => {
                if let Some(user) = self.users.get_mut(id) {
                    user.email = Some(email.clone());
                }
            }
        }
    }

    fn get_user(&self, id: u32) -> Option<&UserView> {
        self.users.get(&id)
    }

    fn all_users(&self) -> Vec<&UserView> {
        self.users.values().collect()
    }
}

fn main() {
    let mut cmd_handler = CommandHandler::new();
    let mut query_handler = QueryHandler::new();

    // Write side
    cmd_handler.handle(Command::CreateUser { id: 1, name: "Alice".into() });
    cmd_handler.handle(Command::CreateUser { id: 2, name: "Bob".into() });
    cmd_handler.handle(Command::UpdateEmail { id: 1, email: "alice@example.com".into() });

    // Project events to read model
    for event in &cmd_handler.events {
        query_handler.apply(event);
    }

    // Query side
    println!("\n--- Query Results ---");
    for user in query_handler.all_users() {
        println!("{:?}", user);
    }
}
