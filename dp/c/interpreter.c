#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Context for storing variables
typedef struct {
    char** variable_names;
    int* variable_values;
    int variable_count;
    int capacity;
} Context;

Context* context_create() {
    Context* context = malloc(sizeof(Context));
    context->variable_names = malloc(10 * sizeof(char*));
    context->variable_values = malloc(10 * sizeof(int));
    context->variable_count = 0;
    context->capacity = 10;
    return context;
}

void context_set_variable(Context* context, const char* name, int value) {
    // Check if variable already exists
    for (int i = 0; i < context->variable_count; i++) {
        if (strcmp(context->variable_names[i], name) == 0) {
            context->variable_values[i] = value;
            printf("Variable '%s' updated to %d\n", name, value);
            return;
        }
    }

    // Add new variable
    if (context->variable_count >= context->capacity) {
        context->capacity *= 2;
        context->variable_names = realloc(context->variable_names, context->capacity * sizeof(char*));
        context->variable_values = realloc(context->variable_values, context->capacity * sizeof(int));
    }

    context->variable_names[context->variable_count] = malloc(strlen(name) + 1);
    strcpy(context->variable_names[context->variable_count], name);
    context->variable_values[context->variable_count] = value;
    context->variable_count++;

    printf("Variable '%s' set to %d\n", name, value);
}

int context_get_variable(Context* context, const char* name) {
    for (int i = 0; i < context->variable_count; i++) {
        if (strcmp(context->variable_names[i], name) == 0) {
            return context->variable_values[i];
        }
    }
    printf("Error: Variable '%s' not found\n", name);
    return 0;
}

void context_destroy(Context* context) {
    for (int i = 0; i < context->variable_count; i++) {
        free(context->variable_names[i]);
    }
    free(context->variable_names);
    free(context->variable_values);
    free(context);
}

// Abstract Expression interface
typedef struct Expression {
    int (*interpret)(struct Expression* self, Context* context);
    void (*destroy)(struct Expression* self);
} Expression;

// Terminal Expression - Number
typedef struct {
    Expression base;
    int value;
} NumberExpression;

int number_interpret(Expression* self, Context* context) {
    NumberExpression* expr = (NumberExpression*)self;
    return expr->value;
}

void number_destroy(Expression* self) {
    free(self);
}

Expression* number_expression_create(int value) {
    NumberExpression* expr = malloc(sizeof(NumberExpression));
    expr->base.interpret = number_interpret;
    expr->base.destroy = number_destroy;
    expr->value = value;
    return (Expression*)expr;
}

// Terminal Expression - Variable
typedef struct {
    Expression base;
    char* name;
} VariableExpression;

int variable_interpret(Expression* self, Context* context) {
    VariableExpression* expr = (VariableExpression*)self;
    return context_get_variable(context, expr->name);
}

void variable_destroy(Expression* self) {
    VariableExpression* expr = (VariableExpression*)self;
    if (expr->name) {
        free(expr->name);
    }
    free(expr);
}

Expression* variable_expression_create(const char* name) {
    VariableExpression* expr = malloc(sizeof(VariableExpression));
    expr->base.interpret = variable_interpret;
    expr->base.destroy = variable_destroy;

    expr->name = malloc(strlen(name) + 1);
    strcpy(expr->name, name);

    return (Expression*)expr;
}

// Non-terminal Expression - Addition
typedef struct {
    Expression base;
    Expression* left;
    Expression* right;
} AddExpression;

int add_interpret(Expression* self, Context* context) {
    AddExpression* expr = (AddExpression*)self;
    int left_value = expr->left->interpret(expr->left, context);
    int right_value = expr->right->interpret(expr->right, context);
    int result = left_value + right_value;
    printf("  %d + %d = %d\n", left_value, right_value, result);
    return result;
}

void add_destroy(Expression* self) {
    AddExpression* expr = (AddExpression*)self;
    expr->left->destroy(expr->left);
    expr->right->destroy(expr->right);
    free(expr);
}

Expression* add_expression_create(Expression* left, Expression* right) {
    AddExpression* expr = malloc(sizeof(AddExpression));
    expr->base.interpret = add_interpret;
    expr->base.destroy = add_destroy;
    expr->left = left;
    expr->right = right;
    return (Expression*)expr;
}

// Non-terminal Expression - Subtraction
typedef struct {
    Expression base;
    Expression* left;
    Expression* right;
} SubtractExpression;

