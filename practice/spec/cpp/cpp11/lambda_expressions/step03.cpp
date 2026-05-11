// =============================================================
// Step 03 — 참조 캡처 [&], [&x]
// =============================================================
// 목표:
//   1) 카운터 변수 cnt 를 "참조로" 캡처해서 호출될 때마다 cnt 를
//      1 증가시키는 람다 `tick` 을 만든다. (반환값 없음, void)
//   2) 합계 변수 total 을 참조 캡처하여, 인자 v 를 받아 total += v 를
//      수행하는 람다 `accumulate` 를 만든다. (반환값 없음)
//   3) [&] 디폴트 참조 캡처를 사용해 vector<int> nums 의 모든 원소를
//      두 배로 만드는 람다 `doubleAll` 을 만든다. (인자 없음)
//
// 핵심 학습 포인트:
//   - 참조 캡처는 원본을 가리킨다 → 람다가 호출되면 외부 상태가 바뀐다.
//   - 람다보다 캡처된 변수의 수명이 짧으면 dangling reference (위험!).
//
// Expected Output (정확히):
// cnt after 3 ticks = 3
// total after accumulating 1,2,3,4 = 10
// nums after doubleAll: 2 4 6 8 10
// =============================================================

#include <iostream>
#include <vector>

int main() {
    int cnt = 0;
    // TODO: [&cnt] 로 캡처한 tick 정의

    tick(); tick(); tick();
    std::cout << "cnt after 3 ticks = " << cnt << '\n';

    int total = 0;
    // TODO: [&total] 로 캡처한 accumulate(int v) 정의

    accumulate(1);
    accumulate(2);
    accumulate(3);
    accumulate(4);
    std::cout << "total after accumulating 1,2,3,4 = " << total << '\n';

    std::vector<int> nums = {1, 2, 3, 4, 5};
    // TODO: [&] 로 nums 를 참조 캡처한 doubleAll 정의

    doubleAll();
    std::cout << "nums after doubleAll:";
    for (int v : nums) std::cout << ' ' << v;
    std::cout << '\n';
    return 0;
}
