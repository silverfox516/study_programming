// Saga Pattern in Rust
// Orchestrate distributed transactions with compensating actions

use std::fmt;

struct SagaStep {
    name: String,
    execute: Box<dyn Fn() -> Result<(), String>>,
    compensate: Box<dyn Fn()>,
}

struct Saga {
    steps: Vec<SagaStep>,
}

impl Saga {
    fn new() -> Self {
        Saga { steps: Vec::new() }
    }

    fn add_step<E, C>(&mut self, name: &str, execute: E, compensate: C)
    where
        E: Fn() -> Result<(), String> + 'static,
        C: Fn() + 'static,
    {
        self.steps.push(SagaStep {
            name: name.to_string(),
            execute: Box::new(execute),
            compensate: Box::new(compensate),
        });
    }

    fn execute(&self) -> Result<(), SagaError> {
        let mut completed = Vec::new();

        for (i, step) in self.steps.iter().enumerate() {
            println!("[Saga] Executing step: {}", step.name);
            match (step.execute)() {
                Ok(()) => {
                    println!("[Saga] Step '{}' succeeded", step.name);
                    completed.push(i);
                }
                Err(e) => {
                    println!("[Saga] Step '{}' FAILED: {}", step.name, e);
                    // Compensate in reverse order
                    println!("[Saga] Starting compensation...");
                    for &idx in completed.iter().rev() {
                        println!("[Saga] Compensating: {}", self.steps[idx].name);
                        (self.steps[idx].compensate)();
                    }
                    return Err(SagaError {
                        failed_step: step.name.clone(),
                        reason: e,
                    });
                }
            }
        }
        println!("[Saga] All steps completed successfully.");
        Ok(())
    }
}

#[derive(Debug)]
struct SagaError {
    failed_step: String,
    reason: String,
}

impl fmt::Display for SagaError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Saga failed at '{}': {}", self.failed_step, self.reason)
    }
}

fn main() {
    // Successful saga
    println!("=== Successful Saga ===");
    let mut saga = Saga::new();
    saga.add_step(
        "Reserve Inventory",
        || { println!("  -> Reserved 5 items"); Ok(()) },
        || println!("  <- Released reserved items"),
    );
    saga.add_step(
        "Charge Payment",
        || { println!("  -> Charged $99.99"); Ok(()) },
        || println!("  <- Refunded $99.99"),
    );
    saga.add_step(
        "Ship Order",
        || { println!("  -> Shipped via FedEx"); Ok(()) },
        || println!("  <- Cancelled shipment"),
    );
    let _ = saga.execute();

    // Failing saga (payment fails)
    println!("\n=== Failing Saga ===");
    let mut saga2 = Saga::new();
    saga2.add_step(
        "Reserve Inventory",
        || { println!("  -> Reserved 3 items"); Ok(()) },
        || println!("  <- Released reserved items"),
    );
    saga2.add_step(
        "Charge Payment",
        || Err("Insufficient funds".to_string()),
        || println!("  <- Refunded payment"),
    );
    saga2.add_step(
        "Ship Order",
        || { println!("  -> Shipped"); Ok(()) },
        || println!("  <- Cancelled shipment"),
    );
    if let Err(e) = saga2.execute() {
        println!("\nSaga result: {}", e);
    }
}
