// Leader/Followers — Concurrency Pattern (POSA2)
//
// Intent: a pool of threads cooperate to share a single event source. At any
//         moment one thread is the "leader" waiting on the source. When an
//         event arrives, the leader promotes a follower to leader and then
//         processes the event itself.
//
// When to use:
//   - Many threads share one event source (socket, queue, completion port)
//   - You want to avoid handing off events through an extra queue
//   - Cache locality matters — the same thread can detect AND process the event
//
// When NOT to use:
//   - Events are slow to process — use Half-Sync/Half-Async with a worker pool
//   - You only have one thread — degenerates to a plain reactor
//
// This in-process simulation uses a queue as the "event source".

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <atomic>
#include <chrono>
#include <string>

// ============================================================================
// Leader/Followers thread pool
// ============================================================================
class LeaderFollowersPool {
public:
    explicit LeaderFollowersPool(std::size_t num_threads) {
        for (std::size_t i = 0; i < num_threads; ++i) {
            threads_.emplace_back([this, i] { thread_loop(static_cast<int>(i)); });
        }
    }

    ~LeaderFollowersPool() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stopping_ = true;
        }
        leader_cv_.notify_all();
        for (auto& t : threads_) t.join();
    }

    void post_event(std::string event) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            events_.push(std::move(event));
        }
        leader_cv_.notify_one();
    }

private:
    void thread_loop(int id) {
        while (true) {
            std::string event;
            {
                std::unique_lock<std::mutex> lock(mtx_);

                // Wait until we are the leader and there is something to do
                leader_cv_.wait(lock, [this] {
                    return stopping_ || (!has_leader_ && !events_.empty()) || (!has_leader_ && events_.empty());
                });

                if (stopping_ && events_.empty()) return;

                // Become the leader
                has_leader_ = true;

                // Wait for an event while holding the leader role
                leader_cv_.wait(lock, [this] { return !events_.empty() || stopping_; });
                if (stopping_ && events_.empty()) {
                    has_leader_ = false;
                    leader_cv_.notify_all();
                    return;
                }

                // Take the event
                event = std::move(events_.front());
                events_.pop();

                // Promote a follower to leader BEFORE processing
                has_leader_ = false;
                leader_cv_.notify_one();
            }

            // Process the event without holding the lock
            std::cout << "  thread " << id << " handles: " << event << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    std::vector<std::thread> threads_;
    std::queue<std::string>  events_;
    std::mutex               mtx_;
    std::condition_variable  leader_cv_;
    bool                     has_leader_ = false;
    bool                     stopping_   = false;
};

int main() {
    std::cout << "=== Leader/Followers Pattern ===\n";
    LeaderFollowersPool pool(4);

    for (int i = 0; i < 12; ++i) {
        pool.post_event("event-" + std::to_string(i));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    std::cout << "\nKey points:\n";
    std::cout << " * Exactly one thread waits on the event source at any time\n";
    std::cout << " * Leader promotes a follower BEFORE processing — keeps throughput\n";
    std::cout << " * No hand-off queue needed between detector and processor\n";
    std::cout << " * Compare with Thread Pool (each worker pulls independently)\n";
    return 0;
}
