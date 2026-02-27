/// Factory Pattern Implementation in Rust
///
/// The Factory pattern creates objects without specifying their concrete classes.
/// In Rust, we use traits and enums to implement factory patterns idiomatically.

use std::collections::HashMap;

/// Product trait that all vehicles must implement
pub trait Vehicle {
    fn start(&self) -> String;
    fn stop(&self) -> String;
    fn get_info(&self) -> String;
}

/// Concrete product implementations
#[derive(Debug, Clone)]
pub struct Car {
    brand: String,
    model: String,
    year: u32,
}

impl Car {
    pub fn new(brand: impl Into<String>, model: impl Into<String>, year: u32) -> Self {
        Self {
            brand: brand.into(),
            model: model.into(),
            year,
        }
    }

    pub fn brand(&self) -> &str {
        &self.brand
    }

    pub fn model(&self) -> &str {
        &self.model
    }

    pub fn year(&self) -> u32 {
        self.year
    }
}

impl Vehicle for Car {
    fn start(&self) -> String {
        format!("Starting {} {} {} engine", self.year, self.brand, self.model)
    }

    fn stop(&self) -> String {
        format!("Stopping {} {} {}", self.year, self.brand, self.model)
    }

    fn get_info(&self) -> String {
        format!("Car: {} {} {}", self.year, self.brand, self.model)
    }
}

#[derive(Debug, Clone)]
pub struct Motorcycle {
    brand: String,
    engine_size: u32,
}

impl Motorcycle {
    pub fn new(brand: impl Into<String>, engine_size: u32) -> Self {
        Self {
            brand: brand.into(),
            engine_size,
        }
    }

    pub fn brand(&self) -> &str {
        &self.brand
    }

    pub fn engine_size(&self) -> u32 {
        self.engine_size
    }
}

impl Vehicle for Motorcycle {
    fn start(&self) -> String {
        format!("Starting {} motorcycle ({}cc)", self.brand, self.engine_size)
    }

    fn stop(&self) -> String {
        format!("Stopping {} motorcycle", self.brand)
    }

    fn get_info(&self) -> String {
        format!("Motorcycle: {} ({}cc)", self.brand, self.engine_size)
    }
}

pub struct Truck {
    pub brand: String,
    pub cargo_capacity: u32,
}

impl Vehicle for Truck {
    fn start(&self) -> String {
        format!("Starting {} truck ({}kg capacity)", self.brand, self.cargo_capacity)
    }

    fn stop(&self) -> String {
        format!("Stopping {} truck", self.brand)
    }

    fn get_info(&self) -> String {
        format!("Truck: {} ({}kg capacity)", self.brand, self.cargo_capacity)
    }
}

/// Vehicle types enum for factory
#[derive(Debug, Clone, Hash, Eq, PartialEq)]
pub enum VehicleType {
    Car,
    Motorcycle,
    Truck,
}

/// Simple Factory implementation
pub struct VehicleFactory;

impl VehicleFactory {
    pub fn create_vehicle(
        vehicle_type: VehicleType,
        config: HashMap<String, String>,
    ) -> Result<Box<dyn Vehicle>, String> {
        match vehicle_type {
            VehicleType::Car => {
                let brand = config.get("brand").ok_or("Brand required for car")?;
                let model = config.get("model").ok_or("Model required for car")?;
                let year = config
                    .get("year")
                    .ok_or("Year required for car")?
                    .parse::<u32>()
                    .map_err(|_| "Invalid year format")?;

                Ok(Box::new(Car {
                    brand: brand.clone(),
                    model: model.clone(),
                    year,
                }))
            }
            VehicleType::Motorcycle => {
                let brand = config.get("brand").ok_or("Brand required for motorcycle")?;
                let engine_size = config
                    .get("engine_size")
                    .ok_or("Engine size required for motorcycle")?
                    .parse::<u32>()
                    .map_err(|_| "Invalid engine size format")?;

                Ok(Box::new(Motorcycle {
                    brand: brand.clone(),
                    engine_size,
                }))
            }
            VehicleType::Truck => {
                let brand = config.get("brand").ok_or("Brand required for truck")?;
                let cargo_capacity = config
                    .get("cargo_capacity")
                    .ok_or("Cargo capacity required for truck")?
                    .parse::<u32>()
                    .map_err(|_| "Invalid cargo capacity format")?;

                Ok(Box::new(Truck {
                    brand: brand.clone(),
                    cargo_capacity,
                }))
            }
        }
    }
}

