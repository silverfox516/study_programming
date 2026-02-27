/// Adapter Pattern Implementation in Rust
///
/// The Adapter pattern allows incompatible interfaces to work together.
/// In Rust, we implement this using traits and wrapper structs.

use std::collections::HashMap;

/// Target interface that our client expects
pub trait MediaPlayer {
    fn play(&self, filename: &str) -> Result<String, String>;
    fn get_supported_formats(&self) -> Vec<String>;
}

/// Legacy audio player that only supports MP3
pub struct Mp3Player;

impl Mp3Player {
    pub fn play_mp3(&self, filename: &str) -> String {
        if filename.ends_with(".mp3") {
            format!("Playing MP3 file: {}", filename)
        } else {
            format!("Error: {} is not an MP3 file", filename)
        }
    }
}

/// Advanced audio player for modern formats
pub trait AdvancedMediaPlayer {
    fn play_vlc(&self, filename: &str) -> String;
    fn play_mp4(&self, filename: &str) -> String;
    fn play_avi(&self, filename: &str) -> String;
}

pub struct VlcPlayer;

impl AdvancedMediaPlayer for VlcPlayer {
    fn play_vlc(&self, filename: &str) -> String {
        format!("Playing VLC file: {}", filename)
    }

    fn play_mp4(&self, filename: &str) -> String {
        format!("Playing MP4 file: {}", filename)
    }

    fn play_avi(&self, filename: &str) -> String {
        format!("Playing AVI file: {}", filename)
    }
}

pub struct Mp4Player;

impl AdvancedMediaPlayer for Mp4Player {
    fn play_vlc(&self, _filename: &str) -> String {
        "MP4 player doesn't support VLC format".to_string()
    }

    fn play_mp4(&self, filename: &str) -> String {
        format!("Playing MP4 file with specialized player: {}", filename)
    }

    fn play_avi(&self, _filename: &str) -> String {
        "MP4 player doesn't support AVI format".to_string()
    }
}

/// Adapter that makes AdvancedMediaPlayer compatible with MediaPlayer interface
pub struct MediaAdapter {
    advanced_player: Box<dyn AdvancedMediaPlayer>,
    supported_formats: Vec<String>,
}

impl MediaAdapter {
    pub fn new(audio_type: &str) -> Result<Self, String> {
        match audio_type.to_lowercase().as_str() {
            "vlc" | "avi" => Ok(MediaAdapter {
                advanced_player: Box::new(VlcPlayer),
                supported_formats: vec!["vlc".to_string(), "avi".to_string(), "mp4".to_string()],
            }),
            "mp4" => Ok(MediaAdapter {
                advanced_player: Box::new(Mp4Player),
                supported_formats: vec!["mp4".to_string()],
            }),
            _ => Err(format!("Unsupported audio format: {}", audio_type)),
        }
    }

    fn get_file_extension(filename: &str) -> Option<&str> {
        filename.split('.').last()
    }
}

impl MediaPlayer for MediaAdapter {
    fn play(&self, filename: &str) -> Result<String, String> {
        let extension = Self::get_file_extension(filename)
            .ok_or_else(|| "Invalid filename format".to_string())?;

        match extension.to_lowercase().as_str() {
            "vlc" => Ok(self.advanced_player.play_vlc(filename)),
            "mp4" => Ok(self.advanced_player.play_mp4(filename)),
            "avi" => Ok(self.advanced_player.play_avi(filename)),
            _ => Err(format!("Unsupported format: {}", extension)),
        }
    }

    fn get_supported_formats(&self) -> Vec<String> {
        self.supported_formats.clone()
    }
}

/// Universal audio player that uses adapter pattern
pub struct AudioPlayer {
    mp3_player: Mp3Player,
    adapter_cache: HashMap<String, MediaAdapter>,
}

impl AudioPlayer {
    pub fn new() -> Self {
        Self {
            mp3_player: Mp3Player,
            adapter_cache: HashMap::new(),
        }
    }

    fn get_file_extension(filename: &str) -> Option<&str> {
        filename.split('.').last()
    }
}

