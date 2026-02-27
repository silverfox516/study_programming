#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <typeinfo>
#include <sstream>
#include <cmath>

// Forward declarations
class Circle;
class Rectangle;
class Triangle;

// Visitor interface
class ShapeVisitor {
public:
    virtual ~ShapeVisitor() = default;
    virtual void visit(const Circle& circle) = 0;
    virtual void visit(const Rectangle& rectangle) = 0;
    virtual void visit(const Triangle& triangle) = 0;
};

// Element interface
class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(ShapeVisitor& visitor) const = 0;
    virtual std::string getName() const = 0;
};

// Concrete Elements
class Circle : public Shape {
private:
    double radius;
    double centerX, centerY;

public:
    Circle(double radius, double x = 0, double y = 0)
        : radius(radius), centerX(x), centerY(y) {}

    void accept(ShapeVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::string getName() const override {
        return "Circle";
    }

    double getRadius() const { return radius; }
    double getCenterX() const { return centerX; }
    double getCenterY() const { return centerY; }
};

class Rectangle : public Shape {
private:
    double width, height;
    double x, y; // Top-left corner

public:
    Rectangle(double width, double height, double x = 0, double y = 0)
        : width(width), height(height), x(x), y(y) {}

    void accept(ShapeVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::string getName() const override {
        return "Rectangle";
    }

    double getWidth() const { return width; }
    double getHeight() const { return height; }
    double getX() const { return x; }
    double getY() const { return y; }
};

class Triangle : public Shape {
private:
    double x1, y1, x2, y2, x3, y3; // Three vertices

public:
    Triangle(double x1, double y1, double x2, double y2, double x3, double y3)
        : x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3) {}

    void accept(ShapeVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::string getName() const override {
        return "Triangle";
    }

    double getX1() const { return x1; }
    double getY1() const { return y1; }
    double getX2() const { return x2; }
    double getY2() const { return y2; }
    double getX3() const { return x3; }
    double getY3() const { return y3; }
};

// Concrete Visitors
class AreaCalculatorVisitor : public ShapeVisitor {
private:
    double totalArea = 0.0;

public:
    void visit(const Circle& circle) override {
        double area = 3.14159 * circle.getRadius() * circle.getRadius();
        std::cout << "Circle area: " << area << std::endl;
        totalArea += area;
    }

    void visit(const Rectangle& rectangle) override {
        double area = rectangle.getWidth() * rectangle.getHeight();
        std::cout << "Rectangle area: " << area << std::endl;
        totalArea += area;
    }

    void visit(const Triangle& triangle) override {
        // Using shoelace formula for triangle area
        double area = 0.5 * std::abs(
            triangle.getX1() * (triangle.getY2() - triangle.getY3()) +
            triangle.getX2() * (triangle.getY3() - triangle.getY1()) +
            triangle.getX3() * (triangle.getY1() - triangle.getY2())
        );
        std::cout << "Triangle area: " << area << std::endl;
        totalArea += area;
    }

    double getTotalArea() const { return totalArea; }
    void reset() { totalArea = 0.0; }
};

class PerimeterCalculatorVisitor : public ShapeVisitor {
private:
    double totalPerimeter = 0.0;

public:
    void visit(const Circle& circle) override {
        double perimeter = 2 * 3.14159 * circle.getRadius();
        std::cout << "Circle perimeter: " << perimeter << std::endl;
        totalPerimeter += perimeter;
    }

    void visit(const Rectangle& rectangle) override {
        double perimeter = 2 * (rectangle.getWidth() + rectangle.getHeight());
        std::cout << "Rectangle perimeter: " << perimeter << std::endl;
        totalPerimeter += perimeter;
    }

    void visit(const Triangle& triangle) override {
        auto distance = [](double x1, double y1, double x2, double y2) {
            return std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
        };

        double side1 = distance(triangle.getX1(), triangle.getY1(), triangle.getX2(), triangle.getY2());
        double side2 = distance(triangle.getX2(), triangle.getY2(), triangle.getX3(), triangle.getY3());
        double side3 = distance(triangle.getX3(), triangle.getY3(), triangle.getX1(), triangle.getY1());
        double perimeter = side1 + side2 + side3;

        std::cout << "Triangle perimeter: " << perimeter << std::endl;
        totalPerimeter += perimeter;
    }

