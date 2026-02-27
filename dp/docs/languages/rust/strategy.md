# Strategy Pattern - Rust Implementation

## 개요

Strategy 패턴은 알고리즘 패밀리를 정의하고 각각을 캡슐화하여 상호 교체 가능하게 만듭니다. 이 패턴을 사용하면 알고리즘을 사용하는 클라이언트와 독립적으로 알고리즘을 변경할 수 있습니다.

## 구조

```rust
pub trait SortStrategy {
    fn sort(&self, data: &mut [i32]);
    fn name(&self) -> &str;
}

pub struct SortContext {
    strategy: Box<dyn SortStrategy>,
}

impl SortContext {
    pub fn new(strategy: Box<dyn SortStrategy>) -> Self {
        Self { strategy }
    }

    pub fn set_strategy(&mut self, strategy: Box<dyn SortStrategy>) {
        self.strategy = strategy;
    }

    pub fn sort(&self, data: &mut [i32]) {
        self.strategy.sort(data);
    }
}
```

## Rust 구현

### 1. 전략 인터페이스
```rust
pub trait SortStrategy {
    fn sort(&self, data: &mut [i32]);
    fn name(&self) -> &str;
}
```

### 2. 구체적인 전략들
```rust
pub struct BubbleSort;

impl SortStrategy for BubbleSort {
    fn sort(&self, data: &mut [i32]) {
        let len = data.len();
        for i in 0..len {
            for j in 0..len - 1 - i {
                if data[j] > data[j + 1] {
                    data.swap(j, j + 1);
                }
            }
        }
    }

    fn name(&self) -> &str {
        "Bubble Sort"
    }
}

pub struct QuickSort;

impl SortStrategy for QuickSort {
    fn sort(&self, data: &mut [i32]) {
        if data.len() <= 1 {
            return;
        }
        self.quicksort_recursive(data, 0, data.len() - 1);
    }

    fn name(&self) -> &str {
        "Quick Sort"
    }
}
```

## 사용 예제

```rust
let mut data = vec![64, 34, 25, 12, 22, 11, 90];

// 버블 정렬 사용
let mut sorter = SortContext::new(Box::new(BubbleSort));
sorter.sort(&mut data);

// 퀵 정렬로 전략 변경
sorter.set_strategy(Box::new(QuickSort));
sorter.sort(&mut data);
```

## 적용 상황

### 1. 정렬 알고리즘 선택
### 2. 결제 처리 방식
### 3. 압축 알고리즘
### 4. 게임 AI 전략
### 5. 데이터 변환 방식