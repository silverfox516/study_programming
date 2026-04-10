// std::condition_variable — C++11
// What: a synchronization primitive for "wait until some predicate becomes true".
// Why:  busy-waiting on a flag wastes CPU; condvars let a thread sleep until notified.
// Pre:  pthread_cond_t with manual mutex pairing, or platform-specific events.

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::mutex              mtx;
std::condition_variable cv;
std::queue<int>         queue;
bool                    done = false;

void producer() {
    for (int i = 1; i <= 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(i);
            std::cout << "produced " << i << "\n";
        }
        cv.notify_one();   // wake one waiting consumer
    }
    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
    }
    cv.notify_all();
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        // The predicate form guards against spurious wake-ups
        cv.wait(lock, []{ return !queue.empty() || done; });

        while (!queue.empty()) {
            int v = queue.front();
            queue.pop();
            std::cout << "consumed " << v << "\n";
        }
        if (done) break;
    }
}

int main() {
    std::thread p(producer);
    std::thread c(consumer);
    p.join();
    c.join();
    return 0;
}
