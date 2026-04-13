// Lazy Initialization Pattern in Rust
// Defer expensive computation until first access

use std::cell::OnceCell;
use std::sync::OnceLock;

// Thread-local lazy (single-threaded)
struct LazyResource {
    data: OnceCell<Vec<u8>>,
}

impl LazyResource {
    fn new() -> Self {
        println!("[LazyResource] Created (data NOT loaded)");
        LazyResource {
            data: OnceCell::new(),
        }
    }

    fn get_data(&self) -> &[u8] {
        self.data.get_or_init(|| {
            println!("[LazyResource] Loading data (expensive)...");
            vec![1, 2, 3, 4, 5]
        })
    }
}

// Thread-safe lazy (static)
fn global_config() -> &'static String {
    static CONFIG: OnceLock<String> = OnceLock::new();
    CONFIG.get_or_init(|| {
        println!("[Global] Loading config...");
        "production".to_string()
    })
}

// Lazy with Option + mutable access
struct LazyComputed {
    input: i32,
    cached: Option<i64>,
}

impl LazyComputed {
    fn new(input: i32) -> Self {
        LazyComputed { input, cached: None }
    }

    fn compute(&mut self) -> i64 {
        if let Some(val) = self.cached {
            return val;
        }
        println!("[LazyComputed] Computing for input {}...", self.input);
        let result = (self.input as i64) * (self.input as i64);
        self.cached = Some(result);
        result
    }
}

fn main() {
    // OnceCell lazy
    println!("=== OnceCell ===");
    let resource = LazyResource::new();
    println!("Before access...");
    println!("Data: {:?}", resource.get_data());
    println!("Data again: {:?}", resource.get_data()); // Not loaded again

    // OnceLock global lazy
    println!("\n=== OnceLock (global) ===");
    println!("Config: {}", global_config());
    println!("Config again: {}", global_config());

    // Mutable lazy
    println!("\n=== Option cache ===");
    let mut calc = LazyComputed::new(42);
    println!("Result: {}", calc.compute());
    println!("Result again: {}", calc.compute());
}
