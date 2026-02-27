/// Proxy Pattern Implementation in Rust
///
/// The Proxy pattern provides a placeholder or surrogate for another object
/// to control access to it. It can add functionality like lazy initialization,
/// access control, caching, or logging without changing the original object.

use std::collections::HashMap;
use std::time::{SystemTime, Duration};
use std::cell::RefCell;

/// Subject trait for image operations
pub trait Image {
    fn display(&self) -> Result<String, String>;
    fn get_info(&self) -> ImageInfo;
    fn get_size(&self) -> (u32, u32);
}

#[derive(Debug, Clone)]
pub struct ImageInfo {
    pub filename: String,
    pub format: String,
    pub file_size: u64,
    pub loaded: bool,
}

/// Real subject - actual image implementation
pub struct RealImage {
    filename: String,
    width: u32,
    height: u32,
    data: Vec<u8>,
    format: String,
    file_size: u64,
}

impl RealImage {
    pub fn new(filename: &str) -> Result<Self, String> {
        println!("Loading image from disk: {}", filename);

        // Simulate loading image from disk
        let data = Self::load_from_disk(filename)?;
        let (width, height) = Self::parse_dimensions(&data);
        let format = Self::detect_format(filename);
        let file_size = data.len() as u64;

        Ok(RealImage {
            filename: filename.to_string(),
            width,
            height,
            data,
            format,
            file_size,
        })
    }

    fn load_from_disk(filename: &str) -> Result<Vec<u8>, String> {
        // Simulate actual file loading
        match filename {
            name if name.ends_with(".jpg") || name.ends_with(".jpeg") => {
                Ok(vec![0xFF, 0xD8, 0xFF, 0xE0]) // JPEG header simulation
            }
            name if name.ends_with(".png") => {
                Ok(vec![0x89, 0x50, 0x4E, 0x47]) // PNG header simulation
            }
            name if name.ends_with(".gif") => {
                Ok(vec![0x47, 0x49, 0x46, 0x38]) // GIF header simulation
            }
            _ => Err(format!("Unsupported file format: {}", filename)),
        }
    }

    fn parse_dimensions(data: &[u8]) -> (u32, u32) {
        // Simulate dimension parsing from image data
        let hash = data.iter().fold(0u32, |acc, &b| acc.wrapping_add(b as u32));
        let width = 800 + (hash % 400);
        let height = 600 + (hash % 300);
        (width, height)
    }

    fn detect_format(filename: &str) -> String {
        if filename.ends_with(".jpg") || filename.ends_with(".jpeg") {
            "JPEG".to_string()
        } else if filename.ends_with(".png") {
            "PNG".to_string()
        } else if filename.ends_with(".gif") {
            "GIF".to_string()
        } else {
            "Unknown".to_string()
        }
    }
}

impl Image for RealImage {
    fn display(&self) -> Result<String, String> {
        Ok(format!(
            "Displaying {} image: {} ({}x{}) - {} bytes",
            self.format, self.filename, self.width, self.height, self.data.len()
        ))
    }

    fn get_info(&self) -> ImageInfo {
        ImageInfo {
            filename: self.filename.clone(),
            format: self.format.clone(),
            file_size: self.file_size,
            loaded: true,
        }
    }

    fn get_size(&self) -> (u32, u32) {
        (self.width, self.height)
    }
}

/// Virtual proxy - lazy loading image proxy
pub struct ImageProxy {
    filename: String,
    real_image: Option<RealImage>,
}

impl ImageProxy {
    pub fn new(filename: &str) -> Self {
        ImageProxy {
            filename: filename.to_string(),
            real_image: None,
        }
    }

    fn load_real_image(&mut self) -> Result<&RealImage, String> {
        if self.real_image.is_none() {
            self.real_image = Some(RealImage::new(&self.filename)?);
        }
        Ok(self.real_image.as_ref().unwrap())
    }
}

impl Image for ImageProxy {
    fn display(&self) -> Result<String, String> {
        // This is a bit tricky with Rust's ownership - in a real implementation
        // you might use RefCell or other interior mutability patterns
        let mut proxy = ImageProxy::new(&self.filename);
        if let Some(ref real_image) = self.real_image {
            real_image.display()
        } else {
            proxy.load_real_image()?.display()
        }
    }

