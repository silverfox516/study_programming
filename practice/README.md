# Practice

`spec/`, `dp/`, `solid/`, `arch/`, `ddd/` 의 학습 자료를 보고 **직접 풀어보는** 단계별 실습 문제 모음입니다.

## 디렉터리 규칙

`practice/<도메인>/...` 는 루트의 `<도메인>/...` 과 **1:1 로 매칭**됩니다.

| 원본 학습 자료 | 대응 실습 폴더 |
|----------------|----------------|
| `spec/cpp/cpp11/lambda_expressions.cpp` | `practice/spec/cpp/cpp11/lambda_expressions/` |
| `dp/cpp/<pattern>.cpp` | `practice/dp/cpp/<pattern>/` |
| `solid/cpp/<principle>.cpp` | `practice/solid/cpp/<principle>/` |
| `arch/cpp/<pattern>.cpp` | `practice/arch/cpp/<pattern>/` |
| `ddd/cpp/<topic>.cpp` | `practice/ddd/cpp/<topic>/` |

즉, **루트 도메인 = 정답/예제**, **practice 하위 동일 경로 = 단계별 빈 칸 채우기 문제** 입니다.

## 파일 규칙

- 한 주제 = 한 폴더, 그 안에 `step01.cpp`, `step02.cpp`, ... 로 단계 분할
- 각 파일 상단 주석에 **목표 / 제약 / Expected Output** 명시
- `// TODO:` 영역만 학습자가 작성
- 폴더에 `README.md` 를 두고 단계 개요 + 원본 학습 파일 링크 명시

## 빌드

`practice/spec/cpp/` 하위는 CMake 로 한 번에 빌드됩니다.

```powershell
cd practice/spec/cpp
cmake -B build
cmake --build build
```

실행 파일은 `build/bin/<standard>/<topic>/stepNN(.exe)` 에 생성됩니다.

개별 파일만 컴파일하려면:

```powershell
g++ -std=c++17 -O0 -Wall step01.cpp -o step01.exe
.\step01.exe
```

## 현재 채워진 항목

- `spec/cpp/cpp11/lambda_expressions/` — step01 ~ step10

나머지 도메인 폴더는 `.gitkeep` 로 자리만 잡혀 있으며 점진적으로 채워집니다.
