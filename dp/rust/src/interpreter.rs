/// Interpreter Pattern Implementation in Rust
///
/// The Interpreter pattern defines a representation for a language's grammar
/// along with an interpreter that uses the representation to interpret
/// sentences in the language.

use std::collections::HashMap;
use std::fmt;

/// Mathematical expression interpreter
pub trait Expression: std::fmt::Debug {
    fn interpret(&self, context: &Context) -> Result<f64, String>;
    fn to_string(&self) -> String;
}

#[derive(Debug, Clone)]
pub struct Context {
    variables: HashMap<String, f64>,
}

impl Context {
    pub fn new() -> Self {
        Context {
            variables: HashMap::new(),
        }
    }

    pub fn set_variable(&mut self, name: &str, value: f64) {
        self.variables.insert(name.to_string(), value);
    }

    pub fn get_variable(&self, name: &str) -> Option<f64> {
        self.variables.get(name).cloned()
    }

    pub fn clear(&mut self) {
        self.variables.clear();
    }

    pub fn get_variables(&self) -> &HashMap<String, f64> {
        &self.variables
    }
}

impl Default for Context {
    fn default() -> Self {
        Self::new()
    }
}

/// Number expression (terminal)
#[derive(Debug, Clone)]
pub struct NumberExpression {
    value: f64,
}

impl NumberExpression {
    pub fn new(value: f64) -> Self {
        NumberExpression { value }
    }
}

impl Expression for NumberExpression {
    fn interpret(&self, _context: &Context) -> Result<f64, String> {
        Ok(self.value)
    }

    fn to_string(&self) -> String {
        self.value.to_string()
    }
}

/// Variable expression (terminal)
#[derive(Debug, Clone)]
pub struct VariableExpression {
    name: String,
}

impl VariableExpression {
    pub fn new(name: &str) -> Self {
        VariableExpression {
            name: name.to_string(),
        }
    }
}

impl Expression for VariableExpression {
    fn interpret(&self, context: &Context) -> Result<f64, String> {
        context
            .get_variable(&self.name)
            .ok_or_else(|| format!("Undefined variable: {}", self.name))
    }

    fn to_string(&self) -> String {
        self.name.clone()
    }
}

/// Binary operation expressions (non-terminal)
#[derive(Debug)]
pub struct AddExpression {
    left: Box<dyn Expression>,
    right: Box<dyn Expression>,
}

impl AddExpression {
    pub fn new(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Self {
        AddExpression { left, right }
    }
}

impl Expression for AddExpression {
    fn interpret(&self, context: &Context) -> Result<f64, String> {
        let left_val = self.left.interpret(context)?;
        let right_val = self.right.interpret(context)?;
        Ok(left_val + right_val)
    }

    fn to_string(&self) -> String {
        format!("({} + {})", self.left.to_string(), self.right.to_string())
    }
}

#[derive(Debug)]
pub struct SubtractExpression {
    left: Box<dyn Expression>,
    right: Box<dyn Expression>,
}

impl SubtractExpression {
    pub fn new(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Self {
        SubtractExpression { left, right }
    }
}

impl Expression for SubtractExpression {
    fn interpret(&self, context: &Context) -> Result<f64, String> {
        let left_val = self.left.interpret(context)?;
        let right_val = self.right.interpret(context)?;
        Ok(left_val - right_val)
    }

    fn to_string(&self) -> String {
        format!("({} - {})", self.left.to_string(), self.right.to_string())
    }
}

#[derive(Debug)]
pub struct MultiplyExpression {
    left: Box<dyn Expression>,
    right: Box<dyn Expression>,
}

impl MultiplyExpression {
    pub fn new(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Self {
        MultiplyExpression { left, right }
    }
}

impl Expression for MultiplyExpression {
    fn interpret(&self, context: &Context) -> Result<f64, String> {
        let left_val = self.left.interpret(context)?;
        let right_val = self.right.interpret(context)?;
        Ok(left_val * right_val)
    }

    fn to_string(&self) -> String {
        format!("({} * {})", self.left.to_string(), self.right.to_string())
    }
}

#[derive(Debug)]
pub struct DivideExpression {
    left: Box<dyn Expression>,
    right: Box<dyn Expression>,
}

impl DivideExpression {
    pub fn new(left: Box<dyn Expression>, right: Box<dyn Expression>) -> Self {
        DivideExpression { left, right }
    }
}

impl Expression for DivideExpression {
    fn interpret(&self, context: &Context) -> Result<f64, String> {
        let left_val = self.left.interpret(context)?;
        let right_val = self.right.interpret(context)?;
        if right_val == 0.0 {
            Err("Division by zero".to_string())
        } else {
            Ok(left_val / right_val)
        }
    }

