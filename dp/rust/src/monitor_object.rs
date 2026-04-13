// Monitor Object Pattern in Rust
// Synchronized object with mutual exclusion using Mutex + Condvar

use std::sync::{Arc, Condvar, Mutex};
use std::thread;

struct BoundedBuffer<T> {
    data: Mutex<BufferInner<T>>,
    not_empty: Condvar,
    not_full: Condvar,
}

struct BufferInner<T> {
    items: Vec<T>,
    capacity: usize,
}

impl<T: std::fmt::Debug> BoundedBuffer<T> {
    fn new(capacity: usize) -> Self {
        BoundedBuffer {
            data: Mutex::new(BufferInner {
                items: Vec::with_capacity(capacity),
                capacity,
            }),
            not_empty: Condvar::new(),
            not_full: Condvar::new(),
        }
    }

    fn put(&self, item: T) {
        let mut buf = self.data.lock().unwrap();
        while buf.items.len() >= buf.capacity {
            buf = self.not_full.wait(buf).unwrap();
        }
        println!("[Monitor] Put: {:?}", item);
        buf.items.push(item);
        self.not_empty.notify_one();
    }

    fn take(&self) -> T {
        let mut buf = self.data.lock().unwrap();
        while buf.items.is_empty() {
            buf = self.not_empty.wait(buf).unwrap();
        }
        let item = buf.items.remove(0);
        println!("[Monitor] Take: {:?}", item);
        self.not_full.notify_one();
        item
    }
}

fn main() {
    let buffer = Arc::new(BoundedBuffer::new(3));

    let producer = {
        let buf = Arc::clone(&buffer);
        thread::spawn(move || {
            for i in 0..6 {
                buf.put(i);
                thread::sleep(std::time::Duration::from_millis(30));
            }
        })
    };

    let consumer = {
        let buf = Arc::clone(&buffer);
        thread::spawn(move || {
            for _ in 0..6 {
                let val = buf.take();
                println!("  Consumed: {}", val);
                thread::sleep(std::time::Duration::from_millis(60));
            }
        })
    };

    producer.join().unwrap();
    consumer.join().unwrap();
}
