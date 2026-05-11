// =============================================================
// Step 09 — 초기화 캡처 (C++14, [name = expr])
// =============================================================
// 목표:
//   1) 큰 객체(std::vector<int>) 를 std::move 로 람다에 옮겨 담아,
//      외부 벡터를 비우고 람다 내부에서만 사용하도록 한다.
//      - 외부 변수 src 의 size 가 람다 생성 후 0 이 되어야 함.
//      - 람다 호출 시 내부에 보관된 벡터 원소의 합을 반환.
//
//      auto owner = [data = std::move(src)]() { ... };
//
//   2) 초기화 캡처로 "0 부터 시작하는 카운터" 를 만든다.
//      - 외부 변수에 의존하지 않고 [n = 0]() mutable -> int { return ++n; }
//      - 호출 4회 결과 출력.
//
//   3) 초기화 캡처에 표현식 사용:
//      - [greeting = std::string("Hello, ") + name]() { std::cout << greeting; ... }
//      - 외부 name 이 변경돼도 람다 내부 greeting 은 그대로여야 한다.
//
// 컴파일 옵션: -std=c++14 이상 (이 파일은 -std=c++17 권장)
//
// Expected Output (정확히):
// src size after move = 0
// owner() sum = 15
// counter -> 1 2 3 4
// greeting fixed at construction: Hello, Alice
// (after name change to Bob) greeting still: Hello, Alice
// =============================================================

#include <iostream>
#include <vector>
#include <string>
#include <numeric>

int main() {
    std::vector<int> src = {1, 2, 3, 4, 5};
    // TODO: [data = std::move(src)] 형태로 owner 람다 정의 (벡터 합 반환)

    std::cout << "src size after move = " << src.size() << '\n';
    std::cout << "owner() sum = " << owner() << '\n';

    // TODO: [n = 0]() mutable 형태로 counter 람다 정의

    std::cout << "counter -> "
              << counter() << ' '
              << counter() << ' '
              << counter() << ' '
              << counter() << '\n';

    std::string name = "Alice";
    // TODO: [greeting = std::string("Hello, ") + name] 형태로 hello 람다 정의
    //       (호출 시 "greeting fixed at construction: <greeting>" 출력)

    hello();
    name = "Bob";
    std::cout << "(after name change to Bob) greeting still: ";
    hello_print(); // 람다가 들고 있는 greeting 만 출력 (개행 X)
    std::cout << '\n';
    return 0;
}

// 힌트:
//   step09 는 두 동작(헤더 출력 / 본문만 출력) 을 따로 쓰고 싶다면
//   람다를 두 개 만들어도 되고, 하나의 람다에 bool 인자 모드를 둬도 됨.
//   위 출력 형식만 맞추면 됩니다.
