// Active Object Pattern in Rust
// Decouples method execution from invocation using async message passing

use std::sync::mpsc;
use std::thread;

enum Command {
    Add(f64, f64, mpsc::Sender<f64>),
    Multiply(f64, f64, mpsc::Sender<f64>),
    Shutdown,
}

struct ActiveCalculator {
    sender: mpsc::Sender<Command>,
    handle: Option<thread::JoinHandle<()>>,
}

impl ActiveCalculator {
    fn new() -> Self {
        let (tx, rx) = mpsc::channel::<Command>();

        let handle = thread::spawn(move || {
            println!("[ActiveObject] Started");
            while let Ok(cmd) = rx.recv() {
                match cmd {
                    Command::Add(a, b, reply) => {
                        println!("[ActiveObject] Computing {} + {}", a, b);
                        let _ = reply.send(a + b);
                    }
                    Command::Multiply(a, b, reply) => {
                        println!("[ActiveObject] Computing {} * {}", a, b);
                        let _ = reply.send(a * b);
                    }
                    Command::Shutdown => {
                        println!("[ActiveObject] Shutting down");
                        break;
                    }
                }
            }
        });

        ActiveCalculator {
            sender: tx,
            handle: Some(handle),
        }
    }

    fn add(&self, a: f64, b: f64) -> f64 {
        let (tx, rx) = mpsc::channel();
        self.sender.send(Command::Add(a, b, tx)).unwrap();
        rx.recv().unwrap()
    }

    fn multiply(&self, a: f64, b: f64) -> f64 {
        let (tx, rx) = mpsc::channel();
        self.sender.send(Command::Multiply(a, b, tx)).unwrap();
        rx.recv().unwrap()
    }

    fn shutdown(mut self) {
        let _ = self.sender.send(Command::Shutdown);
        if let Some(h) = self.handle.take() {
            h.join().unwrap();
        }
    }
}

fn main() {
    let calc = ActiveCalculator::new();

    println!("3 + 4 = {}", calc.add(3.0, 4.0));
    println!("5 * 6 = {}", calc.multiply(5.0, 6.0));
    println!("10 + 20 = {}", calc.add(10.0, 20.0));

    calc.shutdown();
}
