# Dependency Injection Pattern - Rust Implementation

## 개요

Dependency Injection(DI)는 의존성 역전 제어(Inversion of Control)를 달성하기 위한 기법으로, 클래스와 그 의존성 간의 결합도를 낮춥니다. 의존성을 내부에서 생성하는 대신 외부에서 제공받습니다.

이 패턴은 테스트 가능성, 유연성, 모듈화를 향상시킵니다. Rust에서는 trait과 Arc를 활용하여 타입 안전한 의존성 주입을 구현할 수 있습니다.

## 구조

```rust
// 서비스 인터페이스
pub trait Logger {
    fn log(&self, message: &str);
}

pub trait Database {
    fn save(&self, data: &str) -> Result<u32, String>;
    fn find(&self, id: u32) -> Result<String, String>;
}

// 비즈니스 서비스 (의존성을 주입받음)
pub struct UserService {
    logger: Arc<dyn Logger>,
    database: Arc<dyn Database>,
    email_service: Arc<dyn EmailService>,
}

// DI 컨테이너
pub struct Container {
    services: HashMap<String, Box<dyn Any>>,
}
```

## Rust 구현

Rust의 Dependency Injection 구현은 다음과 같은 특징을 가집니다:

### 1. 서비스 인터페이스 정의
```rust
pub trait Logger {
    fn log(&self, message: &str);
}

pub trait Database {
    fn save(&self, data: &str) -> Result<u32, String>;
    fn find(&self, id: u32) -> Result<String, String>;
}

pub trait EmailService {
    fn send_email(&self, to: &str, subject: &str, body: &str) -> Result<(), String>;
}
```

### 2. 구체적인 구현
```rust
pub struct ConsoleLogger;

impl Logger for ConsoleLogger {
    fn log(&self, message: &str) {
        println!("[LOG] {}", message);
    }
}

pub struct FileLogger {
    filename: String,
}

impl Logger for FileLogger {
    fn log(&self, message: &str) {
        println!("[FILE LOG to {}] {}", self.filename, message);
    }
}

pub struct InMemoryDatabase {
    data: Mutex<HashMap<u32, String>>,
    next_id: Mutex<u32>,
}

impl Database for InMemoryDatabase {
    fn save(&self, data: &str) -> Result<u32, String> {
        let mut db = self.data.lock().map_err(|e| e.to_string())?;
        let mut id = self.next_id.lock().map_err(|e| e.to_string())?;

        let current_id = *id;
        db.insert(current_id, data.to_string());
        *id += 1;

        Ok(current_id)
    }

    fn find(&self, id: u32) -> Result<String, String> {
        let db = self.data.lock().map_err(|e| e.to_string())?;
        db.get(&id)
            .cloned()
            .ok_or_else(|| format!("Record with ID {} not found", id))
    }
}
```

### 3. 생성자 주입
```rust
impl UserService {
    // Constructor injection
    pub fn new(
        logger: Arc<dyn Logger>,
        database: Arc<dyn Database>,
        email_service: Arc<dyn EmailService>,
    ) -> Self {
        Self {
            logger,
            database,
            email_service,
        }
    }

    pub fn create_user(&self, name: &str, email: &str) -> Result<u32, String> {
        self.logger.log(&format!("Creating user: {}", name));

        let user_data = format!("{{\"name\":\"{}\",\"email\":\"{}\"}}", name, email);
        let user_id = self.database.save(&user_data)?;

        self.logger.log(&format!("User created with ID: {}", user_id));

        // 환영 이메일 발송
        let subject = "Welcome!";
        let body = &format!("Hello {}, welcome to our service!", name);
        self.email_service.send_email(email, subject, body)?;

        Ok(user_id)
    }
}
```

## 사용 예제

### 1. 수동 의존성 주입
```rust
// 의존성 생성
let logger = Arc::new(ConsoleLogger);
let database = Arc::new(InMemoryDatabase::new());
let email_service = Arc::new(MockEmailService);

// 서비스에 주입
let user_service = UserService::new(logger, database, email_service);

// 서비스 사용
match user_service.create_user("Alice Johnson", "alice@example.com") {
    Ok(id) => {
        println!("User created with ID: {}", id);
        match user_service.get_user(id) {
            Ok(user_data) => println!("User data: {}", user_data),
            Err(e) => println!("Error: {}", e),
        }
    }
    Err(e) => println!("Error creating user: {}", e),
}
```

### 2. 빌더 패턴을 활용한 DI
```rust
pub struct UserServiceBuilder {
    logger: Option<Arc<dyn Logger>>,
    database: Option<Arc<dyn Database>>,
    email_service: Option<Arc<dyn EmailService>>,
}

impl UserServiceBuilder {
    pub fn with_logger(mut self, logger: Arc<dyn Logger>) -> Self {
        self.logger = Some(logger);
        self
    }

    pub fn with_database(mut self, database: Arc<dyn Database>) -> Self {
        self.database = Some(database);
        self
    }

    pub fn build(self) -> Result<UserService, String> {
        let logger = self.logger.ok_or("Logger is required")?;
        let database = self.database.ok_or("Database is required")?;
        let email_service = self.email_service.ok_or("Email service is required")?;

        Ok(UserService::new(logger, database, email_service))
    }
}

// 사용
let user_service = UserServiceBuilder::new()
    .with_logger(Arc::new(FileLogger::new("app.log".to_string())))
    .with_database(Arc::new(InMemoryDatabase::new()))
    .with_email_service(Arc::new(MockEmailService))
    .build()?;
```

