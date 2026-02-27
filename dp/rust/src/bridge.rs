/// Bridge Pattern Implementation in Rust
///
/// The Bridge pattern separates an abstraction from its implementation,
/// allowing both to vary independently. In Rust, this is implemented
/// using traits for the implementation interface and structs for abstractions.

use std::collections::HashMap;

/// Implementation interface for drawing operations
pub trait DrawingAPI {
    fn draw_circle(&self, x: f64, y: f64, radius: f64) -> String;
    fn draw_rectangle(&self, x: f64, y: f64, width: f64, height: f64) -> String;
    fn draw_line(&self, x1: f64, y1: f64, x2: f64, y2: f64) -> String;
    fn get_api_name(&self) -> &str;
}

/// Concrete implementation for Canvas API
pub struct CanvasAPI {
    name: String,
    canvas_size: (u32, u32),
}

impl CanvasAPI {
    pub fn new(width: u32, height: u32) -> Self {
        Self {
            name: "Canvas API".to_string(),
            canvas_size: (width, height),
        }
    }
}

impl DrawingAPI for CanvasAPI {
    fn draw_circle(&self, x: f64, y: f64, radius: f64) -> String {
        format!(
            "Canvas: Drawing circle at ({:.1}, {:.1}) with radius {:.1}",
            x, y, radius
        )
    }

    fn draw_rectangle(&self, x: f64, y: f64, width: f64, height: f64) -> String {
        format!(
            "Canvas: Drawing rectangle at ({:.1}, {:.1}) with size {:.1}x{:.1}",
            x, y, width, height
        )
    }

    fn draw_line(&self, x1: f64, y1: f64, x2: f64, y2: f64) -> String {
        format!(
            "Canvas: Drawing line from ({:.1}, {:.1}) to ({:.1}, {:.1})",
            x1, y1, x2, y2
        )
    }

    fn get_api_name(&self) -> &str {
        &self.name
    }
}

/// Concrete implementation for SVG API
pub struct SvgAPI {
    name: String,
    precision: u32,
}

impl SvgAPI {
    pub fn new(precision: u32) -> Self {
        Self {
            name: "SVG API".to_string(),
            precision,
        }
    }
}

impl DrawingAPI for SvgAPI {
    fn draw_circle(&self, x: f64, y: f64, radius: f64) -> String {
        format!(
            "<circle cx=\"{:.prec$}\" cy=\"{:.prec$}\" r=\"{:.prec$}\" />",
            x,
            y,
            radius,
            prec = self.precision as usize
        )
    }

    fn draw_rectangle(&self, x: f64, y: f64, width: f64, height: f64) -> String {
        format!(
            "<rect x=\"{:.prec$}\" y=\"{:.prec$}\" width=\"{:.prec$}\" height=\"{:.prec$}\" />",
            x,
            y,
            width,
            height,
            prec = self.precision as usize
        )
    }

    fn draw_line(&self, x1: f64, y1: f64, x2: f64, y2: f64) -> String {
        format!(
            "<line x1=\"{:.prec$}\" y1=\"{:.prec$}\" x2=\"{:.prec$}\" y2=\"{:.prec$}\" />",
            x1,
            y1,
            x2,
            y2,
            prec = self.precision as usize
        )
    }

    fn get_api_name(&self) -> &str {
        &self.name
    }
}

/// Concrete implementation for OpenGL API
pub struct OpenGLAPI {
    name: String,
    version: String,
}

impl OpenGLAPI {
    pub fn new(version: &str) -> Self {
        Self {
            name: "OpenGL API".to_string(),
            version: version.to_string(),
        }
    }
}

impl DrawingAPI for OpenGLAPI {
    fn draw_circle(&self, x: f64, y: f64, radius: f64) -> String {
        format!(
            "OpenGL {}: glBegin(GL_TRIANGLE_FAN); glVertex2f({:.2}, {:.2}); /* circle with radius {:.2} */",
            self.version, x, y, radius
        )
    }

    fn draw_rectangle(&self, x: f64, y: f64, width: f64, height: f64) -> String {
        format!(
            "OpenGL {}: glBegin(GL_QUADS); glVertex2f({:.2}, {:.2}); /* {}x{} rectangle */",
            self.version, x, y, width, height
        )
    }

