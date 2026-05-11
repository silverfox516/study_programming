// =============================================================
// Step 05 — mutable 람다
// =============================================================
// 목표:
//   1) 값으로 캡처된 카운터를 람다 내부에서 증가시키는 카운터 람다
//      `counter` 를 만든다.
//      - 시그니처: int()  → 호출할 때마다 1, 2, 3, ... 을 반환
//      - 외부 변수 start 를 값 캡처하지만, 내부적으로 mutable 키워드로
//        복사본을 수정하여 카운트한다.
//      - 외부 start 는 호출 후에도 변하지 않아야 한다.
//   2) `priceCalc` 람다: base(값 캡처) 를 들고 있고, 호출될 때마다
//      base 에 매번 1.1 을 곱한 새 base 로 갱신한 뒤, 현재 base 를
//      int 로 잘라(반환 시 static_cast<int>) 반환한다.
//      - 시작 base = 100, 호출 3회 결과는: 110, 121, 133
//      - 외부 base 는 호출 후에도 100 그대로여야 한다.
//
// 핵심 학습 포인트:
//   - 값 캡처는 기본적으로 const 멤버 → 변경하려면 mutable 필요.
//   - mutable 은 람다 내부 복사본만 변경하며, 외부에는 영향 없음.
//
// Expected Output (정확히):
// counter -> 1 2 3 4
// outside start = 0
// priceCalc -> 110 121 133
// outside base = 100
// =============================================================

#include <iostream>

int main() {
    int start = 0;
    // TODO: mutable 을 사용한 counter 정의 (값 캡처, 호출마다 ++)

    std::cout << "counter -> "
              << counter() << ' '
              << counter() << ' '
              << counter() << ' '
              << counter() << '\n';
    std::cout << "outside start = " << start << '\n';

    int base = 100;
    // TODO: mutable priceCalc 정의

    std::cout << "priceCalc -> "
              << priceCalc() << ' '
              << priceCalc() << ' '
              << priceCalc() << '\n';
    std::cout << "outside base = " << base << '\n';
    return 0;
}
