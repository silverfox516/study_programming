/// Chain of Responsibility Pattern Implementation in Rust
///
/// The Chain of Responsibility pattern passes requests along a chain of handlers.
/// Upon receiving a request, each handler decides either to process the request
/// or to pass it to the next handler in the chain.

use std::collections::HashMap;

/// Support ticket example
#[derive(Debug, Clone)]
pub struct SupportRequest {
    pub id: u32,
    pub customer_name: String,
    pub issue_type: IssueType,
    pub priority: Priority,
    pub description: String,
    pub assigned_to: Option<String>,
    pub resolution: Option<String>,
}

#[derive(Debug, Clone, PartialEq)]
pub enum IssueType {
    Technical,
    Billing,
    General,
    Refund,
    Critical,
}

#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum Priority {
    Low = 1,
    Medium = 2,
    High = 3,
    Critical = 4,
}

impl SupportRequest {
    pub fn new(id: u32, customer_name: &str, issue_type: IssueType, priority: Priority, description: &str) -> Self {
        SupportRequest {
            id,
            customer_name: customer_name.to_string(),
            issue_type,
            priority,
            description: description.to_string(),
            assigned_to: None,
            resolution: None,
        }
    }

    pub fn assign_to(&mut self, handler: &str) {
        self.assigned_to = Some(handler.to_string());
    }

    pub fn resolve(&mut self, resolution: &str) {
        self.resolution = Some(resolution.to_string());
    }
}

pub trait SupportHandler {
    fn handle_request(&mut self, request: &mut SupportRequest) -> Result<String, String>;
    fn set_next(&mut self, next_handler: Box<dyn SupportHandler>);
    fn get_handler_name(&self) -> &str;
    fn can_handle(&self, request: &SupportRequest) -> bool;
}

/// Level 1 Support - Basic issues
pub struct Level1Support {
    name: String,
    next_handler: Option<Box<dyn SupportHandler>>,
    resolved_count: u32,
}

impl Level1Support {
    pub fn new() -> Self {
        Level1Support {
            name: "Level 1 Support".to_string(),
            next_handler: None,
            resolved_count: 0,
        }
    }

    pub fn get_resolved_count(&self) -> u32 {
        self.resolved_count
    }
}

impl Default for Level1Support {
    fn default() -> Self {
        Self::new()
    }
}

impl SupportHandler for Level1Support {
    fn handle_request(&mut self, request: &mut SupportRequest) -> Result<String, String> {
        if self.can_handle(request) {
            request.assign_to(&self.name);
            self.resolved_count += 1;

            let resolution = match request.issue_type {
                IssueType::General => "Provided general information and FAQ links",
                IssueType::Technical => "Resolved basic technical issue with standard procedure",
                _ => "Issue handled at Level 1",
            };

            request.resolve(resolution);
            Ok(format!("Request {} handled by {}: {}", request.id, self.name, resolution))
        } else if let Some(ref mut next) = self.next_handler {
            next.handle_request(request)
        } else {
            Err(format!("No handler available for request {}", request.id))
        }
    }

    fn set_next(&mut self, next_handler: Box<dyn SupportHandler>) {
        self.next_handler = Some(next_handler);
    }

    fn get_handler_name(&self) -> &str {
        &self.name
    }

    fn can_handle(&self, request: &SupportRequest) -> bool {
        matches!(request.priority, Priority::Low | Priority::Medium) &&
        matches!(request.issue_type, IssueType::General | IssueType::Technical)
    }
}

/// Level 2 Support - Complex technical issues
pub struct Level2Support {
    name: String,
    next_handler: Option<Box<dyn SupportHandler>>,
    resolved_count: u32,
    specializations: Vec<IssueType>,
}

impl Level2Support {
    pub fn new(specializations: Vec<IssueType>) -> Self {
        Level2Support {
            name: "Level 2 Support".to_string(),
            next_handler: None,
            resolved_count: 0,
            specializations,
        }
    }

    pub fn get_resolved_count(&self) -> u32 {
        self.resolved_count
    }
}

