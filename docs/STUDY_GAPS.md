# Study Gaps

> Snapshot date: 2026-04-10
> Companion to [STUDY_INVENTORY.md](./STUDY_INVENTORY.md).
> Sources: cppreference C++NN feature lists (recall-based), GoF + modern pattern catalog (PoEAA, Cloud Design Patterns, POSA).
>
> Verification policy: items marked **(verify)** should be re-checked against cppreference
> immediately before a Phase 2 example is written.

Each gap is tagged:

- **H** — High priority. Foundational, frequently asked, or blocks understanding of later features.
- **M** — Medium. Useful but niche or supersededby a higher-priority cousin.
- **L** — Low. Edge cases, deprecated features, or rarely needed in practice.

---

## 1. C++ Standard Feature Gaps

### 1.1 C++98 — `spec/cpp/cpp98/`

> The current cpp98 directory only has 2 files. The intent of this directory needs clarification:
> is it a museum (legacy patterns to contrast with modern C++), or a "C++ basics" track?
> The gap list below assumes **museum** — only features whose modern replacement is studied later.

| Priority | Topic | Why | Modern counterpart |
|:--------:|-------|-----|--------------------|
| H | `functor.cpp` — function objects (`struct` with `operator()`) | Foundation for `std::function`, lambdas, ranges callable concept | C++11 lambdas |
| H | `mem_fun_ref.cpp` — `std::mem_fun`, `std::mem_fun_ref` | Pair with `bind1st_bind2nd.cpp` to complete the legacy callable picture | C++11 `std::mem_fn`, C++17 `std::invoke` |
| M | `auto_ptr.cpp` — `std::auto_ptr` (deprecated/removed) | Shows why move semantics were needed | C++11 `unique_ptr` |
| M | `iterator_categories.cpp` — input/output/forward/bidirectional/random-access | Foundation for understanding ranges | C++20 ranges |
| L | `template_specialization.cpp` — full and partial specialization | Already implicit in many examples | — |

### 1.2 C++11 — `spec/cpp/cpp11/`

22 files exist; the major C++11 surface is well covered. Remaining gaps:

| Priority | Topic | Notes |
|:--------:|-------|-------|
| H | `initializer_list.cpp` | `std::initializer_list<T>`, brace-init constructors |
| H | `chrono.cpp` | `<chrono>` durations, time points, clocks — foundational |
| H | `type_traits.cpp` | `<type_traits>` — basis for SFINAE and concepts later |
| H | `move_semantics.cpp` | Distinct from `rvalue_references.cpp` — focus on `std::move`, move ctor/assign, when copies become moves |
| H | `perfect_forwarding.cpp` | `std::forward`, universal references — usually paired with variadic templates |
| H | `delegating_inheriting_ctors.cpp` | Constructor delegation + inheriting ctors |
| H | `default_delete.cpp` | `= default`, `= delete` |
| H | `uniform_initialization.cpp` | `{}` brace-init, narrowing rules |
| M | `unordered_containers.cpp` | `std::unordered_map`, `unordered_set`, `unordered_multimap` |
| M | `regex.cpp` | `<regex>` basics |
| M | `random.cpp` | `<random>` engines + distributions |
| M | `raw_string_literals.cpp` | `R"(...)"`, char literal prefixes |
| M | `condition_variable.cpp` | Pair with `mutex_locks.cpp` |
| M | `nested_namespaces_alias.cpp` | Namespace aliases (note: nested `A::B` namespace syntax is C++17) |
| M | `ratio.cpp` | `<ratio>` — companion to chrono |
| L | `attribute_noreturn.cpp` | `[[noreturn]]` (introduced C++11) |
| L | `alignas_alignof.cpp` | Alignment control |
| L | `extended_friend.cpp` | Extended friend declarations |
| L | `trailing_return_type.cpp` | `auto f() -> T` |
| L | `explicit_conversion_op.cpp` | `explicit operator T()` |
| L | `unicode_literals.cpp` | `u8`, `u`, `U` string literals |

### 1.3 C++14 — `spec/cpp/cpp14/`

7 files exist. C++14 was a small release; remaining gaps are short.

| Priority | Topic | Notes |
|:--------:|-------|-------|
| H | `make_unique.cpp` | The big library addition of C++14 |
| H | `variable_templates.cpp` | `template<typename T> constexpr T pi = ...;` |
| M | `relaxed_constexpr.cpp` | C++14 loosened `constexpr` rules — show what newly works |
| M | `integer_sequence.cpp` | `std::index_sequence`, `make_index_sequence` — basis for tuple unpacking |
| M | `shared_timed_mutex.cpp` | Timed shared lock |
| L | `quoted.cpp` | `std::quoted` stream manipulator |
| L | `exchange.cpp` | `std::exchange` |
| L | `tuple_addressing_by_type.cpp` | `std::get<T>(tuple)` |

