/// Strategy Pattern Implementation in Rust
///
/// The Strategy pattern defines a family of algorithms, encapsulates each one,
/// and makes them interchangeable. It lets the algorithm vary independently
/// from clients that use it, promoting flexibility and code reuse.

use std::collections::HashMap;

/// Sorting strategy trait
pub trait SortStrategy<T> {
    fn sort(&self, data: &mut [T]) where T: PartialOrd + Clone;
    fn name(&self) -> &str;
    fn complexity(&self) -> &str;
}

/// Bubble sort implementation
pub struct BubbleSort;

impl<T> SortStrategy<T> for BubbleSort {
    fn sort(&self, data: &mut [T]) where T: PartialOrd + Clone {
        let len = data.len();
        for i in 0..len {
            for j in 0..(len - i - 1) {
                if data[j] > data[j + 1] {
                    data.swap(j, j + 1);
                }
            }
        }
    }

    fn name(&self) -> &str {
        "Bubble Sort"
    }

    fn complexity(&self) -> &str {
        "O(n²)"
    }
}

/// Quick sort implementation
pub struct QuickSort;

impl<T> SortStrategy<T> for QuickSort {
    fn sort(&self, data: &mut [T]) where T: PartialOrd + Clone {
        if data.len() <= 1 {
            return;
        }
        self.quick_sort_recursive(data, 0, data.len() - 1);
    }

    fn name(&self) -> &str {
        "Quick Sort"
    }

    fn complexity(&self) -> &str {
        "O(n log n) average, O(n²) worst"
    }
}

impl QuickSort {
    fn quick_sort_recursive<T>(&self, data: &mut [T], low: usize, high: usize)
    where
        T: PartialOrd + Clone,
    {
        if low < high {
            let pivot = self.partition(data, low, high);
            if pivot > 0 {
                self.quick_sort_recursive(data, low, pivot - 1);
            }
            if pivot + 1 < data.len() {
                self.quick_sort_recursive(data, pivot + 1, high);
            }
        }
    }

    fn partition<T>(&self, data: &mut [T], low: usize, high: usize) -> usize
    where
        T: PartialOrd + Clone,
    {
        let mut i = low;
        for j in low..high {
            if data[j] <= data[high] {
                data.swap(i, j);
                i += 1;
            }
        }
        data.swap(i, high);
        i
    }
}

/// Merge sort implementation
pub struct MergeSort;

impl<T> SortStrategy<T> for MergeSort {
    fn sort(&self, data: &mut [T]) where T: PartialOrd + Clone {
        if data.len() <= 1 {
            return;
        }
        let mut temp = data.to_vec();
        self.merge_sort_recursive(data, &mut temp, 0, data.len() - 1);
    }

    fn name(&self) -> &str {
        "Merge Sort"
    }

    fn complexity(&self) -> &str {
        "O(n log n)"
    }
}

impl MergeSort {
    fn merge_sort_recursive<T>(
        &self,
        data: &mut [T],
        temp: &mut [T],
        left: usize,
        right: usize,
    ) where
        T: PartialOrd + Clone,
    {
        if left < right {
            let mid = left + (right - left) / 2;
            self.merge_sort_recursive(data, temp, left, mid);
            self.merge_sort_recursive(data, temp, mid + 1, right);
            self.merge(data, temp, left, mid, right);
        }
    }

    fn merge<T>(
        &self,
        data: &mut [T],
        temp: &mut [T],
        left: usize,
        mid: usize,
        right: usize,
    ) where
        T: PartialOrd + Clone,
    {
        // Copy data to temp array
        for i in left..=right {
            temp[i] = data[i].clone();
        }

        let mut i = left;
        let mut j = mid + 1;
        let mut k = left;

        while i <= mid && j <= right {
            if temp[i] <= temp[j] {
                data[k] = temp[i].clone();
                i += 1;
            } else {
                data[k] = temp[j].clone();
                j += 1;
            }
            k += 1;
        }

        while i <= mid {
            data[k] = temp[i].clone();
            i += 1;
            k += 1;
        }

        while j <= right {
            data[k] = temp[j].clone();
            j += 1;
            k += 1;
        }
    }
}

/// Sorter context that uses different strategies
pub struct Sorter<T> {
    strategy: Box<dyn SortStrategy<T>>,
}

impl<T> Sorter<T> {
    pub fn new(strategy: Box<dyn SortStrategy<T>>) -> Self {
        Sorter { strategy }
    }

