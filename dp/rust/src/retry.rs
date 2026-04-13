// Retry Pattern in Rust
// Automatically retry failed operations with configurable strategy

use std::thread;
use std::time::Duration;

#[derive(Debug)]
struct RetryConfig {
    max_attempts: u32,
    initial_delay: Duration,
    backoff_factor: f64,
}

impl RetryConfig {
    fn fixed(attempts: u32, delay: Duration) -> Self {
        RetryConfig {
            max_attempts: attempts,
            initial_delay: delay,
            backoff_factor: 1.0,
        }
    }

    fn exponential(attempts: u32, initial: Duration) -> Self {
        RetryConfig {
            max_attempts: attempts,
            initial_delay: initial,
            backoff_factor: 2.0,
        }
    }
}

fn retry<F, T, E>(config: &RetryConfig, mut operation: F) -> Result<T, E>
where
    F: FnMut(u32) -> Result<T, E>,
    E: std::fmt::Display,
{
    let mut delay = config.initial_delay;

    for attempt in 1..=config.max_attempts {
        match operation(attempt) {
            Ok(val) => return Ok(val),
            Err(e) => {
                println!("[Retry] Attempt {}/{} failed: {}", attempt, config.max_attempts, e);
                if attempt < config.max_attempts {
                    println!("[Retry] Waiting {:?}...", delay);
                    thread::sleep(delay);
                    delay = Duration::from_secs_f64(delay.as_secs_f64() * config.backoff_factor);
                }
            }
        }
    }
    operation(config.max_attempts) // Final attempt
}

fn main() {
    // Fixed retry
    println!("=== Fixed Retry ===");
    let mut call_count = 0;
    let result = retry(&RetryConfig::fixed(3, Duration::from_millis(50)), |attempt| {
        call_count += 1;
        if attempt < 3 {
            Err(format!("Connection refused (attempt {})", attempt))
        } else {
            Ok("Connected!")
        }
    });
    println!("Result: {:?}\n", result);

    // Exponential backoff
    println!("=== Exponential Backoff ===");
    let result = retry(
        &RetryConfig::exponential(4, Duration::from_millis(10)),
        |attempt| -> Result<i32, String> {
            if attempt < 4 {
                Err(format!("Timeout (attempt {})", attempt))
            } else {
                Ok(42)
            }
        },
    );
    println!("Result: {:?}", result);
}