    double getTotalPerimeter() const { return totalPerimeter; }
    void reset() { totalPerimeter = 0.0; }
};

class DrawingVisitor : public ShapeVisitor {
private:
    std::ostringstream drawing;

public:
    void visit(const Circle& circle) override {
        drawing << "Drawing Circle: radius=" << circle.getRadius()
                << " at (" << circle.getCenterX() << "," << circle.getCenterY() << ")\n";
        std::cout << "🔴 Circle drawn at (" << circle.getCenterX() << "," << circle.getCenterY()
                  << ") with radius " << circle.getRadius() << std::endl;
    }

    void visit(const Rectangle& rectangle) override {
        drawing << "Drawing Rectangle: " << rectangle.getWidth() << "x" << rectangle.getHeight()
                << " at (" << rectangle.getX() << "," << rectangle.getY() << ")\n";
        std::cout << "🟦 Rectangle drawn at (" << rectangle.getX() << "," << rectangle.getY()
                  << ") size " << rectangle.getWidth() << "x" << rectangle.getHeight() << std::endl;
    }

    void visit(const Triangle& triangle) override {
        drawing << "Drawing Triangle: vertices (" << triangle.getX1() << "," << triangle.getY1()
                << "), (" << triangle.getX2() << "," << triangle.getY2()
                << "), (" << triangle.getX3() << "," << triangle.getY3() << ")\n";
        std::cout << "🔺 Triangle drawn with vertices ("
                  << triangle.getX1() << "," << triangle.getY1() << "), ("
                  << triangle.getX2() << "," << triangle.getY2() << "), ("
                  << triangle.getX3() << "," << triangle.getY3() << ")" << std::endl;
    }

    std::string getDrawingInstructions() const {
        return drawing.str();
    }

    void reset() {
        drawing.str("");
        drawing.clear();
    }
};

// Example 2: File System Visitor
class FileSystemNode;
class File;
class Directory;

class FileSystemVisitor {
public:
    virtual ~FileSystemVisitor() = default;
    virtual void visit(const File& file) = 0;
    virtual void visit(const Directory& directory) = 0;
};

class FileSystemNode {
public:
    virtual ~FileSystemNode() = default;
    virtual void accept(FileSystemVisitor& visitor) const = 0;
    virtual std::string getName() const = 0;
};

class File : public FileSystemNode {
private:
    std::string name;
    size_t size;
    std::string extension;

public:
    File(const std::string& name, size_t size, const std::string& extension)
        : name(name), size(size), extension(extension) {}

    void accept(FileSystemVisitor& visitor) const override {
        visitor.visit(*this);
    }

    std::string getName() const override { return name; }
    size_t getSize() const { return size; }
    std::string getExtension() const { return extension; }
};

class Directory : public FileSystemNode {
private:
    std::string name;
    std::vector<std::unique_ptr<FileSystemNode>> children;

public:
    explicit Directory(const std::string& name) : name(name) {}

    void addChild(std::unique_ptr<FileSystemNode> child) {
        children.push_back(std::move(child));
    }

    void accept(FileSystemVisitor& visitor) const override {
        visitor.visit(*this);
        for (const auto& child : children) {
            child->accept(visitor);
        }
    }

    std::string getName() const override { return name; }
    const std::vector<std::unique_ptr<FileSystemNode>>& getChildren() const { return children; }
};

// File System Visitors
class SizeCalculatorVisitor : public FileSystemVisitor {
private:
    size_t totalSize = 0;
    int fileCount = 0;
    int dirCount = 0;

public:
    void visit(const File& file) override {
        totalSize += file.getSize();
        fileCount++;
        std::cout << "📄 " << file.getName() << "." << file.getExtension()
                  << " (" << file.getSize() << " bytes)" << std::endl;
    }

    void visit(const Directory& directory) override {
        dirCount++;
        std::cout << "📁 " << directory.getName() << "/" << std::endl;
    }

    size_t getTotalSize() const { return totalSize; }
    int getFileCount() const { return fileCount; }
    int getDirectoryCount() const { return dirCount; }

    void reset() {
        totalSize = 0;
        fileCount = 0;
        dirCount = 0;
    }
};

class SearchVisitor : public FileSystemVisitor {
private:
    std::string searchTerm;
    std::vector<std::string> results;

public:
    explicit SearchVisitor(const std::string& term) : searchTerm(term) {}

