# Proxy Pattern - Rust Implementation

## 개요

Proxy 패턴은 다른 객체에 대한 접근을 제어하는 대리자나 자리채우개 역할을 하는 객체를 제공합니다. 실제 객체에 대한 참조를 보유하며, 클라이언트가 실제 객체와 동일한 인터페이스를 통해 상호작용할 수 있게 합니다.

## 구조

```rust
pub trait ImageService {
    fn display(&self) -> String;
    fn get_filename(&self) -> &str;
}

// 실제 서비스
pub struct RealImage {
    filename: String,
}

impl ImageService for RealImage {
    fn display(&self) -> String {
        format!("Displaying image: {}", self.filename)
    }

    fn get_filename(&self) -> &str {
        &self.filename
    }
}

// 프록시
pub struct ImageProxy {
    filename: String,
    real_image: Option<RealImage>,
}

impl ImageProxy {
    pub fn new(filename: String) -> Self {
        Self {
            filename,
            real_image: None,
        }
    }

    fn load_image(&mut self) {
        if self.real_image.is_none() {
            println!("Loading image: {}", self.filename);
            self.real_image = Some(RealImage::new(self.filename.clone()));
        }
    }
}

impl ImageService for ImageProxy {
    fn display(&self) -> String {
        if let Some(ref real_image) = self.real_image {
            real_image.display()
        } else {
            format!("Image {} not loaded yet", self.filename)
        }
    }

    fn get_filename(&self) -> &str {
        &self.filename
    }
}
```

## 사용 예제

```rust
let mut image_proxy = ImageProxy::new("large_image.jpg".to_string());

// 이미지가 아직 로드되지 않음
println!("{}", image_proxy.display());

// 실제 이미지 로드 및 표시
image_proxy.load_image();
println!("{}", image_proxy.display());
```

## 프록시 유형

### 1. Virtual Proxy (가상 프록시)
- 생성 비용이 높은 객체의 지연 초기화

### 2. Remote Proxy (원격 프록시)
- 원격 객체에 대한 로컬 대표자

### 3. Protection Proxy (보호 프록시)
- 접근 권한 제어

### 4. Smart Reference Proxy
- 추가 기능 제공 (참조 카운팅, 캐싱 등)

## 적용 상황

### 1. 이미지 지연 로딩
### 2. 원격 서비스 호출
### 3. 접근 권한 제어
### 4. 캐싱 메커니즘
### 5. 로깅 및 모니터링