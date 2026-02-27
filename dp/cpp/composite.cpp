#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

// Component interface
class FileSystemComponent {
public:
    virtual ~FileSystemComponent() = default;
    virtual void display(int depth = 0) const = 0;
    virtual size_t getSize() const = 0;
    virtual std::string getName() const = 0;
    virtual bool isComposite() const { return false; }
    virtual void add(std::unique_ptr<FileSystemComponent> component) {
        throw std::runtime_error("Operation not supported");
    }
    virtual void remove(const std::string& name) {
        throw std::runtime_error("Operation not supported");
    }
    virtual FileSystemComponent* find(const std::string& name) {
        return (getName() == name) ? this : nullptr;
    }
};

// Leaf: File
class File : public FileSystemComponent {
private:
    std::string name;
    size_t size;
    std::string content;

public:
    File(const std::string& name, size_t size, const std::string& content = "")
        : name(name), size(size), content(content) {}

    void display(int depth = 0) const override {
        std::string indent(depth * 2, ' ');
        std::cout << indent << "📄 " << name << " (" << size << " bytes)" << std::endl;
    }

    size_t getSize() const override {
        return size;
    }

    std::string getName() const override {
        return name;
    }

    const std::string& getContent() const {
        return content;
    }

    void setContent(const std::string& newContent) {
        content = newContent;
        size = content.length();
    }
};

// Composite: Directory
class Directory : public FileSystemComponent {
private:
    std::string name;
    std::vector<std::unique_ptr<FileSystemComponent>> children;

public:
    explicit Directory(const std::string& name) : name(name) {}

    void display(int depth = 0) const override {
        std::string indent(depth * 2, ' ');
        std::cout << indent << "📁 " << name << "/" << std::endl;

        for (const auto& child : children) {
            child->display(depth + 1);
        }
    }

    size_t getSize() const override {
        size_t totalSize = 0;
        for (const auto& child : children) {
            totalSize += child->getSize();
        }
        return totalSize;
    }

    std::string getName() const override {
        return name;
    }

    bool isComposite() const override {
        return true;
    }

    void add(std::unique_ptr<FileSystemComponent> component) override {
        children.push_back(std::move(component));
    }

    void remove(const std::string& name) override {
        children.erase(
            std::remove_if(children.begin(), children.end(),
                [&name](const auto& child) {
                    return child->getName() == name;
                }),
            children.end()
        );
    }

    FileSystemComponent* find(const std::string& name) override {
        if (this->name == name) {
            return this;
        }

        for (auto& child : children) {
            if (auto found = child->find(name)) {
                return found;
            }
        }
        return nullptr;
    }

    size_t getChildCount() const {
        return children.size();
    }
};

// Example 2: UI Component System
class UIComponent {
public:
    virtual ~UIComponent() = default;
    virtual void render() const = 0;
    virtual void handleClick(int x, int y) = 0;
    virtual std::string getType() const = 0;
    virtual bool isContainer() const { return false; }
    virtual void add(std::unique_ptr<UIComponent> component) {
        throw std::runtime_error("Cannot add to non-container component");
    }
};

// Leaf: Button
class Button : public UIComponent {
private:
    std::string text;
    int x, y, width, height;

public:
    Button(const std::string& text, int x, int y, int width, int height)
        : text(text), x(x), y(y), width(width), height(height) {}

    void render() const override {
        std::cout << "  [" << text << "] at (" << x << "," << y
                  << ") size " << width << "x" << height << std::endl;
    }

    void handleClick(int clickX, int clickY) override {
        if (clickX >= x && clickX <= x + width &&
            clickY >= y && clickY <= y + height) {
            std::cout << "Button '" << text << "' clicked!" << std::endl;
        }
    }

    std::string getType() const override {
        return "Button";
    }
};

// Leaf: Label
class Label : public UIComponent {
private:
    std::string text;
    int x, y;

public:
    Label(const std::string& text, int x, int y)
        : text(text), x(x), y(y) {}

    void render() const override {
        std::cout << "  Label: \"" << text << "\" at (" << x << "," << y << ")" << std::endl;
    }

    void handleClick(int clickX, int clickY) override {
        // Labels typically don't handle clicks
        std::cout << "Label '" << text << "' is not clickable" << std::endl;
    }

    std::string getType() const override {
        return "Label";
    }
};

// Composite: Panel
class Panel : public UIComponent {
private:
    std::string name;
    std::vector<std::unique_ptr<UIComponent>> components;
    int x, y, width, height;

public:
    Panel(const std::string& name, int x, int y, int width, int height)
        : name(name), x(x), y(y), width(width), height(height) {}

    void render() const override {
        std::cout << "Panel '" << name << "' at (" << x << "," << y
                  << ") size " << width << "x" << height << ":" << std::endl;
        for (const auto& component : components) {
            component->render();
        }
    }

    void handleClick(int clickX, int clickY) override {
        std::cout << "Click at (" << clickX << "," << clickY << ") in panel '" << name << "'" << std::endl;
        for (auto& component : components) {
            component->handleClick(clickX, clickY);
        }
    }

