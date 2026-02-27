/// Observer Pattern Implementation in Rust
///
/// The Observer pattern defines a one-to-many dependency between objects
/// so that when one object changes state, all its dependents are notified
/// and updated automatically. This promotes loose coupling between subjects and observers.

use std::collections::HashMap;
use std::rc::{Rc, Weak};
use std::cell::RefCell;

/// Weather data subject that notifies observers of changes
#[derive(Debug, Clone)]
pub struct WeatherData {
    temperature: f32,
    humidity: f32,
    pressure: f32,
    timestamp: u64,
}

impl WeatherData {
    pub fn new(temperature: f32, humidity: f32, pressure: f32) -> Self {
        WeatherData {
            temperature,
            humidity,
            pressure,
            timestamp: 0,
        }
    }

    pub fn temperature(&self) -> f32 {
        self.temperature
    }

    pub fn humidity(&self) -> f32 {
        self.humidity
    }

    pub fn pressure(&self) -> f32 {
        self.pressure
    }

    pub fn timestamp(&self) -> u64 {
        self.timestamp
    }
}

/// Observer trait for weather updates
pub trait WeatherObserver {
    fn update(&mut self, weather_data: &WeatherData);
    fn get_id(&self) -> String;
}

/// Subject trait for managing observers
pub trait Subject<T> {
    fn attach(&mut self, observer: Rc<RefCell<dyn WeatherObserver>>);
    fn detach(&mut self, observer_id: &str) -> bool;
    fn notify(&self);
    fn get_observer_count(&self) -> usize;
}

/// Weather station that acts as the subject
pub struct WeatherStation {
    observers: Vec<Rc<RefCell<dyn WeatherObserver>>>,
    current_data: WeatherData,
    data_history: Vec<WeatherData>,
}

impl WeatherStation {
    pub fn new() -> Self {
        WeatherStation {
            observers: Vec::new(),
            current_data: WeatherData::new(0.0, 0.0, 0.0),
            data_history: Vec::new(),
        }
    }

    pub fn set_measurements(&mut self, temperature: f32, humidity: f32, pressure: f32) {
        // Store previous data in history
        if self.current_data.timestamp > 0 {
            self.data_history.push(self.current_data.clone());
        }

        // Update current data
        self.current_data = WeatherData {
            temperature,
            humidity,
            pressure,
            timestamp: self.data_history.len() as u64 + 1,
        };

        // Notify all observers
        self.notify();
    }

    pub fn get_current_data(&self) -> &WeatherData {
        &self.current_data
    }

    pub fn get_history(&self) -> &[WeatherData] {
        &self.data_history
    }

    pub fn simulate_weather_changes(&mut self) {
        let changes = [
            (22.5, 65.0, 1013.25),
            (24.0, 70.0, 1012.0),
            (21.0, 80.0, 1008.5),
            (25.5, 60.0, 1015.0),
            (20.0, 85.0, 1005.0),
        ];

        for (temp, humidity, pressure) in &changes {
            println!("Weather update: {}°C, {}% humidity, {} hPa", temp, humidity, pressure);
            self.set_measurements(*temp, *humidity, *pressure);
            println!();
        }
    }
}

impl Default for WeatherStation {
    fn default() -> Self {
        Self::new()
    }
}

impl Subject<WeatherData> for WeatherStation {
    fn attach(&mut self, observer: Rc<RefCell<dyn WeatherObserver>>) {
        self.observers.push(observer);
    }

    fn detach(&mut self, observer_id: &str) -> bool {
        let initial_len = self.observers.len();
        self.observers.retain(|observer| {
            observer.borrow().get_id() != observer_id
        });
        self.observers.len() < initial_len
    }

    fn notify(&self) {
        for observer in &self.observers {
            observer.borrow_mut().update(&self.current_data);
        }
    }

    fn get_observer_count(&self) -> usize {
        self.observers.len()
    }
}

/// Current conditions display
pub struct CurrentConditionsDisplay {
    id: String,
    temperature: f32,
    humidity: f32,
    display_count: u32,
}

