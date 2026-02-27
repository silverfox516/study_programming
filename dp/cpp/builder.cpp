#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <sstream>

// Product class
class Computer {
private:
    std::string cpu;
    std::string gpu;
    int ram_gb;
    int storage_gb;
    std::string storage_type;
    std::string motherboard;
    std::string power_supply;
    std::vector<std::string> peripherals;
    std::optional<std::string> operating_system;
    bool wifi_enabled;
    bool bluetooth_enabled;

public:
    Computer() : ram_gb(0), storage_gb(0), wifi_enabled(false), bluetooth_enabled(false) {}

    // Setters (used by builder)
    void setCPU(const std::string& cpu) { this->cpu = cpu; }
    void setGPU(const std::string& gpu) { this->gpu = gpu; }
    void setRAM(int gb) { this->ram_gb = gb; }
    void setStorage(int gb, const std::string& type) {
        this->storage_gb = gb;
        this->storage_type = type;
    }
    void setMotherboard(const std::string& mb) { this->motherboard = mb; }
    void setPowerSupply(const std::string& psu) { this->power_supply = psu; }
    void addPeripheral(const std::string& peripheral) {
        this->peripherals.push_back(peripheral);
    }
    void setOperatingSystem(const std::string& os) { this->operating_system = os; }
    void enableWifi(bool enable) { this->wifi_enabled = enable; }
    void enableBluetooth(bool enable) { this->bluetooth_enabled = enable; }

    std::string getSpecifications() const {
        std::ostringstream specs;
        specs << "Computer Specifications:\n";
        specs << "  CPU: " << cpu << "\n";
        specs << "  GPU: " << gpu << "\n";
        specs << "  RAM: " << ram_gb << " GB\n";
        specs << "  Storage: " << storage_gb << " GB " << storage_type << "\n";
        specs << "  Motherboard: " << motherboard << "\n";
        specs << "  Power Supply: " << power_supply << "\n";

        if (operating_system.has_value()) {
            specs << "  OS: " << operating_system.value() << "\n";
        }

        specs << "  WiFi: " << (wifi_enabled ? "Enabled" : "Disabled") << "\n";
        specs << "  Bluetooth: " << (bluetooth_enabled ? "Enabled" : "Disabled") << "\n";

        if (!peripherals.empty()) {
            specs << "  Peripherals: ";
            for (size_t i = 0; i < peripherals.size(); ++i) {
                specs << peripherals[i];
                if (i < peripherals.size() - 1) specs << ", ";
            }
            specs << "\n";
        }

        return specs.str();
    }
};

// Abstract Builder Interface
class ComputerBuilder {
protected:
    std::unique_ptr<Computer> computer;

public:
    ComputerBuilder() { reset(); }
    virtual ~ComputerBuilder() = default;

    void reset() {
        computer = std::make_unique<Computer>();
    }

    virtual ComputerBuilder& setCPU(const std::string& cpu) = 0;
    virtual ComputerBuilder& setGPU(const std::string& gpu) = 0;
    virtual ComputerBuilder& setRAM(int gb) = 0;
    virtual ComputerBuilder& setStorage(int gb, const std::string& type) = 0;
    virtual ComputerBuilder& setMotherboard(const std::string& mb) = 0;
    virtual ComputerBuilder& setPowerSupply(const std::string& psu) = 0;
    virtual ComputerBuilder& addPeripheral(const std::string& peripheral) = 0;
    virtual ComputerBuilder& setOperatingSystem(const std::string& os) = 0;
    virtual ComputerBuilder& enableWifi(bool enable = true) = 0;
    virtual ComputerBuilder& enableBluetooth(bool enable = true) = 0;

    std::unique_ptr<Computer> build() {
        return std::move(computer);
    }
};

// Concrete Builder for Gaming Computer
class GamingComputerBuilder : public ComputerBuilder {
public:
    ComputerBuilder& setCPU(const std::string& cpu) override {
        computer->setCPU(cpu);
        return *this;
    }

    ComputerBuilder& setGPU(const std::string& gpu) override {
        computer->setGPU(gpu);
        return *this;
    }

    ComputerBuilder& setRAM(int gb) override {
        computer->setRAM(gb);
        return *this;
    }

