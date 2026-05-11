// asio::strand — serializing handlers across a multi-threaded io_context
// ============================================================================
// What: 여러 worker 스레드가 같은 io_context 를 돌고 있을 때, 특정 핸들러 그룹만큼은
//       *동시에 실행되지 않도록* 직렬화해 주는 executor adapter.
// Why:  thread-pool 모델에선 핸들러가 어떤 스레드에서 실행될지 모른다.
//       공유 상태(예: 한 클라이언트의 송신 큐) 를 만지는 핸들러가 동시에 두 워커에서
//       실행되면 race 가 난다. mutex 로 막을 수도 있지만:
//         - 락 경합 비용
//         - 핸들러가 또 다른 핸들러를 post 할 때 의도가 잘 드러나지 않음
//       strand 는 "이 그룹은 한 번에 하나만" 을 라이브러리 레벨에서 보장한다.
// Pre:  baseline 에는 직접 만든 등가 개념이 없다 (mutex 로 직접 막아야 함).
//
// 핵심 사실:
//   1) make_strand(io.get_executor()) 로 strand 를 만든다.
//   2) asio::post(strand, handler) 로 제출한 핸들러는 같은 strand 의 다른 핸들러와
//      *겹쳐서 실행되지 않는다*. (다른 strand 또는 strand 없는 핸들러와는 병렬 가능.)
//   3) 같은 strand 안의 핸들러끼리는 mutex 없이 공유 상태에 접근해도 안전하다.
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
    constexpr int kWorkers     = 4;
    constexpr int kTasksPerSet = 6;

    asio::io_context io;
    auto guard = asio::make_work_guard(io);

    // 워커 풀
    std::vector<std::thread> workers;
    for (int i = 0; i < kWorkers; ++i) {
        workers.emplace_back([&io] { io.run(); });
    }

    // ---- shared state — strand 가 보호한다는 것을 보이기 위한 공유 카운터 ----
    int  shared_counter = 0;        // strand 안에서만 만진다
    int  max_in_flight  = 0;        // strand 동시 실행 시 1 을 넘으면 buggy
    std::atomic<int> in_flight{0};  // 관찰용 (strand 외부에서 측정)

    auto strand = asio::make_strand(io.get_executor());

    // ---- (A) strand 로 직렬화된 작업 ----
    log("[A] tasks via strand — should never overlap");
    for (int i = 1; i <= kTasksPerSet; ++i) {
        asio::post(strand, [i, &shared_counter, &max_in_flight, &in_flight] {
            int now = ++in_flight;
            if (now > max_in_flight) max_in_flight = now;

            ++shared_counter;  // strand 안이라 mutex 없이 안전
            log("  strand task " + std::to_string(i) +
                " on " + tid() +
                "  shared=" + std::to_string(shared_counter) +
                "  in_flight=" + std::to_string(now));

            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            --in_flight;
        });
    }

    // ---- (B) strand 없이 직접 post — 동시 실행 가능 (병렬성 비교용) ----
    std::atomic<int> bare_in_flight{0};
    std::atomic<int> bare_max{0};
    log("[B] tasks without strand — may overlap on different workers");
    for (int i = 1; i <= kTasksPerSet; ++i) {
        asio::post(io, [i, &bare_in_flight, &bare_max] {
            int now = ++bare_in_flight;
            int prev = bare_max.load();
            while (now > prev && !bare_max.compare_exchange_weak(prev, now)) {}
            log("  bare   task " + std::to_string(i) +
                " on " + tid() +
                "  in_flight=" + std::to_string(now));
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            --bare_in_flight;
        });
    }

    // 모든 작업이 끝날 시간을 충분히 준다
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    guard.reset();
    for (auto& w : workers) w.join();

    log("\n[summary]");
    log("  shared_counter (via strand) = " + std::to_string(shared_counter));
    log("  strand max in-flight        = " + std::to_string(max_in_flight) +
        "   (must be 1)");
    log("  bare   max in-flight        = " + std::to_string(bare_max.load()) +
        "   (typically > 1 with multiple workers)");
}
