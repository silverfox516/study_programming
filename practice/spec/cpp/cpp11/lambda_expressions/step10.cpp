// =============================================================
// Step 10 — 재귀 람다 + 람다 반환 (클로저 팩토리)
// =============================================================
// 목표:
//   1) 재귀 람다로 팩토리얼 계산.
//      - 방법 A) std::function<long long(int)> fact = [&fact](int n){...};
//                참조 캡처 트릭. (가장 흔한 방식)
//      - 방법 B) C++14 제네릭 람다로 self 를 인자로 넘기는 패턴:
//                auto fact = [](auto self, int n) -> long long { ... };
//      두 방법 중 하나로 구현해도 좋고, 둘 다 구현해도 좋다.
//      출력 결과만 일치하면 OK.
//
//   2) `makeAccumulator(int start)` 함수 작성.
//      - 반환: std::function<int(int)>
//      - 호출될 때마다 내부 상태에 값을 더해 누적합을 반환.
//      - 같은 팩토리에서 만든 두 어큐뮬레이터는 상태가 "독립" 이어야 한다.
//        (각자의 클로저가 자기 상태를 가짐)
//
//   3) `compose(f, g)` 람다 작성. (제네릭 람다 권장)
//      - 반환: 입력 x 에 대해 f(g(x)) 를 계산하는 람다.
//      - 예: inc = +1, dbl = *2 일 때 compose(inc, dbl)(5) = 11
//                                       compose(dbl, inc)(5) = 12
//
// 핵심 학습 포인트:
//   - 람다는 자기 자신을 직접 알 수 없으므로, 재귀를 위해
//     std::function 트릭이나 self 인자 전달이 필요.
//   - 클로저는 각각 별개의 상태를 갖는다 (객체와 비슷).
//   - 함수 합성은 람다의 간결함이 가장 빛나는 패턴 중 하나.
//
// Expected Output (정확히):
// fact(0) = 1
// fact(5) = 120
// fact(10) = 3628800
// accA: 5 15 30
// accB: 100 102 105
// compose(inc, dbl)(5) = 11
// compose(dbl, inc)(5) = 12
// =============================================================

#include <iostream>
#include <functional>

// TODO: makeAccumulator 정의

int main() {
    // TODO: 재귀 람다 fact 정의

    std::cout << "fact(0) = "  << fact(0)  << '\n';
    std::cout << "fact(5) = "  << fact(5)  << '\n';
    std::cout << "fact(10) = " << fact(10) << '\n';

    auto accA = makeAccumulator(0);
    auto accB = makeAccumulator(100);

    int a1 = accA(5);
    int a2 = accA(10);
    int a3 = accA(15);
    int b1 = accB(0);
    int b2 = accB(2);
    int b3 = accB(3);

    std::cout << "accA: " << a1 << ' ' << a2 << ' ' << a3 << '\n';
    std::cout << "accB: " << b1 << ' ' << b2 << ' ' << b3 << '\n';

    // TODO: compose 람다 정의 (제네릭 람다 권장)

    auto inc = [](int x) { return x + 1; };
    auto dbl = [](int x) { return x * 2; };
    std::cout << "compose(inc, dbl)(5) = " << compose(inc, dbl)(5) << '\n';
    std::cout << "compose(dbl, inc)(5) = " << compose(dbl, inc)(5) << '\n';
    return 0;
}
