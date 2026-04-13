// Leader-Followers Pattern in Rust
// One thread leads (waits for events), others follow; role rotates

use std::sync::{Arc, Condvar, Mutex};
use std::thread;
use std::time::Duration;

struct LeaderFollowers {
    state: Mutex<LFState>,
    leader_cv: Condvar,
}

struct LFState {
    current_leader: Option<usize>,
    events: Vec<String>,
    shutdown: bool,
}

impl LeaderFollowers {
    fn new() -> Self {
        LeaderFollowers {
            state: Mutex::new(LFState {
                current_leader: None,
                events: Vec::new(),
                shutdown: false,
            }),
            leader_cv: Condvar::new(),
        }
    }

    fn run_worker(&self, id: usize) {
        loop {
            // Compete to become leader
            let event = {
                let mut state = self.state.lock().unwrap();

                // Wait until no leader or shutdown
                while state.current_leader.is_some() && !state.shutdown {
                    state = self.leader_cv.wait(state).unwrap();
                }
                if state.shutdown {
                    break;
                }

                // Become leader
                state.current_leader = Some(id);
                println!("[Worker {}] Became LEADER", id);

                // Wait for event
                while state.events.is_empty() && !state.shutdown {
                    state = self.leader_cv.wait(state).unwrap();
                }
                if state.shutdown {
                    break;
                }

                let event = state.events.remove(0);
                // Demote self, promote next follower
                state.current_leader = None;
                self.leader_cv.notify_one();
                event
            };

            // Process event as worker
            println!("[Worker {}] Processing: {}", id, event);
            thread::sleep(Duration::from_millis(50));
        }
    }

    fn submit(&self, event: String) {
        let mut state = self.state.lock().unwrap();
        state.events.push(event);
        self.leader_cv.notify_all();
    }

    fn shutdown(&self) {
        let mut state = self.state.lock().unwrap();
        state.shutdown = true;
        self.leader_cv.notify_all();
    }
}

fn main() {
    let lf = Arc::new(LeaderFollowers::new());

    let workers: Vec<_> = (0..3)
        .map(|id| {
            let lf = Arc::clone(&lf);
            thread::spawn(move || lf.run_worker(id))
        })
        .collect();

    thread::sleep(Duration::from_millis(50));

    for i in 0..6 {
        lf.submit(format!("Event-{}", i));
        thread::sleep(Duration::from_millis(30));
    }

    thread::sleep(Duration::from_millis(300));
    lf.shutdown();

    for w in workers {
        w.join().unwrap();
    }
    println!("All workers finished.");
}
