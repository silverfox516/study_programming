/// Visitor Pattern Implementation in Rust
///
/// The Visitor pattern lets you define a new operation without changing the classes
/// of the elements on which it operates. It separates algorithms from the objects
/// on which they operate.

use std::collections::HashMap;

/// File system visitor example
pub trait FileSystemVisitor {
    fn visit_file(&mut self, file: &File) -> Result<(), String>;
    fn visit_directory(&mut self, directory: &Directory) -> Result<(), String>;
}

pub trait FileSystemElement {
    fn accept(&self, visitor: &mut dyn FileSystemVisitor) -> Result<(), String>;
    fn get_name(&self) -> &str;
    fn get_size(&self) -> u64;
}

pub struct File {
    name: String,
    size: u64,
    extension: String,
    content: String,
}

impl File {
    pub fn new(name: &str, size: u64, extension: &str, content: &str) -> Self {
        File {
            name: name.to_string(),
            size,
            extension: extension.to_string(),
            content: content.to_string(),
        }
    }

    pub fn get_extension(&self) -> &str {
        &self.extension
    }

    pub fn get_content(&self) -> &str {
        &self.content
    }
}

impl FileSystemElement for File {
    fn accept(&self, visitor: &mut dyn FileSystemVisitor) -> Result<(), String> {
        visitor.visit_file(self)
    }

    fn get_name(&self) -> &str {
        &self.name
    }

    fn get_size(&self) -> u64 {
        self.size
    }
}

pub struct Directory {
    name: String,
    children: Vec<Box<dyn FileSystemElement>>,
}

impl Directory {
    pub fn new(name: &str) -> Self {
        Directory {
            name: name.to_string(),
            children: Vec::new(),
        }
    }

    pub fn add_child(&mut self, child: Box<dyn FileSystemElement>) {
        self.children.push(child);
    }

    pub fn get_children(&self) -> &[Box<dyn FileSystemElement>] {
        &self.children
    }
}

impl FileSystemElement for Directory {
    fn accept(&self, visitor: &mut dyn FileSystemVisitor) -> Result<(), String> {
        visitor.visit_directory(self)?;
        for child in &self.children {
            child.accept(visitor)?;
        }
        Ok(())
    }

    fn get_name(&self) -> &str {
        &self.name
    }

    fn get_size(&self) -> u64 {
        self.children.iter().map(|child| child.get_size()).sum()
    }
}

/// Size calculator visitor
pub struct SizeCalculatorVisitor {
    total_size: u64,
    file_count: u32,
    directory_count: u32,
}

impl SizeCalculatorVisitor {
    pub fn new() -> Self {
        SizeCalculatorVisitor {
            total_size: 0,
            file_count: 0,
            directory_count: 0,
        }
    }

    pub fn get_results(&self) -> (u64, u32, u32) {
        (self.total_size, self.file_count, self.directory_count)
    }
}

impl Default for SizeCalculatorVisitor {
    fn default() -> Self {
        Self::new()
    }
}

impl FileSystemVisitor for SizeCalculatorVisitor {
    fn visit_file(&mut self, file: &File) -> Result<(), String> {
        self.total_size += file.get_size();
        self.file_count += 1;
        Ok(())
    }

    fn visit_directory(&mut self, _directory: &Directory) -> Result<(), String> {
        self.directory_count += 1;
        Ok(())
    }
}

/// File search visitor
pub struct FileSearchVisitor {
    search_pattern: String,
    found_files: Vec<String>,
    search_in_content: bool,
}

impl FileSearchVisitor {
    pub fn new(pattern: &str, search_in_content: bool) -> Self {
        FileSearchVisitor {
            search_pattern: pattern.to_string(),
            found_files: Vec::new(),
            search_in_content,
        }
    }

    pub fn get_found_files(&self) -> &[String] {
        &self.found_files
    }
}

