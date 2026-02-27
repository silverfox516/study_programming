# Bridge Pattern - Rust Implementation

## 개요

Bridge 패턴은 추상화(Abstraction)와 구현(Implementation)을 분리하여 독립적으로 변경할 수 있게 하는 구조적 디자인 패턴입니다. Rust에서는 트레이트를 통한 추상화와 구조체 조합을 활용하여 유연하고 확장 가능한 아키텍처를 구현할 수 있습니다.

이 패턴은 플랫폼별 구현체, 다양한 렌더링 엔진, 통신 프로토콜 등에서 추상화 계층과 구체적 구현을 분리할 때 특히 유용합니다.

## 구조

### 핵심 구성 요소

1. **Implementation Interface** (`DrawingAPI`, `NotificationSender`)
   - 구현체들이 공통으로 제공해야 하는 인터페이스
   - 플랫폼이나 기술별 세부사항을 추상화

2. **Concrete Implementations** (`CanvasAPI`, `SvgAPI`, `OpenGLAPI`)
   - 각기 다른 플랫폼이나 기술의 구체적 구현
   - 동일한 인터페이스로 다른 동작 수행

3. **Abstraction** (`Shape`, `Notification`)
   - 클라이언트가 사용하는 추상화된 인터페이스
   - Implementation 객체에 대한 참조를 보유

4. **Refined Abstraction** (`Circle`, `Rectangle`, `UrgentNotification`)
   - 추상화를 확장하거나 특수화한 클래스
   - 추가적인 기능이나 특수한 동작 제공

## Rust 구현

### 트레이트 기반 구현 인터페이스

```rust
pub trait DrawingAPI {
    fn draw_circle(&self, x: f64, y: f64, radius: f64) -> String;
    fn draw_rectangle(&self, x: f64, y: f64, width: f64, height: f64) -> String;
    fn draw_line(&self, x1: f64, y1: f64, x2: f64, y2: f64) -> String;
    fn get_api_name(&self) -> &str;
}

// 플랫폼별 구체 구현
pub struct CanvasAPI { /* ... */ }
pub struct SvgAPI { /* ... */ }
pub struct OpenGLAPI { /* ... */ }
```

### 추상화와 구체 구현의 분리

```rust
pub trait Shape {
    fn draw(&self) -> Vec<String>;
    fn resize(&mut self, factor: f64);
    fn move_to(&mut self, x: f64, y: f64);
    fn get_area(&self) -> f64;
}

pub struct Circle {
    x: f64, y: f64, radius: f64,
    drawing_api: Box<dyn DrawingAPI>,  // 구현체에 대한 참조
}
```

### 소유권 시스템 활용

1. **Box<dyn Trait>**: 힙에 할당된 트레이트 객체로 다형성 구현
2. **트레이트 객체의 동적 디스패치**: 런타임에 적절한 구현 선택
3. **소유권 기반 리소스 관리**: 자동 메모리 해제와 안전한 참조

### Rust 특수 기능 활용

1. **포맷 매크로와 정밀도 제어**
   ```rust
   fn draw_circle(&self, x: f64, y: f64, radius: f64) -> String {
       format!(
           "<circle cx=\"{:.prec$}\" cy=\"{:.prec$}\" r=\"{:.prec$}\" />",
           x, y, radius, prec = self.precision as usize
       )
   }
   ```

2. **열거형과 패턴 매칭**
   ```rust
   #[derive(Debug, Clone)]
   pub enum NotificationPriority {
       Low, Normal, High, Critical,
   }

   let priority_prefix = match self.priority {
       NotificationPriority::Critical => "[CRITICAL]",
       NotificationPriority::High => "[HIGH]",
       // ...
   };
   ```

3. **Result 타입을 통한 에러 처리**
   ```rust
   pub fn send(&self, message: &str, recipient: &str) -> Result<String, String> {
       if !recipient.contains('@') {
           return Err("Invalid email address".to_string());
       }
       Ok(format!("Email sent to {}: {}", recipient, message))
   }
   ```

### 메모리 안전성과 동시성

- **Borrow Checker**: 댕글링 포인터나 데이터 레이스 방지
- **Send + Sync 트레이트**: 스레드 간 안전한 데이터 공유
- **Arc<Mutex<T>>**: 멀티스레드 환경에서 안전한 공유 가능

