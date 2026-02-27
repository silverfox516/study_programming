/// Abstract Factory Pattern Implementation in Rust
///
/// The Abstract Factory pattern provides an interface for creating families of
/// related objects without specifying their concrete classes. In Rust, we use
/// traits and trait objects to implement this pattern.

use std::fmt::Display;

/// Abstract products - UI components
pub trait Button {
    fn render(&self) -> String;
    fn on_click(&self) -> String;
}

pub trait Checkbox {
    fn render(&self) -> String;
    fn toggle(&self) -> String;
}

pub trait TextInput {
    fn render(&self) -> String;
    fn get_value(&self) -> String;
}

/// Windows-specific implementations
pub struct WindowsButton {
    text: String,
}

impl WindowsButton {
    pub fn new(text: &str) -> Self {
        Self {
            text: text.to_string(),
        }
    }
}

impl Button for WindowsButton {
    fn render(&self) -> String {
        format!("[Windows Button: {}]", self.text)
    }

    fn on_click(&self) -> String {
        format!("Windows button '{}' clicked with mouse", self.text)
    }
}

pub struct WindowsCheckbox {
    label: String,
    checked: bool,
}

impl WindowsCheckbox {
    pub fn new(label: &str) -> Self {
        Self {
            label: label.to_string(),
            checked: false,
        }
    }
}

impl Checkbox for WindowsCheckbox {
    fn render(&self) -> String {
        let symbol = if self.checked { "☑" } else { "☐" };
        format!("{} {}", symbol, self.label)
    }

    fn toggle(&self) -> String {
        format!("Windows checkbox '{}' toggled", self.label)
    }
}

pub struct WindowsTextInput {
    placeholder: String,
    value: String,
}

impl WindowsTextInput {
    pub fn new(placeholder: &str) -> Self {
        Self {
            placeholder: placeholder.to_string(),
            value: String::new(),
        }
    }
}

impl TextInput for WindowsTextInput {
    fn render(&self) -> String {
        format!("[Windows Input: {}]", self.placeholder)
    }

    fn get_value(&self) -> String {
        self.value.clone()
    }
}

/// macOS-specific implementations
pub struct MacButton {
    text: String,
}

impl MacButton {
    pub fn new(text: &str) -> Self {
        Self {
            text: text.to_string(),
        }
    }
}

impl Button for MacButton {
    fn render(&self) -> String {
        format!("( {} )", self.text)
    }

    fn on_click(&self) -> String {
        format!("macOS button '{}' clicked with trackpad", self.text)
    }
}

pub struct MacCheckbox {
    label: String,
    checked: bool,
}

impl MacCheckbox {
    pub fn new(label: &str) -> Self {
        Self {
            label: label.to_string(),
            checked: false,
        }
    }
}

impl Checkbox for MacCheckbox {
    fn render(&self) -> String {
        let symbol = if self.checked { "✓" } else { "○" };
        format!("{} {}", symbol, self.label)
    }

    fn toggle(&self) -> String {
        format!("macOS checkbox '{}' toggled", self.label)
    }
}

pub struct MacTextInput {
    placeholder: String,
    value: String,
}

impl MacTextInput {
    pub fn new(placeholder: &str) -> Self {
        Self {
            placeholder: placeholder.to_string(),
            value: String::new(),
        }
    }
}

impl TextInput for MacTextInput {
    fn render(&self) -> String {
        format!("⌐{}⌐", self.placeholder)
    }

    fn get_value(&self) -> String {
        self.value.clone()
    }
}

/// Linux-specific implementations
pub struct LinuxButton {
    text: String,
}

impl LinuxButton {
    pub fn new(text: &str) -> Self {
        Self {
            text: text.to_string(),
        }
    }
}

impl Button for LinuxButton {
    fn render(&self) -> String {
        format!("< {} >", self.text)
    }

    fn on_click(&self) -> String {
        format!("Linux button '{}' clicked", self.text)
    }
}

pub struct LinuxCheckbox {
    label: String,
    checked: bool,
}

impl LinuxCheckbox {
    pub fn new(label: &str) -> Self {
        Self {
            label: label.to_string(),
            checked: false,
        }
    }
}

impl Checkbox for LinuxCheckbox {
    fn render(&self) -> String {
        let symbol = if self.checked { "[x]" } else { "[ ]" };
        format!("{} {}", symbol, self.label)
    }

    fn toggle(&self) -> String {
        format!("Linux checkbox '{}' toggled", self.label)
    }
}

pub struct LinuxTextInput {
    placeholder: String,
    value: String,
}

impl LinuxTextInput {
    pub fn new(placeholder: &str) -> Self {
        Self {
            placeholder: placeholder.to_string(),
            value: String::new(),
        }
    }
}

impl TextInput for LinuxTextInput {
    fn render(&self) -> String {
        format!("|{}|", self.placeholder)
    }

    fn get_value(&self) -> String {
        self.value.clone()
    }
}