impl MediaPlayer for AudioPlayer {
    fn play(&self, filename: &str) -> Result<String, String> {
        let extension = Self::get_file_extension(filename)
            .ok_or_else(|| "Invalid filename format".to_string())?
            .to_lowercase();

        match extension.as_str() {
            "mp3" => {
                let result = self.mp3_player.play_mp3(filename);
                if result.contains("Error") {
                    Err(result)
                } else {
                    Ok(result)
                }
            }
            "vlc" | "mp4" | "avi" => {
                // Use adapter for advanced formats
                match MediaAdapter::new(&extension) {
                    Ok(adapter) => adapter.play(filename),
                    Err(e) => Err(e),
                }
            }
            _ => Err(format!("Unsupported audio format: {}", extension)),
        }
    }

    fn get_supported_formats(&self) -> Vec<String> {
        vec![
            "mp3".to_string(),
            "mp4".to_string(),
            "vlc".to_string(),
            "avi".to_string(),
        ]
    }
}

impl Default for AudioPlayer {
    fn default() -> Self {
        Self::new()
    }
}

/// Object Adapter Pattern - External API integration
pub trait WeatherService {
    fn get_temperature(&self, city: &str) -> Result<f64, String>;
    fn get_humidity(&self, city: &str) -> Result<f64, String>;
}

/// Third-party weather API with different interface
pub struct ThirdPartyWeatherAPI;

impl ThirdPartyWeatherAPI {
    pub fn fetch_weather_data(&self, location: &str) -> Result<WeatherData, String> {
        // Simulate API call
        if location.is_empty() {
            return Err("Location cannot be empty".to_string());
        }

        Ok(WeatherData {
            temperature_kelvin: 295.15, // 22°C in Kelvin
            humidity_percentage: 65.0,
            pressure_hpa: 1013.25,
            location: location.to_string(),
        })
    }
}

#[derive(Debug, Clone)]
pub struct WeatherData {
    pub temperature_kelvin: f64,
    pub humidity_percentage: f64,
    pub pressure_hpa: f64,
    pub location: String,
}

/// Adapter for third-party weather API
pub struct WeatherAdapter {
    third_party_api: ThirdPartyWeatherAPI,
}

impl WeatherAdapter {
    pub fn new() -> Self {
        Self {
            third_party_api: ThirdPartyWeatherAPI,
        }
    }

    fn kelvin_to_celsius(kelvin: f64) -> f64 {
        kelvin - 273.15
    }
}

impl WeatherService for WeatherAdapter {
    fn get_temperature(&self, city: &str) -> Result<f64, String> {
        let weather_data = self.third_party_api.fetch_weather_data(city)?;
        Ok(Self::kelvin_to_celsius(weather_data.temperature_kelvin))
    }

    fn get_humidity(&self, city: &str) -> Result<f64, String> {
        let weather_data = self.third_party_api.fetch_weather_data(city)?;
        Ok(weather_data.humidity_percentage)
    }
}

impl Default for WeatherAdapter {
    fn default() -> Self {
        Self::new()
    }
}

/// Class Adapter Pattern using composition and traits
pub trait DatabaseConnection {
    fn connect(&self) -> Result<String, String>;
    fn execute_query(&self, query: &str) -> Result<Vec<String>, String>;
    fn close(&self) -> Result<String, String>;
}

/// Legacy database with different interface
pub struct LegacyDatabase {
    pub host: String,
    pub port: u16,
    pub connected: bool,
}

impl LegacyDatabase {
    pub fn new(host: &str, port: u16) -> Self {
        Self {
            host: host.to_string(),
            port,
            connected: false,
        }
    }

    pub fn establish_connection(&mut self) -> bool {
        println!("Establishing legacy connection to {}:{}", self.host, self.port);
        self.connected = true;
        true
    }

    pub fn run_sql(&self, sql: &str) -> Option<Vec<String>> {
        if !self.connected {
            return None;
        }
        Some(vec![format!("Legacy result for: {}", sql)])
    }

    pub fn terminate_connection(&mut self) -> bool {
        println!("Terminating legacy connection");
        self.connected = false;
        true
    }
}

/// Adapter for legacy database
pub struct DatabaseAdapter {
    legacy_db: LegacyDatabase,
}

impl DatabaseAdapter {
    pub fn new(host: &str, port: u16) -> Self {
        Self {
            legacy_db: LegacyDatabase::new(host, port),
        }
    }
}

impl DatabaseConnection for DatabaseAdapter {
    fn connect(&self) -> Result<String, String> {
        // Note: In real implementation, we'd need interior mutability (RefCell/Mutex)
        // For demo purposes, we'll simulate the connection
        if self.legacy_db.host.is_empty() {
            Err("Invalid host".to_string())
        } else {
            Ok(format!("Connected to {}:{}", self.legacy_db.host, self.legacy_db.port))
        }
    }

