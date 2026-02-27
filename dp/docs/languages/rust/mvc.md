# MVC Pattern - Rust Implementation

## 개요

MVC(Model-View-Controller) 패턴은 애플리케이션을 세 개의 상호연결된 구성요소로 분리하여 비즈니스 로직을 사용자 인터페이스로부터 분리합니다.

## 구조

```rust
use std::collections::HashMap;

// Model - 데이터와 비즈니스 로직
#[derive(Debug, Clone)]
pub struct User {
    id: u32,
    name: String,
    email: String,
}

pub struct UserModel {
    users: HashMap<u32, User>,
    next_id: u32,
}

impl UserModel {
    pub fn new() -> Self {
        Self {
            users: HashMap::new(),
            next_id: 1,
        }
    }

    pub fn create_user(&mut self, name: String, email: String) -> u32 {
        let id = self.next_id;
        let user = User { id, name, email };
        self.users.insert(id, user);
        self.next_id += 1;
        id
    }

    pub fn get_user(&self, id: u32) -> Option<&User> {
        self.users.get(&id)
    }

    pub fn get_all_users(&self) -> Vec<&User> {
        self.users.values().collect()
    }

    pub fn update_user(&mut self, id: u32, name: Option<String>, email: Option<String>) -> bool {
        if let Some(user) = self.users.get_mut(&id) {
            if let Some(new_name) = name {
                user.name = new_name;
            }
            if let Some(new_email) = email {
                user.email = new_email;
            }
            true
        } else {
            false
        }
    }

    pub fn delete_user(&mut self, id: u32) -> bool {
        self.users.remove(&id).is_some()
    }
}

// View - 사용자 인터페이스
pub struct UserView;

impl UserView {
    pub fn display_user(&self, user: &User) -> String {
        format!("User[ID: {}, Name: {}, Email: {}]", user.id, user.name, user.email)
    }

    pub fn display_all_users(&self, users: Vec<&User>) -> String {
        let mut output = String::from("All Users:\n");
        for user in users {
            output.push_str(&format!("  {}\n", self.display_user(user)));
        }
        output
    }

    pub fn display_message(&self, message: &str) -> String {
        format!("Message: {}", message)
    }

    pub fn display_error(&self, error: &str) -> String {
        format!("Error: {}", error)
    }
}

// Controller - Model과 View 사이의 중재자
pub struct UserController {
    model: UserModel,
    view: UserView,
}

impl UserController {
    pub fn new() -> Self {
        Self {
            model: UserModel::new(),
            view: UserView,
        }
    }

    pub fn create_user(&mut self, name: String, email: String) -> String {
        let id = self.model.create_user(name.clone(), email);
        self.view.display_message(&format!("User created with ID: {}", id))
    }

    pub fn show_user(&self, id: u32) -> String {
        match self.model.get_user(id) {
            Some(user) => self.view.display_user(user),
            None => self.view.display_error(&format!("User with ID {} not found", id)),
        }
    }

    pub fn show_all_users(&self) -> String {
        let users = self.model.get_all_users();
        self.view.display_all_users(users)
    }

    pub fn update_user(&mut self, id: u32, name: Option<String>, email: Option<String>) -> String {
        if self.model.update_user(id, name, email) {
            self.view.display_message(&format!("User {} updated successfully", id))
        } else {
            self.view.display_error(&format!("User with ID {} not found", id))
        }
    }

    pub fn delete_user(&mut self, id: u32) -> String {
        if self.model.delete_user(id) {
            self.view.display_message(&format!("User {} deleted successfully", id))
        } else {
            self.view.display_error(&format!("User with ID {} not found", id))
        }
    }
}
```

## 사용 예제

```rust
let mut controller = UserController::new();

// 사용자 생성
println!("{}", controller.create_user("Alice".to_string(), "alice@example.com".to_string()));
println!("{}", controller.create_user("Bob".to_string(), "bob@example.com".to_string()));

// 모든 사용자 조회
println!("{}", controller.show_all_users());

// 특정 사용자 조회
println!("{}", controller.show_user(1));

// 사용자 업데이트
println!("{}", controller.update_user(1, Some("Alice Smith".to_string()), None));

// 사용자 삭제
println!("{}", controller.delete_user(2));
```

## MVC의 장점

### 1. 관심사의 분리
- Model: 데이터와 비즈니스 로직
- View: 사용자 인터페이스
- Controller: 중재자 역할

### 2. 테스트 용이성
- 각 구성요소를 독립적으로 테스트 가능

### 3. 유지보수성
- 각 계층의 변경이 다른 계층에 미치는 영향 최소화

## 적용 상황

### 1. 웹 애플리케이션
### 2. 데스크톱 GUI 애플리케이션
### 3. 모바일 앱
### 4. 게임 UI 시스템
### 5. API 서버