    std::string getType() const override {
        return "Panel";
    }

    bool isContainer() const override {
        return true;
    }

    void add(std::unique_ptr<UIComponent> component) override {
        components.push_back(std::move(component));
    }
};

// Example 3: Mathematical Expression Tree
class Expression {
public:
    virtual ~Expression() = default;
    virtual double evaluate() const = 0;
    virtual std::string toString() const = 0;
    virtual bool isLeaf() const = 0;
};

// Leaf: Number
class Number : public Expression {
private:
    double value;

public:
    explicit Number(double value) : value(value) {}

    double evaluate() const override {
        return value;
    }

    std::string toString() const override {
        return std::to_string(value);
    }

    bool isLeaf() const override {
        return true;
    }
};

// Composite: Binary Operation
class BinaryOperation : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    char operation;

public:
    BinaryOperation(std::unique_ptr<Expression> left,
                   std::unique_ptr<Expression> right,
                   char operation)
        : left(std::move(left)), right(std::move(right)), operation(operation) {}

    double evaluate() const override {
        double leftValue = left->evaluate();
        double rightValue = right->evaluate();

        switch (operation) {
            case '+': return leftValue + rightValue;
            case '-': return leftValue - rightValue;
            case '*': return leftValue * rightValue;
            case '/':
                if (rightValue == 0) {
                    throw std::runtime_error("Division by zero");
                }
                return leftValue / rightValue;
            default:
                throw std::runtime_error("Unknown operation");
        }
    }

    std::string toString() const override {
        return "(" + left->toString() + " " + operation + " " + right->toString() + ")";
    }

    bool isLeaf() const override {
        return false;
    }
};

// Example 4: Organization Structure
class Employee {
public:
    virtual ~Employee() = default;
    virtual void showDetails() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getTitle() const = 0;
    virtual double getSalary() const = 0;
    virtual bool isManager() const { return false; }
    virtual void addSubordinate(std::unique_ptr<Employee> employee) {
        throw std::runtime_error("Cannot add subordinates to non-manager");
    }
    virtual size_t getTeamSize() const { return 1; }
};

// Leaf: Individual Employee
class IndividualEmployee : public Employee {
private:
    std::string name;
    std::string title;
    double salary;

public:
    IndividualEmployee(const std::string& name, const std::string& title, double salary)
        : name(name), title(title), salary(salary) {}

    void showDetails() const override {
        std::cout << "  Employee: " << name << " (" << title
                  << ") - $" << salary << std::endl;
    }

    std::string getName() const override { return name; }
    std::string getTitle() const override { return title; }
    double getSalary() const override { return salary; }
};

// Composite: Manager
class Manager : public Employee {
private:
    std::string name;
    std::string title;
    double salary;
    std::vector<std::unique_ptr<Employee>> subordinates;

public:
    Manager(const std::string& name, const std::string& title, double salary)
        : name(name), title(title), salary(salary) {}

    void showDetails() const override {
        std::cout << "Manager: " << name << " (" << title
                  << ") - $" << salary << " [Team size: " << getTeamSize() << "]" << std::endl;
        for (const auto& subordinate : subordinates) {
            subordinate->showDetails();
        }
    }

    std::string getName() const override { return name; }
    std::string getTitle() const override { return title; }
    double getSalary() const override { return salary; }

    bool isManager() const override { return true; }

    void addSubordinate(std::unique_ptr<Employee> employee) override {
        subordinates.push_back(std::move(employee));
    }

    size_t getTeamSize() const override {
        size_t totalSize = 1; // Include self
        for (const auto& subordinate : subordinates) {
            totalSize += subordinate->getTeamSize();
        }
        return totalSize;
    }

    double getTotalTeamSalary() const {
        double total = salary;
        for (const auto& subordinate : subordinates) {
            total += subordinate->getSalary();
            if (subordinate->isManager()) {
                // If it's a manager, add their team's salary too
                auto manager = static_cast<const Manager*>(subordinate.get());
                total += manager->getTotalTeamSalary() - subordinate->getSalary(); // Subtract to avoid double counting
            }
        }
        return total;
    }
};

