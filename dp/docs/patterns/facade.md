# Facade Pattern

## 개요
Facade 패턴은 복잡한 서브시스템에 대한 단순한 인터페이스를 제공하는 구조 패턴입니다. 여러 개의 복잡한 클래스나 인터페이스를 하나의 단순한 인터페이스로 통합하여 클라이언트가 쉽게 사용할 수 있도록 합니다.

## 구조
- **Facade**: 클라이언트에게 단순한 인터페이스를 제공하는 클래스
- **Subsystem Classes**: 실제 작업을 수행하는 복잡한 서브시스템 클래스들
- **Client**: Facade를 통해 서브시스템을 사용하는 클라이언트

## C 언어 구현

### 서브시스템 컴포넌트들
```c
// CPU 서브시스템
typedef struct CPU {
    int temperature;
} CPU;

CPU* create_cpu() {
    CPU* cpu = malloc(sizeof(CPU));
    cpu->temperature = 35;
    return cpu;
}

void cpu_start(CPU* cpu) {
    printf("CPU: Starting processor\n");
    cpu->temperature = 45;
}

void cpu_shutdown(CPU* cpu) {
    printf("CPU: Shutting down processor\n");
    cpu->temperature = 25;
}

void cpu_status(CPU* cpu) {
    printf("CPU: Temperature %d°C\n", cpu->temperature);
}

// 메모리 서브시스템
typedef struct Memory {
    int used_memory;
    int total_memory;
} Memory;

Memory* create_memory() {
    Memory* mem = malloc(sizeof(Memory));
    mem->total_memory = 16000;
    mem->used_memory = 2000;
    return mem;
}

void memory_load(Memory* mem) {
    printf("Memory: Loading system data\n");
    mem->used_memory += 1000;
}

void memory_free(Memory* mem) {
    printf("Memory: Freeing system data\n");
    mem->used_memory = 2000;
}

void memory_status(Memory* mem) {
    printf("Memory: %d/%d MB used\n", mem->used_memory, mem->total_memory);
}

// 하드드라이브 서브시스템
typedef struct HardDrive {
    int used_space;
    int total_space;
} HardDrive;

HardDrive* create_harddrive() {
    HardDrive* hdd = malloc(sizeof(HardDrive));
    hdd->total_space = 500000;
    hdd->used_space = 100000;
    return hdd;
}

void hdd_read_boot_sector(HardDrive* hdd) {
    printf("HDD: Reading boot sector\n");
}

void hdd_spin_up(HardDrive* hdd) {
    printf("HDD: Spinning up disk\n");
}

void hdd_spin_down(HardDrive* hdd) {
    printf("HDD: Spinning down disk\n");
}
```

### Facade 구현
```c
// 컴퓨터 시스템 Facade
typedef struct ComputerFacade {
    CPU* cpu;
    Memory* memory;
    HardDrive* harddrive;
    GPU* gpu;
} ComputerFacade;

ComputerFacade* create_computer_facade() {
    ComputerFacade* computer = malloc(sizeof(ComputerFacade));
    computer->cpu = create_cpu();
    computer->memory = create_memory();
    computer->harddrive = create_harddrive();
    computer->gpu = create_gpu();
    return computer;
}

// 복잡한 시작 절차를 단순한 인터페이스로 제공
void computer_start(ComputerFacade* computer) {
    printf("=== Starting Computer ===\n");

    // 복잡한 시작 시퀀스를 내부적으로 처리
    cpu_start(computer->cpu);
    memory_load(computer->memory);
    hdd_spin_up(computer->harddrive);
    hdd_read_boot_sector(computer->harddrive);
    gpu_initialize(computer->gpu);

    printf("Computer started successfully!\n\n");
}

// 복잡한 종료 절차를 단순한 인터페이스로 제공
void computer_shutdown(ComputerFacade* computer) {
    printf("=== Shutting Down Computer ===\n");

    // 복잡한 종료 시퀀스를 내부적으로 처리
    gpu_shutdown(computer->gpu);
    memory_free(computer->memory);
    hdd_spin_down(computer->harddrive);
    cpu_shutdown(computer->cpu);

    printf("Computer shut down safely!\n\n");
}

// 시스템 상태를 간단하게 확인
void computer_status(ComputerFacade* computer) {
    printf("=== Computer Status ===\n");
    cpu_status(computer->cpu);
    memory_status(computer->memory);
    hdd_status(computer->harddrive);
    gpu_status(computer->gpu);
    printf("\n");
}

// 게이밍 모드 - 복잡한 설정을 단순화
void computer_gaming_mode(ComputerFacade* computer) {
    printf("=== Enabling Gaming Mode ===\n");
    gpu_boost_performance(computer->gpu);
    memory_optimize_for_gaming(computer->memory);
    cpu_turbo_mode(computer->cpu);
    printf("Gaming mode enabled!\n\n");
}

void destroy_computer_facade(ComputerFacade* computer) {
    if (computer) {
        free(computer->cpu);
        free(computer->memory);
        free(computer->harddrive);
        free(computer->gpu);
        free(computer);
    }
}
```

