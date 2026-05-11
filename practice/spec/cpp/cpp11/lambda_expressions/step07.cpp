// =============================================================
// Step 07 — std::function 으로 람다 저장/전달
// =============================================================
// 목표:
//   1) std::function<int(int,int)> 타입의 변수에 "덧셈", "뺄셈", "곱셈"
//      람다를 차례로 담아 결과를 출력한다.
//   2) 함수 `apply` 를 작성한다.
//      - 시그니처:
//          int apply(int x, int y, const std::function<int(int,int)>& op);
//      - 람다를 인자로 넘겨 op(x,y) 결과를 반환.
//   3) `makeMultiplier(int k)` 함수를 작성한다.
//      - 반환: std::function<int(int)>
//      - k 를 값 캡처한 람다를 반환 (클로저 팩토리 패턴)
//      - 반환된 람다는 인자 v 에 대해 v * k 를 반환.
//
// 핵심 학습 포인트:
//   - 람다 타입은 "고유한 익명 타입" → std::function 으로 통일된 타입에 담을 수 있다.
//   - 단, std::function 은 type-erasure 비용이 있음(작은 추상화 비용).
//   - 람다를 반환할 때는 "참조 캡처된 지역 변수가 사라지지 않도록" 주의.
//
// Expected Output (정확히):
// add(3,4) = 7
// sub(10,4) = 6
// mul(6,7) = 42
// apply(2,5,add) = 7
// times3(11) = 33
// times7(11) = 77
// =============================================================

#include <iostream>
#include <functional>

// TODO: apply 선언/정의

// TODO: makeMultiplier 선언/정의

int main() {
    std::function<int(int,int)> op;

    // TODO: op 에 add 람다 담기
    std::cout << "add(3,4) = " << op(3, 4) << '\n';

    // TODO: op 에 sub 람다 담기
    std::cout << "sub(10,4) = " << op(10, 4) << '\n';

    // TODO: op 에 mul 람다 담기
    std::cout << "mul(6,7) = " << op(6, 7) << '\n';

    // apply 사용: 덧셈 람다를 인자로 직접 전달
    // TODO: apply 호출
    std::cout << "apply(2,5,add) = " << /* TODO: apply(...) */ 0 << '\n';

    auto times3 = makeMultiplier(3);
    auto times7 = makeMultiplier(7);
    std::cout << "times3(11) = " << times3(11) << '\n';
    std::cout << "times7(11) = " << times7(11) << '\n';
    return 0;
}
