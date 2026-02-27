/// Null Object Pattern Implementation in Rust
///
/// The Null Object pattern provides an object as a surrogate for the lack of an object
/// of a given type. Instead of using null references, it provides a default behavior.

use std::collections::HashMap;

// Customer trait
pub trait Customer {
    fn greet(&self);
    fn purchase(&mut self, item: &str);
    fn get_discount_rate(&self) -> u32;
    fn is_null(&self) -> bool;
    fn get_name(&self) -> &str;
    fn get_id(&self) -> i32;
}

// Real Customer implementation
#[derive(Debug, Clone)]
pub struct RealCustomer {
    id: i32,
    name: String,
    email: String,
    loyalty_points: u32,
}

impl RealCustomer {
    pub fn new(id: i32, name: String, email: String) -> Self {
        Self {
            id,
            name,
            email,
            loyalty_points: 0,
        }
    }

    pub fn get_email(&self) -> &str {
        &self.email
    }

    pub fn get_loyalty_points(&self) -> u32 {
        self.loyalty_points
    }
}

impl Customer for RealCustomer {
    fn greet(&self) {
        println!("Hello {}! Welcome back!", self.name);
    }

    fn purchase(&mut self, item: &str) {
        println!("{} purchased: {}", self.name, item);
        self.loyalty_points += 10;
        println!("Loyalty points: {}", self.loyalty_points);
    }

    fn get_discount_rate(&self) -> u32 {
        match self.loyalty_points {
            points if points > 100 => 15,
            points if points > 50 => 10,
            _ => 5,
        }
    }

    fn is_null(&self) -> bool {
        false
    }

    fn get_name(&self) -> &str {
        &self.name
    }

    fn get_id(&self) -> i32 {
        self.id
    }
}

// Null Customer implementation (using singleton-like pattern)
#[derive(Debug)]
pub struct NullCustomer;

impl Customer for NullCustomer {
    fn greet(&self) {
        println!("Welcome, guest!");
    }

    fn purchase(&mut self, item: &str) {
        println!("Please register to purchase: {}", item);
    }

    fn get_discount_rate(&self) -> u32 {
        0 // No discount for null customer
    }

    fn is_null(&self) -> bool {
        true
    }

    fn get_name(&self) -> &str {
        "Guest"
    }

    fn get_id(&self) -> i32 {
        -1
    }
}

// Customer Repository
#[derive(Debug)]
pub struct CustomerRepository {
    customers: HashMap<i32, RealCustomer>,
}

impl CustomerRepository {
    pub fn new() -> Self {
        Self {
            customers: HashMap::new(),
        }
    }

    pub fn add_customer(&mut self, customer: RealCustomer) {
        self.customers.insert(customer.get_id(), customer);
    }

    pub fn find_customer(&self, id: i32) -> Box<dyn Customer> {
        if let Some(customer) = self.customers.get(&id) {
            Box::new(customer.clone())
        } else {
            // Return null object instead of Option::None
            Box::new(NullCustomer)
        }
    }

    pub fn get_customer_count(&self) -> usize {
        self.customers.len()
    }
}

// Service using customers
pub struct CustomerService {
    repository: CustomerRepository,
}

impl CustomerService {
    pub fn new() -> Self {
        Self {
            repository: CustomerRepository::new(),
        }
    }

    pub fn add_customer(&mut self, id: i32, name: String, email: String) {
        let customer = RealCustomer::new(id, name, email);
        self.repository.add_customer(customer);
    }

    pub fn process_order(&mut self, customer_id: i32, item: &str, price: f64) {
        let mut customer = self.repository.find_customer(customer_id);

        // No null check needed - null object handles it gracefully
        customer.greet();

        let discount = customer.get_discount_rate();
        let final_price = price * (100 - discount) as f64 / 100.0;

        println!("Processing order for customer ID {}", customer_id);
        println!("Item: {}, Original price: ${:.2}", item, price);
        println!("Discount: {}%, Final price: ${:.2}", discount, final_price);

        customer.purchase(item);

        if customer.is_null() {
            println!("Note: This was a guest purchase");
        }

        println!("---");
    }
}

