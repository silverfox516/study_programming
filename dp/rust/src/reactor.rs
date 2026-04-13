// Reactor Pattern in Rust
// Event demultiplexer dispatches I/O events to registered handlers

use std::collections::HashMap;

type EventHandler = Box<dyn Fn(&str)>;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
enum EventType {
    Read,
    Write,
    Connect,
    Close,
}

struct Reactor {
    handlers: HashMap<(usize, EventType), EventHandler>,
}

struct Event {
    fd: usize,
    event_type: EventType,
    data: String,
}

impl Reactor {
    fn new() -> Self {
        Reactor {
            handlers: HashMap::new(),
        }
    }

    fn register<F>(&mut self, fd: usize, event_type: EventType, handler: F)
    where
        F: Fn(&str) + 'static,
    {
        println!("[Reactor] Registered handler for fd={}, {:?}", fd, event_type);
        self.handlers.insert((fd, event_type), Box::new(handler));
    }

    fn handle_events(&self, events: &[Event]) {
        for event in events {
            let key = (event.fd, event.event_type);
            if let Some(handler) = self.handlers.get(&key) {
                println!(
                    "[Reactor] Dispatching {:?} on fd={}",
                    event.event_type, event.fd
                );
                handler(&event.data);
            } else {
                println!(
                    "[Reactor] No handler for fd={}, {:?}",
                    event.fd, event.event_type
                );
            }
        }
    }
}

fn main() {
    let mut reactor = Reactor::new();

    reactor.register(1, EventType::Read, |data| {
        println!("  [Handler] Read from socket: {}", data);
    });
    reactor.register(1, EventType::Close, |_| {
        println!("  [Handler] Connection closed");
    });
    reactor.register(2, EventType::Connect, |data| {
        println!("  [Handler] New connection: {}", data);
    });

    // Simulate event loop
    let events = vec![
        Event { fd: 2, event_type: EventType::Connect, data: "client:8080".into() },
        Event { fd: 1, event_type: EventType::Read, data: "GET /index".into() },
        Event { fd: 1, event_type: EventType::Read, data: "GET /about".into() },
        Event { fd: 1, event_type: EventType::Close, data: String::new() },
    ];

    println!("\n--- Event Loop ---");
    reactor.handle_events(&events);
}
