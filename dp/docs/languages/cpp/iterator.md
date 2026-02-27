# Iterator Pattern - C++ Implementation

## 개요

Iterator 패턴은 컬렉션의 내부 구조를 노출하지 않고 순차적으로 요소에 접근할 수 있는 방법을 제공하는 행위 패턴입니다. C++에서는 STL 반복자와 범위 기반 for 루프가 이 패턴을 기본적으로 지원하며, 사용자 정의 컨테이너에서도 쉽게 구현할 수 있습니다.

## 구조

- **Iterator**: 요소에 접근하고 순회하기 위한 인터페이스
- **ConcreteIterator**: Iterator 인터페이스의 구체적 구현
- **Aggregate**: Iterator 객체를 생성하는 인터페이스
- **ConcreteAggregate**: 구체적인 컬렉션 구현

## C++ 구현

### 1. 전통적인 Iterator 패턴

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <string>

template<typename T>
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual bool hasNext() const = 0;
    virtual T next() = 0;
    virtual T current() const = 0;
    virtual void reset() = 0;
};

template<typename T>
class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual std::unique_ptr<Iterator<T>> createIterator() = 0;
    virtual size_t size() const = 0;
};

// Concrete Iterator
template<typename T>
class ArrayIterator : public Iterator<T> {
private:
    const std::vector<T>& collection;
    size_t position;

public:
    explicit ArrayIterator(const std::vector<T>& collection)
        : collection(collection), position(0) {}

    bool hasNext() const override {
        return position < collection.size();
    }

    T next() override {
        if (!hasNext()) {
            throw std::out_of_range("No more elements");
        }
        return collection[position++];
    }

    T current() const override {
        if (position == 0 || position > collection.size()) {
            throw std::out_of_range("Invalid position");
        }
        return collection[position - 1];
    }

    void reset() override {
        position = 0;
    }
};
```

### 2. Book Collection 예제

```cpp
class Book {
private:
    std::string title;
    std::string author;
    int year;

public:
    Book(const std::string& title, const std::string& author, int year)
        : title(title), author(author), year(year) {}

    const std::string& getTitle() const { return title; }
    const std::string& getAuthor() const { return author; }
    int getYear() const { return year; }

    std::string toString() const {
        return title + " by " + author + " (" + std::to_string(year) + ")";
    }
};

class BookCollection : public Aggregate<Book> {
private:
    std::vector<Book> books;

public:
    void addBook(const Book& book) {
        books.push_back(book);
    }

    std::unique_ptr<Iterator<Book>> createIterator() override {
        return std::make_unique<ArrayIterator<Book>>(books);
    }

    size_t size() const override {
        return books.size();
    }

    const Book& getBook(size_t index) const {
        return books.at(index);
    }
};
```

### 3. STL 호환 반복자

```cpp
#include <iterator>

template<typename T>
class CustomContainer {
private:
    std::vector<T> data;

public:
    // STL 호환 반복자 정의
    class iterator {
    private:
        typename std::vector<T>::iterator iter;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit iterator(typename std::vector<T>::iterator it) : iter(it) {}

        reference operator*() { return *iter; }
        pointer operator->() { return &(*iter); }

        iterator& operator++() {
            ++iter;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++iter;
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return iter == other.iter;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    // const_iterator 정의
    class const_iterator {
    private:
        typename std::vector<T>::const_iterator iter;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        explicit const_iterator(typename std::vector<T>::const_iterator it) : iter(it) {}

        reference operator*() const { return *iter; }
        pointer operator->() const { return &(*iter); }

        const_iterator& operator++() {
            ++iter;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++iter;
            return tmp;
        }

        bool operator==(const const_iterator& other) const {
            return iter == other.iter;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }
    };

    // 컨테이너 인터페이스
    void push_back(const T& value) {
        data.push_back(value);
    }

    size_t size() const { return data.size(); }
    bool empty() const { return data.empty(); }

    // 반복자 반환 함수들
    iterator begin() { return iterator(data.begin()); }
    iterator end() { return iterator(data.end()); }

    const_iterator begin() const { return const_iterator(data.begin()); }
    const_iterator end() const { return const_iterator(data.end()); }

    const_iterator cbegin() const { return const_iterator(data.begin()); }
    const_iterator cend() const { return const_iterator(data.end()); }
};
```

## 사용 예제

```cpp
int main() {
    // 1. 전통적인 Iterator 패턴
    BookCollection library;
    library.addBook(Book("1984", "George Orwell", 1949));
    library.addBook(Book("To Kill a Mockingbird", "Harper Lee", 1960));
    library.addBook(Book("The Great Gatsby", "F. Scott Fitzgerald", 1925));

    auto iterator = library.createIterator();
    std::cout << "=== Traditional Iterator Pattern ===" << std::endl;
    while (iterator->hasNext()) {
        Book book = iterator->next();
        std::cout << book.toString() << std::endl;
    }

    // 2. STL 호환 컨테이너
    CustomContainer<int> numbers;
    numbers.push_back(1);
    numbers.push_back(2);
    numbers.push_back(3);
    numbers.push_back(4);
    numbers.push_back(5);

    std::cout << "\\n=== STL Compatible Iterator ===" << std::endl;
    // 범위 기반 for 루프 사용
    for (const auto& num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // STL 알고리즘 사용
    std::cout << "Using STL algorithms:" << std::endl;
    auto it = std::find(numbers.begin(), numbers.end(), 3);
    if (it != numbers.end()) {
        std::cout << "Found: " << *it << std::endl;
    }

    // 3. 다양한 순회 방식
    std::cout << "\\n=== Different Traversal Methods ===" << std::endl;

    // 명시적 반복자 사용
    for (auto it = numbers.begin(); it != numbers.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

## C++의 장점

1. **STL 통합**: 표준 라이브러리와 완벽 호환
2. **타입 안전성**: 템플릿 기반 타입 안전 보장
3. **성능**: 인라인 최적화로 오버헤드 최소화
4. **범위 기반 for**: 간편한 문법 지원

## 반복자 종류

1. **Forward Iterator**: 전진만 가능
2. **Bidirectional Iterator**: 양방향 이동 가능
3. **Random Access Iterator**: 임의 접근 가능
4. **Input Iterator**: 입력 전용
5. **Output Iterator**: 출력 전용

## 적용 상황

1. **컬렉션 순회**: 내부 구조를 숨기면서 요소 접근
2. **알고리즘 구현**: STL 호환 알고리즘 작성
3. **데이터 스트림**: 순차적 데이터 처리
4. **파일 처리**: 라인 단위 파일 읽기

Iterator 패턴은 C++에서 가장 자연스럽게 사용되는 패턴 중 하나로, STL의 핵심 개념입니다.