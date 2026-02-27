/// MVC (Model-View-Controller) Pattern Implementation in Rust
///
/// The MVC pattern separates concerns into three interconnected components:
/// - Model: Manages data and business logic
/// - View: Handles the user interface and presentation
/// - Controller: Acts as an intermediary between Model and View

use std::collections::HashMap;

// Model
#[derive(Debug, Clone)]
pub struct User {
    id: u32,
    name: String,
    email: String,
}

impl User {
    pub fn new(id: u32, name: String, email: String) -> Result<Self, String> {
        if name.is_empty() {
            return Err("Name cannot be empty".to_string());
        }
        if email.is_empty() || !email.contains('@') {
            return Err("Invalid email address".to_string());
        }

        Ok(User { id, name, email })
    }

    pub fn id(&self) -> u32 {
        self.id
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn email(&self) -> &str {
        &self.email
    }

    pub fn set_name(&mut self, name: String) -> Result<(), String> {
        if name.is_empty() {
            return Err("Name cannot be empty".to_string());
        }
        self.name = name;
        Ok(())
    }

    pub fn set_email(&mut self, email: String) -> Result<(), String> {
        if email.is_empty() || !email.contains('@') {
            return Err("Invalid email address".to_string());
        }
        self.email = email;
        Ok(())
    }
}

#[derive(Debug, Default)]
pub struct UserModel {
    users: HashMap<u32, User>,
}

impl UserModel {
    pub fn new() -> Self {
        Self {
            users: HashMap::new(),
        }
    }

    pub fn add_user(&mut self, user: User) -> Result<(), String> {
        if self.users.contains_key(&user.id) {
            return Err(format!("User with ID {} already exists", user.id));
        }
        self.users.insert(user.id, user);
        Ok(())
    }

    pub fn get_user(&self, id: u32) -> Option<&User> {
        self.users.get(&id)
    }

    pub fn get_user_mut(&mut self, id: u32) -> Option<&mut User> {
        self.users.get_mut(&id)
    }

    pub fn remove_user(&mut self, id: u32) -> Option<User> {
        self.users.remove(&id)
    }

    pub fn get_all_users(&self) -> Vec<&User> {
        self.users.values().collect()
    }

    pub fn user_count(&self) -> usize {
        self.users.len()
    }

    pub fn find_users_by_name(&self, name: &str) -> Vec<&User> {
        self.users
            .values()
            .filter(|user| user.name.to_lowercase().contains(&name.to_lowercase()))
            .collect()
    }
}

// View trait for different presentation styles
pub trait UserView {
    fn display_user(&self, user: &User);
    fn display_users(&self, users: &[&User]);
    fn display_message(&self, message: &str);
    fn display_error(&self, error: &str);
    fn display_user_not_found(&self, id: u32);
}

// Console View implementation
pub struct ConsoleUserView;

impl UserView for ConsoleUserView {
    fn display_user(&self, user: &User) {
        println!("┌─────────────────────────");
        println!("│ User ID: {}", user.id());
        println!("│ Name: {}", user.name());
        println!("│ Email: {}", user.email());
        println!("└─────────────────────────");
    }

    fn display_users(&self, users: &[&User]) {
        if users.is_empty() {
            self.display_message("No users found");
            return;
        }

        println!("=== All Users ({}) ===", users.len());
        for user in users {
            self.display_user(user);
        }
    }

    fn display_message(&self, message: &str) {
        println!("✓ {}", message);
    }

    fn display_error(&self, error: &str) {
        println!("✗ Error: {}", error);
    }

    fn display_user_not_found(&self, id: u32) {
        println!("✗ User with ID {} not found", id);
    }
}

// JSON View implementation
pub struct JsonUserView;

impl UserView for JsonUserView {
    fn display_user(&self, user: &User) {
        println!("{{");
        println!("  \"id\": {},", user.id());
        println!("  \"name\": \"{}\",", user.name());
        println!("  \"email\": \"{}\"", user.email());
        println!("}}");
    }

    fn display_users(&self, users: &[&User]) {
        println!("[");
        for (i, user) in users.iter().enumerate() {
            print!("  ");
            if i > 0 {
                println!(",");
                print!("  ");
            }
            println!("{{");
            println!("    \"id\": {},", user.id());
            println!("    \"name\": \"{}\",", user.name());
            println!("    \"email\": \"{}\"", user.email());
            print!("  }}");
        }
        if !users.is_empty() {
            println!();
        }
        println!("]");
    }

    fn display_message(&self, message: &str) {
        println!("{{ \"status\": \"success\", \"message\": \"{}\" }}", message);
    }

    fn display_error(&self, error: &str) {
        println!("{{ \"status\": \"error\", \"message\": \"{}\" }}", error);
    }

    fn display_user_not_found(&self, id: u32) {
        println!("{{ \"status\": \"error\", \"message\": \"User with ID {} not found\" }}", id);
    }
}

// Controller
pub struct UserController<V: UserView> {
    model: UserModel,
    view: V,
}

impl<V: UserView> UserController<V> {
    pub fn new(model: UserModel, view: V) -> Self {
        Self { model, view }
    }