impl FileSystemVisitor for FileSearchVisitor {
    fn visit_file(&mut self, file: &File) -> Result<(), String> {
        let matches = file.get_name().contains(&self.search_pattern) ||
                     file.get_extension().contains(&self.search_pattern) ||
                     (self.search_in_content && file.get_content().contains(&self.search_pattern));

        if matches {
            self.found_files.push(file.get_name().to_string());
        }
        Ok(())
    }

    fn visit_directory(&mut self, directory: &Directory) -> Result<(), String> {
        if directory.get_name().contains(&self.search_pattern) {
            self.found_files.push(format!("{}/", directory.get_name()));
        }
        Ok(())
    }
}

/// Expression visitor for mathematical expressions
pub trait ExpressionVisitor<T> {
    fn visit_number(&mut self, value: f64) -> T;
    fn visit_binary_op(&mut self, left: T, right: T, operator: BinaryOperator) -> T;
    fn visit_unary_op(&mut self, operand: T, operator: UnaryOperator) -> T;
}

#[derive(Debug, Clone)]
pub enum Expression {
    Number(f64),
    Binary {
        left: Box<Expression>,
        right: Box<Expression>,
        operator: BinaryOperator,
    },
    Unary {
        operand: Box<Expression>,
        operator: UnaryOperator,
    },
}

impl Expression {
    pub fn accept<T>(&self, visitor: &mut dyn ExpressionVisitor<T>) -> T {
        match self {
            Expression::Number(value) => visitor.visit_number(*value),
            Expression::Binary { left, right, operator } => {
                let left_result = left.accept(visitor);
                let right_result = right.accept(visitor);
                visitor.visit_binary_op(left_result, right_result, operator.clone())
            }
            Expression::Unary { operand, operator } => {
                let operand_result = operand.accept(visitor);
                visitor.visit_unary_op(operand_result, operator.clone())
            }
        }
    }
}

#[derive(Debug, Clone)]
pub enum BinaryOperator {
    Add,
    Subtract,
    Multiply,
    Divide,
    Power,
}

#[derive(Debug, Clone)]
pub enum UnaryOperator {
    Negate,
    Abs,
    Sqrt,
}


/// Evaluator visitor
pub struct EvaluatorVisitor;

impl ExpressionVisitor<f64> for EvaluatorVisitor {
    fn visit_number(&mut self, value: f64) -> f64 {
        value
    }

    fn visit_binary_op(&mut self, left: f64, right: f64, operator: BinaryOperator) -> f64 {
        match operator {
            BinaryOperator::Add => left + right,
            BinaryOperator::Subtract => left - right,
            BinaryOperator::Multiply => left * right,
            BinaryOperator::Divide => {
                if right != 0.0 {
                    left / right
                } else {
                    f64::NAN
                }
            }
            BinaryOperator::Power => left.powf(right),
        }
    }

    fn visit_unary_op(&mut self, operand: f64, operator: UnaryOperator) -> f64 {
        match operator {
            UnaryOperator::Negate => -operand,
            UnaryOperator::Abs => operand.abs(),
            UnaryOperator::Sqrt => operand.sqrt(),
        }
    }
}

/// String formatter visitor
pub struct FormatterVisitor;

impl ExpressionVisitor<String> for FormatterVisitor {
    fn visit_number(&mut self, value: f64) -> String {
        if value.fract() == 0.0 {
            format!("{}", value as i64)
        } else {
            format!("{:.2}", value)
        }
    }

    fn visit_binary_op(&mut self, left: String, right: String, operator: BinaryOperator) -> String {
        let op_str = match operator {
            BinaryOperator::Add => "+",
            BinaryOperator::Subtract => "-",
            BinaryOperator::Multiply => "*",
            BinaryOperator::Divide => "/",
            BinaryOperator::Power => "^",
        };
        format!("({} {} {})", left, op_str, right)
    }

