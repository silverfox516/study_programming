# Command Pattern

## 개요
Command 패턴은 요청을 객체의 형태로 캡슐화하여 사용자가 보낸 요청을 나중에 이용할 수 있도록 매개체 역할을 하는 행위 패턴입니다. 요청을 객체로 저장함으로써 실행 취소(undo), 매크로 기능, 로깅, 큐잉 등을 지원할 수 있습니다.

## 구조
- **Command**: 실행될 기능에 대한 인터페이스 정의 (execute, undo)
- **Concrete Command (LightOnCommand, LightOffCommand)**: 실제로 실행되는 기능을 구현
- **Receiver (Light)**: 명령어에 대한 실제 작업을 수행하는 객체
- **Invoker (RemoteControl)**: 명령어에 대한 요청을 하는 호출자 역할

## C 언어 구현

### 명령 인터페이스 정의
```c
// 명령 인터페이스
typedef struct Command {
    void (*execute)(struct Command* self);
    void (*undo)(struct Command* self);
    void (*destroy)(struct Command* self);
} Command;
```

### 수신자(Receiver) 구현
```c
// 수신자 - 전등
typedef struct Light {
    int is_on;
    char* location;
} Light;

Light* create_light(const char* location) {
    Light* light = malloc(sizeof(Light));
    light->is_on = 0;
    light->location = malloc(strlen(location) + 1);
    strcpy(light->location, location);
    return light;
}

void light_on(Light* light) {
    light->is_on = 1;
    printf("Light in %s is ON\n", light->location);
}

void light_off(Light* light) {
    light->is_on = 0;
    printf("Light in %s is OFF\n", light->location);
}
```

### 구체적 명령 구현
```c
// 전등 켜기 명령
typedef struct LightOnCommand {
    Command base;
    Light* light;
} LightOnCommand;

void light_on_execute(Command* cmd) {
    LightOnCommand* light_cmd = (LightOnCommand*)cmd;
    light_on(light_cmd->light);
}

void light_on_undo(Command* cmd) {
    LightOnCommand* light_cmd = (LightOnCommand*)cmd;
    light_off(light_cmd->light);  // Undo는 반대 동작
}

Command* create_light_on_command(Light* light) {
    LightOnCommand* cmd = malloc(sizeof(LightOnCommand));
    cmd->base.execute = light_on_execute;
    cmd->base.undo = light_on_undo;
    cmd->base.destroy = light_on_destroy;
    cmd->light = light;
    return (Command*)cmd;
}

// 전등 끄기 명령
typedef struct LightOffCommand {
    Command base;
    Light* light;
} LightOffCommand;

void light_off_execute(Command* cmd) {
    LightOffCommand* light_cmd = (LightOffCommand*)cmd;
    light_off(light_cmd->light);
}

void light_off_undo(Command* cmd) {
    LightOffCommand* light_cmd = (LightOffCommand*)cmd;
    light_on(light_cmd->light);  // Undo는 반대 동작
}
```

### 호출자(Invoker) 구현
```c
// 호출자 - 리모컨
typedef struct RemoteControl {
    Command* commands[7];          // 7개 슬롯
    Command* undo_command;         // 마지막 실행된 명령
    Command* history[MAX_HISTORY]; // 명령 히스토리
    int history_count;
} RemoteControl;

RemoteControl* create_remote_control() {
    RemoteControl* remote = malloc(sizeof(RemoteControl));
    for (int i = 0; i < 7; i++) {
        remote->commands[i] = NULL;
    }
    remote->undo_command = NULL;
    remote->history_count = 0;
    return remote;
}

void set_command(RemoteControl* remote, int slot, Command* command) {
    if (slot >= 0 && slot < 7) {
        remote->commands[slot] = command;
    }
}

void press_button(RemoteControl* remote, int slot) {
    if (slot >= 0 && slot < 7 && remote->commands[slot]) {
        remote->commands[slot]->execute(remote->commands[slot]);
        remote->undo_command = remote->commands[slot];

        // 히스토리에 추가
        if (remote->history_count < MAX_HISTORY) {
            remote->history[remote->history_count++] = remote->commands[slot];
        } else {
            // 히스토리 시프트
            for (int i = 0; i < MAX_HISTORY - 1; i++) {
                remote->history[i] = remote->history[i + 1];
            }
            remote->history[MAX_HISTORY - 1] = remote->commands[slot];
        }
    }
}

void press_undo(RemoteControl* remote) {
    if (remote->undo_command) {
        remote->undo_command->undo(remote->undo_command);
    }
}
```

## 사용 예제
```c
int main() {
    // 디바이스 생성
    Light* living_room_light = create_light("Living Room");
    Light* kitchen_light = create_light("Kitchen");

    // 명령 생성
    Command* living_room_on = create_light_on_command(living_room_light);
    Command* living_room_off = create_light_off_command(living_room_light);
    Command* kitchen_on = create_light_on_command(kitchen_light);
    Command* kitchen_off = create_light_off_command(kitchen_light);

    // 리모컨 생성 및 설정
    RemoteControl* remote = create_remote_control();
    set_command(remote, 0, living_room_on);
    set_command(remote, 1, living_room_off);
    set_command(remote, 2, kitchen_on);
    set_command(remote, 3, kitchen_off);

    // 리모컨 사용
    press_button(remote, 0);  // Living room on
    press_button(remote, 2);  // Kitchen on
    press_button(remote, 1);  // Living room off

    // 실행 취소
    press_undo(remote);       // Undo living room off (다시 켜짐)

    return 0;
}
```

출력 예제:
```
Light in Living Room is ON
Light in Kitchen is ON
Light in Living Room is OFF
Light in Living Room is ON
```

## 장점과 단점

### 장점
- **디커플링**: 호출자와 수신자 사이의 결합도를 낮춤
- **실행 취소**: undo 기능을 쉽게 구현할 수 있음
- **매크로 명령**: 여러 명령을 조합하여 복잡한 명령 생성 가능
- **로깅**: 명령 실행 히스토리를 쉽게 관리
- **큐잉**: 명령을 큐에 저장하여 순차적 실행 가능
- **확장성**: 새로운 명령을 기존 코드 수정 없이 추가 가능

### 단점
- **복잡성 증가**: 각 명령마다 클래스가 필요하여 코드량 증가
- **메모리 사용량**: 명령 객체들로 인한 메모리 오버헤드
- **간접적 호출**: 직접 호출보다 성능이 약간 떨어질 수 있음

## 적용 상황
- **GUI 버튼과 메뉴**: 사용자 인터페이스의 액션 처리
- **매크로 기록**: 사용자의 일련의 동작을 기록하고 재실행
- **실행 취소/재실행**: 텍스트 에디터나 그래픽 프로그램의 undo/redo
- **원격 제어**: 스마트 홈 기기의 원격 제어
- **작업 큐**: 백그라운드에서 실행될 작업들의 관리
- **로깅 및 감사**: 시스템에서 실행된 명령들의 기록
- **트랜잭션**: 데이터베이스 트랜잭션의 롤백 기능
- **게임**: 게임에서의 액션 시스템과 리플레이 기능

Command 패턴은 C 언어에서 함수 포인터를 활용하여 요청을 객체화하고, 복잡한 제어 로직을 깔끔하게 구조화하는 데 매우 유용한 패턴입니다.