    void visit(const File& file) override {
        if (file.getName().find(searchTerm) != std::string::npos ||
            file.getExtension().find(searchTerm) != std::string::npos) {
            std::string result = "📄 " + file.getName() + "." + file.getExtension();
            results.push_back(result);
            std::cout << "Found: " << result << std::endl;
        }
    }

    void visit(const Directory& directory) override {
        if (directory.getName().find(searchTerm) != std::string::npos) {
            std::string result = "📁 " + directory.getName() + "/";
            results.push_back(result);
            std::cout << "Found: " << result << std::endl;
        }
    }

    const std::vector<std::string>& getResults() const { return results; }

    void reset(const std::string& newTerm = "") {
        if (!newTerm.empty()) {
            searchTerm = newTerm;
        }
        results.clear();
    }
};

// Example 3: Expression Tree Visitor
class Expression;
class Number;
class BinaryOperation;

class ExpressionVisitor {
public:
    virtual ~ExpressionVisitor() = default;
    virtual void visit(const Number& number) = 0;
    virtual void visit(const BinaryOperation& operation) = 0;
};

class Expression {
public:
    virtual ~Expression() = default;
    virtual void accept(ExpressionVisitor& visitor) const = 0;
};

class Number : public Expression {
private:
    double value;

public:
    explicit Number(double value) : value(value) {}

    void accept(ExpressionVisitor& visitor) const override {
        visitor.visit(*this);
    }

    double getValue() const { return value; }
};

class BinaryOperation : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    char operator_;

public:
    BinaryOperation(std::unique_ptr<Expression> left,
                   std::unique_ptr<Expression> right,
                   char op)
        : left(std::move(left)), right(std::move(right)), operator_(op) {}

    void accept(ExpressionVisitor& visitor) const override {
        left->accept(visitor);
        right->accept(visitor);
        visitor.visit(*this);
    }

    const Expression& getLeft() const { return *left; }
    const Expression& getRight() const { return *right; }
    char getOperator() const { return operator_; }
};

// Expression Visitors
class EvaluatorVisitor : public ExpressionVisitor {
private:
    std::vector<double> stack;

public:
    void visit(const Number& number) override {
        stack.push_back(number.getValue());
        std::cout << "Push " << number.getValue() << std::endl;
    }

    void visit(const BinaryOperation& operation) override {
        if (stack.size() < 2) {
            throw std::runtime_error("Invalid expression");
        }

        double right = stack.back(); stack.pop_back();
        double left = stack.back(); stack.pop_back();
        double result = 0;

        switch (operation.getOperator()) {
            case '+': result = left + right; break;
            case '-': result = left - right; break;
            case '*': result = left * right; break;
            case '/':
                if (right == 0) throw std::runtime_error("Division by zero");
                result = left / right;
                break;
            default: throw std::runtime_error("Unknown operator");
        }

        std::cout << left << " " << operation.getOperator() << " " << right << " = " << result << std::endl;
        stack.push_back(result);
    }

    double getResult() const {
        if (stack.size() != 1) {
            throw std::runtime_error("Invalid expression evaluation");
        }
        return stack.back();
    }

    void reset() {
        stack.clear();
    }
};

class PrintVisitor : public ExpressionVisitor {
private:
    std::vector<std::string> stack;

public:
    void visit(const Number& number) override {
        stack.push_back(std::to_string(number.getValue()));
    }

    void visit(const BinaryOperation& operation) override {
        if (stack.size() < 2) {
            throw std::runtime_error("Invalid expression");
        }

        std::string right = stack.back(); stack.pop_back();
        std::string left = stack.back(); stack.pop_back();
        std::string result = "(" + left + " " + operation.getOperator() + " " + right + ")";
        stack.push_back(result);
    }

    std::string getResult() const {
        if (stack.size() != 1) {
            throw std::runtime_error("Invalid expression");
        }
        return stack.back();
    }