int main() {
    std::cout << "=== Composite Pattern Demo ===" << std::endl;

    // Example 1: File System
    std::cout << "\n1. File System Composite:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    auto root = std::make_unique<Directory>("root");
    auto home = std::make_unique<Directory>("home");
    auto user = std::make_unique<Directory>("user");

    user->add(std::make_unique<File>("document.txt", 1024, "Hello World"));
    user->add(std::make_unique<File>("image.jpg", 2048));

    auto projects = std::make_unique<Directory>("projects");
    projects->add(std::make_unique<File>("main.cpp", 512, "#include <iostream>"));
    projects->add(std::make_unique<File>("Makefile", 256));

    user->add(std::move(projects));
    home->add(std::move(user));
    root->add(std::move(home));

    root->add(std::make_unique<Directory>("var"));
    root->add(std::make_unique<File>("config.ini", 128));

    std::cout << "File system structure:" << std::endl;
    root->display();

    std::cout << "\nTotal size: " << root->getSize() << " bytes" << std::endl;

    // Find a specific file/directory
    auto found = root->find("main.cpp");
    if (found) {
        std::cout << "Found: " << found->getName() << " (" << found->getSize() << " bytes)" << std::endl;
    }

    // Example 2: UI Component System
    std::cout << "\n2. UI Component System:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    auto mainPanel = std::make_unique<Panel>("MainPanel", 0, 0, 800, 600);

    auto headerPanel = std::make_unique<Panel>("HeaderPanel", 0, 0, 800, 100);
    headerPanel->add(std::make_unique<Label>("Application Title", 10, 10));
    headerPanel->add(std::make_unique<Button>("Settings", 700, 10, 80, 30));

    auto contentPanel = std::make_unique<Panel>("ContentPanel", 0, 100, 800, 400);
    contentPanel->add(std::make_unique<Label>("Welcome!", 50, 50));
    contentPanel->add(std::make_unique<Button>("Start", 50, 100, 100, 40));
    contentPanel->add(std::make_unique<Button>("Exit", 200, 100, 100, 40));

    auto footerPanel = std::make_unique<Panel>("FooterPanel", 0, 500, 800, 100);
    footerPanel->add(std::make_unique<Label>("Status: Ready", 10, 10));

    mainPanel->add(std::move(headerPanel));
    mainPanel->add(std::move(contentPanel));
    mainPanel->add(std::move(footerPanel));

    std::cout << "UI Layout:" << std::endl;
    mainPanel->render();

    std::cout << "\nSimulating clicks:" << std::endl;
    mainPanel->handleClick(750, 25); // Settings button
    mainPanel->handleClick(100, 120); // Start button
    mainPanel->handleClick(250, 120); // Exit button

    // Example 3: Mathematical Expression Tree
    std::cout << "\n3. Mathematical Expression Tree:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    // Build expression: (3 + 4) * (5 - 2)
    auto expr = std::make_unique<BinaryOperation>(
        std::make_unique<BinaryOperation>(
            std::make_unique<Number>(3),
            std::make_unique<Number>(4),
            '+'
        ),
        std::make_unique<BinaryOperation>(
            std::make_unique<Number>(5),
            std::make_unique<Number>(2),
            '-'
        ),
        '*'
    );

    std::cout << "Expression: " << expr->toString() << std::endl;
    std::cout << "Result: " << expr->evaluate() << std::endl;

    // More complex expression: ((10 / 2) + 3) * (8 - 6)
    auto complexExpr = std::make_unique<BinaryOperation>(
        std::make_unique<BinaryOperation>(
            std::make_unique<BinaryOperation>(
                std::make_unique<Number>(10),
                std::make_unique<Number>(2),
                '/'
            ),
            std::make_unique<Number>(3),
            '+'
        ),
        std::make_unique<BinaryOperation>(
            std::make_unique<Number>(8),
            std::make_unique<Number>(6),
            '-'
        ),
        '*'
    );

    std::cout << "Complex Expression: " << complexExpr->toString() << std::endl;
    std::cout << "Result: " << complexExpr->evaluate() << std::endl;

    // Example 4: Organization Structure
    std::cout << "\n4. Organization Structure:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    auto ceo = std::make_unique<Manager>("Alice Johnson", "CEO", 200000);

    auto cto = std::make_unique<Manager>("Bob Smith", "CTO", 150000);
    cto->addSubordinate(std::make_unique<IndividualEmployee>("Charlie Brown", "Senior Developer", 90000));
    cto->addSubordinate(std::make_unique<IndividualEmployee>("Diana Prince", "Developer", 75000));

    auto devManager = std::make_unique<Manager>("Eve Wilson", "Dev Manager", 120000);
    devManager->addSubordinate(std::make_unique<IndividualEmployee>("Frank Miller", "Junior Developer", 60000));
    devManager->addSubordinate(std::make_unique<IndividualEmployee>("Grace Lee", "QA Engineer", 65000));

    cto->addSubordinate(std::move(devManager));

    auto cfo = std::make_unique<Manager>("Henry Davis", "CFO", 140000);
    cfo->addSubordinate(std::make_unique<IndividualEmployee>("Iris Taylor", "Accountant", 55000));
    cfo->addSubordinate(std::make_unique<IndividualEmployee>("Jack Wilson", "Financial Analyst", 60000));

    ceo->addSubordinate(std::move(cto));
    ceo->addSubordinate(std::move(cfo));

    std::cout << "Organization Chart:" << std::endl;
    ceo->showDetails();

    std::cout << "\nTotal team size: " << ceo->getTeamSize() << std::endl;
    std::cout << "Total payroll: $" << static_cast<const Manager*>(ceo.get())->getTotalTeamSalary() << std::endl;

    return 0;
}