/// Decorator Pattern Implementation in Rust
///
/// The Decorator pattern allows behavior to be added to objects dynamically
/// without altering their structure. It provides a flexible alternative to
/// subclassing for extending functionality.

use std::collections::HashMap;

/// Base component trait for coffee beverages
pub trait Coffee {
    fn cost(&self) -> f64;
    fn description(&self) -> String;
    fn ingredients(&self) -> Vec<String>;
}

/// Concrete base coffee implementation
pub struct SimpleCoffee {
    size: CoffeeSize,
}

#[derive(Debug, Clone)]
pub enum CoffeeSize {
    Small,
    Medium,
    Large,
}

impl SimpleCoffee {
    pub fn new(size: CoffeeSize) -> Self {
        SimpleCoffee { size }
    }
}

impl Coffee for SimpleCoffee {
    fn cost(&self) -> f64 {
        match self.size {
            CoffeeSize::Small => 2.00,
            CoffeeSize::Medium => 2.50,
            CoffeeSize::Large => 3.00,
        }
    }

    fn description(&self) -> String {
        format!("{:?} Simple Coffee", self.size)
    }

    fn ingredients(&self) -> Vec<String> {
        vec!["Coffee beans".to_string(), "Water".to_string()]
    }
}

/// Milk decorator
pub struct MilkDecorator {
    coffee: Box<dyn Coffee>,
    milk_type: MilkType,
}

#[derive(Debug, Clone)]
pub enum MilkType {
    Whole,
    Skim,
    Almond,
    Oat,
    Soy,
}

impl MilkDecorator {
    pub fn new(coffee: Box<dyn Coffee>, milk_type: MilkType) -> Self {
        MilkDecorator { coffee, milk_type }
    }
}

impl Coffee for MilkDecorator {
    fn cost(&self) -> f64 {
        let milk_cost = match self.milk_type {
            MilkType::Whole | MilkType::Skim => 0.50,
            MilkType::Almond | MilkType::Oat | MilkType::Soy => 0.75,
        };
        self.coffee.cost() + milk_cost
    }

    fn description(&self) -> String {
        format!("{} with {:?} milk", self.coffee.description(), self.milk_type)
    }

    fn ingredients(&self) -> Vec<String> {
        let mut ingredients = self.coffee.ingredients();
        ingredients.push(format!("{:?} milk", self.milk_type));
        ingredients
    }
}

/// Sugar decorator
pub struct SugarDecorator {
    coffee: Box<dyn Coffee>,
    packets: u32,
}

impl SugarDecorator {
    pub fn new(coffee: Box<dyn Coffee>, packets: u32) -> Self {
        SugarDecorator { coffee, packets }
    }
}

impl Coffee for SugarDecorator {
    fn cost(&self) -> f64 {
        self.coffee.cost() + (self.packets as f64 * 0.10)
    }

    fn description(&self) -> String {
        format!("{} with {} sugar packet(s)", self.coffee.description(), self.packets)
    }

    fn ingredients(&self) -> Vec<String> {
        let mut ingredients = self.coffee.ingredients();
        if self.packets > 0 {
            ingredients.push(format!("{} sugar packet(s)", self.packets));
        }
        ingredients
    }
}

/// Whipped cream decorator
pub struct WhippedCreamDecorator {
    coffee: Box<dyn Coffee>,
    extra: bool,
}

impl WhippedCreamDecorator {
    pub fn new(coffee: Box<dyn Coffee>, extra: bool) -> Self {
        WhippedCreamDecorator { coffee, extra }
    }
}

impl Coffee for WhippedCreamDecorator {
    fn cost(&self) -> f64 {
        let cream_cost = if self.extra { 1.00 } else { 0.75 };
        self.coffee.cost() + cream_cost
    }

    fn description(&self) -> String {
        let cream_desc = if self.extra { "extra whipped cream" } else { "whipped cream" };
        format!("{} with {}", self.coffee.description(), cream_desc)
    }

