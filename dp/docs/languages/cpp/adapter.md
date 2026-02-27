# Adapter Pattern - C++ Implementation

## 개요

Adapter 패턴은 호환되지 않는 인터페이스를 가진 클래스들이 함께 작동할 수 있도록 하는 구조적 디자인 패턴입니다. 기존 클래스를 수정하지 않고도 다른 인터페이스와 호환되도록 만들 수 있습니다.

C++에서는 상속, 컴포지션, 그리고 템플릿을 활용하여 다양한 형태의 어댑터를 구현할 수 있으며, 레거시 코드 통합과 서드파티 라이브러리 연동에 특히 유용합니다.

## 구조

### 핵심 구성 요소

1. **Target** (MediaPlayer, PaymentProcessor)
   - 클라이언트가 기대하는 인터페이스
   - 순수 가상 함수로 정의

2. **Adaptee** (Mp3Player, Mp4Player, LegacyPaymentSystem)
   - 기존에 존재하는 클래스
   - 다른 인터페이스를 가지고 있음

3. **Adapter** (AdvancedMediaAdapter, LegacyPaymentAdapter)
   - Target 인터페이스를 구현
   - Adaptee의 기능을 호출하여 변환

4. **Client** (AudioPlayer, PaymentService)
   - Target 인터페이스를 통해 작업 수행

## C++ 구현

### 1. Object Adapter (컴포지션 사용)

#### Target 인터페이스 정의
```cpp
class MediaPlayer {
public:
    virtual ~MediaPlayer() = default;
    virtual void play(const std::string& audioType, const std::string& fileName) = 0;
};
```

#### Adaptee 클래스들
```cpp
class Mp3Player {
public:
    void playMp3(const std::string& fileName) {
        std::cout << "Playing MP3 file: " << fileName << std::endl;
    }
};

class Mp4Player {
public:
    void playMp4(const std::string& fileName) {
        std::cout << "Playing MP4 file: " << fileName << std::endl;
    }
};
```

#### Object Adapter 구현
```cpp
class AdvancedMediaAdapter : public MediaPlayer {
private:
    std::unique_ptr<Mp4Player> mp4Player;
    std::unique_ptr<FlacPlayer> flacPlayer;

public:
    AdvancedMediaAdapter()
        : mp4Player(std::make_unique<Mp4Player>()),
          flacPlayer(std::make_unique<FlacPlayer>()) {}

    void play(const std::string& audioType, const std::string& fileName) override {
        if (audioType == "mp4") {
            mp4Player->playMp4(fileName);
        } else if (audioType == "flac") {
            flacPlayer->playFlac(fileName);
        }
    }
};
```

### 2. Class Adapter (다중 상속 사용)

```cpp
class Rectangle {
public:
    virtual ~Rectangle() = default;
    virtual void draw(int x, int y, int width, int height) = 0;
};

class LegacyRectangle {
public:
    void drawRectangle(int x1, int y1, int x2, int y2) {
        std::cout << "Legacy Rectangle from (" << x1 << "," << y1
                  << ") to (" << x2 << "," << y2 << ")" << std::endl;
    }
};

// Class Adapter - 다중 상속 활용
class RectangleAdapter : public Rectangle, private LegacyRectangle {
public:
    void draw(int x, int y, int width, int height) override {
        int x2 = x + width;
        int y2 = y + height;
        drawRectangle(x, y, x2, y2);  // private 상속으로 캡슐화
    }
};
```

### 3. Template Adapter (템플릿 활용)

```cpp
template<typename Container>
class Stack {
private:
    Container container;

public:
    void push(const typename Container::value_type& value) {
        container.push_back(value);
    }

    void pop() {
        if (!container.empty()) {
            container.pop_back();
        }
    }

    typename Container::value_type& top() {
        return container.back();
    }

    bool empty() const {
        return container.empty();
    }

    size_t size() const {
        return container.size();
    }
};
```

### 4. 복잡한 데이터 변환을 하는 Adapter

```cpp
class LegacyPaymentAdapter : public PaymentProcessor {
private:
    std::unique_ptr<LegacyPaymentSystem> legacySystem;
    std::string lastTransactionId;
    static int transactionCounter;

    double convertToUSD(const std::string& currency, double amount) {
        if (currency == "EUR") return amount * 1.1;
        if (currency == "GBP") return amount * 1.3;
        if (currency == "JPY") return amount * 0.009;
        return amount; // USD
    }

public:
    bool processPayment(const std::string& currency, double amount,
                       const std::string& method) override {
        double usdAmount = convertToUSD(currency, amount);

        std::cout << "Adapting: " << amount << " " << currency
                  << " -> $" << usdAmount << " USD" << std::endl;

        legacySystem->makePayment(usdAmount);

        std::ostringstream oss;
        oss << "TXN_" << ++transactionCounter << "_" << currency;
        lastTransactionId = oss.str();

        return true;
    }

    std::string getTransactionId() const override {
        return lastTransactionId;
    }
};
```

