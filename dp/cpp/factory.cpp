#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

// Abstract Product
class Vehicle {
public:
    virtual ~Vehicle() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual std::string getType() const = 0;
};

// Concrete Products
class Car : public Vehicle {
private:
    std::string model;

public:
    explicit Car(const std::string& model) : model(model) {}

    void start() override {
        std::cout << "Car " << model << " engine started with key ignition" << std::endl;
    }

    void stop() override {
        std::cout << "Car " << model << " engine stopped" << std::endl;
    }

    std::string getType() const override {
        return "Car (" + model + ")";
    }
};

class Motorcycle : public Vehicle {
private:
    std::string brand;

public:
    explicit Motorcycle(const std::string& brand) : brand(brand) {}

    void start() override {
        std::cout << "Motorcycle " << brand << " engine started with kick start" << std::endl;
    }

    void stop() override {
        std::cout << "Motorcycle " << brand << " engine stopped" << std::endl;
    }

    std::string getType() const override {
        return "Motorcycle (" + brand + ")";
    }
};

class Truck : public Vehicle {
private:
    int capacity;

public:
    explicit Truck(int capacity) : capacity(capacity) {}

    void start() override {
        std::cout << "Truck with " << capacity << "T capacity engine started" << std::endl;
    }

    void stop() override {
        std::cout << "Truck engine stopped" << std::endl;
    }

    std::string getType() const override {
        return "Truck (" + std::to_string(capacity) + "T)";
    }
};

// Factory using function pointers and lambdas (Modern C++)
class VehicleFactory {
private:
    using Creator = std::function<std::unique_ptr<Vehicle>(const std::string&)>;
    std::unordered_map<std::string, Creator> creators;

public:
    VehicleFactory() {
        // Register creators using lambdas
        creators["car"] = [](const std::string& param) -> std::unique_ptr<Vehicle> {
            return std::make_unique<Car>(param.empty() ? "Generic Car" : param);
        };

        creators["motorcycle"] = [](const std::string& param) -> std::unique_ptr<Vehicle> {
            return std::make_unique<Motorcycle>(param.empty() ? "Generic Bike" : param);
        };

        creators["truck"] = [](const std::string& param) -> std::unique_ptr<Vehicle> {
            int capacity = param.empty() ? 10 : std::stoi(param);
            return std::make_unique<Truck>(capacity);
        };
    }

    std::unique_ptr<Vehicle> createVehicle(const std::string& type, const std::string& param = "") {
        auto it = creators.find(type);
        if (it != creators.end()) {
            return it->second(param);
        }
        throw std::invalid_argument("Unknown vehicle type: " + type);
    }

    void registerCreator(const std::string& type, Creator creator) {
        creators[type] = std::move(creator);
    }

    auto getAvailableTypes() const {
        std::vector<std::string> types;
        for (const auto& [type, _] : creators) {
            types.push_back(type);
        }
        return types;
    }
};

// Simple Factory (alternative approach)
class SimpleVehicleFactory {
public:
    enum class VehicleType { CAR, MOTORCYCLE, TRUCK };

    static std::unique_ptr<Vehicle> createVehicle(VehicleType type, const std::string& param = "") {
        switch (type) {
            case VehicleType::CAR:
                return std::make_unique<Car>(param.empty() ? "Default Car" : param);
            case VehicleType::MOTORCYCLE:
                return std::make_unique<Motorcycle>(param.empty() ? "Default Bike" : param);
            case VehicleType::TRUCK:
                return std::make_unique<Truck>(param.empty() ? 15 : std::stoi(param));
            default:
                throw std::invalid_argument("Unknown vehicle type");
        }
    }
};

void demonstrateVehicle(std::unique_ptr<Vehicle> vehicle) {
    std::cout << "Created: " << vehicle->getType() << std::endl;
    vehicle->start();
    vehicle->stop();
    std::cout << "---" << std::endl;
}

int main() {
    std::cout << "=== Factory Pattern Demo ===" << std::endl;

    try {
        // Using the modern factory with lambdas
        VehicleFactory factory;

        std::cout << "Available vehicle types: ";
        for (const auto& type : factory.getAvailableTypes()) {
            std::cout << type << " ";
        }
        std::cout << std::endl << std::endl;

        // Create different vehicles
        auto car = factory.createVehicle("car", "Toyota Camry");
        demonstrateVehicle(std::move(car));

        auto motorcycle = factory.createVehicle("motorcycle", "Harley Davidson");
        demonstrateVehicle(std::move(motorcycle));

        auto truck = factory.createVehicle("truck", "25");
        demonstrateVehicle(std::move(truck));

        // Register a new vehicle type at runtime
        factory.registerCreator("electric_car", [](const std::string& param) -> std::unique_ptr<Vehicle> {
            class ElectricCar : public Vehicle {
                std::string model;
            public:
                explicit ElectricCar(const std::string& m) : model(m) {}
                void start() override { std::cout << "Electric car " << model << " started silently" << std::endl; }
                void stop() override { std::cout << "Electric car " << model << " stopped" << std::endl; }
                std::string getType() const override { return "Electric Car (" + model + ")"; }
            };
            return std::make_unique<ElectricCar>(param.empty() ? "Tesla Model 3" : param);
        });

        auto electric = factory.createVehicle("electric_car", "Tesla Model S");
        demonstrateVehicle(std::move(electric));

        std::cout << "\nUsing Simple Factory:" << std::endl;

        // Using simple factory
        auto simpleCar = SimpleVehicleFactory::createVehicle(
            SimpleVehicleFactory::VehicleType::CAR, "BMW X5");
        demonstrateVehicle(std::move(simpleCar));

        auto simpleBike = SimpleVehicleFactory::createVehicle(
            SimpleVehicleFactory::VehicleType::MOTORCYCLE, "Yamaha R1");
        demonstrateVehicle(std::move(simpleBike));

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}