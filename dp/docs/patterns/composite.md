# Composite Pattern

## 개요
Composite 패턴은 객체들을 트리 구조로 구성하여 부분-전체 계층을 표현하는 구조 패턴입니다. 이 패턴을 사용하면 개별 객체와 복합 객체를 동일하게 다룰 수 있으며, 클라이언트에서 단일 객체와 복합 객체의 차이점을 의식하지 않고 사용할 수 있습니다.

## 구조
- **Component**: 모든 구성 요소들에 대한 추상화된 인터페이스 정의
- **Leaf**: 트리의 잎 노드로, 자식 노드가 없는 객체
- **Composite**: 자식 노드들을 가질 수 있는 복합 객체
- **Client**: Component 인터페이스를 통해 트리의 객체들을 다루는 코드

## C 언어 구현

### 컴포넌트 인터페이스 정의
```c
// 컴포넌트 인터페이스
typedef struct Component {
    char* name;
    void (*draw)(struct Component* self, int indent);
    void (*add)(struct Component* self, struct Component* child);
    void (*remove)(struct Component* self, struct Component* child);
    void (*destroy)(struct Component* self);
} Component;
```

### 리프(Leaf) 구현
```c
// 리프 - 단일 요소 (파일)
typedef struct {
    Component base;
} Leaf;

void leaf_draw(Component* self, int indent) {
    for (int i = 0; i < indent; i++) printf("  ");
    printf("- %s (Leaf)\n", self->name);
}

void leaf_add(Component* self, Component* child) {
    printf("Cannot add child to leaf node: %s\n", self->name);
}

void leaf_remove(Component* self, Component* child) {
    printf("Cannot remove child from leaf node: %s\n", self->name);
}

void leaf_destroy(Component* self) {
    if (self->name) {
        free(self->name);
    }
    free(self);
}

Component* leaf_create(const char* name) {
    Leaf* leaf = malloc(sizeof(Leaf));
    leaf->base.name = malloc(strlen(name) + 1);
    strcpy(leaf->base.name, name);

    leaf->base.draw = leaf_draw;
    leaf->base.add = leaf_add;
    leaf->base.remove = leaf_remove;
    leaf->base.destroy = leaf_destroy;

    return (Component*)leaf;
}
```

### 컴포지트(Composite) 구현
```c
// 컴포지트 - 자식을 가질 수 있는 복합 객체 (디렉토리)
typedef struct {
    Component base;
    Component** children;  // 동적 배열
    int child_count;
    int capacity;
} Composite;

void composite_draw(Component* self, int indent) {
    Composite* comp = (Composite*)self;

    // 자신을 먼저 그리기
    for (int i = 0; i < indent; i++) printf("  ");
    printf("+ %s (Composite)\n", self->name);

    // 모든 자식들 그리기
    for (int i = 0; i < comp->child_count; i++) {
        comp->children[i]->draw(comp->children[i], indent + 1);
    }
}

void composite_add(Component* self, Component* child) {
    Composite* comp = (Composite*)self;

    // 용량 확장이 필요한 경우
    if (comp->child_count >= comp->capacity) {
        comp->capacity *= 2;
        comp->children = realloc(comp->children, comp->capacity * sizeof(Component*));
    }

    comp->children[comp->child_count++] = child;
    printf("Added '%s' to composite '%s'\n", child->name, self->name);
}

void composite_remove(Component* self, Component* child) {
    Composite* comp = (Composite*)self;

    for (int i = 0; i < comp->child_count; i++) {
        if (comp->children[i] == child) {
            // 나머지 요소들 시프트
            for (int j = i; j < comp->child_count - 1; j++) {
                comp->children[j] = comp->children[j + 1];
            }
            comp->child_count--;
            printf("Removed '%s' from composite '%s'\n", child->name, self->name);
            return;
        }
    }
    printf("Child '%s' not found in composite '%s'\n", child->name, self->name);
}

void composite_destroy(Component* self) {
    Composite* comp = (Composite*)self;

    // 모든 자식들 재귀적으로 파괴
    for (int i = 0; i < comp->child_count; i++) {
        comp->children[i]->destroy(comp->children[i]);
    }

    if (comp->children) {
        free(comp->children);
    }
    if (self->name) {
        free(self->name);
    }
    free(comp);
}

Component* composite_create(const char* name) {
    Composite* comp = malloc(sizeof(Composite));
    comp->base.name = malloc(strlen(name) + 1);
    strcpy(comp->base.name, name);

    comp->base.draw = composite_draw;
    comp->base.add = composite_add;
    comp->base.remove = composite_remove;
    comp->base.destroy = composite_destroy;

    comp->children = malloc(4 * sizeof(Component*));
    comp->child_count = 0;
    comp->capacity = 4;

    return (Component*)comp;
}
```

