// Throttling Pattern in Rust
// Rate limit operations to prevent overload

use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};

struct RateLimiter {
    max_requests: u32,
    window: Duration,
    state: Mutex<LimiterState>,
}

struct LimiterState {
    request_count: u32,
    window_start: Instant,
}

impl RateLimiter {
    fn new(max_requests: u32, window: Duration) -> Self {
        RateLimiter {
            max_requests,
            window,
            state: Mutex::new(LimiterState {
                request_count: 0,
                window_start: Instant::now(),
            }),
        }
    }

    fn try_acquire(&self) -> bool {
        let mut state = self.state.lock().unwrap();
        let now = Instant::now();

        // Reset window if expired
        if now.duration_since(state.window_start) >= self.window {
            state.request_count = 0;
            state.window_start = now;
        }

        if state.request_count < self.max_requests {
            state.request_count += 1;
            true
        } else {
            false
        }
    }

    fn execute<F, T>(&self, f: F) -> Result<T, String>
    where
        F: FnOnce() -> T,
    {
        if self.try_acquire() {
            Ok(f())
        } else {
            Err("Rate limit exceeded. Try again later.".to_string())
        }
    }
}

fn main() {
    let limiter = Arc::new(RateLimiter::new(3, Duration::from_millis(200)));

    println!("=== Rate Limiting (3 req / 200ms) ===");

    // Send 5 rapid requests
    for i in 0..5 {
        match limiter.execute(|| format!("Response {}", i)) {
            Ok(r) => println!("[Request {}] {}", i, r),
            Err(e) => println!("[Request {}] REJECTED: {}", i, e),
        }
    }

    // Wait for window reset
    println!("\n[Waiting 250ms for window reset...]");
    std::thread::sleep(Duration::from_millis(250));

    // Should work again
    for i in 5..8 {
        match limiter.execute(|| format!("Response {}", i)) {
            Ok(r) => println!("[Request {}] {}", i, r),
            Err(e) => println!("[Request {}] REJECTED: {}", i, e),
        }
    }
}
