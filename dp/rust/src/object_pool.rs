use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;

#[derive(Debug)]
struct DbConnection {
    id: u32,
}

impl DbConnection {
    fn new(id: u32) -> Self {
        println!("Creating new DbConnection #{}", id);
        DbConnection { id }
    }

    fn query(&self, sql: &str) {
        println!("Connection #{} executing: {}", self.id, sql);
    }
}

struct ObjectPool<T> {
    pool: Arc<Mutex<Vec<T>>>,
}

impl<T> ObjectPool<T> {
    fn new(items: Vec<T>) -> Self {
        ObjectPool {
            pool: Arc::new(Mutex::new(items)),
        }
    }

    fn acquire(&self) -> Option<PooledObject<T>> {
        let mut pool = self.pool.lock().unwrap();
        pool.pop().map(|item| PooledObject {
            item: Some(item),
            pool: self.pool.clone(),
        })
    }
}

// Wrapper that returns object to pool on Drop
struct PooledObject<T> {
    item: Option<T>,
    pool: Arc<Mutex<Vec<T>>>,
}

impl<T> std::ops::Deref for PooledObject<T> {
    type Target = T;
    fn deref(&self) -> &Self::Target {
        self.item.as_ref().unwrap()
    }
}

impl<T> Drop for PooledObject<T> {
    fn drop(&mut self) {
        if let Some(item) = self.item.take() {
            let mut pool = self.pool.lock().unwrap();
            pool.push(item);
            // println!("Object returned to pool. Available: {}", pool.len());
        }
    }
}

fn main() {
    println!("=== Object Pool Pattern (Rust) ===");

    // Initialize pool
    let initial_connections: Vec<DbConnection> = (1..=3)
        .map(|i| DbConnection::new(i))
        .collect();

    let pool = Arc::new(ObjectPool::new(initial_connections));

    println!("\n--- Multithreaded Access Simulation ---");

    let mut handles = vec![];

    for i in 0..6 {
        let pool_ref = pool.clone();
        handles.push(thread::spawn(move || {
            match pool_ref.acquire() {
                Some(conn) => {
                    println!("[Thread {}] Acquired connection #{}", i, conn.id);
                    conn.query("SELECT 1");
                    thread::sleep(Duration::from_millis(100)); // Simulate work
                    // conn drops here and returns to pool
                }
                None => {
                    println!("[Thread {}] Failed to acquire connection (Pool empty)", i);
                }
            }
        }));
    }

    for h in handles {
        h.join().unwrap();
    }

    println!("\nAll threads finished.");
}
