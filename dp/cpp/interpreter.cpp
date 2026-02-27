#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <functional>

// Context for the interpreter
class Context {
private:
    std::map<std::string, int> variables;

public:
    void setVariable(const std::string& name, int value) {
        variables[name] = value;
        std::cout << "📝 Set variable " << name << " = " << value << std::endl;
    }

    int getVariable(const std::string& name) const {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }
        throw std::runtime_error("Variable '" + name + "' not found");
    }

    bool hasVariable(const std::string& name) const {
        return variables.find(name) != variables.end();
    }

    void displayVariables() const {
        std::cout << "Variables:" << std::endl;
        for (const auto& [name, value] : variables) {
            std::cout << "  " << name << " = " << value << std::endl;
        }
    }

    void clearVariables() {
        variables.clear();
        std::cout << "🗑️ Variables cleared" << std::endl;
    }
};

// Abstract Expression
class Expression {
public:
    virtual ~Expression() = default;
    virtual int interpret(const Context& context) = 0;
    virtual std::string toString() const = 0;
};

// Terminal Expression - Number
class NumberExpression : public Expression {
private:
    int value;

public:
    explicit NumberExpression(int value) : value(value) {}

    int interpret(const Context& context) override {
        return value;
    }

    std::string toString() const override {
        return std::to_string(value);
    }
};

// Terminal Expression - Variable
class VariableExpression : public Expression {
private:
    std::string name;

public:
    explicit VariableExpression(const std::string& name) : name(name) {}

    int interpret(const Context& context) override {
        return context.getVariable(name);
    }

    std::string toString() const override {
        return name;
    }
};

// Non-terminal Expression - Addition
class AddExpression : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    AddExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}

    int interpret(const Context& context) override {
        int leftValue = left->interpret(context);
        int rightValue = right->interpret(context);
        int result = leftValue + rightValue;
        std::cout << "➕ " << leftValue << " + " << rightValue << " = " << result << std::endl;
        return result;
    }

    std::string toString() const override {
        return "(" + left->toString() + " + " + right->toString() + ")";
    }
};

// Non-terminal Expression - Subtraction
class SubtractExpression : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    SubtractExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}

    int interpret(const Context& context) override {
        int leftValue = left->interpret(context);
        int rightValue = right->interpret(context);
        int result = leftValue - rightValue;
        std::cout << "➖ " << leftValue << " - " << rightValue << " = " << result << std::endl;
        return result;
    }

    std::string toString() const override {
        return "(" + left->toString() + " - " + right->toString() + ")";
    }
};

// Non-terminal Expression - Multiplication
class MultiplyExpression : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    MultiplyExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}

    int interpret(const Context& context) override {
        int leftValue = left->interpret(context);
        int rightValue = right->interpret(context);
        int result = leftValue * rightValue;
        std::cout << "✖️ " << leftValue << " * " << rightValue << " = " << result << std::endl;
        return result;
    }

    std::string toString() const override {
        return "(" + left->toString() + " * " + right->toString() + ")";
    }
};

// Non-terminal Expression - Division
class DivideExpression : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    DivideExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : left(std::move(left)), right(std::move(right)) {}

    int interpret(const Context& context) override {
        int leftValue = left->interpret(context);
        int rightValue = right->interpret(context);
        if (rightValue == 0) {
            throw std::runtime_error("Division by zero");
        }
        int result = leftValue / rightValue;
        std::cout << "➗ " << leftValue << " / " << rightValue << " = " << result << std::endl;
        return result;
    }

    std::string toString() const override {
        return "(" + left->toString() + " / " + right->toString() + ")";
    }
};

// Simple Parser for mathematical expressions
class ExpressionParser {
private:
    std::string input;
    size_t position;

    void skipWhitespace() {
        while (position < input.length() && std::isspace(input[position])) {
            position++;
        }
    }

    std::unique_ptr<Expression> parseNumber() {
        skipWhitespace();
        if (position >= input.length() || !std::isdigit(input[position])) {
            throw std::runtime_error("Expected number at position " + std::to_string(position));
        }

        int value = 0;
        while (position < input.length() && std::isdigit(input[position])) {
            value = value * 10 + (input[position] - '0');
            position++;
        }

        return std::make_unique<NumberExpression>(value);
    }

    std::unique_ptr<Expression> parseVariable() {
        skipWhitespace();
        if (position >= input.length() || !std::isalpha(input[position])) {
            throw std::runtime_error("Expected variable at position " + std::to_string(position));
        }

        std::string name;
        while (position < input.length() && std::isalnum(input[position])) {
            name += input[position];
            position++;
        }

        return std::make_unique<VariableExpression>(name);
    }

