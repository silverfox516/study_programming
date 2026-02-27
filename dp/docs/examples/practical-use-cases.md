# 디자인 패턴 실용 사례

## 🏢 실제 소프트웨어에서의 디자인 패턴 활용

이 문서는 실제 소프트웨어 개발에서 디자인 패턴이 어떻게 활용되는지 구체적인 사례를 통해 설명합니다.

## 🌐 웹 개발에서의 패턴 활용

### 1. MVC 아키텍처 (Multiple Patterns)

**사용된 패턴**: Observer, Strategy, Command, Facade

```cpp
// C++ 웹 프레임워크 예제
class WebApplication {
    // Facade Pattern - 복잡한 웹 서버 기능을 단순화
    std::unique_ptr<Router> router;
    std::unique_ptr<Database> database;
    std::unique_ptr<TemplateEngine> templateEngine;

public:
    void handleRequest(const HttpRequest& request) {
        // Command Pattern - 요청을 객체로 캡슐화
        auto command = router->getCommand(request.getPath());
        auto response = command->execute(request);

        // Observer Pattern - 로깅, 모니터링 등
        notifyObservers(request, response);
    }
};

// Strategy Pattern - 다양한 인증 방식
class AuthenticationStrategy {
public:
    virtual ~AuthenticationStrategy() = default;
    virtual bool authenticate(const User& user) = 0;
};

class JWTAuthentication : public AuthenticationStrategy {
public:
    bool authenticate(const User& user) override {
        // JWT 토큰 검증 로직
        return validateJWTToken(user.getToken());
    }
};
```

### 2. REST API 설계

**사용된 패턴**: Factory, Builder, Adapter

```rust
// Rust REST API 예제
use serde::{Deserialize, Serialize};

// Builder Pattern - 복잡한 API 응답 구성
#[derive(Serialize)]
struct ApiResponse<T> {
    data: Option<T>,
    error: Option<String>,
    metadata: ResponseMetadata,
}

impl<T> ApiResponse<T> {
    pub fn success(data: T) -> Self {
        Self {
            data: Some(data),
            error: None,
            metadata: ResponseMetadata::default(),
        }
    }

    pub fn error(message: String) -> Self {
        Self {
            data: None,
            error: Some(message),
            metadata: ResponseMetadata::default(),
        }
    }
}

// Factory Pattern - 다양한 데이터 소스 생성
trait DataSource {
    fn fetch_users(&self) -> Result<Vec<User>, DatabaseError>;
}

struct DataSourceFactory;

impl DataSourceFactory {
    pub fn create(db_type: &str) -> Box<dyn DataSource> {
        match db_type {
            "postgres" => Box::new(PostgresDataSource::new()),
            "mongodb" => Box::new(MongoDataSource::new()),
            "redis" => Box::new(RedisDataSource::new()),
            _ => Box::new(InMemoryDataSource::new()),
        }
    }
}
```

## 🎮 게임 개발에서의 패턴 활용

### 1. 게임 엔진 아키텍처

**사용된 패턴**: Component, Observer, State, Command

```cpp
// C++ 게임 엔진 예제
class GameObject {
    std::vector<std::unique_ptr<Component>> components;

public:
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        components.push_back(std::move(component));
        return ptr;
    }
};

// State Pattern - 게임 캐릭터 상태
class CharacterState {
public:
    virtual ~CharacterState() = default;
    virtual void update(Character& character, float deltaTime) = 0;
    virtual void enter(Character& character) {}
    virtual void exit(Character& character) {}
};

class IdleState : public CharacterState {
public:
    void update(Character& character, float deltaTime) override {
        if (character.getInput().isMoving()) {
            character.changeState(std::make_unique<MovingState>());
        }
        if (character.getInput().isAttacking()) {
            character.changeState(std::make_unique<AttackingState>());
        }
    }
};

// Command Pattern - 입력 처리
class InputCommand {
public:
    virtual ~InputCommand() = default;
    virtual void execute(Character& character) = 0;
    virtual void undo(Character& character) = 0;
};

class MoveCommand : public InputCommand {
    Vector3 direction;
public:
    MoveCommand(const Vector3& dir) : direction(dir) {}

    void execute(Character& character) override {
        character.move(direction);
    }

    void undo(Character& character) override {
        character.move(-direction);
    }
};
```

### 2. 게임 리소스 관리

**사용된 패턴**: Flyweight, Proxy, Singleton

