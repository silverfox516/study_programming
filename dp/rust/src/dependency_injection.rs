/// Dependency Injection Pattern Implementation in Rust
///
/// Dependency Injection is a technique for achieving Inversion of Control
/// between classes and their dependencies. Instead of creating dependencies
/// internally, they are provided from external sources.

use std::sync::Arc;

// Service traits (interfaces)
pub trait Logger {
    fn log(&self, message: &str);
}

pub trait Database {
    fn save(&self, data: &str) -> Result<u32, String>;
    fn find(&self, id: u32) -> Result<String, String>;
}

pub trait EmailService {
    fn send_email(&self, to: &str, subject: &str, body: &str) -> Result<(), String>;
}

// Concrete implementations
pub struct ConsoleLogger;

impl Logger for ConsoleLogger {
    fn log(&self, message: &str) {
        println!("[LOG] {}", message);
    }
}

pub struct FileLogger {
    filename: String,
}

impl FileLogger {
    pub fn new(filename: String) -> Self {
        Self { filename }
    }
}

impl Logger for FileLogger {
    fn log(&self, message: &str) {
        println!("[FILE LOG to {}] {}", self.filename, message);
    }
}

pub struct InMemoryDatabase {
    data: std::sync::Mutex<std::collections::HashMap<u32, String>>,
    next_id: std::sync::Mutex<u32>,
}

impl InMemoryDatabase {
    pub fn new() -> Self {
        Self {
            data: std::sync::Mutex::new(std::collections::HashMap::new()),
            next_id: std::sync::Mutex::new(1),
        }
    }
}

impl Database for InMemoryDatabase {
    fn save(&self, data: &str) -> Result<u32, String> {
        let mut db = self.data.lock().map_err(|e| e.to_string())?;
        let mut id = self.next_id.lock().map_err(|e| e.to_string())?;

        let current_id = *id;
        db.insert(current_id, data.to_string());
        *id += 1;

        Ok(current_id)
    }

    fn find(&self, id: u32) -> Result<String, String> {
        let db = self.data.lock().map_err(|e| e.to_string())?;
        db.get(&id)
            .cloned()
            .ok_or_else(|| format!("Record with ID {} not found", id))
    }
}

pub struct MockEmailService;

impl EmailService for MockEmailService {
    fn send_email(&self, to: &str, subject: &str, body: &str) -> Result<(), String> {
        println!("[EMAIL] To: {}, Subject: {}, Body: {}", to, subject, body);
        Ok(())
    }
}

// Business service that depends on other services
pub struct UserService {
    logger: Arc<dyn Logger>,
    database: Arc<dyn Database>,
    email_service: Arc<dyn EmailService>,
}

impl UserService {
    // Constructor injection
    pub fn new(
        logger: Arc<dyn Logger>,
        database: Arc<dyn Database>,
        email_service: Arc<dyn EmailService>,
    ) -> Self {
        Self {
            logger,
            database,
            email_service,
        }
    }

    pub fn create_user(&self, name: &str, email: &str) -> Result<u32, String> {
        self.logger.log(&format!("Creating user: {}", name));

        let user_data = format!("{{\"name\":\"{}\",\"email\":\"{}\"}}", name, email);
        let user_id = self.database.save(&user_data)?;

        self.logger.log(&format!("User created with ID: {}", user_id));

        // Send welcome email
        let subject = "Welcome!";
        let body = &format!("Hello {}, welcome to our service!", name);
        self.email_service.send_email(email, subject, body)?;

        Ok(user_id)
    }

    pub fn get_user(&self, id: u32) -> Result<String, String> {
        self.logger.log(&format!("Retrieving user with ID: {}", id));
        self.database.find(id)
    }
}

// Simple Dependency Injection Container
pub struct Container {
    services: std::collections::HashMap<String, Box<dyn std::any::Any>>,
}

impl Container {
    pub fn new() -> Self {
        Self {
            services: std::collections::HashMap::new(),
        }
    }

    pub fn register<T: 'static>(&mut self, name: &str, service: T) {
        self.services.insert(name.to_string(), Box::new(service));
    }

    pub fn resolve<T: 'static>(&self, name: &str) -> Option<&T> {
        self.services
            .get(name)
            .and_then(|service| service.downcast_ref::<T>())
    }
}

// Builder pattern for dependency injection
pub struct UserServiceBuilder {
    logger: Option<Arc<dyn Logger>>,
    database: Option<Arc<dyn Database>>,
    email_service: Option<Arc<dyn EmailService>>,
}

impl UserServiceBuilder {
    pub fn new() -> Self {
        Self {
            logger: None,
            database: None,
            email_service: None,
        }
    }

    pub fn with_logger(mut self, logger: Arc<dyn Logger>) -> Self {
        self.logger = Some(logger);
        self
    }

    pub fn with_database(mut self, database: Arc<dyn Database>) -> Self {
        self.database = Some(database);
        self
    }

    pub fn with_email_service(mut self, email_service: Arc<dyn EmailService>) -> Self {
        self.email_service = Some(email_service);
        self
    }