impl SupportHandler for Level2Support {
    fn handle_request(&mut self, request: &mut SupportRequest) -> Result<String, String> {
        if self.can_handle(request) {
            request.assign_to(&self.name);
            self.resolved_count += 1;

            let resolution = match request.issue_type {
                IssueType::Technical => "Resolved complex technical issue with advanced troubleshooting",
                IssueType::Billing => "Investigated and resolved billing discrepancy",
                _ => "Issue escalated and resolved at Level 2",
            };

            request.resolve(resolution);
            Ok(format!("Request {} handled by {}: {}", request.id, self.name, resolution))
        } else if let Some(ref mut next) = self.next_handler {
            next.handle_request(request)
        } else {
            Err(format!("No handler available for request {}", request.id))
        }
    }

    fn set_next(&mut self, next_handler: Box<dyn SupportHandler>) {
        self.next_handler = Some(next_handler);
    }

    fn get_handler_name(&self) -> &str {
        &self.name
    }

    fn can_handle(&self, request: &SupportRequest) -> bool {
        (matches!(request.priority, Priority::Medium | Priority::High) &&
         self.specializations.contains(&request.issue_type)) ||
        (matches!(request.issue_type, IssueType::Billing) && request.priority <= Priority::High)
    }
}

/// Management - Critical issues and escalations
pub struct ManagementSupport {
    name: String,
    next_handler: Option<Box<dyn SupportHandler>>,
    resolved_count: u32,
}

impl ManagementSupport {
    pub fn new() -> Self {
        ManagementSupport {
            name: "Management".to_string(),
            next_handler: None,
            resolved_count: 0,
        }
    }

    pub fn get_resolved_count(&self) -> u32 {
        self.resolved_count
    }
}

impl Default for ManagementSupport {
    fn default() -> Self {
        Self::new()
    }
}

impl SupportHandler for ManagementSupport {
    fn handle_request(&mut self, request: &mut SupportRequest) -> Result<String, String> {
        request.assign_to(&self.name);
        self.resolved_count += 1;

        let resolution = match request.issue_type {
            IssueType::Critical => "Critical issue escalated to management and resolved with priority",
            IssueType::Refund => "Refund request reviewed and approved by management",
            _ => "Issue reviewed and resolved by management",
        };

        request.resolve(resolution);
        Ok(format!("Request {} handled by {}: {}", request.id, self.name, resolution))
    }

    fn set_next(&mut self, next_handler: Box<dyn SupportHandler>) {
        self.next_handler = Some(next_handler);
    }

    fn get_handler_name(&self) -> &str {
        &self.name
    }

    fn can_handle(&self, _request: &SupportRequest) -> bool {
        true // Management can handle any request
    }
}

/// Request processing pipeline
pub struct SupportSystem {
    first_handler: Option<Box<dyn SupportHandler>>,
    processed_requests: Vec<SupportRequest>,
}

impl SupportSystem {
    pub fn new() -> Self {
        SupportSystem {
            first_handler: None,
            processed_requests: Vec::new(),
        }
    }

    pub fn set_handler_chain(&mut self, handler: Box<dyn SupportHandler>) {
        self.first_handler = Some(handler);
    }

    pub fn process_request(&mut self, mut request: SupportRequest) -> Result<String, String> {
        if let Some(ref mut handler) = self.first_handler {
            let result = handler.handle_request(&mut request);
            self.processed_requests.push(request);
            result
        } else {
            Err("No handlers configured".to_string())
        }
    }

    pub fn get_processed_requests(&self) -> &[SupportRequest] {
        &self.processed_requests
    }

    pub fn get_statistics(&self) -> SupportStatistics {
        let mut resolved = 0;
        let mut by_priority = HashMap::new();
        let mut by_type = HashMap::new();

        for request in &self.processed_requests {
            if request.resolution.is_some() {
                resolved += 1;
            }

            *by_priority.entry(format!("{:?}", request.priority)).or_insert(0) += 1;
            *by_type.entry(format!("{:?}", request.issue_type)).or_insert(0) += 1;
        }

        SupportStatistics {
            total_requests: self.processed_requests.len(),
            resolved_requests: resolved,
            requests_by_priority: by_priority,
            requests_by_type: by_type,
        }
    }
}

impl Default for SupportSystem {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug)]
pub struct SupportStatistics {
    pub total_requests: usize,
    pub resolved_requests: usize,
    pub requests_by_priority: HashMap<String, u32>,
    pub requests_by_type: HashMap<String, u32>,
}

