# Builder Pattern

## 개요
Builder 패턴은 복잡한 객체의 생성 과정을 단계별로 나누어 처리하는 생성 패턴입니다. 동일한 생성 과정에서 서로 다른 표현 결과를 만들 수 있으며, 생성 과정을 캡슐화하여 객체 생성의 복잡성을 숨깁니다. 특히 많은 선택적 매개변수가 있는 객체를 생성할 때 유용합니다.

## 구조
- **Product (Computer)**: 생성되는 복잡한 객체
- **Builder (ComputerBuilder)**: 제품 객체의 부분들을 생성하기 위한 추상 인터페이스
- **Concrete Builder**: Builder 인터페이스를 구현하여 제품의 부품들을 구축하고 결과 제품을 반환
- **Director**: Builder 인터페이스를 사용하는 객체 (이 예제에서는 클라이언트가 직접 수행)

## C 언어 구현

### 제품 클래스 정의
```c
// 생성할 복잡한 객체 - Computer
typedef struct Computer {
    char* cpu;
    char* ram;
    char* storage;
    char* gpu;
    char* motherboard;
    int has_wifi;
    int has_bluetooth;
} Computer;
```

### 빌더 인터페이스
```c
// 빌더 인터페이스 - 메소드 체이닝 지원
typedef struct ComputerBuilder {
    Computer* computer;
    struct ComputerBuilder* (*set_cpu)(struct ComputerBuilder* self, const char* cpu);
    struct ComputerBuilder* (*set_ram)(struct ComputerBuilder* self, const char* ram);
    struct ComputerBuilder* (*set_storage)(struct ComputerBuilder* self, const char* storage);
    struct ComputerBuilder* (*set_gpu)(struct ComputerBuilder* self, const char* gpu);
    struct ComputerBuilder* (*set_motherboard)(struct ComputerBuilder* self, const char* motherboard);
    struct ComputerBuilder* (*add_wifi)(struct ComputerBuilder* self);
    struct ComputerBuilder* (*add_bluetooth)(struct ComputerBuilder* self);
    Computer* (*build)(struct ComputerBuilder* self);
    void (*destroy)(struct ComputerBuilder* self);
} ComputerBuilder;
```

### 빌더 메소드 구현
```c
// CPU 설정 - 메소드 체이닝을 위해 self 반환
ComputerBuilder* builder_set_cpu(ComputerBuilder* self, const char* cpu) {
    if (self->computer->cpu) free(self->computer->cpu);
    self->computer->cpu = malloc(strlen(cpu) + 1);
    strcpy(self->computer->cpu, cpu);
    return self;  // 메소드 체이닝 지원
}

ComputerBuilder* builder_set_ram(ComputerBuilder* self, const char* ram) {
    if (self->computer->ram) free(self->computer->ram);
    self->computer->ram = malloc(strlen(ram) + 1);
    strcpy(self->computer->ram, ram);
    return self;
}

ComputerBuilder* builder_add_wifi(ComputerBuilder* self) {
    self->computer->has_wifi = 1;
    return self;
}

// 최종 제품 반환
Computer* builder_build(ComputerBuilder* self) {
    Computer* result = self->computer;
    self->computer = NULL;  // 소유권 이전
    return result;
}
```

### 빌더 팩토리
```c
ComputerBuilder* create_computer_builder() {
    ComputerBuilder* builder = malloc(sizeof(ComputerBuilder));
    builder->computer = malloc(sizeof(Computer));

    // 컴퓨터 초기화
    builder->computer->cpu = NULL;
    builder->computer->ram = NULL;
    builder->computer->storage = NULL;
    builder->computer->gpu = NULL;
    builder->computer->motherboard = NULL;
    builder->computer->has_wifi = 0;
    builder->computer->has_bluetooth = 0;

    // 함수 포인터 설정
    builder->set_cpu = builder_set_cpu;
    builder->set_ram = builder_set_ram;
    builder->set_storage = builder_set_storage;
    builder->set_gpu = builder_set_gpu;
    builder->set_motherboard = builder_set_motherboard;
    builder->add_wifi = builder_add_wifi;
    builder->add_bluetooth = builder_add_bluetooth;
    builder->build = builder_build;
    builder->destroy = builder_destroy;

    return builder;
}
```