    fn ingredients(&self) -> Vec<String> {
        let mut ingredients = self.coffee.ingredients();
        let cream_desc = if self.extra { "Extra whipped cream" } else { "Whipped cream" };
        ingredients.push(cream_desc.to_string());
        ingredients
    }
}

/// Syrup decorator
pub struct SyrupDecorator {
    coffee: Box<dyn Coffee>,
    flavor: SyrupFlavor,
    shots: u32,
}

#[derive(Debug, Clone)]
pub enum SyrupFlavor {
    Vanilla,
    Caramel,
    Hazelnut,
    Chocolate,
    Pumpkin,
}

impl SyrupDecorator {
    pub fn new(coffee: Box<dyn Coffee>, flavor: SyrupFlavor, shots: u32) -> Self {
        SyrupDecorator { coffee, flavor, shots }
    }
}

impl Coffee for SyrupDecorator {
    fn cost(&self) -> f64 {
        self.coffee.cost() + (self.shots as f64 * 0.60)
    }

    fn description(&self) -> String {
        format!("{} with {} shot(s) of {:?} syrup",
                self.coffee.description(), self.shots, self.flavor)
    }

    fn ingredients(&self) -> Vec<String> {
        let mut ingredients = self.coffee.ingredients();
        if self.shots > 0 {
            ingredients.push(format!("{} shot(s) of {:?} syrup", self.shots, self.flavor));
        }
        ingredients
    }
}

/// Coffee builder for easy decoration chaining
pub struct CoffeeBuilder {
    coffee: Box<dyn Coffee>,
}

impl CoffeeBuilder {
    pub fn new(size: CoffeeSize) -> Self {
        CoffeeBuilder {
            coffee: Box::new(SimpleCoffee::new(size)),
        }
    }

    pub fn with_milk(self, milk_type: MilkType) -> Self {
        CoffeeBuilder {
            coffee: Box::new(MilkDecorator::new(self.coffee, milk_type)),
        }
    }

    pub fn with_sugar(self, packets: u32) -> Self {
        CoffeeBuilder {
            coffee: Box::new(SugarDecorator::new(self.coffee, packets)),
        }
    }

    pub fn with_whipped_cream(self, extra: bool) -> Self {
        CoffeeBuilder {
            coffee: Box::new(WhippedCreamDecorator::new(self.coffee, extra)),
        }
    }

    pub fn with_syrup(self, flavor: SyrupFlavor, shots: u32) -> Self {
        CoffeeBuilder {
            coffee: Box::new(SyrupDecorator::new(self.coffee, flavor, shots)),
        }
    }

    pub fn build(self) -> Box<dyn Coffee> {
        self.coffee
    }
}

/// Text formatting example - another application of Decorator pattern
pub trait TextFormatter {
    fn format(&self, text: &str) -> String;
}

pub struct PlainText;

impl TextFormatter for PlainText {
    fn format(&self, text: &str) -> String {
        text.to_string()
    }
}

pub struct BoldDecorator<T: TextFormatter> {
    formatter: T,
}

impl<T: TextFormatter> BoldDecorator<T> {
    pub fn new(formatter: T) -> Self {
        BoldDecorator { formatter }
    }
}

impl<T: TextFormatter> TextFormatter for BoldDecorator<T> {
    fn format(&self, text: &str) -> String {
        format!("**{}**", self.formatter.format(text))
    }
}

pub struct ItalicDecorator<T: TextFormatter> {
    formatter: T,
}

impl<T: TextFormatter> ItalicDecorator<T> {
    pub fn new(formatter: T) -> Self {
        ItalicDecorator { formatter }
    }
}

impl<T: TextFormatter> TextFormatter for ItalicDecorator<T> {
    fn format(&self, text: &str) -> String {
        format!("*{}*", self.formatter.format(text))
    }
}

