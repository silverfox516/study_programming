# C++ 학습 로드맵

이 문서는 `spec/cpp/`와 `dp/cpp/` 예제를 단계별로 학습하기 위한 가이드입니다.

---

## Phase 1: 기초 (C++98/11 핵심)

C++11이 사실상 "Modern C++"의 출발점입니다. C++98은 레거시 이해를 위해 참고만 합니다.

### 1-1. C++98 레거시 (참고용)
- `auto_ptr.cpp` — 왜 deprecated 되었는지 이해
- `functor.cpp`, `mem_fun.cpp`, `bind1st_bind2nd.cpp` — lambda 이전의 callable
- `iterator_categories.cpp`, `template_specialization.cpp`

### 1-2. C++11 — 타입과 초기화
- `auto.cpp` → `decltype` (cpp14 `decltype_auto.cpp`)
- `nullptr.cpp`, `enum_class.cpp`, `static_assert.cpp`
- `uniform_initialization.cpp`, `initializer_list.cpp`
- `range_based_for.cpp`

### 1-3. C++11 — 리소스 관리
- `rvalue_references.cpp` → `move_semantics.cpp` → `perfect_forwarding.cpp`
- `smart_pointers.cpp` → `weak_ptr.cpp`
- `default_delete.cpp`, `override_final.cpp`
- `delegating_inheriting_ctors.cpp`

### 1-4. C++11 — 함수형 스타일
- `lambda_expressions.cpp` → `bind.cpp`
- `variadic_templates.cpp`
- `trailing_return_type.cpp`, `explicit_conversion_op.cpp`

### 1-5. C++11 — 동시성
- `threads.cpp` → `mutex_locks.cpp` → `condition_variable.cpp`
- `atomic.cpp`, `thread_local.cpp`, `futures.cpp`

### 1-6. C++11 — 라이브러리
- `array.cpp`, `tuple.cpp`, `unordered_containers.cpp`
- `chrono.cpp`, `regex.cpp`, `random.cpp`
- `type_traits.cpp`, `ratio.cpp`
- `user_defined_literals.cpp`, `raw_string_literals.cpp`

---

## Phase 2: 중급 (C++14/17)

### 2-1. C++14 — 편의 개선
- `generic_lambdas.cpp`, `return_type_deduction.cpp`, `decltype_auto.cpp`
- `make_unique.cpp`, `integer_sequence.cpp`
- `binary_literals.cpp`, `digit_separators.cpp`
- `relaxed_constexpr.cpp`, `variable_templates.cpp`

### 2-2. C++17 — 어휘 타입
- `optional.cpp` → `variant.cpp` → `any.cpp`
- `string_view.cpp`, `structured_bindings.cpp`

### 2-3. C++17 — 제어 흐름과 메타프로그래밍
- `if_constexpr.cpp`, `fold_expressions.cpp`
- `class_template_argument_deduction.cpp`
- `inline_variables.cpp`, `nested_namespaces.cpp`

### 2-4. C++17 — 라이브러리
- `filesystem.cpp`, `parallel_algorithms.cpp`
- `shared_mutex.cpp`, `scoped_lock.cpp`
- `invoke.cpp`, `apply.cpp`

---

## Phase 3: 고급 (C++20)

### 3-1. C++20 — 4대 기능
- `concepts.cpp` → `concepts_requires.cpp` → `concepts_standard_library.cpp`
- `ranges.cpp` → `ranges_factories.cpp` → `ranges_pipeline.cpp`
- `coroutines.cpp` → `coroutine_generator.cpp`
- `modules_main.cpp` (모듈 시스템 — 별도 컴파일 필요)

### 3-2. C++20 — 동시성
- `jthread.cpp`, `stop_token.cpp`, `barrier.cpp`, `sync_primitives.cpp`

### 3-3. C++20 — 기타
- `three_way_comparison.cpp` (우주선 연산자)
- `consteval.cpp`, `constinit.cpp`
- `format.cpp` → `format_advanced.cpp`
- `span.cpp`, `designated_initializers.cpp`

---

## Phase 4: 최신 (C++23)

### 4-1. C++23 — 핵심
- `expected.cpp` → `expected_monadic.cpp`
- `monadic_optional.cpp`
- `deducing_this.cpp` (explicit object parameter)
- `print.cpp` → `print_println.cpp`

### 4-2. C++23 — Ranges 확장
- `ranges_to.cpp`, `ranges_zip.cpp`, `ranges_enumerate.cpp`
- `ranges_chunk.cpp`, `ranges_slide.cpp`
- `ranges_adjacent.cpp`, `ranges_cartesian.cpp`

### 4-3. C++23 — 기타
- `generator.cpp`, `mdspan.cpp`
- `if_consteval.cpp`, `static_call_operator.cpp`
- `constexpr_unique_ptr.cpp`

---

## Phase 5: 디자인 패턴 (`dp/cpp/`)

### 5-1. GoF — 생성
- `singleton.cpp`, `factory.cpp` → `factory_improved.cpp` → `abstract_factory.cpp`
- `builder.cpp`, `prototype.cpp`, `object_pool.cpp`

### 5-2. GoF — 구조
- `adapter.cpp`, `bridge.cpp`, `composite.cpp`
- `decorator.cpp`, `facade.cpp`, `flyweight.cpp`, `proxy.cpp`

### 5-3. GoF — 행위
- `strategy.cpp`, `observer.cpp`, `command.cpp`
- `state.cpp`, `template_method.cpp`, `iterator.cpp`
- `mediator.cpp`, `memento.cpp`, `visitor.cpp`
- `chain_of_responsibility.cpp`, `interpreter.cpp`
- `null_object.cpp`

### 5-4. 동시성 패턴
- `producer_consumer.cpp`, `thread_pool.cpp`, `active_object.cpp`
- `monitor_object.cpp`, `reactor.cpp`, `proactor.cpp`
- `double_checked_locking.cpp`, `read_write_lock.cpp`
- `leader_followers.cpp`, `half_sync_half_async.cpp`

### 5-5. 아키텍처/DDD 패턴
- `cqrs.cpp`, `event_sourcing.cpp`, `saga.cpp`
- `unit_of_work.cpp`, `repository.cpp`, `data_mapper.cpp`
- `specification.cpp`, `value_object.cpp`, `domain_event.cpp`
- `service_locator.cpp`, `dependency_injection.cpp`
- `pipes_and_filters.cpp`, `table_module.cpp`, `transaction_script.cpp`
- `mvc.cpp`

### 5-6. 레질리언스 패턴
- `circuit_breaker.cpp`, `retry.cpp`, `bulkhead.cpp`
- `timeout.cpp`, `throttling.cpp`, `cache_aside.cpp`
- `health_endpoint.cpp`, `ambassador.cpp`, `compensating_transaction.cpp`

### 5-7. Modern C++ 관용구
- `pimpl.cpp`, `crtp.cpp`, `type_erasure.cpp`
- `tag_dispatch.cpp`, `policy_based_design.cpp`

---

## 학습 팁

1. **빌드 후 실행**: 각 예제를 직접 빌드하고 실행해보세요
2. **코드 변형**: 예제를 수정하며 컴파일러 에러와 경고를 경험하세요
3. **표준 비교**: 같은 문제를 다른 표준으로 풀어보세요 (e.g., C++11 bind vs C++14 generic lambda)
4. **패턴 적용**: 디자인 패턴 예제를 자신의 프로젝트에 적용해보세요
