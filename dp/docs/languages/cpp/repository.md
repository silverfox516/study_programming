# Repository Pattern - C++ Implementation

## 개요

Repository 패턴은 데이터 액세스 로직을 캡슐화하고 도메인 모델과 데이터 매핑 계층 사이의 느슨한 결합을 제공하는 아키텍처 패턴입니다. 데이터 저장소에 대한 통일된 인터페이스를 제공하여 테스트 가능하고 유지보수하기 쉬운 코드를 만듭니다.

## 구조

- **Entity**: 도메인 객체
- **Repository Interface**: 데이터 액세스 추상화
- **Concrete Repository**: 실제 데이터 액세스 구현
- **Unit of Work**: 트랜잭션 관리 (선택적)

## C++ 구현

```cpp
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <optional>

// Entity
class User {
private:
    int id;
    std::string name;
    std::string email;
    int age;

public:
    User() : id(0), age(0) {}

    User(int id, const std::string& name, const std::string& email, int age)
        : id(id), name(name), email(email), age(age) {}

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    int getAge() const { return age; }

    // Setters
    void setId(int newId) { id = newId; }
    void setName(const std::string& newName) { name = newName; }
    void setEmail(const std::string& newEmail) { email = newEmail; }
    void setAge(int newAge) { age = newAge; }

    std::string toString() const {
        return "User{id=" + std::to_string(id) +
               ", name='" + name +
               "', email='" + email +
               "', age=" + std::to_string(age) + "}";
    }
};

// Repository Interface
class UserRepository {
public:
    virtual ~UserRepository() = default;
    virtual std::optional<User> findById(int id) const = 0;
    virtual std::vector<User> findAll() const = 0;
    virtual std::vector<User> findByName(const std::string& name) const = 0;
    virtual std::vector<User> findByAgeRange(int minAge, int maxAge) const = 0;
    virtual void save(const User& user) = 0;
    virtual void update(const User& user) = 0;
    virtual bool deleteById(int id) = 0;
    virtual size_t count() const = 0;
};

// In-Memory Repository Implementation
class InMemoryUserRepository : public UserRepository {
private:
    std::unordered_map<int, User> users;
    int nextId;

public:
    InMemoryUserRepository() : nextId(1) {}

    std::optional<User> findById(int id) const override {
        auto it = users.find(id);
        if (it != users.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<User> findAll() const override {
        std::vector<User> result;
        for (const auto& [id, user] : users) {
            result.push_back(user);
        }
        return result;
    }

    std::vector<User> findByName(const std::string& name) const override {
        std::vector<User> result;
        for (const auto& [id, user] : users) {
            if (user.getName() == name) {
                result.push_back(user);
            }
        }
        return result;
    }

    std::vector<User> findByAgeRange(int minAge, int maxAge) const override {
        std::vector<User> result;
        for (const auto& [id, user] : users) {
            if (user.getAge() >= minAge && user.getAge() <= maxAge) {
                result.push_back(user);
            }
        }
        return result;
    }

    void save(const User& user) override {
        User newUser = user;
        if (newUser.getId() == 0) {
            newUser.setId(nextId++);
        }
        users[newUser.getId()] = newUser;
        std::cout << "Saved user: " << newUser.toString() << std::endl;
    }

    void update(const User& user) override {
        if (users.find(user.getId()) != users.end()) {
            users[user.getId()] = user;
            std::cout << "Updated user: " << user.toString() << std::endl;
        } else {
            std::cout << "User not found for update: " << user.getId() << std::endl;
        }
    }

    bool deleteById(int id) override {
        auto it = users.find(id);
        if (it != users.end()) {
            std::cout << "Deleted user: " << it->second.toString() << std::endl;
            users.erase(it);
            return true;
        }
        std::cout << "User not found for deletion: " << id << std::endl;
        return false;
    }

    size_t count() const override {
        return users.size();
    }
};

// Database Repository Implementation (mock)
class DatabaseUserRepository : public UserRepository {
private:
    std::string connectionString;

public:
    explicit DatabaseUserRepository(const std::string& connectionString)
        : connectionString(connectionString) {
        std::cout << "Connected to database: " << connectionString << std::endl;
    }

    std::optional<User> findById(int id) const override {
        std::cout << "DB Query: SELECT * FROM users WHERE id = " << id << std::endl;
        // Mock database query result
        if (id == 1) {
            return User(1, "Database User", "db@example.com", 30);
        }
        return std::nullopt;
    }

    std::vector<User> findAll() const override {
        std::cout << "DB Query: SELECT * FROM users" << std::endl;
        return {
            User(1, "Database User 1", "db1@example.com", 25),
            User(2, "Database User 2", "db2@example.com", 35)
        };
    }

    std::vector<User> findByName(const std::string& name) const override {
        std::cout << "DB Query: SELECT * FROM users WHERE name = '" << name << "'" << std::endl;
        return {};
    }

    std::vector<User> findByAgeRange(int minAge, int maxAge) const override {
        std::cout << "DB Query: SELECT * FROM users WHERE age BETWEEN " << minAge << " AND " << maxAge << std::endl;
        return {};
    }

    void save(const User& user) override {
        std::cout << "DB Query: INSERT INTO users (name, email, age) VALUES ('"
                  << user.getName() << "', '" << user.getEmail() << "', " << user.getAge() << ")" << std::endl;
    }

    void update(const User& user) override {
        std::cout << "DB Query: UPDATE users SET name='" << user.getName()
                  << "', email='" << user.getEmail()
                  << "', age=" << user.getAge()
                  << " WHERE id=" << user.getId() << std::endl;
    }

    bool deleteById(int id) override {
        std::cout << "DB Query: DELETE FROM users WHERE id = " << id << std::endl;
        return true;
    }

    size_t count() const override {
        std::cout << "DB Query: SELECT COUNT(*) FROM users" << std::endl;
        return 2; // Mock count
    }
};

// Service Layer using Repository
class UserService {
private:
    std::unique_ptr<UserRepository> repository;

public:
    explicit UserService(std::unique_ptr<UserRepository> repository)
        : repository(std::move(repository)) {}

    void setRepository(std::unique_ptr<UserRepository> newRepository) {
        repository = std::move(newRepository);
    }

    std::string createUser(const std::string& name, const std::string& email, int age) {
        if (name.empty() || email.empty() || age < 0) {
            return "Invalid user data";
        }

        User user(0, name, email, age); // ID will be assigned by repository
        repository->save(user);
        return "User created successfully";
    }

    std::optional<User> getUserById(int id) {
        return repository->findById(id);
    }

    std::vector<User> getAllUsers() {
        return repository->findAll();
    }

    std::vector<User> getUsersByName(const std::string& name) {
        return repository->findByName(name);
    }

    std::vector<User> getYoungUsers() {
        return repository->findByAgeRange(18, 30);
    }

    std::string updateUser(int id, const std::string& name, const std::string& email, int age) {
        auto user = repository->findById(id);
        if (user) {
            user->setName(name);
            user->setEmail(email);
            user->setAge(age);
            repository->update(*user);
            return "User updated successfully";
        }
        return "User not found";
    }

    std::string deleteUser(int id) {
        if (repository->deleteById(id)) {
            return "User deleted successfully";
        }
        return "User not found";
    }

    size_t getUserCount() {
        return repository->count();
    }
};

// Repository Factory
class RepositoryFactory {
public:
    enum class RepositoryType {
        IN_MEMORY,
        DATABASE
    };

    static std::unique_ptr<UserRepository> createUserRepository(RepositoryType type, const std::string& config = "") {
        switch (type) {
            case RepositoryType::IN_MEMORY:
                return std::make_unique<InMemoryUserRepository>();
            case RepositoryType::DATABASE:
                return std::make_unique<DatabaseUserRepository>(
                    config.empty() ? "default_connection" : config
                );
            default:
                return std::make_unique<InMemoryUserRepository>();
        }
    }
};
```