pub struct UnderlineDecorator<T: TextFormatter> {
    formatter: T,
}

impl<T: TextFormatter> UnderlineDecorator<T> {
    pub fn new(formatter: T) -> Self {
        UnderlineDecorator { formatter }
    }
}

impl<T: TextFormatter> TextFormatter for UnderlineDecorator<T> {
    fn format(&self, text: &str) -> String {
        format!("__{text}__", text = self.formatter.format(text))
    }
}

pub struct ColorDecorator<T: TextFormatter> {
    formatter: T,
    color: String,
}

impl<T: TextFormatter> ColorDecorator<T> {
    pub fn new(formatter: T, color: &str) -> Self {
        ColorDecorator {
            formatter,
            color: color.to_string(),
        }
    }
}

impl<T: TextFormatter> TextFormatter for ColorDecorator<T> {
    fn format(&self, text: &str) -> String {
        format!("<span style=\"color: {}\">{}</span>",
                self.color, self.formatter.format(text))
    }
}

/// Stream processing decorator example
pub trait DataStream {
    fn process(&mut self, data: &[u8]) -> Result<Vec<u8>, String>;
    fn flush(&mut self) -> Result<Vec<u8>, String>;
}

pub struct BasicStream {
    buffer: Vec<u8>,
}

impl BasicStream {
    pub fn new() -> Self {
        BasicStream {
            buffer: Vec::new(),
        }
    }
}

impl Default for BasicStream {
    fn default() -> Self {
        Self::new()
    }
}

impl DataStream for BasicStream {
    fn process(&mut self, data: &[u8]) -> Result<Vec<u8>, String> {
        self.buffer.extend_from_slice(data);
        Ok(data.to_vec())
    }

    fn flush(&mut self) -> Result<Vec<u8>, String> {
        let result = self.buffer.clone();
        self.buffer.clear();
        Ok(result)
    }
}

/// Compression decorator
pub struct CompressionDecorator<T: DataStream> {
    stream: T,
    compression_ratio: f32,
}

impl<T: DataStream> CompressionDecorator<T> {
    pub fn new(stream: T, compression_ratio: f32) -> Self {
        CompressionDecorator {
            stream,
            compression_ratio: compression_ratio.clamp(0.1, 0.9),
        }
    }
}

impl<T: DataStream> DataStream for CompressionDecorator<T> {
    fn process(&mut self, data: &[u8]) -> Result<Vec<u8>, String> {
        let processed = self.stream.process(data)?;

        // Simulate compression
        let compressed_size = (processed.len() as f32 * self.compression_ratio) as usize;
        let mut compressed = processed;
        compressed.truncate(compressed_size.max(1));
        compressed.insert(0, b'C'); // Mark as compressed

        Ok(compressed)
    }

    fn flush(&mut self) -> Result<Vec<u8>, String> {
        self.stream.flush()
    }
}

/// Encryption decorator
pub struct EncryptionDecorator<T: DataStream> {
    stream: T,
    key: u8,
}

impl<T: DataStream> EncryptionDecorator<T> {
    pub fn new(stream: T, key: u8) -> Self {
        EncryptionDecorator { stream, key }
    }
}

impl<T: DataStream> DataStream for EncryptionDecorator<T> {
    fn process(&mut self, data: &[u8]) -> Result<Vec<u8>, String> {
        let processed = self.stream.process(data)?;

        // Simple XOR encryption
        let encrypted: Vec<u8> = processed.iter().map(|&b| b ^ self.key).collect();

        Ok(encrypted)
    }

    fn flush(&mut self) -> Result<Vec<u8>, String> {
        let flushed = self.stream.flush()?;
        let encrypted: Vec<u8> = flushed.iter().map(|&b| b ^ self.key).collect();
        Ok(encrypted)
    }
}

/// Coffee shop order system
pub struct CoffeeShop {
    menu: HashMap<String, f64>,
    orders: Vec<CoffeeOrder>,
}

