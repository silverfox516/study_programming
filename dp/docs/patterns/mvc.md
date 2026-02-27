# MVC (Model-View-Controller) Pattern

## 개요
MVC 패턴은 애플리케이션을 Model, View, Controller 세 가지 구성 요소로 분리하는 아키텍처 패턴입니다. 관심사의 분리를 통해 코드의 재사용성과 유지보수성을 향상시킵니다.

## C 언어 구현
```c
// Model
typedef struct StudentModel {
    char name[100];
    int roll_number;
    void (*set_name)(struct StudentModel* self, const char* name);
    char* (*get_name)(struct StudentModel* self);
    void (*set_roll_number)(struct StudentModel* self, int roll);
    int (*get_roll_number)(struct StudentModel* self);
} StudentModel;

void student_set_name(StudentModel* self, const char* name) {
    strncpy(self->name, name, 99);
    self->name[99] = '\0';
}

char* student_get_name(StudentModel* self) {
    return self->name;
}

void student_set_roll_number(StudentModel* self, int roll) {
    self->roll_number = roll;
}

int student_get_roll_number(StudentModel* self) {
    return self->roll_number;
}

StudentModel* create_student_model() {
    StudentModel* model = malloc(sizeof(StudentModel));
    model->set_name = student_set_name;
    model->get_name = student_get_name;
    model->set_roll_number = student_set_roll_number;
    model->get_roll_number = student_get_roll_number;
    strcpy(model->name, "");
    model->roll_number = 0;
    return model;
}

// View
typedef struct StudentView {
    void (*print_student_details)(struct StudentView* self,
                                  const char* name, int roll_number);
} StudentView;

void view_print_student_details(StudentView* self, const char* name, int roll_number) {
    printf("Student Details:\n");
    printf("Name: %s\n", name);
    printf("Roll Number: %d\n", roll_number);
    printf("-------------------\n");
}

StudentView* create_student_view() {
    StudentView* view = malloc(sizeof(StudentView));
    view->print_student_details = view_print_student_details;
    return view;
}

// Controller
typedef struct StudentController {
    StudentModel* model;
    StudentView* view;
    void (*set_student_name)(struct StudentController* self, const char* name);
    char* (*get_student_name)(struct StudentController* self);
    void (*set_student_roll_number)(struct StudentController* self, int roll);
    int (*get_student_roll_number)(struct StudentController* self);
    void (*update_view)(struct StudentController* self);
} StudentController;

void controller_set_student_name(StudentController* self, const char* name) {
    self->model->set_name(self->model, name);
}

char* controller_get_student_name(StudentController* self) {
    return self->model->get_name(self->model);
}

void controller_set_student_roll_number(StudentController* self, int roll) {
    self->model->set_roll_number(self->model, roll);
}

int controller_get_student_roll_number(StudentController* self) {
    return self->model->get_roll_number(self->model);
}

void controller_update_view(StudentController* self) {
    self->view->print_student_details(self->view,
                                      self->model->get_name(self->model),
                                      self->model->get_roll_number(self->model));
}

StudentController* create_student_controller(StudentModel* model, StudentView* view) {
    StudentController* controller = malloc(sizeof(StudentController));
    controller->model = model;
    controller->view = view;
    controller->set_student_name = controller_set_student_name;
    controller->get_student_name = controller_get_student_name;
    controller->set_student_roll_number = controller_set_student_roll_number;
    controller->get_student_roll_number = controller_get_student_roll_number;
    controller->update_view = controller_update_view;
    return controller;
}
```

## 사용 예제
```c
// Model과 View 생성
StudentModel* model = create_student_model();
StudentView* view = create_student_view();

// Controller 생성
StudentController* controller = create_student_controller(model, view);

// Controller를 통해 Model 업데이트하고 View 갱신
controller->set_student_name(controller, "John Doe");
controller->set_student_roll_number(controller, 12345);
controller->update_view(controller);

// 데이터 변경
controller->set_student_name(controller, "Jane Smith");
controller->set_student_roll_number(controller, 54321);
controller->update_view(controller);
```

## 적용 상황
- **웹 애플리케이션**: HTTP 요청 처리와 응답 생성
- **GUI 애플리케이션**: 사용자 인터페이스와 비즈니스 로직 분리
- **게임**: 게임 상태, 렌더링, 입력 처리 분리
- **임베디드 시스템**: 센서 데이터, 디스플레이, 제어 로직 분리