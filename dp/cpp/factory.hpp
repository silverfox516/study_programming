#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <vector>
#include <optional>

namespace DesignPatterns {

// Custom exception for factory errors
class FactoryException : public std::exception {
private:
    std::string message_;

public:
    explicit FactoryException(std::string_view message) : message_(message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }
};

// Abstract Product
class Vehicle {
public:
    virtual ~Vehicle() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual std::string getType() const = 0;

    // Non-copyable but movable
    Vehicle(const Vehicle&) = delete;
    Vehicle& operator=(const Vehicle&) = delete;
    Vehicle(Vehicle&&) = default;
    Vehicle& operator=(Vehicle&&) = default;

protected:
    Vehicle() = default;
};

// Concrete Products
class Car : public Vehicle {
private:
    std::string model_;

public:
    explicit Car(std::string_view model);

    void start() override;
    void stop() override;
    std::string getType() const override;
};

class Motorcycle : public Vehicle {
private:
    std::string brand_;

public:
    explicit Motorcycle(std::string_view brand);

    void start() override;
    void stop() override;
    std::string getType() const override;
};

class Truck : public Vehicle {
private:
    int capacity_;

public:
    explicit Truck(int capacity);

    void start() override;
    void stop() override;
    std::string getType() const override;
};

// Factory using function pointers and lambdas (Modern C++)
class VehicleFactory {
public:
    using Creator = std::function<std::unique_ptr<Vehicle>(std::string_view)>;

private:
    std::unordered_map<std::string, Creator> creators_;

public:
    VehicleFactory();

    // Non-copyable but movable
    VehicleFactory(const VehicleFactory&) = delete;
    VehicleFactory& operator=(const VehicleFactory&) = delete;
    VehicleFactory(VehicleFactory&&) = default;
    VehicleFactory& operator=(VehicleFactory&&) = default;

    std::unique_ptr<Vehicle> createVehicle(std::string_view type,
                                         std::string_view param = "");

    void registerCreator(std::string_view type, Creator creator);

    std::vector<std::string> getAvailableTypes() const;

    bool hasType(std::string_view type) const;
};

// Simple Factory (alternative approach)
class SimpleVehicleFactory {
public:
    enum class VehicleType { CAR, MOTORCYCLE, TRUCK };

    static std::unique_ptr<Vehicle> createVehicle(VehicleType type,
                                                std::string_view param = "");

    static std::optional<VehicleType> stringToVehicleType(std::string_view type);
};

// Utility functions
void demonstrateVehicle(std::unique_ptr<Vehicle> vehicle);

} // namespace DesignPatterns