## 사용 예제

### 파일 시스템 예제
```c
int main() {
    printf("=== File System Example ===\n");

    // 루트 디렉토리 생성
    Component* root = composite_create("root");

    // 서브 디렉토리 생성
    Component* documents = composite_create("Documents");
    Component* pictures = composite_create("Pictures");
    Component* work = composite_create("Work");

    // 파일 생성 (리프)
    Component* resume = leaf_create("resume.pdf");
    Component* photo1 = leaf_create("vacation.jpg");
    Component* photo2 = leaf_create("family.png");
    Component* project = leaf_create("project.docx");
    Component* report = leaf_create("report.xlsx");

    // 구조 구축
    root->add(root, documents);
    root->add(root, pictures);

    documents->add(documents, resume);
    documents->add(documents, work);

    pictures->add(pictures, photo1);
    pictures->add(pictures, photo2);

    work->add(work, project);
    work->add(work, report);

    // 전체 구조 출력
    printf("\nFile system structure:\n");
    root->draw(root, 0);

    // 파일 제거
    printf("\nRemoving vacation.jpg from Pictures:\n");
    pictures->remove(pictures, photo1);

    printf("\nUpdated structure:\n");
    root->draw(root, 0);

    return 0;
}
```

### 그래픽 객체 예제
```c
// 그래픽 예제
Component* canvas = composite_create("Canvas");
Component* shape_group = composite_create("ShapeGroup");

Component* circle = leaf_create("Circle");
Component* rectangle = leaf_create("Rectangle");
Component* line = leaf_create("Line");
Component* triangle = leaf_create("Triangle");

// 구조 구성
canvas->add(canvas, shape_group);
canvas->add(canvas, line);

shape_group->add(shape_group, circle);
shape_group->add(shape_group, rectangle);
shape_group->add(shape_group, triangle);

// 전체 그리기
canvas->draw(canvas, 0);
```

출력 예제:
```
+ root (Composite)
  + Documents (Composite)
    - resume.pdf (Leaf)
    + Work (Composite)
      - project.docx (Leaf)
      - report.xlsx (Leaf)
  + Pictures (Composite)
    - family.png (Leaf)

+ Canvas (Composite)
  + ShapeGroup (Composite)
    - Circle (Leaf)
    - Rectangle (Leaf)
    - Triangle (Leaf)
  - Line (Leaf)
```

## 장점과 단점

### 장점
- **일관된 인터페이스**: 단일 객체와 복합 객체를 동일하게 처리
- **재귀 구조**: 복잡한 트리 구조를 자연스럽게 표현
- **확장성**: 새로운 컴포넌트 타입을 쉽게 추가
- **클라이언트 단순화**: 객체의 구성에 대해 신경 쓸 필요 없음
- **유연성**: 런타임에 트리 구조를 동적으로 변경 가능

### 단점
- **과도한 일반화**: 단순한 경우에도 복잡한 구조 필요
- **타입 안전성**: 컴포넌트 타입을 런타임에 검사해야 할 수 있음
- **메모리 오버헤드**: 포인터와 구조체로 인한 추가 메모리 사용
- **성능**: 재귀 호출로 인한 성능 오버헤드

## 적용 상황
- **파일 시스템**: 디렉토리와 파일의 계층 구조
- **GUI 컴포넌트**: 윈도우, 패널, 버튼 등의 중첩 구조
- **그래픽 객체**: 그래픽 요소들의 그룹화와 변환
- **조직도**: 회사나 팀의 계층적 구조
- **수식 파싱**: 수학 표현식의 트리 구조
- **메뉴 시스템**: 중첩된 메뉴와 메뉴 항목
- **문서 구조**: 문단, 섹션, 챕터 등의 계층 구조
- **게임 씬 그래프**: 게임 객체들의 부모-자식 관계

Composite 패턴은 C 언어에서 함수 포인터와 동적 메모리 할당을 활용하여 복잡한 트리 구조를 효과적으로 관리할 수 있는 강력한 패턴입니다.