## 사용 예제
```c
int main() {
    // 클라이언트는 복잡한 서브시스템을 직접 다루지 않음
    ComputerFacade* computer = create_computer_facade();

    // 단순한 인터페이스로 복잡한 작업 수행
    computer_start(computer);       // 내부적으로 여러 서브시스템 초기화
    computer_status(computer);      // 모든 컴포넌트 상태 확인
    computer_gaming_mode(computer); // 게이밍 최적화 설정
    computer_shutdown(computer);    // 안전한 종료 절차

    destroy_computer_facade(computer);
    return 0;
}
```

출력 예제:
```
=== Starting Computer ===
CPU: Starting processor
Memory: Loading system data
HDD: Spinning up disk
HDD: Reading boot sector
GPU: Initializing graphics
Computer started successfully!

=== Computer Status ===
CPU: Temperature 45°C
Memory: 3000/16000 MB used
HDD: 100000/500000 MB used
GPU: Load 0%

=== Enabling Gaming Mode ===
GPU: Boosting performance to maximum
Memory: Optimizing for gaming applications
CPU: Enabling turbo mode
Gaming mode enabled!
```

## 장점과 단점

### 장점
- **단순화**: 복잡한 서브시스템을 단순한 인터페이스로 감쌈
- **디커플링**: 클라이언트와 서브시스템 간의 결합도 감소
- **사용성**: 복잡한 API를 쉽게 사용할 수 있음
- **유지보수**: 서브시스템 변경이 클라이언트에 미치는 영향 최소화
- **보안**: 서브시스템의 내부 구현을 숨길 수 있음

### 단점
- **제한된 기능**: Facade가 제공하는 기능만 사용 가능
- **추가 레이어**: 한 단계의 추상화 레이어가 추가됨
- **성능 오버헤드**: 간접 호출로 인한 약간의 성능 저하
- **유연성 부족**: 세밀한 제어가 필요한 경우 부적합

## 적용 상황
- **레거시 시스템**: 복잡한 레거시 코드를 단순한 인터페이스로 감쌀 때
- **라이브러리 래핑**: 복잡한 외부 라이브러리를 쉽게 사용하기 위해
- **API 단순화**: 복잡한 내부 API를 외부 사용자에게 단순화해서 제공
- **마이크로서비스**: 여러 마이크로서비스를 하나의 통합 인터페이스로 제공
- **시스템 통합**: 여러 시스템을 하나의 인터페이스로 통합
- **복잡한 초기화**: 여러 컴포넌트의 복잡한 초기화 과정을 단순화
- **워크플로우**: 복잡한 비즈니스 워크플로우를 단순한 메소드로 제공

Facade 패턴은 C 언어에서 복잡한 서브시스템들을 하나의 간단한 인터페이스로 통합하여 코드의 사용성과 유지보수성을 크게 향상시키는 유용한 패턴입니다.