impl CurrentConditionsDisplay {
    pub fn new(id: &str) -> Self {
        CurrentConditionsDisplay {
            id: id.to_string(),
            temperature: 0.0,
            humidity: 0.0,
            display_count: 0,
        }
    }

    pub fn display(&self) -> String {
        format!(
            "[{}] Current conditions: {:.1}°C and {:.1}% humidity (Update #{})",
            self.id, self.temperature, self.humidity, self.display_count
        )
    }

    pub fn get_display_count(&self) -> u32 {
        self.display_count
    }
}

impl WeatherObserver for CurrentConditionsDisplay {
    fn update(&mut self, weather_data: &WeatherData) {
        self.temperature = weather_data.temperature();
        self.humidity = weather_data.humidity();
        self.display_count += 1;
        println!("{}", self.display());
    }

    fn get_id(&self) -> String {
        self.id.clone()
    }
}

/// Statistics display that tracks min, max, and average
pub struct StatisticsDisplay {
    id: String,
    temperatures: Vec<f32>,
    humidity_values: Vec<f32>,
    pressure_values: Vec<f32>,
}

impl StatisticsDisplay {
    pub fn new(id: &str) -> Self {
        StatisticsDisplay {
            id: id.to_string(),
            temperatures: Vec::new(),
            humidity_values: Vec::new(),
            pressure_values: Vec::new(),
        }
    }

    fn calculate_stats(values: &[f32]) -> (f32, f32, f32) {
        if values.is_empty() {
            return (0.0, 0.0, 0.0);
        }

        let min = values.iter().fold(f32::INFINITY, |a, &b| a.min(b));
        let max = values.iter().fold(f32::NEG_INFINITY, |a, &b| a.max(b));
        let avg = values.iter().sum::<f32>() / values.len() as f32;

        (min, max, avg)
    }

    pub fn display(&self) -> String {
        let (temp_min, temp_max, temp_avg) = Self::calculate_stats(&self.temperatures);
        let (humid_min, humid_max, humid_avg) = Self::calculate_stats(&self.humidity_values);
        let (press_min, press_max, press_avg) = Self::calculate_stats(&self.pressure_values);

        format!(
            "[{}] Statistics (based on {} readings):\n\
            Temperature: Min={:.1}°C, Max={:.1}°C, Avg={:.1}°C\n\
            Humidity: Min={:.1}%, Max={:.1}%, Avg={:.1}%\n\
            Pressure: Min={:.1} hPa, Max={:.1} hPa, Avg={:.1} hPa",
            self.id,
            self.temperatures.len(),
            temp_min, temp_max, temp_avg,
            humid_min, humid_max, humid_avg,
            press_min, press_max, press_avg
        )
    }

    pub fn get_reading_count(&self) -> usize {
        self.temperatures.len()
    }
}

impl WeatherObserver for StatisticsDisplay {
    fn update(&mut self, weather_data: &WeatherData) {
        self.temperatures.push(weather_data.temperature());
        self.humidity_values.push(weather_data.humidity());
        self.pressure_values.push(weather_data.pressure());
        println!("{}", self.display());
    }

    fn get_id(&self) -> String {
        self.id.clone()
    }
}

/// Forecast display that makes simple predictions
pub struct ForecastDisplay {
    id: String,
    current_pressure: f32,
    last_pressure: f32,
    forecast_count: u32,
}

impl ForecastDisplay {
    pub fn new(id: &str) -> Self {
        ForecastDisplay {
            id: id.to_string(),
            current_pressure: 0.0,
            last_pressure: 0.0,
            forecast_count: 0,
        }
    }

    fn generate_forecast(&self) -> String {
        let pressure_trend = self.current_pressure - self.last_pressure;

        if pressure_trend > 1.0 {
            "Improving weather on the way!".to_string()
        } else if pressure_trend < -1.0 {
            "Watch out for cooler, rainy weather".to_string()
        } else {
            "More of the same".to_string()
        }
    }

    pub fn display(&self) -> String {
        format!(
            "[{}] Forecast: {} (Pressure trend: {:.1} hPa) - Forecast #{}",
            self.id,
            self.generate_forecast(),
            self.current_pressure - self.last_pressure,
            self.forecast_count
        )
    }

