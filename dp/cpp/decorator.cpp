#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Component interface
class Coffee {
public:
    virtual ~Coffee() = default;
    virtual std::string getDescription() const = 0;
    virtual double getCost() const = 0;
    virtual std::string getSize() const { return "Regular"; }
};

// Concrete Component
class BasicCoffee : public Coffee {
private:
    std::string size;

public:
    explicit BasicCoffee(const std::string& size = "Regular") : size(size) {}

    std::string getDescription() const override {
        return size + " Coffee";
    }

    double getCost() const override {
        if (size == "Small") return 2.50;
        if (size == "Large") return 4.50;
        return 3.50; // Regular
    }

    std::string getSize() const override {
        return size;
    }
};

// Base Decorator
class CoffeeDecorator : public Coffee {
protected:
    std::unique_ptr<Coffee> coffee;

public:
    explicit CoffeeDecorator(std::unique_ptr<Coffee> coffee) : coffee(std::move(coffee)) {}

    std::string getDescription() const override {
        return coffee->getDescription();
    }

    double getCost() const override {
        return coffee->getCost();
    }

    std::string getSize() const override {
        return coffee->getSize();
    }
};

// Concrete Decorators
class MilkDecorator : public CoffeeDecorator {
public:
    explicit MilkDecorator(std::unique_ptr<Coffee> coffee) : CoffeeDecorator(std::move(coffee)) {}

    std::string getDescription() const override {
        return coffee->getDescription() + " + Milk";
    }

    double getCost() const override {
        return coffee->getCost() + 0.60;
    }
};

class SugarDecorator : public CoffeeDecorator {
public:
    explicit SugarDecorator(std::unique_ptr<Coffee> coffee) : CoffeeDecorator(std::move(coffee)) {}

    std::string getDescription() const override {
        return coffee->getDescription() + " + Sugar";
    }

    double getCost() const override {
        return coffee->getCost() + 0.25;
    }
};

class VanillaDecorator : public CoffeeDecorator {
public:
    explicit VanillaDecorator(std::unique_ptr<Coffee> coffee) : CoffeeDecorator(std::move(coffee)) {}

    std::string getDescription() const override {
        return coffee->getDescription() + " + Vanilla";
    }

    double getCost() const override {
        return coffee->getCost() + 0.80;
    }
};

class ChocolateDecorator : public CoffeeDecorator {
public:
    explicit ChocolateDecorator(std::unique_ptr<Coffee> coffee) : CoffeeDecorator(std::move(coffee)) {}

    std::string getDescription() const override {
        return coffee->getDescription() + " + Chocolate";
    }

    double getCost() const override {
        return coffee->getCost() + 1.20;
    }
};

class WhippedCreamDecorator : public CoffeeDecorator {
public:
    explicit WhippedCreamDecorator(std::unique_ptr<Coffee> coffee) : CoffeeDecorator(std::move(coffee)) {}

    std::string getDescription() const override {
        return coffee->getDescription() + " + Whipped Cream";
    }

    double getCost() const override {
        return coffee->getCost() + 0.90;
    }
};

// Example 2: Text Processing Decorators
class TextComponent {
public:
    virtual ~TextComponent() = default;
    virtual std::string getText() const = 0;
    virtual int getLength() const = 0;
};

class PlainText : public TextComponent {
private:
    std::string text;

public:
    explicit PlainText(const std::string& text) : text(text) {}

    std::string getText() const override {
        return text;
    }

    int getLength() const override {
        return text.length();
    }
};

class TextDecorator : public TextComponent {
protected:
    std::unique_ptr<TextComponent> component;

public:
    explicit TextDecorator(std::unique_ptr<TextComponent> component)
        : component(std::move(component)) {}

    std::string getText() const override {
        return component->getText();
    }

    int getLength() const override {
        return component->getLength();
    }
};

class BoldDecorator : public TextDecorator {
public:
    explicit BoldDecorator(std::unique_ptr<TextComponent> component)
        : TextDecorator(std::move(component)) {}

    std::string getText() const override {
        return "<b>" + component->getText() + "</b>";
    }

    int getLength() const override {
        return component->getLength() + 7; // <b></b>
    }
};

class ItalicDecorator : public TextDecorator {
public:
    explicit ItalicDecorator(std::unique_ptr<TextComponent> component)
        : TextDecorator(std::move(component)) {}

    std::string getText() const override {
        return "<i>" + component->getText() + "</i>";
    }

    int getLength() const override {
        return component->getLength() + 7; // <i></i>
    }
};

class UnderlineDecorator : public TextDecorator {
public:
    explicit UnderlineDecorator(std::unique_ptr<TextComponent> component)
        : TextDecorator(std::move(component)) {}

    std::string getText() const override {
        return "<u>" + component->getText() + "</u>";
    }

    int getLength() const override {
        return component->getLength() + 7; // <u></u>
    }
};

class ColorDecorator : public TextDecorator {
private:
    std::string color;

public:
    ColorDecorator(std::unique_ptr<TextComponent> component, const std::string& color)
        : TextDecorator(std::move(component)), color(color) {}

