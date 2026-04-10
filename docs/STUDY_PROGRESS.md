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

#### High (10) — 8 new (latch+semaphore: sync_primitives.cpp; bit_cast: bit_manipulation.cpp)
- [x] H `concepts_requires.cpp` — `requires` clauses, custom concepts
- [x] H `barrier.cpp` — `std::barrier` (latch already in sync_primitives.cpp)
- [-] H `semaphore` — already covered by `sync_primitives.cpp`
- [x] H `stop_token.cpp` — explicit stop_source/stop_callback
- [-] H `bit_cast` — already covered by `bit_manipulation.cpp`
- [x] H `designated_initializers.cpp`
- [x] H `aggregate_init_parens.cpp`
- [x] H `string_starts_ends_with.cpp`
- [x] H `atomic_ref.cpp`
- [x] H `template_lambda.cpp` — `[]<typename T>(T x)`

#### Medium (8) — 7 new (endian: bit_manipulation.cpp)
- [x] M `concepts_standard_library.cpp`
- [x] M `ranges_factories.cpp`
- [x] M `ranges_pipeline.cpp`
- [x] M `chrono_calendar.cpp`
- [x] M `format_advanced.cpp`
- [-] M `endian` — already covered by `bit_manipulation.cpp`
- [x] M `coroutine_generator.cpp`
- [x] M `lambda_pack_capture.cpp`

#### Low (3)
- [x] L `feature_test_macros.cpp`
- [x] L `consteval_propagation.cpp`
- [x] L `aggregate_designated_array.cpp`

### cpp23 — 27 files (`-std=c++2b` / `-std=c++23`)

> libc++ feature support varies. Use `#if __has_include(<header>)` guards for
> known-fragile items: `flat_map`, `mdspan`, `generator`.

#### High (9)
- [x] H `flat_map_set.cpp`
- [x] H `mdspan.cpp`
- [!] H `generator.cpp` — guarded; libc++ lacks `<generator>`, see cpp20/coroutine_generator.cpp
- [x] H `ranges_to.cpp`
- [x] H `deducing_this.cpp`
- [x] H `multidimensional_subscript.cpp`
- [x] H `static_call_operator.cpp`
- [x] H `string_contains.cpp`
- [!] H `move_only_function.cpp` — guarded; libc++ lacks `std::move_only_function`

#### Medium (12)
- [x] M `expected_monadic.cpp`
- [!] M `ranges_zip.cpp` — guarded; libc++ lacks `views::zip`
- [!] M `ranges_enumerate.cpp` — guarded
- [!] M `ranges_adjacent.cpp` — guarded
- [!] M `ranges_chunk.cpp` — guarded
- [!] M `ranges_slide.cpp` — guarded
- [x] M `ranges_join_with.cpp`
- [!] M `ranges_cartesian.cpp` — guarded
- [x] M `ranges_repeat.cpp`
- [x] M `out_ptr.cpp`
- [x] M `print_println.cpp`
- [x] M `expected_constructors.cpp`

#### Low (6)
- [x] L `constexpr_unique_ptr.cpp`
- [x] L `auto_decay_copy.cpp`
- [x] L `size_t_literal.cpp`
- [!] L `extended_floating_point.cpp` — guarded; libc++ lacks `<stdfloat>`
- [x] L `consteval_if_propagation.cpp`
- [x] L `assume_attribute.cpp`

---

## Pattern gaps — 35 files (`dp/cpp/`)

### Concurrency patterns (10)
- [x] H `producer_consumer.cpp`
- [x] H `thread_pool.cpp`
- [x] H `active_object.cpp`
- [x] H `monitor_object.cpp`
- [x] M `reactor.cpp`
- [x] M `proactor.cpp`
- [x] M `double_checked_locking.cpp`
- [x] M `read_write_lock.cpp`
- [x] L `leader_followers.cpp`
- [x] L `half_sync_half_async.cpp`

### Architectural / enterprise / DDD (12)
- [x] H `cqrs.cpp`
- [x] H `unit_of_work.cpp`
- [x] H `specification.cpp`
- [x] H `service_locator.cpp`
- [x] M `lazy_initialization.cpp`
- [x] M `data_mapper.cpp`
- [x] M `value_object.cpp`
- [x] M `domain_event.cpp`
- [x] M `saga.cpp`
- [x] M `pipes_and_filters.cpp`
- [x] L `table_module.cpp`
- [x] L `transaction_script.cpp`

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
- cpp17 H (3) + M (7) + L (5) = 15 files; cpp17 fully done (15/15)
- All 15 new files compiled with `-std=c++17 -Wall -Wextra` and run successfully
- Note: clangd IDE diagnostics show false positives (defaults to older standard) — only `clang++ -std=c++NN` results count
- **Session 3 total: 15. Cumulative: 49 / 132 (37.1%)**

### Session 4 — 2026-04-10
- cpp20 H 8 new + M 7 new + L 3 new = 18 files
- 3 items already covered by existing files (latch+semaphore, bit_cast, endian)
- cpp20 gap items: 21/21
- **Session 4 total: 18. Cumulative: 67 / 132 (50.8%)**

### Session 5 — 2026-04-10
- cpp23 H 9 + M 12 + L 6 = 27 files; cpp23 fully done
- 9 files use `#if defined(__cpp_lib_*)` guards with fallbacks for libc++ gaps
- **Session 5 total: 27. Cumulative: 94 / 132 (71.2%)**

### Session 6 — 2026-04-10
- Concurrency 10/10 complete
- **Session 6 total: 10. Cumulative: 104 / 132 (78.8%)**

### Session 7 — 2026-04-10
- Architectural/DDD H (4): cqrs, unit_of_work, specification, service_locator
- Architectural/DDD M (6): lazy_initialization, data_mapper, value_object, domain_event, saga, pipes_and_filters
- Architectural/DDD L (2): table_module, transaction_script
- Architectural/DDD fully done (12/12)
- All 12 files compiled with `clang++ -std=c++17` and run successfully
- **Session 7 total: 12 files. Cumulative: 116 / 132 (87.9%)**

### Next session starts at

> Read this section first when resuming.

**Pointer**: pattern gaps continue — 13 remaining in `dp/cpp/`:
1. ~~Concurrency (10)~~ ✅
2. ~~Architectural/DDD (12)~~ ✅
3. Resilience (8): retry, bulkhead, timeout, throttling, cache_aside, health_endpoint, ambassador, compensating_transaction
4. Modern C++ idioms (5): pimpl, crtp, type_erasure, tag_dispatch, policy_based_design

Pattern files are richer than spec files (~150-300 lines, 2-4 example domains, modern C++ variant). Match the style of existing `dp/cpp/strategy.cpp`. CMakeLists.txt is glob-based — new files are picked up automatically.

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
