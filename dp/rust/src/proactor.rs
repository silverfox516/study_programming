// Proactor Pattern in Rust
// Asynchronous operations with completion handlers (simulated)

use std::sync::mpsc;
use std::thread;
use std::time::Duration;

#[derive(Debug)]
struct AsyncResult {
    operation: String,
    data: String,
    success: bool,
}

type CompletionHandler = Box<dyn FnOnce(AsyncResult) + Send>;

struct Proactor {
    sender: mpsc::Sender<(String, CompletionHandler)>,
    handle: Option<thread::JoinHandle<()>>,
}

impl Proactor {
    fn new() -> Self {
        let (tx, rx) = mpsc::channel::<(String, CompletionHandler)>();

        let handle = thread::spawn(move || {
            while let Ok((operation, handler)) = rx.recv() {
                // Simulate async I/O
                println!("[Proactor] Starting async: {}", operation);
                thread::sleep(Duration::from_millis(100));

                let result = AsyncResult {
                    operation: operation.clone(),
                    data: format!("Result of '{}'", operation),
                    success: true,
                };
                handler(result);
            }
        });

        Proactor {
            sender: tx,
            handle: Some(handle),
        }
    }

    fn async_read(&self, path: &str, handler: CompletionHandler) {
        let op = format!("read({})", path);
        self.sender.send((op, handler)).unwrap();
    }

    fn async_write(&self, path: &str, handler: CompletionHandler) {
        let op = format!("write({})", path);
        self.sender.send((op, handler)).unwrap();
    }
}

impl Drop for Proactor {
    fn drop(&mut self) {
        // sender is dropped, causing the thread to exit
        if let Some(h) = self.handle.take() {
            h.join().unwrap();
        }
    }
}

fn main() {
    let proactor = Proactor::new();

    proactor.async_read("/etc/config", Box::new(|result| {
        println!("  [Completion] {}: {}", result.operation, result.data);
    }));

    proactor.async_write("/tmp/output", Box::new(|result| {
        println!("  [Completion] {}: success={}", result.operation, result.success);
    }));

    proactor.async_read("/var/log/app.log", Box::new(|result| {
        println!("  [Completion] {}: {}", result.operation, result.data);
    }));

    thread::sleep(Duration::from_millis(500));
    drop(proactor);
    println!("All async operations completed.");
}
