/// Builder Pattern Implementation in Rust
///
/// The Builder pattern constructs complex objects step by step. In Rust, we can
/// implement this pattern using method chaining, Option types, and compile-time
/// guarantees through the type system.

use std::collections::HashMap;

/// Product to be built - Computer configuration
#[derive(Debug, Clone)]
pub struct Computer {
    pub cpu: String,
    pub ram_gb: u32,
    pub storage_gb: u32,
    pub gpu: Option<String>,
    pub motherboard: String,
    pub power_supply_watts: u32,
    pub case_type: String,
    pub operating_system: Option<String>,
    pub peripherals: Vec<String>,
    pub warranty_years: u32,
}

impl Computer {
    pub fn get_specs(&self) -> String {
        let mut specs = format!(
            "Computer Specifications:\n\
             CPU: {}\n\
             RAM: {}GB\n\
             Storage: {}GB\n\
             Motherboard: {}\n\
             PSU: {}W\n\
             Case: {}\n\
             Warranty: {} years",
            self.cpu,
            self.ram_gb,
            self.storage_gb,
            self.motherboard,
            self.power_supply_watts,
            self.case_type,
            self.warranty_years
        );

        if let Some(ref gpu) = self.gpu {
            specs.push_str(&format!("\nGPU: {}", gpu));
        }

        if let Some(ref os) = self.operating_system {
            specs.push_str(&format!("\nOS: {}", os));
        }

        if !self.peripherals.is_empty() {
            specs.push_str(&format!("\nPeripherals: {}", self.peripherals.join(", ")));
        }

        specs
    }

    pub fn estimated_price(&self) -> u32 {
        let mut price = 500; // Base price

        // CPU pricing
        if self.cpu.contains("i9") || self.cpu.contains("Ryzen 9") {
            price += 400;
        } else if self.cpu.contains("i7") || self.cpu.contains("Ryzen 7") {
            price += 300;
        } else if self.cpu.contains("i5") || self.cpu.contains("Ryzen 5") {
            price += 200;
        }

        // RAM pricing (roughly $4 per GB)
        price += self.ram_gb * 4;

        // Storage pricing (roughly $0.1 per GB for HDD, assume SSD if high-end)
        price += if self.storage_gb >= 1000 { self.storage_gb / 5 } else { self.storage_gb / 10 };

        // GPU pricing
        if let Some(ref gpu) = self.gpu {
            if gpu.contains("RTX 40") || gpu.contains("RX 7") {
                price += 800;
            } else if gpu.contains("RTX 30") || gpu.contains("RX 6") {
                price += 500;
            } else {
                price += 200;
            }
        }

        // Peripherals
        price += self.peripherals.len() as u32 * 50;

        price
    }
}

/// Classic Builder Pattern
pub struct ComputerBuilder {
    cpu: Option<String>,
    ram_gb: Option<u32>,
    storage_gb: Option<u32>,
    gpu: Option<String>,
    motherboard: Option<String>,
    power_supply_watts: Option<u32>,
    case_type: Option<String>,
    operating_system: Option<String>,
    peripherals: Vec<String>,
    warranty_years: Option<u32>,
}

impl ComputerBuilder {
    pub fn new() -> Self {
        Self {
            cpu: None,
            ram_gb: None,
            storage_gb: None,
            gpu: None,
            motherboard: None,
            power_supply_watts: None,
            case_type: None,
            operating_system: None,
            peripherals: Vec::new(),
            warranty_years: None,
        }
    }

    pub fn cpu(mut self, cpu: &str) -> Self {
        self.cpu = Some(cpu.to_string());
        self
    }

    pub fn ram(mut self, gb: u32) -> Self {
        self.ram_gb = Some(gb);
        self
    }

    pub fn storage(mut self, gb: u32) -> Self {
        self.storage_gb = Some(gb);
        self
    }