    fn draw_line(&self, x1: f64, y1: f64, x2: f64, y2: f64) -> String {
        format!(
            "OpenGL {}: glBegin(GL_LINES); glVertex2f({:.2}, {:.2}); glVertex2f({:.2}, {:.2});",
            self.version, x1, y1, x2, y2
        )
    }

    fn get_api_name(&self) -> &str {
        &self.name
    }
}

/// Abstraction - Base shape class
pub trait Shape {
    fn draw(&self) -> Vec<String>;
    fn resize(&mut self, factor: f64);
    fn move_to(&mut self, x: f64, y: f64);
    fn get_area(&self) -> f64;
    fn get_info(&self) -> String;
}

/// Refined Abstraction - Circle
pub struct Circle {
    x: f64,
    y: f64,
    radius: f64,
    drawing_api: Box<dyn DrawingAPI>,
}

impl Circle {
    pub fn new(x: f64, y: f64, radius: f64, drawing_api: Box<dyn DrawingAPI>) -> Self {
        Self {
            x,
            y,
            radius,
            drawing_api,
        }
    }
}

impl Shape for Circle {
    fn draw(&self) -> Vec<String> {
        vec![self.drawing_api.draw_circle(self.x, self.y, self.radius)]
    }

    fn resize(&mut self, factor: f64) {
        self.radius *= factor;
    }

    fn move_to(&mut self, x: f64, y: f64) {
        self.x = x;
        self.y = y;
    }

    fn get_area(&self) -> f64 {
        std::f64::consts::PI * self.radius * self.radius
    }

    fn get_info(&self) -> String {
        format!(
            "Circle at ({:.1}, {:.1}) with radius {:.1} using {}",
            self.x,
            self.y,
            self.radius,
            self.drawing_api.get_api_name()
        )
    }
}

/// Refined Abstraction - Rectangle
pub struct Rectangle {
    x: f64,
    y: f64,
    width: f64,
    height: f64,
    drawing_api: Box<dyn DrawingAPI>,
}

impl Rectangle {
    pub fn new(x: f64, y: f64, width: f64, height: f64, drawing_api: Box<dyn DrawingAPI>) -> Self {
        Self {
            x,
            y,
            width,
            height,
            drawing_api,
        }
    }
}

impl Shape for Rectangle {
    fn draw(&self) -> Vec<String> {
        vec![self
            .drawing_api
            .draw_rectangle(self.x, self.y, self.width, self.height)]
    }

    fn resize(&mut self, factor: f64) {
        self.width *= factor;
        self.height *= factor;
    }

    fn move_to(&mut self, x: f64, y: f64) {
        self.x = x;
        self.y = y;
    }

    fn get_area(&self) -> f64 {
        self.width * self.height
    }

    fn get_info(&self) -> String {
        format!(
            "Rectangle at ({:.1}, {:.1}) with size {:.1}x{:.1} using {}",
            self.x,
            self.y,
            self.width,
            self.height,
            self.drawing_api.get_api_name()
        )
    }
}

/// Complex shape that uses multiple drawing operations
pub struct ComplexShape {
    components: Vec<String>,
    drawing_api: Box<dyn DrawingAPI>,
    center_x: f64,
    center_y: f64,
    scale: f64,
}

impl ComplexShape {
    pub fn new(center_x: f64, center_y: f64, drawing_api: Box<dyn DrawingAPI>) -> Self {
        Self {
            components: Vec::new(),
            drawing_api,
            center_x,
            center_y,
            scale: 1.0,
        }
    }

