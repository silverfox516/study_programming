// Half-Sync/Half-Async Pattern in Rust
// Async layer queues work, sync layer processes it

use std::sync::mpsc;
use std::thread;
use std::time::Duration;

#[derive(Debug)]
struct Request {
    id: u32,
    payload: String,
}

#[derive(Debug)]
struct Response {
    request_id: u32,
    result: String,
}

// Async layer: accepts requests without blocking
struct AsyncLayer {
    sender: mpsc::Sender<Request>,
}

impl AsyncLayer {
    fn submit(&self, req: Request) {
        println!("[Async] Queuing request {}", req.id);
        self.sender.send(req).unwrap();
    }
}

// Sync layer: processes requests synchronously in worker threads
fn sync_worker(id: usize, rx: mpsc::Receiver<Request>, result_tx: mpsc::Sender<Response>) {
    while let Ok(req) = rx.recv() {
        println!("[Sync Worker {}] Processing request {}", id, req.id);
        // Simulate blocking work
        thread::sleep(Duration::from_millis(100));
        let resp = Response {
            request_id: req.id,
            result: format!("Processed '{}' by worker {}", req.payload, id),
        };
        result_tx.send(resp).unwrap();
    }
}

fn main() {
    let (req_tx, req_rx) = mpsc::channel::<Request>();
    let (res_tx, res_rx) = mpsc::channel::<Response>();

    let async_layer = AsyncLayer { sender: req_tx };

    // Start sync workers
    let req_rx = std::sync::Arc::new(std::sync::Mutex::new(req_rx));
    let workers: Vec<_> = (0..2)
        .map(|id| {
            let rx = std::sync::Arc::clone(&req_rx);
            let tx = res_tx.clone();
            thread::spawn(move || {
                while let Ok(req) = rx.lock().unwrap().recv() {
                    println!("[Sync Worker {}] Processing request {}", id, req.id);
                    thread::sleep(Duration::from_millis(80));
                    tx.send(Response {
                        request_id: req.id,
                        result: format!("Done '{}' by worker {}", req.payload, id),
                    })
                    .unwrap();
                }
            })
        })
        .collect();
    drop(res_tx);

    // Async layer submits requests
    for i in 0..5 {
        async_layer.submit(Request {
            id: i,
            payload: format!("task-{}", i),
        });
    }
    drop(async_layer);

    // Collect results
    while let Ok(resp) = res_rx.recv() {
        println!("[Result] Request {}: {}", resp.request_id, resp.result);
    }

    for w in workers {
        w.join().unwrap();
    }
    println!("All done.");
}