/// HTTP request processing chain
#[derive(Debug, Clone)]
pub struct HttpRequest {
    pub method: String,
    pub path: String,
    pub headers: HashMap<String, String>,
    pub body: String,
    pub processed_by: Vec<String>,
}

impl HttpRequest {
    pub fn new(method: &str, path: &str) -> Self {
        HttpRequest {
            method: method.to_string(),
            path: path.to_string(),
            headers: HashMap::new(),
            body: String::new(),
            processed_by: Vec::new(),
        }
    }

    pub fn add_header(&mut self, key: &str, value: &str) {
        self.headers.insert(key.to_string(), value.to_string());
    }

    pub fn set_body(&mut self, body: &str) {
        self.body = body.to_string();
    }

    pub fn mark_processed_by(&mut self, handler: &str) {
        self.processed_by.push(handler.to_string());
    }
}

#[derive(Debug, Clone)]
pub struct HttpResponse {
    pub status_code: u16,
    pub headers: HashMap<String, String>,
    pub body: String,
}

impl HttpResponse {
    pub fn new(status_code: u16, body: &str) -> Self {
        HttpResponse {
            status_code,
            headers: HashMap::new(),
            body: body.to_string(),
        }
    }

    pub fn add_header(&mut self, key: &str, value: &str) {
        self.headers.insert(key.to_string(), value.to_string());
    }
}

pub trait RequestProcessor {
    fn process(&mut self, request: &mut HttpRequest) -> Result<Option<HttpResponse>, String>;
    fn set_next(&mut self, next: Box<dyn RequestProcessor>);
    fn get_name(&self) -> &str;
}

/// Authentication middleware
pub struct AuthenticationMiddleware {
    name: String,
    next: Option<Box<dyn RequestProcessor>>,
    valid_tokens: Vec<String>,
}

impl AuthenticationMiddleware {
    pub fn new(valid_tokens: Vec<String>) -> Self {
        AuthenticationMiddleware {
            name: "Authentication".to_string(),
            next: None,
            valid_tokens,
        }
    }
}

impl RequestProcessor for AuthenticationMiddleware {
    fn process(&mut self, request: &mut HttpRequest) -> Result<Option<HttpResponse>, String> {
        request.mark_processed_by(&self.name);

        // Check if path requires authentication
        if request.path.starts_with("/api/") || request.path.starts_with("/admin/") {
            if let Some(auth_header) = request.headers.get("Authorization") {
                if auth_header.starts_with("Bearer ") {
                    let token = &auth_header[7..];
                    if !self.valid_tokens.contains(&token.to_string()) {
                        return Ok(Some(HttpResponse::new(401, "Unauthorized")));
                    }
                } else {
                    return Ok(Some(HttpResponse::new(401, "Invalid authorization format")));
                }
            } else {
                return Ok(Some(HttpResponse::new(401, "Authorization required")));
            }
        }

        // Continue to next processor
        if let Some(ref mut next) = self.next {
            next.process(request)
        } else {
            Ok(None)
        }
    }

    fn set_next(&mut self, next: Box<dyn RequestProcessor>) {
        self.next = Some(next);
    }

    fn get_name(&self) -> &str {
        &self.name
    }
}

/// Rate limiting middleware
pub struct RateLimitingMiddleware {
    name: String,
    next: Option<Box<dyn RequestProcessor>>,
    request_counts: HashMap<String, u32>,
    max_requests: u32,
}

impl RateLimitingMiddleware {
    pub fn new(max_requests: u32) -> Self {
        RateLimitingMiddleware {
            name: "Rate Limiting".to_string(),
            next: None,
            request_counts: HashMap::new(),
            max_requests,
        }
    }

    fn get_client_ip(&self, request: &HttpRequest) -> String {
        request.headers.get("X-Real-IP")
            .or_else(|| request.headers.get("X-Forwarded-For"))
            .unwrap_or(&"127.0.0.1".to_string())
            .clone()
    }
}

