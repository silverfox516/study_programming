# Builder Pattern - Rust Implementation

## 개요

Builder 패턴은 복잡한 객체를 단계별로 구성하는 생성 패턴입니다. Rust에서는 메서드 체이닝, Option 타입, 그리고 타입 시스템의 컴파일 타임 보장을 통해 안전하고 유연한 빌더 패턴을 구현할 수 있습니다.

이 패턴은 특히 많은 선택적 매개변수를 가진 객체 생성, 복잡한 설정 객체, 그리고 단계적 검증이 필요한 객체 구성에서 매우 유용합니다.

## 구조

### 핵심 구성 요소

1. **Product** (`Computer`)
   - 구성될 복잡한 객체
   - 다양한 선택적/필수 속성들을 포함

2. **Builder** (`ComputerBuilder`)
   - 제품을 단계별로 구성하는 인터페이스
   - 메서드 체이닝을 통한 유연한 구성

3. **Concrete Builder** (`ComputerBuilder`, `TypedComputerBuilder`)
   - 실제 구성 로직을 구현
   - 검증과 기본값 설정 담당

4. **Director** (`ComputerDirector`)
   - 특정 구성 패턴을 알고 있는 클래스
   - 미리 정의된 제품 변형을 생성

## Rust 구현

### 기본 빌더 패턴

```rust
#[derive(Debug, Clone)]
pub struct Computer {
    pub cpu: String,
    pub ram_gb: u32,
    pub storage_gb: u32,
    pub gpu: Option<String>,
    // 기타 필드들...
}

pub struct ComputerBuilder {
    cpu: Option<String>,
    ram_gb: Option<u32>,
    storage_gb: Option<u32>,
    gpu: Option<String>,
    // 기타 필드들...
}

impl ComputerBuilder {
    pub fn new() -> Self {
        Self {
            cpu: None,
            ram_gb: None,
            storage_gb: None,
            gpu: None,
        }
    }

    pub fn cpu(mut self, cpu: &str) -> Self {
        self.cpu = Some(cpu.to_string());
        self
    }

    pub fn build(self) -> Result<Computer, String> {
        let cpu = self.cpu.ok_or("CPU is required")?;
        let ram_gb = self.ram_gb.ok_or("RAM is required")?;

        // 검증 로직
        if ram_gb < 4 {
            return Err("Minimum 4GB RAM required".to_string());
        }

        Ok(Computer { cpu, ram_gb, /* ... */ })
    }
}
```

### 소유권 시스템 활용

1. **Move semantics**: 빌더 메서드들이 `self`를 소비하고 반환하여 체이닝 허용
2. **Option<T>**: 선택적 필드들을 안전하게 표현
3. **Result<T, E>**: 빌드 과정의 실패를 명시적으로 처리

### Rust 특수 기능 활용

1. **타입 안전 빌더 (Phantom Types)**
   ```rust
   use std::marker::PhantomData;

   pub struct Required;
   pub struct Optional;

   pub struct TypedComputerBuilder<CpuState, RamState, StorageState> {
       // 실제 데이터
       cpu: Option<String>,
       ram_gb: Option<u32>,
       // 타입 상태 추적
       _cpu_state: PhantomData<CpuState>,
       _ram_state: PhantomData<RamState>,
       _storage_state: PhantomData<StorageState>,
   }

   // 필수 필드가 모두 설정된 경우에만 build 가능
   impl TypedComputerBuilder<Required, Required, Required> {
       pub fn build(self) -> Computer {
           Computer {
               cpu: self.cpu.unwrap(),  // 타입 안전하게 unwrap
               ram_gb: self.ram_gb.unwrap(),
               // ...
           }
       }
   }
   ```

2. **Default 트레이트 구현**
   ```rust
   impl Default for ComputerBuilder {
       fn default() -> Self {
           Self::new()
       }
   }
   ```

3. **메서드 체이닝과 소유권**
   ```rust
   pub fn add_peripheral(mut self, peripheral: &str) -> Self {
       self.peripherals.push(peripheral.to_string());
       self  // 소유권을 이동하여 체이닝 허용
   }
   ```

### 메모리 안전성과 성능

