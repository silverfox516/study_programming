# MVC Pattern - C++ Implementation

## 개요

MVC(Model-View-Controller) 패턴은 애플리케이션을 세 개의 상호 연결된 구성 요소로 분리하여 내부 표현을 사용자에게 정보가 제시되는 방식과 분리하는 아키텍처 패턴입니다.

## 구조

- **Model**: 데이터와 비즈니스 로직을 담당
- **View**: 사용자 인터페이스 담당
- **Controller**: 사용자 입력을 처리하고 Model과 View를 조율

## C++ 구현

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

// Observer pattern for MVC communication
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update() = 0;
};

class Subject {
private:
    std::vector<Observer*> observers;

public:
    void addObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void removeObserver(Observer* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void notifyObservers() {
        for (auto observer : observers) {
            observer->update();
        }
    }
};

// Model
class UserModel : public Subject {
private:
    std::string name;
    std::string email;
    int age;

public:
    UserModel(const std::string& name = "", const std::string& email = "", int age = 0)
        : name(name), email(email), age(age) {}

    // Getters
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    int getAge() const { return age; }

    // Setters with notification
    void setName(const std::string& newName) {
        if (name != newName) {
            name = newName;
            notifyObservers();
        }
    }

    void setEmail(const std::string& newEmail) {
        if (email != newEmail) {
            email = newEmail;
            notifyObservers();
        }
    }

    void setAge(int newAge) {
        if (age != newAge && newAge >= 0) {
            age = newAge;
            notifyObservers();
        }
    }

    // Business logic
    bool isValid() const {
        return !name.empty() && !email.empty() && age > 0 &&
               email.find('@') != std::string::npos;
    }
};

// View
class UserView : public Observer {
private:
    UserModel* model;

public:
    explicit UserView(UserModel* model) : model(model) {
        model->addObserver(this);
    }

    ~UserView() {
        if (model) {
            model->removeObserver(this);
        }
    }

    void update() override {
        display();
    }

    void display() {
        std::cout << "\n=== User Information ===" << std::endl;
        std::cout << "Name: " << model->getName() << std::endl;
        std::cout << "Email: " << model->getEmail() << std::endl;
        std::cout << "Age: " << model->getAge() << std::endl;
        std::cout << "Valid: " << (model->isValid() ? "Yes" : "No") << std::endl;
        std::cout << "========================\n" << std::endl;
    }
};

// Another view for different presentation
class UserSummaryView : public Observer {
private:
    UserModel* model;

public:
    explicit UserSummaryView(UserModel* model) : model(model) {
        model->addObserver(this);
    }

    ~UserSummaryView() {
        if (model) {
            model->removeObserver(this);
        }
    }

    void update() override {
        displaySummary();
    }

    void displaySummary() {
        std::cout << "[SUMMARY] " << model->getName()
                  << " (" << model->getAge() << ")"
                  << " - " << model->getEmail() << std::endl;
    }
};

// Controller
class UserController {
private:
    UserModel* model;

public:
    explicit UserController(UserModel* model) : model(model) {}

    void updateUserName(const std::string& name) {
        std::cout << "Controller: Updating user name to '" << name << "'" << std::endl;
        model->setName(name);
    }

    void updateUserEmail(const std::string& email) {
        std::cout << "Controller: Updating user email to '" << email << "'" << std::endl;
        model->setEmail(email);
    }

    void updateUserAge(int age) {
        std::cout << "Controller: Updating user age to " << age << std::endl;
        model->setAge(age);
    }

    // Complex operations
    void updateUser(const std::string& name, const std::string& email, int age) {
        std::cout << "Controller: Updating complete user profile" << std::endl;
        model->setName(name);
        model->setEmail(email);
        model->setAge(age);
    }

    bool validateAndSave() {
        if (model->isValid()) {
            std::cout << "Controller: User data is valid, saving..." << std::endl;
            return true;
        } else {
            std::cout << "Controller: User data is invalid, cannot save" << std::endl;
            return false;
        }
    }
};

// Simple application class to tie everything together
class UserApplication {
private:
    std::unique_ptr<UserModel> model;
    std::unique_ptr<UserView> view;
    std::unique_ptr<UserSummaryView> summaryView;
    std::unique_ptr<UserController> controller;

public:
    UserApplication() {
        model = std::make_unique<UserModel>();
        view = std::make_unique<UserView>(model.get());
        summaryView = std::make_unique<UserSummaryView>(model.get());
        controller = std::make_unique<UserController>(model.get());
    }

    void run() {
        std::cout << "=== MVC Pattern Demo ===" << std::endl;

        // Initial display
        view->display();

        // Simulate user interactions through controller
        controller->updateUserName("John Doe");
        controller->updateUserEmail("john@example.com");
        controller->updateUserAge(30);

        controller->validateAndSave();

        // Update individual fields
        controller->updateUserEmail("john.doe@newcompany.com");

        // Bulk update
        controller->updateUser("Jane Smith", "jane@example.com", 25);

        controller->validateAndSave();
    }
};
```

## 사용 예제

```cpp
int main() {
    try {
        UserApplication app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

## C++의 장점

1. **관찰자 패턴 통합**: 자동 뷰 업데이트
2. **스마트 포인터**: 안전한 메모리 관리
3. **캡슐화**: 각 구성 요소의 책임 분리
4. **다형성**: 여러 뷰 타입 지원

## MVC의 이점

1. **관심사 분리**: 각 구성 요소의 명확한 역할
2. **재사용성**: Model과 View의 독립적 재사용
3. **테스트 용이성**: 각 구성 요소 개별 테스트 가능
4. **확장성**: 새로운 View나 Controller 쉽게 추가

## 적용 상황

1. **데스크톱 애플리케이션**: GUI 애플리케이션 구조
2. **웹 애플리케이션**: 웹 서비스 아키텍처
3. **게임**: UI와 게임 로직 분리
4. **모바일 앱**: 플랫폼별 UI 분리

MVC 패턴은 복잡한 애플리케이션의 구조를 체계화하고 유지보수성을 크게 향상시키는 핵심 아키텍처 패턴입니다.