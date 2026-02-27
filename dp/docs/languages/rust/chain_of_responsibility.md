# Chain of Responsibility Pattern - Rust Implementation

## 개요

Chain of Responsibility 패턴은 요청을 처리할 수 있는 핸들러들의 체인을 통해 요청을 전달합니다. 각 핸들러는 요청을 받으면 처리할지 아니면 체인의 다음 핸들러로 전달할지 결정합니다.

이 패턴은 요청을 보내는 객체와 요청을 처리하는 객체를 분리하며, 여러 객체가 요청을 처리할 기회를 가질 수 있게 합니다. Rust에서는 trait과 Option을 활용하여 안전하고 효율적으로 구현할 수 있습니다.

## 구조

```rust
pub trait SupportHandler {
    fn handle_request(&mut self, request: &mut SupportRequest) -> Result<String, String>;
    fn set_next(&mut self, next_handler: Box<dyn SupportHandler>);
    fn get_handler_name(&self) -> &str;
    fn can_handle(&self, request: &SupportRequest) -> bool;
}

pub struct Level1Support {
    name: String,
    next_handler: Option<Box<dyn SupportHandler>>,
    resolved_count: u32,
}

pub struct SupportSystem {
    first_handler: Option<Box<dyn SupportHandler>>,
    processed_requests: Vec<SupportRequest>,
}
```

## Rust 구현

Rust의 Chain of Responsibility 구현은 다음과 같은 특징을 가집니다:

### 1. 타입 안전한 핸들러 체인
```rust
impl SupportHandler for Level1Support {
    fn handle_request(&mut self, request: &mut SupportRequest) -> Result<String, String> {
        if self.can_handle(request) {
            request.assign_to(&self.name);
            self.resolved_count += 1;
            // 처리 로직
            Ok(format!("Request {} handled by {}", request.id, self.name))
        } else if let Some(ref mut next) = self.next_handler {
            next.handle_request(request)
        } else {
            Err(format!("No handler available for request {}", request.id))
        }
    }

    fn can_handle(&self, request: &SupportRequest) -> bool {
        matches!(request.priority, Priority::Low | Priority::Medium) &&
        matches!(request.issue_type, IssueType::General | IssueType::Technical)
    }
}
```

### 2. 열거형을 활용한 요청 분류
```rust
#[derive(Debug, Clone, PartialEq)]
pub enum IssueType {
    Technical,
    Billing,
    General,
    Refund,
    Critical,
}

#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum Priority {
    Low = 1,
    Medium = 2,
    High = 3,
    Critical = 4,
}
```

### 3. 멀티레벨 지원 시스템
```rust
// Level 1: 기본 문제
impl Level1Support {
    fn can_handle(&self, request: &SupportRequest) -> bool {
        matches!(request.priority, Priority::Low | Priority::Medium) &&
        matches!(request.issue_type, IssueType::General | IssueType::Technical)
    }
}

// Level 2: 복잡한 기술 문제
impl Level2Support {
    fn can_handle(&self, request: &SupportRequest) -> bool {
        (matches!(request.priority, Priority::Medium | Priority::High) &&
         self.specializations.contains(&request.issue_type)) ||
        (matches!(request.issue_type, IssueType::Billing) && request.priority <= Priority::High)
    }
}

// Management: 모든 문제 처리 가능
impl ManagementSupport {
    fn can_handle(&self, _request: &SupportRequest) -> bool {
        true // 관리자는 모든 요청을 처리할 수 있음
    }
}
```

## 사용 예제

### 1. 지원 티켓 시스템
```rust
// 핸들러 체인 생성
let mut level1 = Level1Support::new();
let mut level2 = Level2Support::new(vec![IssueType::Technical, IssueType::Billing]);
let management = ManagementSupport::new();

// 체인 연결
level2.set_next(Box::new(management));
level1.set_next(Box::new(level2));

// 시스템 설정
let mut support_system = SupportSystem::new();
support_system.set_handler_chain(Box::new(level1));

// 요청 처리
let request = SupportRequest::new(1, "John", IssueType::General, Priority::Low, "Simple question");
match support_system.process_request(request) {
    Ok(result) => println!("Handled: {}", result),
    Err(error) => println!("Error: {}", error),
}
```

