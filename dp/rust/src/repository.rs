/// Repository Pattern Implementation in Rust
///
/// The Repository pattern encapsulates the logic needed to access data sources.
/// It centralizes common data access functionality, providing better maintainability
/// and decoupling the infrastructure or technology used to access databases from the domain model layer.

use std::collections::HashMap;
use std::fs::{File, OpenOptions};
use std::io::{BufRead, BufReader, Write};
use std::path::Path;
use serde::{Deserialize, Serialize};

// Domain Entity
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct Product {
    pub id: u32,
    pub name: String,
    pub price: f64,
    pub stock: u32,
    pub category: String,
}

impl Product {
    pub fn new(id: u32, name: String, price: f64, stock: u32, category: String) -> Result<Self, String> {
        if name.is_empty() {
            return Err("Product name cannot be empty".to_string());
        }
        if price < 0.0 {
            return Err("Product price cannot be negative".to_string());
        }

        Ok(Self {
            id,
            name,
            price,
            stock,
            category,
        })
    }

    pub fn update_stock(&mut self, new_stock: u32) {
        self.stock = new_stock;
    }

    pub fn update_price(&mut self, new_price: f64) -> Result<(), String> {
        if new_price < 0.0 {
            return Err("Price cannot be negative".to_string());
        }
        self.price = new_price;
        Ok(())
    }
}

// Repository trait
pub trait Repository<T, ID> {
    type Error;

    fn save(&mut self, entity: T) -> Result<(), Self::Error>;
    fn find_by_id(&self, id: ID) -> Result<Option<T>, Self::Error>;
    fn find_all(&self) -> Result<Vec<T>, Self::Error>;
    fn update(&mut self, entity: T) -> Result<bool, Self::Error>;
    fn delete(&mut self, id: ID) -> Result<bool, Self::Error>;
    fn count(&self) -> Result<usize, Self::Error>;
}

// Product-specific repository trait
pub trait ProductRepository: Repository<Product, u32> {
    fn find_by_name(&self, name: &str) -> Result<Vec<Product>, Self::Error>;
    fn find_by_category(&self, category: &str) -> Result<Vec<Product>, Self::Error>;
    fn find_by_price_range(&self, min_price: f64, max_price: f64) -> Result<Vec<Product>, Self::Error>;
    fn find_by_stock_level(&self, min_stock: u32) -> Result<Vec<Product>, Self::Error>;
    fn find_out_of_stock(&self) -> Result<Vec<Product>, Self::Error>;
}

// In-Memory Repository Implementation
#[derive(Debug, Default)]
pub struct InMemoryProductRepository {
    products: HashMap<u32, Product>,
}

impl InMemoryProductRepository {
    pub fn new() -> Self {
        Self {
            products: HashMap::new(),
        }
    }

    pub fn with_initial_data(products: Vec<Product>) -> Self {
        let mut repo = Self::new();
        for product in products {
            let _ = repo.save(product);
        }
        repo
    }
}

impl Repository<Product, u32> for InMemoryProductRepository {
    type Error = String;

    fn save(&mut self, product: Product) -> Result<(), Self::Error> {
        self.products.insert(product.id, product);
        Ok(())
    }

    fn find_by_id(&self, id: u32) -> Result<Option<Product>, Self::Error> {
        Ok(self.products.get(&id).cloned())
    }

    fn find_all(&self) -> Result<Vec<Product>, Self::Error> {
        let mut products: Vec<Product> = self.products.values().cloned().collect();
        products.sort_by(|a, b| a.id.cmp(&b.id));
        Ok(products)
    }

    fn update(&mut self, product: Product) -> Result<bool, Self::Error> {
        if self.products.contains_key(&product.id) {
            self.products.insert(product.id, product);
            Ok(true)
        } else {
            Ok(false)
        }
    }

    fn delete(&mut self, id: u32) -> Result<bool, Self::Error> {
        Ok(self.products.remove(&id).is_some())
    }