pub struct CoffeeOrder {
    id: u32,
    coffee: Box<dyn Coffee>,
    customer_name: String,
}

impl CoffeeShop {
    pub fn new() -> Self {
        let mut menu = HashMap::new();
        menu.insert("Small Coffee".to_string(), 2.00);
        menu.insert("Medium Coffee".to_string(), 2.50);
        menu.insert("Large Coffee".to_string(), 3.00);
        menu.insert("Milk".to_string(), 0.50);
        menu.insert("Premium Milk".to_string(), 0.75);
        menu.insert("Sugar".to_string(), 0.10);
        menu.insert("Whipped Cream".to_string(), 0.75);
        menu.insert("Syrup".to_string(), 0.60);

        CoffeeShop {
            menu,
            orders: Vec::new(),
        }
    }

    pub fn create_order(&mut self, customer_name: &str, coffee: Box<dyn Coffee>) -> u32 {
        let id = self.orders.len() as u32 + 1;
        let order = CoffeeOrder {
            id,
            coffee,
            customer_name: customer_name.to_string(),
        };
        self.orders.push(order);
        id
    }

    pub fn get_order(&self, id: u32) -> Option<&CoffeeOrder> {
        self.orders.iter().find(|order| order.id == id)
    }

    pub fn print_receipt(&self, id: u32) -> Option<String> {
        let order = self.get_order(id)?;

        let mut receipt = String::new();
        receipt.push_str("=== COFFEE SHOP RECEIPT ===\n");
        receipt.push_str(&format!("Order ID: {}\n", order.id));
        receipt.push_str(&format!("Customer: {}\n", order.customer_name));
        receipt.push_str(&format!("Item: {}\n", order.coffee.description()));
        receipt.push_str("Ingredients:\n");

        for ingredient in order.coffee.ingredients() {
            receipt.push_str(&format!("  - {}\n", ingredient));
        }

        receipt.push_str(&format!("Total: ${:.2}\n", order.coffee.cost()));
        receipt.push_str("===========================\n");

        Some(receipt)
    }

    pub fn get_menu(&self) -> &HashMap<String, f64> {
        &self.menu
    }

    pub fn total_orders(&self) -> usize {
        self.orders.len()
    }
}

impl Default for CoffeeShop {
    fn default() -> Self {
        Self::new()
    }
}

impl CoffeeOrder {
    pub fn id(&self) -> u32 {
        self.id
    }

    pub fn customer_name(&self) -> &str {
        &self.customer_name
    }