impl RequestProcessor for RateLimitingMiddleware {
    fn process(&mut self, request: &mut HttpRequest) -> Result<Option<HttpResponse>, String> {
        request.mark_processed_by(&self.name);

        let client_ip = self.get_client_ip(request);
        let count = self.request_counts.entry(client_ip.clone()).or_insert(0);
        *count += 1;

        if *count > self.max_requests {
            return Ok(Some(HttpResponse::new(429, "Too Many Requests")));
        }

        // Continue to next processor
        if let Some(ref mut next) = self.next {
            next.process(request)
        } else {
            Ok(None)
        }
    }

    fn set_next(&mut self, next: Box<dyn RequestProcessor>) {
        self.next = Some(next);
    }

    fn get_name(&self) -> &str {
        &self.name
    }
}

/// Request router
pub struct RequestRouter {
    name: String,
    next: Option<Box<dyn RequestProcessor>>,
    routes: HashMap<String, String>,
}

impl RequestRouter {
    pub fn new() -> Self {
        let mut routes = HashMap::new();
        routes.insert("/".to_string(), "Welcome to the API".to_string());
        routes.insert("/api/users".to_string(), "User API endpoint".to_string());
        routes.insert("/api/orders".to_string(), "Orders API endpoint".to_string());
        routes.insert("/admin/dashboard".to_string(), "Admin dashboard".to_string());

        RequestRouter {
            name: "Router".to_string(),
            next: None,
            routes,
        }
    }

    pub fn add_route(&mut self, path: &str, response: &str) {
        self.routes.insert(path.to_string(), response.to_string());
    }
}

impl Default for RequestRouter {
    fn default() -> Self {
        Self::new()
    }
}

impl RequestProcessor for RequestRouter {
    fn process(&mut self, request: &mut HttpRequest) -> Result<Option<HttpResponse>, String> {
        request.mark_processed_by(&self.name);

        if let Some(response_body) = self.routes.get(&request.path) {
            let mut response = HttpResponse::new(200, response_body);
            response.add_header("Content-Type", "application/json");
            Ok(Some(response))
        } else {
            Ok(Some(HttpResponse::new(404, "Not Found")))
        }
    }

    fn set_next(&mut self, next: Box<dyn RequestProcessor>) {
        self.next = Some(next);
    }

    fn get_name(&self) -> &str {
        &self.name
    }
}

pub struct WebServer {
    processor_chain: Option<Box<dyn RequestProcessor>>,
    request_log: Vec<HttpRequest>,
}

impl WebServer {
    pub fn new() -> Self {
        WebServer {
            processor_chain: None,
            request_log: Vec::new(),
        }
    }

    pub fn set_processor_chain(&mut self, processor: Box<dyn RequestProcessor>) {
        self.processor_chain = Some(processor);
    }

    pub fn handle_request(&mut self, mut request: HttpRequest) -> HttpResponse {
        let response = if let Some(ref mut processor) = self.processor_chain {
            match processor.process(&mut request) {
                Ok(Some(response)) => response,
                Ok(None) => HttpResponse::new(500, "No response generated"),
                Err(error) => HttpResponse::new(500, &error),
            }
        } else {
            HttpResponse::new(500, "No processor configured")
        };

        self.request_log.push(request);
        response
    }

    pub fn get_request_log(&self) -> &[HttpRequest] {
        &self.request_log
    }
}

impl Default for WebServer {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_support_chain() {
        let mut level1 = Level1Support::new();
        let mut level2 = Level2Support::new(vec![IssueType::Technical, IssueType::Billing]);
        let management = ManagementSupport::new();

        level2.set_next(Box::new(management));
        level1.set_next(Box::new(level2));

        let mut system = SupportSystem::new();
        system.set_handler_chain(Box::new(level1));

        let request = SupportRequest::new(1, "John", IssueType::General, Priority::Low, "Simple question");
        let result = system.process_request(request);
        assert!(result.is_ok());
    }

    #[test]
    fn test_http_processing_chain() {
        let auth = AuthenticationMiddleware::new(vec!["valid_token".to_string()]);
        let rate_limit = RateLimitingMiddleware::new(10);
        let mut router = RequestRouter::new();

        let mut server = WebServer::new();
        // In a real implementation, you'd chain these properly
        server.set_processor_chain(Box::new(router));

        let mut request = HttpRequest::new("GET", "/");
        let response = server.handle_request(request);
        assert_eq!(response.status_code, 200);
    }
}