    ComputerBuilder& setStorage(int gb, const std::string& type) override {
        computer->setStorage(gb, type);
        return *this;
    }

    ComputerBuilder& setMotherboard(const std::string& mb) override {
        computer->setMotherboard(mb);
        return *this;
    }

    ComputerBuilder& setPowerSupply(const std::string& psu) override {
        computer->setPowerSupply(psu);
        return *this;
    }

    ComputerBuilder& addPeripheral(const std::string& peripheral) override {
        computer->addPeripheral(peripheral);
        return *this;
    }

    ComputerBuilder& setOperatingSystem(const std::string& os) override {
        computer->setOperatingSystem(os);
        return *this;
    }

    ComputerBuilder& enableWifi(bool enable = true) override {
        computer->enableWifi(enable);
        return *this;
    }

    ComputerBuilder& enableBluetooth(bool enable = true) override {
        computer->enableBluetooth(enable);
        return *this;
    }

    // Gaming-specific method
    GamingComputerBuilder& buildHighEndGaming() {
        setCPU("Intel i9-13900K")
            .setGPU("NVIDIA RTX 4090")
            .setRAM(32)
            .setStorage(2000, "NVMe SSD")
            .setMotherboard("ASUS ROG Maximus Z790")
            .setPowerSupply("850W 80+ Gold")
            .addPeripheral("Gaming Keyboard")
            .addPeripheral("Gaming Mouse")
            .addPeripheral("144Hz Monitor")
            .setOperatingSystem("Windows 11")
            .enableWifi()
            .enableBluetooth();
        return *this;
    }
};

// Concrete Builder for Office Computer
class OfficeComputerBuilder : public ComputerBuilder {
public:
    ComputerBuilder& setCPU(const std::string& cpu) override {
        computer->setCPU(cpu);
        return *this;
    }

    ComputerBuilder& setGPU(const std::string& gpu) override {
        computer->setGPU(gpu);
        return *this;
    }

    ComputerBuilder& setRAM(int gb) override {
        computer->setRAM(gb);
        return *this;
    }

    ComputerBuilder& setStorage(int gb, const std::string& type) override {
        computer->setStorage(gb, type);
        return *this;
    }

    ComputerBuilder& setMotherboard(const std::string& mb) override {
        computer->setMotherboard(mb);
        return *this;
    }

    ComputerBuilder& setPowerSupply(const std::string& psu) override {
        computer->setPowerSupply(psu);
        return *this;
    }

    ComputerBuilder& addPeripheral(const std::string& peripheral) override {
        computer->addPeripheral(peripheral);
        return *this;
    }

    ComputerBuilder& setOperatingSystem(const std::string& os) override {
        computer->setOperatingSystem(os);
        return *this;
    }

    ComputerBuilder& enableWifi(bool enable = true) override {
        computer->enableWifi(enable);
        return *this;
    }

    ComputerBuilder& enableBluetooth(bool enable = true) override {
        computer->enableBluetooth(enable);
        return *this;
    }

    // Office-specific method
    OfficeComputerBuilder& buildStandardOffice() {
        setCPU("Intel i5-12400")
            .setGPU("Integrated Graphics")
            .setRAM(16)
            .setStorage(512, "SSD")
            .setMotherboard("Standard ATX")
            .setPowerSupply("500W 80+ Bronze")
            .addPeripheral("Standard Keyboard")
            .addPeripheral("Optical Mouse")
            .addPeripheral("24-inch Monitor")
            .setOperatingSystem("Windows 11 Pro")
            .enableWifi()
            .enableBluetooth();
        return *this;
    }
};

// Director class that knows how to construct specific types
class ComputerDirector {
public:
    std::unique_ptr<Computer> buildBudgetGaming(GamingComputerBuilder& builder) {
        builder.reset();
        return builder.setCPU("AMD Ryzen 5 5600X")
                     .setGPU("NVIDIA RTX 3060")
                     .setRAM(16)
                     .setStorage(1000, "NVMe SSD")
                     .setMotherboard("MSI B550M Pro")
                     .setPowerSupply("650W 80+ Bronze")
                     .addPeripheral("Gaming Keyboard")
                     .addPeripheral("Gaming Mouse")
                     .setOperatingSystem("Windows 11")
                     .enableWifi()
                     .build();
    }

