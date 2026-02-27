/// Event Sourcing Pattern Implementation in Rust
///
/// Event Sourcing stores the state of a business entity as a sequence of state-changing events.
/// Instead of storing just the current state, all changes are stored as events that can be
/// replayed to reconstruct the current state.

use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use uuid::Uuid;

// Event trait
pub trait Event: Clone + std::fmt::Debug {
    fn event_type(&self) -> &'static str;
    fn aggregate_id(&self) -> Uuid;
    fn timestamp(&self) -> chrono::DateTime<chrono::Utc>;
}

// Bank account events
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum BankAccountEvent {
    AccountOpened {
        account_id: Uuid,
        owner_name: String,
        initial_balance: f64,
        timestamp: chrono::DateTime<chrono::Utc>,
    },
    MoneyDeposited {
        account_id: Uuid,
        amount: f64,
        timestamp: chrono::DateTime<chrono::Utc>,
    },
    MoneyWithdrawn {
        account_id: Uuid,
        amount: f64,
        timestamp: chrono::DateTime<chrono::Utc>,
    },
    AccountClosed {
        account_id: Uuid,
        final_balance: f64,
        timestamp: chrono::DateTime<chrono::Utc>,
    },
}

impl Event for BankAccountEvent {
    fn event_type(&self) -> &'static str {
        match self {
            BankAccountEvent::AccountOpened { .. } => "AccountOpened",
            BankAccountEvent::MoneyDeposited { .. } => "MoneyDeposited",
            BankAccountEvent::MoneyWithdrawn { .. } => "MoneyWithdrawn",
            BankAccountEvent::AccountClosed { .. } => "AccountClosed",
        }
    }

    fn aggregate_id(&self) -> Uuid {
        match self {
            BankAccountEvent::AccountOpened { account_id, .. } => *account_id,
            BankAccountEvent::MoneyDeposited { account_id, .. } => *account_id,
            BankAccountEvent::MoneyWithdrawn { account_id, .. } => *account_id,
            BankAccountEvent::AccountClosed { account_id, .. } => *account_id,
        }
    }

    fn timestamp(&self) -> chrono::DateTime<chrono::Utc> {
        match self {
            BankAccountEvent::AccountOpened { timestamp, .. } => *timestamp,
            BankAccountEvent::MoneyDeposited { timestamp, .. } => *timestamp,
            BankAccountEvent::MoneyWithdrawn { timestamp, .. } => *timestamp,
            BankAccountEvent::AccountClosed { timestamp, .. } => *timestamp,
        }
    }
}

// Aggregate (current state)
#[derive(Debug, Clone)]
pub struct BankAccount {
    pub id: Uuid,
    pub owner_name: String,
    pub balance: f64,
    pub is_closed: bool,
    pub version: usize, // For optimistic concurrency control
}

impl BankAccount {
    pub fn new() -> Self {
        Self {
            id: Uuid::new_v4(),
            owner_name: String::new(),
            balance: 0.0,
            is_closed: false,
            version: 0,
        }
    }

    // Apply an event to update the state
    pub fn apply_event(&mut self, event: &BankAccountEvent) {
        match event {
            BankAccountEvent::AccountOpened {
                account_id,
                owner_name,
                initial_balance,
                ..
            } => {
                self.id = *account_id;
                self.owner_name = owner_name.clone();
                self.balance = *initial_balance;
                self.is_closed = false;
            }
            BankAccountEvent::MoneyDeposited { amount, .. } => {
                self.balance += amount;
            }
            BankAccountEvent::MoneyWithdrawn { amount, .. } => {
                self.balance -= amount;
            }
            BankAccountEvent::AccountClosed { .. } => {
                self.is_closed = true;
            }
        }
        self.version += 1;
    }

    // Reconstruct state from events
    pub fn from_events(events: &[BankAccountEvent]) -> Self {
        let mut account = Self::new();
        for event in events {
            account.apply_event(event);
        }
        account
    }
}

// Command (intent to change state)
#[derive(Debug, Clone)]
pub enum BankAccountCommand {
    OpenAccount {
        account_id: Uuid,
        owner_name: String,
        initial_balance: f64,
    },
    DepositMoney {
        account_id: Uuid,
        amount: f64,
    },
    WithdrawMoney {
        account_id: Uuid,
        amount: f64,
    },
    CloseAccount {
        account_id: Uuid,
    },
}

