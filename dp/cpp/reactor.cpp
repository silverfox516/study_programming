// Reactor — Concurrency Pattern (POSA2)
//
// Intent: an event-driven dispatcher that demultiplexes events from one or more
//         sources and synchronously dispatches them to registered handlers.
//         Handlers run on the reactor thread, never concurrently with each other.
//
// When to use:
//   - You have many event sources (timers, sockets, signals) and few threads
//   - Single-threaded execution is desirable for simplicity (no shared-state locks)
//   - Latency from "event arrives -> handler runs" matters
//
// When NOT to use:
//   - Handlers do long blocking work — they will starve everyone else
//     (use Proactor or Half-Sync/Half-Async instead)
//   - You need parallel handler execution — use Thread Pool dispatch
//
// This is a simplified, in-process reactor using a queue of "events" rather than
// real OS file descriptors. The pattern is the same.

#include <iostream>
#include <functional>
#include <queue>
#include <unordered_map>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

// ============================================================================
// Event types this reactor knows about
// ============================================================================
enum class EventType {
    Connect,
    Read,
    Timer,
    Disconnect,
    Shutdown,
};

struct Event {
    EventType   type;
    std::string source;     // who fired it
    std::string payload;
};

// ============================================================================
// The reactor
// ============================================================================
class Reactor {
public:
    using Handler = std::function<void(const Event&)>;

    void register_handler(EventType type, Handler h) {
        handlers_[type] = std::move(h);
    }

    void post(Event e) {
        events_.push(std::move(e));
    }

    void run() {
        running_ = true;
        std::cout << "[reactor] event loop started\n";
        while (running_ && !events_.empty()) {
            Event e = std::move(events_.front());
            events_.pop();
            dispatch(e);
        }
        std::cout << "[reactor] event loop stopped\n";
    }

    void stop() { running_ = false; }

private:
    void dispatch(const Event& e) {
        auto it = handlers_.find(e.type);
        if (it == handlers_.end()) {
            std::cout << "[reactor] no handler for event\n";
            return;
        }
        it->second(e);
    }

    std::queue<Event>                          events_;
    std::unordered_map<EventType, Handler>     handlers_;
    bool                                       running_ = false;
};

// ============================================================================
// Demo: a tiny chat-server simulation
// ============================================================================
int main() {
    std::cout << "=== Reactor Pattern ===\n";
    Reactor reactor;
    int connection_count = 0;

    reactor.register_handler(EventType::Connect, [&](const Event& e) {
        ++connection_count;
        std::cout << "  [Connect]    " << e.source
                  << " — total connections: " << connection_count << "\n";
    });

    reactor.register_handler(EventType::Read, [&](const Event& e) {
        std::cout << "  [Read]       " << e.source << " says: " << e.payload << "\n";
    });

    reactor.register_handler(EventType::Timer, [&](const Event& e) {
        std::cout << "  [Timer]      " << e.payload << "\n";
    });

    reactor.register_handler(EventType::Disconnect, [&](const Event& e) {
        --connection_count;
        std::cout << "  [Disconnect] " << e.source
                  << " — remaining: " << connection_count << "\n";
    });

    reactor.register_handler(EventType::Shutdown, [&](const Event&) {
        std::cout << "  [Shutdown]   stopping reactor\n";
        reactor.stop();
    });

    // Simulate some events
    reactor.post({EventType::Connect,    "client-1", ""});
    reactor.post({EventType::Connect,    "client-2", ""});
    reactor.post({EventType::Read,       "client-1", "hello"});
    reactor.post({EventType::Read,       "client-2", "hi there"});
    reactor.post({EventType::Timer,      "system",   "heartbeat"});
    reactor.post({EventType::Disconnect, "client-1", ""});
    reactor.post({EventType::Read,       "client-2", "bye"});
    reactor.post({EventType::Disconnect, "client-2", ""});
    reactor.post({EventType::Shutdown,   "system",   ""});

    reactor.run();

    std::cout << "\nKey points:\n";
    std::cout << " * Single-threaded — handlers never race each other\n";
    std::cout << " * Demultiplexes many event types via one dispatch table\n";
    std::cout << " * Real reactors use select/poll/epoll/kqueue/IOCP under the hood\n";
    std::cout << " * Handlers must be quick — blocking starves everyone\n";
    return 0;
}
