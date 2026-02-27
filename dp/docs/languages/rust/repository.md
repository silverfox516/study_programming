# Repository Pattern - Rust Implementation

## 개요

Repository 패턴은 데이터 저장소에 대한 접근을 캡슐화하여 데이터 계층을 추상화합니다. 비즈니스 로직과 데이터 접근 로직을 분리하여 테스트 가능하고 유지보수가 용이한 코드를 작성할 수 있게 해줍니다.

## 구조

```rust
use std::collections::HashMap;

#[derive(Debug, Clone)]
pub struct User {
    pub id: u32,
    pub name: String,
    pub email: String,
}

// Repository trait
pub trait UserRepository {
    fn find_by_id(&self, id: u32) -> Result<Option<User>, String>;
    fn find_by_email(&self, email: &str) -> Result<Option<User>, String>;
    fn find_all(&self) -> Result<Vec<User>, String>;
    fn save(&mut self, user: User) -> Result<u32, String>;
    fn update(&mut self, user: User) -> Result<(), String>;
    fn delete(&mut self, id: u32) -> Result<bool, String>;
}

// In-memory implementation
pub struct InMemoryUserRepository {
    users: HashMap<u32, User>,
    next_id: u32,
}

impl InMemoryUserRepository {
    pub fn new() -> Self {
        Self {
            users: HashMap::new(),
            next_id: 1,
        }
    }

    pub fn with_users(users: Vec<User>) -> Self {
        let mut repo = Self::new();
        for user in users {
            repo.users.insert(user.id, user.clone());
            if user.id >= repo.next_id {
                repo.next_id = user.id + 1;
            }
        }
        repo
    }
}

impl UserRepository for InMemoryUserRepository {
    fn find_by_id(&self, id: u32) -> Result<Option<User>, String> {
        Ok(self.users.get(&id).cloned())
    }

    fn find_by_email(&self, email: &str) -> Result<Option<User>, String> {
        let user = self.users
            .values()
            .find(|user| user.email == email)
            .cloned();
        Ok(user)
    }

    fn find_all(&self) -> Result<Vec<User>, String> {
        Ok(self.users.values().cloned().collect())
    }

    fn save(&mut self, mut user: User) -> Result<u32, String> {
        if user.id == 0 {
            user.id = self.next_id;
            self.next_id += 1;
        }

        let id = user.id;
        self.users.insert(id, user);
        Ok(id)
    }

    fn update(&mut self, user: User) -> Result<(), String> {
        if self.users.contains_key(&user.id) {
            self.users.insert(user.id, user);
            Ok(())
        } else {
            Err(format!("User with id {} not found", user.id))
        }
    }

    fn delete(&mut self, id: u32) -> Result<bool, String> {
        Ok(self.users.remove(&id).is_some())
    }
}

// Service layer using repository
pub struct UserService<R: UserRepository> {
    repository: R,
}

impl<R: UserRepository> UserService<R> {
    pub fn new(repository: R) -> Self {
        Self { repository }
    }

    pub fn create_user(&mut self, name: String, email: String) -> Result<u32, String> {
        // 이메일 중복 체크
        if let Ok(Some(_)) = self.repository.find_by_email(&email) {
            return Err(format!("User with email {} already exists", email));
        }

        let user = User {
            id: 0, // Repository에서 자동 할당
            name,
            email,
        };

        self.repository.save(user)
    }

    pub fn get_user(&self, id: u32) -> Result<User, String> {
        match self.repository.find_by_id(id)? {
            Some(user) => Ok(user),
            None => Err(format!("User with id {} not found", id)),
        }
    }

    pub fn update_user_email(&mut self, id: u32, new_email: String) -> Result<(), String> {
        let mut user = self.get_user(id)?;

        // 새 이메일이 다른 사용자에 의해 사용되고 있는지 확인
        if let Ok(Some(existing_user)) = self.repository.find_by_email(&new_email) {
            if existing_user.id != id {
                return Err(format!("Email {} is already taken", new_email));
            }
        }

        user.email = new_email;
        self.repository.update(user)
    }

    pub fn delete_user(&mut self, id: u32) -> Result<(), String> {
        if self.repository.delete(id)? {
            Ok(())
        } else {
            Err(format!("User with id {} not found", id))
        }
    }

    pub fn list_all_users(&self) -> Result<Vec<User>, String> {
        self.repository.find_all()
    }
}

// Database implementation example (mock)
pub struct DatabaseUserRepository {
    // 실제로는 데이터베이스 연결을 가질 것
    connection_string: String,
}

impl DatabaseUserRepository {
    pub fn new(connection_string: String) -> Self {
        Self { connection_string }
    }
}

impl UserRepository for DatabaseUserRepository {
    fn find_by_id(&self, id: u32) -> Result<Option<User>, String> {
        // 실제 구현에서는 SQL 쿼리 실행
        println!("Executing: SELECT * FROM users WHERE id = {}", id);
        // Mock implementation
        Ok(None)
    }

    fn find_by_email(&self, email: &str) -> Result<Option<User>, String> {
        println!("Executing: SELECT * FROM users WHERE email = '{}'", email);
        Ok(None)
    }

    fn find_all(&self) -> Result<Vec<User>, String> {
        println!("Executing: SELECT * FROM users");
        Ok(vec![])
    }

    fn save(&mut self, user: User) -> Result<u32, String> {
        println!("Executing: INSERT INTO users (name, email) VALUES ('{}', '{}')",
                user.name, user.email);
        Ok(1) // Mock ID
    }

    fn update(&mut self, user: User) -> Result<(), String> {
        println!("Executing: UPDATE users SET name = '{}', email = '{}' WHERE id = {}",
                user.name, user.email, user.id);
        Ok(())
    }

    fn delete(&mut self, id: u32) -> Result<bool, String> {
        println!("Executing: DELETE FROM users WHERE id = {}", id);
        Ok(true)
    }
}
```

## 사용 예제

```rust
// In-memory repository 사용
let repository = InMemoryUserRepository::new();
let mut user_service = UserService::new(repository);

// 사용자 생성
let user_id = user_service.create_user(
    "Alice".to_string(),
    "alice@example.com".to_string()
).unwrap();

// 사용자 조회
let user = user_service.get_user(user_id).unwrap();
println!("User: {:?}", user);

// 모든 사용자 나열
let all_users = user_service.list_all_users().unwrap();
println!("All users: {:?}", all_users);

// Database repository로 교체
let db_repository = DatabaseUserRepository::new("postgresql://localhost/mydb".to_string());
let mut db_service = UserService::new(db_repository);
```

## 테스트 예제

```rust
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_create_and_find_user() {
        let repository = InMemoryUserRepository::new();
        let mut service = UserService::new(repository);

        let id = service.create_user("John".to_string(), "john@example.com".to_string()).unwrap();
        let user = service.get_user(id).unwrap();

        assert_eq!(user.name, "John");
        assert_eq!(user.email, "john@example.com");
    }

    #[test]
    fn test_duplicate_email_error() {
        let repository = InMemoryUserRepository::new();
        let mut service = UserService::new(repository);

        service.create_user("John".to_string(), "john@example.com".to_string()).unwrap();

        let result = service.create_user("Jane".to_string(), "john@example.com".to_string());
        assert!(result.is_err());
    }
}
```

## 적용 상황

### 1. 웹 애플리케이션의 데이터 계층
### 2. 마이크로서비스의 데이터 접근
### 3. 테스트를 위한 Mock 데이터
### 4. 여러 데이터 소스 추상화
### 5. 캐싱 레이어 구현