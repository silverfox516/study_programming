use std::time::{Duration, Instant};
use std::error::Error;
use std::sync::{Arc, Mutex};
use std::thread;

#[derive(Debug, PartialEq, Clone, Copy)]
enum State {
    Closed,
    Open,
    HalfOpen,
}

struct CircuitBreaker {
    state: State,
    failure_count: u32,
    failure_threshold: u32,
    last_failure: Option<Instant>,
    recovery_timeout: Duration,
}

impl CircuitBreaker {
    fn new(threshold: u32, timeout: Duration) -> Self {
        CircuitBreaker {
            state: State::Closed,
            failure_count: 0,
            failure_threshold: threshold,
            last_failure: None,
            recovery_timeout: timeout,
        }
    }

    fn call<F, T, E>(&mut self, f: F) -> Result<T, String>
    where
        F: FnOnce() -> Result<T, E>,
        E: std::fmt::Display,
    {
        // Check State logic
        if self.state == State::Open {
            if let Some(last) = self.last_failure {
                if last.elapsed() > self.recovery_timeout {
                    println!("[Circuit] Timeout passed. Switching to HALF-OPEN.");
                    self.state = State::HalfOpen;
                } else {
                    return Err("Circuit is OPEN. Fast fail.".to_string());
                }
            }
        }

        // Execute logic
        match f() {
            Ok(val) => {
                if self.state == State::HalfOpen {
                    println!("[Circuit] Success in HALF-OPEN. Resetting to CLOSED.");
                    self.reset();
                } else if self.failure_count > 0 {
                    self.failure_count = 0;
                }
                Ok(val)
            }
            Err(e) => {
                self.record_failure();
                Err(format!("Service execution failed: {}", e))
            }
        }
    }

    fn record_failure(&mut self) {
        self.failure_count += 1;
        self.last_failure = Some(Instant::now());
        println!("[Circuit] Failure recorded. Count: {}", self.failure_count);

        if self.state == State::HalfOpen {
            self.state = State::Open;
            println!("[Circuit] Failed in HALF-OPEN. Re-opening.");
        } else if self.failure_count >= self.failure_threshold {
            self.state = State::Open;
            println!("[Circuit] Threshold reached. Opening circuit.");
        }
    }

    fn reset(&mut self) {
        self.state = State::Closed;
        self.failure_count = 0;
        self.last_failure = None;
    }
}

// Demo function
fn unreliable_service(should_fail: bool) -> Result<String, &'static str> {
    if should_fail {
        Err("Connection Refused")
    } else {
        Ok("Data Payload".to_string())
    }
}

fn main() {
    println!("=== Circuit Breaker Pattern (Rust) ===");

    let cb = Arc::new(Mutex::new(CircuitBreaker::new(3, Duration::from_secs(2))));

    // Helper to run requests
    let run_request = |fail: bool| {
        let mut breaker = cb.lock().unwrap();
        match breaker.call(|| unreliable_service(fail)) {
            Ok(data) => println!("Success: {}", data),
            Err(e) => println!("Error: {}", e),
        }
    };

    println!("\n1. Normal Calls");
    run_request(false);
    
    println!("\n2. Failures");
    run_request(true);
    run_request(true);
    run_request(true); // Trips here

    println!("\n3. Open State (Fast Fail)");
    run_request(false); // Even if true (healthy), it blocks

    println!("\n... Waiting for timeout (2s) ...");
    thread::sleep(Duration::from_secs(2));

    println!("\n4. Recovery (Half-Open)");
    run_request(false); // Should succeed and close
    
    println!("\n5. Back to Normal");
    run_request(false);
}