    std::unique_ptr<Expression> parseFactor() {
        skipWhitespace();

        if (position < input.length() && input[position] == '(') {
            position++; // Skip '('
            auto expr = parseExpression();
            skipWhitespace();
            if (position >= input.length() || input[position] != ')') {
                throw std::runtime_error("Expected ')' at position " + std::to_string(position));
            }
            position++; // Skip ')'
            return expr;
        } else if (position < input.length() && std::isdigit(input[position])) {
            return parseNumber();
        } else if (position < input.length() && std::isalpha(input[position])) {
            return parseVariable();
        } else {
            throw std::runtime_error("Unexpected character at position " + std::to_string(position));
        }
    }

    std::unique_ptr<Expression> parseTerm() {
        auto left = parseFactor();

        while (true) {
            skipWhitespace();
            if (position >= input.length()) break;

            char op = input[position];
            if (op == '*' || op == '/') {
                position++; // Skip operator
                auto right = parseFactor();
                if (op == '*') {
                    left = std::make_unique<MultiplyExpression>(std::move(left), std::move(right));
                } else {
                    left = std::make_unique<DivideExpression>(std::move(left), std::move(right));
                }
            } else {
                break;
            }
        }

        return left;
    }

    std::unique_ptr<Expression> parseExpression() {
        auto left = parseTerm();

        while (true) {
            skipWhitespace();
            if (position >= input.length()) break;

            char op = input[position];
            if (op == '+' || op == '-') {
                position++; // Skip operator
                auto right = parseTerm();
                if (op == '+') {
                    left = std::make_unique<AddExpression>(std::move(left), std::move(right));
                } else {
                    left = std::make_unique<SubtractExpression>(std::move(left), std::move(right));
                }
            } else {
                break;
            }
        }

        return left;
    }

public:
    std::unique_ptr<Expression> parse(const std::string& expression) {
        input = expression;
        position = 0;

        auto result = parseExpression();

        skipWhitespace();
        if (position < input.length()) {
            throw std::runtime_error("Unexpected characters at end of expression");
        }

        return result;
    }
};

// Example 2: Boolean Expression Interpreter
class BooleanExpression {
public:
    virtual ~BooleanExpression() = default;
    virtual bool evaluate(const Context& context) = 0;
    virtual std::string toString() const = 0;
};

class TrueExpression : public BooleanExpression {
public:
    bool evaluate(const Context& context) override {
        return true;
    }

    std::string toString() const override {
        return "true";
    }
};

class FalseExpression : public BooleanExpression {
public:
    bool evaluate(const Context& context) override {
        return false;
    }

    std::string toString() const override {
        return "false";
    }
};

class AndExpression : public BooleanExpression {
private:
    std::unique_ptr<BooleanExpression> left;
    std::unique_ptr<BooleanExpression> right;

public:
    AndExpression(std::unique_ptr<BooleanExpression> left, std::unique_ptr<BooleanExpression> right)
        : left(std::move(left)), right(std::move(right)) {}

    bool evaluate(const Context& context) override {
        bool leftValue = left->evaluate(context);
        bool rightValue = right->evaluate(context);
        bool result = leftValue && rightValue;
        std::cout << "🔗 " << leftValue << " AND " << rightValue << " = " << result << std::endl;
        return result;
    }

    std::string toString() const override {
        return "(" + left->toString() + " AND " + right->toString() + ")";
    }
};

class OrExpression : public BooleanExpression {
private:
    std::unique_ptr<BooleanExpression> left;
    std::unique_ptr<BooleanExpression> right;

public:
    OrExpression(std::unique_ptr<BooleanExpression> left, std::unique_ptr<BooleanExpression> right)
        : left(std::move(left)), right(std::move(right)) {}

    bool evaluate(const Context& context) override {
        bool leftValue = left->evaluate(context);
        bool rightValue = right->evaluate(context);
        bool result = leftValue || rightValue;
        std::cout << "🔀 " << leftValue << " OR " << rightValue << " = " << result << std::endl;
        return result;
    }

    std::string toString() const override {
        return "(" + left->toString() + " OR " + right->toString() + ")";
    }
};

class NotExpression : public BooleanExpression {
private:
    std::unique_ptr<BooleanExpression> operand;

public:
    explicit NotExpression(std::unique_ptr<BooleanExpression> operand)
        : operand(std::move(operand)) {}

    bool evaluate(const Context& context) override {
        bool value = operand->evaluate(context);
        bool result = !value;
        std::cout << "❌ NOT " << value << " = " << result << std::endl;
        return result;
    }

    std::string toString() const override {
        return "(NOT " + operand->toString() + ")";
    }
};

// Example 3: Simple Command Language Interpreter
class Command {
public:
    virtual ~Command() = default;
    virtual void execute(Context& context) = 0;
    virtual std::string toString() const = 0;
};

class AssignCommand : public Command {
private:
    std::string variable;
    std::unique_ptr<Expression> expression;

public:
    AssignCommand(const std::string& variable, std::unique_ptr<Expression> expr)
        : variable(variable), expression(std::move(expr)) {}

