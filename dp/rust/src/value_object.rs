// Value Object Pattern in Rust
// Immutable objects defined by their attributes, not identity

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
struct Money {
    amount: i64,       // cents to avoid floating-point
    currency: String,
}

impl Money {
    fn new(amount: i64, currency: &str) -> Self {
        Money {
            amount,
            currency: currency.to_uppercase(),
        }
    }

    fn dollars(amount: f64) -> Self {
        Money::new((amount * 100.0) as i64, "USD")
    }

    fn add(&self, other: &Money) -> Result<Money, String> {
        if self.currency != other.currency {
            return Err(format!("Cannot add {} to {}", self.currency, other.currency));
        }
        Ok(Money::new(self.amount + other.amount, &self.currency))
    }

    fn multiply(&self, factor: i64) -> Money {
        Money::new(self.amount * factor, &self.currency)
    }

    fn display_amount(&self) -> String {
        format!("{}.{:02} {}", self.amount / 100, (self.amount % 100).abs(), self.currency)
    }
}

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
struct Address {
    street: String,
    city: String,
    country: String,
    postal_code: String,
}

impl Address {
    fn new(street: &str, city: &str, country: &str, postal_code: &str) -> Self {
        Address {
            street: street.to_string(),
            city: city.to_string(),
            country: country.to_string(),
            postal_code: postal_code.to_string(),
        }
    }

    fn with_street(&self, street: &str) -> Self {
        Address {
            street: street.to_string(),
            ..self.clone()
        }
    }
}

fn main() {
    // Money value objects
    let price1 = Money::dollars(29.99);
    let price2 = Money::dollars(10.01);
    let total = price1.add(&price2).unwrap();
    println!("{} + {} = {}", price1.display_amount(), price2.display_amount(), total.display_amount());

    // Equality by value
    let a = Money::dollars(10.0);
    let b = Money::dollars(10.0);
    println!("$10 == $10: {}", a == b); // true (same value)

    // Immutability: multiply returns new object
    let doubled = a.multiply(2);
    println!("Original: {}, Doubled: {}", a.display_amount(), doubled.display_amount());

    // Address value object
    let addr = Address::new("123 Main St", "Springfield", "US", "62701");
    let moved = addr.with_street("456 Oak Ave");
    println!("\nOriginal: {:?}", addr);
    println!("New addr: {:?}", moved);
    println!("Same? {}", addr == moved); // false (different street)
}
