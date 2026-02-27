# Decorator Pattern - Rust Implementation

## 개요

Decorator 패턴은 기존 객체의 구조를 변경하지 않고 동적으로 새로운 기능을 추가할 수 있게 해줍니다. 상속의 대안으로 기능을 확장하는 유연한 방법을 제공합니다.

이 패턴은 객체를 여러 개의 데코레이터로 감싸서 새로운 행위를 조합할 수 있게 해줍니다. Rust에서는 trait과 제네릭을 활용하여 타입 안전하고 효율적인 데코레이터를 구현할 수 있습니다.

## 구조

```rust
// 기본 컴포넌트 인터페이스
pub trait Coffee {
    fn cost(&self) -> f64;
    fn description(&self) -> String;
    fn ingredients(&self) -> Vec<String>;
}

// 기본 구현
pub struct SimpleCoffee {
    size: CoffeeSize,
}

// 데코레이터
pub struct MilkDecorator {
    coffee: Box<dyn Coffee>,
    milk_type: MilkType,
}

// 빌더 패턴과 결합
pub struct CoffeeBuilder {
    coffee: Box<dyn Coffee>,
}
```

## Rust 구현

Rust의 Decorator 구현은 다음과 같은 특징을 가집니다:

### 1. 기본 컴포넌트 정의
```rust
pub trait Coffee {
    fn cost(&self) -> f64;
    fn description(&self) -> String;
    fn ingredients(&self) -> Vec<String>;
}

pub struct SimpleCoffee {
    size: CoffeeSize,
}

impl Coffee for SimpleCoffee {
    fn cost(&self) -> f64 {
        match self.size {
            CoffeeSize::Small => 2.00,
            CoffeeSize::Medium => 2.50,
            CoffeeSize::Large => 3.00,
        }
    }

    fn description(&self) -> String {
        format!("{:?} Simple Coffee", self.size)
    }

    fn ingredients(&self) -> Vec<String> {
        vec!["Coffee beans".to_string(), "Water".to_string()]
    }
}
```

### 2. 구체적인 데코레이터 구현
```rust
pub struct MilkDecorator {
    coffee: Box<dyn Coffee>,
    milk_type: MilkType,
}

impl Coffee for MilkDecorator {
    fn cost(&self) -> f64 {
        let milk_cost = match self.milk_type {
            MilkType::Whole | MilkType::Skim => 0.50,
            MilkType::Almond | MilkType::Oat | MilkType::Soy => 0.75,
        };
        self.coffee.cost() + milk_cost
    }

    fn description(&self) -> String {
        format!("{} with {:?} milk", self.coffee.description(), self.milk_type)
    }

    fn ingredients(&self) -> Vec<String> {
        let mut ingredients = self.coffee.ingredients();
        ingredients.push(format!("{:?} milk", self.milk_type));
        ingredients
    }
}
```

### 3. 체이닝을 위한 빌더 패턴
```rust
impl CoffeeBuilder {
    pub fn new(size: CoffeeSize) -> Self {
        CoffeeBuilder {
            coffee: Box::new(SimpleCoffee::new(size)),
        }
    }

    pub fn with_milk(self, milk_type: MilkType) -> Self {
        CoffeeBuilder {
            coffee: Box::new(MilkDecorator::new(self.coffee, milk_type)),
        }
    }

    pub fn with_sugar(self, packets: u32) -> Self {
        CoffeeBuilder {
            coffee: Box::new(SugarDecorator::new(self.coffee, packets)),
        }
    }

    pub fn build(self) -> Box<dyn Coffee> {
        self.coffee
    }
}
```

## 사용 예제

### 1. 커피 주문 시스템
```rust
// 복잡한 커피 주문 생성
let coffee = CoffeeBuilder::new(CoffeeSize::Large)
    .with_milk(MilkType::Oat)
    .with_syrup(SyrupFlavor::Caramel, 2)
    .with_sugar(1)
    .with_whipped_cream(true)
    .build();

println!("Order: {}", coffee.description());
println!("Cost: ${:.2}", coffee.cost());

// 결과: Large Simple Coffee with Oat milk with 2 shot(s) of Caramel syrup
//       with 1 sugar packet(s) with extra whipped cream
// Cost: $6.45
```

### 2. 텍스트 포맷팅 데코레이터
```rust
pub trait TextFormatter {
    fn format(&self, text: &str) -> String;
}

pub struct BoldDecorator<T: TextFormatter> {
    formatter: T,
}

impl<T: TextFormatter> TextFormatter for BoldDecorator<T> {
    fn format(&self, text: &str) -> String {
        format!("**{}**", self.formatter.format(text))
    }
}

// 사용 예제
let formatter = ColorDecorator::new(
    UnderlineDecorator::new(
        BoldDecorator::new(
            ItalicDecorator::new(PlainText)
        )
    ),
    "red"
);

let formatted = formatter.format("Important Message");
// 결과: <span style="color: red">__***Important Message***__</span>
```

