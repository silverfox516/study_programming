// Compensating Transaction Pattern in Rust
// Undo completed steps when a multi-step operation fails

use std::fmt;

struct Transaction {
    steps: Vec<Step>,
}

struct Step {
    name: String,
    action: Box<dyn Fn() -> Result<(), String>>,
    compensation: Box<dyn Fn()>,
}

impl Transaction {
    fn new() -> Self {
        Transaction { steps: Vec::new() }
    }

    fn step<A, C>(mut self, name: &str, action: A, compensation: C) -> Self
    where
        A: Fn() -> Result<(), String> + 'static,
        C: Fn() + 'static,
    {
        self.steps.push(Step {
            name: name.to_string(),
            action: Box::new(action),
            compensation: Box::new(compensation),
        });
        self
    }

    fn execute(&self) -> Result<(), TransactionError> {
        let mut completed = Vec::new();

        for step in &self.steps {
            println!("[TX] Executing: {}", step.name);
            match (step.action)() {
                Ok(()) => {
                    completed.push(step);
                }
                Err(e) => {
                    println!("[TX] FAILED at '{}': {}", step.name, e);
                    println!("[TX] Compensating {} completed steps...", completed.len());

                    for step in completed.iter().rev() {
                        println!("[TX] Compensating: {}", step.name);
                        (step.compensation)();
                    }

                    return Err(TransactionError {
                        step: step.name.clone(),
                        cause: e,
                        compensated: completed.iter().map(|s| s.name.clone()).collect(),
                    });
                }
            }
        }

        println!("[TX] All steps completed successfully.");
        Ok(())
    }
}

#[derive(Debug)]
struct TransactionError {
    step: String,
    cause: String,
    compensated: Vec<String>,
}

impl fmt::Display for TransactionError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "Transaction failed at '{}': {}. Compensated: {:?}",
            self.step, self.cause, self.compensated
        )
    }
}

fn main() {
    // Successful transaction
    println!("=== Success Case ===");
    let tx = Transaction::new()
        .step("Debit account", || { println!("  Debited $100"); Ok(()) }, || println!("  Credited $100 back"))
        .step("Reserve stock", || { println!("  Reserved 2 items"); Ok(()) }, || println!("  Released 2 items"))
        .step("Send email", || { println!("  Email sent"); Ok(()) }, || println!("  (email can't be unsent)"));
    let _ = tx.execute();

    // Failing transaction
    println!("\n=== Failure Case ===");
    let tx = Transaction::new()
        .step("Debit account", || { println!("  Debited $50"); Ok(()) }, || println!("  Credited $50 back"))
        .step("Reserve stock", || Err("Out of stock".into()), || println!("  Released stock"))
        .step("Send email", || { println!("  Email sent"); Ok(()) }, || println!("  (not reached)"));

    if let Err(e) = tx.execute() {
        println!("\n{}", e);
    }
}
