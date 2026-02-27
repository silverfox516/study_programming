#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Abstract Products
class Button {
public:
    virtual ~Button() = default;
    virtual void render() = 0;
    virtual void onClick() = 0;
    virtual std::string getStyle() const = 0;
};

class TextField {
public:
    virtual ~TextField() = default;
    virtual void render() = 0;
    virtual void setValue(const std::string& value) = 0;
    virtual std::string getValue() const = 0;
    virtual std::string getStyle() const = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual void render() = 0;
    virtual void setChecked(bool checked) = 0;
    virtual bool isChecked() const = 0;
    virtual std::string getStyle() const = 0;
};

// Windows Style Concrete Products
class WindowsButton : public Button {
private:
    std::string text;

public:
    explicit WindowsButton(const std::string& text) : text(text) {}

    void render() override {
        std::cout << "[Windows Button: " << text << "]" << std::endl;
    }

    void onClick() override {
        std::cout << "Windows button '" << text << "' clicked with mouse" << std::endl;
    }

    std::string getStyle() const override {
        return "Windows";
    }
};

class WindowsTextField : public TextField {
private:
    std::string value;

public:
    WindowsTextField() = default;

    void render() override {
        std::cout << "[Windows TextField: " << value << "]" << std::endl;
    }

    void setValue(const std::string& val) override {
        value = val;
    }

    std::string getValue() const override {
        return value;
    }

    std::string getStyle() const override {
        return "Windows";
    }
};

class WindowsCheckbox : public Checkbox {
private:
    bool checked = false;

public:
    void render() override {
        std::cout << "[Windows Checkbox: " << (checked ? "☑" : "☐") << "]" << std::endl;
    }

    void setChecked(bool check) override {
        checked = check;
    }

    bool isChecked() const override {
        return checked;
    }

    std::string getStyle() const override {
        return "Windows";
    }
};

// macOS Style Concrete Products
class MacButton : public Button {
private:
    std::string text;

public:
    explicit MacButton(const std::string& text) : text(text) {}

    void render() override {
        std::cout << "( " << text << " )" << std::endl;
    }

    void onClick() override {
        std::cout << "Mac button '" << text << "' clicked with trackpad" << std::endl;
    }

    std::string getStyle() const override {
        return "macOS";
    }
};

class MacTextField : public TextField {
private:
    std::string value;

public:
    MacTextField() = default;

    void render() override {
        std::cout << "│ " << value << " │" << std::endl;
    }

    void setValue(const std::string& val) override {
        value = val;
    }

    std::string getValue() const override {
        return value;
    }

    std::string getStyle() const override {
        return "macOS";
    }
};

class MacCheckbox : public Checkbox {
private:
    bool checked = false;

public:
    void render() override {
        std::cout << (checked ? "✓" : "○") << " Mac checkbox" << std::endl;
    }

    void setChecked(bool check) override {
        checked = check;
    }

    bool isChecked() const override {
        return checked;
    }

    std::string getStyle() const override {
        return "macOS";
    }
};

// Linux Style Concrete Products
class LinuxButton : public Button {
private:
    std::string text;

public:
    explicit LinuxButton(const std::string& text) : text(text) {}

    void render() override {
        std::cout << "< " << text << " >" << std::endl;
    }

    void onClick() override {
        std::cout << "Linux button '" << text << "' clicked" << std::endl;
    }

    std::string getStyle() const override {
        return "Linux";
    }
};

class LinuxTextField : public TextField {
private:
    std::string value;

public:
    LinuxTextField() = default;

    void render() override {
        std::cout << "[ " << value << " ]" << std::endl;
    }

    void setValue(const std::string& val) override {
        value = val;
    }

    std::string getValue() const override {
        return value;
    }

    std::string getStyle() const override {
        return "Linux";
    }
};

class LinuxCheckbox : public Checkbox {
private:
    bool checked = false;

public:
    void render() override {
        std::cout << "[" << (checked ? "x" : " ") << "] Linux checkbox" << std::endl;
    }

    void setChecked(bool check) override {
        checked = check;
    }

    bool isChecked() const override {
        return checked;
    }

    std::string getStyle() const override {
        return "Linux";
    }
};

