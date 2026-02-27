#include "factory.hpp"
#include <stdexcept>
#include <algorithm>

namespace DesignPatterns {

// Car implementation
Car::Car(std::string_view model) : model_(model) {
    if (model.empty()) {
        throw FactoryException("Car model cannot be empty");
    }
}

void Car::start() {
    std::cout << "Car " << model_ << " engine started with key ignition\n";
}

void Car::stop() {
    std::cout << "Car " << model_ << " engine stopped\n";
}

std::string Car::getType() const {
    return "Car (" + model_ + ")";
}

// Motorcycle implementation
Motorcycle::Motorcycle(std::string_view brand) : brand_(brand) {
    if (brand.empty()) {
        throw FactoryException("Motorcycle brand cannot be empty");
    }
}

void Motorcycle::start() {
    std::cout << "Motorcycle " << brand_ << " engine started with kick start\n";
}

void Motorcycle::stop() {
    std::cout << "Motorcycle " << brand_ << " engine stopped\n";
}

std::string Motorcycle::getType() const {
    return "Motorcycle (" + brand_ + ")";
}

// Truck implementation
Truck::Truck(int capacity) : capacity_(capacity) {
    if (capacity <= 0) {
        throw FactoryException("Truck capacity must be positive");
    }
}

void Truck::start() {
    std::cout << "Truck with " << capacity_ << "T capacity engine started\n";
}

void Truck::stop() {
    std::cout << "Truck engine stopped\n";
}

std::string Truck::getType() const {
    return "Truck (" + std::to_string(capacity_) + "T)";
}

// VehicleFactory implementation
VehicleFactory::VehicleFactory() {
    // Register creators using lambdas
    creators_["car"] = [](std::string_view param) -> std::unique_ptr<Vehicle> {
        std::string model = param.empty() ? "Generic Car" : std::string(param);
        return std::make_unique<Car>(model);
    };

    creators_["motorcycle"] = [](std::string_view param) -> std::unique_ptr<Vehicle> {
        std::string brand = param.empty() ? "Generic Bike" : std::string(param);
        return std::make_unique<Motorcycle>(brand);
    };

    creators_["truck"] = [](std::string_view param) -> std::unique_ptr<Vehicle> {
        int capacity = 10; // default
        if (!param.empty()) {
            try {
                capacity = std::stoi(std::string(param));
            } catch (const std::exception&) {
                throw FactoryException("Invalid capacity parameter for truck: " + std::string(param));
            }
        }
        return std::make_unique<Truck>(capacity);
    };
}

std::unique_ptr<Vehicle> VehicleFactory::createVehicle(std::string_view type,
                                                      std::string_view param) {
    std::string typeStr(type);
    std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

    auto it = creators_.find(typeStr);
    if (it != creators_.end()) {
        try {
            return it->second(param);
        } catch (const FactoryException&) {
            throw; // Re-throw factory exceptions
        } catch (const std::exception& e) {
            throw FactoryException("Failed to create vehicle of type '" +
                                 std::string(type) + "': " + e.what());
        }
    }
    throw FactoryException("Unknown vehicle type: " + std::string(type));
}

void VehicleFactory::registerCreator(std::string_view type, Creator creator) {
    if (type.empty()) {
        throw FactoryException("Vehicle type cannot be empty");
    }
    if (!creator) {
        throw FactoryException("Creator function cannot be null");
    }

    std::string typeStr(type);
    std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);
    creators_[typeStr] = std::move(creator);
}

std::vector<std::string> VehicleFactory::getAvailableTypes() const {
    std::vector<std::string> types;
    types.reserve(creators_.size());

    for (const auto& [type, _] : creators_) {
        types.push_back(type);
    }

    std::sort(types.begin(), types.end());
    return types;
}

bool VehicleFactory::hasType(std::string_view type) const {
    std::string typeStr(type);
    std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);
    return creators_.find(typeStr) != creators_.end();
}

// SimpleVehicleFactory implementation
std::unique_ptr<Vehicle> SimpleVehicleFactory::createVehicle(VehicleType type,
                                                           std::string_view param) {
    try {
        switch (type) {
            case VehicleType::CAR: {
                std::string model = param.empty() ? "Default Car" : std::string(param);
                return std::make_unique<Car>(model);
            }
            case VehicleType::MOTORCYCLE: {
                std::string brand = param.empty() ? "Default Bike" : std::string(param);
                return std::make_unique<Motorcycle>(brand);
            }
            case VehicleType::TRUCK: {
                int capacity = 15; // default
                if (!param.empty()) {
                    capacity = std::stoi(std::string(param));
                }
                return std::make_unique<Truck>(capacity);
            }
            default:
                throw FactoryException("Unknown vehicle type enum");
        }
    } catch (const FactoryException&) {
        throw; // Re-throw factory exceptions
    } catch (const std::exception& e) {
        throw FactoryException("Failed to create vehicle: " + std::string(e.what()));
    }
}

std::optional<SimpleVehicleFactory::VehicleType>
SimpleVehicleFactory::stringToVehicleType(std::string_view type) {
    std::string typeStr(type);
    std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

    if (typeStr == "car") return VehicleType::CAR;
    if (typeStr == "motorcycle") return VehicleType::MOTORCYCLE;
    if (typeStr == "truck") return VehicleType::TRUCK;

    return std::nullopt;
}

// Utility functions
void demonstrateVehicle(std::unique_ptr<Vehicle> vehicle) {
    if (!vehicle) {
        std::cout << "Error: null vehicle provided\n";
        return;
    }

    std::cout << "Created: " << vehicle->getType() << '\n';
    vehicle->start();
    vehicle->stop();
    std::cout << "---\n";
}

} // namespace DesignPatterns