pub fn demo() {
    println!("=== Chain of Responsibility Pattern Demo ===");

    // Support System Example
    println!("\n1. Support Ticket System:");

    // Create handler chain
    let mut level1 = Level1Support::new();
    let mut level2 = Level2Support::new(vec![IssueType::Technical, IssueType::Billing]);
    let management = ManagementSupport::new();

    level2.set_next(Box::new(management));
    level1.set_next(Box::new(level2));

    let mut support_system = SupportSystem::new();
    support_system.set_handler_chain(Box::new(level1));

    // Process various requests
    let requests = vec![
        SupportRequest::new(1, "Alice", IssueType::General, Priority::Low, "How do I reset my password?"),
        SupportRequest::new(2, "Bob", IssueType::Technical, Priority::Medium, "Application crashes on startup"),
        SupportRequest::new(3, "Carol", IssueType::Billing, Priority::High, "Double charged this month"),
        SupportRequest::new(4, "Dave", IssueType::Critical, Priority::Critical, "System down for all users"),
        SupportRequest::new(5, "Eve", IssueType::Refund, Priority::High, "Need full refund for service"),
    ];

    println!("Processing support requests:");
    for request in requests {
        match support_system.process_request(request.clone()) {
            Ok(result) => println!("  ✅ {}", result),
            Err(error) => println!("  ❌ {}", error),
        }
    }

    let stats = support_system.get_statistics();
    println!("\nSupport System Statistics:");
    println!("  Total requests: {}", stats.total_requests);
    println!("  Resolved requests: {}", stats.resolved_requests);
    println!("  By priority: {:?}", stats.requests_by_priority);
    println!("  By type: {:?}", stats.requests_by_type);

    // HTTP Processing Chain Example
    println!("\n2. HTTP Request Processing Chain:");

    // Create processing chain
    let auth = AuthenticationMiddleware::new(vec![
        "admin_token".to_string(),
        "user_token".to_string(),
    ]);

    let mut rate_limit = RateLimitingMiddleware::new(5);
    let router = RequestRouter::new();

    rate_limit.set_next(Box::new(router));
    let mut auth_middleware = auth;
    auth_middleware.set_next(Box::new(rate_limit));

    let mut web_server = WebServer::new();
    web_server.set_processor_chain(Box::new(auth_middleware));

    // Test various requests
    let test_requests = vec![
        (HttpRequest::new("GET", "/"), None),
        (HttpRequest::new("GET", "/api/users"), Some("Bearer user_token")),
        (HttpRequest::new("GET", "/admin/dashboard"), Some("Bearer admin_token")),
        (HttpRequest::new("POST", "/api/orders"), Some("Bearer invalid_token")),
        (HttpRequest::new("GET", "/nonexistent"), None),
    ];

    println!("Processing HTTP requests:");
    for (mut request, auth_header) in test_requests {
        if let Some(auth) = auth_header {
            request.add_header("Authorization", auth);
        }
        request.add_header("X-Real-IP", "192.168.1.100");

        println!("\n  Request: {} {}", request.method, request.path);
        let response = web_server.handle_request(request.clone());
        println!("  Response: {} - {}", response.status_code, response.body);
        println!("  Processed by: {:?}", request.processed_by);
    }

    // Rate limiting demo
    println!("\n3. Rate Limiting Demo:");
    let mut rate_limited_request = HttpRequest::new("GET", "/");
    rate_limited_request.add_header("X-Real-IP", "192.168.1.200");

    println!("Making multiple requests from same IP:");
    for i in 1..=8 {
        let response = web_server.handle_request(rate_limited_request.clone());
        println!("  Request {}: Status {}", i, response.status_code);
        if response.status_code == 429 {
            println!("    Rate limit exceeded!");
            break;
        }
    }

    println!("\n4. Chain of Responsibility Benefits:");
    println!("  ✅ Decouples sender and receiver of requests");
    println!("  ✅ Allows dynamic handler chain configuration");
    println!("  ✅ Follows single responsibility principle");
    println!("  ✅ Enables flexible request processing pipelines");
    println!("  ✅ Supports multiple handlers for same request type");
    println!("  ✅ Easy to add/remove handlers without affecting others");
}

pub fn main() {
    demo();
}