    fn visit_unary_op(&mut self, operand: String, operator: UnaryOperator) -> String {
        match operator {
            UnaryOperator::Negate => format!("(-{})", operand),
            UnaryOperator::Abs => format!("abs({})", operand),
            UnaryOperator::Sqrt => format!("sqrt({})", operand),
        }
    }
}

/// Shopping cart visitor for e-commerce
pub trait ShoppingItemVisitor {
    fn visit_physical_product(&mut self, product: &PhysicalProduct) -> Result<(), String>;
    fn visit_digital_product(&mut self, product: &DigitalProduct) -> Result<(), String>;
    fn visit_service(&mut self, service: &Service) -> Result<(), String>;
}

pub trait ShoppingItem {
    fn accept(&self, visitor: &mut dyn ShoppingItemVisitor) -> Result<(), String>;
    fn get_name(&self) -> &str;
    fn get_price(&self) -> f64;
    fn get_quantity(&self) -> u32;
}

pub struct PhysicalProduct {
    name: String,
    price: f64,
    quantity: u32,
    weight: f64,
    dimensions: (f64, f64, f64),
}

impl PhysicalProduct {
    pub fn new(name: &str, price: f64, quantity: u32, weight: f64, dimensions: (f64, f64, f64)) -> Self {
        PhysicalProduct {
            name: name.to_string(),
            price,
            quantity,
            weight,
            dimensions,
        }
    }

    pub fn get_weight(&self) -> f64 {
        self.weight
    }

    pub fn get_dimensions(&self) -> (f64, f64, f64) {
        self.dimensions
    }
}

impl ShoppingItem for PhysicalProduct {
    fn accept(&self, visitor: &mut dyn ShoppingItemVisitor) -> Result<(), String> {
        visitor.visit_physical_product(self)
    }

    fn get_name(&self) -> &str {
        &self.name
    }

    fn get_price(&self) -> f64 {
        self.price
    }

    fn get_quantity(&self) -> u32 {
        self.quantity
    }
}

pub struct DigitalProduct {
    name: String,
    price: f64,
    quantity: u32,
    download_size_mb: f64,
    license_type: String,
}

impl DigitalProduct {
    pub fn new(name: &str, price: f64, quantity: u32, download_size_mb: f64, license_type: &str) -> Self {
        DigitalProduct {
            name: name.to_string(),
            price,
            quantity,
            download_size_mb,
            license_type: license_type.to_string(),
        }
    }

    pub fn get_download_size(&self) -> f64 {
        self.download_size_mb
    }

    pub fn get_license_type(&self) -> &str {
        &self.license_type
    }
}

impl ShoppingItem for DigitalProduct {
    fn accept(&self, visitor: &mut dyn ShoppingItemVisitor) -> Result<(), String> {
        visitor.visit_digital_product(self)
    }

    fn get_name(&self) -> &str {
        &self.name
    }

    fn get_price(&self) -> f64 {
        self.price
    }

    fn get_quantity(&self) -> u32 {
        self.quantity
    }
}

pub struct Service {
    name: String,
    price: f64,
    quantity: u32,
    duration_hours: f64,
    service_type: String,
}

impl Service {
    pub fn new(name: &str, price: f64, quantity: u32, duration_hours: f64, service_type: &str) -> Self {
        Service {
            name: name.to_string(),
            price,
            quantity,
            duration_hours,
            service_type: service_type.to_string(),
        }
    }

    pub fn get_duration(&self) -> f64 {
        self.duration_hours
    }

    pub fn get_service_type(&self) -> &str {
        &self.service_type
    }
}

impl ShoppingItem for Service {
    fn accept(&self, visitor: &mut dyn ShoppingItemVisitor) -> Result<(), String> {
        visitor.visit_service(self)
    }

    fn get_name(&self) -> &str {
        &self.name
    }

    fn get_price(&self) -> f64 {
        self.price
    }

    fn get_quantity(&self) -> u32 {
        self.quantity
    }
}

