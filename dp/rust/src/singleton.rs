/// Singleton Pattern Implementation in Rust
///
/// The Singleton pattern ensures a class has only one instance and provides
/// global access to it. In Rust, we use lazy_static or std::sync::Once for
/// thread-safe singleton implementation.

use std::sync::{Arc, Mutex, OnceLock};

/// Thread-safe Singleton using std::sync::OnceLock
#[derive(Debug)]
pub struct Database {
    connection_string: String,
}

impl Database {
    fn new() -> Self {
        println!("Creating database connection...");
        Database {
            connection_string: "postgresql://localhost:5432/myapp".to_string(),
        }
    }

    pub fn query(&self, sql: &str) -> Result<Vec<String>, String> {
        println!("Executing query: {}", sql);
        Ok(vec![format!("Result for: {}", sql)])
    }

    pub fn connection_string(&self) -> &str {
        &self.connection_string
    }
}

static DATABASE_INSTANCE: OnceLock<Arc<Mutex<Database>>> = OnceLock::new();

pub fn get_database() -> Arc<Mutex<Database>> {
    DATABASE_INSTANCE.get_or_init(|| {
        Arc::new(Mutex::new(Database::new()))
    }).clone()
}

/// Alternative implementation using lazy_static (requires lazy_static crate)
/// This is more idiomatic but requires an external dependency
/*
use lazy_static::lazy_static;

lazy_static! {
    static ref DATABASE: Arc<Mutex<Database>> = Arc::new(Mutex::new(Database::new()));
}

pub fn get_database_lazy() -> Arc<Mutex<Database>> {
    DATABASE.clone()
}
*/

/// Configuration Singleton for application settings
#[derive(Debug)]
pub struct Config {
    debug_mode: bool,
    max_connections: u32,
    timeout_seconds: u64,
}

impl Config {
    fn new() -> Self {
        Config {
            debug_mode: true,
            max_connections: 100,
            timeout_seconds: 30,
        }
    }

    pub fn set_debug_mode(&mut self, enabled: bool) {
        self.debug_mode = enabled;
    }

    pub fn debug_mode(&self) -> bool {
        self.debug_mode
    }

    pub fn max_connections(&self) -> u32 {
        self.max_connections
    }

    pub fn timeout_seconds(&self) -> u64 {
        self.timeout_seconds
    }
}

static CONFIG_INSTANCE: OnceLock<Arc<Mutex<Config>>> = OnceLock::new();

pub fn get_config() -> Arc<Mutex<Config>> {
    CONFIG_INSTANCE.get_or_init(|| {
        Arc::new(Mutex::new(Config::new()))
    }).clone()
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::thread;

    #[test]
    fn test_singleton_same_instance() {
        let db1 = get_database();
        let db2 = get_database();

        // Both should point to the same instance
        assert!(Arc::ptr_eq(&db1, &db2));
    }

    #[test]
    fn test_singleton_thread_safety() {
        let handles: Vec<_> = (0..10)
            .map(|i| {
                thread::spawn(move || {
                    let db = get_database();
                    let guard = db.lock().unwrap();
                    guard.query(&format!("SELECT * FROM table_{}", i))
                })
            })
            .collect();

        for handle in handles {
            let result = handle.join().unwrap();
            assert!(result.is_ok());
        }
    }

    #[test]
    fn test_config_singleton() {
        let config1 = get_config();
        let config2 = get_config();

        assert!(Arc::ptr_eq(&config1, &config2));

        // Test mutation
        {
            let mut guard = config1.lock().unwrap();
            guard.set_debug_mode(false);
        }

        {
            let guard = config2.lock().unwrap();
            assert!(!guard.debug_mode());
        }
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Singleton Pattern Demo ===");

    // Get database instances
    let db1 = get_database();
    let db2 = get_database();

    // Verify they're the same instance
    println!("Same instance: {}", Arc::ptr_eq(&db1, &db2));

    // Use the database
    {
        let db = db1.lock().unwrap();
        let _ = db.query("SELECT * FROM users");
        println!("Connection: {}", db.connection_string());
    }

    // Configuration example
    let config = get_config();
    {
        let mut cfg = config.lock().unwrap();
        println!("Debug mode: {}", cfg.debug_mode());
        cfg.set_debug_mode(false);
        println!("Debug mode after change: {}", cfg.debug_mode());
    }
}

fn main() {
    println!("=== Singleton Pattern Demo ===");

    // Test Thread-Safe Singleton
    println!("\n1. Thread-Safe Singleton:");
    let instance1 = get_database();
    let instance2 = get_database();

    println!("Instances are same: {}", Arc::ptr_eq(&instance1, &instance2));

    {
        let db = instance1.lock().unwrap();
        println!("Connection: {}", db.connection_string());
        let _ = db.query("SELECT * FROM users");
    }

    // Test Configuration Singleton
    println!("\n2. Configuration Singleton:");
    let config1 = get_config();
    let config2 = get_config();

    println!("Config instances are same: {}", Arc::ptr_eq(&config1, &config2));

    {
        let mut cfg = config1.lock().unwrap();
        println!("Debug mode: {}", cfg.debug_mode());
        cfg.set_debug_mode(false);
        println!("Debug mode after change: {}", cfg.debug_mode());
    }
}