### 1.4 C++17 — `spec/cpp/cpp17/`

16 files exist; coverage is strong. Remaining gaps:

| Priority | Topic | Notes |
|:--------:|-------|-------|
| H | `class_template_argument_deduction.cpp` (CTAD) | `std::pair p{1, 2.0};` — foundational |
| H | `fold_expressions.cpp` | `(args + ...)` — completes variadic story |
| H | `charconv.cpp` | `std::from_chars`, `std::to_chars` — locale-free fast parsing |
| M | `clamp_sample.cpp` | `std::clamp`, `std::sample` |
| M | `gcd_lcm.cpp` | `std::gcd`, `std::lcm` |
| M | `has_include.cpp` | `__has_include` preprocessor |
| M | `nested_namespaces.cpp` | `namespace A::B::C { ... }` |
| M | `byte.cpp` | `std::byte` |
| M | `memory_resource.cpp` | `<memory_resource>` PMR allocators |
| M | `searcher.cpp` | `std::boyer_moore_searcher` |
| L | `launder.cpp` | `std::launder` (placement new edge cases) |
| L | `aligned_alloc.cpp` | Over-aligned `new` |
| L | `init_statement_if_switch.cpp` | `if (init; cond)` — minor but worth one short example |
| L | `template_template_auto.cpp` | `template<auto V>` |
| L | `pair_apply_constructor.cpp` | `std::piecewise_construct` (already implicit elsewhere) |

### 1.5 C++20 — `spec/cpp/cpp20/`

18 files exist. C++20 is large; several library highlights are still missing.

| Priority | Topic | Notes |
|:--------:|-------|-------|
| H | `concepts_requires.cpp` | Need to verify whether existing `concepts.cpp` already covers `requires` clauses, requires-expressions, custom concept definition. If not, expand or add. |
| H | `latch_barrier.cpp` | `std::latch`, `std::barrier` (verify against existing `sync_primitives.cpp`) |
| H | `semaphore.cpp` | `std::counting_semaphore`, `binary_semaphore` (verify) |
| H | `stop_token.cpp` | `std::stop_token` / `stop_source` — shown via `jthread` but worth a focused example |
| H | `bit_cast.cpp` | `std::bit_cast` (verify against existing `bit_manipulation.cpp`) |
| H | `designated_initializers.cpp` | `Foo{.x = 1, .y = 2}` |
| H | `aggregate_init_parens.cpp` | C++20 allows `()` for aggregates |
| H | `string_starts_ends_with.cpp` | `starts_with`, `ends_with`, `contains` (`contains` is C++23 — split) |
| H | `atomic_ref_atomic_shared_ptr.cpp` | `std::atomic_ref`, atomic smart pointers |
| H | `template_lambda.cpp` | `[]<typename T>(T x)` |
| M | `consteval_vs_constexpr.cpp` | Companion to existing `consteval.cpp` showing the contrast (could merge) |
| M | `concepts_standard_library.cpp` | `<concepts>` header — `same_as`, `derived_from`, `equality_comparable`, etc. |
| M | `ranges_factories.cpp` | `views::iota`, `views::repeat` (repeat is C++23) |
| M | `ranges_pipeline.cpp` | More elaborate pipeline than current `ranges.cpp` |
| M | `chrono_calendar.cpp` | C++20 calendar/timezone additions |
| M | `format_advanced.cpp` | Custom formatters via `std::formatter` specialization |
| M | `endian.cpp` | `std::endian` |
| M | `coroutine_generator.cpp` | Hand-written generator coroutine (C++23 has `std::generator`) |
| M | `lambda_capture_init.cpp` | C++14 introduced init-capture; C++20 added pack capture `[...args = ...]` — could be cpp14 instead |
| L | `using_enum_class.cpp` | Already has `using_enum.cpp` — verify it covers C++20 specifics |
| L | `feature_test_macros.cpp` | `__cpp_lib_*` macros |
| L | `char8_t_conversions.cpp` | Verify existing `char8_t.cpp` covers conversions |

### 1.6 C++23 — `spec/cpp/cpp23/`

9 files exist. C++23 is the most active gap area.

