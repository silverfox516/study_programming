// Baseline event loop — C++11 (standard library only)
// ============================================================================
// "Baseline" 이란?
// ----------------------------------------------------------------------------
// asio::io_context 는 외부 라이브러리지만 본질은 단순하다:
//
//     "작업(task) 을 큐에 쌓아 두고, 워커 스레드가 하나씩 꺼내 실행한다."
//
// 이 파일은 asio 를 쓰지 않고 std 만으로 같은 개념을 직접 구현한다.
// 짝이 되는 asio 예제(asio/core/io_context_basic.cpp 등)와 나란히 보면
// io_context 가 무엇을 추상화/확장한 것인지 한눈에 보인다.
//
//   "asio = 마법" 이 아니라, asio 가 우리 대신 다음 부담을 져준다:
//     · 스레드 안전한 task queue
//     · run() 의 정확한 종료 조건 (작업 끝나면 멈춤 / work_guard 유지)
//     · 다중 스레드 워커 풀, strand 직렬화
//     · timer / socket / 코루틴까지 같은 큐에서 처리
//
// What: 단일/다중 스레드 워커가 task queue 를 처리하는 미니 이벤트 루프.
// Why:  io_context 의 동작 모델을 std 만으로 재현하여 학습용 비교 기준을 만든다.
// Pre:  std::thread, std::mutex, std::condition_variable, std::queue.
//
// asio 대응 매핑:
//   Baseline                         ↔  asio
//   ----------                          ----------
//   EventLoop                        ↔  asio::io_context
//   loop.post(fn)                    ↔  asio::post(io, fn)
//   loop.run()                       ↔  io.run()
//   loop.stop()                      ↔  io.stop()
//   여러 thread 가 loop.run()        ↔  thread pool 패턴 (io_context_thread_pool)
//   keep_alive_ 플래그               ↔  asio::executor_work_guard
// ============================================================================

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>

class EventLoop {
public:
    using Task = std::function<void()>;

    // asio::post(io, fn) 에 대응. 어느 스레드에서 호출해도 안전해야 한다.
    void post(Task task) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            tasks_.push(std::move(task));
        }
        cv_.notify_one();
    }

    // asio::io_context::run() 에 대응.
    // 기본 동작: "작업이 있으면 처리, 없고 keep_alive 가 false 면 종료".
    // 여러 스레드가 동시에 run() 을 호출하면 그대로 워커 풀이 된다.
    void run() {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(mtx_);
                cv_.wait(lock, [this] {
                    return stopped_ || !tasks_.empty() || !keep_alive_;
                });

                if (stopped_) return;
                if (tasks_.empty()) {
                    // 큐가 비었고 keep_alive 가 false 면 자연 종료
                    // → asio::io_context 도 work_guard 없으면 큐가 비는 순간 run() 이 반환됨
                    if (!keep_alive_) return;
                    continue;
                }
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();  // 락 밖에서 실행 — 핸들러가 다시 post 해도 데드락 안 남
        }
    }

    // asio::io_context::stop() 에 대응. 즉시 모든 run() 을 깨워 종료시킨다.
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stopped_ = true;
        }
        cv_.notify_all();
    }

    // asio::executor_work_guard 와 같은 역할.
    // true 로 두면 큐가 비어도 run() 이 종료되지 않는다.
    // 기본값은 false → asio 의 "work guard 없는 io_context" 와 같은 시맨틱.
    void set_keep_alive(bool keep) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            keep_alive_ = keep;
        }
        cv_.notify_all();
    }

private:
    std::mutex              mtx_;
    std::condition_variable cv_;
    std::queue<Task>        tasks_;
    bool                    stopped_   = false;
    bool                    keep_alive_ = false;
};

// ----- 출력 동기화용 (예제 가독성) -----
std::mutex cout_mtx;
void log(const std::string& s) {
    std::lock_guard<std::mutex> lock(cout_mtx);
    std::cout << s << std::endl;
}
std::string tid() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

// ============================================================================
// Demo 1: 단일 스레드 — 가장 작은 io_context 와 같다.
//   asio 등가:
//       asio::io_context io;
//       asio::post(io, []{ ... });
//       io.run();   // 큐가 비면 run() 반환
// ============================================================================
void demo_single_thread() {
    log("\n[Demo 1] single-thread event loop");
    EventLoop loop;

    for (int i = 1; i <= 3; ++i) {
        loop.post([i] { log("  task #" + std::to_string(i) + " on " + tid()); });
    }
    loop.run();  // 큐가 비면 자연 종료
    log("[Demo 1] run() returned");
}

// ============================================================================
// Demo 2: 다중 워커 — asio 의 io_context 를 N 개 스레드에서 run() 하는 패턴.
//   asio 등가:
//       std::vector<std::thread> pool;
//       for (int i=0; i<4; ++i) pool.emplace_back([&]{ io.run(); });
//   keep_alive_ 가 false 면 한 워커가 큐를 비우고 깨어나 종료하며,
//   notify_all 이 다른 워커들도 같이 종료시킨다.
// ============================================================================
void demo_thread_pool() {
    log("\n[Demo 2] thread-pool event loop");
    EventLoop loop;

    for (int i = 1; i <= 8; ++i) {
        loop.post([i] {
            log("  task #" + std::to_string(i) + " on " + tid());
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        });
    }

    std::vector<std::thread> workers;
    for (int i = 0; i < 4; ++i) {
        workers.emplace_back([&loop] { loop.run(); });
    }
    for (auto& w : workers) w.join();
    log("[Demo 2] all workers joined");
}

// ============================================================================
// Demo 3: keep_alive 로 idle 유지 — asio::executor_work_guard 와 같음.
//   guard 가 살아 있는 동안에는 큐가 비어도 run() 이 종료되지 않아,
//   나중에 post 되는 task 도 받아 처리할 수 있다. 외부에서 stop 하면 종료.
//   asio 등가:
//       auto guard = asio::make_work_guard(io);
//       std::thread t([&]{ io.run(); });
//       // ... 나중에 ...
//       guard.reset();  // 또는 io.stop();
// ============================================================================
void demo_keep_alive() {
    log("\n[Demo 3] keep_alive (work_guard equivalent)");
    EventLoop loop;
    loop.set_keep_alive(true);

    std::thread worker([&loop] { loop.run(); });

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    loop.post([] { log("  late task on " + tid()); });
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    loop.set_keep_alive(false);  // → asio 의 guard.reset()
    worker.join();
    log("[Demo 3] worker joined after keep_alive released");
}

int main() {
    log("main thread = " + tid());
    demo_single_thread();
    demo_thread_pool();
    demo_keep_alive();
}