/// Factory Method Pattern - Abstract Creator
pub trait VehicleCreator {
    fn create_vehicle(&self) -> Box<dyn Vehicle>;

    fn deliver_vehicle(&self) -> String {
        let vehicle = self.create_vehicle();
        format!("Delivering: {}", vehicle.get_info())
    }
}

/// Concrete Creators implementing factory methods
pub struct CarCreator {
    pub brand: String,
    pub model: String,
    pub year: u32,
}

impl VehicleCreator for CarCreator {
    fn create_vehicle(&self) -> Box<dyn Vehicle> {
        Box::new(Car {
            brand: self.brand.clone(),
            model: self.model.clone(),
            year: self.year,
        })
    }
}

pub struct MotorcycleCreator {
    pub brand: String,
    pub engine_size: u32,
}

impl VehicleCreator for MotorcycleCreator {
    fn create_vehicle(&self) -> Box<dyn Vehicle> {
        Box::new(Motorcycle {
            brand: self.brand.clone(),
            engine_size: self.engine_size,
        })
    }
}

/// Registry-based Factory for dynamic registration
pub struct VehicleRegistry {
    creators: HashMap<String, Box<dyn Fn() -> Box<dyn Vehicle> + Send + Sync>>,
}

impl VehicleRegistry {
    pub fn new() -> Self {
        VehicleRegistry {
            creators: HashMap::new(),
        }
    }

    pub fn register<F>(&mut self, name: &str, creator: F)
    where
        F: Fn() -> Box<dyn Vehicle> + Send + Sync + 'static,
    {
        self.creators.insert(name.to_string(), Box::new(creator));
    }

    pub fn create(&self, name: &str) -> Result<Box<dyn Vehicle>, String> {
        match self.creators.get(name) {
            Some(creator) => Ok(creator()),
            None => Err(format!("Unknown vehicle type: {}", name)),
        }
    }

    pub fn list_types(&self) -> Vec<String> {
        self.creators.keys().cloned().collect()
    }
}

