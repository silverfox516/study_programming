# Mediator Pattern - Rust Implementation

## 개요

Mediator 패턴은 객체들 간의 복잡한 통신과 제어 로직을 하나의 중재자 클래스로 집중시켜 객체들 간의 결합도를 낮춥니다. 객체들이 서로 직접 참조하지 않고 중재자를 통해 상호작용합니다.

## 구조

```rust
pub trait ChatMediator {
    fn send_message(&self, message: &str, user: &str);
    fn add_user(&mut self, user: Box<dyn ChatUser>);
}

pub trait ChatUser {
    fn send(&self, message: &str, mediator: &dyn ChatMediator);
    fn receive(&self, message: &str);
    fn get_name(&self) -> &str;
}

pub struct ChatRoom {
    users: HashMap<String, Box<dyn ChatUser>>,
}

impl ChatMediator for ChatRoom {
    fn send_message(&self, message: &str, sender: &str) {
        for (name, user) in &self.users {
            if name != sender {
                user.receive(&format!("{}: {}", sender, message));
            }
        }
    }

    fn add_user(&mut self, user: Box<dyn ChatUser>) {
        let name = user.get_name().to_string();
        self.users.insert(name, user);
    }
}
```

## 적용 상황

### 1. 채팅 시스템
### 2. GUI 대화상자
### 3. 워크플로우 엔진
### 4. 게임 이벤트 시스템
### 5. MVC 컨트롤러