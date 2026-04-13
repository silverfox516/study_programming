// Data Mapper Pattern in Rust
// Maps between domain objects and persistence representation

use std::collections::HashMap;

// Domain model (clean, no persistence concerns)
#[derive(Debug, Clone)]
struct User {
    id: u32,
    name: String,
    email: String,
}

// Persistence representation (row in database)
#[derive(Debug)]
struct UserRow {
    user_id: u32,
    user_name: String,
    user_email: String,
    created_at: String,
}

// Data Mapper: translates between domain and persistence
struct UserMapper;

impl UserMapper {
    fn to_domain(row: &UserRow) -> User {
        User {
            id: row.user_id,
            name: row.user_name.clone(),
            email: row.user_email.clone(),
        }
    }

    fn to_row(user: &User) -> UserRow {
        UserRow {
            user_id: user.id,
            user_name: user.name.clone(),
            user_email: user.email.clone(),
            created_at: "2024-01-01".to_string(),
        }
    }
}

// Simulated database
struct Database {
    rows: HashMap<u32, UserRow>,
}

impl Database {
    fn new() -> Self {
        Database { rows: HashMap::new() }
    }

    fn save(&mut self, user: &User) {
        let row = UserMapper::to_row(user);
        println!("[DB] Saving row: {:?}", row);
        self.rows.insert(row.user_id, row);
    }

    fn find(&self, id: u32) -> Option<User> {
        self.rows.get(&id).map(UserMapper::to_domain)
    }

    fn find_all(&self) -> Vec<User> {
        self.rows.values().map(UserMapper::to_domain).collect()
    }
}

fn main() {
    let mut db = Database::new();

    // Domain objects
    let alice = User { id: 1, name: "Alice".into(), email: "alice@example.com".into() };
    let bob = User { id: 2, name: "Bob".into(), email: "bob@example.com".into() };

    // Save through mapper
    db.save(&alice);
    db.save(&bob);

    // Load through mapper
    println!("\nFind by id=1: {:?}", db.find(1));
    println!("All users: {:?}", db.find_all());
}