    void reset() {
        stack.clear();
    }
};

// Utility functions
void processShapes(std::vector<std::unique_ptr<Shape>>& shapes, ShapeVisitor& visitor) {
    for (const auto& shape : shapes) {
        shape->accept(visitor);
    }
}

int main() {
    std::cout << "=== Visitor Pattern Demo ===" << std::endl;

    // Example 1: Shape Processing
    std::cout << "\n1. Shape Processing with Visitor Pattern:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0, 10, 10));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0, 0, 0));
    shapes.push_back(std::make_unique<Triangle>(0, 0, 3, 4, 0, 4));

    // Calculate areas
    std::cout << "\nCalculating areas:" << std::endl;
    AreaCalculatorVisitor areaVisitor;
    processShapes(shapes, areaVisitor);
    std::cout << "Total area: " << areaVisitor.getTotalArea() << std::endl;

    // Calculate perimeters
    std::cout << "\nCalculating perimeters:" << std::endl;
    PerimeterCalculatorVisitor perimeterVisitor;
    processShapes(shapes, perimeterVisitor);
    std::cout << "Total perimeter: " << perimeterVisitor.getTotalPerimeter() << std::endl;

    // Draw shapes
    std::cout << "\nDrawing shapes:" << std::endl;
    DrawingVisitor drawingVisitor;
    processShapes(shapes, drawingVisitor);

    // Example 2: File System Processing
    std::cout << "\n\n2. File System Processing:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    auto root = std::make_unique<Directory>("root");
    auto documents = std::make_unique<Directory>("documents");
    auto photos = std::make_unique<Directory>("photos");

    documents->addChild(std::make_unique<File>("resume", 1024, "pdf"));
    documents->addChild(std::make_unique<File>("letter", 512, "txt"));

    photos->addChild(std::make_unique<File>("vacation1", 2048000, "jpg"));
    photos->addChild(std::make_unique<File>("vacation2", 1856000, "jpg"));

    root->addChild(std::move(documents));
    root->addChild(std::move(photos));
    root->addChild(std::make_unique<File>("readme", 256, "txt"));

    // Calculate total size
    std::cout << "\nCalculating file system size:" << std::endl;
    SizeCalculatorVisitor sizeVisitor;
    root->accept(sizeVisitor);
    std::cout << "\nTotal size: " << sizeVisitor.getTotalSize() << " bytes" << std::endl;
    std::cout << "Files: " << sizeVisitor.getFileCount() << std::endl;
    std::cout << "Directories: " << sizeVisitor.getDirectoryCount() << std::endl;

    // Search for files
    std::cout << "\nSearching for 'vacation':" << std::endl;
    SearchVisitor searchVisitor("vacation");
    root->accept(searchVisitor);

    std::cout << "\nSearching for 'txt' files:" << std::endl;
    searchVisitor.reset("txt");
    root->accept(searchVisitor);

    // Example 3: Expression Tree Processing
    std::cout << "\n\n3. Expression Tree Processing:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    // Build expression: (3 + 4) * (2 - 1)
    auto expr = std::make_unique<BinaryOperation>(
        std::make_unique<BinaryOperation>(
            std::make_unique<Number>(3),
            std::make_unique<Number>(4),
            '+'
        ),
        std::make_unique<BinaryOperation>(
            std::make_unique<Number>(2),
            std::make_unique<Number>(1),
            '-'
        ),
        '*'
    );

    // Print expression
    std::cout << "\nExpression structure:" << std::endl;
    PrintVisitor printVisitor;
    expr->accept(printVisitor);
    std::cout << "Expression: " << printVisitor.getResult() << std::endl;

    // Evaluate expression
    std::cout << "\nEvaluating expression:" << std::endl;
    EvaluatorVisitor evalVisitor;
    expr->accept(evalVisitor);
    std::cout << "Result: " << evalVisitor.getResult() << std::endl;

    // Benefits demonstration
    std::cout << "\n\n4. Visitor Pattern Benefits:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "✓ Separates algorithms from object structure" << std::endl;
    std::cout << "✓ Easy to add new operations without modifying classes" << std::endl;
    std::cout << "✓ Gathers related operations in one class" << std::endl;
    std::cout << "✓ Can accumulate state during traversal" << std::endl;
    std::cout << "✓ Enables operations across class hierarchies" << std::endl;
    std::cout << "✓ Supports multiple different operations on same structure" << std::endl;

    std::cout << "\n5. Visitor Pattern Drawbacks:" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "⚠️ Hard to add new element types (breaks existing visitors)" << std::endl;
    std::cout << "⚠️ Violates encapsulation (visitors need access to internals)" << std::endl;
    std::cout << "⚠️ Circular dependency between visitors and elements" << std::endl;
    std::cout << "⚠️ More complex than direct method calls" << std::endl;

    return 0;
}