    fn count(&self) -> Result<usize, Self::Error> {
        Ok(self.products.len())
    }
}

impl ProductRepository for InMemoryProductRepository {
    fn find_by_name(&self, name: &str) -> Result<Vec<Product>, Self::Error> {
        let results = self.products
            .values()
            .filter(|product| product.name.to_lowercase().contains(&name.to_lowercase()))
            .cloned()
            .collect();
        Ok(results)
    }

    fn find_by_category(&self, category: &str) -> Result<Vec<Product>, Self::Error> {
        let results = self.products
            .values()
            .filter(|product| product.category.eq_ignore_ascii_case(category))
            .cloned()
            .collect();
        Ok(results)
    }

    fn find_by_price_range(&self, min_price: f64, max_price: f64) -> Result<Vec<Product>, Self::Error> {
        if min_price > max_price {
            return Err("Min price cannot be greater than max price".to_string());
        }

        let results = self.products
            .values()
            .filter(|product| product.price >= min_price && product.price <= max_price)
            .cloned()
            .collect();
        Ok(results)
    }

    fn find_by_stock_level(&self, min_stock: u32) -> Result<Vec<Product>, Self::Error> {
        let results = self.products
            .values()
            .filter(|product| product.stock >= min_stock)
            .cloned()
            .collect();
        Ok(results)
    }

    fn find_out_of_stock(&self) -> Result<Vec<Product>, Self::Error> {
        let results = self.products
            .values()
            .filter(|product| product.stock == 0)
            .cloned()
            .collect();
        Ok(results)
    }
}

// File-based Repository Implementation
#[derive(Debug)]
pub struct FileProductRepository {
    file_path: String,
}

impl FileProductRepository {
    pub fn new<P: AsRef<Path>>(file_path: P) -> Self {
        Self {
            file_path: file_path.as_ref().to_string_lossy().to_string(),
        }
    }

    fn load_all_products(&self) -> Result<Vec<Product>, String> {
        if !Path::new(&self.file_path).exists() {
            return Ok(Vec::new());
        }

        let file = File::open(&self.file_path)
            .map_err(|e| format!("Failed to open file: {}", e))?;

        let reader = BufReader::new(file);
        let mut products = Vec::new();

        for line in reader.lines() {
            let line = line.map_err(|e| format!("Failed to read line: {}", e))?;
            if line.trim().is_empty() {
                continue;
            }

            let product: Product = serde_json::from_str(&line)
                .map_err(|e| format!("Failed to deserialize product: {}", e))?;
            products.push(product);
        }

        Ok(products)
    }

    fn save_all_products(&self, products: &[Product]) -> Result<(), String> {
        let mut file = OpenOptions::new()
            .write(true)
            .create(true)
            .truncate(true)
            .open(&self.file_path)
            .map_err(|e| format!("Failed to open file for writing: {}", e))?;

        for product in products {
            let json = serde_json::to_string(product)
                .map_err(|e| format!("Failed to serialize product: {}", e))?;
            writeln!(file, "{}", json)
                .map_err(|e| format!("Failed to write to file: {}", e))?;
        }

        Ok(())
    }
}

impl Repository<Product, u32> for FileProductRepository {
    type Error = String;

    fn save(&mut self, product: Product) -> Result<(), Self::Error> {
        let mut products = self.load_all_products()?;

        // Check if product already exists and update, or add new
        if let Some(existing) = products.iter_mut().find(|p| p.id == product.id) {
            *existing = product;
        } else {
            products.push(product);
        }

        self.save_all_products(&products)
    }

    fn find_by_id(&self, id: u32) -> Result<Option<Product>, Self::Error> {
        let products = self.load_all_products()?;
        Ok(products.into_iter().find(|p| p.id == id))
    }

