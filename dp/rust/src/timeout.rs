// Timeout Pattern in Rust
// Bound the execution time of operations

use std::sync::mpsc;
use std::thread;
use std::time::Duration;

fn with_timeout<F, T>(duration: Duration, f: F) -> Result<T, String>
where
    F: FnOnce() -> T + Send + 'static,
    T: Send + 'static,
{
    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let result = f();
        let _ = tx.send(result);
    });

    rx.recv_timeout(duration)
        .map_err(|_| format!("Operation timed out after {:?}", duration))
}

fn slow_operation(millis: u64) -> String {
    thread::sleep(Duration::from_millis(millis));
    format!("Completed after {}ms", millis)
}

fn main() {
    // Fast operation — succeeds within timeout
    println!("=== Fast Operation ===");
    match with_timeout(Duration::from_millis(200), || slow_operation(50)) {
        Ok(result) => println!("Success: {}", result),
        Err(e) => println!("Failed: {}", e),
    }

    // Slow operation — exceeds timeout
    println!("\n=== Slow Operation ===");
    match with_timeout(Duration::from_millis(100), || slow_operation(500)) {
        Ok(result) => println!("Success: {}", result),
        Err(e) => println!("Failed: {}", e),
    }

    // Multiple operations with different timeouts
    println!("\n=== Multiple Timeouts ===");
    let operations = vec![
        ("Quick", 30u64, 100u64),
        ("Medium", 80, 100),
        ("Slow", 200, 100),
    ];

    for (name, work_ms, timeout_ms) in operations {
        let timeout = Duration::from_millis(timeout_ms);
        let result = with_timeout(timeout, move || slow_operation(work_ms));
        match result {
            Ok(r) => println!("[{}] {}", name, r),
            Err(e) => println!("[{}] {}", name, e),
        }
    }

    thread::sleep(Duration::from_millis(300)); // Let leaked threads finish
}