```rust
// Rust 게임 리소스 관리
use std::collections::HashMap;
use std::sync::{Arc, Mutex, LazyLock};

// Flyweight Pattern - 텍스처 공유
#[derive(Clone)]
struct Texture {
    id: u32,
    width: u32,
    height: u32,
    data: Arc<Vec<u8>>, // 실제 데이터는 공유
}

struct TextureManager {
    textures: HashMap<String, Arc<Texture>>,
}

impl TextureManager {
    fn get_texture(&mut self, path: &str) -> Arc<Texture> {
        if let Some(texture) = self.textures.get(path) {
            return texture.clone();
        }

        let texture = Arc::new(self.load_texture(path));
        self.textures.insert(path.to_string(), texture.clone());
        texture
    }
}

// Singleton Pattern - 글로벌 리소스 매니저
static RESOURCE_MANAGER: LazyLock<Arc<Mutex<ResourceManager>>> =
    LazyLock::new(|| Arc::new(Mutex::new(ResourceManager::new())));

struct ResourceManager {
    texture_manager: TextureManager,
    audio_manager: AudioManager,
    model_manager: ModelManager,
}

// Proxy Pattern - 지연 로딩
struct LazyTexture {
    path: String,
    texture: Option<Arc<Texture>>,
}

impl LazyTexture {
    fn get_texture(&mut self) -> Arc<Texture> {
        if self.texture.is_none() {
            let manager = RESOURCE_MANAGER.lock().unwrap();
            self.texture = Some(manager.texture_manager.get_texture(&self.path));
        }
        self.texture.as_ref().unwrap().clone()
    }
}
```

## 🏭 기업용 소프트웨어에서의 패턴

### 1. 데이터 처리 파이프라인

**사용된 패턴**: Chain of Responsibility, Template Method, Strategy

```cpp
// C++ 데이터 처리 파이프라인
class DataProcessor {
public:
    virtual ~DataProcessor() = default;
    virtual bool canProcess(const DataType& type) = 0;
    virtual ProcessResult process(const Data& data) = 0;
    virtual void setNext(std::unique_ptr<DataProcessor> next) {
        nextProcessor = std::move(next);
    }

protected:
    std::unique_ptr<DataProcessor> nextProcessor;

    ProcessResult processNext(const Data& data) {
        if (nextProcessor) {
            return nextProcessor->process(data);
        }
        return ProcessResult::success();
    }
};

// Template Method - 데이터 처리 템플릿
class AbstractDataImporter {
public:
    ImportResult importData(const std::string& source) {
        if (!validate(source)) {
            return ImportResult::error("Validation failed");
        }

        auto rawData = extract(source);
        auto transformedData = transform(rawData);
        return load(transformedData);
    }

protected:
    virtual bool validate(const std::string& source) = 0;
    virtual RawData extract(const std::string& source) = 0;
    virtual ProcessedData transform(const RawData& data) = 0;
    virtual ImportResult load(const ProcessedData& data) = 0;
};

class CSVImporter : public AbstractDataImporter {
protected:
    bool validate(const std::string& source) override {
        return source.ends_with(".csv");
    }

    RawData extract(const std::string& source) override {
        // CSV 파일 읽기 로직
        return parseCSV(source);
    }

    ProcessedData transform(const RawData& data) override {
        // CSV 데이터 변환 로직
        return convertToStandardFormat(data);
    }

    ImportResult load(const ProcessedData& data) override {
        // 데이터베이스에 저장
        return database.insert(data);
    }
};
```

### 2. 마이크로서비스 아키텍처

**사용된 패턴**: Facade, Adapter, Circuit Breaker (custom)

```rust
// Rust 마이크로서비스 예제
use std::time::{Duration, Instant};
use std::sync::{Arc, Mutex};

// Facade Pattern - 마이크로서비스 통합 인터페이스
pub struct UserService {
    user_repo: Arc<dyn UserRepository>,
    auth_service: Arc<dyn AuthenticationService>,
    notification_service: Arc<dyn NotificationService>,
    circuit_breaker: Arc<Mutex<CircuitBreaker>>,
}

impl UserService {
    pub async fn create_user(&self, user_data: CreateUserRequest) -> Result<User, UserError> {
        // Circuit Breaker Pattern
        if !self.circuit_breaker.lock().unwrap().can_execute() {
            return Err(UserError::ServiceUnavailable);
        }

        // 여러 서비스 조율
        let result = async {
            // 1. 사용자 생성
            let user = self.user_repo.create(user_data).await?;

            // 2. 인증 설정
            self.auth_service.setup_auth(&user).await?;

            // 3. 환영 알림 발송
            self.notification_service.send_welcome_email(&user).await?;

            Ok(user)
        }.await;

        // Circuit Breaker 상태 업데이트
        match &result {
            Ok(_) => self.circuit_breaker.lock().unwrap().record_success(),
            Err(_) => self.circuit_breaker.lock().unwrap().record_failure(),
        }

        result
    }
}

// Adapter Pattern - 외부 서비스 통합
struct EmailServiceAdapter {
    external_client: ExternalEmailClient,
}

impl NotificationService for EmailServiceAdapter {
    async fn send_welcome_email(&self, user: &User) -> Result<(), NotificationError> {
        // 외부 이메일 서비스 API에 맞게 데이터 변환
        let email_request = ExternalEmailRequest {
            to: user.email.clone(),
            subject: "Welcome!".to_string(),
            body: format!("Welcome, {}!", user.name),
            template_id: "welcome_template".to_string(),
        };

        self.external_client.send_email(email_request).await
            .map_err(|e| NotificationError::EmailFailed(e.to_string()))
    }
}
```