    fn get_info(&self) -> ImageInfo {
        ImageInfo {
            filename: self.filename.clone(),
            format: "Unknown".to_string(),
            file_size: 0,
            loaded: self.real_image.is_some(),
        }
    }

    fn get_size(&self) -> (u32, u32) {
        if let Some(ref real_image) = self.real_image {
            real_image.get_size()
        } else {
            (0, 0) // Unknown until loaded
        }
    }
}

/// Protection proxy - access control
pub trait WebService {
    fn get_data(&self, user_id: &str, resource: &str) -> Result<String, String>;
    fn post_data(&self, user_id: &str, resource: &str, data: &str) -> Result<String, String>;
    fn delete_data(&self, user_id: &str, resource: &str) -> Result<String, String>;
}

pub struct RealWebService {
    name: String,
}

impl RealWebService {
    pub fn new(name: &str) -> Self {
        RealWebService {
            name: name.to_string(),
        }
    }
}

impl WebService for RealWebService {
    fn get_data(&self, _user_id: &str, resource: &str) -> Result<String, String> {
        Ok(format!("Data from {}: {}", self.name, resource))
    }

    fn post_data(&self, _user_id: &str, resource: &str, data: &str) -> Result<String, String> {
        Ok(format!("Posted to {} {}: {}", self.name, resource, data))
    }

    fn delete_data(&self, _user_id: &str, resource: &str) -> Result<String, String> {
        Ok(format!("Deleted from {} {}", self.name, resource))
    }
}

#[derive(Debug, Clone, PartialEq)]
pub enum Permission {
    Read,
    Write,
    Delete,
    Admin,
}

pub struct ProtectionProxy {
    real_service: RealWebService,
    user_permissions: HashMap<String, Vec<Permission>>,
    access_log: RefCell<Vec<AccessLogEntry>>,
}

#[derive(Debug, Clone)]
pub struct AccessLogEntry {
    user_id: String,
    action: String,
    resource: String,
    timestamp: SystemTime,
    success: bool,
}

impl ProtectionProxy {
    pub fn new(service_name: &str) -> Self {
        let mut user_permissions = HashMap::new();

        // Set up some default permissions
        user_permissions.insert(
            "admin".to_string(),
            vec![Permission::Read, Permission::Write, Permission::Delete, Permission::Admin],
        );
        user_permissions.insert(
            "user".to_string(),
            vec![Permission::Read, Permission::Write],
        );
        user_permissions.insert(
            "guest".to_string(),
            vec![Permission::Read],
        );

        ProtectionProxy {
            real_service: RealWebService::new(service_name),
            user_permissions,
            access_log: RefCell::new(Vec::new()),
        }
    }

    fn has_permission(&self, user_id: &str, required_permission: &Permission) -> bool {
        self.user_permissions
            .get(user_id)
            .map(|perms| perms.contains(required_permission))
            .unwrap_or(false)
    }

    fn log_access(&self, user_id: &str, action: &str, resource: &str, success: bool) {
        let entry = AccessLogEntry {
            user_id: user_id.to_string(),
            action: action.to_string(),
            resource: resource.to_string(),
            timestamp: SystemTime::now(),
            success,
        };
        self.access_log.borrow_mut().push(entry);
    }

    pub fn add_user_permission(&mut self, user_id: &str, permission: Permission) {
        self.user_permissions
            .entry(user_id.to_string())
            .or_insert_with(Vec::new)
            .push(permission);
    }

    pub fn get_access_log(&self) -> Vec<AccessLogEntry> {
        self.access_log.borrow().clone()
    }

    pub fn get_user_permissions(&self, user_id: &str) -> Option<&Vec<Permission>> {
        self.user_permissions.get(user_id)
    }
}

impl WebService for ProtectionProxy {
    fn get_data(&self, user_id: &str, resource: &str) -> Result<String, String> {
        if self.has_permission(user_id, &Permission::Read) {
            let result = self.real_service.get_data(user_id, resource);
            self.log_access(user_id, "GET", resource, result.is_ok());
            result
        } else {
            self.log_access(user_id, "GET", resource, false);
            Err(format!("Access denied: {} lacks READ permission", user_id))
        }
    }

