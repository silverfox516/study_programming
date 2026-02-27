# Design Patterns Implementation in C, C++, and Rust

이 프로젝트는 23개의 GoF 디자인 패턴과 현대적인 추가 패턴들을 C, C++, Rust 세 언어로 구현한 포괄적인 레퍼런스입니다. 각 패턴은 실용적인 예제와 함께 구현되었으며, 각 언어의 특성을 활용한 관용적인(idiomatic) 코드로 작성되었습니다.

## 📁 프로젝트 구조

```
design-patterns/
├── c/                     # C 언어 구현
│   ├── CMakeLists.txt     # C 전용 CMake 설정
│   └── *.c               # 각 패턴 구현
├── cpp/                   # C++ 언어 구현
│   ├── CMakeLists.txt     # C++ 전용 CMake 설정
│   └── *.cpp             # 각 패턴 구현
├── rust/                  # Rust 언어 구현
│   ├── Cargo.toml        # Rust 프로젝트 설정
│   └── src/              # 소스 디렉토리
│       └── *.rs          # 각 패턴 구현
└── README.md             # 이 파일
```

## 🚀 빠른 시작

### C 언어 빌드 및 실행

```bash
cd c
mkdir build && cd build
cmake ..
make

# 특정 패턴 실행 (예: factory)
./factory
```

### C++ 빌드 및 실행

```bash
cd cpp
mkdir build && cd build
cmake ..
make

# 특정 패턴 실행 (예: factory)
./factory
```

### Rust 빌드 및 실행

```bash
cd rust

# 모든 패턴 빌드
cargo build --release

# 특정 패턴 실행 (예: factory)
cargo run --bin factory

# C 패턴만 빌드 및 실행
make build-c run-c

# C++ 패턴만 빌드 및 실행
make build-cpp run-cpp

# Rust 패턴만 빌드 및 실행
make build-rust run-rust
```

### CMake 사용

```bash
mkdir build && cd build
cmake ..
make
ctest  # 테스트 실행
```

### 개별 패턴 실행

**C:**
```bash
gcc -o singleton c/singleton.c && ./singleton
```

**C++:**
```bash
g++ -std=c++17 -o singleton cpp/singleton.cpp && ./singleton
```

**Rust:**
```bash
cd rust && cargo run --bin singleton
```

## 📋 구현된 디자인 패턴

### 🏗️ 생성 패턴 (Creational Patterns)

| 패턴 | C | C++ | Rust | 설명 |
|------|---|-----|------|------|
| **Singleton** | ✅ | ✅ | ✅ | 클래스의 인스턴스가 하나만 존재하도록 보장 |
| **Factory** | ✅ | ✅ | ✅ | 객체 생성 로직을 캡슐화 |
| **Abstract Factory** | ✅ | ✅ | ✅ | 관련된 객체들의 패밀리를 생성 |
| **Builder** | ✅ | ✅ | ✅ | 복잡한 객체를 단계별로 구성 |
| **Prototype** | ✅ | ✅ | ✅ | 기존 객체를 복제하여 새 객체 생성 |

### 🔧 구조 패턴 (Structural Patterns)

| 패턴 | C | C++ | Rust | 설명 |
|------|---|-----|------|------|
| **Adapter** | ✅ | ✅ | ✅ | 호환되지 않는 인터페이스를 연결 |
| **Bridge** | ✅ | ✅ | ✅ | 추상화와 구현을 분리 |
| **Composite** | ✅ | ✅ | ✅ | 객체들을 트리 구조로 구성 |
| **Decorator** | ✅ | ✅ | ✅ | 객체에 동적으로 기능 추가 |
| **Facade** | ✅ | ✅ | ✅ | 복잡한 서브시스템에 단순한 인터페이스 제공 |
| **Flyweight** | ✅ | ✅ | ✅ | 많은 수의 객체를 효율적으로 공유 |
| **Proxy** | ✅ | ✅ | ✅ | 다른 객체에 대한 대리자 역할 |

### ⚡ 행동 패턴 (Behavioral Patterns)

| 패턴 | C | C++ | Rust | 설명 |
|------|---|-----|------|------|
| **Observer** | ✅ | ✅ | ✅ | 객체 상태 변화를 다른 객체들에게 통지 |
| **Strategy** | ✅ | ✅ | ✅ | 알고리즘을 런타임에 선택 |
| **Command** | ✅ | ✅ | ✅ | 요청을 객체로 캡슐화 |
| **State** | ✅ | ✅ | ✅ | 내부 상태에 따라 객체 행동 변경 |
| **Template Method** | ✅ | ✅ | ✅ | 알고리즘의 구조를 정의하고 세부사항은 하위 클래스에서 구현 |
| **Visitor** | ✅ | ✅ | ✅ | 객체 구조의 원소들에 대해 수행할 연산을 정의 |
| **Chain of Responsibility** | ✅ | ✅ | ✅ | 요청을 처리할 수 있는 객체들의 체인 구성 |
| **Mediator** | ✅ | ✅ | ✅ | 객체 간의 상호작용을 중재자를 통해 처리 |
| **Memento** | ✅ | ✅ | ✅ | 객체의 상태를 저장하고 복원 |
| **Iterator** | ✅ | ✅ | ✅ | 컬렉션의 원소들에 순차적으로 접근 |
| **Interpreter** | ✅ | ✅ | ✅ | 언어의 문법을 표현하고 해석 |

## 💡 언어별 특징

### C 구현 특징
- **함수 포인터**를 사용한 다형성 구현
- **구조체 기반** 객체 지향 시뮬레이션
- **명시적 메모리 관리** (malloc/free)
- **실용적인 예제**: 로거, 데이터베이스 연결, 파일 시스템 등