    /// Draw a house-like shape
    pub fn draw_house(&mut self) -> Vec<String> {
        let mut commands = Vec::new();

        // Base of house (rectangle)
        commands.push(self.drawing_api.draw_rectangle(
            self.center_x - 50.0 * self.scale,
            self.center_y - 30.0 * self.scale,
            100.0 * self.scale,
            60.0 * self.scale,
        ));

        // Roof (triangle made with lines)
        commands.push(self.drawing_api.draw_line(
            self.center_x - 60.0 * self.scale,
            self.center_y - 30.0 * self.scale,
            self.center_x,
            self.center_y - 80.0 * self.scale,
        ));

        commands.push(self.drawing_api.draw_line(
            self.center_x,
            self.center_y - 80.0 * self.scale,
            self.center_x + 60.0 * self.scale,
            self.center_y - 30.0 * self.scale,
        ));

        // Door (rectangle)
        commands.push(self.drawing_api.draw_rectangle(
            self.center_x - 10.0 * self.scale,
            self.center_y + 5.0 * self.scale,
            20.0 * self.scale,
            25.0 * self.scale,
        ));

        // Window (circle)
        commands.push(self.drawing_api.draw_circle(
            self.center_x + 25.0 * self.scale,
            self.center_y - 10.0 * self.scale,
            8.0 * self.scale,
        ));

        self.components = commands.clone();
        commands
    }
}

impl Shape for ComplexShape {
    fn draw(&self) -> Vec<String> {
        self.components.clone()
    }

    fn resize(&mut self, factor: f64) {
        self.scale *= factor;
    }

    fn move_to(&mut self, x: f64, y: f64) {
        self.center_x = x;
        self.center_y = y;
    }

    fn get_area(&self) -> f64 {
        // Simplified area calculation for the house
        let base_area = 100.0 * 60.0 * self.scale * self.scale;
        let roof_area = 0.5 * 120.0 * 50.0 * self.scale * self.scale;
        base_area + roof_area
    }

    fn get_info(&self) -> String {
        format!(
            "Complex Shape (House) at ({:.1}, {:.1}) with scale {:.2} using {}",
            self.center_x,
            self.center_y,
            self.scale,
            self.drawing_api.get_api_name()
        )
    }
}

/// Notification Bridge Pattern Example
pub trait NotificationSender {
    fn send(&self, message: &str, recipient: &str) -> Result<String, String>;
    fn get_sender_type(&self) -> &str;
}

pub struct EmailSender {
    smtp_server: String,
}

impl EmailSender {
    pub fn new(smtp_server: &str) -> Self {
        Self {
            smtp_server: smtp_server.to_string(),
        }
    }
}

impl NotificationSender for EmailSender {
    fn send(&self, message: &str, recipient: &str) -> Result<String, String> {
        if !recipient.contains('@') {
            return Err("Invalid email address".to_string());
        }
        Ok(format!(
            "Email sent via {} to {}: {}",
            self.smtp_server, recipient, message
        ))
    }

    fn get_sender_type(&self) -> &str {
        "Email"
    }
}

pub struct SmsSender {
    provider: String,
}

impl SmsSender {
    pub fn new(provider: &str) -> Self {
        Self {
            provider: provider.to_string(),
        }
    }
}

impl NotificationSender for SmsSender {
    fn send(&self, message: &str, recipient: &str) -> Result<String, String> {
        if recipient.len() < 10 {
            return Err("Invalid phone number".to_string());
        }
        Ok(format!(
            "SMS sent via {} to {}: {}",
            self.provider, recipient, message
        ))
    }

    fn get_sender_type(&self) -> &str {
        "SMS"
    }
}

pub struct SlackSender {
    workspace: String,
}

impl SlackSender {
    pub fn new(workspace: &str) -> Self {
        Self {
            workspace: workspace.to_string(),
        }
    }
}

impl NotificationSender for SlackSender {
    fn send(&self, message: &str, recipient: &str) -> Result<String, String> {
        if !recipient.starts_with('#') && !recipient.starts_with('@') {
            return Err("Invalid Slack recipient format".to_string());
        }
        Ok(format!(
            "Slack message sent in workspace {} to {}: {}",
            self.workspace, recipient, message
        ))
    }

    fn get_sender_type(&self) -> &str {
        "Slack"
    }
}

/// Notification abstraction
pub struct Notification {
    sender: Box<dyn NotificationSender>,
    priority: NotificationPriority,
}

#[derive(Debug, Clone)]
pub enum NotificationPriority {
    Low,
    Normal,
    High,
    Critical,
}

impl Notification {
    pub fn new(sender: Box<dyn NotificationSender>, priority: NotificationPriority) -> Self {
        Self { sender, priority }
    }