    fn post_data(&self, user_id: &str, resource: &str, data: &str) -> Result<String, String> {
        if self.has_permission(user_id, &Permission::Write) {
            let result = self.real_service.post_data(user_id, resource, data);
            self.log_access(user_id, "POST", resource, result.is_ok());
            result
        } else {
            self.log_access(user_id, "POST", resource, false);
            Err(format!("Access denied: {} lacks WRITE permission", user_id))
        }
    }

    fn delete_data(&self, user_id: &str, resource: &str) -> Result<String, String> {
        if self.has_permission(user_id, &Permission::Delete) {
            let result = self.real_service.delete_data(user_id, resource);
            self.log_access(user_id, "DELETE", resource, result.is_ok());
            result
        } else {
            self.log_access(user_id, "DELETE", resource, false);
            Err(format!("Access denied: {} lacks DELETE permission", user_id))
        }
    }
}

/// Caching proxy
pub trait DataService {
    fn fetch_data(&mut self, key: &str) -> Result<String, String>;
    fn is_cached(&self, key: &str) -> bool;
}

pub struct ExpensiveDataService {
    name: String,
    request_count: u32,
}

impl ExpensiveDataService {
    pub fn new(name: &str) -> Self {
        ExpensiveDataService {
            name: name.to_string(),
            request_count: 0,
        }
    }

    pub fn get_request_count(&self) -> u32 {
        self.request_count
    }
}

impl DataService for ExpensiveDataService {
    fn fetch_data(&mut self, key: &str) -> Result<String, String> {
        self.request_count += 1;

        // Simulate expensive operation
        std::thread::sleep(Duration::from_millis(100));

        Ok(format!("Expensive data for '{}' from {} (request #{})",
                  key, self.name, self.request_count))
    }

    fn is_cached(&self, _key: &str) -> bool {
        false // Real service doesn't cache
    }
}

#[derive(Debug, Clone)]
struct CacheEntry {
    data: String,
    timestamp: SystemTime,
    hit_count: u32,
}

pub struct CachingProxy {
    real_service: ExpensiveDataService,
    cache: HashMap<String, CacheEntry>,
    cache_ttl: Duration,
    max_cache_size: usize,
}

impl CachingProxy {
    pub fn new(service_name: &str, cache_ttl_secs: u64, max_cache_size: usize) -> Self {
        CachingProxy {
            real_service: ExpensiveDataService::new(service_name),
            cache: HashMap::new(),
            cache_ttl: Duration::from_secs(cache_ttl_secs),
            max_cache_size,
        }
    }

    fn is_cache_valid(&self, entry: &CacheEntry) -> bool {
        SystemTime::now()
            .duration_since(entry.timestamp)
            .map(|duration| duration < self.cache_ttl)
            .unwrap_or(false)
    }

    fn evict_oldest_entry(&mut self) {
        if self.cache.len() >= self.max_cache_size {
            // Find and remove the oldest entry
            let oldest_key = self.cache
                .iter()
                .min_by_key(|(_, entry)| entry.timestamp)
                .map(|(key, _)| key.clone());

            if let Some(key) = oldest_key {
                self.cache.remove(&key);
            }
        }
    }

    pub fn get_cache_stats(&self) -> CacheStats {
        let total_hits: u32 = self.cache.values().map(|entry| entry.hit_count).sum();
        let total_requests = self.real_service.get_request_count() + total_hits;

        CacheStats {
            cache_size: self.cache.len(),
            max_cache_size: self.max_cache_size,
            total_requests,
            cache_hits: total_hits,
            cache_misses: self.real_service.get_request_count(),
            hit_ratio: if total_requests > 0 {
                total_hits as f32 / total_requests as f32
            } else {
                0.0
            },
        }
    }

    pub fn clear_cache(&mut self) {
        self.cache.clear();
    }

    pub fn get_cached_keys(&self) -> Vec<String> {
        self.cache.keys().cloned().collect()
    }
}

#[derive(Debug)]
pub struct CacheStats {
    pub cache_size: usize,
    pub max_cache_size: usize,
    pub total_requests: u32,
    pub cache_hits: u32,
    pub cache_misses: u32,
    pub hit_ratio: f32,
}

