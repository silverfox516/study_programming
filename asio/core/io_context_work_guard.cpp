// asio::executor_work_guard — keeping run() alive while idle
// ============================================================================
// What: io_context.run() 이 "큐가 비면 즉시 반환된다" 는 기본 동작을 막는 장치.
// Why:  실제 서버는 외부 이벤트(네트워크 수신, timer 등)를 기다려야 하므로
//       run() 이 임시로 큐가 비었다고 종료해 버리면 안 된다.
//       work_guard 를 쥐고 있는 동안에는 io_context 가 "할 일이 남아 있다" 고
//       간주되어 run() 이 블록한다.
// Pre:  baseline 의 EventLoop::set_keep_alive(true) 와 동일 의미.
//
// 핵심 사실:
//   1) work_guard 는 RAII — 소멸하거나 .reset() 호출 시 work 가 사라진다.
//   2) 하나라도 살아 있으면 run() 은 계속 돌고, 모두 사라지면 자연 종료.
//   3) 강제로 즉시 멈추려면 io.stop() — 큐에 남은 작업도 폐기된다.
//      "끝까지 처리하고 종료" 가 목적이면 stop() 이 아니라 work_guard 해제.
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
    log("main tid = " + tid());

    // ---- Demo 1: work_guard 없이 run() — 즉시 반환 ----
    {
        log("\n[1] no work_guard");
        asio::io_context io;
        std::thread t([&io] {
            log("  worker: run() start");
            io.run();   // 큐가 비어 있으니 곧장 반환
            log("  worker: run() end (immediate)");
        });
        t.join();
    }

    // ---- Demo 2: work_guard 로 idle 유지 ----
    {
        log("\n[2] with work_guard, late post, then guard.reset()");
        asio::io_context io;
        auto guard = asio::make_work_guard(io);

        std::thread worker([&io] {
            log("  worker: run() start, will block while guard is alive");
            io.run();
            log("  worker: run() end");
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        log("  main: posting late task");
        asio::post(io, [] { log("  late task executed on " + tid()); });

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        log("  main: releasing work_guard → run() will drain queue and return");
        guard.reset();   // ← 여기서부터는 큐가 비는 순간 run() 이 반환된다

        worker.join();
    }

    // ---- Demo 3: stop() 은 work_guard 와 무관하게 즉시 종료 ----
    {
        log("\n[3] stop() interrupts even with work_guard alive");
        asio::io_context io;
        auto guard = asio::make_work_guard(io);

        asio::post(io, [] {
            log("  long task starting");
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            log("  long task done");
        });
        asio::post(io, [] { log("  this may not run if stop() races in"); });

        std::thread worker([&io] { io.run(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        io.stop();   // 즉시 종료 — 큐에 남은 작업은 폐기 가능
        worker.join();
        log("  io.stopped() = " + std::string(io.stopped() ? "true" : "false"));
    }

    log("\n[main] done");
}
