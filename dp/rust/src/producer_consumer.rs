// Producer-Consumer Pattern in Rust
// Uses channels (mpsc) for thread-safe communication

use std::sync::mpsc;
use std::thread;
use std::time::Duration;

fn main() {
    let (tx, rx) = mpsc::channel::<String>();

    // Multiple producers
    for i in 0..3 {
        let tx = tx.clone();
        thread::spawn(move || {
            for j in 0..3 {
                let msg = format!("Producer {}: item {}", i, j);
                println!("[P{}] Sending: {}", i, msg);
                tx.send(msg).unwrap();
                thread::sleep(Duration::from_millis(50));
            }
        });
    }
    drop(tx); // Close the original sender

    // Consumer
    let consumer = thread::spawn(move || {
        let mut count = 0;
        while let Ok(msg) = rx.recv() {
            println!("[Consumer] Received: {}", msg);
            count += 1;
        }
        println!("[Consumer] Total items processed: {}", count);
    });

    consumer.join().unwrap();
}
