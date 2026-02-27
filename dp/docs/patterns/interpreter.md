# Interpreter Pattern

## 개요
Interpreter 패턴은 언어에 대해 문법 표현을 정의하고 그 문법을 사용하여 언어의 문장을 해석하는 해석자를 제공하는 행위 패턴입니다.

## C 언어 구현
```c
typedef struct Expression Expression;

struct Expression {
    int (*interpret)(Expression* self);
    void (*destroy)(Expression* self);
};

// Terminal Expression - 숫자
typedef struct NumberExpression {
    Expression base;
    int number;
} NumberExpression;

int number_interpret(Expression* self) {
    NumberExpression* num_expr = (NumberExpression*)self;
    return num_expr->number;
}

NumberExpression* create_number_expression(int number) {
    NumberExpression* expr = malloc(sizeof(NumberExpression));
    expr->base.interpret = number_interpret;
    expr->number = number;
    return expr;
}

// Non-terminal Expression - 덧셈
typedef struct AddExpression {
    Expression base;
    Expression* left;
    Expression* right;
} AddExpression;

int add_interpret(Expression* self) {
    AddExpression* add_expr = (AddExpression*)self;
    return add_expr->left->interpret(add_expr->left) +
           add_expr->right->interpret(add_expr->right);
}

AddExpression* create_add_expression(Expression* left, Expression* right) {
    AddExpression* expr = malloc(sizeof(AddExpression));
    expr->base.interpret = add_interpret;
    expr->left = left;
    expr->right = right;
    return expr;
}

// Non-terminal Expression - 뺄셈
typedef struct SubtractExpression {
    Expression base;
    Expression* left;
    Expression* right;
} SubtractExpression;

int subtract_interpret(Expression* self) {
    SubtractExpression* sub_expr = (SubtractExpression*)self;
    return sub_expr->left->interpret(sub_expr->left) -
           sub_expr->right->interpret(sub_expr->right);
}
```

## 사용 예제
```c
// (10 + 5) - (3 + 2) = 10 구성
Expression* ten = (Expression*)create_number_expression(10);
Expression* five = (Expression*)create_number_expression(5);
Expression* three = (Expression*)create_number_expression(3);
Expression* two = (Expression*)create_number_expression(2);

Expression* left_add = (Expression*)create_add_expression(ten, five);      // 10 + 5
Expression* right_add = (Expression*)create_add_expression(three, two);    // 3 + 2
Expression* result = (Expression*)create_subtract_expression(left_add, right_add); // (10+5) - (3+2)

printf("Result: %d\n", result->interpret(result));  // 10
```

## 적용 상황
- **간단한 스크립트 언어**: 도메인 특화 언어 구현
- **수식 계산기**: 수학 표현식 파싱 및 계산
- **설정 파일 파서**: 설정 언어 해석
- **SQL 쿼리 해석**: 간단한 쿼리 언어 구현