    std::string getText() const override {
        return "<span style=\"color:" + color + "\">" + component->getText() + "</span>";
    }

    int getLength() const override {
        return component->getLength() + 27 + color.length(); // HTML tags
    }
};

// Example 3: Data Stream Processing
class DataStream {
public:
    virtual ~DataStream() = default;
    virtual std::string read() = 0;
    virtual void write(const std::string& data) = 0;
    virtual std::string getInfo() const = 0;
};

class FileDataStream : public DataStream {
private:
    std::string filename;
    std::string data;

public:
    explicit FileDataStream(const std::string& filename)
        : filename(filename), data("Sample file content: " + filename) {}

    std::string read() override {
        return data;
    }

    void write(const std::string& newData) override {
        data = newData;
    }

    std::string getInfo() const override {
        return "File: " + filename;
    }
};

class DataStreamDecorator : public DataStream {
protected:
    std::unique_ptr<DataStream> stream;

public:
    explicit DataStreamDecorator(std::unique_ptr<DataStream> stream)
        : stream(std::move(stream)) {}

    std::string read() override {
        return stream->read();
    }

    void write(const std::string& data) override {
        stream->write(data);
    }

    std::string getInfo() const override {
        return stream->getInfo();
    }
};

class EncryptionDecorator : public DataStreamDecorator {
public:
    explicit EncryptionDecorator(std::unique_ptr<DataStream> stream)
        : DataStreamDecorator(std::move(stream)) {}

    std::string read() override {
        std::string data = stream->read();
        return decrypt(data);
    }

    void write(const std::string& data) override {
        stream->write(encrypt(data));
    }

    std::string getInfo() const override {
        return stream->getInfo() + " [Encrypted]";
    }

private:
    std::string encrypt(const std::string& data) {
        // Simple Caesar cipher for demonstration
        std::string encrypted = data;
        for (char& c : encrypted) {
            if (std::isalpha(c)) {
                c = ((c - 'a' + 3) % 26) + 'a';
            }
        }
        return encrypted;
    }

    std::string decrypt(const std::string& data) {
        // Reverse Caesar cipher
        std::string decrypted = data;
        for (char& c : decrypted) {
            if (std::isalpha(c)) {
                c = ((c - 'a' - 3 + 26) % 26) + 'a';
            }
        }
        return decrypted;
    }
};

class CompressionDecorator : public DataStreamDecorator {
public:
    explicit CompressionDecorator(std::unique_ptr<DataStream> stream)
        : DataStreamDecorator(std::move(stream)) {}

    std::string read() override {
        std::string data = stream->read();
        return decompress(data);
    }

    void write(const std::string& data) override {
        stream->write(compress(data));
    }

    std::string getInfo() const override {
        return stream->getInfo() + " [Compressed]";
    }

private:
    std::string compress(const std::string& data) {
        // Simple run-length encoding simulation
        return "[COMPRESSED]" + data;
    }

    std::string decompress(const std::string& data) {
        // Remove compression marker
        if (data.substr(0, 12) == "[COMPRESSED]") {
            return data.substr(12);
        }
        return data;
    }
};

class LoggingDecorator : public DataStreamDecorator {
public:
    explicit LoggingDecorator(std::unique_ptr<DataStream> stream)
        : DataStreamDecorator(std::move(stream)) {}

    std::string read() override {
        std::cout << "[LOG] Reading from " << stream->getInfo() << std::endl;
        return stream->read();
    }

    void write(const std::string& data) override {
        std::cout << "[LOG] Writing to " << stream->getInfo() << ": " << data.length() << " bytes" << std::endl;
        stream->write(data);
    }

    std::string getInfo() const override {
        return stream->getInfo() + " [Logged]";
    }
};

// Coffee Order Builder for convenience
class CoffeeOrderBuilder {
private:
    std::unique_ptr<Coffee> coffee;

public:
    explicit CoffeeOrderBuilder(const std::string& size = "Regular") {
        coffee = std::make_unique<BasicCoffee>(size);
    }

    CoffeeOrderBuilder& addMilk() {
        coffee = std::make_unique<MilkDecorator>(std::move(coffee));
        return *this;
    }

    CoffeeOrderBuilder& addSugar() {
        coffee = std::make_unique<SugarDecorator>(std::move(coffee));
        return *this;
    }

    CoffeeOrderBuilder& addVanilla() {
        coffee = std::make_unique<VanillaDecorator>(std::move(coffee));
        return *this;
    }

    CoffeeOrderBuilder& addChocolate() {
        coffee = std::make_unique<ChocolateDecorator>(std::move(coffee));
        return *this;
    }

    CoffeeOrderBuilder& addWhippedCream() {
        coffee = std::make_unique<WhippedCreamDecorator>(std::move(coffee));
        return *this;
    }

    std::unique_ptr<Coffee> build() {
        return std::move(coffee);
    }
};