// Abstract Factory
class UIFactory {
public:
    virtual ~UIFactory() = default;
    virtual std::unique_ptr<Button> createButton(const std::string& text) = 0;
    virtual std::unique_ptr<TextField> createTextField() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
    virtual std::string getTheme() const = 0;
};

// Concrete Factories
class WindowsUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton(const std::string& text) override {
        return std::make_unique<WindowsButton>(text);
    }

    std::unique_ptr<TextField> createTextField() override {
        return std::make_unique<WindowsTextField>();
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<WindowsCheckbox>();
    }

    std::string getTheme() const override {
        return "Windows";
    }
};

class MacUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton(const std::string& text) override {
        return std::make_unique<MacButton>(text);
    }

    std::unique_ptr<TextField> createTextField() override {
        return std::make_unique<MacTextField>();
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<MacCheckbox>();
    }

    std::string getTheme() const override {
        return "macOS";
    }
};

class LinuxUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton(const std::string& text) override {
        return std::make_unique<LinuxButton>(text);
    }

    std::unique_ptr<TextField> createTextField() override {
        return std::make_unique<LinuxTextField>();
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<LinuxCheckbox>();
    }

    std::string getTheme() const override {
        return "Linux";
    }
};

// Client class that uses the factory
class Application {
private:
    std::unique_ptr<UIFactory> factory;
    std::vector<std::unique_ptr<Button>> buttons;
    std::vector<std::unique_ptr<TextField>> textFields;
    std::vector<std::unique_ptr<Checkbox>> checkboxes;

public:
    explicit Application(std::unique_ptr<UIFactory> uiFactory)
        : factory(std::move(uiFactory)) {}

    void createUI() {
        std::cout << "Creating UI with " << factory->getTheme() << " theme:" << std::endl;

        // Create UI components
        buttons.push_back(factory->createButton("OK"));
        buttons.push_back(factory->createButton("Cancel"));

        textFields.push_back(factory->createTextField());
        textFields.push_back(factory->createTextField());

        checkboxes.push_back(factory->createCheckbox());
        checkboxes.push_back(factory->createCheckbox());

        // Set some initial values
        textFields[0]->setValue("Username");
        textFields[1]->setValue("Password");
        checkboxes[0]->setChecked(true);
        checkboxes[1]->setChecked(false);
    }

    void renderUI() {
        std::cout << "\nRendering " << factory->getTheme() << " UI:" << std::endl;
        std::cout << "------------------------" << std::endl;

        for (auto& textField : textFields) {
            textField->render();
        }

        for (auto& checkbox : checkboxes) {
            checkbox->render();
        }

        for (auto& button : buttons) {
            button->render();
        }
    }

    void simulateInteraction() {
        std::cout << "\nSimulating user interaction:" << std::endl;
        if (!buttons.empty()) {
            buttons[0]->onClick();
        }
        if (!textFields.empty()) {
            textFields[0]->setValue("john_doe");
            std::cout << "Text field updated to: " << textFields[0]->getValue() << std::endl;
        }
        if (!checkboxes.empty()) {
            checkboxes[1]->setChecked(true);
            std::cout << "Checkbox 2 is now: " << (checkboxes[1]->isChecked() ? "checked" : "unchecked") << std::endl;
        }
    }
};

// Factory method to create the appropriate factory based on OS
std::unique_ptr<UIFactory> createUIFactory(const std::string& os) {
    if (os == "Windows") {
        return std::make_unique<WindowsUIFactory>();
    } else if (os == "macOS") {
        return std::make_unique<MacUIFactory>();
    } else if (os == "Linux") {
        return std::make_unique<LinuxUIFactory>();
    } else {
        throw std::invalid_argument("Unsupported OS: " + os);
    }
}

int main() {
    std::cout << "=== Abstract Factory Pattern Demo ===" << std::endl;

    std::vector<std::string> platforms = {"Windows", "macOS", "Linux"};

    for (const auto& platform : platforms) {
        try {
            std::cout << "\n" << std::string(50, '=') << std::endl;

            auto factory = createUIFactory(platform);
            Application app(std::move(factory));

            app.createUI();
            app.renderUI();
            app.simulateInteraction();

        } catch (const std::exception& e) {
            std::cerr << "Error creating UI for " << platform << ": " << e.what() << std::endl;
        }
    }

    return 0;
}