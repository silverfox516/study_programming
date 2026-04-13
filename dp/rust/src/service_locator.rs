// Service Locator Pattern in Rust
// Central registry for looking up services by type

use std::any::{Any, TypeId};
use std::collections::HashMap;

struct ServiceLocator {
    services: HashMap<TypeId, Box<dyn Any>>,
}

impl ServiceLocator {
    fn new() -> Self {
        ServiceLocator {
            services: HashMap::new(),
        }
    }

    fn register<T: 'static>(&mut self, service: T) {
        self.services.insert(TypeId::of::<T>(), Box::new(service));
    }

    fn get<T: 'static>(&self) -> Option<&T> {
        self.services
            .get(&TypeId::of::<T>())
            .and_then(|s| s.downcast_ref::<T>())
    }
}

// Example services
struct Logger {
    prefix: String,
}

impl Logger {
    fn log(&self, msg: &str) {
        println!("[{}] {}", self.prefix, msg);
    }
}

struct Database {
    url: String,
}

impl Database {
    fn query(&self, sql: &str) -> String {
        format!("Result from {}: {}", self.url, sql)
    }
}

struct Config {
    debug: bool,
}

fn main() {
    let mut locator = ServiceLocator::new();

    // Register services
    locator.register(Logger { prefix: "APP".into() });
    locator.register(Database { url: "postgres://localhost/db".into() });
    locator.register(Config { debug: true });

    // Resolve services
    if let Some(logger) = locator.get::<Logger>() {
        logger.log("Application started");
    }

    if let Some(db) = locator.get::<Database>() {
        let result = db.query("SELECT * FROM users");
        println!("{}", result);
    }

    if let Some(config) = locator.get::<Config>() {
        println!("Debug mode: {}", config.debug);
    }

    // Type not registered
    let missing = locator.get::<Vec<String>>();
    println!("Vec<String> found: {}", missing.is_some());
}