/// Abstract Factory trait
pub trait UIFactory {
    fn create_button(&self, text: &str) -> Box<dyn Button>;
    fn create_checkbox(&self, label: &str) -> Box<dyn Checkbox>;
    fn create_text_input(&self, placeholder: &str) -> Box<dyn TextInput>;
}

/// Concrete factories
pub struct WindowsFactory;

impl UIFactory for WindowsFactory {
    fn create_button(&self, text: &str) -> Box<dyn Button> {
        Box::new(WindowsButton::new(text))
    }

    fn create_checkbox(&self, label: &str) -> Box<dyn Checkbox> {
        Box::new(WindowsCheckbox::new(label))
    }

    fn create_text_input(&self, placeholder: &str) -> Box<dyn TextInput> {
        Box::new(WindowsTextInput::new(placeholder))
    }
}

pub struct MacFactory;

impl UIFactory for MacFactory {
    fn create_button(&self, text: &str) -> Box<dyn Button> {
        Box::new(MacButton::new(text))
    }

    fn create_checkbox(&self, label: &str) -> Box<dyn Checkbox> {
        Box::new(MacCheckbox::new(label))
    }

    fn create_text_input(&self, placeholder: &str) -> Box<dyn TextInput> {
        Box::new(MacTextInput::new(placeholder))
    }
}

pub struct LinuxFactory;

impl UIFactory for LinuxFactory {
    fn create_button(&self, text: &str) -> Box<dyn Button> {
        Box::new(LinuxButton::new(text))
    }

    fn create_checkbox(&self, label: &str) -> Box<dyn Checkbox> {
        Box::new(LinuxCheckbox::new(label))
    }

    fn create_text_input(&self, placeholder: &str) -> Box<dyn TextInput> {
        Box::new(LinuxTextInput::new(placeholder))
    }
}

/// Operating system detection for factory selection
#[derive(Debug, Clone)]
pub enum OperatingSystem {
    Windows,
    MacOS,
    Linux,
    Unknown,
}

impl Display for OperatingSystem {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            OperatingSystem::Windows => write!(f, "Windows"),
            OperatingSystem::MacOS => write!(f, "macOS"),
            OperatingSystem::Linux => write!(f, "Linux"),
            OperatingSystem::Unknown => write!(f, "Unknown"),
        }
    }
}

/// Factory provider that returns appropriate factory based on OS
pub struct UIFactoryProvider;

impl UIFactoryProvider {
    pub fn get_factory(os: OperatingSystem) -> Result<Box<dyn UIFactory>, String> {
        match os {
            OperatingSystem::Windows => Ok(Box::new(WindowsFactory)),
            OperatingSystem::MacOS => Ok(Box::new(MacFactory)),
            OperatingSystem::Linux => Ok(Box::new(LinuxFactory)),
            OperatingSystem::Unknown => Err("Unsupported operating system".to_string()),
        }
    }

    /// Detect current operating system (simplified)
    pub fn detect_os() -> OperatingSystem {
        if cfg!(target_os = "windows") {
            OperatingSystem::Windows
        } else if cfg!(target_os = "macos") {
            OperatingSystem::MacOS
        } else if cfg!(target_os = "linux") {
            OperatingSystem::Linux
        } else {
            OperatingSystem::Unknown
        }
    }
}

/// Application that uses the abstract factory
pub struct Application {
    factory: Box<dyn UIFactory>,
}

impl Application {
    pub fn new(factory: Box<dyn UIFactory>) -> Self {
        Self { factory }
    }

    pub fn create_login_form(&self) -> LoginForm {
        let username_input = self.factory.create_text_input("Username");
        let password_input = self.factory.create_text_input("Password");
        let remember_me = self.factory.create_checkbox("Remember me");
        let login_button = self.factory.create_button("Login");
        let cancel_button = self.factory.create_button("Cancel");

        LoginForm {
            username_input,
            password_input,
            remember_me,
            login_button,
            cancel_button,
        }
    }
}

pub struct LoginForm {
    username_input: Box<dyn TextInput>,
    password_input: Box<dyn TextInput>,
    remember_me: Box<dyn Checkbox>,
    login_button: Box<dyn Button>,
    cancel_button: Box<dyn Button>,
}

impl LoginForm {
    pub fn render(&self) -> String {
        format!(
            "Login Form:\n{}\n{}\n{}\n{} {}",
            self.username_input.render(),
            self.password_input.render(),
            self.remember_me.render(),
            self.login_button.render(),
            self.cancel_button.render()
        )
    }