    pub fn send(&self, message: &str, recipient: &str) -> Result<String, String> {
        let priority_prefix = match self.priority {
            NotificationPriority::Low => "[LOW]",
            NotificationPriority::Normal => "[INFO]",
            NotificationPriority::High => "[HIGH]",
            NotificationPriority::Critical => "[CRITICAL]",
        };

        let formatted_message = format!("{} {}", priority_prefix, message);
        self.sender.send(&formatted_message, recipient)
    }

    pub fn get_info(&self) -> String {
        format!("{} notification with {:?} priority", self.sender.get_sender_type(), self.priority)
    }
}

/// Urgent notification - extends the abstraction
pub struct UrgentNotification {
    sender: Box<dyn NotificationSender>,
    max_retries: u32,
}

impl UrgentNotification {
    pub fn new(sender: Box<dyn NotificationSender>, max_retries: u32) -> Self {
        Self { sender, max_retries }
    }

    pub fn send_with_retry(&self, message: &str, recipient: &str) -> Result<String, String> {
        let urgent_message = format!("[URGENT] {}", message);

        for attempt in 1..=self.max_retries {
            match self.sender.send(&urgent_message, recipient) {
                Ok(result) => return Ok(format!("Attempt {}: {}", attempt, result)),
                Err(e) if attempt == self.max_retries => return Err(format!("Failed after {} attempts: {}", self.max_retries, e)),
                Err(_) => continue,
            }
        }

        Err("Unexpected error".to_string())
    }