    pub fn get_forecast_count(&self) -> u32 {
        self.forecast_count
    }
}

impl WeatherObserver for ForecastDisplay {
    fn update(&mut self, weather_data: &WeatherData) {
        self.last_pressure = self.current_pressure;
        self.current_pressure = weather_data.pressure();
        self.forecast_count += 1;
        println!("{}", self.display());
    }

    fn get_id(&self) -> String {
        self.id.clone()
    }
}

/// Event system example using observer pattern
#[derive(Debug, Clone)]
pub enum SystemEvent {
    UserLogin { user_id: String, timestamp: u64 },
    UserLogout { user_id: String, timestamp: u64 },
    FileAccessed { file_path: String, user_id: String, timestamp: u64 },
    SystemError { error_code: i32, message: String, timestamp: u64 },
    DataUpdated { table_name: String, record_id: String, timestamp: u64 },
}

impl SystemEvent {
    pub fn get_timestamp(&self) -> u64 {
        match self {
            SystemEvent::UserLogin { timestamp, .. } => *timestamp,
            SystemEvent::UserLogout { timestamp, .. } => *timestamp,
            SystemEvent::FileAccessed { timestamp, .. } => *timestamp,
            SystemEvent::SystemError { timestamp, .. } => *timestamp,
            SystemEvent::DataUpdated { timestamp, .. } => *timestamp,
        }
    }

    pub fn get_type(&self) -> &str {
        match self {
            SystemEvent::UserLogin { .. } => "UserLogin",
            SystemEvent::UserLogout { .. } => "UserLogout",
            SystemEvent::FileAccessed { .. } => "FileAccessed",
            SystemEvent::SystemError { .. } => "SystemError",
            SystemEvent::DataUpdated { .. } => "DataUpdated",
        }
    }
}

pub trait EventObserver {
    fn handle_event(&mut self, event: &SystemEvent);
    fn get_observer_name(&self) -> String;
    fn is_interested_in(&self, event: &SystemEvent) -> bool;
}

pub struct EventManager {
    observers: Vec<Rc<RefCell<dyn EventObserver>>>,
    event_history: Vec<SystemEvent>,
    event_counter: u64,
}

impl EventManager {
    pub fn new() -> Self {
        EventManager {
            observers: Vec::new(),
            event_history: Vec::new(),
            event_counter: 0,
        }
    }

    pub fn subscribe(&mut self, observer: Rc<RefCell<dyn EventObserver>>) {
        self.observers.push(observer);
    }

    pub fn unsubscribe(&mut self, observer_name: &str) -> bool {
        let initial_len = self.observers.len();
        self.observers.retain(|observer| {
            observer.borrow().get_observer_name() != observer_name
        });
        self.observers.len() < initial_len
    }

    pub fn publish_event(&mut self, mut event: SystemEvent) {
        // Set timestamp if not already set
        match &mut event {
            SystemEvent::UserLogin { timestamp, .. } |
            SystemEvent::UserLogout { timestamp, .. } |
            SystemEvent::FileAccessed { timestamp, .. } |
            SystemEvent::SystemError { timestamp, .. } |
            SystemEvent::DataUpdated { timestamp, .. } => {
                if *timestamp == 0 {
                    *timestamp = self.event_counter;
                }
            }
        }

        self.event_counter += 1;
        self.event_history.push(event.clone());

        // Notify interested observers
        for observer in &self.observers {
            let mut observer_borrowed = observer.borrow_mut();
            if observer_borrowed.is_interested_in(&event) {
                observer_borrowed.handle_event(&event);
            }
        }
    }

    pub fn get_event_count(&self) -> usize {
        self.event_history.len()
    }

    pub fn get_observer_count(&self) -> usize {
        self.observers.len()
    }

    pub fn get_recent_events(&self, count: usize) -> Vec<&SystemEvent> {
        self.event_history.iter().rev().take(count).collect()
    }
}

impl Default for EventManager {
    fn default() -> Self {
        Self::new()
    }
}

