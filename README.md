# Study Programming

C++ 언어 스펙, 디자인 패턴, 설계 원칙, 아키텍처를 체계적으로 학습하기 위한 예제 모음.

## 구조

```
study_programming/
├── spec/cpp/          # C++ 표준별 기능 예제 (97 files)
│   ├── cpp98/         #   C++98 — 7 files
│   ├── cpp11/         #   C++11 — 43 files
│   ├── cpp14/         #   C++14 — 15 files
│   ├── cpp17/         #   C++17 — 31 files (incl. extended set)
│   ├── cpp20/         #   C++20 — 35 files
│   └── cpp23/         #   C++23 — 36 files
├── dp/                # 디자인 패턴 구현 (65 files per language)
│   ├── cpp/           #   C++ — GoF 23 + concurrency + arch/DDD + resilience + idioms
│   ├── c/             #   C
│   └── rust/          #   Rust
├── solid/cpp/         # SOLID 설계 원칙 (7 files)
├── arch/cpp/          # 소프트웨어 아키텍처 패턴 (5 files)
├── ddd/cpp/           # Domain-Driven Design 심화 (6 files)
└── docs/              # 진행 추적 문서
    ├── STUDY_PROGRESS.md
    ├── STUDY_GAPS.md
    └── STUDY_INVENTORY.md
```

## 빌드

### spec/cpp

```bash
cd spec/cpp
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```

실행 파일은 `build/bin/{standard}/` 하위에 생성됩니다.

> `modules_main.cpp`은 C++20 모듈 프리컴파일이 필요하여 CMake 빌드에서 제외됩니다.

### dp/cpp

```bash
cd dp/cpp
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```

## 컴파일러 요구사항

- Apple Clang 15+ 또는 GCC 13+ / Clang 17+
- C++23 기능 중 일부는 `__has_include` / `__cpp_lib_*` 가드 처리됨 (libc++ 미지원 시 fallback)

## 진행 현황

| 영역 | 파일 수 | 상태 |
|------|---------|------|
| spec/cpp (C++98~23) | 180 | 전체 빌드 검증 완료 |
| dp/cpp (디자인 패턴) | 65 | 전체 빌드 검증 완료 |
| dp/c (디자인 패턴) | 65 | 전체 빌드 검증 완료 |
| dp/rust (디자인 패턴) | 65 | 전체 빌드 검증 완료 |
| solid/cpp (설계 원칙) | 7 | 전체 빌드 검증 완료 |
| arch/cpp (아키텍처) | 5 | 전체 빌드 검증 완료 |
| ddd/cpp (DDD) | 6 | 전체 빌드 검증 완료 |

자세한 항목별 진행 상황은 [docs/STUDY_PROGRESS.md](docs/STUDY_PROGRESS.md)를 참고하세요.

## 학습 로드맵

단계별 학습 순서는 [docs/ROADMAP.md](docs/ROADMAP.md)를 참고하세요.