    pub fn add_user(&mut self, id: u32, name: String, email: String) {
        match User::new(id, name, email) {
            Ok(user) => {
                match self.model.add_user(user) {
                    Ok(()) => self.view.display_message("User added successfully"),
                    Err(e) => self.view.display_error(&e),
                }
            }
            Err(e) => self.view.display_error(&e),
        }
    }

    pub fn show_user(&self, id: u32) {
        match self.model.get_user(id) {
            Some(user) => self.view.display_user(user),
            None => self.view.display_user_not_found(id),
        }
    }

    pub fn show_all_users(&self) {
        let users = self.model.get_all_users();
        self.view.display_users(&users);
    }

    pub fn remove_user(&mut self, id: u32) {
        match self.model.remove_user(id) {
            Some(_) => self.view.display_message("User removed successfully"),
            None => self.view.display_user_not_found(id),
        }
    }

    pub fn update_user_name(&mut self, id: u32, new_name: String) {
        match self.model.get_user_mut(id) {
            Some(user) => {
                match user.set_name(new_name) {
                    Ok(()) => self.view.display_message("User name updated successfully"),
                    Err(e) => self.view.display_error(&e),
                }
            }
            None => self.view.display_user_not_found(id),
        }
    }

    pub fn update_user_email(&mut self, id: u32, new_email: String) {
        match self.model.get_user_mut(id) {
            Some(user) => {
                match user.set_email(new_email) {
                    Ok(()) => self.view.display_message("User email updated successfully"),
                    Err(e) => self.view.display_error(&e),
                }
            }
            None => self.view.display_user_not_found(id),
        }
    }

    pub fn search_users(&self, name_query: &str) {
        let users = self.model.find_users_by_name(name_query);
        if users.is_empty() {
            self.view.display_message(&format!("No users found matching '{}'", name_query));
        } else {
            println!("Search results for '{}':", name_query);
            self.view.display_users(&users);
        }
    }

    pub fn show_user_count(&self) {
        self.view.display_message(&format!("Total users: {}", self.model.user_count()));
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_user_creation() {
        let user = User::new(1, "John Doe".to_string(), "john@example.com".to_string()).unwrap();
        assert_eq!(user.id(), 1);
        assert_eq!(user.name(), "John Doe");
        assert_eq!(user.email(), "john@example.com");
    }

    #[test]
    fn test_invalid_user_creation() {
        assert!(User::new(1, "".to_string(), "john@example.com".to_string()).is_err());
        assert!(User::new(1, "John".to_string(), "invalid-email".to_string()).is_err());
    }

    #[test]
    fn test_user_model() {
        let mut model = UserModel::new();
        let user = User::new(1, "John".to_string(), "john@example.com".to_string()).unwrap();

        assert!(model.add_user(user).is_ok());
        assert_eq!(model.user_count(), 1);
        assert!(model.get_user(1).is_some());
        assert!(model.get_user(2).is_none());
    }

    #[test]
    fn test_mvc_integration() {
        let model = UserModel::new();
        let view = ConsoleUserView;
        let mut controller = UserController::new(model, view);

        controller.add_user(1, "Alice".to_string(), "alice@example.com".to_string());
        controller.show_user_count();
    }
}

fn main() {
    println!("=== MVC Pattern Demo ===\n");

    // Create MVC components with Console View
    let model = UserModel::new();
    let view = ConsoleUserView;
    let mut controller = UserController::new(model, view);

    // Add users
    controller.add_user(1, "Alice Johnson".to_string(), "alice@example.com".to_string());
    controller.add_user(2, "Bob Smith".to_string(), "bob@example.com".to_string());
    controller.add_user(3, "Charlie Brown".to_string(), "charlie@example.com".to_string());

    println!();
    controller.show_user_count();

    // Show specific user
    println!("\nShowing user with ID 2:");
    controller.show_user(2);

    // Show all users
    println!("\nShowing all users:");
    controller.show_all_users();

    // Test search functionality
    println!("\nSearching for users with 'Alice':");
    controller.search_users("Alice");

    // Test updates
    println!("\nUpdating user 1's name:");
    controller.update_user_name(1, "Alice Cooper".to_string());
    controller.show_user(1);

    // Test error handling
    println!("\nTesting error handling:");
    controller.add_user(4, "".to_string(), "invalid@example.com".to_string()); // Empty name
    controller.add_user(1, "Duplicate".to_string(), "duplicate@example.com".to_string()); // Duplicate ID

    // Remove user
    println!("\nRemoving user with ID 2:");
    controller.remove_user(2);
    controller.show_user_count();

    // Demonstrate JSON View
    println!("\n=== Using JSON View ===");
    let json_model = UserModel::new();
    let json_view = JsonUserView;
    let mut json_controller = UserController::new(json_model, json_view);

    json_controller.add_user(1, "John Doe".to_string(), "john@example.com".to_string());
    json_controller.show_user(1);
}