/// Security monitor that watches for security-related events
pub struct SecurityMonitor {
    name: String,
    failed_login_attempts: HashMap<String, u32>,
    alerts_generated: u32,
}

impl SecurityMonitor {
    pub fn new(name: &str) -> Self {
        SecurityMonitor {
            name: name.to_string(),
            failed_login_attempts: HashMap::new(),
            alerts_generated: 0,
        }
    }

    fn check_failed_logins(&mut self, user_id: &str) {
        let attempts = self.failed_login_attempts.entry(user_id.to_string()).or_insert(0);
        *attempts += 1;

        if *attempts >= 3 {
            self.alerts_generated += 1;
            println!("🚨 SECURITY ALERT: Multiple failed login attempts for user {}", user_id);
        }
    }

    pub fn get_alerts_count(&self) -> u32 {
        self.alerts_generated
    }
}

impl EventObserver for SecurityMonitor {
    fn handle_event(&mut self, event: &SystemEvent) {
        match event {
            SystemEvent::UserLogin { user_id, timestamp } => {
                // Reset failed attempts on successful login
                self.failed_login_attempts.remove(user_id);
                println!("[{}] User {} logged in at {}", self.name, user_id, timestamp);
            }
            SystemEvent::SystemError { error_code, message, timestamp } => {
                if *error_code == 401 {  // Unauthorized
                    // Extract user_id from error message (simplified)
                    if let Some(user_id) = message.split_whitespace().last() {
                        self.check_failed_logins(user_id);
                    }
                }
                println!("[{}] Security-relevant error {} at {}: {}",
                        self.name, error_code, timestamp, message);
            }
            _ => {} // Not interested in other events for security
        }
    }

    fn get_observer_name(&self) -> String {
        self.name.clone()
    }

    fn is_interested_in(&self, event: &SystemEvent) -> bool {
        matches!(event,
            SystemEvent::UserLogin { .. } |
            SystemEvent::UserLogout { .. } |
            SystemEvent::SystemError { .. }
        )
    }
}

/// Logger that records all system events
pub struct EventLogger {
    name: String,
    logged_events: Vec<String>,
    log_level: LogLevel,
}

#[derive(Debug, Clone, PartialEq)]
pub enum LogLevel {
    Debug,
    Info,
    Warning,
    Error,
}

impl EventLogger {
    pub fn new(name: &str, log_level: LogLevel) -> Self {
        EventLogger {
            name: name.to_string(),
            logged_events: Vec::new(),
            log_level,
        }
    }

    fn should_log(&self, event: &SystemEvent) -> bool {
        match (&self.log_level, event) {
            (LogLevel::Error, SystemEvent::SystemError { .. }) => true,
            (LogLevel::Warning, SystemEvent::SystemError { .. }) => true,
            (LogLevel::Info, _) => !matches!(event, SystemEvent::FileAccessed { .. }),
            (LogLevel::Debug, _) => true,
            _ => false,
        }
    }

    pub fn get_log_count(&self) -> usize {
        self.logged_events.len()
    }

    pub fn get_recent_logs(&self, count: usize) -> Vec<&String> {
        self.logged_events.iter().rev().take(count).collect()
    }
}

impl EventObserver for EventLogger {
    fn handle_event(&mut self, event: &SystemEvent) {
        if self.should_log(event) {
            let log_entry = format!("[{}] {} - {:?}", self.name, event.get_type(), event);
            self.logged_events.push(log_entry.clone());
            println!("{}", log_entry);
        }
    }

    fn get_observer_name(&self) -> String {
        self.name.clone()
    }

    fn is_interested_in(&self, event: &SystemEvent) -> bool {
        self.should_log(event)
    }
}

/// Performance monitor that tracks system metrics
pub struct PerformanceMonitor {
    name: String,
    file_access_count: u32,
    error_count: u32,
    user_sessions: HashMap<String, u64>, // user_id -> login_timestamp
}

impl PerformanceMonitor {
    pub fn new(name: &str) -> Self {
        PerformanceMonitor {
            name: name.to_string(),
            file_access_count: 0,
            error_count: 0,
            user_sessions: HashMap::new(),
        }
    }