| Priority | Topic | Notes |
|:--------:|-------|-------|
| H | `flat_map_set.cpp` | `std::flat_map`, `std::flat_set` (libc++ support varies — verify before writing) |
| H | `mdspan.cpp` | `std::mdspan` (libc++ support varies) |
| H | `generator.cpp` | `std::generator<T>` coroutine (libc++ support varies) |
| H | `ranges_to.cpp` | `std::ranges::to<Container>()` |
| H | `deducing_this.cpp` | Explicit object parameter — major language feature |
| H | `multidimensional_subscript.cpp` | `m[i, j]` operator |
| H | `if_consteval.cpp` | **Already exists** — verify content |
| H | `static_call_operator.cpp` | `static operator()` |
| H | `string_contains.cpp` | `std::string::contains` |
| M | `expected_monadic.cpp` | `expected::and_then` etc. — pair with existing `expected.cpp` |
| M | `ranges_zip_enumerate.cpp` | `views::zip`, `views::enumerate`, `views::adjacent` |
| M | `ranges_chunk_slide.cpp` | `views::chunk`, `views::slide` |
| M | `ranges_join_with.cpp` | `views::join_with` |
| M | `ranges_cartesian.cpp` | `views::cartesian_product` |
| M | `print_println.cpp` | **Already exists as `print.cpp`** — verify covers `println`, `vprint_*` |
| M | `expected_basic.cpp` | **Already exists** — verify |
| M | `byteswap.cpp` | **Already exists** — verify |
| M | `to_underlying.cpp` | **Already exists** — verify |
| M | `unreachable.cpp` | **Already exists** — verify |
| M | `monadic_optional.cpp` | **Already exists** — verify |
| M | `out_ptr_inout_ptr.cpp` | C interop helpers |
| M | `byteswap_endian.cpp` | Already covered in cpp20 endian + cpp23 byteswap |
| M | `move_only_function.cpp` | `std::move_only_function` |
| L | `constexpr_unique_ptr.cpp` | constexpr `unique_ptr` and other constexpr expansions |
| L | `auto_cast.cpp` | `auto(x)` decay copy |
| L | `multidim_array_subscript.cpp` | Same as `multidimensional_subscript.cpp` |
| L | `consteval_propagation.cpp` | `if consteval` interactions |
| L | `size_t_literal.cpp` | `1uz` literal suffix |
| L | `extended_floating_point.cpp` | `std::float16_t`, `std::bfloat16_t`, etc. (limited support) |

---

## 2. Design Pattern Gaps — `dp/cpp/`

### 2.1 GoF (23/23 present)

**No gaps.** All 23 GoF patterns have a corresponding `.cpp` file.
Quality review of existing files is out of scope.

### 2.2 Modern / enterprise / concurrency / cloud patterns

Currently present (7): DI, Repository, MVC, Null Object, Object Pool, Circuit Breaker, Event Sourcing.

Patterns worth adding for "comprehensive practical coverage":

#### Concurrency patterns (POSA2)

| Priority | Pattern              | Why |
|:--------:|----------------------|-----|
| H | `active_object.cpp`         | Decouples method invocation from execution; classic POSA2 |
| H | `producer_consumer.cpp`     | Blocking queue / bounded buffer — foundational |
| H | `thread_pool.cpp`           | Worker pool with task queue |
| H | `monitor_object.cpp`        | Synchronized class methods (paired with mutex/condvar) |
| M | `reactor.cpp`               | Event demultiplexing — POSA2 |
| M | `proactor.cpp`              | Async I/O dispatch — POSA2 |
| M | `double_checked_locking.cpp`| Singleton variant + classic example of why it's tricky |
| M | `read_write_lock.cpp`       | Use of `shared_mutex` in pattern form |
| L | `leader_followers.cpp`      | Server thread coordination — niche |
| L | `half_sync_half_async.cpp`  | Layered async — niche |

#### Architectural / enterprise patterns (PoEAA, DDD)

| Priority | Pattern                | Why |
|:--------:|------------------------|-----|
| H | `cqrs.cpp`                    | Command Query Responsibility Segregation — pairs with existing event_sourcing |
| H | `unit_of_work.cpp`            | PoEAA — DB transaction boundary |
| H | `specification.cpp`           | Composable business rules / query criteria |
| H | `service_locator.cpp`         | Counterpart/contrast to existing DI example |
| M | `lazy_initialization.cpp`     | `std::call_once`, `std::once_flag`, lazy holders |
| M | `data_mapper.cpp`             | PoEAA — separates domain model from DB schema |
| M | `value_object.cpp`            | DDD building block |
| M | `domain_event.cpp`            | DDD — pairs with event_sourcing |
| M | `saga.cpp`                    | Distributed transaction coordination — pairs with cqrs |
| M | `pipes_and_filters.cpp`       | Stream processing topology |
| L | `table_module.cpp`            | PoEAA — DB-table-centric module |
| L | `transaction_script.cpp`      | PoEAA — opposite of domain model |