    void execute(Context& context) override {
        int value = expression->interpret(context);
        context.setVariable(variable, value);
        std::cout << "✅ Assigned " << variable << " = " << value << std::endl;
    }

    std::string toString() const override {
        return variable + " = " + expression->toString();
    }
};

class PrintCommand : public Command {
private:
    std::unique_ptr<Expression> expression;

public:
    explicit PrintCommand(std::unique_ptr<Expression> expr)
        : expression(std::move(expr)) {}

    void execute(Context& context) override {
        int value = expression->interpret(context);
        std::cout << "📄 Print: " << expression->toString() << " = " << value << std::endl;
    }

    std::string toString() const override {
        return "print " + expression->toString();
    }
};

class Program {
private:
    std::vector<std::unique_ptr<Command>> commands;

public:
    void addCommand(std::unique_ptr<Command> command) {
        commands.push_back(std::move(command));
    }

    void execute(Context& context) {
        std::cout << "🚀 Executing program..." << std::endl;
        for (auto& command : commands) {
            std::cout << "  Executing: " << command->toString() << std::endl;
            command->execute(context);
        }
        std::cout << "✅ Program execution completed" << std::endl;
    }

    void displayProgram() const {
        std::cout << "📋 Program:" << std::endl;
        for (size_t i = 0; i < commands.size(); ++i) {
            std::cout << "  " << (i + 1) << ": " << commands[i]->toString() << std::endl;
        }
    }
};

// Example 4: Modern C++ Functional Interpreter
template<typename T>
using InterpreterFunc = std::function<T(const Context&)>;

template<typename T>
class FunctionalExpression {
private:
    InterpreterFunc<T> interpreter;
    std::string description;

public:
    FunctionalExpression(InterpreterFunc<T> func, const std::string& desc)
        : interpreter(func), description(desc) {}

    T evaluate(const Context& context) const {
        return interpreter(context);
    }

    const std::string& getDescription() const {
        return description;
    }
};

class FunctionalInterpreter {
public:
    static FunctionalExpression<int> number(int value) {
        return FunctionalExpression<int>(
            [value](const Context&) { return value; },
            std::to_string(value)
        );
    }

    static FunctionalExpression<int> variable(const std::string& name) {
        return FunctionalExpression<int>(
            [name](const Context& ctx) { return ctx.getVariable(name); },
            name
        );
    }

    static FunctionalExpression<int> add(const FunctionalExpression<int>& left,
                                        const FunctionalExpression<int>& right) {
        return FunctionalExpression<int>(
            [left, right](const Context& ctx) {
                int l = left.evaluate(ctx);
                int r = right.evaluate(ctx);
                return l + r;
            },
            "(" + left.getDescription() + " + " + right.getDescription() + ")"
        );
    }

    static FunctionalExpression<int> multiply(const FunctionalExpression<int>& left,
                                             const FunctionalExpression<int>& right) {
        return FunctionalExpression<int>(
            [left, right](const Context& ctx) {
                int l = left.evaluate(ctx);
                int r = right.evaluate(ctx);
                return l * r;
            },
            "(" + left.getDescription() + " * " + right.getDescription() + ")"
        );
    }
};