/// Tax calculator visitor
pub struct TaxCalculatorVisitor {
    physical_tax_rate: f64,
    digital_tax_rate: f64,
    service_tax_rate: f64,
    total_tax: f64,
    breakdown: HashMap<String, f64>,
}

impl TaxCalculatorVisitor {
    pub fn new(physical_tax_rate: f64, digital_tax_rate: f64, service_tax_rate: f64) -> Self {
        TaxCalculatorVisitor {
            physical_tax_rate,
            digital_tax_rate,
            service_tax_rate,
            total_tax: 0.0,
            breakdown: HashMap::new(),
        }
    }

    pub fn get_total_tax(&self) -> f64 {
        self.total_tax
    }

    pub fn get_breakdown(&self) -> &HashMap<String, f64> {
        &self.breakdown
    }
}

impl ShoppingItemVisitor for TaxCalculatorVisitor {
    fn visit_physical_product(&mut self, product: &PhysicalProduct) -> Result<(), String> {
        let subtotal = product.get_price() * product.get_quantity() as f64;
        let tax = subtotal * self.physical_tax_rate;
        self.total_tax += tax;
        self.breakdown.insert(
            format!("{} (Physical)", product.get_name()),
            tax
        );
        Ok(())
    }

    fn visit_digital_product(&mut self, product: &DigitalProduct) -> Result<(), String> {
        let subtotal = product.get_price() * product.get_quantity() as f64;
        let tax = subtotal * self.digital_tax_rate;
        self.total_tax += tax;
        self.breakdown.insert(
            format!("{} (Digital)", product.get_name()),
            tax
        );
        Ok(())
    }

    fn visit_service(&mut self, service: &Service) -> Result<(), String> {
        let subtotal = service.get_price() * service.get_quantity() as f64;
        let tax = subtotal * self.service_tax_rate;
        self.total_tax += tax;
        self.breakdown.insert(
            format!("{} (Service)", service.get_name()),
            tax
        );
        Ok(())
    }
}

/// Shipping calculator visitor
pub struct ShippingCalculatorVisitor {
    total_weight: f64,
    total_volume: f64,
    shipping_cost: f64,
    has_physical_items: bool,
}

impl ShippingCalculatorVisitor {
    pub fn new() -> Self {
        ShippingCalculatorVisitor {
            total_weight: 0.0,
            total_volume: 0.0,
            shipping_cost: 0.0,
            has_physical_items: false,
        }
    }

    pub fn get_shipping_cost(&self) -> f64 {
        if !self.has_physical_items {
            return 0.0;
        }

        // Calculate shipping based on weight and volume
        let weight_cost = self.total_weight * 0.5; // $0.50 per kg
        let volume_cost = self.total_volume * 0.1; // $0.10 per cubic unit

        (weight_cost + volume_cost).max(5.0) // Minimum $5 shipping
    }

    pub fn get_total_weight(&self) -> f64 {
        self.total_weight
    }

    pub fn get_total_volume(&self) -> f64 {
        self.total_volume
    }
}

impl Default for ShippingCalculatorVisitor {
    fn default() -> Self {
        Self::new()
    }
}

impl ShoppingItemVisitor for ShippingCalculatorVisitor {
    fn visit_physical_product(&mut self, product: &PhysicalProduct) -> Result<(), String> {
        self.has_physical_items = true;
        self.total_weight += product.get_weight() * product.get_quantity() as f64;

        let (l, w, h) = product.get_dimensions();
        let volume = l * w * h * product.get_quantity() as f64;
        self.total_volume += volume;

        Ok(())
    }

    fn visit_digital_product(&mut self, _product: &DigitalProduct) -> Result<(), String> {
        // Digital products don't affect shipping
        Ok(())
    }