- **스택 할당**: 빌더 객체는 기본적으로 스택에 할당
- **Zero-cost abstractions**: 빌더 패턴의 런타임 오버헤드 최소화
- **Move semantics**: 불필요한 복사 방지

## 사용 예제

### 1. 기본 빌더 사용
```rust
let computer = ComputerBuilder::new()
    .cpu("Intel Core i7-13700K")
    .ram(32)
    .storage(1000)
    .gpu("NVIDIA RTX 4070")
    .motherboard("ASUS ROG STRIX")
    .power_supply(750)
    .case_type("Mid Tower RGB")
    .operating_system("Windows 11")
    .add_peripheral("Gaming Keyboard")
    .add_peripheral("Gaming Mouse")
    .warranty_years(2)
    .build()?;

println!("{}", computer.get_specs());
println!("Estimated Price: ${}", computer.estimated_price());
```

### 2. Director 패턴 활용
```rust
// 미리 정의된 구성으로 빠른 생성
let gaming_pc = ComputerDirector::build_gaming_pc()?;
let office_pc = ComputerDirector::build_office_pc()?;
let budget_pc = ComputerDirector::build_budget_pc()?;
let workstation = ComputerDirector::build_workstation()?;
```

### 3. 검증과 에러 처리
```rust
let result = ComputerBuilder::new()
    .cpu("Intel i5")
    .ram(2)  // 최소 요구사항보다 낮음
    .storage(50)  // 최소 요구사항보다 낮음
    .build();

match result {
    Ok(computer) => println!("Built: {}", computer.get_specs()),
    Err(e) => println!("Build failed: {}", e),
}
```

### 4. 유연한 빌더 (Fluent Builder)
```rust
let result = FluentComputerBuilder::new()
    .cpu("AMD Ryzen 7")
    .ram(32)
    .validate_and_build();

match result {
    Ok(computer) => println!("Success: {}", computer.cpu),
    Err(errors) => {
        for error in errors {
            println!("Error: {}", error);
        }
    }
}
```

## Rust의 장점

### 1. 컴파일 타임 안전성
- 필수 필드 누락을 컴파일 시점에 감지
- 타입 상태를 통한 빌드 단계 검증
- 잘못된 메서드 호출 방지

### 2. 제로코스트 추상화
- 빌더 패턴의 런타임 오버헤드 없음
- 컴파일러 최적화로 직접 생성과 동일한 성능
- 인라인 최적화

### 3. 메모리 효율성
- 스택 기반 빌더 객체
- Move semantics로 불필요한 복사 방지
- 자동 메모리 관리

### 4. 에러 처리의 명시성
- Result<T, E>로 빌드 실패를 타입 시스템에 반영
- ? 연산자로 간결한 에러 전파
- 컴파일러 강제 에러 처리

### 5. 표현력과 가독성
- 메서드 체이닝으로 자연스러운 객체 구성
- 선택적 매개변수의 명확한 표현
- 도메인 특화 언어(DSL) 형태의 API

### 6. 확장성
- 새로운 필드 추가가 기존 코드에 미치는 영향 최소화
- 기본값 설정의 유연성
- 다양한 변형 패턴 지원

## 적용 상황

### 1. 복잡한 설정 객체
- 애플리케이션 구성 설정
- 네트워크 연결 설정
- 데이터베이스 연결 문자열

### 2. UI 컴포넌트 생성
- 복잡한 위젯 구성
- 스타일과 레이아웃 설정
- 이벤트 핸들러 바인딩

### 3. 빌드 시스템
- 컴파일러 옵션 설정
- 패키징 구성
- 배포 파이프라인 설정

### 4. 테스트 데이터 생성
- 테스트 객체 구성
- Mock 데이터 생성
- 다양한 테스트 시나리오

### 5. API 클라이언트
- HTTP 요청 구성
- 인증 정보 설정
- 요청 옵션과 헤더

### 6. 게임 객체 생성
- 캐릭터 속성 설정
- 아이템 구성
- 게임 월드 생성

이 구현은 Rust의 타입 시스템과 소유권 모델을 활용하여 안전하고 효율적인 Builder 패턴을 보여줍니다. 특히 컴파일 타임 검증과 런타임 성능의 균형을 잘 보여주는 패턴입니다.