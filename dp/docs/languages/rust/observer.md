# Observer Pattern - Rust Implementation

## 개요

Observer 패턴은 객체들 사이에 일대다 의존관계를 정의하여, 한 객체의 상태가 변할 때 모든 의존 객체들이 자동으로 알림을 받고 업데이트되도록 합니다. 이는 주제(subject)와 관찰자(observer) 간의 느슨한 결합을 촉진합니다.

## 구조

```rust
pub trait WeatherObserver {
    fn update(&mut self, weather_data: &WeatherData);
    fn get_id(&self) -> String;
}

pub struct WeatherStation {
    observers: Vec<Weak<RefCell<dyn WeatherObserver>>>,
    weather_data: WeatherData,
}

impl WeatherStation {
    pub fn add_observer(&mut self, observer: Weak<RefCell<dyn WeatherObserver>>) {
        self.observers.push(observer);
    }

    pub fn notify_observers(&mut self) {
        for observer in &self.observers {
            if let Some(obs) = observer.upgrade() {
                obs.borrow_mut().update(&self.weather_data);
            }
        }
    }
}
```

## Rust 구현

### 1. 관찰자 인터페이스
```rust
pub trait WeatherObserver {
    fn update(&mut self, weather_data: &WeatherData);
    fn get_id(&self) -> String;
}

// 구체적인 관찰자
pub struct CurrentConditionsDisplay {
    id: String,
    temperature: f32,
    humidity: f32,
}

impl WeatherObserver for CurrentConditionsDisplay {
    fn update(&mut self, weather_data: &WeatherData) {
        self.temperature = weather_data.temperature();
        self.humidity = weather_data.humidity();
        self.display();
    }

    fn get_id(&self) -> String {
        self.id.clone()
    }
}
```

### 2. 주제(Subject) 구현
```rust
pub struct WeatherStation {
    observers: Vec<Weak<RefCell<dyn WeatherObserver>>>,
    weather_data: WeatherData,
}

impl WeatherStation {
    pub fn set_measurements(&mut self, temperature: f32, humidity: f32, pressure: f32) {
        self.weather_data = WeatherData::new(temperature, humidity, pressure);
        self.notify_observers();
    }

    pub fn notify_observers(&mut self) {
        self.observers.retain(|observer| observer.strong_count() > 0);

        for observer in &self.observers {
            if let Some(obs) = observer.upgrade() {
                obs.borrow_mut().update(&self.weather_data);
            }
        }
    }
}
```

## 사용 예제

```rust
let mut weather_station = WeatherStation::new();

// 관찰자들 생성 및 등록
let current_display = Rc::new(RefCell::new(CurrentConditionsDisplay::new("Display1")));
let statistics_display = Rc::new(RefCell::new(StatisticsDisplay::new("Stats1")));

weather_station.add_observer(Rc::downgrade(&current_display));
weather_station.add_observer(Rc::downgrade(&statistics_display));

// 날씨 데이터 업데이트 - 모든 관찰자에게 자동 알림
weather_station.set_measurements(25.5, 65.0, 1013.25);
weather_station.set_measurements(27.2, 70.0, 1015.30);
```

## Rust의 장점

### 1. 메모리 안전성
- `Weak` 참조로 순환 참조 방지
- 자동 메모리 관리

### 2. 스레드 안전성
- 멀티스레드 환경에서도 안전한 관찰자 패턴

### 3. 타입 안전성
- 컴파일 타임에 타입 검증

## 적용 상황

### 1. MVC 아키텍처의 뷰 업데이트
### 2. 이벤트 처리 시스템
### 3. 모델 변경 알림
### 4. 실시간 데이터 모니터링
### 5. 게임의 상태 변화 알림

Observer 패턴은 Rust의 소유권 시스템과 잘 어울리며, 안전하고 효율적인 이벤트 기반 시스템을 구축할 수 있습니다.