    fn execute_query(&self, query: &str) -> Result<Vec<String>, String> {
        if query.is_empty() {
            return Err("Query cannot be empty".to_string());
        }

        // Simulate query execution
        Ok(vec![format!("Adapted result for: {}", query)])
    }

    fn close(&self) -> Result<String, String> {
        Ok("Legacy connection closed".to_string())
    }
}

/// Utility adapter for data format conversion
pub trait DataFormatter {
    fn format(&self, data: &str) -> String;
}

pub struct JsonFormatter;
impl DataFormatter for JsonFormatter {
    fn format(&self, data: &str) -> String {
        format!("{{\"data\": \"{}\"}}", data)
    }
}

pub struct XmlFormatter;
impl DataFormatter for XmlFormatter {
    fn format(&self, data: &str) -> String {
        format!("<data>{}</data>", data)
    }
}

/// Legacy CSV formatter with different interface
pub struct CsvFormatter;
impl CsvFormatter {
    pub fn to_csv(&self, fields: &[&str]) -> String {
        fields.join(",")
    }
}

/// Adapter for CSV formatter
pub struct CsvAdapter {
    csv_formatter: CsvFormatter,
}

impl CsvAdapter {
    pub fn new() -> Self {
        Self {
            csv_formatter: CsvFormatter,
        }
    }
}

impl DataFormatter for CsvAdapter {
    fn format(&self, data: &str) -> String {
        let fields: Vec<&str> = data.split_whitespace().collect();
        self.csv_formatter.to_csv(&fields)
    }
}

impl Default for CsvAdapter {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_media_adapter() {
        let vlc_adapter = MediaAdapter::new("vlc").unwrap();
        let result = vlc_adapter.play("test.vlc").unwrap();
        assert!(result.contains("Playing VLC file"));

        let mp4_adapter = MediaAdapter::new("mp4").unwrap();
        let result = mp4_adapter.play("test.mp4").unwrap();
        assert!(result.contains("Playing MP4 file"));
    }

    #[test]
    fn test_audio_player() {
        let player = AudioPlayer::new();

        // Test MP3 support
        let result = player.play("song.mp3").unwrap();
        assert!(result.contains("Playing MP3"));

        // Test adapted formats
        let result = player.play("video.mp4").unwrap();
        assert!(result.contains("Playing MP4"));

        // Test unsupported format
        let result = player.play("file.wav");
        assert!(result.is_err());
    }

    #[test]
    fn test_weather_adapter() {
        let weather_service = WeatherAdapter::new();

        let temp = weather_service.get_temperature("New York").unwrap();
        assert!((temp - 22.0).abs() < 1.0); // Should be around 22°C

        let humidity = weather_service.get_humidity("New York").unwrap();
        assert_eq!(humidity, 65.0);
    }

    #[test]
    fn test_database_adapter() {
        let db = DatabaseAdapter::new("localhost", 5432);

        let conn_result = db.connect().unwrap();
        assert!(conn_result.contains("Connected"));

        let query_result = db.execute_query("SELECT * FROM users").unwrap();
        assert!(!query_result.is_empty());

        let close_result = db.close().unwrap();
        assert!(close_result.contains("closed"));
    }

    #[test]
    fn test_data_format_adapters() {
        let json = JsonFormatter;
        let xml = XmlFormatter;
        let csv = CsvAdapter::new();

        let data = "hello world";

        assert_eq!(json.format(data), "{\"data\": \"hello world\"}");
        assert_eq!(xml.format(data), "<data>hello world</data>");
        assert_eq!(csv.format(data), "hello,world");
    }

