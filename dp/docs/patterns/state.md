# State Pattern

## 개요
State 패턴은 객체의 내부 상태가 바뀜에 따라 객체의 행동을 바꿀 수 있는 행위 패턴입니다. 객체가 마치 자신의 클래스를 바꾸는 것처럼 보입니다.

## C 언어 구현
```c
typedef struct VendingMachine VendingMachine;
typedef struct State State;

struct State {
    void (*insert_coin)(State* self, VendingMachine* machine);
    void (*eject_coin)(State* self, VendingMachine* machine);
    void (*turn_crank)(State* self, VendingMachine* machine);
    void (*dispense)(State* self, VendingMachine* machine);
};

struct VendingMachine {
    State* no_coin_state;
    State* has_coin_state;
    State* sold_state;
    State* sold_out_state;
    State* current_state;
    int count;
};

// No Coin State
void no_coin_insert_coin(State* self, VendingMachine* machine) {
    printf("동전을 넣었습니다.\n");
    machine->current_state = machine->has_coin_state;
}

void no_coin_eject_coin(State* self, VendingMachine* machine) {
    printf("동전이 없습니다.\n");
}

void no_coin_turn_crank(State* self, VendingMachine* machine) {
    printf("동전을 먼저 넣어주세요.\n");
}

State no_coin_state = {
    .insert_coin = no_coin_insert_coin,
    .eject_coin = no_coin_eject_coin,
    .turn_crank = no_coin_turn_crank
};

// Has Coin State
void has_coin_insert_coin(State* self, VendingMachine* machine) {
    printf("이미 동전이 있습니다.\n");
}

void has_coin_eject_coin(State* self, VendingMachine* machine) {
    printf("동전이 반환되었습니다.\n");
    machine->current_state = machine->no_coin_state;
}

void has_coin_turn_crank(State* self, VendingMachine* machine) {
    printf("크랭크를 돌렸습니다.\n");
    machine->current_state = machine->sold_state;
}

State has_coin_state = {
    .insert_coin = has_coin_insert_coin,
    .eject_coin = has_coin_eject_coin,
    .turn_crank = has_coin_turn_crank
};
```

## 사용 예제
```c
VendingMachine* machine = create_vending_machine(5);

machine->current_state->insert_coin(machine->current_state, machine);  // 동전을 넣었습니다.
machine->current_state->turn_crank(machine->current_state, machine);   // 크랭크를 돌렸습니다.
                                                                       // 상품이 나왔습니다.
```

## 적용 상황
- **상태 기계**: 명확한 상태가 있는 시스템
- **게임 캐릭터**: 캐릭터의 다양한 상태 (달리기, 점프, 공격)
- **네트워크 연결**: 연결, 대기, 끊김 등의 상태
- **문서 워크플로**: 초안, 검토, 승인, 게시 상태