    fn to_string(&self) -> String {
        format!("({} / {})", self.left.to_string(), self.right.to_string())
    }
}

/// Function expressions
#[derive(Debug)]
pub struct SinExpression {
    operand: Box<dyn Expression>,
}

impl SinExpression {
    pub fn new(operand: Box<dyn Expression>) -> Self {
        SinExpression { operand }
    }
}

impl Expression for SinExpression {
    fn interpret(&self, context: &Context) -> Result<f64, String> {
        let val = self.operand.interpret(context)?;
        Ok(val.sin())
    }

    fn to_string(&self) -> String {
        format!("sin({})", self.operand.to_string())
    }
}

#[derive(Debug)]
pub struct CosExpression {
    operand: Box<dyn Expression>,
}

impl CosExpression {
    pub fn new(operand: Box<dyn Expression>) -> Self {
        CosExpression { operand }
    }
}

impl Expression for CosExpression {
    fn interpret(&self, context: &Context) -> Result<f64, String> {
        let val = self.operand.interpret(context)?;
        Ok(val.cos())
    }

    fn to_string(&self) -> String {
        format!("cos({})", self.operand.to_string())
    }
}

/// Simple expression parser
pub struct ExpressionParser;

impl ExpressionParser {
    pub fn parse(input: &str) -> Result<Box<dyn Expression>, String> {
        let tokens = Self::tokenize(input)?;
        Self::parse_expression(&tokens, 0).map(|(expr, _)| expr)
    }

    fn tokenize(input: &str) -> Result<Vec<String>, String> {
        let mut tokens = Vec::new();
        let mut current_token = String::new();

        for ch in input.chars() {
            match ch {
                ' ' | '\t' => {
                    if !current_token.is_empty() {
                        tokens.push(current_token);
                        current_token = String::new();
                    }
                }
                '+' | '-' | '*' | '/' | '(' | ')' => {
                    if !current_token.is_empty() {
                        tokens.push(current_token);
                        current_token = String::new();
                    }
                    tokens.push(ch.to_string());
                }
                _ => {
                    current_token.push(ch);
                }
            }
        }

        if !current_token.is_empty() {
            tokens.push(current_token);
        }

        Ok(tokens)
    }

    fn parse_expression(tokens: &[String], pos: usize) -> Result<(Box<dyn Expression>, usize), String> {
        let (mut left, mut pos) = Self::parse_term(tokens, pos)?;

        while pos < tokens.len() {
            match tokens[pos].as_str() {
                "+" => {
                    let (right, new_pos) = Self::parse_term(tokens, pos + 1)?;
                    left = Box::new(AddExpression::new(left, right));
                    pos = new_pos;
                }
                "-" => {
                    let (right, new_pos) = Self::parse_term(tokens, pos + 1)?;
                    left = Box::new(SubtractExpression::new(left, right));
                    pos = new_pos;
                }
                _ => break,
            }
        }

        Ok((left, pos))
    }

    fn parse_term(tokens: &[String], pos: usize) -> Result<(Box<dyn Expression>, usize), String> {
        let (mut left, mut pos) = Self::parse_factor(tokens, pos)?;

        while pos < tokens.len() {
            match tokens[pos].as_str() {
                "*" => {
                    let (right, new_pos) = Self::parse_factor(tokens, pos + 1)?;
                    left = Box::new(MultiplyExpression::new(left, right));
                    pos = new_pos;
                }
                "/" => {
                    let (right, new_pos) = Self::parse_factor(tokens, pos + 1)?;
                    left = Box::new(DivideExpression::new(left, right));
                    pos = new_pos;
                }
                _ => break,
            }
        }

        Ok((left, pos))
    }