    #[test]
    fn test_adapter_error_handling() {
        let result = MediaAdapter::new("unsupported");
        assert!(result.is_err());

        let weather = WeatherAdapter::new();
        let result = weather.get_temperature("");
        assert!(result.is_err());
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Adapter Pattern Demo ===");

    // Audio player adapter demo
    println!("\n1. Audio Player Adapter:");
    let audio_player = AudioPlayer::new();

    let files = ["song.mp3", "movie.mp4", "video.avi", "clip.vlc", "audio.wav"];

    for file in &files {
        match audio_player.play(file) {
            Ok(result) => println!("✓ {}", result),
            Err(error) => println!("✗ {}", error),
        }
    }

    println!("Supported formats: {:?}", audio_player.get_supported_formats());

    // Weather service adapter demo
    println!("\n2. Weather Service Adapter:");
    let weather_service = WeatherAdapter::new();

    match weather_service.get_temperature("New York") {
        Ok(temp) => println!("Temperature in New York: {:.1}°C", temp),
        Err(e) => println!("Error: {}", e),
    }

    match weather_service.get_humidity("London") {
        Ok(humidity) => println!("Humidity in London: {:.1}%", humidity),
        Err(e) => println!("Error: {}", e),
    }

    // Database adapter demo
    println!("\n3. Database Connection Adapter:");
    let db_adapter = DatabaseAdapter::new("localhost", 5432);

    match db_adapter.connect() {
        Ok(msg) => println!("✓ {}", msg),
        Err(e) => println!("✗ Connection failed: {}", e),
    }

    match db_adapter.execute_query("SELECT * FROM users WHERE active = true") {
        Ok(results) => {
            println!("Query results:");
            for result in results {
                println!("  - {}", result);
            }
        }
        Err(e) => println!("✗ Query failed: {}", e),
    }

    match db_adapter.close() {
        Ok(msg) => println!("✓ {}", msg),
        Err(e) => println!("✗ Close failed: {}", e),
    }

    // Data formatter adapter demo
    println!("\n4. Data Format Adapters:");
    let formatters: Vec<(&str, Box<dyn DataFormatter>)> = vec![
        ("JSON", Box::new(JsonFormatter)),
        ("XML", Box::new(XmlFormatter)),
        ("CSV", Box::new(CsvAdapter::new())),
    ];

    let sample_data = "user profile data";

    for (name, formatter) in formatters {
        let formatted = formatter.format(sample_data);
        println!("{}: {}", name, formatted);
    }

    // Advanced media adapter demo
    println!("\n5. Individual Media Adapters:");
    let adapters = vec![
        ("VLC", MediaAdapter::new("vlc")),
        ("MP4", MediaAdapter::new("mp4")),
        ("AVI", MediaAdapter::new("vlc")), // VLC supports AVI
    ];

    for (name, adapter_result) in adapters {
        match adapter_result {
            Ok(adapter) => {
                println!("{} adapter created successfully", name);
                println!("  Supported formats: {:?}", adapter.get_supported_formats());

                // Test with appropriate file
                let test_file = match name {
                    "VLC" => "test.vlc",
                    "MP4" => "test.mp4",
                    "AVI" => "test.avi",
                    _ => "test.unknown",
                };

                match adapter.play(test_file) {
                    Ok(result) => println!("  ✓ {}", result),
                    Err(e) => println!("  ✗ {}", e),
                }
            }
            Err(e) => println!("✗ Failed to create {} adapter: {}", name, e),
        }
    }
}

fn main() {
    println!("=== Adapter Pattern Demo ===");

    // Demo Media Adapter
    println!("\n1. Media Adapter:");
    match MediaAdapter::new("vlc") {
        Ok(media_adapter) => {
            match media_adapter.play("song.mp3") {
                Ok(result) => println!("✓ {}", result),
                Err(e) => println!("✗ Error: {}", e),
            }

            match media_adapter.play("movie.mp4") {
                Ok(result) => println!("✓ {}", result),
                Err(e) => println!("✗ Error: {}", e),
            }
        }
        Err(e) => println!("✗ Failed to create MediaAdapter: {}", e),
    }

    // Demo Weather Adapter
    println!("\n2. Weather Adapter:");
    let weather_adapter = WeatherAdapter::new();
    match weather_adapter.get_temperature("New York") {
        Ok(temp) => println!("✓ Temperature in New York: {}°C", temp),
        Err(e) => println!("✗ Error: {}", e),
    }

    // Demo Database Adapter
    println!("\n3. Database Adapter:");
    let db_adapter = DatabaseAdapter::new("localhost", 5432);
    match db_adapter.connect() {
        Ok(result) => println!("✓ Database connection: {}", result),
        Err(e) => println!("✗ Connection error: {}", e),
    }

    // Demo CSV Adapter
    println!("\n4. CSV Adapter:");
    let csv_adapter = CsvAdapter::new();
    let test_data = "John Doe 30 NYC";
    let result = csv_adapter.format(test_data);
    println!("✓ CSV formatting successful: {}", result);
}