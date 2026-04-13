// Double-Checked Locking Pattern in Rust
// In Rust, std::sync::OnceLock (1.70+) / Once / Lazy provide this idiom safely

use std::sync::{Mutex, OnceLock};

// Expensive resource
struct Config {
    db_url: String,
    max_connections: u32,
}

impl Config {
    fn load() -> Self {
        println!("[Config] Loading (expensive operation)...");
        Config {
            db_url: "postgres://localhost/mydb".to_string(),
            max_connections: 10,
        }
    }
}

// OnceLock: the idiomatic Rust approach (no manual double-check needed)
fn global_config() -> &'static Config {
    static CONFIG: OnceLock<Config> = OnceLock::new();
    CONFIG.get_or_init(Config::load)
}

// Manual double-checked locking (educational — prefer OnceLock in practice)
struct LazyInit<T> {
    value: OnceLock<T>,
    _init_lock: Mutex<()>,
}

impl<T> LazyInit<T> {
    const fn new() -> Self {
        LazyInit {
            value: OnceLock::new(),
            _init_lock: Mutex::new(()),
        }
    }

    fn get_or_init<F>(&self, f: F) -> &T
    where
        F: FnOnce() -> T,
    {
        // First check (no lock)
        if let Some(val) = self.value.get() {
            return val;
        }
        // Acquire lock
        let _guard = self._init_lock.lock().unwrap();
        // Second check (with lock)
        self.value.get_or_init(f)
    }
}

static LAZY_VALUE: LazyInit<String> = LazyInit::new();

fn main() {
    // OnceLock approach
    println!("=== OnceLock ===");
    let cfg1 = global_config();
    let cfg2 = global_config(); // Not loaded again
    println!("DB: {}", cfg1.db_url);
    println!("Same instance: {}", std::ptr::eq(cfg1, cfg2));

    // Manual approach
    println!("\n=== Manual LazyInit ===");
    let v1 = LAZY_VALUE.get_or_init(|| {
        println!("[LazyInit] Initializing...");
        "initialized".to_string()
    });
    let v2 = LAZY_VALUE.get_or_init(|| {
        println!("[LazyInit] Should NOT print");
        "again".to_string()
    });
    println!("Value: {}", v1);
    println!("Same: {}", std::ptr::eq(v1, v2));
}
