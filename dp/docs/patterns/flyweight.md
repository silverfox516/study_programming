# Flyweight Pattern

## 개요
Flyweight 패턴은 많은 수의 유사한 객체를 효율적으로 지원하기 위한 구조 패턴입니다. 객체의 내재적 상태를 공유하여 메모리 사용량을 최소화합니다.

## C 언어 구현
```c
// Flyweight 인터페이스
typedef struct TreeType {
    char* name;
    char* color;
    char* sprite;
    void (*render)(struct TreeType* self, int x, int y);
} TreeType;

void tree_type_render(TreeType* self, int x, int y) {
    printf("Rendering %s %s tree at (%d, %d)\n", self->color, self->name, x, y);
}

// Flyweight Factory
#define MAX_TREE_TYPES 100
TreeType* tree_types[MAX_TREE_TYPES];
int tree_type_count = 0;

TreeType* get_tree_type(const char* name, const char* color, const char* sprite) {
    // 기존에 있는지 검색
    for (int i = 0; i < tree_type_count; i++) {
        if (strcmp(tree_types[i]->name, name) == 0 &&
            strcmp(tree_types[i]->color, color) == 0) {
            return tree_types[i];  // 기존 객체 반환
        }
    }

    // 새로 생성
    TreeType* tree_type = malloc(sizeof(TreeType));
    tree_type->name = malloc(strlen(name) + 1);
    strcpy(tree_type->name, name);
    tree_type->color = malloc(strlen(color) + 1);
    strcpy(tree_type->color, color);
    tree_type->render = tree_type_render;

    tree_types[tree_type_count++] = tree_type;
    printf("Created new TreeType: %s %s\n", color, name);
    return tree_type;
}

// Context (외재적 상태)
typedef struct Tree {
    int x, y;
    TreeType* type;  // Flyweight 참조
} Tree;

Tree* create_tree(int x, int y, const char* name, const char* color) {
    Tree* tree = malloc(sizeof(Tree));
    tree->x = x;
    tree->y = y;
    tree->type = get_tree_type(name, color, "sprite.png");
    return tree;
}

void tree_render(Tree* self) {
    self->type->render(self->type, self->x, self->y);
}
```

## 사용 예제
```c
Tree* trees[1000];
int tree_count = 0;

// 많은 나무 생성 (같은 타입은 공유됨)
trees[tree_count++] = create_tree(10, 20, "Oak", "Green");
trees[tree_count++] = create_tree(50, 60, "Oak", "Green");  // 같은 타입 재사용
trees[tree_count++] = create_tree(30, 40, "Pine", "Green");
trees[tree_count++] = create_tree(70, 80, "Oak", "Green");  // 같은 타입 재사용

printf("Total TreeType objects: %d\n", tree_type_count);  // 2개만 생성됨
printf("Total Tree objects: %d\n", tree_count);           // 4개 생성됨

for (int i = 0; i < tree_count; i++) {
    tree_render(trees[i]);
}
```

## 적용 상황
- **게임**: 많은 수의 유사한 게임 객체 (총알, 파티클, 타일)
- **텍스트 에디터**: 문자 객체의 폰트, 크기 정보 공유
- **GUI**: 아이콘, 버튼 등의 시각적 요소 공유