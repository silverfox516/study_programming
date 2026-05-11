// =============================================================
// Step 06 — STL 알고리즘과 람다
// =============================================================
// 목표:
//   주어진 vector<int> nums = {5, 2, 9, 1, 7, 3, 8, 4, 6} 에 대해
//   람다 + STL 알고리즘으로 다음을 모두 수행한다.
//
//   1) std::sort 와 람다로 "내림차순" 정렬한다. (greater<>(), 함수 객체 X)
//   2) std::count_if 와 람다로 "5 보다 큰 원소 개수" 를 센다.
//   3) std::for_each 와 람다(참조 캡처) 로 모든 원소의 합 sumAll 을 구한다.
//   4) std::find_if 와 람다로 "처음으로 짝수인 원소의 값" 을 찾는다.
//      (정렬 이후 기준이며, 못 찾을 일은 없다고 가정)
//
// Expected Output (정확히):
// sorted desc: 9 8 7 6 5 4 3 2 1
// count > 5: 4
// sumAll: 45
// first even (after sort desc): 8
// =============================================================

#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> nums = {5, 2, 9, 1, 7, 3, 8, 4, 6};

    // TODO 1) std::sort + 람다 (내림차순)

    std::cout << "sorted desc:";
    for (int v : nums) std::cout << ' ' << v;
    std::cout << '\n';

    // TODO 2) std::count_if + 람다 → countGt5 변수에 저장
    long countGt5 = 0; // 필요시 타입을 std::count_if 의 반환 타입에 맞춰 조정해도 됨

    std::cout << "count > 5: " << countGt5 << '\n';

    // TODO 3) std::for_each + 람다(참조 캡처) → sumAll
    int sumAll = 0;

    std::cout << "sumAll: " << sumAll << '\n';

    // TODO 4) std::find_if + 람다 → 짝수의 첫 값
    int firstEven = 0;

    std::cout << "first even (after sort desc): " << firstEven << '\n';
    return 0;
}