// Event Store trait
pub trait EventStore {
    type Error;

    fn append_events(&mut self, aggregate_id: Uuid, events: Vec<BankAccountEvent>) -> Result<(), Self::Error>;
    fn get_events(&self, aggregate_id: Uuid) -> Result<Vec<BankAccountEvent>, Self::Error>;
    fn get_all_events(&self) -> Result<Vec<BankAccountEvent>, Self::Error>;
}

// In-memory event store
#[derive(Debug, Default)]
pub struct InMemoryEventStore {
    events: HashMap<Uuid, Vec<BankAccountEvent>>,
}

impl InMemoryEventStore {
    pub fn new() -> Self {
        Self {
            events: HashMap::new(),
        }
    }
}

impl EventStore for InMemoryEventStore {
    type Error = String;

    fn append_events(&mut self, aggregate_id: Uuid, events: Vec<BankAccountEvent>) -> Result<(), Self::Error> {
        let event_list = self.events.entry(aggregate_id).or_insert_with(Vec::new);
        event_list.extend(events);
        Ok(())
    }

    fn get_events(&self, aggregate_id: Uuid) -> Result<Vec<BankAccountEvent>, Self::Error> {
        Ok(self.events.get(&aggregate_id).cloned().unwrap_or_default())
    }

    fn get_all_events(&self) -> Result<Vec<BankAccountEvent>, Self::Error> {
        let mut all_events = Vec::new();
        for events in self.events.values() {
            all_events.extend(events.clone());
        }

        // Sort by timestamp
        all_events.sort_by(|a, b| a.timestamp().cmp(&b.timestamp()));
        Ok(all_events)
    }
}

// Command Handler
pub struct BankAccountCommandHandler<E: EventStore> {
    event_store: E,
}

impl<E: EventStore> BankAccountCommandHandler<E> {
    pub fn new(event_store: E) -> Self {
        Self { event_store }
    }

    pub fn handle_command(&mut self, command: BankAccountCommand) -> Result<Vec<BankAccountEvent>, E::Error> {
        let events = match command {
            BankAccountCommand::OpenAccount {
                account_id,
                owner_name,
                initial_balance,
            } => {
                // Validate command
                if initial_balance < 0.0 {
                    return Err("Initial balance cannot be negative".to_string().into());
                }

                // Check if account already exists
                let existing_events = self.event_store.get_events(account_id)?;
                if !existing_events.is_empty() {
                    return Err("Account already exists".to_string().into());
                }

                vec![BankAccountEvent::AccountOpened {
                    account_id,
                    owner_name,
                    initial_balance,
                    timestamp: chrono::Utc::now(),
                }]
            }
            BankAccountCommand::DepositMoney { account_id, amount } => {
                // Validate command
                if amount <= 0.0 {
                    return Err("Deposit amount must be positive".to_string().into());
                }

                // Load current state
                let events = self.event_store.get_events(account_id)?;
                if events.is_empty() {
                    return Err("Account does not exist".to_string().into());
                }

                let account = BankAccount::from_events(&events);
                if account.is_closed {
                    return Err("Cannot deposit to closed account".to_string().into());
                }

                vec![BankAccountEvent::MoneyDeposited {
                    account_id,
                    amount,
                    timestamp: chrono::Utc::now(),
                }]
            }
            BankAccountCommand::WithdrawMoney { account_id, amount } => {
                // Validate command
                if amount <= 0.0 {
                    return Err("Withdrawal amount must be positive".to_string().into());
                }

                // Load current state
                let events = self.event_store.get_events(account_id)?;
                if events.is_empty() {
                    return Err("Account does not exist".to_string().into());
                }

                let account = BankAccount::from_events(&events);
                if account.is_closed {
                    return Err("Cannot withdraw from closed account".to_string().into());
                }

                if account.balance < amount {
                    return Err("Insufficient funds".to_string().into());
                }

                vec![BankAccountEvent::MoneyWithdrawn {
                    account_id,
                    amount,
                    timestamp: chrono::Utc::now(),
                }]
            }
            BankAccountCommand::CloseAccount { account_id } => {
                // Load current state
                let events = self.event_store.get_events(account_id)?;
                if events.is_empty() {
                    return Err("Account does not exist".to_string().into());
                }

                let account = BankAccount::from_events(&events);
                if account.is_closed {
                    return Err("Account is already closed".to_string().into());
                }

                vec![BankAccountEvent::AccountClosed {
                    account_id,
                    final_balance: account.balance,
                    timestamp: chrono::Utc::now(),
                }]
            }
        };

        // Store events
        let aggregate_id = events[0].aggregate_id();
        self.event_store.append_events(aggregate_id, events.clone())?;

        Ok(events)
    }