    pub fn gpu(mut self, gpu: &str) -> Self {
        self.gpu = Some(gpu.to_string());
        self
    }

    pub fn motherboard(mut self, motherboard: &str) -> Self {
        self.motherboard = Some(motherboard.to_string());
        self
    }

    pub fn power_supply(mut self, watts: u32) -> Self {
        self.power_supply_watts = Some(watts);
        self
    }

    pub fn case_type(mut self, case_type: &str) -> Self {
        self.case_type = Some(case_type.to_string());
        self
    }

    pub fn operating_system(mut self, os: &str) -> Self {
        self.operating_system = Some(os.to_string());
        self
    }

    pub fn add_peripheral(mut self, peripheral: &str) -> Self {
        self.peripherals.push(peripheral.to_string());
        self
    }

    pub fn warranty_years(mut self, years: u32) -> Self {
        self.warranty_years = Some(years);
        self
    }

    pub fn build(self) -> Result<Computer, String> {
        let cpu = self.cpu.ok_or("CPU is required")?;
        let ram_gb = self.ram_gb.ok_or("RAM is required")?;
        let storage_gb = self.storage_gb.ok_or("Storage is required")?;
        let motherboard = self.motherboard.ok_or("Motherboard is required")?;
        let power_supply_watts = self.power_supply_watts.ok_or("Power supply is required")?;
        let case_type = self.case_type.ok_or("Case type is required")?;
        let warranty_years = self.warranty_years.unwrap_or(1);

        // Validate configuration
        if ram_gb < 4 {
            return Err("Minimum 4GB RAM required".to_string());
        }

        if storage_gb < 120 {
            return Err("Minimum 120GB storage required".to_string());
        }

        Ok(Computer {
            cpu,
            ram_gb,
            storage_gb,
            gpu: self.gpu,
            motherboard,
            power_supply_watts,
            case_type,
            operating_system: self.operating_system,
            peripherals: self.peripherals,
            warranty_years,
        })
    }
}

impl Default for ComputerBuilder {
    fn default() -> Self {
        Self::new()
    }
}

/// Type-safe Builder Pattern using phantom types
/// This ensures required fields are set at compile time
use std::marker::PhantomData;

pub struct Required;
pub struct Optional;

pub struct TypedComputerBuilder<CpuState, RamState, StorageState, MotherboardState> {
    cpu: Option<String>,
    ram_gb: Option<u32>,
    storage_gb: Option<u32>,
    gpu: Option<String>,
    motherboard: Option<String>,
    power_supply_watts: Option<u32>,
    case_type: Option<String>,
    operating_system: Option<String>,
    peripherals: Vec<String>,
    warranty_years: Option<u32>,
    _cpu_state: PhantomData<CpuState>,
    _ram_state: PhantomData<RamState>,
    _storage_state: PhantomData<StorageState>,
    _motherboard_state: PhantomData<MotherboardState>,
}

impl TypedComputerBuilder<Optional, Optional, Optional, Optional> {
    pub fn new() -> Self {
        Self {
            cpu: None,
            ram_gb: None,
            storage_gb: None,
            gpu: None,
            motherboard: None,
            power_supply_watts: None,
            case_type: None,
            operating_system: None,
            peripherals: Vec::new(),
            warranty_years: None,
            _cpu_state: PhantomData,
            _ram_state: PhantomData,
            _storage_state: PhantomData,
            _motherboard_state: PhantomData,
        }
    }
}

impl<RamState, StorageState, MotherboardState>
    TypedComputerBuilder<Optional, RamState, StorageState, MotherboardState>
{
    pub fn cpu(
        self,
        cpu: &str,
    ) -> TypedComputerBuilder<Required, RamState, StorageState, MotherboardState> {
        TypedComputerBuilder {
            cpu: Some(cpu.to_string()),
            ram_gb: self.ram_gb,
            storage_gb: self.storage_gb,
            gpu: self.gpu,
            motherboard: self.motherboard,
            power_supply_watts: self.power_supply_watts,
            case_type: self.case_type,
            operating_system: self.operating_system,
            peripherals: self.peripherals,
            warranty_years: self.warranty_years,
            _cpu_state: PhantomData,
            _ram_state: PhantomData,
            _storage_state: PhantomData,
            _motherboard_state: PhantomData,
        }
    }
}

