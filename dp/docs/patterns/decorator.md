# Decorator Pattern

## 개요
Decorator 패턴은 객체에 새로운 기능을 동적으로 추가할 수 있는 구조 패턴입니다. 상속을 사용하지 않고도 객체의 기능을 확장할 수 있으며, 여러 개의 장식자를 조합하여 다양한 기능 조합을 만들 수 있습니다. 기존 코드를 변경하지 않고도 기능을 추가할 수 있는 유연한 방법을 제공합니다.

## 구조
- **Component (Coffee)**: 기본 기능을 정의하는 인터페이스
- **Concrete Component (SimpleCoffee)**: 기본 기능의 구현체
- **Decorator**: Component를 참조하고 Component와 같은 인터페이스를 구현
- **Concrete Decorator (MilkDecorator, SugarDecorator)**: 추가 기능을 구현하는 장식자

## C 언어 구현

### 컴포넌트 인터페이스 정의
```c
// 커피 컴포넌트 인터페이스
typedef struct Coffee {
    char* (*get_description)(struct Coffee* self);
    double (*cost)(struct Coffee* self);
    void (*destroy)(struct Coffee* self);
} Coffee;
```

### 기본 구현체(Concrete Component)
```c
// 기본 커피 구현
typedef struct SimpleCoffee {
    Coffee base;
} SimpleCoffee;

char* simple_coffee_description(Coffee* self) {
    static char desc[] = "Simple Coffee";
    return desc;
}

double simple_coffee_cost(Coffee* self) {
    return 2.0;
}

void simple_coffee_destroy(Coffee* self) {
    free(self);
}

Coffee* create_simple_coffee() {
    SimpleCoffee* coffee = malloc(sizeof(SimpleCoffee));
    coffee->base.get_description = simple_coffee_description;
    coffee->base.cost = simple_coffee_cost;
    coffee->base.destroy = simple_coffee_destroy;
    return (Coffee*)coffee;
}
```

### 장식자(Decorator) 구현

#### 우유 장식자
```c
typedef struct MilkDecorator {
    Coffee base;
    Coffee* coffee;                // 장식할 커피 객체
    char* description_buffer;      // 동적 문자열 버퍼
} MilkDecorator;

char* milk_description(Coffee* self) {
    MilkDecorator* milk = (MilkDecorator*)self;
    if (milk->description_buffer) {
        free(milk->description_buffer);
    }

    // 기존 설명에 ", Milk" 추가
    char* base_desc = milk->coffee->get_description(milk->coffee);
    milk->description_buffer = malloc(strlen(base_desc) + 20);
    sprintf(milk->description_buffer, "%s, Milk", base_desc);
    return milk->description_buffer;
}

double milk_cost(Coffee* self) {
    MilkDecorator* milk = (MilkDecorator*)self;
    return milk->coffee->cost(milk->coffee) + 0.5;  // 기존 가격 + 우유 가격
}

void milk_destroy(Coffee* self) {
    MilkDecorator* milk = (MilkDecorator*)self;
    if (milk->description_buffer) {
        free(milk->description_buffer);
    }
    milk->coffee->destroy(milk->coffee);  // 내부 커피 객체도 파괴
    free(milk);
}

Coffee* add_milk(Coffee* coffee) {
    MilkDecorator* milk = malloc(sizeof(MilkDecorator));
    milk->base.get_description = milk_description;
    milk->base.cost = milk_cost;
    milk->base.destroy = milk_destroy;
    milk->coffee = coffee;
    milk->description_buffer = NULL;
    return (Coffee*)milk;
}
```

#### 설탕 장식자
```c
typedef struct SugarDecorator {
    Coffee base;
    Coffee* coffee;
    char* description_buffer;
} SugarDecorator;

char* sugar_description(Coffee* self) {
    SugarDecorator* sugar = (SugarDecorator*)self;
    if (sugar->description_buffer) {
        free(sugar->description_buffer);
    }

    char* base_desc = sugar->coffee->get_description(sugar->coffee);
    sugar->description_buffer = malloc(strlen(base_desc) + 20);
    sprintf(sugar->description_buffer, "%s, Sugar", base_desc);
    return sugar->description_buffer;
}

double sugar_cost(Coffee* self) {
    SugarDecorator* sugar = (SugarDecorator*)self;
    return sugar->coffee->cost(sugar->coffee) + 0.2;
}

Coffee* add_sugar(Coffee* coffee) {
    SugarDecorator* sugar = malloc(sizeof(SugarDecorator));
    sugar->base.get_description = sugar_description;
    sugar->base.cost = sugar_cost;
    sugar->base.destroy = sugar_destroy;
    sugar->coffee = coffee;
    sugar->description_buffer = NULL;
    return (Coffee*)sugar;
}
```

