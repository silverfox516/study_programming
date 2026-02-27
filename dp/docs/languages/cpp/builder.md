# Builder Pattern - C++ Implementation

## 개요

Builder 패턴은 복잡한 객체의 생성 과정을 단계별로 나누어 구성하는 창조적 디자인 패턴입니다. 동일한 생성 절차를 통해 서로 다른 표현의 객체를 만들 수 있으며, 객체 생성의 복잡성을 캡슐화합니다.

C++에서는 메서드 체이닝(Fluent Interface), 스마트 포인터, 그리고 `std::optional`과 같은 모던 C++ 기능을 활용하여 안전하고 직관적인 Builder를 구현할 수 있습니다.

## 구조

### 핵심 구성 요소

1. **Product** (Computer)
   - 생성되는 복잡한 객체
   - 여러 부분으로 구성된 복합 객체

2. **Builder** (ComputerBuilder)
   - 제품 객체의 부분들을 생성하는 추상 인터페이스
   - 단계별 생성 메서드 제공

3. **Concrete Builder** (GamingComputerBuilder, OfficeComputerBuilder)
   - Builder 인터페이스를 구현
   - 특정 유형의 제품 생성

4. **Director** (ComputerDirector)
   - Builder를 사용하여 객체를 구성하는 순서 정의
   - 복잡한 생성 로직 캡슐화

## C++ 구현

### 1. Product 클래스 정의

```cpp
class Computer {
private:
    std::string cpu;
    std::string gpu;
    int ram_gb;
    int storage_gb;
    std::string storage_type;
    std::string motherboard;
    std::string power_supply;
    std::vector<std::string> peripherals;
    std::optional<std::string> operating_system;  // 선택적 컴포넌트
    bool wifi_enabled;
    bool bluetooth_enabled;

public:
    Computer() : ram_gb(0), storage_gb(0), wifi_enabled(false), bluetooth_enabled(false) {}

    // Setters (Builder에서 사용)
    void setCPU(const std::string& cpu) { this->cpu = cpu; }
    void setGPU(const std::string& gpu) { this->gpu = gpu; }
    void setRAM(int gb) { this->ram_gb = gb; }
    void setStorage(int gb, const std::string& type) {
        this->storage_gb = gb;
        this->storage_type = type;
    }
    void addPeripheral(const std::string& peripheral) {
        this->peripherals.push_back(peripheral);
    }
    void setOperatingSystem(const std::string& os) {
        this->operating_system = os;
    }

    std::string getSpecifications() const {
        std::ostringstream specs;
        specs << "Computer Specifications:\n";
        specs << "  CPU: " << cpu << "\n";
        specs << "  GPU: " << gpu << "\n";
        specs << "  RAM: " << ram_gb << " GB\n";
        specs << "  Storage: " << storage_gb << " GB " << storage_type << "\n";

        if (operating_system.has_value()) {
            specs << "  OS: " << operating_system.value() << "\n";
        }

        if (!peripherals.empty()) {
            specs << "  Peripherals: ";
            for (size_t i = 0; i < peripherals.size(); ++i) {
                specs << peripherals[i];
                if (i < peripherals.size() - 1) specs << ", ";
            }
            specs << "\n";
        }
        return specs.str();
    }
};
```

### 2. Abstract Builder 인터페이스

```cpp
class ComputerBuilder {
protected:
    std::unique_ptr<Computer> computer;

public:
    ComputerBuilder() { reset(); }
    virtual ~ComputerBuilder() = default;

    void reset() {
        computer = std::make_unique<Computer>();
    }

    // 순수 가상 함수들 - 체이닝을 위해 참조 반환
    virtual ComputerBuilder& setCPU(const std::string& cpu) = 0;
    virtual ComputerBuilder& setGPU(const std::string& gpu) = 0;
    virtual ComputerBuilder& setRAM(int gb) = 0;
    virtual ComputerBuilder& setStorage(int gb, const std::string& type) = 0;
    virtual ComputerBuilder& addPeripheral(const std::string& peripheral) = 0;
    virtual ComputerBuilder& setOperatingSystem(const std::string& os) = 0;
    virtual ComputerBuilder& enableWifi(bool enable = true) = 0;
    virtual ComputerBuilder& enableBluetooth(bool enable = true) = 0;

    std::unique_ptr<Computer> build() {
        return std::move(computer);  // 소유권 이전
    }
};
```

### 3. Concrete Builder - 게이밍 컴퓨터

