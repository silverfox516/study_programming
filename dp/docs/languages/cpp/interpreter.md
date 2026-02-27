# Interpreter Pattern - C++ Implementation

## 개요

Interpreter 패턴은 언어의 문법을 정의하고, 그 문법을 사용하여 언어로 작성된 문장을 해석하는 행위 패턴입니다. 특정 도메인에 특화된 언어(DSL)를 구현할 때 주로 사용됩니다.

## 구조

- **AbstractExpression**: 공통 인터프리터 인터페이스
- **TerminalExpression**: 문법의 말단 표현식
- **NonTerminalExpression**: 문법의 비말단 표현식
- **Context**: 인터프리터에 대한 전역 정보

## C++ 구현

### 1. 기본 수식 해석기

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <map>

// Context class
class Context {
private:
    std::map<std::string, int> variables;
    
public:
    void setVariable(const std::string& name, int value) {
        variables[name] = value;
    }
    
    int getVariable(const std::string& name) const {
        auto it = variables.find(name);
        return (it != variables.end()) ? it->second : 0;
    }
};

// Abstract Expression
class Expression {
public:
    virtual ~Expression() = default;
    virtual int interpret(const Context& context) = 0;
};

// Terminal Expressions
class NumberExpression : public Expression {
private:
    int number;
    
public:
    explicit NumberExpression(int number) : number(number) {}
    
    int interpret(const Context& context) override {
        return number;
    }
};

class VariableExpression : public Expression {
private:
    std::string variableName;
    
public:
    explicit VariableExpression(const std::string& variableName)
        : variableName(variableName) {}
    
    int interpret(const Context& context) override {
        return context.getVariable(variableName);
    }
};

// Non-Terminal Expressions
class AddExpression : public Expression {
private:
    std::unique_ptr<Expression> leftExpression;
    std::unique_ptr<Expression> rightExpression;
    
public:
    AddExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : leftExpression(std::move(left)), rightExpression(std::move(right)) {}
    
    int interpret(const Context& context) override {
        return leftExpression->interpret(context) + rightExpression->interpret(context);
    }
};

class SubtractExpression : public Expression {
private:
    std::unique_ptr<Expression> leftExpression;
    std::unique_ptr<Expression> rightExpression;
    
public:
    SubtractExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : leftExpression(std::move(left)), rightExpression(std::move(right)) {}
    
    int interpret(const Context& context) override {
        return leftExpression->interpret(context) - rightExpression->interpret(context);
    }
};
```

### 2. 기본 SQL 해석기

```cpp
#include <vector>
#include <algorithm>

// Simple SQL-like interpreter
class SQLExpression {
public:
    virtual ~SQLExpression() = default;
    virtual std::vector<std::string> execute(const std::vector<std::vector<std::string>>& table) = 0;
};

class SelectAllExpression : public SQLExpression {
public:
    std::vector<std::string> execute(const std::vector<std::vector<std::string>>& table) override {
        std::vector<std::string> result;
        for (const auto& row : table) {
            std::string rowStr;
            for (const auto& col : row) {
                rowStr += col + "\t";
            }
            result.push_back(rowStr);
        }
        return result;
    }
};

class WhereExpression : public SQLExpression {
private:
    int columnIndex;
    std::string value;
    std::unique_ptr<SQLExpression> baseExpression;
    
public:
    WhereExpression(std::unique_ptr<SQLExpression> base, int colIndex, const std::string& val)
        : baseExpression(std::move(base)), columnIndex(colIndex), value(val) {}
    
    std::vector<std::string> execute(const std::vector<std::vector<std::string>>& table) override {
        std::vector<std::vector<std::string>> filteredTable;
        
        for (const auto& row : table) {
            if (columnIndex < row.size() && row[columnIndex] == value) {
                filteredTable.push_back(row);
            }
        }
        
        return baseExpression->execute(filteredTable);
    }
};
```

## 사용 예제

```cpp
int main() {
    // 1. 수식 해석기 예제
    Context context;
    context.setVariable("x", 10);
    context.setVariable("y", 5);
    
    // x + y - 3 표현식 구성
    auto expression = std::make_unique<SubtractExpression>(
        std::make_unique<AddExpression>(
            std::make_unique<VariableExpression>("x"),
            std::make_unique<VariableExpression>("y")
        ),
        std::make_unique<NumberExpression>(3)
    );
    
    int result = expression->interpret(context);
    std::cout << "Result: " << result << std::endl; // 12
    
    // 2. SQL 해석기 예제
    std::vector<std::vector<std::string>> table = {
        {"John", "25", "Engineer"},
        {"Jane", "30", "Manager"},
        {"Bob", "25", "Developer"}
    };
    
    // SELECT * WHERE age = '25'
    auto sqlQuery = std::make_unique<WhereExpression>(
        std::make_unique<SelectAllExpression>(),
        1, // age column
        "25"
    );
    
    auto queryResult = sqlQuery->execute(table);
    for (const auto& row : queryResult) {
        std::cout << row << std::endl;
    }
    
    return 0;
}
```

## C++의 장점

1. **스마트 포인터**: 안전한 메모리 관리
2. **다형성**: 가상 함수로 유연한 설계
3. **타입 안전성**: 컴파일 타임 체크

## 적용 상황

1. **DSL 구현**: 도메인 특화 언어
2. **설정 파일**: 복잡한 설정 문법
3. **스크립트 엔진**: 간단한 스크립트 언어
4. **콜그룰드 엔진**: 규칙 평가 시스템

Interpreter 패턴은 새로운 언어나 표현식을 쉽게 추가할 수 있게 해주는 유연한 패턴입니다.