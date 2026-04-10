# Study Inventory

> Snapshot date: 2026-04-10
> Scope: `spec/cpp/**` (language/library feature examples) and `dp/cpp/**` (design patterns).
> `dp/c`, `dp/rust` are out of scope for this study plan (tracked elsewhere).

This document is a **current-state matrix**. It does not list what is missing — see
[STUDY_GAPS.md](./STUDY_GAPS.md) for that.

---

## 1. `spec/cpp/` — C++ Standard Feature Examples

### 1.1 File counts per standard

| Standard | Files | Directory |
|----------|------:|-----------|
| C++98    |     2 | `spec/cpp/cpp98/` |
| C++11    |    22 | `spec/cpp/cpp11/` |
| C++14    |     7 | `spec/cpp/cpp14/` |
| C++17    |    16 | `spec/cpp/cpp17/` |
| C++20    |    18 | `spec/cpp/cpp20/` |
| C++23    |     9 | `spec/cpp/cpp23/` |
| **Total**| **74**| |

### 1.2 C++98 (`spec/cpp/cpp98/`)

| File | Topic |
|------|-------|
| `bind1st_bind2nd.cpp` | Legacy binders (`std::bind1st`, `std::bind2nd`) |
| `for_each.cpp`        | `std::for_each` algorithm |

### 1.3 C++11 (`spec/cpp/cpp11/`)

| File | Topic group |
|------|-------------|
| `array.cpp`                  | Containers — `std::array` |
| `atomic.cpp`                 | Concurrency — atomics |
| `auto.cpp`                   | Type deduction — `auto` |
| `bind.cpp`                   | Callable utilities — `std::bind` |
| `constexpr.cpp`              | Compile-time — `constexpr` |
| `enum_class.cpp`             | Types — scoped enums |
| `futures.cpp`                | Concurrency — `std::future`, `std::async` |
| `lambda_expressions.cpp`     | Callable utilities — lambdas |
| `mutex_locks.cpp`            | Concurrency — `std::mutex`, `lock_guard`, `unique_lock` |
| `noexcept.cpp`               | Error handling — `noexcept` |
| `nullptr.cpp`                | Types — `nullptr` |
| `override_final.cpp`         | OOP — `override`, `final` |
| `range_based_for.cpp`        | Language — range-for |
| `rvalue_references.cpp`      | Move semantics — rvalue refs |
| `smart_pointers.cpp`         | Memory — `unique_ptr`, `shared_ptr` |
| `static_assert.cpp`          | Compile-time — `static_assert` |
| `thread_local.cpp`           | Concurrency — TLS |
| `threads.cpp`                | Concurrency — `std::thread` |
| `tuple.cpp`                  | Utility — `std::tuple` |
| `user_defined_literals.cpp`  | Language — UDLs |
| `variadic_templates.cpp`     | Templates — variadic |
| `weak_ptr.cpp`               | Memory — `std::weak_ptr` |

### 1.4 C++14 (`spec/cpp/cpp14/`)

| File | Topic |
|------|-------|
| `binary_literals.cpp`                    | Language — `0b...` |
| `decltype_auto.cpp`                      | Type deduction — `decltype(auto)` |
| `deprecated_attribute.cpp`               | Attributes — `[[deprecated]]` |
| `digit_separators.cpp`                   | Language — `1'000'000` |
| `generic_lambdas.cpp`                    | Lambdas — `auto` parameters |
| `generic_lambda_bind_alternative.cpp`    | Lambdas replacing `std::bind` |
| `return_type_deduction.cpp`              | Functions — `auto` return |

### 1.5 C++17 (`spec/cpp/cpp17/`)