impl DataService for CachingProxy {
    fn fetch_data(&mut self, key: &str) -> Result<String, String> {
        // Check if we have a cached entry
        let should_use_cache = if let Some(entry) = self.cache.get(key) {
            self.is_cache_valid(entry)
        } else {
            false
        };

        if should_use_cache {
            if let Some(entry) = self.cache.get_mut(key) {
                entry.hit_count += 1;
                return Ok(format!("{} [CACHED]", entry.data));
            }
        }

        // Remove expired entry if exists
        if self.cache.contains_key(key) && !should_use_cache {
            self.cache.remove(key);
        }

        // Fetch from real service
        let data = self.real_service.fetch_data(key)?;

        // Evict oldest entry if cache is full
        self.evict_oldest_entry();

        // Cache the result
        let cache_entry = CacheEntry {
            data: data.clone(),
            timestamp: SystemTime::now(),
            hit_count: 0,
        };
        self.cache.insert(key.to_string(), cache_entry);

        Ok(data)
    }

    fn is_cached(&self, key: &str) -> bool {
        self.cache.get(key)
            .map(|entry| self.is_cache_valid(entry))
            .unwrap_or(false)
    }
}

/// Smart proxy - adds intelligence and convenience methods
pub trait FileSystem {
    fn read_file(&mut self, path: &str) -> Result<String, String>;
    fn write_file(&mut self, path: &str, content: &str) -> Result<(), String>;
    fn delete_file(&mut self, path: &str) -> Result<(), String>;
    fn file_exists(&self, path: &str) -> bool;
}

pub struct SimpleFileSystem;

impl FileSystem for SimpleFileSystem {
    fn read_file(&mut self, path: &str) -> Result<String, String> {
        // Simulate file reading
        if path.starts_with("/") || path.starts_with("C:\\") {
            Ok(format!("Content of file: {}", path))
        } else {
            Err(format!("File not found: {}", path))
        }
    }

    fn write_file(&mut self, path: &str, content: &str) -> Result<(), String> {
        if content.len() > 1000000 {
            Err("File too large".to_string())
        } else {
            Ok(())
        }
    }

    fn delete_file(&mut self, path: &str) -> Result<(), String> {
        if path.ends_with(".sys") {
            Err("Cannot delete system files".to_string())
        } else {
            Ok(())
        }
    }

    fn file_exists(&self, path: &str) -> bool {
        path.starts_with("/") || path.starts_with("C:\\")
    }
}

pub struct SmartFileSystemProxy {
    real_fs: SimpleFileSystem,
    operation_log: Vec<FileOperation>,
    backup_enabled: bool,
}

#[derive(Debug, Clone)]
pub struct FileOperation {
    operation: String,
    path: String,
    timestamp: SystemTime,
    success: bool,
}

impl SmartFileSystemProxy {
    pub fn new(backup_enabled: bool) -> Self {
        SmartFileSystemProxy {
            real_fs: SimpleFileSystem,
            operation_log: Vec::new(),
            backup_enabled,
        }
    }

    pub fn batch_read(&mut self, paths: &[&str]) -> HashMap<String, Result<String, String>> {
        let mut results = HashMap::new();

        for &path in paths {
            let result = self.read_file(path);
            results.insert(path.to_string(), result);
        }

        results
    }

    pub fn copy_file(&mut self, source: &str, destination: &str) -> Result<(), String> {
        let content = self.read_file(source)?;
        self.write_file(destination, &content)?;

        self.log_operation("COPY", &format!("{} -> {}", source, destination), true);
        Ok(())
    }

    pub fn backup_file(&mut self, path: &str) -> Result<String, String> {
        if !self.backup_enabled {
            return Err("Backup not enabled".to_string());
        }

        let content = self.read_file(path)?;
        let backup_path = format!("{}.backup", path);
        self.write_file(&backup_path, &content)?;

        self.log_operation("BACKUP", path, true);
        Ok(backup_path)
    }

    pub fn get_file_stats(&self, path: &str) -> FileStats {
        let exists = self.file_exists(path);
        let operations: Vec<_> = self.operation_log
            .iter()
            .filter(|op| op.path == path || op.path.contains(path))
            .cloned()
            .collect();

        FileStats {
            path: path.to_string(),
            exists,
            operation_count: operations.len(),
            operations,
        }
    }

    pub fn get_operation_log(&self) -> &[FileOperation] {
        &self.operation_log
    }