## 📱 모바일 앱 개발

### 1. MVP 아키텍처 (iOS/Android)

**사용된 패턴**: MVP, Observer, Command

```cpp
// C++ 크로스 플랫폼 모바일 앱
class UserListPresenter {
    UserListView* view;
    UserRepository* repository;

public:
    UserListPresenter(UserListView* v, UserRepository* r)
        : view(v), repository(r) {}

    void loadUsers() {
        view->showLoading();

        // Command Pattern - 비동기 작업
        auto command = std::make_unique<LoadUsersCommand>(
            repository,
            [this](const std::vector<User>& users) {
                view->hideLoading();
                view->displayUsers(users);
            },
            [this](const std::string& error) {
                view->hideLoading();
                view->showError(error);
            }
        );

        commandQueue.execute(std::move(command));
    }

    void onUserSelected(const User& user) {
        // Navigator pattern
        navigator->navigateToUserDetail(user.getId());
    }
};

// Observer Pattern - UI 업데이트
class UserListView : public Observer<UserListEvent> {
public:
    void update(const UserListEvent& event) override {
        switch (event.type) {
            case UserListEvent::USERS_LOADED:
                displayUsers(event.users);
                break;
            case UserListEvent::USER_ADDED:
                addUserToList(event.user);
                break;
            case UserListEvent::USER_REMOVED:
                removeUserFromList(event.user.getId());
                break;
        }
    }
};
```

## 🤖 AI/ML 시스템에서의 패턴

### 1. 머신러닝 파이프라인

**사용된 패턴**: Strategy, Pipeline, Factory

```python
# Python ML 파이프라인 (개념적 예제)
from abc import ABC, abstractmethod
from typing import Any, List

# Strategy Pattern - 다양한 전처리 알고리즘
class PreprocessingStrategy(ABC):
    @abstractmethod
    def preprocess(self, data: Any) -> Any:
        pass

class StandardScalingStrategy(PreprocessingStrategy):
    def preprocess(self, data):
        return (data - data.mean()) / data.std()

class MinMaxScalingStrategy(PreprocessingStrategy):
    def preprocess(self, data):
        return (data - data.min()) / (data.max() - data.min())

# Pipeline Pattern - ML 워크플로우
class MLPipeline:
    def __init__(self):
        self.steps: List[tuple] = []

    def add_step(self, name: str, processor: Any):
        self.steps.append((name, processor))
        return self

    def fit_transform(self, data):
        for name, processor in self.steps:
            print(f"Executing {name}...")
            data = processor.fit_transform(data)
        return data

# Factory Pattern - 모델 생성
class ModelFactory:
    @staticmethod
    def create_model(model_type: str, **kwargs):
        if model_type == "linear_regression":
            return LinearRegressionModel(**kwargs)
        elif model_type == "random_forest":
            return RandomForestModel(**kwargs)
        elif model_type == "neural_network":
            return NeuralNetworkModel(**kwargs)
        else:
            raise ValueError(f"Unknown model type: {model_type}")
```

## 🔧 시스템 프로그래밍

### 1. 운영체제 커널 모듈

**사용된 패턴**: Singleton, Factory, Strategy

```c
// C 운영체제 커널 예제
#include <linux/module.h>
#include <linux/kernel.h>

// Singleton Pattern - 디바이스 드라이버 관리자
static struct device_manager* dev_manager = NULL;

struct device_manager* get_device_manager(void) {
    if (dev_manager == NULL) {
        dev_manager = kmalloc(sizeof(struct device_manager), GFP_KERNEL);
        if (dev_manager) {
            init_device_manager(dev_manager);
        }
    }
    return dev_manager;
}

// Strategy Pattern - 디바이스별 처리 전략
struct device_ops {
    int (*open)(struct inode*, struct file*);
    int (*close)(struct inode*, struct file*);
    ssize_t (*read)(struct file*, char*, size_t, loff_t*);
    ssize_t (*write)(struct file*, const char*, size_t, loff_t*);
};

// Factory Pattern - 디바이스 드라이버 생성
struct device_driver* create_device_driver(const char* device_type) {
    if (strcmp(device_type, "block") == 0) {
        return create_block_device_driver();
    } else if (strcmp(device_type, "char") == 0) {
        return create_char_device_driver();
    } else if (strcmp(device_type, "network") == 0) {
        return create_network_device_driver();
    }
    return NULL;
}
```