    fn parse_factor(tokens: &[String], pos: usize) -> Result<(Box<dyn Expression>, usize), String> {
        if pos >= tokens.len() {
            return Err("Unexpected end of expression".to_string());
        }

        match tokens[pos].as_str() {
            "(" => {
                let (expr, new_pos) = Self::parse_expression(tokens, pos + 1)?;
                if new_pos >= tokens.len() || tokens[new_pos] != ")" {
                    return Err("Missing closing parenthesis".to_string());
                }
                Ok((expr, new_pos + 1))
            }
            "sin" => {
                if pos + 1 >= tokens.len() || tokens[pos + 1] != "(" {
                    return Err("Expected '(' after 'sin'".to_string());
                }
                let (operand, new_pos) = Self::parse_expression(tokens, pos + 2)?;
                if new_pos >= tokens.len() || tokens[new_pos] != ")" {
                    return Err("Missing closing parenthesis for sin".to_string());
                }
                Ok((Box::new(SinExpression::new(operand)), new_pos + 1))
            }
            "cos" => {
                if pos + 1 >= tokens.len() || tokens[pos + 1] != "(" {
                    return Err("Expected '(' after 'cos'".to_string());
                }
                let (operand, new_pos) = Self::parse_expression(tokens, pos + 2)?;
                if new_pos >= tokens.len() || tokens[new_pos] != ")" {
                    return Err("Missing closing parenthesis for cos".to_string());
                }
                Ok((Box::new(CosExpression::new(operand)), new_pos + 1))
            }
            token => {
                if let Ok(value) = token.parse::<f64>() {
                    Ok((Box::new(NumberExpression::new(value)), pos + 1))
                } else if token.chars().all(|c| c.is_alphabetic() || c == '_') {
                    Ok((Box::new(VariableExpression::new(token)), pos + 1))
                } else {
                    Err(format!("Invalid token: {}", token))
                }
            }
        }
    }
}

/// Boolean expression interpreter
pub trait BooleanExpression: std::fmt::Debug {
    fn interpret(&self, context: &BooleanContext) -> Result<bool, String>;
    fn to_string(&self) -> String;
}

#[derive(Debug, Clone)]
pub struct BooleanContext {
    variables: HashMap<String, bool>,
}

impl BooleanContext {
    pub fn new() -> Self {
        BooleanContext {
            variables: HashMap::new(),
        }
    }

    pub fn set_variable(&mut self, name: &str, value: bool) {
        self.variables.insert(name.to_string(), value);
    }

    pub fn get_variable(&self, name: &str) -> Option<bool> {
        self.variables.get(name).cloned()
    }
}

impl Default for BooleanContext {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug, Clone)]
pub struct BooleanConstant {
    value: bool,
}

impl BooleanConstant {
    pub fn new(value: bool) -> Self {
        BooleanConstant { value }
    }
}

impl BooleanExpression for BooleanConstant {
    fn interpret(&self, _context: &BooleanContext) -> Result<bool, String> {
        Ok(self.value)
    }

    fn to_string(&self) -> String {
        self.value.to_string()
    }
}

#[derive(Debug, Clone)]
pub struct BooleanVariable {
    name: String,
}

impl BooleanVariable {
    pub fn new(name: &str) -> Self {
        BooleanVariable {
            name: name.to_string(),
        }
    }
}

impl BooleanExpression for BooleanVariable {
    fn interpret(&self, context: &BooleanContext) -> Result<bool, String> {
        context
            .get_variable(&self.name)
            .ok_or_else(|| format!("Undefined boolean variable: {}", self.name))
    }

    fn to_string(&self) -> String {
        self.name.clone()
    }
}

#[derive(Debug)]
pub struct AndExpression {
    left: Box<dyn BooleanExpression>,
    right: Box<dyn BooleanExpression>,
}

impl AndExpression {
    pub fn new(left: Box<dyn BooleanExpression>, right: Box<dyn BooleanExpression>) -> Self {
        AndExpression { left, right }
    }
}

impl BooleanExpression for AndExpression {
    fn interpret(&self, context: &BooleanContext) -> Result<bool, String> {
        let left_val = self.left.interpret(context)?;
        let right_val = self.right.interpret(context)?;
        Ok(left_val && right_val)
    }

    fn to_string(&self) -> String {
        format!("({} AND {})", self.left.to_string(), self.right.to_string())
    }
}

#[derive(Debug)]
pub struct OrExpression {
    left: Box<dyn BooleanExpression>,
    right: Box<dyn BooleanExpression>,
}

impl OrExpression {
    pub fn new(left: Box<dyn BooleanExpression>, right: Box<dyn BooleanExpression>) -> Self {
        OrExpression { left, right }
    }
}

impl BooleanExpression for OrExpression {
    fn interpret(&self, context: &BooleanContext) -> Result<bool, String> {
        let left_val = self.left.interpret(context)?;
        let right_val = self.right.interpret(context)?;
        Ok(left_val || right_val)
    }

