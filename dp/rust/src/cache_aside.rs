// Cache-Aside Pattern in Rust
// Application manages cache explicitly: read-through with lazy population

use std::collections::HashMap;
use std::time::{Duration, Instant};

struct CacheEntry<T> {
    value: T,
    expires_at: Instant,
}

struct Cache<T> {
    entries: HashMap<String, CacheEntry<T>>,
    ttl: Duration,
}

impl<T: Clone + std::fmt::Debug> Cache<T> {
    fn new(ttl: Duration) -> Self {
        Cache {
            entries: HashMap::new(),
            ttl,
        }
    }

    fn get(&self, key: &str) -> Option<&T> {
        self.entries.get(key).and_then(|entry| {
            if Instant::now() < entry.expires_at {
                println!("[Cache] HIT: {}", key);
                Some(&entry.value)
            } else {
                println!("[Cache] EXPIRED: {}", key);
                None
            }
        })
    }

    fn put(&mut self, key: String, value: T) {
        println!("[Cache] PUT: {} = {:?}", key, value);
        self.entries.insert(key, CacheEntry {
            value,
            expires_at: Instant::now() + self.ttl,
        });
    }

    fn invalidate(&mut self, key: &str) {
        println!("[Cache] INVALIDATE: {}", key);
        self.entries.remove(key);
    }
}

// Simulated database
struct Database {
    data: HashMap<String, String>,
}

impl Database {
    fn new() -> Self {
        let mut data = HashMap::new();
        data.insert("user:1".into(), "Alice".into());
        data.insert("user:2".into(), "Bob".into());
        data.insert("user:3".into(), "Charlie".into());
        Database { data }
    }

    fn query(&self, key: &str) -> Option<String> {
        println!("[DB] Query: {}", key);
        self.data.get(key).cloned()
    }
}

// Cache-aside service
fn get_user(cache: &mut Cache<String>, db: &Database, key: &str) -> Option<String> {
    // 1. Check cache
    if let Some(val) = cache.get(key) {
        return Some(val.clone());
    }

    // 2. Cache miss — load from DB
    println!("[Cache] MISS: {}", key);
    if let Some(val) = db.query(key) {
        // 3. Populate cache
        cache.put(key.to_string(), val.clone());
        Some(val)
    } else {
        None
    }
}

fn main() {
    let mut cache = Cache::new(Duration::from_millis(300));
    let db = Database::new();

    // First access — cache miss
    println!("=== First access ===");
    let user = get_user(&mut cache, &db, "user:1");
    println!("Result: {:?}\n", user);

    // Second access — cache hit
    println!("=== Second access ===");
    let user = get_user(&mut cache, &db, "user:1");
    println!("Result: {:?}\n", user);

    // Invalidate on write
    println!("=== After invalidation ===");
    cache.invalidate("user:1");
    let user = get_user(&mut cache, &db, "user:1");
    println!("Result: {:?}\n", user);

    // TTL expiration
    println!("=== After TTL expiry ===");
    std::thread::sleep(Duration::from_millis(350));
    let user = get_user(&mut cache, &db, "user:1");
    println!("Result: {:?}", user);
}