    pub fn set_strategy(&mut self, strategy: Box<dyn SortStrategy<T>>) {
        self.strategy = strategy;
    }

    pub fn sort(&self, data: &mut [T]) where T: PartialOrd + Clone {
        self.strategy.sort(data);
    }

    pub fn get_strategy_info(&self) -> (&str, &str) {
        (self.strategy.name(), self.strategy.complexity())
    }
}

/// Payment processing strategies
pub trait PaymentStrategy {
    fn pay(&self, amount: f64) -> Result<String, String>;
    fn validate(&self) -> Result<(), String>;
    fn get_name(&self) -> &str;
    fn get_fees(&self, amount: f64) -> f64;
}

pub struct CreditCardPayment {
    card_number: String,
    cardholder_name: String,
    expiry_date: String,
    cvv: String,
}

impl CreditCardPayment {
    pub fn new(card_number: &str, cardholder_name: &str, expiry_date: &str, cvv: &str) -> Self {
        CreditCardPayment {
            card_number: card_number.to_string(),
            cardholder_name: cardholder_name.to_string(),
            expiry_date: expiry_date.to_string(),
            cvv: cvv.to_string(),
        }
    }

    fn validate_card_number(&self) -> bool {
        // Simple Luhn algorithm check (simplified)
        self.card_number.len() >= 13 && self.card_number.len() <= 19
    }

    fn mask_card_number(&self) -> String {
        if self.card_number.len() < 4 {
            return "*".repeat(self.card_number.len());
        }
        let last_four = &self.card_number[self.card_number.len() - 4..];
        format!("****-****-****-{}", last_four)
    }
}

impl PaymentStrategy for CreditCardPayment {
    fn pay(&self, amount: f64) -> Result<String, String> {
        self.validate()?;
        let fees = self.get_fees(amount);
        let total = amount + fees;

        Ok(format!(
            "Paid ${:.2} (+ ${:.2} fees) using credit card {}. Transaction ID: CC-{}",
            amount,
            fees,
            self.mask_card_number(),
            format!("{:08X}", (amount * 1000.0) as u32)
        ))
    }

    fn validate(&self) -> Result<(), String> {
        if !self.validate_card_number() {
            return Err("Invalid card number".to_string());
        }
        if self.cardholder_name.is_empty() {
            return Err("Cardholder name is required".to_string());
        }
        if self.cvv.len() != 3 && self.cvv.len() != 4 {
            return Err("Invalid CVV".to_string());
        }
        Ok(())
    }

    fn get_name(&self) -> &str {
        "Credit Card"
    }

    fn get_fees(&self, amount: f64) -> f64 {
        amount * 0.029 + 0.30 // 2.9% + $0.30
    }
}

pub struct PayPalPayment {
    email: String,
    password: String,
}

impl PayPalPayment {
    pub fn new(email: &str, password: &str) -> Self {
        PayPalPayment {
            email: email.to_string(),
            password: password.to_string(),
        }
    }

    fn validate_email(&self) -> bool {
        self.email.contains('@') && self.email.contains('.')
    }
}

impl PaymentStrategy for PayPalPayment {
    fn pay(&self, amount: f64) -> Result<String, String> {
        self.validate()?;
        let fees = self.get_fees(amount);
        let total = amount + fees;

        Ok(format!(
            "Paid ${:.2} (+ ${:.2} fees) using PayPal account {}. Transaction ID: PP-{}",
            amount,
            fees,
            self.email,
            format!("{:08X}", (amount * 2000.0) as u32)
        ))
    }

    fn validate(&self) -> Result<(), String> {
        if !self.validate_email() {
            return Err("Invalid email address".to_string());
        }
        if self.password.len() < 6 {
            return Err("Password too short".to_string());
        }
        Ok(())
    }

    fn get_name(&self) -> &str {
        "PayPal"
    }

    fn get_fees(&self, amount: f64) -> f64 {
        if amount <= 10.00 {
            0.39 + amount * 0.024
        } else {
            amount * 0.034 + 0.49
        }
    }
}

pub struct BankTransferPayment {
    account_number: String,
    routing_number: String,
    bank_name: String,
}

impl BankTransferPayment {
    pub fn new(account_number: &str, routing_number: &str, bank_name: &str) -> Self {
        BankTransferPayment {
            account_number: account_number.to_string(),
            routing_number: routing_number.to_string(),
            bank_name: bank_name.to_string(),
        }
    }