int subtract_interpret(Expression* self, Context* context) {
    SubtractExpression* expr = (SubtractExpression*)self;
    int left_value = expr->left->interpret(expr->left, context);
    int right_value = expr->right->interpret(expr->right, context);
    int result = left_value - right_value;
    printf("  %d - %d = %d\n", left_value, right_value, result);
    return result;
}

void subtract_destroy(Expression* self) {
    SubtractExpression* expr = (SubtractExpression*)self;
    expr->left->destroy(expr->left);
    expr->right->destroy(expr->right);
    free(expr);
}

Expression* subtract_expression_create(Expression* left, Expression* right) {
    SubtractExpression* expr = malloc(sizeof(SubtractExpression));
    expr->base.interpret = subtract_interpret;
    expr->base.destroy = subtract_destroy;
    expr->left = left;
    expr->right = right;
    return (Expression*)expr;
}

// Non-terminal Expression - Multiplication
typedef struct {
    Expression base;
    Expression* left;
    Expression* right;
} MultiplyExpression;

int multiply_interpret(Expression* self, Context* context) {
    MultiplyExpression* expr = (MultiplyExpression*)self;
    int left_value = expr->left->interpret(expr->left, context);
    int right_value = expr->right->interpret(expr->right, context);
    int result = left_value * right_value;
    printf("  %d * %d = %d\n", left_value, right_value, result);
    return result;
}

void multiply_destroy(Expression* self) {
    MultiplyExpression* expr = (MultiplyExpression*)self;
    expr->left->destroy(expr->left);
    expr->right->destroy(expr->right);
    free(expr);
}

Expression* multiply_expression_create(Expression* left, Expression* right) {
    MultiplyExpression* expr = malloc(sizeof(MultiplyExpression));
    expr->base.interpret = multiply_interpret;
    expr->base.destroy = multiply_destroy;
    expr->left = left;
    expr->right = right;
    return (Expression*)expr;
}

// Simple Calculator Parser
typedef struct {
    char* input;
    int position;
    int length;
} Parser;

Parser* parser_create(const char* input) {
    Parser* parser = malloc(sizeof(Parser));
    parser->input = malloc(strlen(input) + 1);
    strcpy(parser->input, input);
    parser->position = 0;
    parser->length = strlen(input);
    return parser;
}

void parser_skip_whitespace(Parser* parser) {
    while (parser->position < parser->length && isspace(parser->input[parser->position])) {
        parser->position++;
    }
}

char parser_peek(Parser* parser) {
    parser_skip_whitespace(parser);
    if (parser->position < parser->length) {
        return parser->input[parser->position];
    }
    return '\0';
}

char parser_consume(Parser* parser) {
    parser_skip_whitespace(parser);
    if (parser->position < parser->length) {
        return parser->input[parser->position++];
    }
    return '\0';
}

int parser_parse_number(Parser* parser) {
    int result = 0;
    while (parser->position < parser->length && isdigit(parser->input[parser->position])) {
        result = result * 10 + (parser->input[parser->position] - '0');
        parser->position++;
    }
    return result;
}

char* parser_parse_variable(Parser* parser) {
    int start = parser->position;
    while (parser->position < parser->length &&
           (isalnum(parser->input[parser->position]) || parser->input[parser->position] == '_')) {
        parser->position++;
    }

    int length = parser->position - start;
    char* variable = malloc(length + 1);
    strncpy(variable, &parser->input[start], length);
    variable[length] = '\0';

    return variable;
}

Expression* parser_parse_factor(Parser* parser);
Expression* parser_parse_term(Parser* parser);
Expression* parser_parse_expression(Parser* parser);

Expression* parser_parse_factor(Parser* parser) {
    char ch = parser_peek(parser);

    if (ch == '(') {
        parser_consume(parser); // consume '('
        Expression* expr = parser_parse_expression(parser);
        parser_consume(parser); // consume ')'
        return expr;
    } else if (isdigit(ch)) {
        int value = parser_parse_number(parser);
        return number_expression_create(value);
    } else if (isalpha(ch)) {
        char* variable = parser_parse_variable(parser);
        Expression* expr = variable_expression_create(variable);
        free(variable);
        return expr;
    }

    printf("Error: Unexpected character '%c'\n", ch);
    return NULL;
}

