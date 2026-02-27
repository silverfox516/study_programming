# Event Sourcing Pattern

## 개요
Event Sourcing 패턴은 애플리케이션 상태의 변경을 일련의 이벤트로 저장하는 패턴입니다. 현재 상태를 직접 저장하는 대신 상태를 변경하는 모든 이벤트를 순차적으로 저장하고, 필요할 때 이벤트를 재생하여 현재 상태를 복원합니다.

## 구조
- **Event**: 시스템에서 발생한 상태 변경을 나타내는 불변 객체
- **Event Store**: 모든 이벤트를 순서대로 저장하는 저장소
- **Aggregate**: 이벤트를 적용하여 현재 상태를 유지하는 객체
- **Command**: 상태 변경 요청을 나타내는 객체
- **Command Handler**: 명령을 처리하여 이벤트를 생성하는 컴포넌트

## C 언어 구현

### 이벤트 정의
```c
// 이벤트 타입
typedef enum {
    EVENT_ACCOUNT_OPENED,
    EVENT_MONEY_DEPOSITED,
    EVENT_MONEY_WITHDRAWN,
    EVENT_ACCOUNT_CLOSED
} EventType;

// 기본 이벤트 구조체
typedef struct {
    EventType type;
    char aggregate_id[UUID_LEN];
    time_t timestamp;

    // 이벤트별 데이터 (union 사용)
    union {
        struct {
            char owner_name[MAX_NAME_LEN];
            double initial_balance;
        } account_opened;

        struct {
            double amount;
        } money_deposited;

        struct {
            double amount;
        } money_withdrawn;

        struct {
            double final_balance;
        } account_closed;
    } data;
} Event;
```

### 애그리게이트(Aggregate) 정의
```c
// 은행 계좌 애그리게이트
typedef struct {
    char id[UUID_LEN];
    char owner_name[MAX_NAME_LEN];
    double balance;
    int is_closed;
    int version;
} BankAccount;
```

### 이벤트 스토어
```c
// 이벤트 저장소
typedef struct {
    Event events[MAX_EVENTS];
    int event_count;
} EventStore;

// 이벤트 저장
int event_store_append(EventStore* store, const Event* event) {
    if (!store || !event || store->event_count >= MAX_EVENTS) {
        return 0;
    }

    store->events[store->event_count] = *event;
    store->event_count++;
    return 1;
}

// 특정 애그리게이트의 이벤트 조회
int event_store_get_events_for_aggregate(EventStore* store, const char* aggregate_id,
                                        Event* events, int max_events) {
    int count = 0;
    for (int i = 0; i < store->event_count && count < max_events; i++) {
        if (strcmp(store->events[i].aggregate_id, aggregate_id) == 0) {
            events[count++] = store->events[i];
        }
    }
    return count;
}
```

### 명령 처리
```c
// 명령 타입
typedef enum {
    CMD_OPEN_ACCOUNT,
    CMD_DEPOSIT_MONEY,
    CMD_WITHDRAW_MONEY,
    CMD_CLOSE_ACCOUNT
} CommandType;

typedef struct {
    CommandType type;
    char account_id[UUID_LEN];

    union {
        struct {
            char owner_name[MAX_NAME_LEN];
            double initial_balance;
        } open_account;

        struct {
            double amount;
        } deposit_money;

        struct {
            double amount;
        } withdraw_money;
    } data;
} Command;
```

### 이벤트 적용 및 상태 복원
```c
// 이벤트를 애그리게이트에 적용
void apply_event_to_account(BankAccount* account, const Event* event) {
    if (!account || !event) return;

    switch (event->type) {
        case EVENT_ACCOUNT_OPENED:
            strcpy(account->id, event->aggregate_id);
            strcpy(account->owner_name, event->data.account_opened.owner_name);
            account->balance = event->data.account_opened.initial_balance;
            account->is_closed = 0;
            account->version++;
            break;

        case EVENT_MONEY_DEPOSITED:
            account->balance += event->data.money_deposited.amount;
            account->version++;
            break;

        case EVENT_MONEY_WITHDRAWN:
            account->balance -= event->data.money_withdrawn.amount;
            account->version++;
            break;

        case EVENT_ACCOUNT_CLOSED:
            account->is_closed = 1;
            account->version++;
            break;
    }
}

// 이벤트 스토어에서 상태 복원
BankAccount* rebuild_account_from_events(EventStore* store, const char* account_id) {
    Event events[MAX_EVENTS];
    int event_count = event_store_get_events_for_aggregate(store, account_id, events, MAX_EVENTS);

    if (event_count == 0) return NULL;

    BankAccount* account = malloc(sizeof(BankAccount));
    if (!account) return NULL;

    memset(account, 0, sizeof(BankAccount));

    // 모든 이벤트를 순차적으로 적용
    for (int i = 0; i < event_count; i++) {
        apply_event_to_account(account, &events[i]);
    }

    return account;
}
```

## 사용 예제
```c
int main() {
    EventStore* store = create_event_store();

    // 계좌 개설 이벤트 생성 및 저장
    Event* opened_event = create_account_opened_event("ACC001", "John Doe", 1000.0);
    event_store_append(store, opened_event);

    // 입금 이벤트
    Event* deposit_event = create_money_deposited_event("ACC001", 500.0);
    event_store_append(store, deposit_event);

    // 출금 이벤트
    Event* withdraw_event = create_money_withdrawn_event("ACC001", 200.0);
    event_store_append(store, withdraw_event);

    // 이벤트로부터 현재 상태 복원
    BankAccount* account = rebuild_account_from_events(store, "ACC001");

    printf("Account ID: %s\n", account->id);
    printf("Owner: %s\n", account->owner_name);
    printf("Balance: %.2f\n", account->balance);  // 1300.0
    printf("Version: %d\n", account->version);    // 3

    return 0;
}
```

## 장점과 단점

### 장점
- **완전한 감사 추적**: 모든 변경사항이 이벤트로 기록됨
- **시간 여행**: 임의의 시점의 상태를 재구성할 수 있음
- **이벤트 재생**: 시스템을 다른 상태로 복원하거나 테스트 가능
- **확장성**: 이벤트를 다양한 목적으로 활용 가능
- **디버깅**: 버그 발생 시 전체 히스토리를 추적할 수 있음

### 단점
- **복잡성**: 기존 CRUD보다 복잡한 구현
- **저장 공간**: 모든 이벤트를 저장하므로 많은 공간 필요
- **성능**: 상태 복원 시 모든 이벤트를 재생해야 함
- **이벤트 진화**: 시간이 지나면서 이벤트 스키마가 변경될 수 있음
- **쿼리 복잡성**: 복잡한 쿼리를 위해서는 별도의 읽기 모델 필요

## 적용 상황
- **금융 시스템**: 모든 거래 내역이 중요한 금융 애플리케이션
- **감사가 중요한 시스템**: 규정 준수가 필요한 시스템
- **협업 도구**: 문서나 데이터의 변경 이력 추적
- **게임**: 플레이어 행동과 게임 상태 변경 기록
- **IoT 시스템**: 센서 데이터와 디바이스 상태 변경
- **버전 관리**: 데이터나 설정의 버전 관리
- **실시간 분석**: 이벤트 스트림을 통한 실시간 데이터 분석

Event Sourcing 패턴은 C 언어에서 구조체와 이벤트 스토어를 활용하여 시스템의 모든 상태 변경을 추적하고 재생할 수 있는 강력한 아키텍처 패턴입니다.