    fn to_string(&self) -> String {
        format!("({} OR {})", self.left.to_string(), self.right.to_string())
    }
}

#[derive(Debug)]
pub struct NotExpression {
    operand: Box<dyn BooleanExpression>,
}

impl NotExpression {
    pub fn new(operand: Box<dyn BooleanExpression>) -> Self {
        NotExpression { operand }
    }
}

impl BooleanExpression for NotExpression {
    fn interpret(&self, context: &BooleanContext) -> Result<bool, String> {
        let val = self.operand.interpret(context)?;
        Ok(!val)
    }

    fn to_string(&self) -> String {
        format!("NOT({})", self.operand.to_string())
    }
}

/// SQL-like query interpreter
#[derive(Debug, Clone)]
pub struct Record {
    fields: HashMap<String, String>,
}

impl Record {
    pub fn new() -> Self {
        Record {
            fields: HashMap::new(),
        }
    }

    pub fn set_field(&mut self, name: &str, value: &str) {
        self.fields.insert(name.to_string(), value.to_string());
    }

    pub fn get_field(&self, name: &str) -> Option<&String> {
        self.fields.get(name)
    }

    pub fn get_fields(&self) -> &HashMap<String, String> {
        &self.fields
    }
}

impl Default for Record {
    fn default() -> Self {
        Self::new()
    }
}

pub trait QueryExpression: std::fmt::Debug {
    fn evaluate(&self, record: &Record) -> Result<bool, String>;
    fn to_string(&self) -> String;
}

#[derive(Debug, Clone)]
pub struct FieldEqualsExpression {
    field_name: String,
    value: String,
}

impl FieldEqualsExpression {
    pub fn new(field_name: &str, value: &str) -> Self {
        FieldEqualsExpression {
            field_name: field_name.to_string(),
            value: value.to_string(),
        }
    }
}

impl QueryExpression for FieldEqualsExpression {
    fn evaluate(&self, record: &Record) -> Result<bool, String> {
        if let Some(field_value) = record.get_field(&self.field_name) {
            Ok(field_value == &self.value)
        } else {
            Err(format!("Field '{}' not found", self.field_name))
        }
    }

    fn to_string(&self) -> String {
        format!("{} = '{}'", self.field_name, self.value)
    }
}

#[derive(Debug, Clone)]
pub struct FieldContainsExpression {
    field_name: String,
    value: String,
}

impl FieldContainsExpression {
    pub fn new(field_name: &str, value: &str) -> Self {
        FieldContainsExpression {
            field_name: field_name.to_string(),
            value: value.to_string(),
        }
    }
}

impl QueryExpression for FieldContainsExpression {
    fn evaluate(&self, record: &Record) -> Result<bool, String> {
        if let Some(field_value) = record.get_field(&self.field_name) {
            Ok(field_value.contains(&self.value))
        } else {
            Err(format!("Field '{}' not found", self.field_name))
        }
    }

    fn to_string(&self) -> String {
        format!("{} CONTAINS '{}'", self.field_name, self.value)
    }
}

#[derive(Debug)]
pub struct QueryAndExpression {
    left: Box<dyn QueryExpression>,
    right: Box<dyn QueryExpression>,
}

impl QueryAndExpression {
    pub fn new(left: Box<dyn QueryExpression>, right: Box<dyn QueryExpression>) -> Self {
        QueryAndExpression { left, right }
    }
}

impl QueryExpression for QueryAndExpression {
    fn evaluate(&self, record: &Record) -> Result<bool, String> {
        let left_result = self.left.evaluate(record)?;
        let right_result = self.right.evaluate(record)?;
        Ok(left_result && right_result)
    }

    fn to_string(&self) -> String {
        format!("({} AND {})", self.left.to_string(), self.right.to_string())
    }
}

/// Expression evaluator and calculator
pub struct Calculator {
    context: Context,
}

impl Calculator {
    pub fn new() -> Self {
        Calculator {
            context: Context::new(),
        }
    }

    pub fn set_variable(&mut self, name: &str, value: f64) {
        self.context.set_variable(name, value);
    }

    pub fn evaluate(&self, expression_str: &str) -> Result<f64, String> {
        let expression = ExpressionParser::parse(expression_str)?;
        expression.interpret(&self.context)
    }

    pub fn evaluate_expression(&self, expression: &dyn Expression) -> Result<f64, String> {
        expression.interpret(&self.context)
    }