    pub fn build(self) -> Result<UserService, String> {
        let logger = self.logger.ok_or("Logger is required")?;
        let database = self.database.ok_or("Database is required")?;
        let email_service = self.email_service.ok_or("Email service is required")?;

        Ok(UserService::new(logger, database, email_service))
    }
}

// Factory for creating configured services
pub struct ServiceFactory;

impl ServiceFactory {
    pub fn create_development_user_service() -> UserService {
        let logger = Arc::new(ConsoleLogger);
        let database = Arc::new(InMemoryDatabase::new());
        let email_service = Arc::new(MockEmailService);

        UserService::new(logger, database, email_service)
    }

    pub fn create_production_user_service() -> UserService {
        let logger = Arc::new(FileLogger::new("app.log".to_string()));
        let database = Arc::new(InMemoryDatabase::new()); // In real app, would be PostgreSQL, etc.
        let email_service = Arc::new(MockEmailService); // In real app, would be SMTP service

        UserService::new(logger, database, email_service)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    struct TestLogger {
        messages: std::sync::Mutex<Vec<String>>,
    }

    impl TestLogger {
        fn new() -> Self {
            Self {
                messages: std::sync::Mutex::new(Vec::new()),
            }
        }

        fn get_messages(&self) -> Vec<String> {
            self.messages.lock().unwrap().clone()
        }
    }

    impl Logger for TestLogger {
        fn log(&self, message: &str) {
            self.messages.lock().unwrap().push(message.to_string());
        }
    }

    #[test]
    fn test_dependency_injection() {
        let logger = Arc::new(TestLogger::new());
        let database = Arc::new(InMemoryDatabase::new());
        let email_service = Arc::new(MockEmailService);

        let user_service = UserService::new(
            logger.clone(),
            database,
            email_service,
        );

        let user_id = user_service.create_user("John Doe", "john@example.com").unwrap();
        assert_eq!(user_id, 1);

        let messages = logger.get_messages();
        assert!(messages.contains(&"Creating user: John Doe".to_string()));
        assert!(messages.contains(&"User created with ID: 1".to_string()));
    }

    #[test]
    fn test_builder_pattern() {
        let logger = Arc::new(ConsoleLogger);
        let database = Arc::new(InMemoryDatabase::new());
        let email_service = Arc::new(MockEmailService);

        let user_service = UserServiceBuilder::new()
            .with_logger(logger)
            .with_database(database)
            .with_email_service(email_service)
            .build()
            .unwrap();

        let user_id = user_service.create_user("Jane Doe", "jane@example.com").unwrap();
        assert_eq!(user_id, 1);
    }
}

fn main() {
    println!("=== Dependency Injection Pattern Demo ===\n");

    // Method 1: Manual dependency injection
    println!("1. Manual Dependency Injection:");
    let logger = Arc::new(ConsoleLogger);
    let database = Arc::new(InMemoryDatabase::new());
    let email_service = Arc::new(MockEmailService);

    let user_service = UserService::new(logger, database, email_service);

    match user_service.create_user("Alice Johnson", "alice@example.com") {
        Ok(id) => {
            println!("User created successfully with ID: {}", id);

            match user_service.get_user(id) {
                Ok(user_data) => println!("Retrieved user data: {}", user_data),
                Err(e) => println!("Error retrieving user: {}", e),
            }
        }
        Err(e) => println!("Error creating user: {}", e),
    }

    // Method 2: Using builder pattern
    println!("\n2. Builder Pattern for DI:");
    let logger2 = Arc::new(FileLogger::new("users.log".to_string()));
    let database2 = Arc::new(InMemoryDatabase::new());
    let email_service2 = Arc::new(MockEmailService);

    match UserServiceBuilder::new()
        .with_logger(logger2)
        .with_database(database2)
        .with_email_service(email_service2)
        .build()
    {
        Ok(user_service2) => {
            let _ = user_service2.create_user("Bob Smith", "bob@example.com");
        }
        Err(e) => println!("Error building service: {}", e),
    }

    // Method 3: Using factory
    println!("\n3. Factory Pattern for DI:");
    let dev_service = ServiceFactory::create_development_user_service();
    let _ = dev_service.create_user("Charlie Brown", "charlie@example.com");

    let prod_service = ServiceFactory::create_production_user_service();
    let _ = prod_service.create_user("Diana Prince", "diana@example.com");

    // Method 4: Simple container
    println!("\n4. Simple DI Container:");
    let mut container = Container::new();
    container.register("console_logger", ConsoleLogger);
    container.register("memory_db", InMemoryDatabase::new());
    container.register("email_service", MockEmailService);

    if let (Some(logger), Some(database), Some(email_service)) = (
        container.resolve::<ConsoleLogger>("console_logger"),
        container.resolve::<InMemoryDatabase>("memory_db"),
        container.resolve::<MockEmailService>("email_service"),
    ) {
        println!("Services resolved from container successfully");
    }

    println!("\nDependency Injection pattern demonstration completed!");
}