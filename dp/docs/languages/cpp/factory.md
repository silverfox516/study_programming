# Factory Pattern - C++ Implementation

## 개요

Factory 패턴은 객체 생성 로직을 별도의 클래스로 분리하여 객체 생성을 캡슐화하는 생성 패턴입니다. C++에서는 스마트 포인터, 람다 함수, 템플릿, std::function 등의 모던 C++ 기능을 활용하여 유연하고 타입 안전한 팩토리를 구현할 수 있습니다.

이 프로젝트에서는 세 가지 다른 Factory 패턴 구현 방식을 제공합니다:
- **factory.cpp**: 기본적인 Factory 패턴과 Simple Factory 패턴
- **factory_impl.cpp**: 네임스페이스와 예외 처리를 포함한 개선된 구현
- **factory_improved.cpp**: 최신 C++ 기능을 활용한 고급 구현
- **factory.hpp**: 모던 C++ 스타일의 헤더 파일 구조

## 구조

Factory 패턴의 주요 구성 요소:
- **Abstract Product**: 생성될 객체의 인터페이스 (Vehicle)
- **Concrete Products**: 실제 구현체들 (Car, Motorcycle, Truck)
- **Creator/Factory**: 객체 생성을 담당하는 팩토리 클래스
- **Client**: 팩토리를 사용하는 클라이언트 코드

## C++ 구현

### 1. 기본 Factory 패턴 (factory.cpp)

#### 추상 제품 클래스
```cpp
class Vehicle {
public:
    virtual ~Vehicle() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual std::string getType() const = 0;
};
```

#### 구체적인 제품들
```cpp
class Car : public Vehicle {
private:
    std::string model;
public:
    explicit Car(const std::string& model) : model(model) {}

    void start() override {
        std::cout << "Car " << model << " engine started with key ignition" << std::endl;
    }

    void stop() override {
        std::cout << "Car " << model << " engine stopped" << std::endl;
    }

    std::string getType() const override {
        return "Car (" + model + ")";
    }
};
```

#### 모던 C++ Factory (람다와 std::function 활용)
```cpp
class VehicleFactory {
private:
    using Creator = std::function<std::unique_ptr<Vehicle>(const std::string&)>;
    std::unordered_map<std::string, Creator> creators;

public:
    VehicleFactory() {
        // 람다를 사용한 생성자 등록
        creators["car"] = [](const std::string& param) -> std::unique_ptr<Vehicle> {
            return std::make_unique<Car>(param.empty() ? "Generic Car" : param);
        };

        creators["motorcycle"] = [](const std::string& param) -> std::unique_ptr<Vehicle> {
            return std::make_unique<Motorcycle>(param.empty() ? "Generic Bike" : param);
        };

        creators["truck"] = [](const std::string& param) -> std::unique_ptr<Vehicle> {
            int capacity = param.empty() ? 10 : std::stoi(param);
            return std::make_unique<Truck>(capacity);
        };
    }

    std::unique_ptr<Vehicle> createVehicle(const std::string& type, const std::string& param = "") {
        auto it = creators.find(type);
        if (it != creators.end()) {
            return it->second(param);
        }
        throw std::invalid_argument("Unknown vehicle type: " + type);
    }

    void registerCreator(const std::string& type, Creator creator) {
        creators[type] = std::move(creator);
    }
};
```

### 2. Simple Factory 패턴
```cpp
class SimpleVehicleFactory {
public:
    enum class VehicleType { CAR, MOTORCYCLE, TRUCK };

    static std::unique_ptr<Vehicle> createVehicle(VehicleType type, const std::string& param = "") {
        switch (type) {
            case VehicleType::CAR:
                return std::make_unique<Car>(param.empty() ? "Default Car" : param);
            case VehicleType::MOTORCYCLE:
                return std::make_unique<Motorcycle>(param.empty() ? "Default Bike" : param);
            case VehicleType::TRUCK:
                return std::make_unique<Truck>(param.empty() ? 15 : std::stoi(param));
            default:
                throw std::invalid_argument("Unknown vehicle type");
        }
    }
};
```

### 3. 개선된 구현 (factory_impl.cpp)

#### 헤더 파일 구조 (factory.hpp)
```cpp
#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <vector>
#include <optional>

namespace DesignPatterns {

// Custom exception for factory errors
class FactoryException : public std::exception {
private:
    std::string message_;

public:
    explicit FactoryException(std::string_view message) : message_(message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

// Abstract Product
class Vehicle {
public:
    virtual ~Vehicle() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual std::string getType() const = 0;

    // Non-copyable but movable
    Vehicle(const Vehicle&) = delete;
    Vehicle& operator=(const Vehicle&) = delete;
    Vehicle(Vehicle&&) = default;
    Vehicle& operator=(Vehicle&&) = default;

protected:
    Vehicle() = default;
};

class VehicleFactory {
public:
    using Creator = std::function<std::unique_ptr<Vehicle>(std::string_view)>;

private:
    std::unordered_map<std::string, Creator> creators_;

public:
    VehicleFactory();

    // Non-copyable but movable
    VehicleFactory(const VehicleFactory&) = delete;
    VehicleFactory& operator=(const VehicleFactory&) = delete;
    VehicleFactory(VehicleFactory&&) = default;
    VehicleFactory& operator=(VehicleFactory&&) = default;

    std::unique_ptr<Vehicle> createVehicle(std::string_view type,
                                         std::string_view param = "");

    void registerCreator(std::string_view type, Creator creator);
    std::vector<std::string> getAvailableTypes() const;
    bool hasType(std::string_view type) const;
};

} // namespace DesignPatterns
```