    pub fn handle_login_click(&self) -> String {
        self.login_button.on_click()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_windows_factory() {
        let factory = WindowsFactory;
        let button = factory.create_button("OK");
        assert!(button.render().contains("Windows Button"));
        assert!(button.on_click().contains("mouse"));
    }

    #[test]
    fn test_mac_factory() {
        let factory = MacFactory;
        let checkbox = factory.create_checkbox("Enable notifications");
        assert!(checkbox.render().contains("○")); // Unchecked
        assert!(checkbox.toggle().contains("macOS"));
    }

    #[test]
    fn test_linux_factory() {
        let factory = LinuxFactory;
        let input = factory.create_text_input("Enter text");
        assert!(input.render().contains("|"));
    }

    #[test]
    fn test_factory_provider() {
        let factory = UIFactoryProvider::get_factory(OperatingSystem::Windows).unwrap();
        let button = factory.create_button("Test");
        assert!(button.render().contains("Windows"));

        let result = UIFactoryProvider::get_factory(OperatingSystem::Unknown);
        assert!(result.is_err());
    }

    #[test]
    fn test_application_with_different_factories() {
        let windows_app = Application::new(Box::new(WindowsFactory));
        let form = windows_app.create_login_form();
        let rendered = form.render();
        assert!(rendered.contains("Windows"));

        let mac_app = Application::new(Box::new(MacFactory));
        let form = mac_app.create_login_form();
        let rendered = form.render();
        assert!(rendered.contains("⌐"));
    }

    #[test]
    fn test_os_detection() {
        let os = UIFactoryProvider::detect_os();
        // This will vary based on the system running the tests
        assert!(matches!(
            os,
            OperatingSystem::Windows
                | OperatingSystem::MacOS
                | OperatingSystem::Linux
                | OperatingSystem::Unknown
        ));
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Abstract Factory Pattern Demo ===");

    // Detect current OS and create appropriate factory
    let os = UIFactoryProvider::detect_os();
    println!("Detected OS: {}", os);

    let factory = match UIFactoryProvider::get_factory(os.clone()) {
        Ok(f) => f,
        Err(e) => {
            println!("Error: {}. Using Windows factory as default.", e);
            Box::new(WindowsFactory)
        }
    };

    // Create application with the factory
    let app = Application::new(factory);
    let login_form = app.create_login_form();

    println!("\nRendered login form:");
    println!("{}", login_form.render());

    println!("\nInteraction example:");
    println!("{}", login_form.handle_login_click());

    // Demonstrate different factories
    println!("\n=== Comparing Different OS Implementations ===");

    let factories: Vec<(OperatingSystem, Box<dyn UIFactory>)> = vec![
        (OperatingSystem::Windows, Box::new(WindowsFactory)),
        (OperatingSystem::MacOS, Box::new(MacFactory)),
        (OperatingSystem::Linux, Box::new(LinuxFactory)),
    ];

    for (os, factory) in factories {
        println!("\n{} style:", os);
        let button = factory.create_button("Submit");
        let checkbox = factory.create_checkbox("Agree to terms");
        let input = factory.create_text_input("Email address");

        println!("  {}", button.render());
        println!("  {}", checkbox.render());
        println!("  {}", input.render());
    }
}

fn main() {
    println!("=== Abstract Factory Pattern Demo ===");

    // Demo UI Factory
    println!("\n1. UI Factory:");
    let windows_factory = WindowsFactory;
    let windows_button = windows_factory.create_button("Submit");
    let windows_checkbox = windows_factory.create_checkbox("Agree to terms");
    let windows_input = windows_factory.create_text_input("Email address");

    println!("Windows components:");
    println!("  {}", windows_button.render());
    println!("  {}", windows_checkbox.render());
    println!("  {}", windows_input.render());

    let mac_factory = MacFactory;
    let mac_button = mac_factory.create_button("Submit");
    let mac_checkbox = mac_factory.create_checkbox("Agree to terms");
    let mac_input = mac_factory.create_text_input("Email address");

    println!("Mac components:");
    println!("  {}", mac_button.render());
    println!("  {}", mac_checkbox.render());
    println!("  {}", mac_input.render());

    // Demo Linux Factory
    println!("\n2. Linux Factory:");
    let linux_factory = LinuxFactory;
    let linux_button = linux_factory.create_button("Submit");
    let linux_checkbox = linux_factory.create_checkbox("Agree to terms");
    let linux_input = linux_factory.create_text_input("Email address");

    println!("Linux components:");
    println!("  {}", linux_button.render());
    println!("  {}", linux_checkbox.render());
    println!("  {}", linux_input.render());

    // Demo UI Factory Provider
    println!("\n3. UI Factory Provider:");
    let current_os = UIFactoryProvider::detect_os();
    println!("Detected OS: {:?}", current_os);

    if let Ok(factory) = UIFactoryProvider::get_factory(current_os) {
        let button = factory.create_button("Dynamic Button");
        let checkbox = factory.create_checkbox("Dynamic Checkbox");
        let input = factory.create_text_input("Dynamic Input");

        println!("OS-specific components:");
        println!("  {}", button.render());
        println!("  {}", checkbox.render());
        println!("  {}", input.render());
    }

    // Demo Application
    println!("\n4. Application Demo:");
    if let Ok(factory) = UIFactoryProvider::get_factory(OperatingSystem::Windows) {
        let app = Application::new(factory);
        let login_form = app.create_login_form();
        println!("Login form rendered: {}", login_form.render());
    }
}