## 사용 예제
```c
int main() {
    // 게이밍 컴퓨터 구성
    printf("Building Gaming Computer:\n");
    ComputerBuilder* gaming_builder = create_computer_builder();
    Computer* gaming_pc = gaming_builder
        ->set_cpu(gaming_builder, "Intel i9-13900K")
        ->set_ram(gaming_builder, "32GB DDR5")
        ->set_storage(gaming_builder, "1TB NVMe SSD")
        ->set_gpu(gaming_builder, "RTX 4080")
        ->set_motherboard(gaming_builder, "ASUS ROG Strix Z790")
        ->add_wifi(gaming_builder)
        ->add_bluetooth(gaming_builder)
        ->build(gaming_builder);

    print_computer(gaming_pc);

    // 오피스 컴퓨터 구성 (더 간단)
    printf("Building Office Computer:\n");
    ComputerBuilder* office_builder = create_computer_builder();
    Computer* office_pc = office_builder
        ->set_cpu(office_builder, "Intel i5-13400")
        ->set_ram(office_builder, "16GB DDR4")
        ->set_storage(office_builder, "512GB SSD")
        ->set_motherboard(office_builder, "MSI Pro B660M")
        ->add_wifi(office_builder)
        ->build(office_builder);

    print_computer(office_pc);

    // 예산형 컴퓨터 (최소 구성)
    printf("Building Budget Computer:\n");
    ComputerBuilder* budget_builder = create_computer_builder();
    Computer* budget_pc = budget_builder
        ->set_cpu(budget_builder, "AMD Ryzen 5 5600G")
        ->set_ram(budget_builder, "8GB DDR4")
        ->set_storage(budget_builder, "256GB SSD")
        ->build(budget_builder);

    print_computer(budget_pc);

    return 0;
}
```

출력 예제:
```
Building Gaming Computer:
Computer Configuration:
  CPU: Intel i9-13900K
  RAM: 32GB DDR5
  Storage: 1TB NVMe SSD
  GPU: RTX 4080
  Motherboard: ASUS ROG Strix Z790
  WiFi: Yes
  Bluetooth: Yes

Building Office Computer:
Computer Configuration:
  CPU: Intel i5-13400
  RAM: 16GB DDR4
  Storage: 512GB SSD
  GPU: Not specified
  Motherboard: MSI Pro B660M
  WiFi: Yes
  Bluetooth: No
```

## 장점과 단점

### 장점
- **단계별 구성**: 복잡한 객체를 단계적으로 구성할 수 있음
- **가독성**: 메소드 체이닝으로 코드 가독성 향상
- **유연성**: 동일한 생성 과정으로 다양한 객체 생성 가능
- **선택적 매개변수**: 필요한 속성만 선택적으로 설정 가능
- **불변 객체 지원**: 빌더를 통해 불변 객체를 안전하게 생성

### 단점
- **복잡성 증가**: 빌더 클래스가 추가로 필요함
- **메모리 오버헤드**: 빌더 객체와 제품 객체 모두 필요
- **코드 중복**: 빌더 메소드에서 유사한 코드 반복 가능성

## 적용 상황
- **복잡한 객체 생성**: 많은 매개변수를 가진 객체를 생성할 때
- **선택적 매개변수**: 일부는 필수, 일부는 선택적인 매개변수가 있을 때
- **설정 객체**: 애플리케이션 설정이나 구성 객체
- **SQL 쿼리 빌더**: 복잡한 SQL 쿼리를 단계별로 구성
- **HTTP 요청 빌더**: 다양한 옵션을 가진 HTTP 요청 생성
- **UI 컴포넌트**: 복잡한 UI 컴포넌트의 설정
- **게임 캐릭터**: 다양한 속성을 가진 게임 캐릭터 생성

Builder 패턴은 C 언어에서 함수 포인터와 메소드 체이닝을 활용하여 복잡한 객체 생성 과정을 간단하고 명확하게 만들어주는 유용한 패턴입니다.