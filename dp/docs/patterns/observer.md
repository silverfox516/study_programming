# Observer Pattern

## 개요
Observer 패턴은 객체 사이에 일대다의 종속성을 정의하여, 한 객체의 상태가 변할 때 그 객체에 종속된 모든 객체들이 자동으로 알림을 받고 갱신되는 행위 패턴입니다.

## 구조
- **Subject**: 관찰 대상이 되는 객체, 옵저버들을 관리
- **Observer**: 주제의 변화를 관찰하는 인터페이스
- **Concrete Observer**: 구체적인 옵저버 구현체

## C 언어 구현

### 기본 인터페이스
```c
typedef struct Observer Observer;
typedef struct Subject Subject;

struct Observer {
    void (*update)(Observer* self, Subject* subject);
    char* name;
};

struct Subject {
    Observer* observers[MAX_OBSERVERS];
    int observer_count;
    int state;
};
```

### Subject 구현
```c
void subject_attach(Subject* subject, Observer* observer) {
    if (subject->observer_count < MAX_OBSERVERS) {
        subject->observers[subject->observer_count++] = observer;
        printf("Observer %s attached\n", observer->name);
    }
}

void subject_detach(Subject* subject, Observer* observer) {
    for (int i = 0; i < subject->observer_count; i++) {
        if (subject->observers[i] == observer) {
            // 배열에서 제거
            for (int j = i; j < subject->observer_count - 1; j++) {
                subject->observers[j] = subject->observers[j + 1];
            }
            subject->observer_count--;
            printf("Observer %s detached\n", observer->name);
            break;
        }
    }
}

void subject_notify(Subject* subject) {
    for (int i = 0; i < subject->observer_count; i++) {
        subject->observers[i]->update(subject->observers[i], subject);
    }
}

void subject_set_state(Subject* subject, int new_state) {
    subject->state = new_state;
    subject_notify(subject);  // 상태 변경시 자동 알림
}
```

### 구체적 Observer들
```c
// 디스플레이 옵저버
void display_observer_update(Observer* self, Subject* subject) {
    printf("Display %s: Temperature changed to %d°C\n",
           self->name, subject->state);
}

Observer* create_display_observer(const char* name) {
    Observer* obs = malloc(sizeof(Observer));
    obs->update = display_observer_update;
    obs->name = malloc(strlen(name) + 1);
    strcpy(obs->name, name);
    return obs;
}

// 경고 옵저버
void alert_observer_update(Observer* self, Subject* subject) {
    if (subject->state > 30) {
        printf("Alert %s: WARNING! High temperature: %d°C\n",
               self->name, subject->state);
    } else if (subject->state < 0) {
        printf("Alert %s: WARNING! Freezing temperature: %d°C\n",
               self->name, subject->state);
    }
}
```

## 사용 예제
```c
int main() {
    // Subject 생성 (온도 센서)
    Subject* temperature_sensor = create_subject();

    // 다양한 Observer들 생성
    Observer* display1 = create_display_observer("LCD Display");
    Observer* display2 = create_display_observer("LED Display");
    Observer* alert = create_alert_observer("Temperature Alert");

    // Observer들을 Subject에 등록
    subject_attach(temperature_sensor, display1);
    subject_attach(temperature_sensor, display2);
    subject_attach(temperature_sensor, alert);

    // 온도 변경 - 모든 Observer들이 자동으로 알림받음
    printf("\n--- Setting temperature to 25°C ---\n");
    subject_set_state(temperature_sensor, 25);

    printf("\n--- Setting temperature to 35°C ---\n");
    subject_set_state(temperature_sensor, 35);

    printf("\n--- Setting temperature to -5°C ---\n");
    subject_set_state(temperature_sensor, -5);

    // Observer 분리
    printf("\n--- Detaching LCD Display ---\n");
    subject_detach(temperature_sensor, display1);

    printf("\n--- Setting temperature to 20°C ---\n");
    subject_set_state(temperature_sensor, 20);

    return 0;
}
```

출력 예제:
```
Observer LCD Display attached
Observer LED Display attached
Observer Temperature Alert attached

--- Setting temperature to 25°C ---
Display LCD Display: Temperature changed to 25°C
Display LED Display: Temperature changed to 25°C

--- Setting temperature to 35°C ---
Display LCD Display: Temperature changed to 35°C
Display LED Display: Temperature changed to 35°C
Alert Temperature Alert: WARNING! High temperature: 35°C

Observer LCD Display detached

--- Setting temperature to 20°C ---
Display LED Display: Temperature changed to 20°C
```

## 장점과 단점

### 장점
- **느슨한 결합**: Subject와 Observer 사이의 결합도 최소화
- **동적 관계**: 런타임에 Observer를 추가/제거 가능
- **개방/폐쇄 원칙**: 새로운 Observer 타입을 쉽게 추가
- **브로드캐스트**: 한 번에 여러 객체에게 알림 전송

### 단점
- **예측 불가능한 업데이트**: Observer의 실행 순서 보장 안됨
- **메모리 누수**: Observer 해제를 잊기 쉬움
- **성능**: 많은 Observer가 있을 때 알림 비용 증가
- **복잡한 종속성**: Observer 체인이 복잡해질 수 있음

## 적용 상황
- **모델-뷰 아키텍처**: 데이터 모델 변경시 뷰 업데이트
- **이벤트 관리**: GUI 이벤트 처리 시스템
- **센서 모니터링**: 센서 값 변화 모니터링
- **알림 시스템**: 상태 변화에 따른 다중 알림
- **발행-구독 패턴**: 메시지 시스템
- **캐시 무효화**: 데이터 변경시 캐시 갱신

Observer 패턴은 C 언어에서 함수 포인터를 활용하여 객체 간의 느슨한 결합을 통한 효과적인 알림 시스템을 구현하는 데 매우 유용합니다.