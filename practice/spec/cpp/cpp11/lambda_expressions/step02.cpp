// =============================================================
// Step 02 — 값 캡처 [=], [x]
// =============================================================
// 목표:
//   1) 지역 변수 base 를 "값으로" 캡처해서, 인자 n 을 받아 base + n 을
//      반환하는 람다 `addBase` 를 만든다.  ([base] 형태)
//   2) base 를 캡처한 뒤, base 를 본문 밖에서 수정해도 람다 결과가 바뀌지
//      않음을 출력으로 확인한다.
//   3) 디폴트 값 캡처 [=] 를 사용하여 두 변수 a, b 를 한꺼번에 캡처해서
//      a*x + b 를 반환하는 람다 `linear` 를 만든다.
//
// 핵심 학습 포인트:
//   - 값 캡처는 람다 "생성 시점" 의 복사본을 들고 다닌다.
//   - 따라서 캡처된 변수의 이후 변경은 람다 안에서 보이지 않는다.
//
// Expected Output (정확히):
// base before = 10
// addBase(5) = 15
// after base = 100, addBase(5) is still 15
// linear(3) with a=2, b=7 -> 13
// =============================================================

#include <iostream>

int main() {
    int base = 10;
    std::cout << "base before = " << base << '\n';

    // TODO: [base] 로 캡처한 addBase 정의

    std::cout << "addBase(5) = " << addBase(5) << '\n';

    base = 100; // 캡처 이후 변경
    std::cout << "after base = " << base
              << ", addBase(5) is still " << addBase(5) << '\n';

    int a = 2, b = 7;
    // TODO: [=] 로 a, b 를 캡처한 linear(int x) 정의 (반환: a*x + b)

    std::cout << "linear(3) with a=2, b=7 -> " << linear(3) << '\n';
    return 0;
}
