// asio::post vs dispatch vs defer
// ============================================================================
// What: 같은 "작업 제출" 함수처럼 보이지만 의미가 다른 세 가지 비교.
// Why:  잘못된 선택은 락을 다시 잡거나, 재귀 호출이 깊어지거나,
//       반대로 직렬화 의도가 깨져서 race 를 만든다.
//
// 요약:
//   asio::post(ex, h)     → 무조건 큐에 넣고 반환. 절대 호출자 스택에서 실행 X.
//   asio::dispatch(ex, h) → 호출자가 같은 executor 위에서 *이미* 실행 중이면
//                           즉시 그 자리에서 실행. 아니면 post 와 같음.
//   asio::defer(ex, h)    → 항상 큐잉. post 와 비슷하나 "체이닝 의도" 표현.
//                           (실제 실행 시점에서 어떤 알림 fence 가 들어갈 수
//                            있다는 점에서 구현체에 따라 약간 다른 hint.)
// ============================================================================

#include <asio.hpp>

#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

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
    auto guard = asio::make_work_guard(io);

    std::thread worker([&io] { io.run(); });

    log("main tid = " + tid());

    // ---- Case A: io_context 외부에서 호출 ----
    // 이 시점에 메인 스레드는 io.run() 안이 아니다.
    // → post / dispatch / defer 모두 결국 worker 에서 실행된다.
    log("\n[A] called from OUTSIDE io.run()");
    asio::post    (io, [] { log("  A.post     on " + tid()); });
    asio::dispatch(io, [] { log("  A.dispatch on " + tid()); });
    asio::defer   (io, [] { log("  A.defer    on " + tid()); });

    // ---- Case B: io_context 핸들러 *안에서* 호출 ----
    // 이 시점에는 호출자 스레드가 곧 worker 다.
    // → dispatch 만 즉시 (재귀적으로) 같은 스택에서 실행된다.
    //   post/defer 는 한 바퀴 돌아 다시 큐에서 꺼내져 실행된다.
    asio::post(io, [&io] {
        log("\n[B] called from INSIDE io.run() — outer handler on " + tid());

        asio::post(io, [] {
            log("  B.post     on " + tid() + "  (queued, runs after outer returns)");
        });
        asio::dispatch(io, [] {
            log("  B.dispatch on " + tid() + "  (immediate, before outer returns)");
        });
        asio::defer(io, [] {
            log("  B.defer    on " + tid() + "  (queued, runs after outer returns)");
        });

        log("[B] outer handler returning");
    });

    // 모든 작업 처리 후 종료
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    guard.reset();
    worker.join();
    log("\n[main] done");
}