impl<CpuState, StorageState, MotherboardState>
    TypedComputerBuilder<CpuState, Optional, StorageState, MotherboardState>
{
    pub fn ram(
        self,
        gb: u32,
    ) -> TypedComputerBuilder<CpuState, Required, StorageState, MotherboardState> {
        TypedComputerBuilder {
            cpu: self.cpu,
            ram_gb: Some(gb),
            storage_gb: self.storage_gb,
            gpu: self.gpu,
            motherboard: self.motherboard,
            power_supply_watts: self.power_supply_watts,
            case_type: self.case_type,
            operating_system: self.operating_system,
            peripherals: self.peripherals,
            warranty_years: self.warranty_years,
            _cpu_state: PhantomData,
            _ram_state: PhantomData,
            _storage_state: PhantomData,
            _motherboard_state: PhantomData,
        }
    }
}

// Similar implementations for storage and motherboard would go here...
// For brevity, I'll implement a simpler version that can build when all required fields are set

impl TypedComputerBuilder<Required, Required, Required, Required> {
    pub fn build(self) -> Computer {
        Computer {
            cpu: self.cpu.unwrap(),
            ram_gb: self.ram_gb.unwrap(),
            storage_gb: self.storage_gb.unwrap(),
            gpu: self.gpu,
            motherboard: self.motherboard.unwrap(),
            power_supply_watts: self.power_supply_watts.unwrap_or(650),
            case_type: self.case_type.unwrap_or_else(|| "Mid Tower".to_string()),
            operating_system: self.operating_system,
            peripherals: self.peripherals,
            warranty_years: self.warranty_years.unwrap_or(1),
        }
    }
}

/// Director that knows how to build specific computer configurations
pub struct ComputerDirector;

impl ComputerDirector {
    pub fn build_gaming_pc() -> Result<Computer, String> {
        ComputerBuilder::new()
            .cpu("Intel Core i7-13700K")
            .ram(32)
            .storage(1000)
            .gpu("NVIDIA RTX 4070")
            .motherboard("ASUS ROG STRIX Z790-E")
            .power_supply(750)
            .case_type("Mid Tower RGB")
            .operating_system("Windows 11")
            .add_peripheral("Gaming Keyboard")
            .add_peripheral("Gaming Mouse")
            .add_peripheral("Gaming Headset")
            .warranty_years(2)
            .build()
    }

    pub fn build_office_pc() -> Result<Computer, String> {
        ComputerBuilder::new()
            .cpu("Intel Core i5-13400")
            .ram(16)
            .storage(512)
            .motherboard("ASUS PRIME B660M-A")
            .power_supply(500)
            .case_type("Small Form Factor")
            .operating_system("Windows 11")
            .add_peripheral("Wireless Keyboard")
            .add_peripheral("Wireless Mouse")
            .warranty_years(3)
            .build()
    }

    pub fn build_budget_pc() -> Result<Computer, String> {
        ComputerBuilder::new()
            .cpu("AMD Ryzen 5 5600G")
            .ram(8)
            .storage(256)
            .motherboard("MSI A520M-A PRO")
            .power_supply(400)
            .case_type("Micro ATX")
            .operating_system("Ubuntu 22.04")
            .warranty_years(1)
            .build()
    }

