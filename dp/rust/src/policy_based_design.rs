// Policy-Based Design in Rust
// Compose behavior from interchangeable policy traits via generics

use std::fmt;

// --- Logging policy ---
trait LogPolicy {
    fn log(&self, msg: &str);
}

struct ConsoleLog;
impl LogPolicy for ConsoleLog {
    fn log(&self, msg: &str) { println!("[Console] {}", msg); }
}

struct SilentLog;
impl LogPolicy for SilentLog {
    fn log(&self, _msg: &str) {} // no-op
}

// --- Validation policy ---
trait ValidationPolicy {
    fn validate(&self, value: &str) -> Result<(), String>;
}

struct StrictValidation;
impl ValidationPolicy for StrictValidation {
    fn validate(&self, value: &str) -> Result<(), String> {
        if value.is_empty() { return Err("empty value".into()); }
        if value.len() > 100 { return Err("too long".into()); }
        if value.contains('<') { return Err("HTML not allowed".into()); }
        Ok(())
    }
}

struct LenientValidation;
impl ValidationPolicy for LenientValidation {
    fn validate(&self, value: &str) -> Result<(), String> {
        if value.is_empty() { return Err("empty value".into()); }
        Ok(())
    }
}

// --- Storage policy ---
trait StoragePolicy {
    fn save(&mut self, key: &str, value: &str);
    fn load(&self, key: &str) -> Option<String>;
}

struct InMemoryStorage {
    data: std::collections::HashMap<String, String>,
}

impl InMemoryStorage {
    fn new() -> Self { InMemoryStorage { data: std::collections::HashMap::new() } }
}

impl StoragePolicy for InMemoryStorage {
    fn save(&mut self, key: &str, value: &str) {
        self.data.insert(key.to_string(), value.to_string());
    }
    fn load(&self, key: &str) -> Option<String> {
        self.data.get(key).cloned()
    }
}

// --- Composed service using policies ---
struct ConfigService<L: LogPolicy, V: ValidationPolicy, S: StoragePolicy> {
    logger: L,
    validator: V,
    storage: S,
}

impl<L: LogPolicy, V: ValidationPolicy, S: StoragePolicy> ConfigService<L, V, S> {
    fn new(logger: L, validator: V, storage: S) -> Self {
        ConfigService { logger, validator, storage }
    }

    fn set(&mut self, key: &str, value: &str) -> Result<(), String> {
        self.logger.log(&format!("Setting {} = {}", key, value));
        self.validator.validate(value)?;
        self.storage.save(key, value);
        self.logger.log(&format!("Saved {}", key));
        Ok(())
    }

    fn get(&self, key: &str) -> Option<String> {
        self.logger.log(&format!("Getting {}", key));
        self.storage.load(key)
    }
}

fn main() {
    // Strict + verbose config
    println!("=== Strict + ConsoleLog ===");
    let mut strict_svc = ConfigService::new(
        ConsoleLog, StrictValidation, InMemoryStorage::new(),
    );
    let _ = strict_svc.set("name", "Alice");
    let _ = strict_svc.set("bad", "<script>");
    println!("name = {:?}", strict_svc.get("name"));

    // Lenient + silent config
    println!("\n=== Lenient + SilentLog ===");
    let mut lenient_svc = ConfigService::new(
        SilentLog, LenientValidation, InMemoryStorage::new(),
    );
    let _ = lenient_svc.set("html", "<b>bold</b>");
    println!("html = {:?}", lenient_svc.get("html"));
}