## 사용 예제

### 1. 다양한 드로잉 API 사용
```rust
// 동일한 Shape 인터페이스로 다른 렌더링 엔진 사용
let canvas_circle = Circle::new(10.0, 20.0, 5.0, Box::new(CanvasAPI::new(800, 600)));
let svg_circle = Circle::new(10.0, 20.0, 5.0, Box::new(SvgAPI::new(2)));
let opengl_circle = Circle::new(10.0, 20.0, 5.0, Box::new(OpenGLAPI::new("4.0")));

// 모두 동일한 방식으로 호출
let canvas_output = canvas_circle.draw();
let svg_output = svg_circle.draw();
let opengl_output = opengl_circle.draw();
```

### 2. 통지 시스템 브릿지
```rust
let email_notification = Notification::new(
    Box::new(EmailSender::new("smtp.company.com")),
    NotificationPriority::High
);

let sms_notification = Notification::new(
    Box::new(SmsSender::new("Twilio")),
    NotificationPriority::Critical
);

// 동일한 인터페이스로 다른 전송 방식 사용
email_notification.send("Server maintenance", "admin@company.com")?;
sms_notification.send("Critical alert", "1234567890")?;
```

### 3. 복합 도형 처리
```rust
let mut house = ComplexShape::new(200.0, 200.0, Box::new(SvgAPI::new(1)));
let house_commands = house.draw_house();

// API 변경 없이 렌더링 엔진만 교체
let mut house_canvas = ComplexShape::new(200.0, 200.0, Box::new(CanvasAPI::new(800, 600)));
let canvas_commands = house_canvas.draw_house();
```

## Rust의 장점

### 1. 제로코스트 추상화
- 트레이트 객체를 통한 런타임 다형성
- 정적 디스패치 선택 가능으로 성능 최적화
- 컴파일 타임 최적화

### 2. 타입 안전성
- 트레이트 바운드로 컴파일 타임 인터페이스 검증
- 잘못된 타입 조합을 컴파일 단계에서 방지
- 런타임 에러 최소화

### 3. 메모리 안전성
- 소유권 시스템으로 메모리 누수 방지
- 댕글링 포인터 불가능
- RAII 패턴 자동 적용

### 4. 동시성 지원
- Send/Sync 트레이트를 통한 스레드 안전성 보장
- Arc/Rc를 통한 안전한 공유 참조
- 데이터 레이스 컴파일 타임 방지

### 5. 패턴 매칭의 표현력
- 열거형과 match를 통한 명확한 상태 처리
- 완전성 검사(exhaustiveness check)
- 코드의 가독성과 안전성 향상

### 6. 에러 처리의 명시성
- Result<T, E>를 통한 명시적 에러 처리
- ? 연산자를 통한 에러 전파
- 컴파일러 레벨의 에러 처리 강제

## 적용 상황

### 1. 크로스 플랫폼 렌더링
- 다양한 그래픽 API (OpenGL, DirectX, Vulkan)
- 웹 브라우저 렌더링 엔진
- 게임 엔진의 렌더링 백엔드

### 2. 통신 프로토콜 추상화
- 다양한 메시징 시스템 (Email, SMS, Push notification)
- 네트워크 프로토콜 (HTTP, WebSocket, TCP)
- 데이터베이스 드라이버 (MySQL, PostgreSQL, MongoDB)

### 3. UI 프레임워크
- 플랫폼별 위젯 구현 (Windows, macOS, Linux)
- 테마나 스타일 시스템
- 반응형 디자인 시스템

### 4. 파일 시스템 추상화
- 로컬/클라우드 스토리지
- 압축/암호화 계층
- 백업/동기화 시스템

### 5. 테스트 환경
- Mock 객체와 실제 구현의 전환
- 다양한 테스트 환경 지원
- 의존성 주입과 격리

이 구현은 Rust의 강력한 타입 시스템과 트레이트를 활용하여 유연하고 안전한 Bridge 패턴을 보여줍니다. 특히 컴파일 타임 안전성과 런타임 성능의 균형을 잘 보여주는 패턴입니다.