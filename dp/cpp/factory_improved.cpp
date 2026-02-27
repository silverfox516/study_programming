#include "factory.hpp"
#include <iostream>
#include <exception>

using namespace DesignPatterns;

int main() {
    std::cout << "=== Modern Factory Pattern Demo ===\n";

    try {
        // Using the modern factory with lambdas
        VehicleFactory factory;

        std::cout << "Available vehicle types: ";
        for (const auto& type : factory.getAvailableTypes()) {
            std::cout << type << " ";
        }
        std::cout << "\n\n";

        // Create different vehicles with improved error handling
        auto car = factory.createVehicle("car", "Toyota Camry");
        demonstrateVehicle(std::move(car));

        auto motorcycle = factory.createVehicle("motorcycle", "Harley Davidson");
        demonstrateVehicle(std::move(motorcycle));

        auto truck = factory.createVehicle("truck", "25");
        demonstrateVehicle(std::move(truck));

        // Register a new vehicle type at runtime
        factory.registerCreator("electric_car",
            [](std::string_view param) -> std::unique_ptr<Vehicle> {
                class ElectricCar : public Vehicle {
                private:
                    std::string model_;

                public:
                    explicit ElectricCar(std::string_view model) : model_(model) {
                        if (model.empty()) {
                            throw FactoryException("Electric car model cannot be empty");
                        }
                    }

                    void start() override {
                        std::cout << "Electric car " << model_ << " started silently\n";
                    }

                    void stop() override {
                        std::cout << "Electric car " << model_ << " stopped\n";
                    }

                    std::string getType() const override {
                        return "Electric Car (" + model_ + ")";
                    }
                };

                std::string model = param.empty() ? "Tesla Model 3" : std::string(param);
                return std::make_unique<ElectricCar>(model);
            });

        auto electric = factory.createVehicle("electric_car", "Tesla Model S");
        demonstrateVehicle(std::move(electric));

        std::cout << "\n=== Using Simple Factory ===\n";

        // Using simple factory with type safety
        auto simpleCar = SimpleVehicleFactory::createVehicle(
            SimpleVehicleFactory::VehicleType::CAR, "BMW X5");
        demonstrateVehicle(std::move(simpleCar));

        auto simpleBike = SimpleVehicleFactory::createVehicle(
            SimpleVehicleFactory::VehicleType::MOTORCYCLE, "Yamaha R1");
        demonstrateVehicle(std::move(simpleBike));

        // Demonstrate string to enum conversion
        auto vehicleType = SimpleVehicleFactory::stringToVehicleType("truck");
        if (vehicleType) {
            auto simpleTruck = SimpleVehicleFactory::createVehicle(*vehicleType, "50");
            demonstrateVehicle(std::move(simpleTruck));
        }

        std::cout << "\n=== Error Handling Demo ===\n";

        // Test error handling
        try {
            auto invalid = factory.createVehicle("airplane", "Boeing 737");
        } catch (const FactoryException& e) {
            std::cout << "Caught expected exception: " << e.what() << "\n";
        }

        try {
            auto invalidCar = Car("");  // Empty model should throw
        } catch (const FactoryException& e) {
            std::cout << "Caught expected exception: " << e.what() << "\n";
        }

    } catch (const FactoryException& e) {
        std::cerr << "Factory Error: " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected Error: " << e.what() << '\n';
        return 1;
    }

    std::cout << "\nFactory pattern demonstration completed successfully!\n";
    return 0;
}