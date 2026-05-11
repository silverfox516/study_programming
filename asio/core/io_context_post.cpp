// asio::post — submitting work to an io_context
// ============================================================================
// What: 어느 스레드에서든 io_context 에 작업을 던지는 표준 진입점.
// Why:  worker 스레드가 따로 돌고 있는 io_context 에 다른 스레드에서 작업을
//       안전하게 위임하는 것이 asio 사용의 90% 패턴이다.
// Pre:  baseline 의 EventLoop::post() 와 동일한 의미.
//
// 핵심 사실:
//   1) asio::post(executor, handler) 는 *항상* 핸들러를 큐에 넣고 반환한다.
//      현재 스레드에서 즉시 실행하지 않는다 (← dispatch 와의 차이).
//   2) post 는 thread-safe. 어떤 스레드에서 호출해도 된다.
//   3) executor 인자로 io_context 자체를 넘기면 io.get_executor() 와 같은 의미.
// ============================================================================

#include <asio.hpp>

#include <atomic>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

namespace {
std::mutex cout_mtx;
void log(const std::string& s) {
    std::lock_guard<std::mutex> lock(cout_mtx);
    std::cout << s << '\n';
}
std::string tid() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}
}

int main() {
    asio::io_context io;

    // worker 한 개로 io_context 를 돌린다. 큐가 비면 work_guard 가 없으니 종료한다.
    // 일부러 keep-alive 하지 않고, "post 가 끝난 뒤 큐가 비면 자연 종료" 를 보여준다.
    auto guard = asio::make_work_guard(io);  // 데모 동안 idle 종료 방지
    std::thread worker([&io] {
        log("[worker] run() start on " + tid());
        io.run();
        log("[worker] run() end on " + tid());
    });

    // 메인 스레드에서 여러 작업을 던진다.
    std::atomic<int> done{0};
    constexpr int N = 5;
    for (int i = 1; i <= N; ++i) {
        asio::post(io, [i, &done] {
            log("  task " + std::to_string(i) + " executed on " + tid());
            ++done;
        });
    }
    log("[main] all posts submitted from " + tid());

    // 다른 producer 스레드 두 개에서도 동시에 post — thread-safe 함을 보임.
    auto producer = [&io](int base) {
        for (int i = 1; i <= 3; ++i) {
            asio::post(io, [base, i] {
                log("  producer-" + std::to_string(base) + " task " +
                    std::to_string(i) + " on " + tid());
            });
        }
    };
    std::thread p1(producer, 1);
    std::thread p2(producer, 2);
    p1.join();
    p2.join();

    // 모든 작업이 끝날 때까지 잠시 대기 후 work_guard 해제 → worker 자연 종료
    while (done.load() < N) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    guard.reset();
    worker.join();

    log("[main] done");
}