### 3. 팩토리 패턴과 결합
```rust
pub struct ServiceFactory;

impl ServiceFactory {
    pub fn create_development_user_service() -> UserService {
        let logger = Arc::new(ConsoleLogger);
        let database = Arc::new(InMemoryDatabase::new());
        let email_service = Arc::new(MockEmailService);

        UserService::new(logger, database, email_service)
    }

    pub fn create_production_user_service() -> UserService {
        let logger = Arc::new(FileLogger::new("app.log".to_string()));
        let database = Arc::new(PostgreSQLDatabase::new());  // 실제 DB
        let email_service = Arc::new(SMTPEmailService::new());  // 실제 이메일 서비스

        UserService::new(logger, database, email_service)
    }
}

// 환경에 따른 서비스 생성
let user_service = if cfg!(debug_assertions) {
    ServiceFactory::create_development_user_service()
} else {
    ServiceFactory::create_production_user_service()
};
```

### 4. 간단한 DI 컨테이너
```rust
pub struct Container {
    services: HashMap<String, Box<dyn Any>>,
}

impl Container {
    pub fn register<T: 'static>(&mut self, name: &str, service: T) {
        self.services.insert(name.to_string(), Box::new(service));
    }

    pub fn resolve<T: 'static>(&self, name: &str) -> Option<&T> {
        self.services
            .get(name)
            .and_then(|service| service.downcast_ref::<T>())
    }
}

// 사용
let mut container = Container::new();
container.register("logger", ConsoleLogger);
container.register("database", InMemoryDatabase::new());
container.register("email_service", MockEmailService);

// 의존성 해결
let logger = container.resolve::<ConsoleLogger>("logger").unwrap();
let database = container.resolve::<InMemoryDatabase>("database").unwrap();
```

## Rust의 장점

### 1. 타입 안전성
- 컴파일 타임에 의존성 타입이 검증됩니다
- trait object를 통한 안전한 추상화

### 2. 메모리 안전성
```rust
// Arc로 안전한 공유 참조
pub struct UserService {
    logger: Arc<dyn Logger>,
    database: Arc<dyn Database>,
}
```

### 3. 제로 코스트 추상화
- trait 시스템으로 성능 오버헤드 최소화
- 컴파일 타임 최적화

### 4. 에러 처리
```rust
// Result를 활용한 명시적 에러 처리
pub fn build(self) -> Result<UserService, String> {
    let logger = self.logger.ok_or("Logger is required")?;
    // ...
}
```

### 5. 테스트 용이성
```rust
#[cfg(test)]
mod tests {
    struct TestLogger {
        messages: Mutex<Vec<String>>,
    }

    impl Logger for TestLogger {
        fn log(&self, message: &str) {
            self.messages.lock().unwrap().push(message.to_string());
        }
    }

    #[test]
    fn test_user_service() {
        let test_logger = Arc::new(TestLogger::new());
        let user_service = UserService::new(test_logger.clone(), /* ... */);

        user_service.create_user("Test", "test@example.com").unwrap();

        let messages = test_logger.get_messages();
        assert!(messages.contains(&"Creating user: Test".to_string()));
    }
}
```

## 적용 상황

### 1. 웹 애플리케이션
```rust
pub struct WebController {
    user_service: Arc<dyn UserService>,
    auth_service: Arc<dyn AuthService>,
    logger: Arc<dyn Logger>,
}

pub struct WebApplication {
    container: Container,
}

impl WebApplication {
    pub fn configure_services(&mut self) {
        self.container.register("logger", FileLogger::new("app.log"));
        self.container.register("database", PostgreSQLDatabase::new());
        self.container.register("user_service", UserService::new(/* ... */));
    }
}
```

### 2. 마이크로서비스
```rust
trait OrderService {
    fn create_order(&self, order: Order) -> Result<OrderId, OrderError>;
}

trait PaymentService {
    fn process_payment(&self, payment: Payment) -> Result<PaymentId, PaymentError>;
}

pub struct OrderProcessor {
    order_service: Arc<dyn OrderService>,
    payment_service: Arc<dyn PaymentService>,
    notification_service: Arc<dyn NotificationService>,
}
```

### 3. 게임 엔진
```rust
trait Renderer {
    fn render(&self, objects: &[GameObject]);
}

trait AudioSystem {
    fn play_sound(&self, sound_id: SoundId);
}

pub struct GameEngine {
    renderer: Arc<dyn Renderer>,
    audio: Arc<dyn AudioSystem>,
    physics: Arc<dyn PhysicsEngine>,
}
```

### 4. 데이터 처리 파이프라인
```rust
trait DataSource {
    fn read_data(&self) -> Result<Vec<Record>, DataError>;
}

trait DataProcessor {
    fn process(&self, data: Vec<Record>) -> Result<Vec<ProcessedRecord>, ProcessingError>;
}

trait DataSink {
    fn write_data(&self, data: Vec<ProcessedRecord>) -> Result<(), WriteError>;
}

pub struct DataPipeline {
    source: Arc<dyn DataSource>,
    processor: Arc<dyn DataProcessor>,
    sink: Arc<dyn DataSink>,
}
```

### 5. 설정 기반 DI
```rust
#[derive(Deserialize)]
pub struct AppConfig {
    database: DatabaseConfig,
    logging: LoggingConfig,
    email: EmailConfig,
}

impl AppConfig {
    pub fn create_user_service(&self) -> UserService {
        let logger = self.logging.create_logger();
        let database = self.database.create_connection();
        let email_service = self.email.create_service();

        UserService::new(logger, database, email_service)
    }
}
```

이 패턴은 Rust의 소유권 시스템과 trait 시스템과 잘 어울리며, 안전하고 테스트 가능한 모듈화된 아키텍처를 구축할 수 있게 해줍니다.