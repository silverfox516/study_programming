// asio::io_context — basic lifecycle
// ============================================================================
// What: io_context 의 가장 기본 사용법. post → run → 자연 종료 / restart.
// Why:  io_context 가 "task queue + 워커 진입점" 이라는 점을 먼저 체득한다.
// Pre:  std 만으로는 spec/cpp/cpp11/event_loop_baseline.cpp 의 EventLoop 와 같음.
//
// 핵심 사실:
//   1) io.run() 은 처리할 작업이 더 없으면 *반환된다*. 영원히 안 돈다.
//      (work_guard 가 없으면 이 동작이 기본 — io_context_work_guard.cpp 참고)
//   2) 한 번 run() 이 끝난 io_context 는 stopped() 상태가 된다.
//      재사용하려면 io.restart() 가 필요하다.
//   3) io.stop() 은 모든 run() 을 깨워 즉시 종료시킨다.
//      큐에 남은 작업은 실행되지 않는다.
// ============================================================================

#include <asio.hpp>

#include <iostream>
#include <thread>

int main() {
    asio::io_context io;

    // post 한 시점에 task 가 큐에 들어가지만 실행되진 않는다.
    asio::post(io, [] { std::cout << "task A\n"; });
    asio::post(io, [] { std::cout << "task B\n"; });

    std::cout << "before run() — nothing printed yet\n";

    // run() 이 시작되면 큐에 쌓인 핸들러를 순서대로 실행한다.
    // 큐가 비는 순간 run() 은 반환된다.
    io.run();

    std::cout << "after run() — io.stopped() = " << std::boolalpha
              << io.stopped() << "\n";

    // stopped() 상태에서 다시 post 해도 *실행되지 않는다* — 먼저 restart() 필요.
    asio::post(io, [] { std::cout << "task C (won't run before restart)\n"; });
    io.run();  // stopped 상태이므로 즉시 반환
    std::cout << "post-only run() returned without executing C\n";

    // restart() 후 다시 run() 하면 큐에 쌓여 있던 C 가 실행된다.
    io.restart();
    io.run();
    std::cout << "after restart + run() — io.stopped() = " << io.stopped() << "\n";

    // stop() 데모: 별도 스레드가 run() 중일 때 외부에서 종료
    io.restart();
    asio::post(io, [] {
        std::cout << "long task starting\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << "long task done\n";
    });
    asio::post(io, [] { std::cout << "this may be skipped if stop() races in\n"; });

    std::thread worker([&io] { io.run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    io.stop();   // 즉시 종료 신호 — 남은 작업은 실행되지 않을 수 있음
    worker.join();

    std::cout << "final — io.stopped() = " << io.stopped() << "\n";
}