    fn mask_account_number(&self) -> String {
        if self.account_number.len() < 4 {
            return "*".repeat(self.account_number.len());
        }
        let last_four = &self.account_number[self.account_number.len() - 4..];
        format!("****{}", last_four)
    }
}

impl PaymentStrategy for BankTransferPayment {
    fn pay(&self, amount: f64) -> Result<String, String> {
        self.validate()?;
        let fees = self.get_fees(amount);
        let total = amount + fees;

        Ok(format!(
            "Paid ${:.2} (+ ${:.2} fees) via bank transfer from {} account {}. Transaction ID: BT-{}",
            amount,
            fees,
            self.bank_name,
            self.mask_account_number(),
            format!("{:08X}", (amount * 3000.0) as u32)
        ))
    }

    fn validate(&self) -> Result<(), String> {
        if self.account_number.len() < 8 {
            return Err("Invalid account number".to_string());
        }
        if self.routing_number.len() != 9 {
            return Err("Invalid routing number".to_string());
        }
        if self.bank_name.is_empty() {
            return Err("Bank name is required".to_string());
        }
        Ok(())
    }

    fn get_name(&self) -> &str {
        "Bank Transfer"
    }

    fn get_fees(&self, amount: f64) -> f64 {
        if amount >= 1000.00 {
            0.00 // Free for large transfers
        } else {
            15.00 // Flat fee for smaller transfers
        }
    }
}

/// Shopping cart that uses different payment strategies
pub struct ShoppingCart {
    items: HashMap<String, CartItem>,
    payment_strategy: Option<Box<dyn PaymentStrategy>>,
}

#[derive(Debug, Clone)]
pub struct CartItem {
    pub name: String,
    pub price: f64,
    pub quantity: u32,
}

impl ShoppingCart {
    pub fn new() -> Self {
        ShoppingCart {
            items: HashMap::new(),
            payment_strategy: None,
        }
    }

    pub fn add_item(&mut self, name: &str, price: f64, quantity: u32) {
        let item = CartItem {
            name: name.to_string(),
            price,
            quantity,
        };
        self.items.insert(name.to_string(), item);
    }

    pub fn remove_item(&mut self, name: &str) -> Option<CartItem> {
        self.items.remove(name)
    }

    pub fn get_total(&self) -> f64 {
        self.items
            .values()
            .map(|item| item.price * item.quantity as f64)
            .sum()
    }

    pub fn set_payment_strategy(&mut self, strategy: Box<dyn PaymentStrategy>) {
        self.payment_strategy = Some(strategy);
    }

    pub fn checkout(&self) -> Result<String, String> {
        if self.items.is_empty() {
            return Err("Cart is empty".to_string());
        }

        let strategy = self.payment_strategy
            .as_ref()
            .ok_or("No payment method selected")?;

        let total = self.get_total();
        let fees = strategy.get_fees(total);

        println!("Order Summary:");
        for item in self.items.values() {
            println!("  {} x{} @ ${:.2} = ${:.2}",
                    item.name, item.quantity, item.price,
                    item.price * item.quantity as f64);
        }
        println!("  Subtotal: ${:.2}", total);
        println!("  Payment fees: ${:.2}", fees);
        println!("  Total: ${:.2}", total + fees);
        println!();

        strategy.pay(total)
    }

    pub fn get_payment_method_info(&self) -> Option<(&str, f64)> {
        self.payment_strategy.as_ref().map(|strategy| {
            let total = self.get_total();
            (strategy.get_name(), strategy.get_fees(total))
        })
    }

    pub fn get_item_count(&self) -> usize {
        self.items.len()
    }

    pub fn is_empty(&self) -> bool {
        self.items.is_empty()
    }
}

impl Default for ShoppingCart {
    fn default() -> Self {
        Self::new()
    }
}

/// Compression strategies
pub trait CompressionStrategy {
    fn compress(&self, data: &[u8]) -> Result<Vec<u8>, String>;
    fn decompress(&self, data: &[u8]) -> Result<Vec<u8>, String>;
    fn get_name(&self) -> &str;
    fn get_compression_ratio(&self, original_size: usize, compressed_size: usize) -> f32;
}

pub struct ZipCompression;

impl CompressionStrategy for ZipCompression {
    fn compress(&self, data: &[u8]) -> Result<Vec<u8>, String> {
        // Simulate ZIP compression
        let compressed_size = (data.len() as f32 * 0.6) as usize;
        let mut compressed = vec![0x50, 0x4B]; // ZIP signature
        compressed.extend(data.iter().take(compressed_size));
        Ok(compressed)
    }