Expression* parser_parse_term(Parser* parser) {
    Expression* left = parser_parse_factor(parser);

    while (true) {
        char op = parser_peek(parser);
        if (op == '*') {
            parser_consume(parser);
            Expression* right = parser_parse_factor(parser);
            left = multiply_expression_create(left, right);
        } else {
            break;
        }
    }

    return left;
}

Expression* parser_parse_expression(Parser* parser) {
    Expression* left = parser_parse_term(parser);

    while (true) {
        char op = parser_peek(parser);
        if (op == '+') {
            parser_consume(parser);
            Expression* right = parser_parse_term(parser);
            left = add_expression_create(left, right);
        } else if (op == '-') {
            parser_consume(parser);
            Expression* right = parser_parse_term(parser);
            left = subtract_expression_create(left, right);
        } else {
            break;
        }
    }

    return left;
}

void parser_destroy(Parser* parser) {
    if (parser->input) {
        free(parser->input);
    }
    free(parser);
}

// Boolean Expression Interpreter
typedef enum {
    TRUE_EXPR,
    FALSE_EXPR,
    AND_EXPR,
    OR_EXPR,
    NOT_EXPR,
    VAR_EXPR
} BooleanExprType;

typedef struct BooleanExpression {
    BooleanExprType type;
    bool (*evaluate)(struct BooleanExpression* self, Context* context);
    void (*destroy)(struct BooleanExpression* self);
} BooleanExpression;

// Boolean True Expression
typedef struct {
    BooleanExpression base;
} TrueExpression;

bool true_evaluate(BooleanExpression* self, Context* context) {
    return true;
}

void true_destroy(BooleanExpression* self) {
    free(self);
}

BooleanExpression* true_expression_create() {
    TrueExpression* expr = malloc(sizeof(TrueExpression));
    expr->base.type = TRUE_EXPR;
    expr->base.evaluate = true_evaluate;
    expr->base.destroy = true_destroy;
    return (BooleanExpression*)expr;
}

// Boolean False Expression
typedef struct {
    BooleanExpression base;
} FalseExpression;

bool false_evaluate(BooleanExpression* self, Context* context) {
    return false;
}

void false_destroy(BooleanExpression* self) {
    free(self);
}

BooleanExpression* false_expression_create() {
    FalseExpression* expr = malloc(sizeof(FalseExpression));
    expr->base.type = FALSE_EXPR;
    expr->base.evaluate = false_evaluate;
    expr->base.destroy = false_destroy;
    return (BooleanExpression*)expr;
}

// Boolean AND Expression
typedef struct {
    BooleanExpression base;
    BooleanExpression* left;
    BooleanExpression* right;
} AndExpression;

bool and_evaluate(BooleanExpression* self, Context* context) {
    AndExpression* expr = (AndExpression*)self;
    bool left_val = expr->left->evaluate(expr->left, context);
    bool right_val = expr->right->evaluate(expr->right, context);
    bool result = left_val && right_val;
    printf("  %s AND %s = %s\n",
           left_val ? "true" : "false",
           right_val ? "true" : "false",
           result ? "true" : "false");
    return result;
}

void and_destroy(BooleanExpression* self) {
    AndExpression* expr = (AndExpression*)self;
    expr->left->destroy(expr->left);
    expr->right->destroy(expr->right);
    free(expr);
}

BooleanExpression* and_expression_create(BooleanExpression* left, BooleanExpression* right) {
    AndExpression* expr = malloc(sizeof(AndExpression));
    expr->base.type = AND_EXPR;
    expr->base.evaluate = and_evaluate;
    expr->base.destroy = and_destroy;
    expr->left = left;
    expr->right = right;
    return (BooleanExpression*)expr;
}

// Boolean OR Expression
typedef struct {
    BooleanExpression base;
    BooleanExpression* left;
    BooleanExpression* right;
} OrExpression;

bool or_evaluate(BooleanExpression* self, Context* context) {
    OrExpression* expr = (OrExpression*)self;
    bool left_val = expr->left->evaluate(expr->left, context);
    bool right_val = expr->right->evaluate(expr->right, context);
    bool result = left_val || right_val;
    printf("  %s OR %s = %s\n",
           left_val ? "true" : "false",
           right_val ? "true" : "false",
           result ? "true" : "false");
    return result;
}

void or_destroy(BooleanExpression* self) {
    OrExpression* expr = (OrExpression*)self;
    expr->left->destroy(expr->left);
    expr->right->destroy(expr->right);
    free(expr);
}