    pub fn get_variables(&self) -> &HashMap<String, f64> {
        self.context.get_variables()
    }

    pub fn clear_variables(&mut self) {
        self.context.clear();
    }
}

impl Default for Calculator {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_arithmetic() {
        let mut calc = Calculator::new();

        assert_eq!(calc.evaluate("2 + 3").unwrap(), 5.0);
        assert_eq!(calc.evaluate("10 - 4").unwrap(), 6.0);
        assert_eq!(calc.evaluate("3 * 4").unwrap(), 12.0);
        assert_eq!(calc.evaluate("15 / 3").unwrap(), 5.0);
    }

    #[test]
    fn test_variables() {
        let mut calc = Calculator::new();
        calc.set_variable("x", 10.0);
        calc.set_variable("y", 5.0);

        assert_eq!(calc.evaluate("x + y").unwrap(), 15.0);
        assert_eq!(calc.evaluate("x * y").unwrap(), 50.0);
    }

    #[test]
    fn test_complex_expression() {
        let mut calc = Calculator::new();
        calc.set_variable("pi", 3.14159);

        let result = calc.evaluate("2 * pi").unwrap();
        assert!((result - 6.28318).abs() < 0.001);
    }

    #[test]
    fn test_boolean_expressions() {
        let mut context = BooleanContext::new();
        context.set_variable("a", true);
        context.set_variable("b", false);

        let expr = AndExpression::new(
            Box::new(BooleanVariable::new("a")),
            Box::new(BooleanVariable::new("b")),
        );

        assert_eq!(expr.interpret(&context).unwrap(), false);
    }