    pub fn coffee(&self) -> &dyn Coffee {
        &*self.coffee
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_simple_coffee() {
        let coffee = SimpleCoffee::new(CoffeeSize::Medium);
        assert_eq!(coffee.cost(), 2.50);
        assert!(coffee.description().contains("Medium"));
        assert_eq!(coffee.ingredients().len(), 2);
    }

    #[test]
    fn test_coffee_decorators() {
        let coffee = SimpleCoffee::new(CoffeeSize::Large);
        let coffee = MilkDecorator::new(Box::new(coffee), MilkType::Oat);
        let coffee = SugarDecorator::new(Box::new(coffee), 2);
        let coffee = WhippedCreamDecorator::new(Box::new(coffee), true);

        assert_eq!(coffee.cost(), 3.00 + 0.75 + 0.20 + 1.00); // 4.95
        assert!(coffee.description().contains("Oat milk"));
        assert!(coffee.description().contains("2 sugar"));
        assert!(coffee.description().contains("extra whipped cream"));
        assert_eq!(coffee.ingredients().len(), 5);
    }

    #[test]
    fn test_coffee_builder() {
        let coffee = CoffeeBuilder::new(CoffeeSize::Medium)
            .with_milk(MilkType::Almond)
            .with_syrup(SyrupFlavor::Vanilla, 1)
            .with_whipped_cream(false)
            .build();

        assert_eq!(coffee.cost(), 2.50 + 0.75 + 0.60 + 0.75); // 4.60
        assert!(coffee.description().contains("Almond milk"));
        assert!(coffee.description().contains("Vanilla syrup"));
    }

    #[test]
    fn test_text_formatter_decorators() {
        let formatter = ColorDecorator::new(
            UnderlineDecorator::new(
                BoldDecorator::new(
                    ItalicDecorator::new(PlainText)
                )
            ),
            "red"
        );

        let result = formatter.format("Hello");
        assert!(result.contains("*"));  // Italic
        assert!(result.contains("**")); // Bold
        assert!(result.contains("__")); // Underline
        assert!(result.contains("red")); // Color
    }

    #[test]
    fn test_stream_decorators() {
        let stream = BasicStream::new();
        let mut encrypted_stream = EncryptionDecorator::new(stream, 0xFF);

        let data = b"Hello, World!";
        let result = encrypted_stream.process(data).unwrap();

        // Decrypt to verify
        let decrypted: Vec<u8> = result.iter().map(|&b| b ^ 0xFF).collect();
        assert_eq!(decrypted, data);
    }

    #[test]
    fn test_coffee_shop() {
        let mut shop = CoffeeShop::new();

        let coffee = CoffeeBuilder::new(CoffeeSize::Large)
            .with_milk(MilkType::Whole)
            .with_sugar(1)
            .build();

        let order_id = shop.create_order("Alice", coffee);
        assert_eq!(order_id, 1);

        let receipt = shop.print_receipt(order_id).unwrap();
        assert!(receipt.contains("Alice"));
        assert!(receipt.contains("Large"));
        assert_eq!(shop.total_orders(), 1);
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Decorator Pattern Demo ===");

    // Coffee ordering example
    println!("\n1. Coffee Decorator:");

    // Build a complex coffee order
    let coffee = CoffeeBuilder::new(CoffeeSize::Large)
        .with_milk(MilkType::Oat)
        .with_syrup(SyrupFlavor::Caramel, 2)
        .with_sugar(1)
        .with_whipped_cream(true)
        .build();

    println!("Order: {}", coffee.description());
    println!("Cost: ${:.2}", coffee.cost());
    println!("Ingredients:");
    for ingredient in coffee.ingredients() {
        println!("  - {}", ingredient);
    }

    // Coffee shop system
    println!("\n2. Coffee Shop Order System:");
    let mut shop = CoffeeShop::new();

    let simple_coffee = CoffeeBuilder::new(CoffeeSize::Medium)
        .with_milk(MilkType::Almond)
        .build();

    let order_id = shop.create_order("Bob", simple_coffee);
    if let Some(receipt) = shop.print_receipt(order_id) {
        println!("{}", receipt);
    }

    // Text formatting example
    println!("3. Text Formatting Decorators:");

    let bold_italic = BoldDecorator::new(ItalicDecorator::new(PlainText));
    let formatted_text = bold_italic.format("Important Message");
    println!("Formatted: {}", formatted_text);

    let fancy_formatter = ColorDecorator::new(
        UnderlineDecorator::new(
            BoldDecorator::new(PlainText)
        ),
        "blue"
    );
    let fancy_text = fancy_formatter.format("Fancy Title");
    println!("Fancy: {}", fancy_text);

    // Stream processing example
    println!("\n4. Stream Processing Decorators:");

    let stream = BasicStream::new();
    let mut decorated_stream = CompressionDecorator::new(
        EncryptionDecorator::new(stream, 42),
        0.7
    );

    let data = b"This is some sample data to process";
    match decorated_stream.process(data) {
        Ok(processed) => {
            println!("Original size: {} bytes", data.len());
            println!("Processed size: {} bytes", processed.len());
            println!("Processed data: {:?}", processed);
        }
        Err(e) => println!("Error: {}", e),
    }
}

pub fn main() {
    demo();
}