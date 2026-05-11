// asio::io_context — running on multiple worker threads (thread pool)
// ============================================================================
// What: 동일한 io_context 의 run() 을 여러 스레드에서 호출하면 곧 워커 풀이 된다.
// Why:  CPU-bound 핸들러나 다수의 I/O 콜백을 병렬로 처리하기 위함.
//       asio 자체가 별도의 thread pool 클래스(asio::thread_pool)도 제공하지만,
//       io_context + N 개 worker 패턴이 가장 일반적이며 strand 와 함께 쓴다.
// Pre:  baseline 의 demo_thread_pool() 과 동일 모델.
//
// 핵심 사실:
//   1) 같은 io_context 에 대해 run() 을 N 번 호출해도 안전하다.
//      asio 내부가 thread-safe 하게 큐를 분배한다.
//   2) 어떤 핸들러가 어떤 스레드에서 실행될지는 정해져 있지 않다.
//      → 핸들러 사이에 공유 상태가 있으면 직접 동기화하거나 strand 를 써야 한다.
//      (다음 예제: io_context_strand.cpp)
//   3) work_guard 가 없으면 큐가 비는 순간 모든 worker 가 동시에 run() 에서 반환한다.
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
    constexpr int kWorkers = 4;
    constexpr int kTasks   = 16;

    asio::io_context io;
    auto guard = asio::make_work_guard(io);

    // 워커 풀 띄우기
    std::vector<std::thread> workers;
    workers.reserve(kWorkers);
    for (int i = 0; i < kWorkers; ++i) {
        workers.emplace_back([&io, i] {
            log("[worker " + std::to_string(i) + "] start on " + tid());
            io.run();
            log("[worker " + std::to_string(i) + "] end");
        });
    }

    // 작업 던지기 — 어느 worker 에서 실행될지는 asio 가 결정
    std::atomic<int> remaining{kTasks};
    for (int i = 1; i <= kTasks; ++i) {
        asio::post(io, [i, &remaining] {
            log("  task " + std::to_string(i) + " on " + tid());
            // 일부러 짧게 잠재워 워커들이 골고루 활동하는지 보이게 함
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            --remaining;
        });
    }

    // 모든 작업이 끝날 때까지 기다린 뒤 work_guard 해제 → 워커 정상 종료
    while (remaining.load() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    guard.reset();

    for (auto& w : workers) w.join();
    log("[main] all workers joined");
}