    fn find_all(&self) -> Result<Vec<Product>, Self::Error> {
        let mut products = self.load_all_products()?;
        products.sort_by(|a, b| a.id.cmp(&b.id));
        Ok(products)
    }

    fn update(&mut self, product: Product) -> Result<bool, Self::Error> {
        let mut products = self.load_all_products()?;

        if let Some(existing) = products.iter_mut().find(|p| p.id == product.id) {
            *existing = product;
            self.save_all_products(&products)?;
            Ok(true)
        } else {
            Ok(false)
        }
    }

    fn delete(&mut self, id: u32) -> Result<bool, Self::Error> {
        let mut products = self.load_all_products()?;
        let original_len = products.len();

        products.retain(|p| p.id != id);

        if products.len() < original_len {
            self.save_all_products(&products)?;
            Ok(true)
        } else {
            Ok(false)
        }
    }

    fn count(&self) -> Result<usize, Self::Error> {
        let products = self.load_all_products()?;
        Ok(products.len())
    }
}

impl ProductRepository for FileProductRepository {
    fn find_by_name(&self, name: &str) -> Result<Vec<Product>, Self::Error> {
        let products = self.load_all_products()?;
        let results = products
            .into_iter()
            .filter(|product| product.name.to_lowercase().contains(&name.to_lowercase()))
            .collect();
        Ok(results)
    }

    fn find_by_category(&self, category: &str) -> Result<Vec<Product>, Self::Error> {
        let products = self.load_all_products()?;
        let results = products
            .into_iter()
            .filter(|product| product.category.eq_ignore_ascii_case(category))
            .collect();
        Ok(results)
    }

    fn find_by_price_range(&self, min_price: f64, max_price: f64) -> Result<Vec<Product>, Self::Error> {
        if min_price > max_price {
            return Err("Min price cannot be greater than max price".to_string());
        }

        let products = self.load_all_products()?;
        let results = products
            .into_iter()
            .filter(|product| product.price >= min_price && product.price <= max_price)
            .collect();
        Ok(results)
    }

    fn find_by_stock_level(&self, min_stock: u32) -> Result<Vec<Product>, Self::Error> {
        let products = self.load_all_products()?;
        let results = products
            .into_iter()
            .filter(|product| product.stock >= min_stock)
            .collect();
        Ok(results)
    }

    fn find_out_of_stock(&self) -> Result<Vec<Product>, Self::Error> {
        let products = self.load_all_products()?;
        let results = products
            .into_iter()
            .filter(|product| product.stock == 0)
            .collect();
        Ok(results)
    }
}

// Implement traits for Box<dyn ProductRepository> to enable trait objects
impl Repository<Product, u32> for Box<dyn ProductRepository<Error = String>> {
    type Error = String;

    fn save(&mut self, entity: Product) -> Result<(), Self::Error> {
        self.as_mut().save(entity)
    }

    fn find_by_id(&self, id: u32) -> Result<Option<Product>, Self::Error> {
        self.as_ref().find_by_id(id)
    }

    fn find_all(&self) -> Result<Vec<Product>, Self::Error> {
        self.as_ref().find_all()
    }

    fn update(&mut self, entity: Product) -> Result<bool, Self::Error> {
        self.as_mut().update(entity)
    }

    fn delete(&mut self, id: u32) -> Result<bool, Self::Error> {
        self.as_mut().delete(id)
    }

    fn count(&self) -> Result<usize, Self::Error> {
        self.as_ref().count()
    }
}

impl ProductRepository for Box<dyn ProductRepository<Error = String>> {
    fn find_by_name(&self, name: &str) -> Result<Vec<Product>, Self::Error> {
        self.as_ref().find_by_name(name)
    }

    fn find_by_category(&self, category: &str) -> Result<Vec<Product>, Self::Error> {
        self.as_ref().find_by_category(category)
    }