#### 휘핑크림 장식자
```c
typedef struct WhipDecorator {
    Coffee base;
    Coffee* coffee;
    char* description_buffer;
} WhipDecorator;

char* whip_description(Coffee* self) {
    WhipDecorator* whip = (WhipDecorator*)self;
    if (whip->description_buffer) {
        free(whip->description_buffer);
    }

    char* base_desc = whip->coffee->get_description(whip->coffee);
    whip->description_buffer = malloc(strlen(base_desc) + 20);
    sprintf(whip->description_buffer, "%s, Whip", base_desc);
    return whip->description_buffer;
}

double whip_cost(Coffee* self) {
    WhipDecorator* whip = (WhipDecorator*)self;
    return whip->coffee->cost(whip->coffee) + 0.7;
}

Coffee* add_whip(Coffee* coffee) {
    WhipDecorator* whip = malloc(sizeof(WhipDecorator));
    whip->base.get_description = whip_description;
    whip->base.cost = whip_cost;
    whip->base.destroy = whip_destroy;
    whip->coffee = coffee;
    whip->description_buffer = NULL;
    return (Coffee*)whip;
}
```

## 사용 예제
```c
int main() {
    printf("--- Coffee Shop with Decorator Pattern ---\n");

    // 1. 기본 커피
    Coffee* coffee1 = create_simple_coffee();
    printf("%s: $%.2f\n", coffee1->get_description(coffee1), coffee1->cost(coffee1));
    coffee1->destroy(coffee1);

    // 2. 우유 추가
    Coffee* coffee2 = add_milk(create_simple_coffee());
    printf("%s: $%.2f\n", coffee2->get_description(coffee2), coffee2->cost(coffee2));
    coffee2->destroy(coffee2);

    // 3. 우유와 설탕 추가
    Coffee* coffee3 = add_sugar(add_milk(create_simple_coffee()));
    printf("%s: $%.2f\n", coffee3->get_description(coffee3), coffee3->cost(coffee3));
    coffee3->destroy(coffee3);

    // 4. 모든 토핑 추가
    Coffee* coffee4 = add_whip(add_sugar(add_milk(create_simple_coffee())));
    printf("%s: $%.2f\n", coffee4->get_description(coffee4), coffee4->cost(coffee4));
    coffee4->destroy(coffee4);

    return 0;
}
```

출력 예제:
```
--- Coffee Shop with Decorator Pattern ---
Simple Coffee: $2.00
Simple Coffee, Milk: $2.50
Simple Coffee, Milk, Sugar: $2.70
Simple Coffee, Milk, Sugar, Whip: $3.40
```

## 장점과 단점

### 장점
- **동적 기능 추가**: 런타임에 객체의 기능을 동적으로 확장
- **상속 대안**: 상속보다 유연한 기능 확장 방법 제공
- **조합 가능**: 여러 장식자를 조합하여 다양한 기능 조합 생성
- **단일 책임 원칙**: 각 장식자는 하나의 기능에만 집중
- **개방/폐쇄 원칙**: 기존 코드를 수정하지 않고 새로운 기능 추가

### 단점
- **복잡성 증가**: 많은 작은 클래스들로 인한 복잡성
- **디버깅 어려움**: 중첩된 장식자들로 인한 디버깅 복잡성
- **객체 정체성**: 장식된 객체의 원래 타입 파악이 어려움
- **메모리 오버헤드**: 여러 장식자 객체로 인한 메모리 사용 증가
- **순서 의존성**: 장식자를 적용하는 순서가 결과에 영향을 미칠 수 있음

## 적용 상황
- **스트림 처리**: 파일 스트림에 압축, 암호화, 버퍼링 기능 추가
- **UI 컴포넌트**: 스크롤, 테두리, 그림자 등의 시각적 효과 추가
- **네트워크**: HTTP 요청에 인증, 로깅, 캐싱 등의 기능 추가
- **텍스트 처리**: 텍스트에 볼드, 이탤릭, 밑줄 등의 서식 적용
- **게임 아이템**: 게임 아이템에 다양한 능력 부여
- **이미지 처리**: 이미지에 필터, 효과, 변환 등을 순차적으로 적용
- **로깅 시스템**: 로그 메시지에 타임스탬프, 레벨, 색상 등 추가
- **데이터 검증**: 입력 데이터에 여러 단계의 검증 규칙 적용

Decorator 패턴은 C 언어에서 함수 포인터와 구조체 합성을 활용하여 객체의 기능을 유연하게 확장할 수 있는 강력한 패턴입니다.