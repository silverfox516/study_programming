// Transaction Script Pattern in Rust
// Organize business logic as procedural scripts per transaction type

use std::collections::HashMap;

struct Account {
    id: u32,
    name: String,
    balance: f64,
}

struct AccountStore {
    accounts: HashMap<u32, Account>,
}

impl AccountStore {
    fn new() -> Self {
        AccountStore { accounts: HashMap::new() }
    }

    fn add(&mut self, id: u32, name: &str, balance: f64) {
        self.accounts.insert(id, Account { id, name: name.to_string(), balance });
    }

    fn get(&self, id: u32) -> Option<&Account> {
        self.accounts.get(&id)
    }

    fn get_mut(&mut self, id: u32) -> Option<&mut Account> {
        self.accounts.get_mut(&id)
    }
}

// Transaction Scripts — each function is one complete transaction
fn transfer(store: &mut AccountStore, from: u32, to: u32, amount: f64) -> Result<(), String> {
    println!("[Transfer] ${:.2} from #{} to #{}", amount, from, to);

    // Validate
    let from_balance = store.get(from)
        .ok_or_else(|| format!("Account #{} not found", from))?
        .balance;

    if from_balance < amount {
        return Err(format!("Insufficient funds: {:.2} < {:.2}", from_balance, amount));
    }

    if store.get(to).is_none() {
        return Err(format!("Account #{} not found", to));
    }

    // Execute
    store.get_mut(from).unwrap().balance -= amount;
    store.get_mut(to).unwrap().balance += amount;

    println!("[Transfer] Success");
    Ok(())
}

fn apply_interest(store: &mut AccountStore, rate: f64) {
    println!("[Interest] Applying {:.1}% interest", rate * 100.0);
    for account in store.accounts.values_mut() {
        let interest = account.balance * rate;
        account.balance += interest;
        println!("  #{} {}: +{:.2} -> {:.2}", account.id, account.name, interest, account.balance);
    }
}

fn close_account(store: &mut AccountStore, id: u32, target_id: u32) -> Result<(), String> {
    println!("[Close] Account #{}", id);
    let balance = store.get(id)
        .ok_or_else(|| format!("Account #{} not found", id))?
        .balance;

    if balance > 0.0 {
        transfer(store, id, target_id, balance)?;
    }

    store.accounts.remove(&id);
    println!("[Close] Account #{} closed", id);
    Ok(())
}

fn print_balances(store: &AccountStore) {
    println!("\n--- Balances ---");
    for acc in store.accounts.values() {
        println!("  #{} {}: ${:.2}", acc.id, acc.name, acc.balance);
    }
    println!();
}

fn main() {
    let mut store = AccountStore::new();
    store.add(1, "Alice", 1000.0);
    store.add(2, "Bob", 500.0);
    store.add(3, "Charlie", 250.0);

    print_balances(&store);

    transfer(&mut store, 1, 2, 200.0).unwrap();
    print_balances(&store);

    apply_interest(&mut store, 0.05);
    print_balances(&store);

    close_account(&mut store, 3, 1).unwrap();
    print_balances(&store);

    // Failing transfer
    if let Err(e) = transfer(&mut store, 2, 1, 9999.0) {
        println!("Transfer failed: {}", e);
    }
}
