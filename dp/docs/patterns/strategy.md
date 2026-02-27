# Strategy Pattern

## 개요
Strategy 패턴은 알고리즘군을 정의하고 각각을 캡슐화하여 상호 교환 가능하게 만드는 행위 패턴입니다. 클라이언트와 독립적으로 알고리즘을 변경할 수 있게 해줍니다.

## 구조
- **Strategy**: 모든 구체적인 전략들이 구현해야 하는 인터페이스
- **Concrete Strategy**: Strategy 인터페이스를 구현한 구체적인 알고리즘
- **Context**: Strategy 객체를 사용하는 클래스

## C 언어 구현

### 전략 인터페이스
```c
typedef struct PaymentStrategy {
    void (*pay)(struct PaymentStrategy* self, double amount);
    void (*destroy)(struct PaymentStrategy* self);
} PaymentStrategy;
```

### 구체적 전략들
```c
// 신용카드 결제 전략
typedef struct CreditCardPayment {
    PaymentStrategy base;
    char* card_number;
    char* name;
    char* cvv;
    char* expiry_date;
} CreditCardPayment;

void credit_card_pay(PaymentStrategy* strategy, double amount) {
    CreditCardPayment* cc = (CreditCardPayment*)strategy;
    printf("Paid $%.2f using Credit Card\n", amount);
    printf("Card: ****-****-****-%s\n", &cc->card_number[strlen(cc->card_number) - 4]);
}

PaymentStrategy* create_credit_card_payment(const char* card_number,
                                          const char* name,
                                          const char* cvv,
                                          const char* expiry_date) {
    CreditCardPayment* cc = malloc(sizeof(CreditCardPayment));
    cc->base.pay = credit_card_pay;
    cc->base.destroy = credit_card_destroy;

    cc->card_number = malloc(strlen(card_number) + 1);
    strcpy(cc->card_number, card_number);

    cc->name = malloc(strlen(name) + 1);
    strcpy(cc->name, name);

    return (PaymentStrategy*)cc;
}

// PayPal 결제 전략
typedef struct PayPalPayment {
    PaymentStrategy base;
    char* email;
    char* password;
} PayPalPayment;

void paypal_pay(PaymentStrategy* strategy, double amount) {
    PayPalPayment* pp = (PayPalPayment*)strategy;
    printf("Paid $%.2f using PayPal\n", amount);
    printf("Account: %s\n", pp->email);
}

// 비트코인 결제 전략
typedef struct BitcoinPayment {
    PaymentStrategy base;
    char* wallet_address;
} BitcoinPayment;

void bitcoin_pay(PaymentStrategy* strategy, double amount) {
    BitcoinPayment* btc = (BitcoinPayment*)strategy;
    printf("Paid $%.2f using Bitcoin\n", amount);
    printf("Wallet: %s\n", btc->wallet_address);
}
```

### Context 클래스
```c
typedef struct ShoppingCart {
    PaymentStrategy* payment_strategy;
    double total_amount;
} ShoppingCart;

ShoppingCart* create_shopping_cart() {
    ShoppingCart* cart = malloc(sizeof(ShoppingCart));
    cart->payment_strategy = NULL;
    cart->total_amount = 0.0;
    return cart;
}

void shopping_cart_set_payment_strategy(ShoppingCart* cart, PaymentStrategy* strategy) {
    cart->payment_strategy = strategy;
}

void shopping_cart_add_item(ShoppingCart* cart, double price) {
    cart->total_amount += price;
    printf("Added item: $%.2f (Total: $%.2f)\n", price, cart->total_amount);
}

void shopping_cart_checkout(ShoppingCart* cart) {
    if (cart->payment_strategy && cart->total_amount > 0) {
        printf("\nChecking out...\n");
        cart->payment_strategy->pay(cart->payment_strategy, cart->total_amount);
        cart->total_amount = 0.0;
        printf("Checkout completed!\n\n");
    } else {
        printf("No payment method selected or cart is empty\n");
    }
}
```

## 사용 예제
```c
int main() {
    // 쇼핑카트 생성
    ShoppingCart* cart = create_shopping_cart();

    // 아이템 추가
    shopping_cart_add_item(cart, 29.99);
    shopping_cart_add_item(cart, 15.50);
    shopping_cart_add_item(cart, 8.75);

    // 신용카드 결제 전략 사용
    printf("\n=== Credit Card Payment ===\n");
    PaymentStrategy* credit_card = create_credit_card_payment(
        "1234567890123456", "John Doe", "123", "12/25"
    );
    shopping_cart_set_payment_strategy(cart, credit_card);
    shopping_cart_checkout(cart);

    // 새 아이템 추가
    shopping_cart_add_item(cart, 45.00);
    shopping_cart_add_item(cart, 12.25);

    // PayPal 결제 전략으로 변경
    printf("=== PayPal Payment ===\n");
    PaymentStrategy* paypal = create_paypal_payment("john@example.com", "password123");
    shopping_cart_set_payment_strategy(cart, paypal);
    shopping_cart_checkout(cart);

    // 또 다른 아이템 추가
    shopping_cart_add_item(cart, 99.99);

    // 비트코인 결제 전략으로 변경
    printf("=== Bitcoin Payment ===\n");
    PaymentStrategy* bitcoin = create_bitcoin_payment("1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa");
    shopping_cart_set_payment_strategy(cart, bitcoin);
    shopping_cart_checkout(cart);

    return 0;
}
```

출력 예제:
```
Added item: $29.99 (Total: $29.99)
Added item: $15.50 (Total: $45.49)
Added item: $8.75 (Total: $54.24)

=== Credit Card Payment ===
Checking out...
Paid $54.24 using Credit Card
Card: ****-****-****-3456
Cardholder: John Doe
Checkout completed!

=== PayPal Payment ===
Checking out...
Paid $57.25 using PayPal
Account: john@example.com
Checkout completed!

=== Bitcoin Payment ===
Checking out...
Paid $99.99 using Bitcoin
Wallet: 1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa
Checkout completed!
```

## 장점과 단점

### 장점
- **알고리즘 교체 용이**: 런타임에 전략을 변경할 수 있음
- **코드 재사용**: 각 전략을 독립적으로 재사용 가능
- **확장성**: 새로운 전략을 쉽게 추가
- **테스트 용이**: 각 전략을 독립적으로 테스트 가능
- **조건문 제거**: 복잡한 if-else 구문을 제거

### 단점
- **객체 수 증가**: 각 전략마다 클래스가 필요
- **클라이언트 지식**: 클라이언트가 전략들을 알고 있어야 함
- **통신 오버헤드**: Context와 Strategy 간의 통신 비용

## 적용 상황
- **결제 시스템**: 다양한 결제 방법 지원
- **정렬 알고리즘**: 데이터 크기에 따른 최적 정렬 선택
- **압축 알고리즘**: 파일 타입별 최적 압축 방식
- **할인 정책**: 고객 등급별 할인 전략
- **게임 AI**: 상황별 다른 AI 전략
- **데이터 포맷**: 다양한 데이터 출력 형식
- **인증 방식**: 다중 인증 메커니즘 지원

Strategy 패턴은 C 언어에서 함수 포인터를 활용하여 알고리즘의 유연한 교체와 확장을 가능하게 하는 매우 유용한 패턴입니다.