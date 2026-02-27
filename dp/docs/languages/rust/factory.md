# Factory Pattern - Rust Implementation

## 개요

Factory 패턴은 구체적인 클래스를 지정하지 않고 객체를 생성하는 인터페이스를 제공합니다. Rust에서는 trait과 enum을 활용하여 관용적인 팩토리 패턴을 구현할 수 있습니다.

이 패턴은 객체 생성 로직을 캡슐화하고, 생성할 객체의 타입을 런타임에 결정할 수 있게 해줍니다.

## 구조

```rust
pub trait Vehicle {
    fn start(&self) -> String;
    fn stop(&self) -> String;
    fn get_info(&self) -> String;
}

pub enum VehicleType {
    Car,
    Motorcycle,
    Truck,
}

pub struct VehicleFactory;

impl VehicleFactory {
    pub fn create_vehicle(vehicle_type: VehicleType, params: VehicleParams) -> Box<dyn Vehicle> {
        match vehicle_type {
            VehicleType::Car => Box::new(Car::new(params.brand, params.model, params.year)),
            VehicleType::Motorcycle => Box::new(Motorcycle::new(params.brand, params.engine_size)),
            VehicleType::Truck => Box::new(Truck::new(params.brand, params.capacity)),
        }
    }
}
```

## Rust 구현

### 1. 제품 인터페이스 정의
```rust
pub trait Vehicle {
    fn start(&self) -> String;
    fn stop(&self) -> String;
    fn get_info(&self) -> String;
}
```

### 2. 구체적인 제품 구현
```rust
#[derive(Debug, Clone)]
pub struct Car {
    brand: String,
    model: String,
    year: u32,
}

impl Vehicle for Car {
    fn start(&self) -> String {
        format!("Starting {} {} {} engine", self.year, self.brand, self.model)
    }

    fn stop(&self) -> String {
        format!("Stopping {} {} {}", self.year, self.brand, self.model)
    }

    fn get_info(&self) -> String {
        format!("Car: {} {} {}", self.year, self.brand, self.model)
    }
}
```

### 3. 팩토리 구현
```rust
pub struct VehicleFactory;

impl VehicleFactory {
    pub fn create_vehicle(vehicle_type: VehicleType, params: VehicleParams) -> Box<dyn Vehicle> {
        match vehicle_type {
            VehicleType::Car => Box::new(Car::new(params.brand, params.model, params.year)),
            VehicleType::Motorcycle => Box::new(Motorcycle::new(params.brand, params.engine_size)),
            VehicleType::Truck => Box::new(Truck::new(params.brand, params.capacity)),
        }
    }
}
```

## 사용 예제

```rust
use VehicleType::*;

// 자동차 생성
let car = VehicleFactory::create_vehicle(
    Car,
    VehicleParams::car("Toyota", "Camry", 2023)
);

// 오토바이 생성
let motorcycle = VehicleFactory::create_vehicle(
    Motorcycle,
    VehicleParams::motorcycle("Honda", 600)
);

// 트럭 생성
let truck = VehicleFactory::create_vehicle(
    Truck,
    VehicleParams::truck("Ford", 5000)
);

println!("{}", car.start());
println!("{}", motorcycle.get_info());
println!("{}", truck.stop());
```

## Rust의 장점

### 1. 패턴 매칭
- `match` 표현식으로 명확한 객체 생성 로직

### 2. 타입 안전성
- 컴파일 타임에 모든 경우의 수 검증

### 3. 제로 코스트 추상화
- trait을 통한 효율적인 다형성

## 적용 상황

### 1. 게임 객체 생성
### 2. GUI 컴포넌트 팩토리
### 3. 데이터베이스 연결 팩토리
### 4. 네트워크 프로토콜 팩토리
### 5. 파일 포맷 파서 팩토리

Factory 패턴은 Rust의 enum과 pattern matching과 매우 잘 어울리며, 타입 안전한 객체 생성을 보장합니다.