```cpp
class GamingComputerBuilder : public ComputerBuilder {
public:
    ComputerBuilder& setCPU(const std::string& cpu) override {
        computer->setCPU(cpu);
        return *this;  // 메서드 체이닝을 위한 참조 반환
    }

    ComputerBuilder& setGPU(const std::string& gpu) override {
        computer->setGPU(gpu);
        return *this;
    }

    // ... 다른 메서드들

    // 게이밍 특화 빌드 메서드
    GamingComputerBuilder& buildHighEndGaming() {
        setCPU("Intel i9-13900K")
            .setGPU("NVIDIA RTX 4090")
            .setRAM(32)
            .setStorage(2000, "NVMe SSD")
            .setMotherboard("ASUS ROG Maximus Z790")
            .setPowerSupply("850W 80+ Gold")
            .addPeripheral("Gaming Keyboard")
            .addPeripheral("Gaming Mouse")
            .addPeripheral("144Hz Monitor")
            .setOperatingSystem("Windows 11")
            .enableWifi()
            .enableBluetooth();
        return *this;
    }
};
```

### 4. Director 클래스

```cpp
class ComputerDirector {
public:
    std::unique_ptr<Computer> buildBudgetGaming(GamingComputerBuilder& builder) {
        builder.reset();
        return builder.setCPU("AMD Ryzen 5 5600X")
                     .setGPU("NVIDIA RTX 3060")
                     .setRAM(16)
                     .setStorage(1000, "NVMe SSD")
                     .setMotherboard("MSI B550M Pro")
                     .setPowerSupply("650W 80+ Bronze")
                     .addPeripheral("Gaming Keyboard")
                     .addPeripheral("Gaming Mouse")
                     .setOperatingSystem("Windows 11")
                     .enableWifi()
                     .build();
    }

    std::unique_ptr<Computer> buildWorkstation(OfficeComputerBuilder& builder) {
        builder.reset();
        return builder.setCPU("Intel Xeon W-2295")
                     .setGPU("NVIDIA Quadro RTX 4000")
                     .setRAM(64)
                     .setStorage(2000, "NVMe SSD")
                     .setMotherboard("Workstation Motherboard")
                     .setPowerSupply("1000W 80+ Platinum")
                     .addPeripheral("Professional Keyboard")
                     .addPeripheral("Precision Mouse")
                     .addPeripheral("4K Monitor")
                     .setOperatingSystem("Windows 11 Pro")
                     .enableWifi()
                     .enableBluetooth()
                     .build();
    }
};
```

### 5. 모던 C++ 스타일 Builder

```cpp
class ModernComputerBuilder {
private:
    std::unique_ptr<Computer> computer;

public:
    ModernComputerBuilder() : computer(std::make_unique<Computer>()) {}

    // auto 반환 타입으로 간단히
    auto& cpu(const std::string& cpu) {
        computer->setCPU(cpu);
        return *this;
    }

    auto& gpu(const std::string& gpu) {
        computer->setGPU(gpu);
        return *this;
    }

    auto& ram(int gb) {
        computer->setRAM(gb);
        return *this;
    }

    auto& storage(int gb, const std::string& type = "SSD") {
        computer->setStorage(gb, type);
        return *this;
    }

    auto& peripheral(const std::string& peripheral) {
        computer->addPeripheral(peripheral);
        return *this;
    }

    auto& os(const std::string& operating_system) {
        computer->setOperatingSystem(operating_system);
        return *this;
    }

    auto& wifi(bool enable = true) {
        computer->enableWifi(enable);
        return *this;
    }

    auto& bluetooth(bool enable = true) {
        computer->enableBluetooth(enable);
        return *this;
    }

    std::unique_ptr<Computer> build() {
        return std::move(computer);
    }
};
```

## 사용 예제

### 1. 전통적인 Builder 사용
```cpp
GamingComputerBuilder gamingBuilder;
auto gamingPC = gamingBuilder.buildHighEndGaming().build();
std::cout << gamingPC->getSpecifications() << std::endl;
```

### 2. Director 사용
```cpp
ComputerDirector director;
GamingComputerBuilder budgetBuilder;
auto budgetGaming = director.buildBudgetGaming(budgetBuilder);
std::cout << budgetGaming->getSpecifications() << std::endl;
```