    fn find_by_price_range(&self, min_price: f64, max_price: f64) -> Result<Vec<Product>, Self::Error> {
        self.as_ref().find_by_price_range(min_price, max_price)
    }

    fn find_by_stock_level(&self, min_stock: u32) -> Result<Vec<Product>, Self::Error> {
        self.as_ref().find_by_stock_level(min_stock)
    }

    fn find_out_of_stock(&self) -> Result<Vec<Product>, Self::Error> {
        self.as_ref().find_out_of_stock()
    }
}

// Service Layer
pub struct ProductService<R: ProductRepository> {
    repository: R,
}

impl<R: ProductRepository> ProductService<R>
where
    R::Error: std::fmt::Display,
{
    pub fn new(repository: R) -> Self {
        Self { repository }
    }

    pub fn add_product(&mut self, id: u32, name: String, price: f64, stock: u32, category: String) -> Result<(), String> {
        let product = Product::new(id, name, price, stock, category)?;

        // Check if product already exists
        match self.repository.find_by_id(id) {
            Ok(Some(_)) => Err(format!("Product with ID {} already exists", id)),
            Ok(None) => self.repository.save(product).map_err(|e| e.to_string()),
            Err(e) => Err(e.to_string()),
        }
    }

    pub fn get_product(&self, id: u32) -> Result<Option<Product>, String> {
        self.repository.find_by_id(id).map_err(|e| e.to_string())
    }

    pub fn update_product(&mut self, product: Product) -> Result<bool, String> {
        self.repository.update(product).map_err(|e| e.to_string())
    }

    pub fn remove_product(&mut self, id: u32) -> Result<bool, String> {
        self.repository.delete(id).map_err(|e| e.to_string())
    }

    pub fn list_all_products(&self) -> Result<Vec<Product>, String> {
        self.repository.find_all().map_err(|e| e.to_string())
    }

    pub fn search_products(&self, query: &str) -> Result<Vec<Product>, String> {
        self.repository.find_by_name(query).map_err(|e| e.to_string())
    }

    pub fn get_products_by_category(&self, category: &str) -> Result<Vec<Product>, String> {
        self.repository.find_by_category(category).map_err(|e| e.to_string())
    }

    pub fn get_affordable_products(&self, max_budget: f64) -> Result<Vec<Product>, String> {
        self.repository.find_by_price_range(0.0, max_budget).map_err(|e| e.to_string())
    }

    pub fn get_available_products(&self, min_stock: u32) -> Result<Vec<Product>, String> {
        self.repository.find_by_stock_level(min_stock).map_err(|e| e.to_string())
    }

    pub fn get_out_of_stock_products(&self) -> Result<Vec<Product>, String> {
        self.repository.find_out_of_stock().map_err(|e| e.to_string())
    }

    pub fn get_product_count(&self) -> Result<usize, String> {
        self.repository.count().map_err(|e| e.to_string())
    }

    pub fn restock_product(&mut self, id: u32, additional_stock: u32) -> Result<bool, String> {
        match self.repository.find_by_id(id) {
            Ok(Some(mut product)) => {
                product.update_stock(product.stock + additional_stock);
                self.repository.update(product).map_err(|e| e.to_string())
            }
            Ok(None) => Ok(false),
            Err(e) => Err(e.to_string()),
        }
    }

    pub fn update_product_price(&mut self, id: u32, new_price: f64) -> Result<bool, String> {
        match self.repository.find_by_id(id) {
            Ok(Some(mut product)) => {
                product.update_price(new_price)?;
                self.repository.update(product).map_err(|e| e.to_string())
            }
            Ok(None) => Ok(false),
            Err(e) => Err(e.to_string()),
        }
    }
}

// Repository factory for different storage types
pub enum StorageType {
    InMemory,
    File(String),
}

pub struct RepositoryFactory;

