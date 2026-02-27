# Null Object Pattern - Rust Implementation

## 개요

Null Object 패턴은 null 참조 대신 기본 행동을 제공하는 객체를 사용합니다. Rust에서는 `Option`과 함께 사용하여 null 안전성을 더욱 강화할 수 있습니다.

## 구조

```rust
pub trait Logger {
    fn log(&self, message: &str);
    fn set_level(&mut self, level: LogLevel);
}

#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum LogLevel {
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
}

// 실제 구현
pub struct FileLogger {
    filename: String,
    level: LogLevel,
}

impl FileLogger {
    pub fn new(filename: String, level: LogLevel) -> Self {
        Self { filename, level }
    }
}

impl Logger for FileLogger {
    fn log(&self, message: &str) {
        println!("[FILE LOG to {}] {}", self.filename, message);
    }

    fn set_level(&mut self, level: LogLevel) {
        self.level = level;
    }
}

// Null Object 구현
pub struct NullLogger;

impl Logger for NullLogger {
    fn log(&self, _message: &str) {
        // 아무것도 하지 않음
    }

    fn set_level(&mut self, _level: LogLevel) {
        // 아무것도 하지 않음
    }
}

// Logger Provider
pub struct LoggerProvider;

impl LoggerProvider {
    pub fn get_logger(enabled: bool, filename: Option<String>) -> Box<dyn Logger> {
        if enabled {
            if let Some(file) = filename {
                Box::new(FileLogger::new(file, LogLevel::Info))
            } else {
                Box::new(ConsoleLogger::new(LogLevel::Info))
            }
        } else {
            Box::new(NullLogger)
        }
    }
}

// Service that uses logger
pub struct UserService {
    logger: Box<dyn Logger>,
}

impl UserService {
    pub fn new(logger: Box<dyn Logger>) -> Self {
        Self { logger }
    }

    pub fn create_user(&self, name: &str) -> String {
        self.logger.log(&format!("Creating user: {}", name));

        // 비즈니스 로직
        let user_id = "user_123";

        self.logger.log(&format!("User created with ID: {}", user_id));
        user_id.to_string()
    }

    pub fn delete_user(&self, user_id: &str) -> bool {
        self.logger.log(&format!("Deleting user: {}", user_id));

        // 비즈니스 로직
        let success = true;

        if success {
            self.logger.log(&format!("User {} deleted successfully", user_id));
        } else {
            self.logger.log(&format!("Failed to delete user {}", user_id));
        }

        success
    }
}
```

## 사용 예제

```rust
// 로깅이 활성화된 경우
let logger = LoggerProvider::get_logger(true, Some("app.log".to_string()));
let service_with_logging = UserService::new(logger);
service_with_logging.create_user("Alice");

// 로깅이 비활성화된 경우 - NullLogger 사용
let null_logger = LoggerProvider::get_logger(false, None);
let service_without_logging = UserService::new(null_logger);
service_without_logging.create_user("Bob"); // 로그 없음

// 조건부 로깅
let debug_mode = std::env::var("DEBUG").is_ok();
let logger = LoggerProvider::get_logger(debug_mode, Some("debug.log".to_string()));
let service = UserService::new(logger);
```

## 다른 예제: 캐시 시스템

```rust
pub trait Cache {
    fn get(&self, key: &str) -> Option<String>;
    fn set(&mut self, key: &str, value: String);
    fn clear(&mut self);
}

pub struct MemoryCache {
    data: HashMap<String, String>,
}

impl Cache for MemoryCache {
    fn get(&self, key: &str) -> Option<String> {
        self.data.get(key).cloned()
    }

    fn set(&mut self, key: &str, value: String) {
        self.data.insert(key.to_string(), value);
    }

    fn clear(&mut self) {
        self.data.clear();
    }
}

// Null Object for Cache
pub struct NoCache;

impl Cache for NoCache {
    fn get(&self, _key: &str) -> Option<String> {
        None // 항상 캐시 미스
    }

    fn set(&mut self, _key: &str, _value: String) {
        // 캐싱하지 않음
    }

    fn clear(&mut self) {
        // 아무것도 할 게 없음
    }
}
```

## Rust에서의 특별한 고려사항

### Option과의 조합
```rust
pub struct ServiceWithOptionalLogger {
    logger: Option<Box<dyn Logger>>,
}

impl ServiceWithOptionalLogger {
    pub fn log(&self, message: &str) {
        // Option 사용
        if let Some(ref logger) = self.logger {
            logger.log(message);
        }

        // 또는 Null Object 패턴 사용
        // self.logger.log(message); // logger가 NullLogger일 수 있음
    }
}
```

## 적용 상황

### 1. 로깅 시스템 (선택적 로깅)
### 2. 캐싱 시스템 (캐시 비활성화)
### 3. 알림 시스템 (알림 비활성화)
### 4. 데이터 검증 (검증 생략)
### 5. 테스트 더블 (Mock 객체 대신)