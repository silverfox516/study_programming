# Study Progress Tracker

> Started: 2026-04-10
> Goal: Fill all gaps from [STUDY_GAPS.md](./STUDY_GAPS.md).
> Total target: **132 new files** (97 spec + 35 patterns) — see breakdown below.
>
> **For the agent picking up next session**: read this file first. The "Next session
> starts at" pointer at the bottom tells you exactly where to resume.

## Conventions

- `[ ]` = not started
- `[~]` = file written but compile not yet verified
- `[x]` = written + compiled + verified working
- `[!]` = written but compile failed (compiler/lib limitation) — guarded with `#if __has_include` or marked with `// Requires: ...`
- `[-]` = intentionally skipped (with reason in note)

Each file should follow the style of `spec/cpp/cpp11/bind.cpp` (minimal, single `main`,
3-line header comment) for **spec/** items, and the style of `dp/cpp/strategy.cpp`
(multi-example, modern C++ variant included) for **pattern/** items.

Compilation policy: every spec file must compile under Apple clang 21 with the
correct `-std=c++NN` flag before it is marked `[x]`.

---

## Spec gaps — 97 files

### cpp98 — 5 files (museum, compile with `-std=c++14`)

> C++98 itself is not deprecated, but the specific features in this directory were
> removed in C++17. See `spec/cpp/cpp98/README.md`. Compile these with
> `clang++ -std=c++14 -Wno-deprecated-declarations file.cpp`.

- [x] H `functor.cpp` — function objects (`struct` with `operator()`)
- [x] H `mem_fun.cpp` — `std::mem_fun`, `std::mem_fun_ref`
- [x] M `auto_ptr.cpp` — `std::auto_ptr` (removed C++17)
- [x] M `iterator_categories.cpp` — input/output/forward/bidirectional/random-access tags
- [x] L `template_specialization.cpp` — full and partial specialization

### cpp11 — 21 files (`-std=c++11`)

#### High (8)
- [x] H `initializer_list.cpp` — `std::initializer_list<T>`, brace-init constructors
- [x] H `chrono.cpp` — durations, time points, clocks
- [x] H `type_traits.cpp` — `<type_traits>` foundation
- [x] H `move_semantics.cpp` — `std::move`, move ctor/assign, when copies become moves
- [x] H `perfect_forwarding.cpp` — `std::forward`, universal references
- [x] H `delegating_inheriting_ctors.cpp` — constructor delegation, inheriting ctors
- [x] H `default_delete.cpp` — `= default`, `= delete`
- [x] H `uniform_initialization.cpp` — `{}` brace-init, narrowing rules

#### Medium (7)
- [x] M `unordered_containers.cpp` — `unordered_map/set/multimap`
- [x] M `regex.cpp` — `<regex>` basics
- [x] M `random.cpp` — `<random>` engines + distributions
- [x] M `raw_string_literals.cpp` — `R"(...)"`, char literal prefixes
- [x] M `condition_variable.cpp` — pair with mutex_locks
- [x] M `namespace_alias.cpp` — `namespace ns = a::b::c;`
- [x] M `ratio.cpp` — `<ratio>` companion to chrono

#### Low (6)
- [x] L `attribute_noreturn.cpp` — `[[noreturn]]`
- [x] L `alignas_alignof.cpp` — alignment control
- [x] L `extended_friend.cpp` — extended friend declarations
- [x] L `trailing_return_type.cpp` — `auto f() -> T`
- [x] L `explicit_conversion_op.cpp` — `explicit operator T()`
- [x] L `unicode_literals.cpp` — `u8`, `u`, `U` string literals

### cpp14 — 8 files (`-std=c++14`)

#### High (2)
- [x] H `make_unique.cpp` — `std::make_unique`
- [x] H `variable_templates.cpp` — `template<typename T> constexpr T pi = ...;`

#### Medium (3)
- [x] M `relaxed_constexpr.cpp` — multi-statement constexpr functions
- [x] M `integer_sequence.cpp` — `std::index_sequence`, basis for tuple unpacking
- [x] M `shared_timed_mutex.cpp` — timed shared lock

#### Low (3)
- [x] L `quoted.cpp` — `std::quoted` stream manipulator
- [x] L `exchange.cpp` — `std::exchange`
- [x] L `tuple_get_by_type.cpp` — `std::get<T>(tuple)`

### cpp17 — 15 files (`-std=c++17`)

#### High (3)
- [x] H `class_template_argument_deduction.cpp` — CTAD
- [x] H `fold_expressions.cpp` — `(args + ...)`
- [x] H `charconv.cpp` — `std::from_chars`, `to_chars`

#### Medium (7)
- [x] M `clamp_sample.cpp` — `std::clamp`, `std::sample`
- [x] M `gcd_lcm.cpp` — `std::gcd`, `std::lcm`
- [x] M `has_include.cpp` — `__has_include`
- [x] M `nested_namespaces.cpp` — `namespace A::B::C { ... }`
- [x] M `byte.cpp` — `std::byte`
- [x] M `memory_resource.cpp` — `<memory_resource>` PMR
- [x] M `searcher.cpp` — `std::boyer_moore_searcher`

#### Low (5)
- [x] L `launder.cpp` — `std::launder`
- [x] L `aligned_alloc.cpp` — over-aligned `new`
- [x] L `if_init_statement.cpp` — `if (init; cond)`
- [x] L `template_auto.cpp` — `template<auto V>`
- [x] L `piecewise_construct.cpp` — `std::piecewise_construct`

### cpp20 — 21 files (`-std=c++20`)

#### High (10)
- [ ] H `concepts_requires.cpp` — `requires` clauses, requires-expressions, custom concepts
- [ ] H `latch_barrier.cpp` — `std::latch`, `std::barrier`
- [ ] H `semaphore.cpp` — counting/binary semaphore
- [ ] H `stop_token.cpp` — `std::stop_token` / `stop_source`
- [ ] H `bit_cast.cpp` — `std::bit_cast`
- [ ] H `designated_initializers.cpp` — `Foo{.x = 1}`
- [ ] H `aggregate_init_parens.cpp` — `()` for aggregates
- [ ] H `string_starts_ends_with.cpp` — `starts_with`, `ends_with`
- [ ] H `atomic_ref.cpp` — `std::atomic_ref`, `atomic<shared_ptr>`
- [ ] H `template_lambda.cpp` — `[]<typename T>(T x)`

#### Medium (8)
- [ ] M `concepts_standard_library.cpp` — `<concepts>` header
- [ ] M `ranges_factories.cpp` — `views::iota`, `views::single`
- [ ] M `ranges_pipeline.cpp` — elaborate pipeline beyond current `ranges.cpp`
- [ ] M `chrono_calendar.cpp` — calendar/timezone additions
- [ ] M `format_advanced.cpp` — custom `std::formatter` specialization
- [ ] M `endian.cpp` — `std::endian`
- [ ] M `coroutine_generator.cpp` — hand-written generator coroutine
- [ ] M `lambda_pack_capture.cpp` — `[...args = ...]` pack capture

#### Low (3)
- [ ] L `feature_test_macros.cpp` — `__cpp_lib_*` macros
- [ ] L `consteval_propagation.cpp` — `consteval` interaction edge cases
- [ ] L `aggregate_designated_array.cpp` — designated init for arrays/nested

### cpp23 — 27 files (`-std=c++2b` / `-std=c++23`)

> libc++ feature support varies. Use `#if __has_include(<header>)` guards for
> known-fragile items: `flat_map`, `mdspan`, `generator`.

#### High (9)
- [ ] H `flat_map_set.cpp` — `std::flat_map`, `std::flat_set` *(verify libc++)*
- [ ] H `mdspan.cpp` — `std::mdspan` *(verify libc++)*
- [ ] H `generator.cpp` — `std::generator<T>` *(verify libc++)*
- [ ] H `ranges_to.cpp` — `std::ranges::to<Container>()`
- [ ] H `deducing_this.cpp` — explicit object parameter
- [ ] H `multidimensional_subscript.cpp` — `m[i, j]`
- [ ] H `static_call_operator.cpp` — `static operator()`
- [ ] H `string_contains.cpp` — `std::string::contains`
- [ ] H `move_only_function.cpp` — `std::move_only_function`

#### Medium (12)
- [ ] M `expected_monadic.cpp` — `expected::and_then` chain
- [ ] M `ranges_zip.cpp` — `views::zip`
- [ ] M `ranges_enumerate.cpp` — `views::enumerate`
- [ ] M `ranges_adjacent.cpp` — `views::adjacent`
- [ ] M `ranges_chunk.cpp` — `views::chunk`
- [ ] M `ranges_slide.cpp` — `views::slide`
- [ ] M `ranges_join_with.cpp` — `views::join_with`
- [ ] M `ranges_cartesian.cpp` — `views::cartesian_product`
- [ ] M `ranges_repeat.cpp` — `views::repeat`
- [ ] M `out_ptr.cpp` — `std::out_ptr`, `inout_ptr`
- [ ] M `print_println.cpp` — extend existing `print.cpp` if missing `println`
- [ ] M `expected_constructors.cpp` — `expected` construction patterns

#### Low (6)
- [ ] L `constexpr_unique_ptr.cpp` — constexpr expansion
- [ ] L `auto_decay_copy.cpp` — `auto(x)` decay copy
- [ ] L `size_t_literal.cpp` — `1uz` literal suffix
- [ ] L `extended_floating_point.cpp` — `float16_t` etc.
- [ ] L `consteval_if_propagation.cpp` — interactions with `consteval`
- [ ] L `assume_attribute.cpp` — `[[assume(...)]]`

---

## Pattern gaps — 35 files (`dp/cpp/`)

### Concurrency patterns (10)
- [ ] H `producer_consumer.cpp` — bounded blocking queue
- [ ] H `thread_pool.cpp` — worker pool with task queue
- [ ] H `active_object.cpp` — POSA2 method invocation decoupling
- [ ] H `monitor_object.cpp` — synchronized class methods
- [ ] M `reactor.cpp` — event demultiplexing
- [ ] M `proactor.cpp` — async I/O dispatch
- [ ] M `double_checked_locking.cpp` — singleton variant
- [ ] M `read_write_lock.cpp` — `shared_mutex` pattern form
- [ ] L `leader_followers.cpp`
- [ ] L `half_sync_half_async.cpp`

### Architectural / enterprise / DDD (12)
- [ ] H `cqrs.cpp` — pairs with existing `event_sourcing.cpp`
- [ ] H `unit_of_work.cpp` — PoEAA tx boundary
- [ ] H `specification.cpp` — composable business rules
- [ ] H `service_locator.cpp` — contrast with DI
- [ ] M `lazy_initialization.cpp` — `std::call_once`, `once_flag`
- [ ] M `data_mapper.cpp` — PoEAA
- [ ] M `value_object.cpp` — DDD building block
- [ ] M `domain_event.cpp` — DDD
- [ ] M `saga.cpp` — distributed transaction
- [ ] M `pipes_and_filters.cpp` — stream topology
- [ ] L `table_module.cpp` — PoEAA
- [ ] L `transaction_script.cpp` — PoEAA

### Resilience / cloud (8)
- [ ] H `retry.cpp` — pairs with `circuit_breaker.cpp`
- [ ] H `bulkhead.cpp` — resource isolation
- [ ] H `timeout.cpp` — timeout pattern
- [ ] M `throttling.cpp` — rate limiting / backpressure
- [ ] M `cache_aside.cpp` — cache + repository
- [ ] M `health_endpoint.cpp` — `/health` checker
- [ ] L `ambassador.cpp` — sidecar networking proxy
- [ ] L `compensating_transaction.cpp` — pairs with saga

### Modern C++ idiom patterns (5)
- [ ] H `pimpl.cpp` — pointer-to-implementation
- [ ] H `crtp.cpp` — Curiously Recurring Template Pattern
- [ ] H `type_erasure.cpp` — `std::function`-style erasure
- [ ] H `tag_dispatch.cpp` — compile-time strategy selection
- [ ] H `policy_based_design.cpp` — Alexandrescu-style policies

---

## Phase 4 — finalization tasks

- [ ] Add `.gitignore` at repo root (Mach-O artifacts: `*.o`, no extension binaries in `spec/cpp/**`)
- [ ] Remove existing build artifacts: `spec/cpp/cpp98/bind1st_bind2nd`, `cpp11/bind`, `cpp14/generic_lambda`, `cpp20/bind_front`, `cpp23/bind_back`
- [ ] Add `spec/cpp/CMakeLists.txt` with per-standard subdirectories
- [ ] Verify whole-tree build of `dp/cpp` and `spec/cpp`
- [ ] Update repo `README.md` to link `docs/ROADMAP.md` and `docs/STUDY_PROGRESS.md`
- [ ] Phase 1: write `docs/ROADMAP.md` (curriculum)

---

## Session log

### Session 1 — 2026-04-10
- Inventory + gap analysis (Phase 0) complete
- `docs/STUDY_PROGRESS.md` created
- cpp98 museum complete: README + 5 files
- cpp11 High complete: 8 files
- **Session 1 total: 13 files**

### Session 2 — 2026-04-10
- cpp11 Medium (7), cpp11 Low (6), cpp14 (8)
- cpp11 21/21, cpp14 8/8
- **Session 2 total: 21 files. Cumulative: 34 / 132 (25.8%)**

### Session 3 — 2026-04-10
- cpp17 High (3): class_template_argument_deduction, fold_expressions, charconv
- cpp17 Medium (7): clamp_sample, gcd_lcm, has_include, nested_namespaces, byte, memory_resource, searcher
- cpp17 Low (5): launder, aligned_alloc, if_init_statement, template_auto, piecewise_construct
- cpp17 fully done (15/15)
- All 15 new files compiled with `-std=c++17 -Wall -Wextra` and run successfully
- Note: clangd IDE diagnostics show false positives (defaults to older standard) — only `clang++ -std=c++17` results count
- **Session 3 total: 15 files. Cumulative: 49 / 132 (37.1%)**

### Next session starts at

> Read this section first when resuming.

**Pointer**: cpp20 batch (21 files) — start with the 10 High items, then 8 Medium,
then 3 Low. Use `-std=c++20 -Wall -Wextra`.

After cpp20, continue with cpp23 (27), then patterns (35).

**Compilation command template**:
```
cd /Users/igsong/priv/repositories/silverfox516/study_programming
clang++ -std=c++NN -Wall -Wextra -Wno-deprecated-declarations \
    spec/cpp/cppNN/<file>.cpp -o /tmp/spec_test && /tmp/spec_test && rm /tmp/spec_test
```

**File header template** (spec):
```cpp
// <Feature name>  — introduced in C++NN
// What: one-line description of what the example demonstrates
// Why:  one-line description of why this feature was added / what problem it solves
// Pre:  how this was done before this standard (if applicable)
```

**File header template** (pattern):
```cpp
// <Pattern name>  — <category>
// Intent: one-line intent
// When to use:  short bullet list
// When NOT to use: short bullet list
```
