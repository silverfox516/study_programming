// Thread Pool Pattern in Rust
// Fixed pool of workers that process jobs from a shared queue

use std::sync::{mpsc, Arc, Mutex};
use std::thread;

type Job = Box<dyn FnOnce() + Send + 'static>;

struct ThreadPool {
    workers: Vec<thread::JoinHandle<()>>,
    sender: Option<mpsc::Sender<Job>>,
}

impl ThreadPool {
    fn new(size: usize) -> Self {
        let (tx, rx) = mpsc::channel::<Job>();
        let rx = Arc::new(Mutex::new(rx));

        let workers: Vec<_> = (0..size)
            .map(|id| {
                let rx = Arc::clone(&rx);
                thread::spawn(move || loop {
                    let job = rx.lock().unwrap().recv();
                    match job {
                        Ok(job) => {
                            println!("[Worker {}] Executing job", id);
                            job();
                        }
                        Err(_) => {
                            println!("[Worker {}] Shutting down", id);
                            break;
                        }
                    }
                })
            })
            .collect();

        ThreadPool {
            workers,
            sender: Some(tx),
        }
    }

    fn execute<F>(&self, f: F)
    where
        F: FnOnce() + Send + 'static,
    {
        if let Some(ref sender) = self.sender {
            sender.send(Box::new(f)).unwrap();
        }
    }

    fn shutdown(&mut self) {
        // Drop sender to signal workers
        self.sender.take();
        for worker in self.workers.drain(..) {
            worker.join().unwrap();
        }
    }
}

fn main() {
    let mut pool = ThreadPool::new(3);

    for i in 0..8 {
        pool.execute(move || {
            println!("  Task {} running on {:?}", i, thread::current().id());
            thread::sleep(std::time::Duration::from_millis(100));
        });
    }

    thread::sleep(std::time::Duration::from_millis(500));
    pool.shutdown();
    println!("All tasks completed.");
}
