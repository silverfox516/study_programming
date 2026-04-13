// Ambassador Pattern in Rust
// Proxy that offloads cross-cutting concerns (logging, retry, metrics)

use std::time::Instant;

trait Service {
    fn call(&self, request: &str) -> Result<String, String>;
}

// Real service
struct RemoteService;

impl Service for RemoteService {
    fn call(&self, request: &str) -> Result<String, String> {
        // Simulate occasional failures
        if request.contains("fail") {
            Err("Service unavailable".into())
        } else {
            Ok(format!("Response for '{}'", request))
        }
    }
}

// Ambassador: wraps the service with cross-cutting concerns
struct Ambassador<S: Service> {
    inner: S,
    name: String,
}

impl<S: Service> Ambassador<S> {
    fn new(inner: S, name: &str) -> Self {
        Ambassador {
            inner,
            name: name.to_string(),
        }
    }
}

impl<S: Service> Service for Ambassador<S> {
    fn call(&self, request: &str) -> Result<String, String> {
        let start = Instant::now();

        // Logging
        println!("[Ambassador:{}] -> Request: {}", self.name, request);

        // Retry logic
        let mut last_err = String::new();
        for attempt in 1..=3 {
            match self.inner.call(request) {
                Ok(response) => {
                    let elapsed = start.elapsed();
                    println!(
                        "[Ambassador:{}] <- Response ({}ms, attempt {}): {}",
                        self.name,
                        elapsed.as_millis(),
                        attempt,
                        response
                    );
                    return Ok(response);
                }
                Err(e) => {
                    println!(
                        "[Ambassador:{}] Attempt {} failed: {}",
                        self.name, attempt, e
                    );
                    last_err = e;
                }
            }
        }

        let elapsed = start.elapsed();
        println!(
            "[Ambassador:{}] All retries exhausted ({}ms)",
            self.name,
            elapsed.as_millis()
        );
        Err(last_err)
    }
}

fn main() {
    let service = Ambassador::new(RemoteService, "api-gw");

    // Successful call
    println!("=== Successful Call ===");
    match service.call("get_users") {
        Ok(r) => println!("Result: {}\n", r),
        Err(e) => println!("Error: {}\n", e),
    }

    // Failing call (retries exhausted)
    println!("=== Failing Call ===");
    match service.call("fail_please") {
        Ok(r) => println!("Result: {}\n", r),
        Err(e) => println!("Error: {}\n", e),
    }
}