BooleanExpression* or_expression_create(BooleanExpression* left, BooleanExpression* right) {
    OrExpression* expr = malloc(sizeof(OrExpression));
    expr->base.type = OR_EXPR;
    expr->base.evaluate = or_evaluate;
    expr->base.destroy = or_destroy;
    expr->left = left;
    expr->right = right;
    return (BooleanExpression*)expr;
}

// Client code
int main() {
    printf("=== Interpreter Pattern Demo - Mathematical Expressions ===\n\n");

    // Create context and set some variables
    Context* context = context_create();
    context_set_variable(context, "x", 10);
    context_set_variable(context, "y", 5);
    context_set_variable(context, "z", 3);
    printf("\n");

    // Example 1: Manual expression construction
    printf("=== Manual Expression Construction ===\n");
    printf("Expression: (x + y) * z\n");

    Expression* var_x = variable_expression_create("x");
    Expression* var_y = variable_expression_create("y");
    Expression* var_z = variable_expression_create("z");

    Expression* add_xy = add_expression_create(var_x, var_y);
    Expression* multiply_result = multiply_expression_create(add_xy, var_z);

    printf("Evaluating:\n");
    int result = multiply_result->interpret(multiply_result, context);
    printf("Final result: %d\n\n", result);

    multiply_result->destroy(multiply_result);

    // Example 2: Using parser
    printf("=== Using Expression Parser ===\n");

    const char* expressions[] = {
        "10 + 5",
        "x * y",
        "x + y - z",
        "10 * 2 + 5",
        "(x + y) * z"
    };

    int num_expressions = sizeof(expressions) / sizeof(expressions[0]);

    for (int i = 0; i < num_expressions; i++) {
        printf("Expression: %s\n", expressions[i]);

        Parser* parser = parser_create(expressions[i]);
        Expression* expr = parser_parse_expression(parser);

        if (expr) {
            printf("Evaluating:\n");
            int result = expr->interpret(expr, context);
            printf("Result: %d\n", result);
            expr->destroy(expr);
        }

        parser_destroy(parser);
        printf("---\n");
    }

    // Example 3: Boolean expressions
    printf("\n=== Boolean Expression Interpreter ===\n");

    // Create boolean expressions: true AND (false OR true)
    BooleanExpression* true_expr = true_expression_create();
    BooleanExpression* false_expr = false_expression_create();
    BooleanExpression* true_expr2 = true_expression_create();

    BooleanExpression* or_expr = or_expression_create(false_expr, true_expr2);
    BooleanExpression* and_expr = and_expression_create(true_expr, or_expr);

    printf("Expression: true AND (false OR true)\n");
    printf("Evaluating:\n");
    bool boolean_result = and_expr->evaluate(and_expr, context);
    printf("Final result: %s\n", boolean_result ? "true" : "false");

    and_expr->destroy(and_expr);

    // Example 4: Command processing
    printf("\n=== Command Processing Example ===\n");
    printf("Processing variable assignments and calculations:\n");

    const char* commands[] = {
        "a = 20",
        "b = 15",
        "result = a + b * 2"
    };

    for (int i = 0; i < 3; i++) {
        printf("Command: %s\n", commands[i]);

        // Simple command parsing (for demonstration)
        if (strstr(commands[i], "=")) {
            char var_name[10];
            char expr_str[50];
            sscanf(commands[i], "%s = %[^\n]", var_name, expr_str);

            if (isdigit(expr_str[0])) {
                // Simple number assignment
                int value = atoi(expr_str);
                context_set_variable(context, var_name, value);
            } else {
                // Expression assignment
                Parser* parser = parser_create(expr_str);
                Expression* expr = parser_parse_expression(parser);
                if (expr) {
                    int value = expr->interpret(expr, context);
                    context_set_variable(context, var_name, value);
                    expr->destroy(expr);
                }
                parser_destroy(parser);
            }
        }
        printf("---\n");
    }

    context_destroy(context);

    printf("\n=== Interpreter Pattern Benefits ===\n");
    printf("1. Easy to change grammar: Add new expression types easily\n");
    printf("2. Easy to implement: Each grammar rule is a class\n");
    printf("3. Complex grammars: Can handle recursive and nested structures\n");
    printf("4. Extensible: New operations can be added to expressions\n");

    return 0;
}