int main() {
    std::cout << "=== Interpreter Pattern Demo ===" << std::endl;

    // Example 1: Mathematical Expression Interpreter
    std::cout << "\n1. Mathematical Expression Interpreter:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    Context context;
    context.setVariable("x", 10);
    context.setVariable("y", 5);
    context.setVariable("z", 3);

    ExpressionParser parser;

    std::vector<std::string> expressions = {
        "x + y",
        "x * y - z",
        "(x + y) * z",
        "x / y + z * 2",
        "100 - x * 5"
    };

    for (const auto& exprStr : expressions) {
        try {
            std::cout << "\nEvaluating: " << exprStr << std::endl;
            auto expression = parser.parse(exprStr);
            std::cout << "Parsed as: " << expression->toString() << std::endl;

            int result = expression->interpret(context);
            std::cout << "🎯 Final result: " << result << std::endl;

        } catch (const std::exception& e) {
            std::cout << "❌ Error: " << e.what() << std::endl;
        }
    }

    // Example 2: Boolean Expression Interpreter
    std::cout << "\n\n2. Boolean Expression Interpreter:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    // true AND (false OR true)
    auto boolExpr1 = std::make_unique<AndExpression>(
        std::make_unique<TrueExpression>(),
        std::make_unique<OrExpression>(
            std::make_unique<FalseExpression>(),
            std::make_unique<TrueExpression>()
        )
    );

    std::cout << "Expression: " << boolExpr1->toString() << std::endl;
    std::cout << "Evaluation:" << std::endl;
    bool result1 = boolExpr1->evaluate(context);
    std::cout << "🎯 Result: " << (result1 ? "true" : "false") << std::endl;

    // NOT (true AND false)
    std::cout << "\nExpression: NOT (true AND false)" << std::endl;
    auto boolExpr2 = std::make_unique<NotExpression>(
        std::make_unique<AndExpression>(
            std::make_unique<TrueExpression>(),
            std::make_unique<FalseExpression>()
        )
    );

    std::cout << "Evaluation:" << std::endl;
    bool result2 = boolExpr2->evaluate(context);
    std::cout << "🎯 Result: " << (result2 ? "true" : "false") << std::endl;

    // Example 3: Simple Command Language
    std::cout << "\n\n3. Simple Command Language:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    Program program;

    // Create a simple program
    // a = 5
    program.addCommand(std::make_unique<AssignCommand>("a",
        std::make_unique<NumberExpression>(5)));

    // b = a * 3
    program.addCommand(std::make_unique<AssignCommand>("b",
        std::make_unique<MultiplyExpression>(
            std::make_unique<VariableExpression>("a"),
            std::make_unique<NumberExpression>(3)
        )));

    // c = a + b
    program.addCommand(std::make_unique<AssignCommand>("c",
        std::make_unique<AddExpression>(
            std::make_unique<VariableExpression>("a"),
            std::make_unique<VariableExpression>("b")
        )));

    // print c
    program.addCommand(std::make_unique<PrintCommand>(
        std::make_unique<VariableExpression>("c")));

    program.displayProgram();

    Context programContext;
    std::cout << "\nExecution:" << std::endl;
    program.execute(programContext);

    std::cout << "\nFinal context:" << std::endl;
    programContext.displayVariables();

    // Example 4: Functional Interpreter
    std::cout << "\n\n4. Functional Interpreter (Modern C++):" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    Context funcContext;
    funcContext.setVariable("num1", 7);
    funcContext.setVariable("num2", 3);

    // Create expressions using functional approach
    auto expr1 = FunctionalInterpreter::number(10);
    auto expr2 = FunctionalInterpreter::variable("num1");
    auto expr3 = FunctionalInterpreter::variable("num2");

    auto addExpr = FunctionalInterpreter::add(expr2, expr3);
    auto complexExpr = FunctionalInterpreter::multiply(expr1, addExpr);

    std::cout << "Expression 1: " << expr1.getDescription() << " = " << expr1.evaluate(funcContext) << std::endl;
    std::cout << "Expression 2: " << addExpr.getDescription() << " = " << addExpr.evaluate(funcContext) << std::endl;
    std::cout << "Complex Expression: " << complexExpr.getDescription() << " = " << complexExpr.evaluate(funcContext) << std::endl;

    // Practical example: Formula interpreter
    std::cout << "\n\n5. Practical Formula Interpreter:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    Context formulaContext;
    formulaContext.setVariable("price", 100);
    formulaContext.setVariable("tax_rate", 8);
    formulaContext.setVariable("discount", 15);

    try {
        // total = price + (price * tax_rate / 100) - discount
        std::string formula = "price + price * tax_rate / 100 - discount";
        std::cout << "Formula: " << formula << std::endl;

        auto totalExpr = parser.parse(formula);
        std::cout << "Variables:" << std::endl;
        formulaContext.displayVariables();

        std::cout << "\nCalculation:" << std::endl;
        int total = totalExpr->interpret(formulaContext);
        std::cout << "🎯 Total: " << total << std::endl;

    } catch (const std::exception& e) {
        std::cout << "❌ Error in formula calculation: " << e.what() << std::endl;
    }

    // Benefits and use cases
    std::cout << "\n\n6. Interpreter Pattern Benefits:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "✓ Easy to implement simple grammars" << std::endl;
    std::cout << "✓ Grammar rules are explicit in code structure" << std::endl;
    std::cout << "✓ Easy to add new ways to interpret expressions" << std::endl;
    std::cout << "✓ Supports complex tree-walking algorithms" << std::endl;
    std::cout << "✓ Can be combined with Visitor pattern" << std::endl;

    std::cout << "\n7. When to Use Interpreter Pattern:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "• Simple domain-specific languages (DSL)" << std::endl;
    std::cout << "• Configuration file parsers" << std::endl;
    std::cout << "• Mathematical expression evaluators" << std::endl;
    std::cout << "• Rule engines" << std::endl;
    std::cout << "• Query language interpreters" << std::endl;
    std::cout << "• Scripting language implementations" << std::endl;

    std::cout << "\n8. Considerations:" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "⚠️ Can become complex for large grammars" << std::endl;
    std::cout << "⚠️ May have performance overhead" << std::endl;
    std::cout << "⚠️ Consider parser generators for complex languages" << std::endl;
    std::cout << "⚠️ Memory usage can be high for large expression trees" << std::endl;

    return 0;
}