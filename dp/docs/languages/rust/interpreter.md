# Interpreter Pattern - Rust Implementation

## 개요

Interpreter 패턴은 언어의 문법을 정의하고 그 언어로 작성된 문장들을 해석하는 해석자를 제공합니다. 간단한 언어나 표현식을 파싱하고 실행할 때 사용됩니다.

## 구조

```rust
// Abstract Expression
pub trait Expression {
    fn interpret(&self, context: &Context) -> Result<i32, String>;
    fn to_string(&self) -> String;
}

// Context
pub struct Context {
    variables: HashMap<String, i32>,
}

impl Context {
    pub fn new() -> Self {
        Self {
            variables: HashMap::new(),
        }
    }

    pub fn set_variable(&mut self, name: String, value: i32) {
        self.variables.insert(name, value);
    }

    pub fn get_variable(&self, name: &str) -> Option<i32> {
        self.variables.get(name).copied()
    }
}

// Terminal Expression - Number
pub struct NumberExpression {
    value: i32,
}

impl NumberExpression {
    pub fn new(value: i32) -> Self {
        Self { value }
    }
}

impl Expression for NumberExpression {
    fn interpret(&self, _context: &Context) -> Result<i32, String> {
        Ok(self.value)
    }

    fn to_string(&self) -> String {
        self.value.to_string()
    }
}

// Terminal Expression - Variable
pub struct VariableExpression {
    name: String,
}

impl VariableExpression {
    pub fn new(name: String) -> Self {
        Self { name }
    }
}

impl Expression for VariableExpression {
    fn interpret(&self, context: &Context) -> Result<i32, String> {
        context.get_variable(&self.name)
            .ok_or_else(|| format!("Variable '{}' not found", self.name))
    }

    fn to_string(&self) -> String {
        self.name.clone()
    }
}

// Non-terminal Expression - Addition
pub struct AddExpression {
    left: Box<dyn Expression>,
    right: Box<dyn Expression>,
}

impl AddExpression {
    pub fn new(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Self {
        Self { left, right }
    }
}

impl Expression for AddExpression {
    fn interpret(&self, context: &Context) -> Result<i32, String> {
        let left_val = self.left.interpret(context)?;
        let right_val = self.right.interpret(context)?;
        Ok(left_val + right_val)
    }

    fn to_string(&self) -> String {
        format!("({} + {})", self.left.to_string(), self.right.to_string())
    }
}

// Parser
pub struct ExpressionParser;

impl ExpressionParser {
    pub fn parse(expression: &str) -> Result<Box<dyn Expression>, String> {
        let tokens: Vec<&str> = expression.split_whitespace().collect();
        Self::parse_tokens(&tokens)
    }

    fn parse_tokens(tokens: &[&str]) -> Result<Box<dyn Expression>, String> {
        if tokens.is_empty() {
            return Err("Empty expression".to_string());
        }

        // 간단한 파싱 예제 (실제로는 더 복잡한 파서가 필요)
        if tokens.len() == 1 {
            let token = tokens[0];
            if let Ok(value) = token.parse::<i32>() {
                Ok(Box::new(NumberExpression::new(value)))
            } else {
                Ok(Box::new(VariableExpression::new(token.to_string())))
            }
        } else if tokens.len() == 3 && tokens[1] == "+" {
            let left = Self::parse_tokens(&[tokens[0]])?;
            let right = Self::parse_tokens(&[tokens[2]])?;
            Ok(Box::new(AddExpression::new(left, right)))
        } else {
            Err("Unsupported expression format".to_string())
        }
    }
}
```

## 사용 예제

```rust
let mut context = Context::new();
context.set_variable("x".to_string(), 10);
context.set_variable("y".to_string(), 5);

// "x + y" 표현식 파싱 및 실행
let expression = ExpressionParser::parse("x + y").unwrap();
let result = expression.interpret(&context).unwrap();

println!("Expression: {}", expression.to_string()); // (x + y)
println!("Result: {}", result); // 15

// "10 + 20" 표현식 파싱 및 실행
let expression2 = ExpressionParser::parse("10 + 20").unwrap();
let result2 = expression2.interpret(&context).unwrap();

println!("Expression: {}", expression2.to_string()); // (10 + 20)
println!("Result: {}", result2); // 30
```

## 적용 상황

### 1. 간단한 수식 계산기
### 2. SQL 쿼리 인터프리터
### 3. 정규표현식 엔진
### 4. 스크립팅 언어 구현
### 5. 설정 파일 파서