    fn decompress(&self, data: &[u8]) -> Result<Vec<u8>, String> {
        if data.len() < 2 || data[0] != 0x50 || data[1] != 0x4B {
            return Err("Invalid ZIP format".to_string());
        }
        // Simulate decompression by expanding data
        let original_size = ((data.len() - 2) as f32 / 0.6) as usize;
        Ok(vec![0u8; original_size])
    }

    fn get_name(&self) -> &str {
        "ZIP"
    }

    fn get_compression_ratio(&self, original_size: usize, compressed_size: usize) -> f32 {
        if original_size == 0 {
            return 0.0;
        }
        (compressed_size as f32 / original_size as f32) * 100.0
    }
}

pub struct GzipCompression;

impl CompressionStrategy for GzipCompression {
    fn compress(&self, data: &[u8]) -> Result<Vec<u8>, String> {
        // Simulate GZIP compression
        let compressed_size = (data.len() as f32 * 0.4) as usize;
        let mut compressed = vec![0x1F, 0x8B]; // GZIP signature
        compressed.extend(data.iter().take(compressed_size));
        Ok(compressed)
    }

    fn decompress(&self, data: &[u8]) -> Result<Vec<u8>, String> {
        if data.len() < 2 || data[0] != 0x1F || data[1] != 0x8B {
            return Err("Invalid GZIP format".to_string());
        }
        let original_size = ((data.len() - 2) as f32 / 0.4) as usize;
        Ok(vec![0u8; original_size])
    }

    fn get_name(&self) -> &str {
        "GZIP"
    }

    fn get_compression_ratio(&self, original_size: usize, compressed_size: usize) -> f32 {
        if original_size == 0 {
            return 0.0;
        }
        (compressed_size as f32 / original_size as f32) * 100.0
    }
}

pub struct LzwCompression;

impl CompressionStrategy for LzwCompression {
    fn compress(&self, data: &[u8]) -> Result<Vec<u8>, String> {
        // Simulate LZW compression
        let compressed_size = (data.len() as f32 * 0.5) as usize;
        let mut compressed = vec![0x4C, 0x5A]; // LZ signature
        compressed.extend(data.iter().take(compressed_size));
        Ok(compressed)
    }

    fn decompress(&self, data: &[u8]) -> Result<Vec<u8>, String> {
        if data.len() < 2 || data[0] != 0x4C || data[1] != 0x5A {
            return Err("Invalid LZW format".to_string());
        }
        let original_size = ((data.len() - 2) as f32 / 0.5) as usize;
        Ok(vec![0u8; original_size])
    }

    fn get_name(&self) -> &str {
        "LZW"
    }

    fn get_compression_ratio(&self, original_size: usize, compressed_size: usize) -> f32 {
        if original_size == 0 {
            return 0.0;
        }
        (compressed_size as f32 / original_size as f32) * 100.0
    }
}

/// File compressor that uses different compression strategies
pub struct FileCompressor {
    strategy: Box<dyn CompressionStrategy>,
}

impl FileCompressor {
    pub fn new(strategy: Box<dyn CompressionStrategy>) -> Self {
        FileCompressor { strategy }
    }

    pub fn set_strategy(&mut self, strategy: Box<dyn CompressionStrategy>) {
        self.strategy = strategy;
    }

    pub fn compress_data(&self, data: &[u8]) -> Result<(Vec<u8>, f32), String> {
        let compressed = self.strategy.compress(data)?;
        let ratio = self.strategy.get_compression_ratio(data.len(), compressed.len());
        Ok((compressed, ratio))
    }

    pub fn decompress_data(&self, data: &[u8]) -> Result<Vec<u8>, String> {
        self.strategy.decompress(data)
    }

    pub fn get_strategy_name(&self) -> &str {
        self.strategy.get_name()
    }