    pub fn get_account_state(&self, account_id: Uuid) -> Result<Option<BankAccount>, E::Error> {
        let events = self.event_store.get_events(account_id)?;
        if events.is_empty() {
            Ok(None)
        } else {
            Ok(Some(BankAccount::from_events(&events)))
        }
    }
}

// Read model (projection)
#[derive(Debug, Clone)]
pub struct AccountSummary {
    pub id: Uuid,
    pub owner_name: String,
    pub balance: f64,
    pub total_deposits: f64,
    pub total_withdrawals: f64,
    pub transaction_count: usize,
    pub is_closed: bool,
}

impl AccountSummary {
    pub fn from_events(events: &[BankAccountEvent]) -> Self {
        let mut summary = AccountSummary {
            id: Uuid::nil(),
            owner_name: String::new(),
            balance: 0.0,
            total_deposits: 0.0,
            total_withdrawals: 0.0,
            transaction_count: 0,
            is_closed: false,
        };

        for event in events {
            match event {
                BankAccountEvent::AccountOpened {
                    account_id,
                    owner_name,
                    initial_balance,
                    ..
                } => {
                    summary.id = *account_id;
                    summary.owner_name = owner_name.clone();
                    summary.balance = *initial_balance;
                    summary.total_deposits = *initial_balance;
                    summary.transaction_count += 1;
                }
                BankAccountEvent::MoneyDeposited { amount, .. } => {
                    summary.balance += amount;
                    summary.total_deposits += amount;
                    summary.transaction_count += 1;
                }
                BankAccountEvent::MoneyWithdrawn { amount, .. } => {
                    summary.balance -= amount;
                    summary.total_withdrawals += amount;
                    summary.transaction_count += 1;
                }
                BankAccountEvent::AccountClosed { .. } => {
                    summary.is_closed = true;
                    summary.transaction_count += 1;
                }
            }
        }

        summary
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_event_sourcing_flow() {
        let mut event_store = InMemoryEventStore::new();
        let mut command_handler = BankAccountCommandHandler::new(&mut event_store);

        let account_id = Uuid::new_v4();

        // Open account
        let events = command_handler
            .handle_command(BankAccountCommand::OpenAccount {
                account_id,
                owner_name: "John Doe".to_string(),
                initial_balance: 100.0,
            })
            .unwrap();

        assert_eq!(events.len(), 1);
        assert!(matches!(events[0], BankAccountEvent::AccountOpened { .. }));

        // Deposit money
        command_handler
            .handle_command(BankAccountCommand::DepositMoney {
                account_id,
                amount: 50.0,
            })
            .unwrap();

        // Check current state
        let account = command_handler.get_account_state(account_id).unwrap().unwrap();
        assert_eq!(account.balance, 150.0);
        assert_eq!(account.owner_name, "John Doe");
    }

    #[test]
    fn test_insufficient_funds() {
        let mut event_store = InMemoryEventStore::new();
        let mut command_handler = BankAccountCommandHandler::new(&mut event_store);

        let account_id = Uuid::new_v4();

        // Open account with small balance
        command_handler
            .handle_command(BankAccountCommand::OpenAccount {
                account_id,
                owner_name: "Jane Doe".to_string(),
                initial_balance: 10.0,
            })
            .unwrap();

        // Try to withdraw more than balance
        let result = command_handler.handle_command(BankAccountCommand::WithdrawMoney {
            account_id,
            amount: 20.0,
        });

        assert!(result.is_err());
        assert!(result.unwrap_err().to_string().contains("Insufficient funds"));
    }
}

fn main() {
    println!("=== Event Sourcing Pattern Demo ===\n");

    let mut event_store = InMemoryEventStore::new();
    let mut command_handler = BankAccountCommandHandler::new(&mut event_store);

    // Create some bank accounts
    let account1_id = Uuid::new_v4();
    let account2_id = Uuid::new_v4();

    println!("1. Opening bank accounts:");

    // Open first account
    match command_handler.handle_command(BankAccountCommand::OpenAccount {
        account_id: account1_id,
        owner_name: "Alice Johnson".to_string(),
        initial_balance: 1000.0,
    }) {
        Ok(_) => println!("✓ Account opened for Alice Johnson"),
        Err(e) => println!("✗ Failed to open account: {:?}", e),
    }

    // Open second account
    match command_handler.handle_command(BankAccountCommand::OpenAccount {
        account_id: account2_id,
        owner_name: "Bob Smith".to_string(),
        initial_balance: 500.0,
    }) {
        Ok(_) => println!("✓ Account opened for Bob Smith"),
        Err(e) => println!("✗ Failed to open account: {:?}", e),
    }

    println!("\n2. Performing transactions:");

    // Alice deposits money
    match command_handler.handle_command(BankAccountCommand::DepositMoney {
        account_id: account1_id,
        amount: 250.0,
    }) {
        Ok(_) => println!("✓ Alice deposited $250"),
        Err(e) => println!("✗ Deposit failed: {:?}", e),
    }

    // Bob withdraws money
    match command_handler.handle_command(BankAccountCommand::WithdrawMoney {
        account_id: account2_id,
        amount: 100.0,
    }) {
        Ok(_) => println!("✓ Bob withdrew $100"),
        Err(e) => println!("✗ Withdrawal failed: {:?}", e),
    }

    // Alice makes another deposit
    match command_handler.handle_command(BankAccountCommand::DepositMoney {
        account_id: account1_id,
        amount: 500.0,
    }) {
        Ok(_) => println!("✓ Alice deposited $500"),
        Err(e) => println!("✗ Deposit failed: {:?}", e),
    }

    println!("\n3. Current account states:");

    // Show current states
    match command_handler.get_account_state(account1_id) {
        Ok(Some(account)) => {
            println!("Alice's account: ${:.2} ({})", account.balance,
                    if account.is_closed { "Closed" } else { "Open" });
        }
        Ok(None) => println!("Alice's account not found"),
        Err(e) => println!("Error getting Alice's account: {:?}", e),
    }

    match command_handler.get_account_state(account2_id) {
        Ok(Some(account)) => {
            println!("Bob's account: ${:.2} ({})", account.balance,
                    if account.is_closed { "Closed" } else { "Open" });
        }
        Ok(None) => println!("Bob's account not found"),
        Err(e) => println!("Error getting Bob's account: {:?}", e),
    }

    println!("\n4. Event history (Alice's account):");

    if let Ok(events) = event_store.get_events(account1_id) {
        for (i, event) in events.iter().enumerate() {
            println!("Event {}: {} at {}",
                    i + 1,
                    event.event_type(),
                    event.timestamp().format("%Y-%m-%d %H:%M:%S UTC"));
        }
    }

    println!("\n5. Account summary (projection):");

    if let Ok(events) = event_store.get_events(account1_id) {
        let summary = AccountSummary::from_events(&events);
        println!("Alice's Summary:");
        println!("  Total deposits: ${:.2}", summary.total_deposits);
        println!("  Total withdrawals: ${:.2}", summary.total_withdrawals);
        println!("  Transaction count: {}", summary.transaction_count);
        println!("  Current balance: ${:.2}", summary.balance);
    }

    println!("\n6. Testing business rules:");

    // Try to withdraw more than balance
    match command_handler.handle_command(BankAccountCommand::WithdrawMoney {
        account_id: account2_id,
        amount: 1000.0,
    }) {
        Ok(_) => println!("✓ Large withdrawal succeeded"),
        Err(e) => println!("✗ Large withdrawal failed: {}", e),
    }

    // Close Bob's account
    match command_handler.handle_command(BankAccountCommand::CloseAccount {
        account_id: account2_id,
    }) {
        Ok(_) => println!("✓ Bob's account closed"),
        Err(e) => println!("✗ Failed to close account: {}", e),
    }

    // Try to deposit to closed account
    match command_handler.handle_command(BankAccountCommand::DepositMoney {
        account_id: account2_id,
        amount: 50.0,
    }) {
        Ok(_) => println!("✓ Deposit to closed account succeeded"),
        Err(e) => println!("✗ Deposit to closed account failed: {}", e),
    }

    println!("\nEvent Sourcing provides complete audit trail and allows");
    println!("reconstruction of state at any point in time!");
}