## 사용 예제

```cpp
int main() {
    std::cout << "=== Repository Pattern Demo ===" << std::endl;

    // 1. Using In-Memory Repository
    std::cout << "\n1. Using In-Memory Repository:" << std::endl;

    auto inMemoryRepo = RepositoryFactory::createUserRepository(
        RepositoryFactory::RepositoryType::IN_MEMORY
    );
    UserService userService(std::move(inMemoryRepo));

    // Create users
    userService.createUser("Alice Johnson", "alice@example.com", 28);
    userService.createUser("Bob Smith", "bob@example.com", 32);
    userService.createUser("Charlie Brown", "charlie@example.com", 25);

    std::cout << "\nTotal users: " << userService.getUserCount() << std::endl;

    // Find all users
    std::cout << "\nAll users:" << std::endl;
    auto allUsers = userService.getAllUsers();
    for (const auto& user : allUsers) {
        std::cout << "  " << user.toString() << std::endl;
    }

    // Find young users
    std::cout << "\nYoung users (18-30):" << std::endl;
    auto youngUsers = userService.getYoungUsers();
    for (const auto& user : youngUsers) {
        std::cout << "  " << user.toString() << std::endl;
    }

    // Update user
    std::cout << "\n" << userService.updateUser(1, "Alice Williams", "alice.williams@example.com", 29) << std::endl;

    // Delete user
    std::cout << userService.deleteUser(2) << std::endl;

    // 2. Switch to Database Repository
    std::cout << "\n2. Switching to Database Repository:" << std::endl;

    auto dbRepo = RepositoryFactory::createUserRepository(
        RepositoryFactory::RepositoryType::DATABASE,
        "postgresql://localhost:5432/mydb"
    );
    userService.setRepository(std::move(dbRepo));

    // Perform operations with database repository
    std::cout << "\nFinding user by ID 1:" << std::endl;
    auto user = userService.getUserById(1);
    if (user) {
        std::cout << "Found: " << user->toString() << std::endl;
    }

    std::cout << "\nAll users from database:" << std::endl;
    auto dbUsers = userService.getAllUsers();
    for (const auto& user : dbUsers) {
        std::cout << "  " << user.toString() << std::endl;
    }

    userService.createUser("Database User", "newuser@db.com", 40);

    return 0;
}
```

## C++의 장점

1. **인터페이스 분리**: 순수 가상 함수로 계약 정의
2. **스마트 포인터**: 안전한 메모리 관리
3. **템플릿**: 제네릭 Repository 구현 가능
4. **옵셔널**: null 안전한 조회 결과

## Repository의 이점

1. **테스트 용이성**: Mock Repository로 쉬운 단위 테스트
2. **유연성**: 다른 데이터 저장소로 쉬운 교체
3. **캡슐화**: 데이터 액세스 로직 캡슐화
4. **중앙화**: 쿼리 로직의 중앙 관리

## 적용 상황

1. **데이터 액세스 계층**: ORM 없이 데이터베이스 접근
2. **테스트**: 인메모리 Repository로 빠른 테스트
3. **마이크로서비스**: 서비스별 데이터 액세스 추상화
4. **레거시 시스템**: 기존 데이터베이스와의 통합

Repository 패턴은 데이터 액세스 로직을 깔끔하게 분리하고 테스트 가능한 코드를 작성하는 데 매우 유용한 패턴입니다.