| File | Topic |
|------|-------|
| `any.cpp`                 | Utility — `std::any` |
| `apply.cpp`               | Utility — `std::apply` |
| `attributes.cpp`          | Attributes — `[[fallthrough]]`, `[[maybe_unused]]` |
| `filesystem.cpp`          | Library — `<filesystem>` |
| `if_constexpr.cpp`        | Language — `if constexpr` |
| `inline_variables.cpp`    | Language — inline variables |
| `invoke.cpp`              | Utility — `std::invoke` |
| `nodiscard.cpp`           | Attributes — `[[nodiscard]]` |
| `numeric_algos.cpp`       | Algorithms — `reduce`, `transform_reduce`, etc. |
| `optional.cpp`            | Utility — `std::optional` |
| `parallel_algorithms.cpp` | Algorithms — execution policies |
| `scoped_lock.cpp`         | Concurrency — `std::scoped_lock` |
| `shared_mutex.cpp`        | Concurrency — `std::shared_mutex` |
| `string_view.cpp`         | Strings — `std::string_view` |
| `structured_bindings.cpp` | Language — structured bindings |
| `variant.cpp`             | Utility — `std::variant` |

### 1.6 C++20 (`spec/cpp/cpp20/`)

| File | Topic |
|------|-------|
| `bind_front.cpp`           | Callable — `std::bind_front` |
| `bit_manipulation.cpp`     | Library — `<bit>` |
| `char8_t.cpp`              | Types — UTF-8 char type |
| `concepts.cpp`             | Templates — concepts |
| `consteval.cpp`            | Compile-time — immediate functions |
| `constinit.cpp`            | Compile-time — constant initialization |
| `coroutines.cpp`           | Language — coroutines |
| `format.cpp`               | Library — `std::format` |
| `jthread.cpp`              | Concurrency — `std::jthread` |
| `math_constants.cpp`       | Library — `<numbers>` |
| `modules_main.cpp`         | Language — modules (driver) |
| `hello.cppm` *(module IF)* | Language — modules (module unit) |
| `ranges.cpp`               | Library — `<ranges>` |
| `source_location.cpp`      | Library — `std::source_location` |
| `span.cpp`                 | Library — `std::span` |
| `sync_primitives.cpp`      | Concurrency — latch/barrier/semaphore? (verify) |
| `three_way_comparison.cpp` | Operators — `<=>` |
| `using_enum.cpp`           | Language — `using enum` |

> Note: `hello.cppm` is a module interface unit paired with `modules_main.cpp`, not a standalone example.

### 1.7 C++23 (`spec/cpp/cpp23/`)

| File | Topic |
|------|-------|
| `bind_back.cpp`         | Callable — `std::bind_back` |
| `byteswap.cpp`          | Library — `std::byteswap` |
| `expected.cpp`          | Error handling — `std::expected` |
| `if_consteval.cpp`      | Compile-time — `if consteval` |
| `monadic_optional.cpp`  | Utility — `optional::and_then`/`transform`/`or_else` |
| `print.cpp`             | Library — `std::print` / `std::println` |
| `stacktrace.cpp`        | Diagnostics — `std::stacktrace` |
| `to_underlying.cpp`     | Utility — `std::to_underlying` |
| `unreachable.cpp`       | Utility — `std::unreachable` |

### 1.8 Topic coverage (cross-standard)

Rough buckets showing which **themes** already have meaningful coverage:

| Theme                           | cpp98 | cpp11 | cpp14 | cpp17 | cpp20 | cpp23 |
|---------------------------------|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| Type deduction / `auto`         |       |   ✓   |   ✓   |       |       |       |
| Move semantics                  |       |   ✓   |       |       |       |       |
| Smart pointers                  |       |   ✓   |       |       |       |       |
| Lambdas / callable              |   ✓   |   ✓   |   ✓   |   ✓   |   ✓   |   ✓   |
| Concurrency — threads           |       |   ✓   |       |       |   ✓   |       |
| Concurrency — sync primitives   |       |   ✓   |       |   ✓   |   ~   |       |
| Concurrency — futures/atomics   |       |   ✓   |       |       |       |       |
| Sum types (`optional`/`variant`/`expected`) | | |   |   ✓   |       |   ✓   |
| Strings / views                 |       |       |       |   ✓   |       |       |
| Ranges / views                  |       |       |       |       |   ✓   |       |
| Compile-time (`constexpr` family)|      |   ✓   |       |   ✓   |   ✓   |   ✓   |
| Templates — concepts/variadics  |       |   ✓   |       |       |   ✓   |       |
| Attributes                      |       |       |   ✓   |   ✓   |       |       |
| Modules / coroutines            |       |       |       |       |   ✓   |       |
| Error handling (`expected`, stacktrace) |  |   |       |       |       |   ✓   |
| Filesystem / I/O                |       |       |       |   ✓   |       |       |
| Formatting / printing           |       |       |       |       |   ✓   |   ✓   |