#### Resilience / cloud patterns (Cloud Design Patterns)

| Priority | Pattern              | Why |
|:--------:|----------------------|-----|
| H | `retry.cpp`                 | Pairs with existing `circuit_breaker.cpp` |
| H | `bulkhead.cpp`              | Resource isolation — resilience trio with retry/circuit breaker |
| H | `timeout.cpp`               | Often missing in real systems |
| M | `throttling.cpp`            | Rate limiting / backpressure |
| M | `cache_aside.cpp`           | Cache + repository combo |
| M | `health_endpoint.cpp`       | `/health` checker pattern |
| L | `ambassador.cpp`            | Sidecar networking proxy — usually distributed |
| L | `compensating_transaction.cpp` | Pairs with saga |

#### Functional / modern C++ idiom patterns

| Priority | Pattern              | Why |
|:--------:|----------------------|-----|
| H | `pimpl.cpp`                 | Pointer-to-implementation — C++ idiomatic |
| H | `crtp.cpp`                  | Curiously Recurring Template Pattern |
| H | `type_erasure.cpp`          | `std::function`-style erasure technique |
| H | `tag_dispatch.cpp`          | Compile-time strategy selection |
| H | `policy_based_design.cpp`   | Andrei Alexandrescu-style policy classes |
| M | `expression_templates.cpp`  | Numeric DSL technique |
| M | `mixin_via_crtp.cpp`        | Variant of CRTP |
| M | `nullable_object.cpp`       | Could fold into existing `null_object.cpp` |
| M | `raii_lock_token.cpp`       | RAII as pattern, not just idiom |
| L | `tag_invoke.cpp`            | Customization point design — niche but modern |

---

## 3. Suggested Phase 2 starting batches

These are **not** prescriptions — just convenient grouping if user wants to start somewhere
specific. Each batch is ~4–6 files, sized for a single working session.

### Spec batches

- **Batch S1 (cpp11 foundations)**: `chrono`, `type_traits`, `initializer_list`, `move_semantics`, `perfect_forwarding`
- **Batch S2 (cpp14 small)**: `make_unique`, `variable_templates`, `relaxed_constexpr`, `integer_sequence`
- **Batch S3 (cpp17 highlights)**: `class_template_argument_deduction`, `fold_expressions`, `charconv`, `byte`
- **Batch S4 (cpp20 concurrency)**: `latch_barrier`, `semaphore`, `stop_token`, `atomic_ref`
- **Batch S5 (cpp20 language)**: `designated_initializers`, `aggregate_init_parens`, `template_lambda`, `string_starts_ends_with`
- **Batch S6 (cpp23 ranges)**: `ranges_to`, `ranges_zip_enumerate`, `ranges_chunk_slide`, `ranges_join_with`
- **Batch S7 (cpp23 language)**: `deducing_this`, `multidimensional_subscript`, `static_call_operator`, `move_only_function`

### Pattern batches

- **Batch P1 (concurrency essentials)**: `producer_consumer`, `thread_pool`, `active_object`, `monitor_object`
- **Batch P2 (resilience)**: `retry`, `bulkhead`, `timeout`, `throttling`
- **Batch P3 (modern C++ idioms)**: `pimpl`, `crtp`, `type_erasure`, `tag_dispatch`
- **Batch P4 (DDD/CQRS family)**: `cqrs`, `specification`, `unit_of_work`, `value_object`
- **Batch P5 (legacy/contrast)**: `service_locator`, `lazy_initialization`, `double_checked_locking`

---

## 4. Open questions to resolve before Phase 2

1. **cpp98 intent** — museum or basics? Affects whether we add `iterator_categories.cpp` etc.
2. **Existing-file quality review** — explicitly out of scope. If user wants to revisit, that becomes a separate phase.
3. **Verification mode** — should every cpp23 example be compiled before commit, or written-then-batch-verified?
4. **Build system for `spec/cpp`** — add CMake (Phase 4) or keep ad-hoc compile-on-demand?
5. **Pattern scope** — adopt all H-tagged patterns above, or trim? The "all patterns" interpretation produces ~25+ new files in `dp/cpp/`.
