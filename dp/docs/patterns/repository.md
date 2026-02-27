# Repository Pattern

## 개요
Repository 패턴은 데이터 저장소에 대한 접근을 캡슐화하여 도메인 모델과 데이터 매핑 레이어 사이를 분리하는 구조 패턴입니다. 데이터 접근 로직을 중앙화하고 테스트 가능성을 향상시킵니다.

## C 언어 구현
```c
typedef struct User {
    int id;
    char name[100];
    char email[100];
} User;

typedef struct UserRepository {
    User* (*find_by_id)(struct UserRepository* self, int id);
    User* (*find_by_email)(struct UserRepository* self, const char* email);
    int (*save)(struct UserRepository* self, User* user);
    int (*delete)(struct UserRepository* self, int id);
    void (*destroy)(struct UserRepository* self);
} UserRepository;

// 메모리 기반 Repository 구현
typedef struct InMemoryUserRepository {
    UserRepository base;
    User users[100];
    int count;
    int next_id;
} InMemoryUserRepository;

User* memory_find_by_id(UserRepository* self, int id) {
    InMemoryUserRepository* repo = (InMemoryUserRepository*)self;
    for (int i = 0; i < repo->count; i++) {
        if (repo->users[i].id == id) {
            return &repo->users[i];
        }
    }
    return NULL;
}

User* memory_find_by_email(UserRepository* self, const char* email) {
    InMemoryUserRepository* repo = (InMemoryUserRepository*)self;
    for (int i = 0; i < repo->count; i++) {
        if (strcmp(repo->users[i].email, email) == 0) {
            return &repo->users[i];
        }
    }
    return NULL;
}

int memory_save(UserRepository* self, User* user) {
    InMemoryUserRepository* repo = (InMemoryUserRepository*)self;
    if (repo->count >= 100) return 0;

    if (user->id == 0) {
        user->id = ++repo->next_id;
        repo->users[repo->count++] = *user;
        printf("User created with ID: %d\n", user->id);
    } else {
        User* existing = memory_find_by_id(self, user->id);
        if (existing) {
            *existing = *user;
            printf("User updated: %d\n", user->id);
        }
    }
    return 1;
}

int memory_delete(UserRepository* self, int id) {
    InMemoryUserRepository* repo = (InMemoryUserRepository*)self;
    for (int i = 0; i < repo->count; i++) {
        if (repo->users[i].id == id) {
            for (int j = i; j < repo->count - 1; j++) {
                repo->users[j] = repo->users[j + 1];
            }
            repo->count--;
            printf("User deleted: %d\n", id);
            return 1;
        }
    }
    return 0;
}

UserRepository* create_memory_user_repository() {
    InMemoryUserRepository* repo = malloc(sizeof(InMemoryUserRepository));
    repo->base.find_by_id = memory_find_by_id;
    repo->base.find_by_email = memory_find_by_email;
    repo->base.save = memory_save;
    repo->base.delete = memory_delete;
    repo->count = 0;
    repo->next_id = 0;
    return (UserRepository*)repo;
}
```

## 사용 예제
```c
UserRepository* repo = create_memory_user_repository();

// 사용자 생성
User user1 = {0, "Alice", "alice@example.com"};
repo->save(repo, &user1);

User user2 = {0, "Bob", "bob@example.com"};
repo->save(repo, &user2);

// 사용자 조회
User* found = repo->find_by_email(repo, "alice@example.com");
if (found) {
    printf("Found user: %s (ID: %d)\n", found->name, found->id);
}

// 사용자 삭제
repo->delete(repo, user1.id);

// 다시 조회 (없어야 함)
found = repo->find_by_id(repo, user1.id);
printf("User after deletion: %s\n", found ? "Found" : "Not found");
```

## 적용 상황
- **데이터 접근 추상화**: 데이터베이스 구현체를 숨김
- **테스트**: Mock Repository를 사용한 단위 테스트
- **다중 데이터 소스**: 파일, DB, 메모리 등 다양한 저장소 지원
- **도메인 주도 설계**: 도메인과 인프라 분리