// Logger example with Null Object pattern
pub trait Logger {
    fn log(&self, level: &str, message: &str);
    fn is_null(&self) -> bool {
        false
    }
}

// Console Logger
#[derive(Debug)]
pub struct ConsoleLogger;

impl Logger for ConsoleLogger {
    fn log(&self, level: &str, message: &str) {
        println!("[{}] {}", level, message);
    }
}

// File Logger (simulated)
#[derive(Debug)]
pub struct FileLogger {
    filename: String,
}

impl FileLogger {
    pub fn new(filename: String) -> Self {
        Self { filename }
    }
}

impl Logger for FileLogger {
    fn log(&self, level: &str, message: &str) {
        println!("[FILE LOG to {}] [{}] {}", self.filename, level, message);
    }
}

// Null Logger
#[derive(Debug)]
pub struct NullLogger;

impl Logger for NullLogger {
    fn log(&self, _level: &str, _message: &str) {
        // Do nothing - silent logging
    }

    fn is_null(&self) -> bool {
        true
    }
}

// Application using logger
pub struct Application {
    logger: Box<dyn Logger>,
    name: String,
}

impl Application {
    pub fn new(name: String, logger: Option<Box<dyn Logger>>) -> Self {
        Self {
            name,
            // Use null logger if none provided
            logger: logger.unwrap_or_else(|| Box::new(NullLogger)),
        }
    }

    pub fn run(&self) {
        self.logger.log("INFO", &format!("{} application starting", self.name));
        self.logger.log("DEBUG", "Processing data");

        // Simulate some work
        for i in 1..=3 {
            self.logger.log("DEBUG", &format!("Processing step {}", i));
        }

        self.logger.log("INFO", &format!("{} application finished", self.name));

        if self.logger.is_null() {
            println!("(Logging was disabled for this application)");
        }
    }
}

// Database connection example
pub trait DatabaseConnection {
    fn execute_query(&self, query: &str) -> Result<Vec<String>, String>;
    fn is_connected(&self) -> bool;
    fn is_null(&self) -> bool {
        false
    }
}

#[derive(Debug)]
pub struct PostgreSQLConnection {
    connection_string: String,
    connected: bool,
}

impl PostgreSQLConnection {
    pub fn new(connection_string: String) -> Result<Self, String> {
        // Simulate connection attempt
        if connection_string.is_empty() {
            return Err("Invalid connection string".to_string());
        }

        Ok(Self {
            connection_string,
            connected: true,
        })
    }
}

impl DatabaseConnection for PostgreSQLConnection {
    fn execute_query(&self, query: &str) -> Result<Vec<String>, String> {
        if !self.connected {
            return Err("Not connected to database".to_string());
        }

        println!("Executing query on PostgreSQL: {}", query);
        Ok(vec![format!("Result for: {}", query)])
    }

    fn is_connected(&self) -> bool {
        self.connected
    }
}

// Null Database Connection
#[derive(Debug)]
pub struct NullDatabaseConnection;

impl DatabaseConnection for NullDatabaseConnection {
    fn execute_query(&self, query: &str) -> Result<Vec<String>, String> {
        println!("Database not available. Query ignored: {}", query);
        Ok(vec!["No database connection available".to_string()])
    }

    fn is_connected(&self) -> bool {
        false
    }

    fn is_null(&self) -> bool {
        true
    }
}

pub struct DatabaseService {
    connection: Box<dyn DatabaseConnection>,
}

impl DatabaseService {
    pub fn new(connection: Option<Box<dyn DatabaseConnection>>) -> Self {
        Self {
            connection: connection.unwrap_or_else(|| Box::new(NullDatabaseConnection)),
        }
    }

    pub fn fetch_users(&self) -> Result<Vec<String>, String> {
        self.connection.execute_query("SELECT * FROM users")
    }

