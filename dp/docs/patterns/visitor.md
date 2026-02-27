# Visitor Pattern

## 개요
Visitor 패턴은 알고리즘을 객체 구조에서 분리시키는 행위 패턴입니다. 구조를 수정하지 않고도 새로운 연산을 기존의 객체 구조에 추가할 수 있습니다.

## C 언어 구현
```c
typedef struct Element Element;
typedef struct Visitor Visitor;

struct Visitor {
    void (*visit_book)(Visitor* self, void* book);
    void (*visit_cd)(Visitor* self, void* cd);
};

struct Element {
    void (*accept)(Element* self, Visitor* visitor);
};

typedef struct Book {
    Element base;
    char* title;
    double price;
    int pages;
} Book;

typedef struct CD {
    Element base;
    char* title;
    double price;
    int tracks;
} CD;

void book_accept(Element* self, Visitor* visitor) {
    visitor->visit_book(visitor, self);
}

void cd_accept(Element* self, Visitor* visitor) {
    visitor->visit_cd(visitor, self);
}

// Price Calculator Visitor
typedef struct PriceCalculator {
    Visitor base;
    double total_price;
} PriceCalculator;

void price_visit_book(Visitor* self, void* book) {
    PriceCalculator* calc = (PriceCalculator*)self;
    Book* b = (Book*)book;
    calc->total_price += b->price;
    printf("Book: %s - $%.2f\n", b->title, b->price);
}

void price_visit_cd(Visitor* self, void* cd) {
    PriceCalculator* calc = (PriceCalculator*)self;
    CD* c = (CD*)cd;
    calc->total_price += c->price;
    printf("CD: %s - $%.2f\n", c->title, c->price);
}

PriceCalculator* create_price_calculator() {
    PriceCalculator* calc = malloc(sizeof(PriceCalculator));
    calc->base.visit_book = price_visit_book;
    calc->base.visit_cd = price_visit_cd;
    calc->total_price = 0.0;
    return calc;
}

// Inventory Visitor
typedef struct InventoryVisitor {
    Visitor base;
    int book_count;
    int cd_count;
} InventoryVisitor;

void inventory_visit_book(Visitor* self, void* book) {
    InventoryVisitor* inv = (InventoryVisitor*)self;
    Book* b = (Book*)book;
    inv->book_count++;
    printf("Book inventory: %s (%d pages)\n", b->title, b->pages);
}

void inventory_visit_cd(Visitor* self, void* cd) {
    InventoryVisitor* inv = (InventoryVisitor*)self;
    CD* c = (CD*)cd;
    inv->cd_count++;
    printf("CD inventory: %s (%d tracks)\n", c->title, c->tracks);
}
```

## 사용 예제
```c
// 요소들 생성
Book* book1 = create_book("Design Patterns", 45.00, 395);
Book* book2 = create_book("Clean Code", 38.50, 464);
CD* cd1 = create_cd("Greatest Hits", 15.99, 12);

Element* items[] = {
    (Element*)book1,
    (Element*)book2,
    (Element*)cd1
};

// 가격 계산 방문자
PriceCalculator* price_calc = create_price_calculator();
for (int i = 0; i < 3; i++) {
    items[i]->accept(items[i], (Visitor*)price_calc);
}
printf("Total Price: $%.2f\n\n", price_calc->total_price);

// 재고 방문자
InventoryVisitor* inventory = create_inventory_visitor();
for (int i = 0; i < 3; i++) {
    items[i]->accept(items[i], (Visitor*)inventory);
}
printf("Books: %d, CDs: %d\n", inventory->book_count, inventory->cd_count);
```

## 적용 상황
- **컴파일러**: AST 노드에 대한 다양한 연산 (파싱, 최적화, 코드 생성)
- **파일 시스템**: 파일과 디렉토리에 대한 다양한 연산
- **문서 처리**: 문서 요소들에 대한 렌더링, 변환, 검증
- **쇼핑몰**: 상품들에 대한 가격 계산, 재고 관리, 배송비 계산