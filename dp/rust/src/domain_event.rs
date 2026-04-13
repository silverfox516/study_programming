// Domain Event Pattern in Rust
// Capture state changes as events for decoupled communication

use std::fmt;

#[derive(Debug, Clone)]
struct Timestamp(String);

impl Timestamp {
    fn now() -> Self {
        Timestamp("2024-01-15T10:30:00Z".to_string())
    }
}

// Domain events
#[derive(Debug, Clone)]
enum DomainEvent {
    OrderPlaced { order_id: u32, customer: String, total: f64, at: Timestamp },
    OrderShipped { order_id: u32, tracking: String, at: Timestamp },
    OrderCancelled { order_id: u32, reason: String, at: Timestamp },
    PaymentReceived { order_id: u32, amount: f64, at: Timestamp },
}

impl fmt::Display for DomainEvent {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            DomainEvent::OrderPlaced { order_id, customer, total, .. } =>
                write!(f, "OrderPlaced(#{}, {}, ${:.2})", order_id, customer, total),
            DomainEvent::OrderShipped { order_id, tracking, .. } =>
                write!(f, "OrderShipped(#{}, {})", order_id, tracking),
            DomainEvent::OrderCancelled { order_id, reason, .. } =>
                write!(f, "OrderCancelled(#{}, {})", order_id, reason),
            DomainEvent::PaymentReceived { order_id, amount, .. } =>
                write!(f, "PaymentReceived(#{}, ${:.2})", order_id, amount),
        }
    }
}

// Event bus
type Handler = Box<dyn Fn(&DomainEvent)>;

struct EventBus {
    handlers: Vec<Handler>,
}

impl EventBus {
    fn new() -> Self {
        EventBus { handlers: Vec::new() }
    }

    fn subscribe(&mut self, handler: Handler) {
        self.handlers.push(handler);
    }

    fn publish(&self, event: &DomainEvent) {
        println!("[EventBus] Publishing: {}", event);
        for handler in &self.handlers {
            handler(event);
        }
    }
}

fn main() {
    let mut bus = EventBus::new();

    // Subscribe handlers
    bus.subscribe(Box::new(|event| {
        if let DomainEvent::OrderPlaced { customer, .. } = event {
            println!("  [Email] Confirmation sent to {}", customer);
        }
    }));

    bus.subscribe(Box::new(|event| {
        if let DomainEvent::PaymentReceived { order_id, amount, .. } = event {
            println!("  [Accounting] Recorded ${:.2} for order #{}", amount, order_id);
        }
    }));

    bus.subscribe(Box::new(|event| {
        println!("  [Audit] Event logged: {}", event);
    }));

    // Publish events
    bus.publish(&DomainEvent::OrderPlaced {
        order_id: 1001, customer: "Alice".into(), total: 99.99, at: Timestamp::now(),
    });
    println!();
    bus.publish(&DomainEvent::PaymentReceived {
        order_id: 1001, amount: 99.99, at: Timestamp::now(),
    });
    println!();
    bus.publish(&DomainEvent::OrderShipped {
        order_id: 1001, tracking: "TRK-12345".into(), at: Timestamp::now(),
    });
}