    fn visit_service(&mut self, _service: &Service) -> Result<(), String> {
        // Services don't affect shipping
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_file_system_visitor() {
        let mut root = Directory::new("root");
        root.add_child(Box::new(File::new("file1.txt", 100, "txt", "content1")));
        root.add_child(Box::new(File::new("file2.jpg", 200, "jpg", "content2")));

        let mut size_visitor = SizeCalculatorVisitor::new();
        root.accept(&mut size_visitor).unwrap();

        let (total_size, file_count, dir_count) = size_visitor.get_results();
        assert_eq!(total_size, 300);
        assert_eq!(file_count, 2);
        assert_eq!(dir_count, 1);
    }

    #[test]
    fn test_expression_visitor() {
        // Create expression: (5 + 3) * 2
        let expr = Expression::Binary {
            left: Box::new(Expression::Binary {
                left: Box::new(Expression::Number(5.0)),
                right: Box::new(Expression::Number(3.0)),
                operator: BinaryOperator::Add,
            }),
            right: Box::new(Expression::Number(2.0)),
            operator: BinaryOperator::Multiply,
        };

        let mut evaluator = EvaluatorVisitor;
        let result = expr.accept(&mut evaluator);
        assert_eq!(result, 16.0);

        let mut formatter = FormatterVisitor;
        let formatted = expr.accept(&mut formatter);
        assert!(formatted.contains("+"));
        assert!(formatted.contains("*"));
    }

    #[test]
    fn test_shopping_cart_visitor() {
        let items: Vec<Box<dyn ShoppingItem>> = vec![
            Box::new(PhysicalProduct::new("Book", 29.99, 2, 0.5, (20.0, 15.0, 2.0))),
            Box::new(DigitalProduct::new("Software", 99.99, 1, 500.0, "Single User")),
            Box::new(Service::new("Consultation", 150.0, 1, 2.0, "Professional")),
        ];

        let mut tax_visitor = TaxCalculatorVisitor::new(0.08, 0.05, 0.10);
        for item in &items {
            item.accept(&mut tax_visitor).unwrap();
        }

        assert!(tax_visitor.get_total_tax() > 0.0);
        assert_eq!(tax_visitor.get_breakdown().len(), 3);
    }
}

pub fn demo() {
    println!("=== Visitor Pattern Demo ===");

    // File System Example
    println!("\n1. File System Visitor:");
    let mut root = Directory::new("project");

    let mut src_dir = Directory::new("src");
    src_dir.add_child(Box::new(File::new("main.rs", 1500, "rs", "fn main() { println!(\"Hello\"); }")));
    src_dir.add_child(Box::new(File::new("lib.rs", 2500, "rs", "pub mod utils;")));
    src_dir.add_child(Box::new(File::new("utils.rs", 800, "rs", "pub fn helper() {}")));

    let mut docs_dir = Directory::new("docs");
    docs_dir.add_child(Box::new(File::new("README.md", 1200, "md", "# Project Documentation")));
    docs_dir.add_child(Box::new(File::new("API.md", 3000, "md", "## API Reference")));

    root.add_child(Box::new(src_dir));
    root.add_child(Box::new(docs_dir));
    root.add_child(Box::new(File::new("Cargo.toml", 400, "toml", "[package]")));

    // Size calculation
    let mut size_visitor = SizeCalculatorVisitor::new();
    root.accept(&mut size_visitor).unwrap();
    let (total_size, file_count, dir_count) = size_visitor.get_results();

    println!("Project analysis:");
    println!("  Total size: {} bytes", total_size);
    println!("  Files: {}", file_count);
    println!("  Directories: {}", dir_count);

    // File search
    let mut rust_search = FileSearchVisitor::new("rs", false);
    root.accept(&mut rust_search).unwrap();
    println!("  Rust files found: {:?}", rust_search.get_found_files());

    let mut content_search = FileSearchVisitor::new("println", true);
    root.accept(&mut content_search).unwrap();
    println!("  Files containing 'println': {:?}", content_search.get_found_files());

    // Expression Evaluation
    println!("\n2. Expression Visitor:");

    // Create expression: sqrt(abs(-16)) + (10 / 2)
    let complex_expr = Expression::Binary {
        left: Box::new(Expression::Unary {
            operand: Box::new(Expression::Unary {
                operand: Box::new(Expression::Number(-16.0)),
                operator: UnaryOperator::Abs,
            }),
            operator: UnaryOperator::Sqrt,
        }),
        right: Box::new(Expression::Binary {
            left: Box::new(Expression::Number(10.0)),
            right: Box::new(Expression::Number(2.0)),
            operator: BinaryOperator::Divide,
        }),
        operator: BinaryOperator::Add,
    };

    let mut evaluator = EvaluatorVisitor;
    let result = complex_expr.accept(&mut evaluator);
    println!("Expression result: {}", result);

    let mut formatter = FormatterVisitor;
    let formatted = complex_expr.accept(&mut formatter);
    println!("Expression string: {}", formatted);

    // Shopping Cart Analysis
    println!("\n3. Shopping Cart Visitor:");

    let cart_items: Vec<Box<dyn ShoppingItem>> = vec![
        Box::new(PhysicalProduct::new("Laptop", 999.99, 1, 2.5, (35.0, 25.0, 3.0))),
        Box::new(PhysicalProduct::new("Mouse", 29.99, 2, 0.1, (12.0, 8.0, 4.0))),
        Box::new(DigitalProduct::new("Antivirus Software", 49.99, 1, 150.0, "1 Year")),
        Box::new(DigitalProduct::new("Game", 59.99, 2, 8000.0, "Standard")),
        Box::new(Service::new("Setup Service", 99.99, 1, 2.0, "Technical")),
        Box::new(Service::new("Extended Warranty", 199.99, 1, 0.0, "Insurance")),
    ];

    println!("Cart contents:");
    for item in &cart_items {
        println!("  {} - ${:.2} x{}", item.get_name(), item.get_price(), item.get_quantity());
    }

    // Calculate taxes
    let mut tax_visitor = TaxCalculatorVisitor::new(0.08, 0.05, 0.10); // 8%, 5%, 10%
    for item in &cart_items {
        item.accept(&mut tax_visitor).unwrap();
    }

    println!("\nTax calculation:");
    println!("  Total tax: ${:.2}", tax_visitor.get_total_tax());
    println!("  Tax breakdown:");
    for (item, tax) in tax_visitor.get_breakdown() {
        println!("    {}: ${:.2}", item, tax);
    }

    // Calculate shipping
    let mut shipping_visitor = ShippingCalculatorVisitor::new();
    for item in &cart_items {
        item.accept(&mut shipping_visitor).unwrap();
    }

    println!("\nShipping calculation:");
    println!("  Total weight: {:.2} kg", shipping_visitor.get_total_weight());
    println!("  Total volume: {:.2} cubic units", shipping_visitor.get_total_volume());
    println!("  Shipping cost: ${:.2}", shipping_visitor.get_shipping_cost());

    // Calculate totals
    let subtotal: f64 = cart_items.iter()
        .map(|item| item.get_price() * item.get_quantity() as f64)
        .sum();
    let total = subtotal + tax_visitor.get_total_tax() + shipping_visitor.get_shipping_cost();

    println!("\nOrder summary:");
    println!("  Subtotal: ${:.2}", subtotal);
    println!("  Tax: ${:.2}", tax_visitor.get_total_tax());
    println!("  Shipping: ${:.2}", shipping_visitor.get_shipping_cost());
    println!("  TOTAL: ${:.2}", total);

    println!("\n4. Visitor Pattern Benefits:");
    println!("  ✅ Separates algorithms from object structures");
    println!("  ✅ Makes adding new operations easy");
    println!("  ✅ Centralizes related operations");
    println!("  ✅ Enables operations across heterogeneous objects");
    println!("  ✅ Accumulates state during traversal");
    println!("  ✅ Follows open/closed principle for operations");
}

pub fn main() {
    demo();
}