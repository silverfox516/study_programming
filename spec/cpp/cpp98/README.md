# C++98 — Museum Track

This directory holds **historical examples** of C++98/03 features that have either been
deprecated or fully removed in later standards. Their purpose is comparative learning:
"this is what we used to do, and this is why it was replaced".

## Important: most files here will NOT compile under modern C++

| File                       | Status                                       | Modern replacement                  |
|----------------------------|----------------------------------------------|-------------------------------------|
| `bind1st_bind2nd.cpp`      | `bind1st`/`bind2nd` removed in **C++17**     | `std::bind` (C++11), lambdas (C++11)|
| `mem_fun.cpp`              | `mem_fun`/`mem_fun_ref` removed in **C++17** | `std::mem_fn` (C++11)               |
| `auto_ptr.cpp`             | `std::auto_ptr` removed in **C++17**         | `std::unique_ptr` (C++11)           |
| `functor.cpp`              | Still legal — function objects               | Lambdas (C++11)                     |
| `for_each.cpp`             | Still legal                                  | Range-based for (C++11), ranges (C++20) |
| `iterator_categories.cpp`  | Still legal — iterator tag dispatch          | Concepts (C++20), ranges (C++20)    |
| `template_specialization.cpp` | Still legal                              | (no replacement — still useful)     |

## How to compile

Use **`-std=c++14`** for the museum files. C++14 is the latest standard that still
contains all of these:

```bash
clang++ -std=c++14 -Wno-deprecated-declarations \
    spec/cpp/cpp98/<file>.cpp -o /tmp/x && /tmp/x
```

The `-Wno-deprecated-declarations` flag silences the warnings about `auto_ptr`,
`bind1st`, etc. You will see the warnings without it — that is by design, the
compiler is telling you exactly what this directory documents.

## What this directory is NOT

- It is **not** a "C++ basics" tutorial. For modern C++ basics, start with `cpp11/`.
- It is **not** an exhaustive C++98 feature list. Only features whose evolution is
  worth tracking from C++98 → C++11 → C++14 → C++17 → C++20 → C++23 are kept here.

## Why C++98 examples at all?

C++98 itself is not deprecated — it is the foundation of every later standard.
But several specific *library* and *language* features from C++98 were declared
obsolete and eventually removed:

- `std::auto_ptr` (deprecated C++11, removed C++17) — broken move semantics
- `std::bind1st` / `std::bind2nd` (deprecated C++11, removed C++17) — replaced by `std::bind`
- `std::ptr_fun` (deprecated C++11, removed C++17)
- `std::mem_fun` / `std::mem_fun_ref` (deprecated C++11, removed C++17) — replaced by `std::mem_fn`
- `std::unary_function` / `std::binary_function` (deprecated C++11, removed C++17)
- `std::random_shuffle` (deprecated C++14, removed C++17) — replaced by `std::shuffle`
- Dynamic exception specifications `throw(T)` (deprecated C++11, removed C++17) — replaced by `noexcept`
- `register` keyword (removed C++17)

Studying these alongside their modern replacements is the best way to internalize
**why** modern C++ took the shape it did.