### 3. 모던 Builder 사용 (Fluent Interface)
```cpp
auto customPC = ModernComputerBuilder()
    .cpu("AMD Ryzen 9 7950X")
    .gpu("NVIDIA RTX 4080")
    .ram(32)
    .storage(1000, "NVMe SSD")
    .storage(4000, "HDD")  // 여러 스토리지 가능
    .motherboard("ASUS X670E")
    .powerSupply("750W 80+ Gold")
    .peripheral("Mechanical Keyboard")
    .peripheral("Wireless Mouse")
    .peripheral("Ultrawide Monitor")
    .peripheral("Webcam")
    .os("Linux Ubuntu 22.04")
    .wifi()
    .bluetooth()
    .build();

std::cout << customPC->getSpecifications() << std::endl;
```

### 실행 결과
```
Computer Specifications:
  CPU: Intel i9-13900K
  GPU: NVIDIA RTX 4090
  RAM: 32 GB
  Storage: 2000 GB NVMe SSD
  Motherboard: ASUS ROG Maximus Z790
  Power Supply: 850W 80+ Gold
  OS: Windows 11
  WiFi: Enabled
  Bluetooth: Enabled
  Peripherals: Gaming Keyboard, Gaming Mouse, 144Hz Monitor
```

## C++의 장점

### 1. 스마트 포인터를 통한 안전한 메모리 관리
```cpp
std::unique_ptr<Computer> computer;  // 자동 메모리 해제
return std::move(computer);          // 소유권 이전
```

### 2. std::optional로 선택적 컴포넌트 처리
```cpp
std::optional<std::string> operating_system;  // 선택적 요소
if (operating_system.has_value()) {
    specs << "  OS: " << operating_system.value() << "\n";
}
```

### 3. 메서드 체이닝 (Fluent Interface)
```cpp
return *this;  // 연속적인 메서드 호출 가능
```

### 4. Move 시맨틱으로 효율적 객체 전달
```cpp
std::unique_ptr<Computer> build() {
    return std::move(computer);  // 불필요한 복사 방지
}
```

### 5. auto 키워드로 간단한 반환 타입
```cpp
auto& cpu(const std::string& cpu) {  // 타입 추론
    computer->setCPU(cpu);
    return *this;
}
```

### 6. STL 컨테이너 활용
```cpp
std::vector<std::string> peripherals;  // 동적 배열
std::ostringstream specs;              // 문자열 스트림
```

### 7. RAII 패턴
```cpp
ComputerBuilder() { reset(); }  // 생성자에서 초기화
~ComputerBuilder() = default;   // 자동 리소스 정리
```

## 적용 상황

### 1. 복잡한 객체 생성
- 많은 매개변수를 가진 생성자 대안
- 단계별 객체 구성이 필요한 경우

### 2. 설정 가능한 객체
- 다양한 옵션 조합
- 선택적 컴포넌트 포함

### 3. 불변 객체 생성
- 생성 후 변경 불가능한 객체
- 단계별 초기화 필요

### 4. 복합 객체 생성
- 여러 부품으로 구성된 제품
- 각 부품의 독립적 설정

### 5. SQL 쿼리 빌더
```cpp
auto query = QueryBuilder()
    .select("name, age")
    .from("users")
    .where("age > 18")
    .orderBy("name")
    .build();
```

### 6. HTTP 요청 빌더
```cpp
auto request = RequestBuilder()
    .url("https://api.example.com")
    .method("POST")
    .header("Content-Type", "application/json")
    .body(jsonData)
    .timeout(5000)
    .build();
```

## 장점과 단점

### 장점
1. **복잡성 관리**: 복잡한 생성 과정을 단순화
2. **가독성**: 명확한 메서드 이름으로 직관적 사용
3. **유연성**: 다양한 객체 구성 가능
4. **재사용성**: 동일한 Builder로 다양한 객체 생성
5. **불변성**: 생성 후 객체 상태 보장

### 단점
1. **복잡도 증가**: 추가적인 클래스들 필요
2. **메모리 오버헤드**: Builder 객체 생성 비용
3. **타입 안전성**: 필수 속성 누락 가능성

## 관련 패턴

- **Abstract Factory**: 제품군 생성 (vs Builder: 단일 복잡 객체 생성)
- **Composite**: 복합 객체 구조 (Builder와 함께 사용 가능)
- **Strategy**: 생성 전략 변경 (Builder 내부에서 활용 가능)

Builder 패턴은 복잡한 객체의 생성 과정을 체계적으로 관리하고, 코드의 가독성과 유지보수성을 크게 향상시키는 매우 유용한 패턴입니다.