### 2. HTTP 미들웨어 체인
```rust
pub trait RequestProcessor {
    fn process(&mut self, request: &mut HttpRequest) -> Result<Option<HttpResponse>, String>;
    fn set_next(&mut self, next: Box<dyn RequestProcessor>);
}

// 인증 미들웨어
pub struct AuthenticationMiddleware {
    name: String,
    next: Option<Box<dyn RequestProcessor>>,
    valid_tokens: Vec<String>,
}

impl RequestProcessor for AuthenticationMiddleware {
    fn process(&mut self, request: &mut HttpRequest) -> Result<Option<HttpResponse>, String> {
        if request.path.starts_with("/api/") {
            if let Some(auth_header) = request.headers.get("Authorization") {
                if !self.is_valid_token(auth_header) {
                    return Ok(Some(HttpResponse::new(401, "Unauthorized")));
                }
            } else {
                return Ok(Some(HttpResponse::new(401, "Authorization required")));
            }
        }

        // 다음 처리기로 전달
        if let Some(ref mut next) = self.next {
            next.process(request)
        } else {
            Ok(None)
        }
    }
}
```

### 3. 체인 구성 예제
```rust
// 미들웨어 체인 생성
let auth = AuthenticationMiddleware::new(vec!["valid_token".to_string()]);
let mut rate_limit = RateLimitingMiddleware::new(10);
let router = RequestRouter::new();

// 체인 연결
rate_limit.set_next(Box::new(router));
let mut auth_middleware = auth;
auth_middleware.set_next(Box::new(rate_limit));

// 웹 서버에 설정
let mut web_server = WebServer::new();
web_server.set_processor_chain(Box::new(auth_middleware));
```

## Rust의 장점

### 1. 메모리 안전성
- 소유권 시스템으로 체인에서 핸들러가 안전하게 관리됩니다
- 댕글링 포인터나 메모리 누수 문제가 없습니다

### 2. 패턴 매칭
```rust
let resolution = match request.issue_type {
    IssueType::Technical => "Resolved with technical solution",
    IssueType::Billing => "Billing issue resolved",
    IssueType::Critical => "Critical issue escalated",
    _ => "General resolution applied",
};
```

### 3. Option과 Result 활용
```rust
// 안전한 체인 순회
if let Some(ref mut next) = self.next_handler {
    next.handle_request(request)
} else {
    Err("No handler available".to_string())
}
```

### 4. 타입 안전한 요청 처리
- 컴파일 타임에 요청과 응답 타입이 검증됩니다
- 잘못된 타입의 요청 전달을 방지합니다

### 5. 효율적인 메모리 관리
```rust
pub struct SupportSystem {
    first_handler: Option<Box<dyn SupportHandler>>,
    processed_requests: Vec<SupportRequest>,
}
```

## 적용 상황

### 1. 지원 티켓 시스템
- 다단계 고객 지원 (Level 1 → Level 2 → Management)
- 문제 유형과 우선순위에 따른 라우팅
- 자동 에스컬레이션 시스템

### 2. HTTP 미들웨어
- 인증, 인가, 로깅, 압축 등의 순차 처리
- 각 미들웨어가 독립적으로 동작
- 조건에 따른 요청 중단 또는 전달

### 3. 결제 처리 시스템
```rust
enum PaymentMethod {
    CreditCard,
    PayPal,
    BankTransfer,
    Cryptocurrency,
}

trait PaymentProcessor {
    fn process_payment(&mut self, payment: &mut Payment) -> Result<(), PaymentError>;
    fn can_process(&self, method: &PaymentMethod) -> bool;
    fn set_next(&mut self, next: Box<dyn PaymentProcessor>);
}
```

### 4. 로그 처리 시스템
- 로그 레벨에 따른 다양한 핸들러 (콘솔, 파일, 네트워크)
- 조건부 필터링 및 포맷팅
- 실시간 로그 분석 및 알림

### 5. 명령어 처리기
```rust
trait CommandHandler {
    fn execute(&mut self, command: &Command) -> Result<CommandResult, CommandError>;
    fn can_handle(&self, command: &Command) -> bool;
}

struct CommandProcessor {
    handlers: Vec<Box<dyn CommandHandler>>,
}
```

### 6. 데이터 검증 체인
- 입력 데이터의 다단계 검증 (형식, 범위, 비즈니스 로직)
- 각 검증기가 독립적으로 동작
- 검증 실패 시 상세한 오류 정보 제공

이 패턴은 Rust의 소유권 모델과 타입 시스템과 잘 어울리며, 안전하고 효율적인 요청 처리 파이프라인을 구축할 수 있게 해줍니다.