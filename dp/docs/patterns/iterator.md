# Iterator Pattern

## 개요
Iterator 패턴은 컬렉션의 구현 방법을 노출시키지 않으면서도 그 집합체의 모든 원소에 순차적으로 접근할 수 있는 방법을 제공하는 행위 패턴입니다.

## 구조
- **Iterator**: 순회 인터페이스를 정의
- **Concrete Iterator**: Iterator 인터페이스를 구현하는 구체적인 반복자
- **Aggregate**: Iterator를 생성하는 인터페이스
- **Concrete Aggregate**: 구체적인 컬렉션 구현

## C 언어 구현
```c
typedef struct Iterator {
    int (*has_next)(struct Iterator* self);
    void* (*next)(struct Iterator* self);
    void (*destroy)(struct Iterator* self);
} Iterator;

typedef struct ArrayList {
    void** items;
    int count;
    int capacity;
} ArrayList;

typedef struct ArrayListIterator {
    Iterator base;
    ArrayList* list;
    int current_position;
} ArrayListIterator;

int array_list_iterator_has_next(Iterator* self) {
    ArrayListIterator* iter = (ArrayListIterator*)self;
    return iter->current_position < iter->list->count;
}

void* array_list_iterator_next(Iterator* self) {
    ArrayListIterator* iter = (ArrayListIterator*)self;
    if (iter->current_position < iter->list->count) {
        return iter->list->items[iter->current_position++];
    }
    return NULL;
}

Iterator* create_array_list_iterator(ArrayList* list) {
    ArrayListIterator* iter = malloc(sizeof(ArrayListIterator));
    iter->base.has_next = array_list_iterator_has_next;
    iter->base.next = array_list_iterator_next;
    iter->list = list;
    iter->current_position = 0;
    return (Iterator*)iter;
}
```

## 사용 예제
```c
ArrayList* list = create_array_list();
array_list_add(list, "Apple");
array_list_add(list, "Banana");
array_list_add(list, "Cherry");

Iterator* iter = create_array_list_iterator(list);
while (iter->has_next(iter)) {
    char* item = (char*)iter->next(iter);
    printf("%s\n", item);
}
```

## 적용 상황
- **컬렉션 순회**: 다양한 자료구조의 통일된 순회 방법 제공
- **다중 순회**: 동시에 여러 반복자로 컬렉션 순회
- **필터링**: 조건에 맞는 요소만 순회하는 필터 반복자