#### 구현 파일에서의 예외 처리 (factory_impl.cpp)
```cpp
std::unique_ptr<Vehicle> VehicleFactory::createVehicle(std::string_view type,
                                                      std::string_view param) {
    std::string typeStr(type);
    std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

    auto it = creators_.find(typeStr);
    if (it != creators_.end()) {
        try {
            return it->second(param);
        } catch (const FactoryException&) {
            throw; // Re-throw factory exceptions
        } catch (const std::exception& e) {
            throw FactoryException("Failed to create vehicle of type '" +
                                 std::string(type) + "': " + e.what());
        }
    }
    throw FactoryException("Unknown vehicle type: " + std::string(type));
}
```

### 4. 런타임 확장 기능
```cpp
// 런타임에 새로운 차량 타입 등록
factory.registerCreator("electric_car", [](const std::string& param) -> std::unique_ptr<Vehicle> {
    class ElectricCar : public Vehicle {
        std::string model;
    public:
        explicit ElectricCar(const std::string& m) : model(m) {}
        void start() override {
            std::cout << "Electric car " << model << " started silently" << std::endl;
        }
        void stop() override {
            std::cout << "Electric car " << model << " stopped" << std::endl;
        }
        std::string getType() const override {
            return "Electric Car (" + model + ")";
        }
    };
    return std::make_unique<ElectricCar>(param.empty() ? "Tesla Model 3" : param);
});
```

## 사용 예제

### 기본 사용법
```cpp
VehicleFactory factory;

// 다양한 차량 생성
auto car = factory.createVehicle("car", "Toyota Camry");
auto motorcycle = factory.createVehicle("motorcycle", "Harley Davidson");
auto truck = factory.createVehicle("truck", "25");

// 차량 동작 테스트
car->start();
car->stop();
```

### Simple Factory 사용법
```cpp
auto simpleCar = SimpleVehicleFactory::createVehicle(
    SimpleVehicleFactory::VehicleType::CAR, "BMW X5");
simpleCar->start();
```

### 예외 처리
```cpp
try {
    auto invalid = factory.createVehicle("airplane", "Boeing 737");
} catch (const FactoryException& e) {
    std::cout << "Caught expected exception: " << e.what() << std::endl;
}
```

## C++의 장점

### 1. 메모리 안전성
- **스마트 포인터 활용**: `std::unique_ptr`로 자동 메모리 관리
- **RAII 패턴**: 리소스 자동 해제

### 2. 타입 안전성
- **강력한 타입 시스템**: 컴파일 타임 타입 체크
- **enum class**: 타입 안전한 열거형

### 3. 성능 최적화
- **이동 의미론**: `std::move`로 불필요한 복사 방지
- **인라인 최적화**: 컴파일러 최적화 지원

### 4. 모던 C++ 기능 활용
- **람다 함수**: 생성자 로직을 간결하게 표현
- **std::function**: 함수 객체의 유연한 저장
- **auto 키워드**: 타입 추론으로 코드 간소화
- **범위 기반 for 루프**: 컨테이너 순회 간소화

### 5. 함수형 프로그래밍 지원
```cpp
// 람다를 사용한 생성자 등록
creators["car"] = [](const std::string& param) -> std::unique_ptr<Vehicle> {
    return std::make_unique<Car>(param.empty() ? "Generic Car" : param);
};
```

### 6. 예외 안전성
```cpp
try {
    return it->second(param);
} catch (const FactoryException&) {
    throw; // Re-throw factory exceptions
} catch (const std::exception& e) {
    throw FactoryException("Failed to create vehicle: " + std::string(e.what()));
}
```

## 적용 상황

### 1. 객체 생성이 복잡한 경우
- 여러 매개변수가 필요한 객체
- 초기화 로직이 복잡한 객체

### 2. 런타임에 생성할 객체 타입이 결정되는 경우
- 사용자 입력에 따른 객체 생성
- 설정 파일 기반 객체 생성

### 3. 객체 생성 로직을 중앙화하고 싶은 경우
- 일관된 객체 생성 규칙 적용
- 생성 로직의 재사용

### 4. 확장 가능한 시스템 구축
- 플러그인 시스템
- 모듈러 아키텍처

### 5. 단위 테스트 지원
- 목 객체 생성
- 테스트용 구현체 주입

## Factory vs Simple Factory vs Abstract Factory

### Simple Factory
- 단순한 if-else나 switch 문 사용
- 확장성 제한적
- 구현이 간단

### Factory Method
- 생성 메서드를 서브클래스에서 오버라이드
- 확장성 좋음
- 상속 기반

### Abstract Factory
- 관련된 객체 패밀리 생성
- 제품군 전체를 교체 가능
- 더 복잡한 구조

이 프로젝트의 구현은 주로 Factory Method 패턴과 Simple Factory 패턴을 다루며, 모던 C++의 함수 객체를 활용해 유연성과 확장성을 동시에 확보했습니다.