    pub fn get_info(&self) -> String {
        format!("Urgent {} notification with {} max retries", self.sender.get_sender_type(), self.max_retries)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_circle_with_different_apis() {
        let canvas_circle = Circle::new(10.0, 20.0, 5.0, Box::new(CanvasAPI::new(800, 600)));
        let svg_circle = Circle::new(10.0, 20.0, 5.0, Box::new(SvgAPI::new(2)));

        let canvas_output = canvas_circle.draw();
        let svg_output = svg_circle.draw();

        assert!(canvas_output[0].contains("Canvas"));
        assert!(svg_output[0].contains("circle"));
        assert_eq!(canvas_circle.get_area(), svg_circle.get_area());
    }

    #[test]
    fn test_shape_operations() {
        let mut rect = Rectangle::new(0.0, 0.0, 10.0, 20.0, Box::new(CanvasAPI::new(800, 600)));

        let initial_area = rect.get_area();
        rect.resize(2.0);
        assert_eq!(rect.get_area(), initial_area * 4.0);

        rect.move_to(50.0, 60.0);
        assert!(rect.get_info().contains("50.0, 60.0"));
    }

    #[test]
    fn test_notification_bridge() {
        let email_notification = Notification::new(
            Box::new(EmailSender::new("smtp.gmail.com")),
            NotificationPriority::High
        );

        let result = email_notification.send("Test message", "user@example.com").unwrap();
        assert!(result.contains("[HIGH]"));
        assert!(result.contains("smtp.gmail.com"));

        let sms_notification = Notification::new(
            Box::new(SmsSender::new("Twilio")),
            NotificationPriority::Critical
        );

        let result = sms_notification.send("Alert", "1234567890").unwrap();
        assert!(result.contains("[CRITICAL]"));
        assert!(result.contains("Twilio"));
    }

    #[test]
    fn test_urgent_notification() {
        let urgent = UrgentNotification::new(
            Box::new(SlackSender::new("dev-team")),
            3
        );

        let result = urgent.send_with_retry("System down!", "@channel").unwrap();
        assert!(result.contains("Attempt"));
        assert!(result.contains("[URGENT]"));
    }

    #[test]
    fn test_drawing_api_differences() {
        let canvas_api = CanvasAPI::new(800, 600);
        let svg_api = SvgAPI::new(3);
        let opengl_api = OpenGLAPI::new("4.0");

        let canvas_circle = canvas_api.draw_circle(10.0, 10.0, 5.0);
        let svg_circle = svg_api.draw_circle(10.0, 10.0, 5.0);
        let opengl_circle = opengl_api.draw_circle(10.0, 10.0, 5.0);

        assert!(canvas_circle.contains("Canvas"));
        assert!(svg_circle.contains("<circle"));
        assert!(opengl_circle.contains("OpenGL"));
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Bridge Pattern Demo ===");

    // Drawing API Bridge example
    println!("\n1. Drawing API Bridge:");

    let apis: Vec<Box<dyn DrawingAPI>> = vec![
        Box::new(CanvasAPI::new(800, 600)),
        Box::new(SvgAPI::new(2)),
        Box::new(OpenGLAPI::new("4.0")),
    ];

    for (i, api) in apis.into_iter().enumerate() {
        println!("\nUsing {}:", api.get_api_name());

        // Create shapes with different APIs
        let mut circle = Circle::new(100.0, 100.0, 50.0, api);
        println!("  {}", circle.get_info());

        let draw_commands = circle.draw();
        for command in draw_commands {
            println!("  Command: {}", command);
        }

        circle.resize(1.5);
        circle.move_to(150.0, 150.0);
        println!("  After resize and move: {}", circle.get_info());
        println!("  Area: {:.2}", circle.get_area());
    }

    // Complex shape example
    println!("\n2. Complex Shape Example:");
    let mut house = ComplexShape::new(200.0, 200.0, Box::new(SvgAPI::new(1)));

    println!("Drawing house:");
    let house_commands = house.draw_house();
    for (i, command) in house_commands.iter().enumerate() {
        println!("  {}: {}", i + 1, command);
    }

    house.resize(0.8);
    house.move_to(300.0, 250.0);
    println!("House after resize and move: {}", house.get_info());

    // Notification Bridge example
    println!("\n3. Notification System Bridge:");

    let senders: Vec<Box<dyn NotificationSender>> = vec![
        Box::new(EmailSender::new("smtp.company.com")),
        Box::new(SmsSender::new("Twilio")),
        Box::new(SlackSender::new("engineering")),
    ];

    let priorities = vec![
        NotificationPriority::Normal,
        NotificationPriority::High,
        NotificationPriority::Critical,
    ];

    for (sender, priority) in senders.into_iter().zip(priorities.into_iter()) {
        let notification = Notification::new(sender, priority);
        println!("\n{}", notification.get_info());

        let recipients = match notification.sender.get_sender_type() {
            "Email" => vec!["user@example.com", "admin@example.com"],
            "SMS" => vec!["1234567890", "0987654321"],
            "Slack" => vec!["@user", "#general"],
            _ => vec!["unknown"],
        };

        for recipient in recipients {
            match notification.send("Server maintenance scheduled", recipient) {
                Ok(result) => println!("  ✓ {}", result),
                Err(error) => println!("  ✗ {}", error),
            }
        }
    }

    // Urgent notification example
    println!("\n4. Urgent Notification Example:");
    let urgent_email = UrgentNotification::new(
        Box::new(EmailSender::new("emergency.smtp.com")),
        3
    );

    println!("{}", urgent_email.get_info());
    match urgent_email.send_with_retry("Critical system failure detected!", "ops@company.com") {
        Ok(result) => println!("✓ {}", result),
        Err(error) => println!("✗ {}", error),
    }

    // Cross-platform drawing example
    println!("\n5. Cross-Platform Drawing:");
    let shapes_with_apis: Vec<(Box<dyn Shape>, &str)> = vec![
        (
            Box::new(Rectangle::new(0.0, 0.0, 100.0, 50.0, Box::new(CanvasAPI::new(1024, 768)))),
            "Canvas Rectangle"
        ),
        (
            Box::new(Circle::new(50.0, 50.0, 25.0, Box::new(SvgAPI::new(3)))),
            "SVG Circle"
        ),
        (
            Box::new(Rectangle::new(0.0, 0.0, 80.0, 80.0, Box::new(OpenGLAPI::new("3.3")))),
            "OpenGL Rectangle"
        ),
    ];

    for (shape, description) in shapes_with_apis {
        println!("\n{}:", description);
        println!("  Info: {}", shape.get_info());
        println!("  Area: {:.2}", shape.get_area());

        let commands = shape.draw();
        for command in commands {
            println!("  Draw: {}", command);
        }
    }
}

fn main() {
    demo();
}