    pub fn get_metrics(&self) -> PerformanceMetrics {
        PerformanceMetrics {
            file_access_count: self.file_access_count,
            error_count: self.error_count,
            active_sessions: self.user_sessions.len(),
            error_rate: if self.file_access_count > 0 {
                self.error_count as f32 / (self.file_access_count + self.error_count) as f32
            } else {
                0.0
            },
        }
    }
}

#[derive(Debug)]
pub struct PerformanceMetrics {
    pub file_access_count: u32,
    pub error_count: u32,
    pub active_sessions: usize,
    pub error_rate: f32,
}

impl EventObserver for PerformanceMonitor {
    fn handle_event(&mut self, event: &SystemEvent) {
        match event {
            SystemEvent::UserLogin { user_id, timestamp } => {
                self.user_sessions.insert(user_id.clone(), *timestamp);
                println!("[{}] User session started: {}", self.name, user_id);
            }
            SystemEvent::UserLogout { user_id, .. } => {
                self.user_sessions.remove(user_id);
                println!("[{}] User session ended: {}", self.name, user_id);
            }
            SystemEvent::FileAccessed { file_path, .. } => {
                self.file_access_count += 1;
                if self.file_access_count % 10 == 0 {
                    println!("[{}] File access milestone: {} accesses", self.name, self.file_access_count);
                }
            }
            SystemEvent::SystemError { .. } => {
                self.error_count += 1;
                println!("[{}] Error detected. Total errors: {}", self.name, self.error_count);
            }
            _ => {}
        }
    }

    fn get_observer_name(&self) -> String {
        self.name.clone()
    }

