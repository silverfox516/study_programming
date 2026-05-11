# C++ Lambda 단계별 학습 문제

> **원본 학습 자료:** [`spec/cpp/cpp11/lambda_expressions.cpp`](../../../../../spec/cpp/cpp11/lambda_expressions.cpp)
>
> 위 파일에서 람다 문법을 먼저 훑은 뒤 아래 step 을 순서대로 풀어보세요.

각 파일(`step01.cpp` ~ `step10.cpp`)에 문제와 예상 출력이 주석으로 적혀 있습니다.

## 풀이 방법
1. 각 파일을 열어 상단 주석의 문제와 요구사항을 읽으세요.
2. `// TODO:` 영역만 작성합니다. (그 외 영역은 출력 형식이 깨지지 않도록 가급적 그대로 두세요.)
3. 컴파일 & 실행 후 **예상 출력(Expected Output)** 과 동일하게 나오면 통과입니다.

## 컴파일 예시

### 단일 파일 (PowerShell)
```powershell
g++ -std=c++17 -O0 -Wall step01.cpp -o step01.exe
.\step01.exe
```

### 단일 파일 (bash / zsh)
```bash
g++ -std=c++17 -O0 -Wall step01.cpp -o step01
./step01
```

### CMake 로 전체 step 한 번에 빌드
```powershell
# 리포지토리 루트 기준
cd practice/spec/cpp
cmake -B build
cmake --build build
# 산출물: build/bin/cpp11/lambda_expressions/stepNN(.exe)
```

> 각 step 의 `// TODO:` 영역이 비어 있으면 빌드가 실패합니다.
> 풀이 중인 step 만 빌드하려면:
> ```powershell
> cmake --build build --target practice_cpp11_lambda_expressions_step01
> ```

## 단계 개요
| Step | 주제 |
|------|------|
| 01 | 가장 단순한 람다 (캡처 없음) |
| 02 | 값 캡처 `[=]`, `[x]` |
| 03 | 참조 캡처 `[&]`, `[&x]` |
| 04 | 혼합 캡처 / 디폴트 캡처 |
| 05 | `mutable` 람다 |
| 06 | STL 알고리즘과 람다 (`std::sort`, `std::for_each`, `std::count_if`) |
| 07 | `std::function` 으로 람다 저장/전달 |
| 08 | 제네릭 람다 (C++14, `auto` 매개변수) |
| 09 | 초기화 캡처 (C++14, `[v = std::move(x)]`) |
| 10 | 재귀 람다 + 람다 반환 (클로저 팩토리) |

평가가 필요하면 코드 작성 후 알려주세요. 캡처 의도, 수명, 부수효과까지 같이 봐드립니다.