## 사용 예제

### 미디어 플레이어 예제
```cpp
auto audioPlayer = std::make_unique<AudioPlayer>();

audioPlayer->play("mp3", "song.mp3");     // 직접 지원
audioPlayer->play("mp4", "video.mp4");    // 어댑터를 통해 지원
audioPlayer->play("flac", "audio.flac");  // 어댑터를 통해 지원
```

### 결제 시스템 예제
```cpp
PaymentService paymentService;
paymentService.addProcessor(std::make_unique<LegacyPaymentAdapter>());

paymentService.processAllPayments("EUR", 85.75, "PayPal");
// Output:
// Adapting: 85.75 EUR -> $94.33 USD
// Legacy payment: $94.33 processed
// Transaction ID: TXN_1_EUR
```

### STL 컨테이너 어댑터
```cpp
Stack<std::vector<int>> stack;

for (int i = 1; i <= 5; ++i) {
    stack.push(i);
}

while (!stack.empty()) {
    std::cout << stack.top() << " ";
    stack.pop();
}
// Output: 5 4 3 2 1
```

## C++의 장점

### 1. 다중 상속 지원
- Class Adapter 패턴 구현 가능
- private 상속을 통한 구현 세부사항 은닉
- 인터페이스와 구현의 분리

### 2. 템플릿 활용
- 타입에 독립적인 어댑터 구현
- 컴파일 타임 최적화
- STL 컨테이너 어댑터와 같은 범용 구현

### 3. 스마트 포인터를 통한 메모리 관리
```cpp
std::unique_ptr<Mp4Player> mp4Player;  // 자동 메모리 관리
```

### 4. RAII와 예외 안전성
- 자원의 자동 정리
- 예외 발생 시에도 안전한 메모리 해제

### 5. static 멤버를 통한 상태 관리
```cpp
static int transactionCounter;  // 클래스 레벨 상태 관리
```

### 6. 함수 오버로딩과 기본 매개변수
```cpp
void draw(int x, int y, int width, int height);
void draw(const Rectangle& rect);  // 오버로딩
```

## 적용 상황

### 1. 레거시 시스템 통합
- 기존 시스템을 새로운 인터페이스에 맞춤
- 점진적 마이그레이션 지원

### 2. 서드파티 라이브러리 통합
- 외부 라이브러리의 인터페이스 표준화
- 의존성 격리

### 3. 데이터 형식 변환
- 서로 다른 데이터 형식 간 변환
- 프로토콜 어댑터

### 4. STL 컨테이너 어댑터
- `std::stack`, `std::queue`, `std::priority_queue`
- 기존 컨테이너에 새로운 인터페이스 제공

### 5. 크로스 플랫폼 호환성
- 플랫폼별 API를 통합된 인터페이스로 제공
- 플랫폼 차이 추상화

### 6. API 버전 호환성
- 구 버전 API를 새 버전에서 사용
- 하위 호환성 유지

## 장점과 단점

### 장점
1. **기존 코드 보존**: 레거시 코드 수정 없이 새 인터페이스 적용
2. **관심사 분리**: 인터페이스 변환 로직과 비즈니스 로직 분리
3. **재사용성**: 하나의 어댑터로 여러 클라이언트 지원
4. **유연성**: 런타임에 어댑터 교체 가능

### 단점
1. **복잡성 증가**: 추가적인 추상화 계층
2. **성능 오버헤드**: 간접 호출로 인한 미미한 성능 저하
3. **유지보수**: 어댑터 자체의 관리 필요

## 관련 패턴

- **Facade**: 복잡한 시스템을 단순화 (vs Adapter: 인터페이스 변환)
- **Decorator**: 객체에 새 기능 추가 (vs Adapter: 인터페이스 변환)
- **Proxy**: 객체에 대한 접근 제어 (vs Adapter: 인터페이스 호환성)

이 패턴은 호환되지 않는 인터페이스들 사이의 다리 역할을 하여, 기존 코드의 재사용성을 크게 높이고 시스템 통합을 용이하게 만듭니다.