    pub fn build_workstation() -> Result<Computer, String> {
        ComputerBuilder::new()
            .cpu("AMD Ryzen 9 7950X")
            .ram(64)
            .storage(2000)
            .gpu("NVIDIA RTX 4090")
            .motherboard("ASUS ProArt X670E-CREATOR")
            .power_supply(1000)
            .case_type("Full Tower")
            .operating_system("Windows 11 Pro")
            .add_peripheral("Professional Keyboard")
            .add_peripheral("Precision Mouse")
            .add_peripheral("4K Monitor")
            .add_peripheral("Drawing Tablet")
            .warranty_years(3)
            .build()
    }
}

/// Fluent Builder with method chaining validation
pub struct FluentComputerBuilder {
    config: HashMap<String, String>,
    errors: Vec<String>,
}

impl FluentComputerBuilder {
    pub fn new() -> Self {
        Self {
            config: HashMap::new(),
            errors: Vec::new(),
        }
    }

    pub fn cpu(mut self, cpu: &str) -> Self {
        if cpu.is_empty() {
            self.errors.push("CPU cannot be empty".to_string());
        } else {
            self.config.insert("cpu".to_string(), cpu.to_string());
        }
        self
    }

    pub fn ram(mut self, gb: u32) -> Self {
        if gb < 4 {
            self.errors.push("RAM must be at least 4GB".to_string());
        } else {
            self.config.insert("ram_gb".to_string(), gb.to_string());
        }
        self
    }

