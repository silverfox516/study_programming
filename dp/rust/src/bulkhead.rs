// Bulkhead Pattern in Rust
// Isolate failures by partitioning resources into pools

use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;

// Simple counting semaphore using Mutex + Condvar
struct Bulkhead {
    name: String,
    state: Arc<Mutex<BulkheadState>>,
    max_concurrent: usize,
}

struct BulkheadState {
    active: usize,
    max: usize,
}

impl Bulkhead {
    fn new(name: &str, max_concurrent: usize) -> Self {
        Bulkhead {
            name: name.to_string(),
            state: Arc::new(Mutex::new(BulkheadState {
                active: 0,
                max: max_concurrent,
            })),
            max_concurrent,
        }
    }

    fn execute<F, T>(&self, f: F) -> Result<T, String>
    where
        F: FnOnce() -> T,
    {
        {
            let mut state = self.state.lock().unwrap();
            if state.active >= state.max {
                return Err(format!(
                    "[{}] Rejected: all {} slots in use",
                    self.name, self.max_concurrent
                ));
            }
            state.active += 1;
            println!("[{}] Acquired permit ({}/{})", self.name, state.active, state.max);
        }

        let result = f();

        {
            let mut state = self.state.lock().unwrap();
            state.active -= 1;
            println!("[{}] Released permit ({}/{})", self.name, state.active, state.max);
        }

        Ok(result)
    }
}

fn main() {
    // Separate bulkheads for different services
    let api_bulkhead = Arc::new(Bulkhead::new("API", 2));
    let db_bulkhead = Arc::new(Bulkhead::new("Database", 3));

    println!("=== API Bulkhead (max 2) ===");
    let handles: Vec<_> = (0..4)
        .map(|i| {
            let bh = Arc::clone(&api_bulkhead);
            thread::spawn(move || {
                match bh.execute(|| {
                    println!("  [Task {}] Processing API request...", i);
                    thread::sleep(Duration::from_millis(100));
                    format!("API result {}", i)
                }) {
                    Ok(result) => println!("  [Task {}] {}", i, result),
                    Err(e) => println!("  [Task {}] {}", i, e),
                }
            })
        })
        .collect();

    for h in handles {
        h.join().unwrap();
    }

    println!("\n=== DB Bulkhead (max 3) ===");
    let handles: Vec<_> = (0..5)
        .map(|i| {
            let bh = Arc::clone(&db_bulkhead);
            thread::spawn(move || {
                match bh.execute(|| {
                    println!("  [Query {}] Running...", i);
                    thread::sleep(Duration::from_millis(80));
                    i * 10
                }) {
                    Ok(result) => println!("  [Query {}] Result: {}", i, result),
                    Err(e) => println!("  [Query {}] {}", i, e),
                }
            })
        })
        .collect();

    for h in handles {
        h.join().unwrap();
    }
}