impl RepositoryFactory {
    pub fn create_product_repository(storage_type: StorageType) -> Box<dyn ProductRepository<Error = String>> {
        match storage_type {
            StorageType::InMemory => Box::new(InMemoryProductRepository::new()),
            StorageType::File(path) => Box::new(FileProductRepository::new(path)),
        }
    }

    pub fn create_product_service(storage_type: StorageType) -> ProductService<Box<dyn ProductRepository<Error = String>>> {
        let repository = Self::create_product_repository(storage_type);
        ProductService::new(repository)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs;

    #[test]
    fn test_product_creation() {
        let product = Product::new(1, "Test Product".to_string(), 19.99, 10, "Electronics".to_string()).unwrap();
        assert_eq!(product.id, 1);
        assert_eq!(product.name, "Test Product");
        assert_eq!(product.price, 19.99);
        assert_eq!(product.stock, 10);
    }

    #[test]
    fn test_product_validation() {
        assert!(Product::new(1, "".to_string(), 19.99, 10, "Electronics".to_string()).is_err());
        assert!(Product::new(1, "Test".to_string(), -1.0, 10, "Electronics".to_string()).is_err());
    }

    #[test]
    fn test_in_memory_repository() {
        let mut repo = InMemoryProductRepository::new();
        let product = Product::new(1, "Test".to_string(), 19.99, 10, "Electronics".to_string()).unwrap();

        // Test save
        assert!(repo.save(product.clone()).is_ok());
        assert_eq!(repo.count().unwrap(), 1);

        // Test find_by_id
        let found = repo.find_by_id(1).unwrap();
        assert!(found.is_some());
        assert_eq!(found.unwrap(), product);

        // Test find_all
        let all = repo.find_all().unwrap();
        assert_eq!(all.len(), 1);

        // Test delete
        assert!(repo.delete(1).unwrap());
        assert_eq!(repo.count().unwrap(), 0);
    }

    #[test]
    fn test_product_service() {
        let repo = InMemoryProductRepository::new();
        let mut service = ProductService::new(repo);

        // Test add product
        assert!(service.add_product(1, "Laptop".to_string(), 999.99, 5, "Electronics".to_string()).is_ok());
        assert_eq!(service.get_product_count().unwrap(), 1);

        // Test get product
        let product = service.get_product(1).unwrap();
        assert!(product.is_some());
        assert_eq!(product.unwrap().name, "Laptop");

        // Test update price
        assert!(service.update_product_price(1, 899.99).unwrap());

        // Test restock
        assert!(service.restock_product(1, 3).unwrap());
        let updated_product = service.get_product(1).unwrap().unwrap();
        assert_eq!(updated_product.stock, 8);
    }

    #[test]
    fn test_file_repository() {
        let test_file = "test_products.json";

        // Clean up before test
        let _ = fs::remove_file(test_file);

        {
            let mut repo = FileProductRepository::new(test_file);
            let product = Product::new(1, "Test Product".to_string(), 29.99, 15, "Test".to_string()).unwrap();

            // Test save and persistence
            assert!(repo.save(product.clone()).is_ok());
        }

        {
            // Test loading from file
            let repo = FileProductRepository::new(test_file);
            let products = repo.find_all().unwrap();
            assert_eq!(products.len(), 1);
            assert_eq!(products[0].name, "Test Product");
        }

        // Clean up after test
        let _ = fs::remove_file(test_file);
    }
}

fn main() {
    println!("=== Repository Pattern Demo ===\n");

    // 1. In-Memory Repository Demo
    println!("1. In-Memory Repository Demo:");
    let mut memory_service = RepositoryFactory::create_product_service(StorageType::InMemory);

    // Add products
    let products = vec![
        (1, "MacBook Pro", 2499.99, 5, "Electronics"),
        (2, "Wireless Mouse", 79.99, 25, "Electronics"),
        (3, "Office Chair", 299.99, 10, "Furniture"),
        (4, "Standing Desk", 599.99, 3, "Furniture"),
        (5, "Coffee Mug", 15.99, 0, "Kitchen"),
    ];

    for (id, name, price, stock, category) in products {
        match memory_service.add_product(id, name.to_string(), price, stock, category.to_string()) {
            Ok(_) => println!("✓ Added product: {}", name),
            Err(e) => println!("✗ Failed to add {}: {}", name, e),
        }
    }

    println!("\nTotal products: {}", memory_service.get_product_count().unwrap());

    // Search operations
    println!("\n2. Search Operations:");

    // Find by name
    match memory_service.search_products("Mac") {
        Ok(results) => {
            println!("Products containing 'Mac': {} found", results.len());
            for product in results {
                println!("  - {} (${:.2})", product.name, product.price);
            }
        }
        Err(e) => println!("Search failed: {}", e),
    }

    // Find by category
    match memory_service.get_products_by_category("Electronics") {
        Ok(electronics) => {
            println!("Electronics category: {} products", electronics.len());
            for product in electronics {
                println!("  - {} (Stock: {})", product.name, product.stock);
            }
        }
        Err(e) => println!("Category search failed: {}", e),
    }

    // Find affordable products
    match memory_service.get_affordable_products(100.0) {
        Ok(affordable) => {
            println!("Products under $100: {} found", affordable.len());
            for product in affordable {
                println!("  - {} (${:.2})", product.name, product.price);
            }
        }
        Err(e) => println!("Price range search failed: {}", e),
    }

    // Find out of stock
    match memory_service.get_out_of_stock_products() {
        Ok(out_of_stock) => {
            println!("Out of stock products: {}", out_of_stock.len());
            for product in out_of_stock {
                println!("  - {} (Stock: {})", product.name, product.stock);
            }
        }
        Err(e) => println!("Out of stock search failed: {}", e),
    }

    // 3. Product updates
    println!("\n3. Product Operations:");

    // Update price
    match memory_service.update_product_price(1, 2299.99) {
        Ok(true) => {
            println!("✓ Updated MacBook Pro price");
            if let Ok(Some(product)) = memory_service.get_product(1) {
                println!("  New price: ${:.2}", product.price);
            }
        }
        Ok(false) => println!("✗ Product not found for price update"),
        Err(e) => println!("✗ Price update failed: {}", e),
    }

    // Restock product
    match memory_service.restock_product(5, 20) {
        Ok(true) => {
            println!("✓ Restocked Coffee Mug");
            if let Ok(Some(product)) = memory_service.get_product(5) {
                println!("  New stock: {}", product.stock);
            }
        }
        Ok(false) => println!("✗ Product not found for restocking"),
        Err(e) => println!("✗ Restocking failed: {}", e),
    }

    // 4. File Repository Demo
    println!("\n4. File Repository Demo:");
    let test_file = "demo_products.json";

    {
        let mut file_service = RepositoryFactory::create_product_service(StorageType::File(test_file.to_string()));

        // Add products to file repository
        let _ = file_service.add_product(100, "File Product 1".to_string(), 49.99, 15, "Test".to_string());
        let _ = file_service.add_product(101, "File Product 2".to_string(), 89.99, 8, "Test".to_string());

        println!("✓ Products saved to file: {}", test_file);
        println!("File repository product count: {}", file_service.get_product_count().unwrap());
    }

    // Test persistence by loading from file
    {
        let file_service = RepositoryFactory::create_product_service(StorageType::File(test_file.to_string()));
        println!("✓ Loaded from file - Product count: {}", file_service.get_product_count().unwrap());

        if let Ok(products) = file_service.list_all_products() {
            println!("Persistent products:");
            for product in products {
                println!("  - {} (${:.2}, Stock: {})", product.name, product.price, product.stock);
            }
        }
    }

    // Clean up
    let _ = std::fs::remove_file(test_file);

    println!("\n✅ Repository pattern provides consistent data access");
    println!("   across different storage implementations!");
}