    #[test]
    fn test_query_expressions() {
        let mut record = Record::new();
        record.set_field("name", "John");
        record.set_field("department", "Engineering");

        let expr = FieldEqualsExpression::new("name", "John");
        assert_eq!(expr.evaluate(&record).unwrap(), true);

        let expr2 = FieldContainsExpression::new("department", "Eng");
        assert_eq!(expr2.evaluate(&record).unwrap(), true);
    }
}

pub fn demo() {
    println!("=== Interpreter Pattern Demo ===");

    // Mathematical Expression Interpreter
    println!("\n1. Mathematical Expression Interpreter:");
    let mut calculator = Calculator::new();

    let expressions = [
        "5 + 3",
        "10 - 4",
        "6 * 7",
        "15 / 3",
        "2 + 3 * 4",
        "(2 + 3) * 4",
    ];

    for expr_str in &expressions {
        match calculator.evaluate(expr_str) {
            Ok(result) => println!("  {} = {}", expr_str, result),
            Err(error) => println!("  {} -> Error: {}", expr_str, error),
        }
    }

    // Variables
    println!("\n2. Variables in Expressions:");
    calculator.set_variable("x", 10.0);
    calculator.set_variable("y", 5.0);
    calculator.set_variable("pi", 3.14159);

    let var_expressions = [
        "x + y",
        "x * y",
        "x / y",
        "2 * pi",
        "x + 2 * y",
    ];

    println!("Variables: {:?}", calculator.get_variables());
    for expr_str in &var_expressions {
        match calculator.evaluate(expr_str) {
            Ok(result) => println!("  {} = {:.3}", expr_str, result),
            Err(error) => println!("  {} -> Error: {}", expr_str, error),
        }
    }

    // Function expressions
    println!("\n3. Function Expressions:");
    let func_expressions = [
        "sin(0)",
        "cos(0)",
        "sin(pi)",
        "cos(pi)",
    ];

    for expr_str in &func_expressions {
        match calculator.evaluate(expr_str) {
            Ok(result) => println!("  {} = {:.6}", expr_str, result),
            Err(error) => println!("  {} -> Error: {}", expr_str, error),
        }
    }

    // Boolean Expression Interpreter
    println!("\n4. Boolean Expression Interpreter:");
    let mut bool_context = BooleanContext::new();
    bool_context.set_variable("is_admin", true);
    bool_context.set_variable("is_logged_in", true);
    bool_context.set_variable("has_permission", false);

    // Create boolean expressions
    let admin_and_logged_in = AndExpression::new(
        Box::new(BooleanVariable::new("is_admin")),
        Box::new(BooleanVariable::new("is_logged_in")),
    );

    let access_granted = OrExpression::new(
        Box::new(admin_and_logged_in),
        Box::new(BooleanVariable::new("has_permission")),
    );

    let access_denied = NotExpression::new(Box::new(access_granted));

    println!("Boolean variables: is_admin=true, is_logged_in=true, has_permission=false");
    println!("  (is_admin AND is_logged_in) = {}",
             AndExpression::new(
                 Box::new(BooleanVariable::new("is_admin")),
                 Box::new(BooleanVariable::new("is_logged_in")),
             ).interpret(&bool_context).unwrap());

    println!("  ((is_admin AND is_logged_in) OR has_permission) = {}",
             OrExpression::new(
                 Box::new(AndExpression::new(
                     Box::new(BooleanVariable::new("is_admin")),
                     Box::new(BooleanVariable::new("is_logged_in")),
                 )),
                 Box::new(BooleanVariable::new("has_permission")),
             ).interpret(&bool_context).unwrap());

    // Query Expression Interpreter
    println!("\n5. Database Query Interpreter:");

    // Create sample records
    let mut records = Vec::new();

    let mut record1 = Record::new();
    record1.set_field("name", "Alice");
    record1.set_field("department", "Engineering");
    record1.set_field("salary", "75000");
    records.push(record1);

    let mut record2 = Record::new();
    record2.set_field("name", "Bob");
    record2.set_field("department", "Marketing");
    record2.set_field("salary", "65000");
    records.push(record2);

    let mut record3 = Record::new();
    record3.set_field("name", "Charlie");
    record3.set_field("department", "Engineering");
    record3.set_field("salary", "80000");
    records.push(record3);

    // Query: department = 'Engineering'
    let engineering_query = FieldEqualsExpression::new("department", "Engineering");
    println!("Query: {}", engineering_query.to_string());
    println!("Results:");
    for (i, record) in records.iter().enumerate() {
        if engineering_query.evaluate(record).unwrap_or(false) {
            println!("  Record {}: {:?}", i + 1, record.get_fields());
        }
    }

    // Complex query: department = 'Engineering' AND name CONTAINS 'e'
    let complex_query = QueryAndExpression::new(
        Box::new(FieldEqualsExpression::new("department", "Engineering")),
        Box::new(FieldContainsExpression::new("name", "e")),
    );
    println!("\nComplex Query: {}", complex_query.to_string());
    println!("Results:");
    for (i, record) in records.iter().enumerate() {
        if complex_query.evaluate(record).unwrap_or(false) {
            println!("  Record {}: {:?}", i + 1, record.get_fields());
        }
    }

    // Expression building and evaluation
    println!("\n6. Dynamic Expression Building:");

    // Build expression: (x + y) * 2
    let x_var = Box::new(VariableExpression::new("x"));
    let y_var = Box::new(VariableExpression::new("y"));
    let sum = Box::new(AddExpression::new(x_var, y_var));
    let two = Box::new(NumberExpression::new(2.0));
    let result_expr = MultiplyExpression::new(sum, two);

    calculator.set_variable("x", 15.0);
    calculator.set_variable("y", 25.0);

    println!("Built expression: {}", result_expr.to_string());
    match calculator.evaluate_expression(&result_expr) {
        Ok(result) => println!("Result with x=15, y=25: {}", result),
        Err(error) => println!("Error: {}", error),
    }

    // Error handling
    println!("\n7. Error Handling:");
    let error_cases = [
        "5 / 0",           // Division by zero
        "undefined_var",   // Undefined variable
        "5 + + 3",        // Invalid syntax (would need better parser)
    ];

    for expr_str in &error_cases {
        match calculator.evaluate(expr_str) {
            Ok(result) => println!("  {} = {}", expr_str, result),
            Err(error) => println!("  {} -> Error: {}", expr_str, error),
        }
    }

    println!("\n8. Interpreter Pattern Benefits:");
    println!("  ✅ Implements grammar rules for a language");
    println!("  ✅ Easy to change and extend grammar");
    println!("  ✅ Each grammar rule is a separate class");
    println!("  ✅ Supports complex expressions through composition");
    println!("  ✅ Enables domain-specific languages (DSLs)");
    println!("  ✅ Facilitates rule-based systems");
    println!("  ✅ Provides flexible expression evaluation");

    println!("\n9. Common Use Cases:");
    println!("  • Mathematical expression evaluators");
    println!("  • Configuration file parsers");
    println!("  • Query languages (SQL-like)");
    println!("  • Regular expression engines");
    println!("  • Scripting language interpreters");
    println!("  • Business rule engines");
    println!("  • Template processors");
}

pub fn main() {
    demo();
}