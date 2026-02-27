# Iterator Pattern - Rust Implementation

## 개요

Iterator 패턴은 컬렉션의 요소들에 순차적으로 접근하는 방법을 제공하면서도 내부 표현을 노출하지 않습니다. Rust는 내장 Iterator trait을 통해 이 패턴을 언어 차원에서 지원합니다.

## 구조

```rust
pub struct BookCollection {
    books: Vec<Book>,
}

impl BookCollection {
    pub fn iter(&self) -> impl Iterator<Item = &Book> {
        self.books.iter()
    }

    pub fn iter_mut(&mut self) -> impl Iterator<Item = &mut Book> {
        self.books.iter_mut()
    }

    pub fn into_iter(self) -> impl Iterator<Item = Book> {
        self.books.into_iter()
    }
}

// 커스텀 반복자
pub struct BookIterator<'a> {
    collection: &'a BookCollection,
    index: usize,
}

impl<'a> Iterator for BookIterator<'a> {
    type Item = &'a Book;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index < self.collection.books.len() {
            let book = &self.collection.books[self.index];
            self.index += 1;
            Some(book)
        } else {
            None
        }
    }
}
```

## 사용 예제

```rust
let collection = BookCollection::new();

// 내장 반복자 사용
for book in collection.iter() {
    println!("{}", book.title());
}

// 함수형 스타일
let titles: Vec<String> = collection
    .iter()
    .filter(|book| book.year() > 2020)
    .map(|book| book.title().to_string())
    .collect();

// 커스텀 반복자
let mut iterator = BookIterator::new(&collection);
while let Some(book) = iterator.next() {
    println!("Found: {}", book.title());
}
```

## 적용 상황

### 1. 컬렉션 순회
### 2. 데이터 스트림 처리
### 3. 파일 라인별 읽기
### 4. 트리/그래프 순회
### 5. 무한 시퀀스 생성