    fn log_operation(&mut self, operation: &str, path: &str, success: bool) {
        let op = FileOperation {
            operation: operation.to_string(),
            path: path.to_string(),
            timestamp: SystemTime::now(),
            success,
        };
        self.operation_log.push(op);
    }
}

#[derive(Debug)]
pub struct FileStats {
    pub path: String,
    pub exists: bool,
    pub operation_count: usize,
    pub operations: Vec<FileOperation>,
}

impl FileSystem for SmartFileSystemProxy {
    fn read_file(&mut self, path: &str) -> Result<String, String> {
        let result = self.real_fs.read_file(path);
        self.log_operation("READ", path, result.is_ok());
        result
    }

    fn write_file(&mut self, path: &str, content: &str) -> Result<(), String> {
        // Auto-backup before writing if enabled
        if self.backup_enabled && self.file_exists(path) {
            let _ = self.backup_file(path);
        }

        let result = self.real_fs.write_file(path, content);
        self.log_operation("WRITE", path, result.is_ok());
        result
    }

    fn delete_file(&mut self, path: &str) -> Result<(), String> {
        // Auto-backup before deleting if enabled
        if self.backup_enabled && self.file_exists(path) {
            let _ = self.backup_file(path);
        }

        let result = self.real_fs.delete_file(path);
        self.log_operation("DELETE", path, result.is_ok());
        result
    }

