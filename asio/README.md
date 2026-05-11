# asio

[Standalone Asio](https://think-async.com/Asio/) 라이브러리 학습용 예제 모음.
표준 라이브러리만으로 만든 베이스라인 코드와 짝지어 학습한다.

## 무엇을 학습하나

asio는 **비동기 I/O + 실행 컨텍스트(executor)** 라이브러리다. 핵심은 `io_context`라는
이벤트 루프 위에 timer / socket / coroutine 같은 기능이 얹히는 구조.

이 디렉토리는 그 핵심을 한 layer씩 벗겨 본다.

```
asio/
├── core/            # io_context, executor, strand — 실행 기반
├── timer/           # (예정) steady_timer, async_wait
├── net/             # (예정) tcp/udp acceptor, socket
└── coroutine/       # (예정) co_spawn, awaitable (C++20)
```

## 베이스라인 비교

asio 없이 std 만으로 동등한 개념을 직접 구현한 코드를 짝지어 둔다.
"asio가 마법이 아니라 어떤 부담을 대신 져 주는 라이브러리인지" 가 보인다.

| asio 예제                              | 베이스라인 (std only)                          |
|---------------------------------------|------------------------------------------------|
| `core/io_context_basic.cpp`           | `spec/cpp/cpp11/event_loop_baseline.cpp`       |
| `core/io_context_thread_pool.cpp`     | (위 베이스라인의 워커 N개 변형)                 |

## 빌드

```bash
cd asio
mkdir build && cd build
cmake ..
cmake --build . -j
```

- 첫 빌드 시 CMake가 GitHub에서 standalone asio (header-only)를 자동으로 받아온다.
- Boost 의존성 없음.
- 실행 파일은 `build/bin/{module}/` 하위에 생성된다.

### 컴파일러 요구

- C++17 이상
- Threads 라이브러리 (CMake가 자동으로 찾음)
- Windows: ws2_32, mswsock 자동 링크 (CMake에서 처리)

## core/ 예제 목록

| 파일                                       | 학습 포인트                                      |
|-------------------------------------------|--------------------------------------------------|
| `io_context_basic.cpp`                    | `run()`/`stop()`/`restart()` 라이프사이클        |
| `io_context_post.cpp`                     | `asio::post()` 로 작업 큐잉                       |
| `io_context_post_dispatch_defer.cpp`      | `post` vs `dispatch` vs `defer` 의미 차이         |
| `io_context_work_guard.cpp`               | `executor_work_guard` 로 idle 유지               |
| `io_context_thread_pool.cpp`              | 여러 스레드에서 `run()` — 워커 풀                 |
| `io_context_strand.cpp`                   | `strand` 로 핸들러 직렬화 (멀티스레드 안전성)      |

권장 학습 순서: 위에서 아래로. 각 파일은 단독으로 실행 가능.

## 참고

- [Asio C++ Library — Tutorial](https://think-async.com/Asio/asio-1.30.2/doc/asio/tutorial.html)
- [io_context reference](https://think-async.com/Asio/asio-1.30.2/doc/asio/reference/io_context.html)
- [Networking TS / std::execution 진행 상황 (C++26)](https://en.cppreference.com/w/cpp/experimental/networking)