### C++ 구현 특징
- **현대적 C++ (C++17/20)** 기능 활용
- **스마트 포인터** (unique_ptr, shared_ptr) 사용
- **RAII 원칙** 준수
- **STL 컨테이너**와 알고리즘 활용
- **람다 함수**와 함수형 프로그래밍
- **템플릿 메타프로그래밍**

### Rust 구현 특징
- **소유권 시스템**과 빌림 검사기 활용
- **트레이트(Trait)** 기반 다형성
- **열거형(Enum)**과 패턴 매칭
- **Result 타입**을 통한 에러 처리
- **제로 코스트 추상화**
- **메모리 안전성** 보장

## 🛠️ 빌드 시스템

### Makefile 명령어

```bash
make help              # 도움말 표시
make all               # 모든 언어 빌드
make build-c           # C 패턴만 빌드
make build-cpp         # C++ 패턴만 빌드
make build-rust        # Rust 패턴만 빌드
make run-c             # C 패턴 실행
make run-cpp           # C++ 패턴 실행
make run-rust          # Rust 패턴 실행
make test              # 모든 패턴 테스트
make clean             # 빌드 아티팩트 정리
make lint              # 코드 린팅
make benchmark         # 성능 벤치마크
make package           # 배포 패키지 생성
```

### CMake 빌드

```bash
mkdir build && cd build
cmake ..
make
make test
```

### Rust Cargo

```bash
cd rust
cargo build --release
cargo run --bin <pattern_name>
cargo test
```

## 📚 실용적인 예제들

각 패턴은 실제 소프트웨어 개발에서 사용되는 실용적인 예제로 구현되었습니다:

- **Singleton**: 데이터베이스 연결, 로거, 설정 관리
- **Factory**: 도형 생성, UI 컴포넌트 팩토리
- **Observer**: 날씨 스테이션, 이벤트 시스템
- **Strategy**: 결제 시스템, 정렬 알고리즘
- **Command**: 텍스트 에디터, 스마트 홈 제어
- **Decorator**: 커피숍 주문, 텍스트 포맷팅
- **Facade**: 홈 시어터 시스템, 컴퓨터 부팅

## 🆕 추가 현대 패턴들

GoF 23 패턴 외에 현대 소프트웨어 개발에서 자주 사용되는 패턴들:

### 아키텍처 패턴
- **MVC (Model-View-Controller)**: 사용자 관리 시스템
- **Repository**: 데이터 접근 계층 추상화
- **Event Sourcing**: 은행 계좌 이벤트 기록

### 의존성 관리 패턴
- [ ] Dependency Injection: 서비스 계층 의존성 주입
- [ ] Null Object: 기본값 제공 및 예외 방지

### 시스템 안정성 & 성능 패턴
- **Object Pool**: 객체 재사용을 통한 할당 비용 절감
- **Circuit Breaker**: 장애 전파 방지 및 시스템 안정성 확보

### 실제 예제 구현
- **MVC**: 사용자 CRUD 시스템 (Console/JSON 뷰)
- **Repository**: 제품 관리 (In-Memory/File 저장소)
- **DI**: 로거, 데이터베이스, 이메일 서비스 주입
- [ ] Event Sourcing: 은행 계좌 거래 이벤트 추적
- [ ] Null Object: 고객 시스템의 게스트 처리
- **Object Pool**: DB 커넥션 풀 시뮬레이션
- **Circuit Breaker**: 불안정한 외부 서비스 호출 제어

## 🧪 테스트 및 검증

모든 패턴은 다음과 같이 테스트되었습니다:

- **단위 테스트**: 각 패턴의 핵심 기능 검증
- **통합 테스트**: 실제 사용 시나리오 테스트
- **메모리 검사**: C/C++ 메모리 누수 검사
- **성능 테스트**: 패턴별 성능 특성 분석

```bash
# 전체 테스트 실행
make test

# 개별 언어 테스트
make test-c
make test-cpp
cd rust && cargo test
```

## 📈 성능 비교

각 언어별 패턴 구현의 성능 특성:

| 측면 | C | C++ | Rust |
|------|---|-----|------|
| **실행 속도** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **메모리 효율성** | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **안전성** | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **개발 생산성** | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |

## 🎯 학습 가이드

### 초보자를 위한 학습 순서

1. **생성 패턴부터 시작**
   - Singleton → Factory → Builder

2. **구조 패턴 학습**
   - Adapter → Decorator → Facade

3. **행동 패턴 마스터**
   - Observer → Strategy → Command

### 언어별 학습 순서

- **C 먼저**: 기본 개념 이해
- **C++ 다음**: 객체지향 및 현대적 기능
- **Rust 마지막**: 안전성과 소유권 시스템

## 🔍 상세 문서

각 패턴에 대한 상세한 설명은 다음 디렉토리에서 확인할 수 있습니다:

- [`docs/patterns/`](docs/patterns/) - 패턴별 상세 설명
- [`docs/languages/`](docs/languages/) - 언어별 구현 특징
- [`docs/examples/`](docs/examples/) - 실용적인 예제들

## 🤝 기여하기

이 프로젝트에 기여하고 싶으시다면:

1. Fork 후 브랜치 생성
2. 새로운 패턴 구현 또는 기존 개선
3. 테스트 추가
4. Pull Request 제출

## 📄 라이선스

MIT License - 자유롭게 사용, 수정, 배포 가능합니다.

## 🙏 감사의 말

이 프로젝트는 Gang of Four의 "Design Patterns" 책을 기반으로 하며, 각 언어 커뮤니티의 모범 사례를 따랐습니다.

---

**Happy Coding! 🚀**

*이 프로젝트를 통해 디자인 패턴을 마스터하고 세 언어의 차이점을 이해하시길 바랍니다.*