    std::unique_ptr<Computer> buildWorkstation(OfficeComputerBuilder& builder) {
        builder.reset();
        return builder.setCPU("Intel Xeon W-2295")
                     .setGPU("NVIDIA Quadro RTX 4000")
                     .setRAM(64)
                     .setStorage(2000, "NVMe SSD")
                     .setMotherboard("Workstation Motherboard")
                     .setPowerSupply("1000W 80+ Platinum")
                     .addPeripheral("Professional Keyboard")
                     .addPeripheral("Precision Mouse")
                     .addPeripheral("4K Monitor")
                     .setOperatingSystem("Windows 11 Pro")
                     .enableWifi()
                     .enableBluetooth()
                     .build();
    }
};

// Modern C++ style with fluent interface (method chaining)
class ModernComputerBuilder {
private:
    std::unique_ptr<Computer> computer;

public:
    ModernComputerBuilder() : computer(std::make_unique<Computer>()) {}

    auto& cpu(const std::string& cpu) {
        computer->setCPU(cpu);
        return *this;
    }

    auto& gpu(const std::string& gpu) {
        computer->setGPU(gpu);
        return *this;
    }

    auto& ram(int gb) {
        computer->setRAM(gb);
        return *this;
    }

    auto& storage(int gb, const std::string& type = "SSD") {
        computer->setStorage(gb, type);
        return *this;
    }

    auto& motherboard(const std::string& mb) {
        computer->setMotherboard(mb);
        return *this;
    }

    auto& powerSupply(const std::string& psu) {
        computer->setPowerSupply(psu);
        return *this;
    }

    auto& peripheral(const std::string& peripheral) {
        computer->addPeripheral(peripheral);
        return *this;
    }

    auto& os(const std::string& operating_system) {
        computer->setOperatingSystem(operating_system);
        return *this;
    }

    auto& wifi(bool enable = true) {
        computer->enableWifi(enable);
        return *this;
    }

    auto& bluetooth(bool enable = true) {
        computer->enableBluetooth(enable);
        return *this;
    }

    std::unique_ptr<Computer> build() {
        return std::move(computer);
    }
};

int main() {
    std::cout << "=== Builder Pattern Demo ===" << std::endl;

    // Using traditional builder pattern
    std::cout << "\n1. Gaming Computer (High-End):" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    GamingComputerBuilder gamingBuilder;
    auto gamingPC = gamingBuilder.buildHighEndGaming().build();
    std::cout << gamingPC->getSpecifications() << std::endl;

    // Using director
    std::cout << "\n2. Budget Gaming Computer (using Director):" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    ComputerDirector director;
    GamingComputerBuilder budgetBuilder;
    auto budgetGaming = director.buildBudgetGaming(budgetBuilder);
    std::cout << budgetGaming->getSpecifications() << std::endl;

    // Using office builder
    std::cout << "\n3. Office Computer:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    OfficeComputerBuilder officeBuilder;
    auto officePC = officeBuilder.buildStandardOffice().build();
    std::cout << officePC->getSpecifications() << std::endl;

    // Using workstation with director
    std::cout << "\n4. Workstation (using Director):" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    OfficeComputerBuilder workstationBuilder;
    auto workstation = director.buildWorkstation(workstationBuilder);
    std::cout << workstation->getSpecifications() << std::endl;

    // Using modern fluent builder
    std::cout << "\n5. Custom Computer (Modern Builder):" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    auto customPC = ModernComputerBuilder()
        .cpu("AMD Ryzen 9 7950X")
        .gpu("NVIDIA RTX 4080")
        .ram(32)
        .storage(1000, "NVMe SSD")
        .storage(4000, "HDD")  // Multiple storage devices
        .motherboard("ASUS X670E")
        .powerSupply("750W 80+ Gold")
        .peripheral("Mechanical Keyboard")
        .peripheral("Wireless Mouse")
        .peripheral("Ultrawide Monitor")
        .peripheral("Webcam")
        .os("Linux Ubuntu 22.04")
        .wifi()
        .bluetooth()
        .build();

    std::cout << customPC->getSpecifications() << std::endl;

    return 0;
}