    pub fn is_database_available(&self) -> bool {
        self.connection.is_connected() && !self.connection.is_null()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_real_customer() {
        let mut customer = RealCustomer::new(1, "John Doe".to_string(), "john@example.com".to_string());
        assert!(!customer.is_null());
        assert_eq!(customer.get_discount_rate(), 5);

        customer.purchase("Laptop");
        assert_eq!(customer.get_loyalty_points(), 10);
        assert_eq!(customer.get_discount_rate(), 5);
    }

    #[test]
    fn test_null_customer() {
        let customer = NullCustomer;
        assert!(customer.is_null());
        assert_eq!(customer.get_discount_rate(), 0);
        assert_eq!(customer.get_name(), "Guest");
        assert_eq!(customer.get_id(), -1);
    }

    #[test]
    fn test_customer_repository() {
        let mut repo = CustomerRepository::new();
        let customer = RealCustomer::new(1, "Alice".to_string(), "alice@example.com".to_string());
        repo.add_customer(customer);

        // Find existing customer
        let found = repo.find_customer(1);
        assert!(!found.is_null());
        assert_eq!(found.get_name(), "Alice");

        // Find non-existing customer - should return null object
        let not_found = repo.find_customer(999);
        assert!(not_found.is_null());
        assert_eq!(not_found.get_name(), "Guest");
    }

    #[test]
    fn test_logger_null_object() {
        let console_logger: Box<dyn Logger> = Box::new(ConsoleLogger);
        let null_logger: Box<dyn Logger> = Box::new(NullLogger);

        assert!(!console_logger.is_null());
        assert!(null_logger.is_null());
    }

    #[test]
    fn test_database_null_object() {
        let real_db = PostgreSQLConnection::new("postgresql://localhost".to_string()).unwrap();
        let null_db = NullDatabaseConnection;

        assert!(!real_db.is_null());
        assert!(real_db.is_connected());

        assert!(null_db.is_null());
        assert!(!null_db.is_connected());

        // Both should handle queries gracefully
        let _ = real_db.execute_query("SELECT 1");
        let _ = null_db.execute_query("SELECT 1");
    }
}

fn main() {
    println!("=== Null Object Pattern Demo ===\n");

    // 1. Customer service example
    println!("1. Customer Service Example:");
    let mut service = CustomerService::new();

    // Add real customers
    service.add_customer(1, "Alice Johnson".to_string(), "alice@example.com".to_string());
    service.add_customer(2, "Bob Smith".to_string(), "bob@example.com".to_string());

    // Process orders for existing customers
    service.process_order(1, "Laptop", 1000.0);
    service.process_order(2, "Mouse", 50.0);

    // Process order for non-existent customer - null object handles it gracefully
    service.process_order(999, "Keyboard", 100.0);

    // 2. Logger example
    println!("\n2. Logger Example:");

    println!("With console logger:");
    let app_with_logger = Application::new(
        "MyApp".to_string(),
        Some(Box::new(ConsoleLogger))
    );
    app_with_logger.run();

    println!("\nWith null logger (silent):");
    let app_without_logger = Application::new(
        "SilentApp".to_string(),
        None // This will use null logger
    );
    app_without_logger.run();

    println!("\nWith file logger:");
    let app_with_file_logger = Application::new(
        "FileApp".to_string(),
        Some(Box::new(FileLogger::new("app.log".to_string())))
    );
    app_with_file_logger.run();

    // 3. Database example
    println!("\n3. Database Example:");

    // Service with real database
    let db_connection = PostgreSQLConnection::new("postgresql://localhost:5432/mydb".to_string()).ok();
    let db_service = DatabaseService::new(db_connection.map(|conn| Box::new(conn) as Box<dyn DatabaseConnection>));

    println!("Database available: {}", db_service.is_database_available());
    match db_service.fetch_users() {
        Ok(users) => println!("Users: {:?}", users),
        Err(e) => println!("Error: {}", e),
    }

    // Service without database (null object)
    let offline_service = DatabaseService::new(None);
    println!("\nOffline service:");
    println!("Database available: {}", offline_service.is_database_available());
    match offline_service.fetch_users() {
        Ok(users) => println!("Users: {:?}", users),
        Err(e) => println!("Error: {}", e),
    }

    println!("\n✅ Null Object pattern prevents null pointer errors");
    println!("   and provides graceful default behavior!");
}