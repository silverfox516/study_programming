// =============================================================
// Step 04 — 혼합 캡처 / 디폴트 캡처
// =============================================================
// 목표:
//   1) 디폴트는 값 캡처(=) 로 두고, 합계 sum 만 참조 캡처(&sum) 하는
//      람다 `addToSum` 을 만든다.
//      - 시그니처: void(int v)  → sum += v * factor;
//      - factor 는 외부에 정의된 int (값 캡처되어야 함)
//   2) 디폴트는 참조 캡처(&) 로 두고, 상수 한도 limit 만 값 캡처(=limit)
//      하는 람다 `clampPush` 를 만든다.
//      - 시그니처: void(int v) → v 가 limit 이하면 vec 에 push_back, 아니면 무시
//      - vec 는 외부 vector<int>, limit 은 외부 int
//
// 핵심 학습 포인트:
//   - 디폴트 캡처를 정해두고 일부만 다르게 캡처하는 패턴: [=, &x] / [&, x]
//   - 같은 변수를 두 가지 방식으로 동시에 캡처할 수 없다.
//
// Expected Output (정확히):
// sum after addToSum(1..4) with factor=10 = 100
// vec after clampPush of {3,7,12,5,20} limit=10: 3 7 5
// =============================================================

#include <iostream>
#include <vector>

int main() {
    int sum = 0;
    int factor = 10;
    // TODO: [=, &sum] 형태로 addToSum 정의

    addToSum(1);
    addToSum(2);
    addToSum(3);
    addToSum(4);
    std::cout << "sum after addToSum(1..4) with factor=10 = " << sum << '\n';

    std::vector<int> vec;
    int limit = 10;
    // TODO: [&, limit] 형태로 clampPush 정의

    for (int v : {3, 7, 12, 5, 20}) clampPush(v);
    std::cout << "vec after clampPush of {3,7,12,5,20} limit=10:";
    for (int v : vec) std::cout << ' ' << v;
    std::cout << '\n';
    return 0;
}