    fn file_exists(&self, path: &str) -> bool {
        self.real_fs.file_exists(path)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_image_proxy_lazy_loading() {
        let proxy = ImageProxy::new("test.jpg");
        let info = proxy.get_info();

        assert!(!info.loaded);
        assert_eq!(info.filename, "test.jpg");
        assert_eq!(proxy.get_size(), (0, 0)); // Not loaded yet
    }

    #[test]
    fn test_protection_proxy() {
        let mut proxy = ProtectionProxy::new("test_service");

        // Guest can read
        assert!(proxy.get_data("guest", "data1").is_ok());

        // Guest cannot write
        assert!(proxy.post_data("guest", "data1", "content").is_err());

        // Admin can do everything
        assert!(proxy.get_data("admin", "data1").is_ok());
        assert!(proxy.post_data("admin", "data1", "content").is_ok());
        assert!(proxy.delete_data("admin", "data1").is_ok());

        // Check access log
        let log = proxy.get_access_log();
        assert!(log.len() >= 5);
    }

    #[test]
    fn test_caching_proxy() {
        let mut proxy = CachingProxy::new("test_service", 60, 10);

        // First call should hit the real service
        let result1 = proxy.fetch_data("key1").unwrap();
        assert!(!result1.contains("[CACHED]"));

        // Second call should be cached
        let result2 = proxy.fetch_data("key1").unwrap();
        assert!(result2.contains("[CACHED]"));

        let stats = proxy.get_cache_stats();
        assert_eq!(stats.cache_hits, 1);
        assert_eq!(stats.cache_misses, 1);
        assert_eq!(stats.hit_ratio, 0.5);
    }

    #[test]
    fn test_smart_file_system_proxy() {
        let mut proxy = SmartFileSystemProxy::new(true);

        // Test file operations
        assert!(proxy.write_file("/test.txt", "content").is_ok());
        assert!(proxy.read_file("/test.txt").is_ok());

        // Test batch operations
        let paths = ["/test.txt", "/other.txt"];
        let results = proxy.batch_read(&paths);
        assert_eq!(results.len(), 2);

        // Check operation log
        let log = proxy.get_operation_log();
        assert!(log.len() >= 3); // write, read, read operations

        // Test file stats
        let stats = proxy.get_file_stats("/test.txt");
        assert!(stats.operation_count > 0);
    }

    #[test]
    fn test_cache_eviction() {
        let mut proxy = CachingProxy::new("test_service", 60, 2); // Max 2 items

        // Fill cache
        proxy.fetch_data("key1").unwrap();
        proxy.fetch_data("key2").unwrap();

        assert_eq!(proxy.get_cache_stats().cache_size, 2);

        // This should evict the oldest entry
        proxy.fetch_data("key3").unwrap();
        assert_eq!(proxy.get_cache_stats().cache_size, 2);
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Proxy Pattern Demo ===");

    // Virtual Proxy - Lazy Loading
    println!("\n1. Virtual Proxy - Lazy Loading Images:");
    let proxy = ImageProxy::new("large_image.jpg");

    println!("Image proxy created for: {}", proxy.get_info().filename);
    println!("Image loaded: {}", proxy.get_info().loaded);

    // Image loads only when actually needed
    match proxy.display() {
        Ok(display_info) => println!("{}", display_info),
        Err(e) => println!("Error: {}", e),
    }

    // Protection Proxy - Access Control
    println!("\n2. Protection Proxy - Access Control:");
    let mut web_proxy = ProtectionProxy::new("SecureAPI");

    // Test different user permissions
    let users = ["guest", "user", "admin"];
    let operations = [
        ("GET", "users", ""),
        ("POST", "users", "new_user_data"),
        ("DELETE", "users/123", ""),
    ];

    for user in &users {
        println!("\nTesting permissions for: {}", user);
        for (method, resource, data) in &operations {
            let result = match *method {
                "GET" => web_proxy.get_data(user, resource),
                "POST" => web_proxy.post_data(user, resource, data),
                "DELETE" => web_proxy.delete_data(user, resource),
                _ => Ok("Unknown method".to_string()),
            };

            match result {
                Ok(response) => println!("  {} {}: {}", method, resource, response),
                Err(error) => println!("  {} {}: {}", method, resource, error),
            }
        }
    }

    // Show access log
    println!("\nAccess Log:");
    for entry in web_proxy.get_access_log().iter().take(5) {
        println!("  {} {} {} - Success: {}",
                entry.user_id, entry.action, entry.resource, entry.success);
    }

    // Caching Proxy
    println!("\n3. Caching Proxy - Performance Optimization:");
    let mut cache_proxy = CachingProxy::new("ExpensiveService", 300, 5);

    // Demonstrate caching benefits
    let keys = ["user:123", "product:456", "user:123", "order:789", "user:123"];

    for key in &keys {
        println!("\nFetching data for: {}", key);
        let start = SystemTime::now();

        match cache_proxy.fetch_data(key) {
            Ok(data) => {
                let duration = SystemTime::now().duration_since(start).unwrap();
                println!("  {}", data);
                println!("  Response time: {:?}", duration);
            }
            Err(e) => println!("  Error: {}", e),
        }
    }

    // Show cache statistics
    let stats = cache_proxy.get_cache_stats();
    println!("\nCache Statistics:");
    println!("  Cache size: {}/{}", stats.cache_size, stats.max_cache_size);
    println!("  Total requests: {}", stats.total_requests);
    println!("  Cache hits: {}", stats.cache_hits);
    println!("  Cache misses: {}", stats.cache_misses);
    println!("  Hit ratio: {:.1}%", stats.hit_ratio * 100.0);

    // Smart Proxy - Enhanced Functionality
    println!("\n4. Smart Proxy - Enhanced File System:");
    let mut smart_fs = SmartFileSystemProxy::new(true);

    // Demonstrate enhanced file operations
    println!("Writing files...");
    smart_fs.write_file("/documents/report.txt", "Quarterly report content").unwrap();
    smart_fs.write_file("/documents/notes.txt", "Meeting notes").unwrap();

    // Batch read operation
    println!("\nBatch reading files:");
    let paths = ["/documents/report.txt", "/documents/notes.txt", "/missing.txt"];
    let results = smart_fs.batch_read(&paths);

    for (path, result) in &results {
        match result {
            Ok(content) => println!("  {}: {}", path, content),
            Err(error) => println!("  {}: Error - {}", path, error),
        }
    }

    // File copy operation
    println!("\nCopying file:");
    match smart_fs.copy_file("/documents/report.txt", "/backup/report_copy.txt") {
        Ok(()) => println!("  File copied successfully"),
        Err(e) => println!("  Copy failed: {}", e),
    }

    // Show operation statistics
    println!("\nFile Operation Log:");
    for op in smart_fs.get_operation_log().iter().take(6) {
        println!("  {} {} - Success: {}", op.operation, op.path, op.success);
    }

    let stats = smart_fs.get_file_stats("/documents/report.txt");
    println!("\nFile stats for {}:", stats.path);
    println!("  Exists: {}", stats.exists);
    println!("  Operations performed: {}", stats.operation_count);
}

pub fn main() {
    demo();
}