`~` = partial / needs verification.

---

## 2. `dp/cpp/` — Design Pattern Examples

### 2.1 File inventory

33 `.cpp` files + 1 `.hpp`. `factory_impl.cpp` is a non-`main` implementation paired with `factory_improved.cpp` (handled specially in CMake).

### 2.2 GoF catalog coverage

**Creational (5/5)**

| Pattern           | File                                   |
|-------------------|----------------------------------------|
| Abstract Factory  | `abstract_factory.cpp`                 |
| Builder           | `builder.cpp`                          |
| Factory Method    | `factory.cpp` + `factory_improved.cpp` + `factory_impl.cpp` + `factory.hpp` |
| Prototype         | `prototype.cpp`                        |
| Singleton         | `singleton.cpp`                        |

**Structural (7/7)**

| Pattern   | File              |
|-----------|-------------------|
| Adapter   | `adapter.cpp`     |
| Bridge    | `bridge.cpp`      |
| Composite | `composite.cpp`   |
| Decorator | `decorator.cpp`   |
| Facade    | `facade.cpp`      |
| Flyweight | `flyweight.cpp`   |
| Proxy     | `proxy.cpp`       |

**Behavioral (11/11)**

| Pattern                 | File                          |
|-------------------------|-------------------------------|
| Chain of Responsibility | `chain_of_responsibility.cpp` |
| Command                 | `command.cpp`                 |
| Interpreter             | `interpreter.cpp`             |
| Iterator                | `iterator.cpp`                |
| Mediator                | `mediator.cpp`                |
| Memento                 | `memento.cpp`                 |
| Observer                | `observer.cpp`                |
| State                   | `state.cpp`                   |
| Strategy                | `strategy.cpp`                |
| Template Method         | `template_method.cpp`         |
| Visitor                 | `visitor.cpp`                 |

**GoF total: 23/23 present.**

### 2.3 Modern / enterprise patterns present

| Pattern               | File                        | Category            |
|-----------------------|-----------------------------|---------------------|
| Dependency Injection  | `dependency_injection.cpp`  | Architectural       |
| Repository            | `repository.cpp`            | Enterprise / DDD    |
| MVC                   | `mvc.cpp`                   | Architectural       |
| Null Object           | `null_object.cpp`           | Behavioral extension|
| Object Pool           | `object_pool.cpp`           | Concurrency / perf  |
| Circuit Breaker       | `circuit_breaker.cpp`       | Resilience          |
| Event Sourcing        | `event_sourcing.cpp`        | DDD / CQRS family   |

### 2.4 Build system

- `dp/cpp/CMakeLists.txt` uses `file(GLOB CPP_SOURCES "*.cpp")` — new `.cpp` files are picked up automatically.
- `factory_impl.cpp` is excluded from auto-targets and linked into `factory_improved` explicitly.
- No `CMakeLists.txt` for `spec/cpp/**` yet.

---

## 3. Build artifacts checked into untracked state

The following `git status` `??` entries are **compiled Mach-O executables**, not source or directories:

```
spec/cpp/cpp11/bind
spec/cpp/cpp14/generic_lambda
spec/cpp/cpp20/bind_front
spec/cpp/cpp23/bind_back
spec/cpp/cpp98/bind1st_bind2nd
```

There is **no `.gitignore`** at the repo root. Phase 4 should add one and remove these artifacts.

---

## 4. Toolchain snapshot

- Compiler: **Apple clang 21** (arm64 macOS). Supports most of C++23; some library features depend on libc++ version and may require feature-test macros.

---

## 5. How to read this document

- **Present** = a file with that name/topic exists. **This does not imply the example is complete or high-quality.**
- "Gap" analysis (what should exist but does not) lives in [STUDY_GAPS.md](./STUDY_GAPS.md).
- Quality review of existing files is **out of scope** for the current study plan — the focus is filling in missing pieces, not rewriting what is there.
