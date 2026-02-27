# Mediator Pattern

## 개요
Mediator 패턴은 객체들 간의 상호작용을 캡슐화하여 객체들이 서로 직접 참조하지 않도록 하는 행위 패턴입니다. 느슨한 결합을 촉진하고 상호작용 로직을 한 곳에 집중시킵니다.

## C 언어 구현
```c
typedef struct Component Component;
typedef struct Mediator Mediator;

struct Mediator {
    void (*notify)(Mediator* self, Component* sender, const char* event);
};

struct Component {
    Mediator* mediator;
    char* name;
};

// 채팅 메디에이터
typedef struct ChatMediator {
    Mediator base;
    Component* users[10];
    int user_count;
} ChatMediator;

void chat_mediator_notify(Mediator* self, Component* sender, const char* event) {
    ChatMediator* mediator = (ChatMediator*)self;
    printf("[MEDIATOR] %s sent: %s\n", sender->name, event);

    // 모든 사용자에게 메시지 전달 (발신자 제외)
    for (int i = 0; i < mediator->user_count; i++) {
        if (mediator->users[i] != sender) {
            printf("  -> Delivered to %s\n", mediator->users[i]->name);
        }
    }
}

ChatMediator* create_chat_mediator() {
    ChatMediator* mediator = malloc(sizeof(ChatMediator));
    mediator->base.notify = chat_mediator_notify;
    mediator->user_count = 0;
    return mediator;
}

// 사용자 컴포넌트
typedef struct User {
    Component base;
} User;

void user_send_message(Component* self, const char* message) {
    printf("%s: %s\n", self->name, message);
    self->mediator->notify(self->mediator, self, message);
}

User* create_user(const char* name, Mediator* mediator) {
    User* user = malloc(sizeof(User));
    user->base.mediator = mediator;
    user->base.name = malloc(strlen(name) + 1);
    strcpy(user->base.name, name);

    // 메디에이터에 등록
    ChatMediator* chat_med = (ChatMediator*)mediator;
    chat_med->users[chat_med->user_count++] = (Component*)user;

    return user;
}
```

## 사용 예제
```c
ChatMediator* chat = create_chat_mediator();

User* alice = create_user("Alice", (Mediator*)chat);
User* bob = create_user("Bob", (Mediator*)chat);
User* charlie = create_user("Charlie", (Mediator*)chat);

user_send_message((Component*)alice, "Hello everyone!");
user_send_message((Component*)bob, "Hi Alice!");
user_send_message((Component*)charlie, "Hey there!");
```

## 적용 상황
- **채팅 시스템**: 사용자들 간의 메시지 전달
- **GUI 컴포넌트**: 다양한 UI 요소들 간의 상호작용
- **항공 교통 관제**: 비행기들 간의 통신 중재
- **워크플로우**: 다양한 작업 단계들 간의 조정