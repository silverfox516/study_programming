# Singleton Pattern - Rust Implementation

## 개요

Singleton 패턴은 클래스의 인스턴스가 하나만 존재하도록 보장하고 전역 접근점을 제공합니다. Rust에서는 `OnceLock`이나 `lazy_static`을 사용하여 스레드 안전한 싱글톤을 구현할 수 있습니다.

이 패턴은 데이터베이스 연결, 설정 관리, 로깅 시스템 등에서 유용하지만, 전역 상태를 도입하므로 신중하게 사용해야 합니다.

## 구조

```rust
use std::sync::{Arc, Mutex, OnceLock};

#[derive(Debug)]
pub struct Database {
    connection_string: String,
}

static DATABASE_INSTANCE: OnceLock<Arc<Mutex<Database>>> = OnceLock::new();

pub fn get_database() -> Arc<Mutex<Database>> {
    DATABASE_INSTANCE.get_or_init(|| {
        Arc::new(Mutex::new(Database::new()))
    }).clone()
}
```

## Rust 구현

### 1. OnceLock을 사용한 구현
```rust
use std::sync::{Arc, Mutex, OnceLock};

#[derive(Debug)]
pub struct Database {
    connection_string: String,
}

impl Database {
    fn new() -> Self {
        println!("Creating database connection...");
        Database {
            connection_string: "postgresql://localhost:5432/myapp".to_string(),
        }
    }

    pub fn query(&self, sql: &str) -> Result<Vec<String>, String> {
        println!("Executing query: {}", sql);
        Ok(vec![format!("Result for: {}", sql)])
    }
}

static DATABASE_INSTANCE: OnceLock<Arc<Mutex<Database>>> = OnceLock::new();

pub fn get_database() -> Arc<Mutex<Database>> {
    DATABASE_INSTANCE.get_or_init(|| {
        Arc::new(Mutex::new(Database::new()))
    }).clone()
}
```

### 2. 설정 싱글톤
```rust
#[derive(Debug)]
pub struct Config {
    debug_mode: bool,
    max_connections: u32,
    timeout_seconds: u64,
}

impl Config {
    fn new() -> Self {
        Config {
            debug_mode: true,
            max_connections: 100,
            timeout_seconds: 30,
        }
    }

    pub fn set_debug_mode(&mut self, enabled: bool) {
        self.debug_mode = enabled;
    }

    pub fn debug_mode(&self) -> bool {
        self.debug_mode
    }
}

static CONFIG_INSTANCE: OnceLock<Arc<Mutex<Config>>> = OnceLock::new();

pub fn get_config() -> Arc<Mutex<Config>> {
    CONFIG_INSTANCE.get_or_init(|| {
        Arc::new(Mutex::new(Config::new()))
    }).clone()
}
```

## 사용 예제

```rust
// 데이터베이스 싱글톤 사용
let db = get_database();
let results = db.lock().unwrap().query("SELECT * FROM users")?;

// 설정 싱글톤 사용
let config = get_config();
config.lock().unwrap().set_debug_mode(false);

// 여러 스레드에서 동일한 인스턴스 공유
use std::thread;

let handles: Vec<_> = (0..3).map(|i| {
    thread::spawn(move || {
        let db = get_database();
        let query = format!("SELECT * FROM table_{}", i);
        db.lock().unwrap().query(&query)
    })
}).collect();
```

## Rust의 장점

### 1. 스레드 안전성
- `OnceLock`을 통한 초기화 보장
- `Arc<Mutex<T>>`로 안전한 공유

### 2. 메모리 안전성
- 소유권 시스템으로 메모리 누수 방지
- 댕글링 포인터 불가능

### 3. 컴파일 타임 보장
- 타입 안전성 검증
- 경쟁 조건 방지

## 적용 상황

### 1. 데이터베이스 연결 풀
### 2. 애플리케이션 설정 관리
### 3. 로깅 시스템
### 4. 캐시 매니저
### 5. 리소스 매니저

이 패턴은 전역 상태를 안전하게 관리해야 할 때 유용하지만, 의존성 주입 등의 대안을 고려하는 것이 좋습니다.