    pub fn benchmark_compression(&mut self, data: &[u8]) -> CompressionBenchmark {
        let strategies: Vec<Box<dyn CompressionStrategy>> = vec![
            Box::new(ZipCompression),
            Box::new(GzipCompression),
            Box::new(LzwCompression),
        ];

        let mut results = Vec::new();

        for strategy in strategies {
            let name = strategy.get_name().to_string();
            self.set_strategy(strategy);

            let start_time = std::time::Instant::now();
            match self.compress_data(data) {
                Ok((compressed, ratio)) => {
                    let compress_time = start_time.elapsed();

                    let start_time = std::time::Instant::now();
                    match self.decompress_data(&compressed) {
                        Ok(_) => {
                            let decompress_time = start_time.elapsed();
                            results.push(CompressionResult {
                                algorithm: name,
                                compression_ratio: ratio,
                                compress_time: compress_time.as_millis(),
                                decompress_time: decompress_time.as_millis(),
                                success: true,
                            });
                        }
                        Err(_) => {
                            results.push(CompressionResult {
                                algorithm: name,
                                compression_ratio: 0.0,
                                compress_time: compress_time.as_millis(),
                                decompress_time: 0,
                                success: false,
                            });
                        }
                    }
                }
                Err(_) => {
                    results.push(CompressionResult {
                        algorithm: name,
                        compression_ratio: 0.0,
                        compress_time: 0,
                        decompress_time: 0,
                        success: false,
                    });
                }
            }
        }

        CompressionBenchmark {
            original_size: data.len(),
            results,
        }
    }
}

#[derive(Debug)]
pub struct CompressionBenchmark {
    pub original_size: usize,
    pub results: Vec<CompressionResult>,
}

#[derive(Debug)]
pub struct CompressionResult {
    pub algorithm: String,
    pub compression_ratio: f32,
    pub compress_time: u128,
    pub decompress_time: u128,
    pub success: bool,
}

impl CompressionBenchmark {
    pub fn get_best_compression(&self) -> Option<&CompressionResult> {
        self.results
            .iter()
            .filter(|r| r.success)
            .min_by(|a, b| a.compression_ratio.partial_cmp(&b.compression_ratio).unwrap())
    }