    pub fn validate_and_build(self) -> Result<Computer, Vec<String>> {
        if !self.errors.is_empty() {
            return Err(self.errors);
        }

        // Convert HashMap to Computer (simplified for demo)
        let cpu = self.config.get("cpu").cloned().unwrap_or_default();
        let ram_gb = self.config
            .get("ram_gb")
            .and_then(|s| s.parse().ok())
            .unwrap_or(8);

        Ok(Computer {
            cpu,
            ram_gb,
            storage_gb: 512,
            gpu: None,
            motherboard: "Default".to_string(),
            power_supply_watts: 650,
            case_type: "Mid Tower".to_string(),
            operating_system: None,
            peripherals: Vec::new(),
            warranty_years: 1,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_builder() {
        let computer = ComputerBuilder::new()
            .cpu("Intel i5")
            .ram(16)
            .storage(512)
            .motherboard("ASUS B550")
            .power_supply(650)
            .case_type("Mid Tower")
            .build()
            .unwrap();

        assert_eq!(computer.cpu, "Intel i5");
        assert_eq!(computer.ram_gb, 16);
        assert_eq!(computer.storage_gb, 512);
    }

    #[test]
    fn test_builder_validation() {
        let result = ComputerBuilder::new()
            .cpu("Intel i5")
            .ram(2) // Below minimum
            .storage(512)
            .motherboard("ASUS B550")
            .power_supply(650)
            .case_type("Mid Tower")
            .build();

        assert!(result.is_err());
        assert!(result.unwrap_err().contains("4GB"));
    }

    #[test]
    fn test_director_gaming_pc() {
        let gaming_pc = ComputerDirector::build_gaming_pc().unwrap();
        assert!(gaming_pc.cpu.contains("i7"));
        assert!(gaming_pc.gpu.is_some());
        assert!(!gaming_pc.peripherals.is_empty());
    }

    #[test]
    fn test_computer_pricing() {
        let computer = ComputerDirector::build_gaming_pc().unwrap();
        let price = computer.estimated_price();
        assert!(price > 1000); // Gaming PC should be expensive
    }

    #[test]
    fn test_fluent_builder() {
        let computer = FluentComputerBuilder::new()
            .cpu("AMD Ryzen 5")
            .ram(16)
            .validate_and_build()
            .unwrap();

        assert_eq!(computer.cpu, "AMD Ryzen 5");
        assert_eq!(computer.ram_gb, 16);
    }

    #[test]
    fn test_fluent_builder_validation() {
        let result = FluentComputerBuilder::new()
            .cpu("")
            .ram(2)
            .validate_and_build();

        assert!(result.is_err());
        let errors = result.unwrap_err();
        assert_eq!(errors.len(), 2);
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Builder Pattern Demo ===");

    // Basic builder usage
    println!("\n1. Basic Builder:");
    match ComputerBuilder::new()
        .cpu("Intel Core i7-13700K")
        .ram(32)
        .storage(1000)
        .gpu("NVIDIA RTX 4070")
        .motherboard("ASUS ROG")
        .power_supply(750)
        .case_type("Gaming Tower")
        .operating_system("Windows 11")
        .add_peripheral("Mechanical Keyboard")
        .add_peripheral("Gaming Mouse")
        .warranty_years(2)
        .build()
    {
        Ok(computer) => {
            println!("{}", computer.get_specs());
            println!("Estimated Price: ${}", computer.estimated_price());
        }
        Err(e) => println!("Build failed: {}", e),
    }

    // Director examples
    println!("\n2. Director Patterns:");

    println!("\nGaming PC:");
    if let Ok(gaming_pc) = ComputerDirector::build_gaming_pc() {
        println!("{}", gaming_pc.get_specs());
        println!("Price: ${}", gaming_pc.estimated_price());
    }

    println!("\nOffice PC:");
    if let Ok(office_pc) = ComputerDirector::build_office_pc() {
        println!("{}", office_pc.get_specs());
        println!("Price: ${}", office_pc.estimated_price());
    }

    println!("\nBudget PC:");
    if let Ok(budget_pc) = ComputerDirector::build_budget_pc() {
        println!("{}", budget_pc.get_specs());
        println!("Price: ${}", budget_pc.estimated_price());
    }

    // Error handling example
    println!("\n3. Error Handling:");
    match ComputerBuilder::new()
        .cpu("Intel i5")
        .ram(2) // Too low
        .storage(50) // Too low
        .build()
    {
        Ok(_) => println!("Build successful"),
        Err(e) => println!("Build failed: {}", e),
    }

    // Fluent builder example
    println!("\n4. Fluent Builder:");
    match FluentComputerBuilder::new()
        .cpu("AMD Ryzen 7")
        .ram(32)
        .validate_and_build()
    {
        Ok(computer) => println!("Fluent build successful: {}", computer.cpu),
        Err(errors) => println!("Fluent build failed: {:?}", errors),
    }
}

fn main() {
    println!("=== Builder Pattern Demo ===");

    // Demo Computer Builder
    println!("\n1. Computer Builder:");
    let mut builder = ComputerBuilder::new();
    match builder
        .cpu("Intel Core i7-12700K")
        .ram(32)
        .storage(1024)
        .gpu("NVIDIA RTX 4080")
        .build()
    {
        Ok(computer) => println!("Built computer: {}", computer.get_specs()),
        Err(e) => println!("Build failed: {}", e),
    }

    // Demo Typed Builder (simplified implementation)
    println!("\n2. Typed Builder:");
    // Note: TypedComputerBuilder has a simplified implementation
    // that requires all fields to be set during construction
    println!("TypedComputerBuilder uses compile-time type checking");
    println!("but has simplified implementation in this example");

    // Demo Director Pattern
    println!("\n3. Director Pattern:");
    match ComputerDirector::build_gaming_pc() {
        Ok(gaming_pc) => println!("Gaming PC: {}", gaming_pc.get_specs()),
        Err(e) => println!("Gaming PC build failed: {}", e),
    }

    match ComputerDirector::build_office_pc() {
        Ok(office_pc) => println!("Office PC: {}", office_pc.get_specs()),
        Err(e) => println!("Office PC build failed: {}", e),
    }

    // Demo Fluent API Builder
    println!("\n4. Fluent API Builder:");
    match FluentComputerBuilder::new()
        .cpu("AMD Ryzen 9")
        .ram(64)
        .validate_and_build()
    {
        Ok(computer) => println!("Fluent build successful: {}", computer.cpu),
        Err(errors) => println!("Fluent build failed: {:?}", errors),
    }
}