void printCoffeeOrder(const Coffee& coffee) {
    std::cout << "Order: " << coffee.getDescription() << std::endl;
    std::cout << "Cost: $" << std::fixed << std::setprecision(2) << coffee.getCost() << std::endl;
    std::cout << "Size: " << coffee.getSize() << std::endl;
    std::cout << std::string(40, '-') << std::endl;
}

int main() {
    std::cout << "=== Decorator Pattern Demo ===" << std::endl;

    // Example 1: Coffee Shop
    std::cout << "\n1. Coffee Shop Orders:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    // Basic coffee
    auto basicCoffee = std::make_unique<BasicCoffee>("Large");
    printCoffeeOrder(*basicCoffee);

    // Coffee with milk
    auto coffeeWithMilk = std::make_unique<MilkDecorator>(
        std::make_unique<BasicCoffee>("Regular"));
    printCoffeeOrder(*coffeeWithMilk);

    // Complex coffee order
    auto complexCoffee = std::make_unique<WhippedCreamDecorator>(
        std::make_unique<ChocolateDecorator>(
            std::make_unique<VanillaDecorator>(
                std::make_unique<MilkDecorator>(
                    std::make_unique<SugarDecorator>(
                        std::make_unique<BasicCoffee>("Large"))))));
    printCoffeeOrder(*complexCoffee);

    // Using builder pattern with decorator
    std::cout << "\nUsing Coffee Builder:" << std::endl;
    auto builderCoffee = CoffeeOrderBuilder("Small")
        .addMilk()
        .addSugar()
        .addVanilla()
        .build();
    printCoffeeOrder(*builderCoffee);

    // Example 2: Text Formatting
    std::cout << "\n2. Text Formatting:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    auto plainText = std::make_unique<PlainText>("Hello World");
    std::cout << "Plain: " << plainText->getText() << " (length: " << plainText->getLength() << ")" << std::endl;

    auto boldText = std::make_unique<BoldDecorator>(
        std::make_unique<PlainText>("Hello World"));
    std::cout << "Bold: " << boldText->getText() << " (length: " << boldText->getLength() << ")" << std::endl;

    auto styledText = std::make_unique<ColorDecorator>(
        std::make_unique<UnderlineDecorator>(
            std::make_unique<ItalicDecorator>(
                std::make_unique<BoldDecorator>(
                    std::make_unique<PlainText>("Styled Text")))), "red");
    std::cout << "Styled: " << styledText->getText() << std::endl;
    std::cout << "Length: " << styledText->getLength() << std::endl;

    // Example 3: Data Stream Processing
    std::cout << "\n3. Data Stream Processing:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    // Basic file stream
    auto fileStream = std::make_unique<FileDataStream>("data.txt");
    std::cout << "Basic stream info: " << fileStream->getInfo() << std::endl;
    std::cout << "Content: " << fileStream->read() << std::endl;

    // Add logging
    auto loggedStream = std::make_unique<LoggingDecorator>(
        std::make_unique<FileDataStream>("logged_data.txt"));
    std::cout << "\nLogged stream:" << std::endl;
    std::cout << "Info: " << loggedStream->getInfo() << std::endl;
    std::string content = loggedStream->read();
    std::cout << "Content: " << content << std::endl;

    // Complex stream with multiple decorators
    std::cout << "\nComplex stream (File -> Compression -> Encryption -> Logging):" << std::endl;
    auto complexStream = std::make_unique<LoggingDecorator>(
        std::make_unique<EncryptionDecorator>(
            std::make_unique<CompressionDecorator>(
                std::make_unique<FileDataStream>("secure_data.txt"))));

    std::cout << "Info: " << complexStream->getInfo() << std::endl;
    complexStream->write("This is sensitive data that needs protection");
    std::string secureContent = complexStream->read();
    std::cout << "Retrieved content: " << secureContent << std::endl;

    // Example 4: Multiple coffee orders
    std::cout << "\n4. Coffee Shop Menu Simulation:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    std::vector<std::unique_ptr<Coffee>> orders;

    // Create various orders
    orders.push_back(CoffeeOrderBuilder("Small").addSugar().build());
    orders.push_back(CoffeeOrderBuilder("Regular").addMilk().addVanilla().build());
    orders.push_back(CoffeeOrderBuilder("Large").addChocolate().addWhippedCream().build());
    orders.push_back(CoffeeOrderBuilder("Regular")
        .addMilk().addSugar().addVanilla().addChocolate().addWhippedCream().build());

    double totalRevenue = 0.0;
    for (size_t i = 0; i < orders.size(); ++i) {
        std::cout << "Order #" << (i + 1) << ":" << std::endl;
        std::cout << "  " << orders[i]->getDescription() << std::endl;
        std::cout << "  Cost: $" << std::fixed << std::setprecision(2) << orders[i]->getCost() << std::endl;
        totalRevenue += orders[i]->getCost();
    }

    std::cout << "\nTotal Revenue: $" << std::fixed << std::setprecision(2) << totalRevenue << std::endl;

    return 0;
}