    pub fn get_fastest_compression(&self) -> Option<&CompressionResult> {
        self.results
            .iter()
            .filter(|r| r.success)
            .min_by_key(|r| r.compress_time)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sorting_strategies() {
        let mut data = vec![64, 34, 25, 12, 22, 11, 90];
        let original = data.clone();

        let mut sorter = Sorter::new(Box::new(BubbleSort));
        sorter.sort(&mut data);
        assert_eq!(data, vec![11, 12, 22, 25, 34, 64, 90]);

        // Reset data and try QuickSort
        data = original.clone();
        sorter.set_strategy(Box::new(QuickSort));
        sorter.sort(&mut data);
        assert_eq!(data, vec![11, 12, 22, 25, 34, 64, 90]);

        // Test strategy info
        let (name, complexity) = sorter.get_strategy_info();
        assert_eq!(name, "Quick Sort");
        assert!(complexity.contains("O(n log n)"));
    }

    #[test]
    fn test_payment_strategies() {
        let mut cart = ShoppingCart::new();
        cart.add_item("Laptop", 999.99, 1);
        cart.add_item("Mouse", 29.99, 2);

        let credit_card = CreditCardPayment::new(
            "1234567890123456",
            "John Doe",
            "12/25",
            "123"
        );

        cart.set_payment_strategy(Box::new(credit_card));

        assert!(cart.checkout().is_ok());
        assert_eq!(cart.get_total(), 1059.97);
    }

    #[test]
    fn test_compression_strategies() {
        let data = b"Hello, World! This is test data for compression.";
        let mut compressor = FileCompressor::new(Box::new(ZipCompression));

        let (compressed, ratio) = compressor.compress_data(data).unwrap();
        assert!(compressed.len() < data.len());
        assert!(ratio < 100.0);

        let decompressed = compressor.decompress_data(&compressed).unwrap();
        assert_eq!(decompressed.len(), ((data.len() as f32) / 0.6) as usize);
    }

    #[test]
    fn test_invalid_payment_validation() {
        let invalid_card = CreditCardPayment::new("123", "John", "12/25", "12");
        assert!(invalid_card.validate().is_err());

        let invalid_paypal = PayPalPayment::new("invalid-email", "123");
        assert!(invalid_paypal.validate().is_err());

        let invalid_bank = BankTransferPayment::new("123", "12345", "Bank");
        assert!(invalid_bank.validate().is_err());
    }

    #[test]
    fn test_shopping_cart_operations() {
        let mut cart = ShoppingCart::new();
        assert!(cart.is_empty());

        cart.add_item("Book", 19.99, 3);
        assert_eq!(cart.get_item_count(), 1);
        assert_eq!(cart.get_total(), 59.97);

        let removed = cart.remove_item("Book");
        assert!(removed.is_some());
        assert!(cart.is_empty());
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Strategy Pattern Demo ===");

    // Sorting strategies
    println!("\n1. Sorting Algorithms Strategy:");
    let mut data = vec![64, 34, 25, 12, 22, 11, 90];
    println!("Original data: {:?}", data);

    let strategies: Vec<Box<dyn SortStrategy<i32>>> = vec![
        Box::new(BubbleSort),
        Box::new(QuickSort),
        Box::new(MergeSort),
    ];

    for strategy in strategies {
        let mut test_data = data.clone();
        let sorter = Sorter::new(strategy);
        let (name, complexity) = sorter.get_strategy_info();

        println!("\nUsing {}: {}", name, complexity);
        sorter.sort(&mut test_data);
        println!("Sorted: {:?}", test_data);
    }

    // Payment strategies
    println!("\n2. Payment Processing Strategies:");
    let mut cart = ShoppingCart::new();

    cart.add_item("Gaming Laptop", 1299.99, 1);
    cart.add_item("Wireless Mouse", 79.99, 1);
    cart.add_item("Mechanical Keyboard", 149.99, 1);

    println!("Shopping Cart Total: ${:.2}", cart.get_total());

    // Try different payment methods
    let payment_methods: Vec<Box<dyn PaymentStrategy>> = vec![
        Box::new(CreditCardPayment::new(
            "4532123456789012",
            "Alice Johnson",
            "12/26",
            "123"
        )),
        Box::new(PayPalPayment::new(
            "alice@example.com",
            "secure_password"
        )),
        Box::new(BankTransferPayment::new(
            "1234567890",
            "123456789",
            "Chase Bank"
        )),
    ];

    for (i, payment_method) in payment_methods.into_iter().enumerate() {
        println!("\n--- Payment Method {} ---", i + 1);
        cart.set_payment_strategy(payment_method);

        if let Some((method, fees)) = cart.get_payment_method_info() {
            println!("Payment method: {} (fees: ${:.2})", method, fees);
        }

        match cart.checkout() {
            Ok(receipt) => println!("✅ {}", receipt),
            Err(error) => println!("❌ Payment failed: {}", error),
        }
    }

    // Compression strategies
    println!("\n3. File Compression Strategies:");
    let test_data = b"Lorem ipsum dolor sit amet, consectetur adipiscing elit. \
                     Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. \
                     Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris.";

    let mut compressor = FileCompressor::new(Box::new(ZipCompression));

    println!("Original data size: {} bytes", test_data.len());
    println!("Original data: {}...", String::from_utf8_lossy(&test_data[..50]));

    // Test each compression strategy
    let strategies: Vec<Box<dyn CompressionStrategy>> = vec![
        Box::new(ZipCompression),
        Box::new(GzipCompression),
        Box::new(LzwCompression),
    ];

    for strategy in strategies {
        compressor.set_strategy(strategy);
        let name = compressor.strategy.get_name();

        match compressor.compress_data(test_data) {
            Ok((compressed, ratio)) => {
                println!("\n{} compression:", name);
                println!("  Compressed size: {} bytes", compressed.len());
                println!("  Compression ratio: {:.1}%", ratio);

                match compressor.decompress_data(&compressed) {
                    Ok(decompressed) => {
                        println!("  Decompressed size: {} bytes", decompressed.len());
                        println!("  ✅ Round-trip successful");
                    }
                    Err(e) => println!("  ❌ Decompression failed: {}", e),
                }
            }
            Err(e) => println!("\n{} compression failed: {}", name, e),
        }
    }

    // Compression benchmark
    println!("\n4. Compression Algorithm Benchmark:");
    let benchmark = compressor.benchmark_compression(test_data);

    println!("Benchmark results for {} bytes:", benchmark.original_size);
    for result in &benchmark.results {
        if result.success {
            println!("  {}: {:.1}% ratio, {}ms compress, {}ms decompress",
                    result.algorithm,
                    result.compression_ratio,
                    result.compress_time,
                    result.decompress_time);
        } else {
            println!("  {}: Failed", result.algorithm);
        }
    }

    if let Some(best) = benchmark.get_best_compression() {
        println!("\nBest compression: {} ({:.1}% ratio)",
                best.algorithm, best.compression_ratio);
    }

    if let Some(fastest) = benchmark.get_fastest_compression() {
        println!("Fastest compression: {} ({}ms)",
                fastest.algorithm, fastest.compress_time);
    }
}

pub fn main() {
    demo();
}