### 3. 스트림 처리 데코레이터
```rust
pub trait DataStream {
    fn process(&mut self, data: &[u8]) -> Result<Vec<u8>, String>;
    fn flush(&mut self) -> Result<Vec<u8>, String>;
}

pub struct CompressionDecorator<T: DataStream> {
    stream: T,
    compression_ratio: f32,
}

pub struct EncryptionDecorator<T: DataStream> {
    stream: T,
    key: u8,
}

// 사용 예제
let stream = BasicStream::new();
let mut decorated_stream = CompressionDecorator::new(
    EncryptionDecorator::new(stream, 42),
    0.7
);

let data = b"This is some sample data to process";
let processed = decorated_stream.process(data)?;
```

### 4. 여러 데코레이터 조합
```rust
// 수동으로 데코레이터 중첩
let coffee = SimpleCoffee::new(CoffeeSize::Medium);
let coffee = MilkDecorator::new(Box::new(coffee), MilkType::Almond);
let coffee = SyrupDecorator::new(Box::new(coffee), SyrupFlavor::Vanilla, 1);
let coffee = WhippedCreamDecorator::new(Box::new(coffee), false);

println!("Final cost: ${:.2}", coffee.cost());
println!("Description: {}", coffee.description());
```

## Rust의 장점

### 1. 타입 안전성
- 컴파일 타임에 모든 데코레이터 조합이 검증됩니다
- 잘못된 타입의 데코레이터 적용을 방지합니다

### 2. 제로 코스트 추상화
```rust
// 제네릭을 사용한 컴파일 타임 데코레이터
pub struct BoldDecorator<T: TextFormatter> {
    formatter: T, // Box가 아닌 직접 소유
}
```

### 3. 메모리 효율성
- 불필요한 힙 할당 최소화
- 소유권 시스템으로 안전한 메모리 관리

### 4. 패턴 매칭 활용
```rust
impl Coffee for SyrupDecorator {
    fn cost(&self) -> f64 {
        let syrup_cost = match self.flavor {
            SyrupFlavor::Vanilla => 0.50,
            SyrupFlavor::Caramel => 0.60,
            SyrupFlavor::Hazelnut => 0.55,
            _ => 0.60,
        };
        self.coffee.cost() + (self.shots as f64 * syrup_cost)
    }
}
```

### 5. 에러 처리
```rust
pub trait DataStream {
    fn process(&mut self, data: &[u8]) -> Result<Vec<u8>, String>;
    fn flush(&mut self) -> Result<Vec<u8>, String>;
}
```

## 적용 상황

### 1. UI 컴포넌트 장식
```rust
trait Widget {
    fn render(&self) -> String;
    fn get_size(&self) -> (u32, u32);
}

struct BorderDecorator<T: Widget> {
    widget: T,
    border_style: BorderStyle,
}

struct ScrollbarDecorator<T: Widget> {
    widget: T,
    scrollable: bool,
}
```

### 2. HTTP 미들웨어
```rust
trait HttpHandler {
    fn handle(&self, request: &HttpRequest) -> HttpResponse;
}

struct AuthenticationDecorator<T: HttpHandler> {
    handler: T,
    auth_service: Arc<AuthService>,
}

struct LoggingDecorator<T: HttpHandler> {
    handler: T,
    logger: Arc<Logger>,
}
```

### 3. 데이터 변환 파이프라인
```rust
trait DataProcessor {
    fn process(&self, data: Data) -> Result<Data, ProcessingError>;
}

struct ValidationDecorator<T: DataProcessor> {
    processor: T,
    validator: Box<dyn Validator>,
}

struct CacheDecorator<T: DataProcessor> {
    processor: T,
    cache: Arc<Mutex<HashMap<String, Data>>>,
}
```

### 4. 게임 아이템 시스템
```rust
trait GameItem {
    fn get_stats(&self) -> ItemStats;
    fn get_description(&self) -> String;
}

struct EnchantmentDecorator {
    item: Box<dyn GameItem>,
    enchantment: EnchantmentType,
    level: u32,
}
```

### 5. 로깅 시스템
```rust
trait Logger {
    fn log(&self, message: &str, level: LogLevel);
}

struct TimestampDecorator<T: Logger> {
    logger: T,
}

struct FileDecorator<T: Logger> {
    logger: T,
    file_path: PathBuf,
}
```

### 6. 금융 거래 처리
```rust
trait TransactionProcessor {
    fn process(&self, transaction: Transaction) -> Result<Receipt, TransactionError>;
}

struct FraudDetectionDecorator<T: TransactionProcessor> {
    processor: T,
    fraud_detector: Arc<FraudDetector>,
}

struct AuditDecorator<T: TransactionProcessor> {
    processor: T,
    audit_logger: Arc<AuditLogger>,
}
```

이 패턴은 Rust의 타입 시스템과 소유권 모델과 매우 잘 어울리며, 안전하고 효율적인 기능 확장을 가능하게 합니다. 특히 제네릭을 활용한 컴파일 타임 데코레이터는 런타임 오버헤드 없이 유연한 조합을 제공합니다.