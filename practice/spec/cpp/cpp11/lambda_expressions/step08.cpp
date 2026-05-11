// =============================================================
// Step 08 — 제네릭 람다 (C++14, auto 매개변수)
// =============================================================
// 목표:
//   1) 두 인자의 합을 반환하는 제네릭 람다 `gAdd` 를 만든다.
//      - 시그니처: auto(auto a, auto b)  →  return a + b;
//      - int + int, double + double, string + string 모두 동작해야 함.
//   2) 컨테이너를 받아 모든 원소를 출력 후 줄바꿈하는 제네릭 람다 `printAll`.
//      - 시그니처: void(const auto& container)
//      - 사이는 공백 한 칸으로 구분, 끝에 '\n'
//   3) `applyTwice` 제네릭 람다.
//      - 시그니처: auto(auto f, auto x)  →  return f(f(x));
//      - 즉 f 를 두 번 적용한 결과 반환.
//
// 컴파일 옵션: -std=c++14 이상 (이 파일은 -std=c++17 권장)
//
// Expected Output (정확히):
// gAdd(2,3) = 5
// gAdd(1.5,2.25) = 3.75
// gAdd("foo","bar") = foobar
// printAll vector<int>: 1 2 3 4
// printAll vector<string>: hello world
// applyTwice(+1, 10) = 12
// applyTwice(*2, 3) = 12
// =============================================================

#include <iostream>
#include <vector>
#include <string>

int main() {
    // TODO: gAdd 정의 (제네릭 람다)

    std::cout << "gAdd(2,3) = " << gAdd(2, 3) << '\n';
    std::cout << "gAdd(1.5,2.25) = " << gAdd(1.5, 2.25) << '\n';
    std::cout << "gAdd(\"foo\",\"bar\") = "
              << gAdd(std::string("foo"), std::string("bar")) << '\n';

    // TODO: printAll 정의 (제네릭 람다)

    std::vector<int> vi = {1, 2, 3, 4};
    std::vector<std::string> vs = {"hello", "world"};
    std::cout << "printAll vector<int>: ";  printAll(vi);
    std::cout << "printAll vector<string>: "; printAll(vs);

    // TODO: applyTwice 정의 (제네릭 람다)

    auto inc = [](int x) { return x + 1; };
    auto dbl = [](int x) { return x * 2; };
    std::cout << "applyTwice(+1, 10) = " << applyTwice(inc, 10) << '\n';
    std::cout << "applyTwice(*2, 3) = "  << applyTwice(dbl, 3)  << '\n';
    return 0;
}
