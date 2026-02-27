# Adapter Pattern - Rust Implementation

## 개요

Adapter 패턴은 호환되지 않는 인터페이스들이 함께 작동할 수 있도록 하는 구조적 디자인 패턴입니다. Rust에서는 트레이트(trait)와 래퍼 구조체(wrapper struct)를 활용하여 기존 코드를 수정하지 않고도 다른 인터페이스와 호환되도록 만들 수 있습니다.

이 패턴은 특히 레거시 코드 통합, 서드파티 라이브러리 사용, 그리고 서로 다른 API 인터페이스를 통합할 때 매우 유용합니다.

## 구조

### 핵심 구성 요소

1. **Target Trait** (`MediaPlayer`, `WeatherService`, `DatabaseConnection`)
   - 클라이언트가 기대하는 인터페이스 정의
   - 통합된 API 제공

2. **Adaptee** (`Mp3Player`, `ThirdPartyWeatherAPI`, `LegacyDatabase`)
   - 기존에 존재하는 클래스나 라이브러리
   - 다른 인터페이스를 가지고 있어 직접 사용할 수 없음

3. **Adapter** (`MediaAdapter`, `WeatherAdapter`, `DatabaseAdapter`)
   - Target 인터페이스를 구현하면서 Adaptee를 감싸는 래퍼
   - 인터페이스 변환 로직을 포함

4. **Client** (`AudioPlayer`, `Application`)
   - Target 인터페이스를 통해 기능을 사용하는 코드

## Rust 구현

### 객체 어댑터 패턴

```rust
// Target 인터페이스
pub trait MediaPlayer {
    fn play(&self, filename: &str) -> Result<String, String>;
    fn get_supported_formats(&self) -> Vec<String>;
}

// Adaptee - 호환되지 않는 인터페이스
pub trait AdvancedMediaPlayer {
    fn play_vlc(&self, filename: &str) -> String;
    fn play_mp4(&self, filename: &str) -> String;
    fn play_avi(&self, filename: &str) -> String;
}

// Adapter - 인터페이스 변환
pub struct MediaAdapter {
    advanced_player: Box<dyn AdvancedMediaPlayer>,
    supported_formats: Vec<String>,
}

impl MediaPlayer for MediaAdapter {
    fn play(&self, filename: &str) -> Result<String, String> {
        let extension = self.get_file_extension(filename)?;
        match extension.to_lowercase().as_str() {
            "vlc" => Ok(self.advanced_player.play_vlc(filename)),
            "mp4" => Ok(self.advanced_player.play_mp4(filename)),
            "avi" => Ok(self.advanced_player.play_avi(filename)),
            _ => Err(format!("Unsupported format: {}", extension)),
        }
    }
}
```

### 소유권 시스템 활용

1. **Box<dyn Trait>**: 힙 할당된 트레이트 객체로 다형성 구현
2. **Result<T, E>**: 안전한 에러 처리와 명시적 실패 케이스 표현
3. **Option<T>**: null 안전성과 명시적 없음 상태 표현

### Rust 특수 기능 활용

1. **패턴 매칭을 통한 포맷 감지**
   ```rust
   match extension.to_lowercase().as_str() {
       "vlc" => Ok(self.advanced_player.play_vlc(filename)),
       "mp4" => Ok(self.advanced_player.play_mp4(filename)),
       "avi" => Ok(self.advanced_player.play_avi(filename)),
       _ => Err(format!("Unsupported format: {}", extension)),
   }
   ```

2. **Result 체이닝과 에러 전파**
   ```rust
   pub fn get_temperature(&self, city: &str) -> Result<f64, String> {
       let weather_data = self.third_party_api.fetch_weather_data(city)?;
       Ok(Self::kelvin_to_celsius(weather_data.temperature_kelvin))
   }
   ```

3. **Default 트레이트 구현**
   ```rust
   impl Default for AudioPlayer {
       fn default() -> Self {
           Self::new()
       }
   }
   ```

4. **구조체 분해와 메서드 체이닝**
   ```rust
   fn get_file_extension(filename: &str) -> Option<&str> {
       filename.split('.').last()
   }
   ```

### 메모리 안전성과 동시성

- **Borrow Checker**: 런타임에 메모리 안전성 보장, 댕글링 포인터 방지
- **Zero-cost abstractions**: 런타임 오버헤드 없는 추상화
- **Interior mutability**: `RefCell`, `Mutex` 등을 통한 안전한 가변성 제어

## 사용 예제

### 1. 미디어 플레이어 어댑터
```rust
let audio_player = AudioPlayer::new();

// 기본 지원 포맷 (MP3)
let result = audio_player.play("song.mp3")?;

// 어댑터를 통한 확장 포맷 (MP4, VLC, AVI)
let result = audio_player.play("movie.mp4")?;
```

### 2. API 통합 어댑터
```rust
let weather_service = WeatherAdapter::new();
let temperature = weather_service.get_temperature("New York")?;  // 켈빈을 섭씨로 변환
let humidity = weather_service.get_humidity("London")?;
```

### 3. 레거시 시스템 어댑터
```rust
let db_adapter = DatabaseAdapter::new("localhost", 5432);
db_adapter.connect()?;
let results = db_adapter.execute_query("SELECT * FROM users")?;
db_adapter.close()?;
```

## Rust의 장점

### 1. 타입 안전성
- 컴파일 타임에 인터페이스 호환성 검증
- 런타임 오류 방지

### 2. 제로코스트 추상화
- 어댑터 패턴이 런타임 성능에 미치는 영향 최소화
- 인라인 최적화로 간접 호출 오버헤드 제거

### 3. 에러 처리의 명시성
- `Result<T, E>` 타입으로 실패 가능성을 타입 시스템에 반영
- `?` 연산자로 깔끔한 에러 전파

### 4. 메모리 효율성
- 스택 할당 우선, 필요시에만 힙 할당
- 자동 메모리 관리로 메모리 누수 방지

### 5. 패턴 매칭의 완전성
- 모든 경우를 처리했는지 컴파일 타임에 검증
- `match` 표현식의 exhaustiveness 체크

### 6. 트레이트 시스템
- 코드 재사용성과 확장성
- 다중 상속 없이도 유연한 인터페이스 구현

## 적용 상황

### 1. 레거시 시스템 통합
- 기존 시스템을 새로운 인터페이스에 맞춤
- 코드 수정 없이 호환성 제공

### 2. 서드파티 라이브러리 통합
- 외부 API의 인터페이스를 내부 표준에 맞춤
- 의존성 격리와 인터페이스 통일

### 3. 데이터 형식 변환
- JSON, XML, CSV 등 다양한 포맷 간 변환
- 통일된 데이터 처리 인터페이스 제공

### 4. 플랫폼별 구현 통합
- 운영체제별, 플랫폼별 구현체를 통일된 인터페이스로 제공
- 크로스 플랫폼 호환성

### 5. 테스트 더블(Test Double)
- Mock 객체나 Stub을 실제 구현체와 동일한 인터페이스로 제공
- 테스트 격리와 의존성 주입

이 구현은 Rust의 강력한 타입 시스템과 소유권 모델을 활용하여 안전하고 효율적인 Adapter 패턴을 보여줍니다. 특히 에러 처리의 명시성과 메모리 안전성에서 다른 언어 구현 대비 큰 장점을 가집니다.