    fn is_interested_in(&self, _event: &SystemEvent) -> bool {
        true // Interested in all events for performance monitoring
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_weather_station_observer_pattern() {
        let mut station = WeatherStation::new();

        let current_display = Rc::new(RefCell::new(
            CurrentConditionsDisplay::new("Display1")
        ));
        let stats_display = Rc::new(RefCell::new(
            StatisticsDisplay::new("Stats1")
        ));

        station.attach(current_display.clone());
        station.attach(stats_display.clone());

        assert_eq!(station.get_observer_count(), 2);

        station.set_measurements(25.0, 60.0, 1013.0);

        // Check that observers were updated
        assert_eq!(current_display.borrow().get_display_count(), 1);
        assert_eq!(stats_display.borrow().get_reading_count(), 1);

        // Test detaching observer
        let removed = station.detach("Display1");
        assert!(removed);
        assert_eq!(station.get_observer_count(), 1);

        station.set_measurements(26.0, 65.0, 1014.0);
        assert_eq!(current_display.borrow().get_display_count(), 1); // Not updated
        assert_eq!(stats_display.borrow().get_reading_count(), 2); // Updated
    }

    #[test]
    fn test_event_manager_system() {
        let mut event_manager = EventManager::new();

        let security_monitor = Rc::new(RefCell::new(
            SecurityMonitor::new("SecurityMonitor")
        ));
        let logger = Rc::new(RefCell::new(
            EventLogger::new("Logger", LogLevel::Info)
        ));

        event_manager.subscribe(security_monitor.clone());
        event_manager.subscribe(logger.clone());

        assert_eq!(event_manager.get_observer_count(), 2);

        // Publish some events
        event_manager.publish_event(SystemEvent::UserLogin {
            user_id: "alice".to_string(),
            timestamp: 0,
        });

        event_manager.publish_event(SystemEvent::SystemError {
            error_code: 401,
            message: "Unauthorized access by bob".to_string(),
            timestamp: 0,
        });

        assert_eq!(event_manager.get_event_count(), 2);
        assert!(logger.borrow().get_log_count() > 0);
    }

    #[test]
    fn test_observer_filtering() {
        let mut event_manager = EventManager::new();

        let security_monitor = Rc::new(RefCell::new(
            SecurityMonitor::new("SecurityMonitor")
        ));

        event_manager.subscribe(security_monitor.clone());

        // Security monitor should be interested in login events but not file access
        let login_event = SystemEvent::UserLogin {
            user_id: "alice".to_string(),
            timestamp: 1,
        };

        let file_event = SystemEvent::FileAccessed {
            file_path: "/test.txt".to_string(),
            user_id: "alice".to_string(),
            timestamp: 2,
        };

        assert!(security_monitor.borrow().is_interested_in(&login_event));
        assert!(!security_monitor.borrow().is_interested_in(&file_event));
    }

    #[test]
    fn test_performance_monitor_metrics() {
        let mut monitor = PerformanceMonitor::new("PerfMonitor");

        // Simulate some events
        monitor.handle_event(&SystemEvent::UserLogin {
            user_id: "user1".to_string(),
            timestamp: 1,
        });

        monitor.handle_event(&SystemEvent::FileAccessed {
            file_path: "/file1.txt".to_string(),
            user_id: "user1".to_string(),
            timestamp: 2,
        });

        monitor.handle_event(&SystemEvent::SystemError {
            error_code: 500,
            message: "Database error".to_string(),
            timestamp: 3,
        });

        let metrics = monitor.get_metrics();
        assert_eq!(metrics.file_access_count, 1);
        assert_eq!(metrics.error_count, 1);
        assert_eq!(metrics.active_sessions, 1);
        assert!(metrics.error_rate > 0.0);
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Observer Pattern Demo ===");

    // Weather Station Example
    println!("\n1. Weather Station with Multiple Displays:");
    let mut weather_station = WeatherStation::new();

    // Create different types of displays
    let current_display = Rc::new(RefCell::new(
        CurrentConditionsDisplay::new("MainDisplay")
    ));

    let statistics_display = Rc::new(RefCell::new(
        StatisticsDisplay::new("StatsBoard")
    ));

    let forecast_display = Rc::new(RefCell::new(
        ForecastDisplay::new("ForecastScreen")
    ));

    // Register displays as observers
    weather_station.attach(current_display.clone());
    weather_station.attach(statistics_display.clone());
    weather_station.attach(forecast_display.clone());

    println!("Weather station setup with {} observers", weather_station.get_observer_count());

    // Simulate weather changes
    println!("\nSimulating weather changes:");
    weather_station.simulate_weather_changes();

    // Show final statistics
    println!("\nFinal Display Counts:");
    println!("Current Display Updates: {}", current_display.borrow().get_display_count());
    println!("Statistics Readings: {}", statistics_display.borrow().get_reading_count());
    println!("Forecasts Generated: {}", forecast_display.borrow().get_forecast_count());

    // Event System Example
    println!("\n2. Event Management System:");
    let mut event_manager = EventManager::new();

    // Create different types of observers
    let security_monitor = Rc::new(RefCell::new(
        SecurityMonitor::new("SecurityCenter")
    ));

    let system_logger = Rc::new(RefCell::new(
        EventLogger::new("SystemLogger", LogLevel::Info)
    ));

    let performance_monitor = Rc::new(RefCell::new(
        PerformanceMonitor::new("PerformanceTracker")
    ));

    // Subscribe observers
    event_manager.subscribe(security_monitor.clone());
    event_manager.subscribe(system_logger.clone());
    event_manager.subscribe(performance_monitor.clone());

    println!("Event system setup with {} observers", event_manager.get_observer_count());

    // Simulate system events
    println!("\nSimulating system events:");

    let events = [
        SystemEvent::UserLogin {
            user_id: "alice".to_string(),
            timestamp: 0,
        },
        SystemEvent::FileAccessed {
            file_path: "/documents/report.pdf".to_string(),
            user_id: "alice".to_string(),
            timestamp: 0,
        },
        SystemEvent::UserLogin {
            user_id: "bob".to_string(),
            timestamp: 0,
        },
        SystemEvent::SystemError {
            error_code: 401,
            message: "Unauthorized access attempt by charlie".to_string(),
            timestamp: 0,
        },
        SystemEvent::DataUpdated {
            table_name: "users".to_string(),
            record_id: "alice_123".to_string(),
            timestamp: 0,
        },
        SystemEvent::SystemError {
            error_code: 401,
            message: "Failed login attempt by charlie".to_string(),
            timestamp: 0,
        },
        SystemEvent::SystemError {
            error_code: 401,
            message: "Another failed login by charlie".to_string(),
            timestamp: 0,
        },
        SystemEvent::UserLogout {
            user_id: "alice".to_string(),
            timestamp: 0,
        },
    ];

    for event in &events {
        event_manager.publish_event(event.clone());
        println!("---");
    }

    // Show system statistics
    println!("\nSystem Statistics:");
    println!("Total events processed: {}", event_manager.get_event_count());
    println!("Security alerts generated: {}", security_monitor.borrow().get_alerts_count());
    println!("Log entries created: {}", system_logger.borrow().get_log_count());

    let perf_metrics = performance_monitor.borrow().get_metrics();
    println!("Performance Metrics:");
    println!("  File accesses: {}", perf_metrics.file_access_count);
    println!("  Errors: {}", perf_metrics.error_count);
    println!("  Active sessions: {}", perf_metrics.active_sessions);
    println!("  Error rate: {:.1}%", perf_metrics.error_rate * 100.0);

    // Demonstrate observer removal
    println!("\n3. Dynamic Observer Management:");
    let removed = event_manager.unsubscribe("SecurityCenter");
    println!("Security monitor removed: {}", removed);
    println!("Remaining observers: {}", event_manager.get_observer_count());

    // Publish one more event to show security monitor is no longer notified
    event_manager.publish_event(SystemEvent::UserLogin {
        user_id: "dave".to_string(),
        timestamp: 0,
    });
}

pub fn main() {
    demo();

    // Run async demo
    println!("\n=== Async Observer with Tokio (Modern Rust) ===");
    let rt = tokio::runtime::Runtime::new().unwrap();
    rt.block_on(async_demo());
}

// --- Async Implementation ---

use tokio::sync::broadcast;
use std::sync::Arc;

#[derive(Debug, Clone)]
struct AsyncEvent {
    id: u32,
    message: String,
}

struct AsyncEventHub {
    tx: broadcast::Sender<AsyncEvent>,
}

impl AsyncEventHub {
    fn new() -> Self {
        let (tx, _) = broadcast::channel(100);
        AsyncEventHub { tx }
    }

    fn publish(&self, event: AsyncEvent) {
        // Send returns Err if there are no receivers, which we can ignore or log
        let _ = self.tx.send(event);
    }

    // fn subscribe(&self) -> broadcast::Receiver<AsyncEvent> {
    //     self.tx.subscribe()
    // }
    // actually, just exposing tx.subscribe() via a method is cleaner
    fn subscribe(&self) -> broadcast::Receiver<AsyncEvent> {
        self.tx.subscribe()
    }
}

async fn async_demo() {
    println!("Starting Async Observer Demo...");
    let hub = Arc::new(AsyncEventHub::new());
    
    // Subscriber 1: Logger
    let hub1 = hub.clone();
    let mut rx1 = hub1.subscribe();
    tokio::spawn(async move {
        while let Ok(event) = rx1.recv().await {
            println!("[AsyncLogger] Received event #{}: {}", event.id, event.message);
        }
    });

    // Subscriber 2: Alert System (only cares about odd IDs)
    let hub2 = hub.clone();
    let mut rx2 = hub2.subscribe();
    tokio::spawn(async move {
        while let Ok(event) = rx2.recv().await {
            if event.id % 2 != 0 {
                println!("[AsyncAlert] 🚨 ODD ID ALERT! Event #{}", event.id);
            }
        }
    });

    // Publisher
    println!("Broadcasting events...");
    for i in 1..=5 {
        hub.publish(AsyncEvent {
            id: i,
            message: format!("Event payload {}", i),
        });
        tokio::time::sleep(tokio::time::Duration::from_millis(100)).await;
    }
    
    // Give some time for async tasks to process
    tokio::time::sleep(tokio::time::Duration::from_millis(100)).await;
    println!("Async Demo Finished.");
}