## 📊 성능 최적화 패턴

### 1. 캐싱 시스템

**사용된 패턴**: Proxy, Decorator, Strategy

```rust
// Rust 고성능 캐싱 시스템
use std::collections::HashMap;
use std::sync::{Arc, RwLock};
use std::time::{Duration, Instant};

// Strategy Pattern - 다양한 캐시 정책
trait EvictionStrategy: Send + Sync {
    fn should_evict(&self, key: &str, metadata: &CacheMetadata) -> bool;
    fn on_access(&self, key: &str, metadata: &mut CacheMetadata);
}

struct LRUEvictionStrategy {
    max_age: Duration,
}

impl EvictionStrategy for LRUEvictionStrategy {
    fn should_evict(&self, _key: &str, metadata: &CacheMetadata) -> bool {
        metadata.last_access.elapsed() > self.max_age
    }

    fn on_access(&self, _key: &str, metadata: &mut CacheMetadata) {
        metadata.last_access = Instant::now();
        metadata.access_count += 1;
    }
}

// Proxy Pattern - 캐시가 포함된 데이터 접근
struct CachedDataService<T> {
    cache: Arc<RwLock<HashMap<String, CacheEntry<T>>>>,
    data_source: Arc<dyn DataSource<T>>,
    eviction_strategy: Arc<dyn EvictionStrategy>,
}

impl<T: Clone> CachedDataService<T> {
    async fn get(&self, key: &str) -> Result<T, DataError> {
        // 캐시에서 먼저 확인
        {
            let cache = self.cache.read().unwrap();
            if let Some(entry) = cache.get(key) {
                if !self.eviction_strategy.should_evict(key, &entry.metadata) {
                    return Ok(entry.data.clone());
                }
            }
        }

        // 캐시 미스 - 원본 데이터 소스에서 로드
        let data = self.data_source.get(key).await?;

        // 캐시에 저장
        {
            let mut cache = self.cache.write().unwrap();
            cache.insert(key.to_string(), CacheEntry {
                data: data.clone(),
                metadata: CacheMetadata::new(),
            });
        }

        Ok(data)
    }
}
```

## 🎯 패턴 선택 가이드라인

### 프로젝트 유형별 추천 패턴

| 프로젝트 유형 | 핵심 패턴 | 이유 |
|---------------|-----------|------|
| **웹 애플리케이션** | MVC, Factory, Observer | 요청 처리, 객체 생성, 이벤트 처리 |
| **게임** | State, Command, Component | 상태 관리, 입력 처리, 모듈화 |
| **기업용 SW** | Template Method, Chain of Responsibility | 비즈니스 로직, 워크플로우 |
| **모바일 앱** | MVP/MVVM, Observer, Command | UI 분리, 반응형 UI, 비동기 처리 |
| **시스템 SW** | Singleton, Factory, Strategy | 리소스 관리, 플랫폼 추상화 |

### 성능이 중요한 시스템

```rust
// 제로 코스트 추상화를 활용한 고성능 패턴
trait DataProcessor {
    type Input;
    type Output;

    fn process(&self, input: Self::Input) -> Self::Output;
}

// 컴파일 타임에 완전히 최적화됨
struct Pipeline<T1, T2>
where
    T1: DataProcessor,
    T2: DataProcessor<Input = T1::Output>,
{
    first: T1,
    second: T2,
}

impl<T1, T2> DataProcessor for Pipeline<T1, T2>
where
    T1: DataProcessor,
    T2: DataProcessor<Input = T1::Output>,
{
    type Input = T1::Input;
    type Output = T2::Output;

    #[inline]
    fn process(&self, input: Self::Input) -> Self::Output {
        let intermediate = self.first.process(input);
        self.second.process(intermediate)
    }
}
```

## 💡 결론

디자인 패턴은 도구일 뿐입니다. 중요한 것은:

1. **문제 이해**: 패턴을 적용하기 전에 실제 문제를 명확히 파악
2. **적절한 선택**: 상황에 맞는 패턴 선택
3. **과도한 사용 피하기**: 패턴을 위한 패턴은 피하기
4. **언어의 특성 활용**: 각 언어의 강점을 살린 구현

실제 프로젝트에서는 여러 패턴이 조합되어 사용되며, 이들이 시스템의 아키텍처를 구성하는 기본 블록이 됩니다.