impl Default for VehicleRegistry {
    fn default() -> Self {
        let mut registry = VehicleRegistry::new();

        // Register default vehicle types
        registry.register("default_car", || {
            Box::new(Car {
                brand: "Toyota".to_string(),
                model: "Camry".to_string(),
                year: 2023,
            })
        });

        registry.register("default_motorcycle", || {
            Box::new(Motorcycle {
                brand: "Honda".to_string(),
                engine_size: 600,
            })
        });

        registry.register("default_truck", || {
            Box::new(Truck {
                brand: "Ford".to_string(),
                cargo_capacity: 5000,
            })
        });

        registry
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_simple_factory() {
        let mut config = HashMap::new();
        config.insert("brand".to_string(), "Toyota".to_string());
        config.insert("model".to_string(), "Camry".to_string());
        config.insert("year".to_string(), "2023".to_string());

        let car = VehicleFactory::create_vehicle(VehicleType::Car, config).unwrap();
        assert!(car.get_info().contains("Toyota"));
        assert!(car.get_info().contains("Camry"));
    }

    #[test]
    fn test_factory_method() {
        let car_creator = CarCreator {
            brand: "BMW".to_string(),
            model: "X5".to_string(),
            year: 2023,
        };

        let vehicle = car_creator.create_vehicle();
        assert!(vehicle.get_info().contains("BMW"));

        let delivery_msg = car_creator.deliver_vehicle();
        assert!(delivery_msg.contains("Delivering"));
    }

    #[test]
    fn test_registry_factory() {
        let mut registry = VehicleRegistry::new();

        registry.register("sports_car", || {
            Box::new(Car {
                brand: "Ferrari".to_string(),
                model: "458".to_string(),
                year: 2023,
            })
        });

        let vehicle = registry.create("sports_car").unwrap();
        assert!(vehicle.get_info().contains("Ferrari"));

        let types = registry.list_types();
        assert!(types.contains(&"sports_car".to_string()));
    }

    #[test]
    fn test_factory_error_handling() {
        let config = HashMap::new(); // Empty config
        let result = VehicleFactory::create_vehicle(VehicleType::Car, config);
        assert!(result.is_err());
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Factory Pattern Demo ===");

    // Simple Factory example
    println!("\n1. Simple Factory:");
    let mut car_config = HashMap::new();
    car_config.insert("brand".to_string(), "Tesla".to_string());
    car_config.insert("model".to_string(), "Model 3".to_string());
    car_config.insert("year".to_string(), "2023".to_string());

    match VehicleFactory::create_vehicle(VehicleType::Car, car_config) {
        Ok(car) => {
            println!("{}", car.start());
            println!("{}", car.get_info());
        }
        Err(e) => println!("Error: {}", e),
    }

    // Factory Method example
    println!("\n2. Factory Method:");
    let motorcycle_creator = MotorcycleCreator {
        brand: "Harley-Davidson".to_string(),
        engine_size: 1200,
    };

    let motorcycle = motorcycle_creator.create_vehicle();
    println!("{}", motorcycle.start());
    println!("{}", motorcycle_creator.deliver_vehicle());

    // Registry Factory example
    println!("\n3. Registry Factory:");
    let mut registry = VehicleRegistry::default();

    // Register custom vehicle
    registry.register("luxury_car", || {
        Box::new(Car {
            brand: "Mercedes".to_string(),
            model: "S-Class".to_string(),
            year: 2024,
        })
    });

    println!("Available types: {:?}", registry.list_types());

    if let Ok(luxury_car) = registry.create("luxury_car") {
        println!("{}", luxury_car.get_info());
        println!("{}", luxury_car.start());
    }
}

fn main() {
    println!("=== Factory Pattern Demo ===");

    // Demo Vehicle Factory
    println!("\n1. Vehicle Factory:");
    let mut car_config = HashMap::new();
    car_config.insert("brand".to_string(), "Toyota".to_string());
    car_config.insert("model".to_string(), "Camry".to_string());
    car_config.insert("year".to_string(), "2024".to_string());

    if let Ok(vehicle) = VehicleFactory::create_vehicle(VehicleType::Car, car_config) {
        println!("Created: {}", vehicle.get_info());
        println!("Action: {}", vehicle.start());
    }

    let mut truck_config = HashMap::new();
    truck_config.insert("brand".to_string(), "Ford".to_string());
    truck_config.insert("cargo_capacity".to_string(), "5000".to_string());

    if let Ok(vehicle) = VehicleFactory::create_vehicle(VehicleType::Truck, truck_config) {
        println!("Created: {}", vehicle.get_info());
        println!("Action: {}", vehicle.start());
    }

    // Demo Creator Pattern
    println!("\n2. Creator Pattern:");
    let car_creator = CarCreator {
        brand: "Honda".to_string(),
        model: "Civic".to_string(),
        year: 2023,
    };
    let car = car_creator.create_vehicle();
    println!("Created: {}", car.get_info());
    println!("Action: {}", car.start());

    // Demo Motorcycle Creator
    println!("\n3. Motorcycle Creator:");
    let motorcycle_creator = MotorcycleCreator {
        brand: "Harley Davidson".to_string(),
        engine_size: 1200,
    };
    let motorcycle = motorcycle_creator.create_vehicle();
    println!("Created: {}", motorcycle.get_info());
    println!("Action: {}", motorcycle.start());

    // Demo Registry Factory
    println!("\n4. Registry Factory:");
    let mut registry = VehicleRegistry::new();

    // Register vehicle types
    registry.register("sedan", Box::new(|| {
        Box::new(Car {
            brand: "Toyota".to_string(),
            model: "Camry".to_string(),
            year: 2024,
        }) as Box<dyn Vehicle>
    }));

    println!("Available types: {:?}", registry.list_types());

    if let Ok(sedan) = registry.create("sedan") {
        println!("{}", sedan.get_info());
        println!("{}", sedan.start());
    }
}