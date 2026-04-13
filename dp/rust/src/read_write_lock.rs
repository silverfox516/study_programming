// Read-Write Lock Pattern in Rust
// Multiple readers OR one writer using std::sync::RwLock

use std::collections::HashMap;
use std::sync::{Arc, RwLock};
use std::thread;
use std::time::Duration;

struct SharedCache {
    data: RwLock<HashMap<String, String>>,
}

impl SharedCache {
    fn new() -> Self {
        SharedCache {
            data: RwLock::new(HashMap::new()),
        }
    }

    fn read(&self, key: &str) -> Option<String> {
        let guard = self.data.read().unwrap();
        guard.get(key).cloned()
    }

    fn write(&self, key: String, value: String) {
        let mut guard = self.data.write().unwrap();
        println!("[Writer] {} = {}", key, value);
        guard.insert(key, value);
    }

    fn len(&self) -> usize {
        self.data.read().unwrap().len()
    }
}

fn main() {
    let cache = Arc::new(SharedCache::new());

    // Writer thread
    let writer = {
        let cache = Arc::clone(&cache);
        thread::spawn(move || {
            for i in 0..5 {
                cache.write(format!("key{}", i), format!("value{}", i));
                thread::sleep(Duration::from_millis(50));
            }
        })
    };

    // Multiple reader threads
    let readers: Vec<_> = (0..3)
        .map(|id| {
            let cache = Arc::clone(&cache);
            thread::spawn(move || {
                for i in 0..5 {
                    let key = format!("key{}", i);
                    match cache.read(&key) {
                        Some(val) => println!("[Reader {}] {} = {}", id, key, val),
                        None => println!("[Reader {}] {} not found yet", id, key),
                    }
                    thread::sleep(Duration::from_millis(30));
                }
            })
        })
        .collect();

    writer.join().unwrap();
    for r in readers {
        r.join().unwrap();
    }

    println!("Cache size: {}", cache.len());
}
