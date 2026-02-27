/// Facade Pattern Implementation in Rust
///
/// The Facade pattern provides a simplified interface to a complex subsystem.
/// It defines a higher-level interface that makes the subsystem easier to use
/// by hiding the complexities of the underlying components.

use std::collections::HashMap;
use std::time::{SystemTime, UNIX_EPOCH};

/// Complex subsystem components for a home theater system

/// DVD Player subsystem
pub struct DvdPlayer {
    powered_on: bool,
    current_disc: Option<String>,
    volume: u8,
}

impl DvdPlayer {
    pub fn new() -> Self {
        DvdPlayer {
            powered_on: false,
            current_disc: None,
            volume: 50,
        }
    }

    pub fn power_on(&mut self) -> String {
        self.powered_on = true;
        "DVD Player: Powering on...".to_string()
    }

    pub fn power_off(&mut self) -> String {
        self.powered_on = false;
        self.current_disc = None;
        "DVD Player: Powering off...".to_string()
    }

    pub fn insert_disc(&mut self, title: &str) -> String {
        if !self.powered_on {
            return "DVD Player: Please power on first".to_string();
        }
        self.current_disc = Some(title.to_string());
        format!("DVD Player: Inserting disc '{}'", title)
    }

    pub fn play(&self) -> String {
        if !self.powered_on {
            return "DVD Player: Not powered on".to_string();
        }
        match &self.current_disc {
            Some(title) => format!("DVD Player: Playing '{}'", title),
            None => "DVD Player: No disc inserted".to_string(),
        }
    }

    pub fn stop(&self) -> String {
        if self.powered_on {
            "DVD Player: Stopping playback".to_string()
        } else {
            "DVD Player: Not powered on".to_string()
        }
    }

    pub fn set_volume(&mut self, volume: u8) -> String {
        if self.powered_on {
            self.volume = volume.min(100);
            format!("DVD Player: Volume set to {}", self.volume)
        } else {
            "DVD Player: Not powered on".to_string()
        }
    }
}

impl Default for DvdPlayer {
    fn default() -> Self {
        Self::new()
    }
}

/// Projector subsystem
pub struct Projector {
    powered_on: bool,
    input_source: Option<String>,
    brightness: u8,
}

impl Projector {
    pub fn new() -> Self {
        Projector {
            powered_on: false,
            input_source: None,
            brightness: 75,
        }
    }

    pub fn power_on(&mut self) -> String {
        self.powered_on = true;
        "Projector: Warming up... Ready".to_string()
    }

    pub fn power_off(&mut self) -> String {
        self.powered_on = false;
        self.input_source = None;
        "Projector: Cooling down and powering off".to_string()
    }

    pub fn set_input(&mut self, source: &str) -> String {
        if !self.powered_on {
            return "Projector: Please power on first".to_string();
        }
        self.input_source = Some(source.to_string());
        format!("Projector: Input source set to {}", source)
    }

    pub fn set_wide_screen_mode(&self) -> String {
        if self.powered_on {
            "Projector: Setting wide screen mode (16:9)".to_string()
        } else {
            "Projector: Not powered on".to_string()
        }
    }

    pub fn adjust_brightness(&mut self, brightness: u8) -> String {
        if self.powered_on {
            self.brightness = brightness.min(100);
            format!("Projector: Brightness adjusted to {}", self.brightness)
        } else {
            "Projector: Not powered on".to_string()
        }
    }
}

impl Default for Projector {
    fn default() -> Self {
        Self::new()
    }
}

/// Sound System subsystem
pub struct SoundSystem {
    powered_on: bool,
    volume: u8,
    surround_sound: bool,
    input_source: Option<String>,
}

impl SoundSystem {
    pub fn new() -> Self {
        SoundSystem {
            powered_on: false,
            volume: 30,
            surround_sound: false,
            input_source: None,
        }
    }

    pub fn power_on(&mut self) -> String {
        self.powered_on = true;
        "Sound System: Powering on amplifier and speakers".to_string()
    }

    pub fn power_off(&mut self) -> String {
        self.powered_on = false;
        self.input_source = None;
        "Sound System: Powering off".to_string()
    }

    pub fn set_volume(&mut self, volume: u8) -> String {
        if self.powered_on {
            self.volume = volume.min(100);
            format!("Sound System: Volume set to {}", self.volume)
        } else {
            "Sound System: Not powered on".to_string()
        }
    }

    pub fn set_surround_sound(&mut self, enabled: bool) -> String {
        if self.powered_on {
            self.surround_sound = enabled;
            let status = if enabled { "enabled" } else { "disabled" };
            format!("Sound System: Surround sound {}", status)
        } else {
            "Sound System: Not powered on".to_string()
        }
    }

    pub fn set_input(&mut self, source: &str) -> String {
        if self.powered_on {
            self.input_source = Some(source.to_string());
            format!("Sound System: Input source set to {}", source)
        } else {
            "Sound System: Not powered on".to_string()
        }
    }
}

impl Default for SoundSystem {
    fn default() -> Self {
        Self::new()
    }
}

/// Lighting subsystem
pub struct Lights {
    dimmed: bool,
    brightness: u8,
}

impl Lights {
    pub fn new() -> Self {
        Lights {
            dimmed: false,
            brightness: 100,
        }
    }

    pub fn dim(&mut self, level: u8) -> String {
        self.dimmed = true;
        self.brightness = level.min(100);
        format!("Lights: Dimmed to {}%", self.brightness)
    }

    pub fn turn_on(&mut self) -> String {
        self.dimmed = false;
        self.brightness = 100;
        "Lights: Turned on to full brightness".to_string()
    }

    pub fn turn_off(&mut self) -> String {
        self.brightness = 0;
        "Lights: Turned off".to_string()
    }
}

impl Default for Lights {
    fn default() -> Self {
        Self::new()
    }
}

/// Popcorn Machine subsystem
pub struct PopcornMachine {
    powered_on: bool,
    popping: bool,
}

impl PopcornMachine {
    pub fn new() -> Self {
        PopcornMachine {
            powered_on: false,
            popping: false,
        }
    }

    pub fn power_on(&mut self) -> String {
        self.powered_on = true;
        "Popcorn Machine: Powered on".to_string()
    }

    pub fn power_off(&mut self) -> String {
        self.powered_on = false;
        self.popping = false;
        "Popcorn Machine: Powered off".to_string()
    }

    pub fn start_popping(&mut self) -> String {
        if self.powered_on {
            self.popping = true;
            "Popcorn Machine: Popping corn... Pop! Pop! Pop!".to_string()
        } else {
            "Popcorn Machine: Not powered on".to_string()
        }
    }

    pub fn stop_popping(&mut self) -> String {
        if self.powered_on {
            self.popping = false;
            "Popcorn Machine: Finished popping".to_string()
        } else {
            "Popcorn Machine: Not powered on".to_string()
        }
    }
}

impl Default for PopcornMachine {
    fn default() -> Self {
        Self::new()
    }
}

/// Home Theater Facade - Simplified interface to the complex subsystem
pub struct HomeTheaterFacade {
    dvd_player: DvdPlayer,
    projector: Projector,
    sound_system: SoundSystem,
    lights: Lights,
    popcorn_machine: PopcornMachine,
    presets: HashMap<String, TheaterPreset>,
}

pub struct TheaterPreset {
    pub volume: u8,
    pub brightness: u8,
    pub surround_sound: bool,
    pub light_level: u8,
}

impl HomeTheaterFacade {
    pub fn new() -> Self {
        let mut presets = HashMap::new();

        presets.insert("movie".to_string(), TheaterPreset {
            volume: 70,
            brightness: 85,
            surround_sound: true,
            light_level: 10,
        });

        presets.insert("music".to_string(), TheaterPreset {
            volume: 60,
            brightness: 50,
            surround_sound: true,
            light_level: 30,
        });

        presets.insert("gaming".to_string(), TheaterPreset {
            volume: 80,
            brightness: 90,
            surround_sound: true,
            light_level: 40,
        });

        HomeTheaterFacade {
            dvd_player: DvdPlayer::new(),
            projector: Projector::new(),
            sound_system: SoundSystem::new(),
            lights: Lights::new(),
            popcorn_machine: PopcornMachine::new(),
            presets,
        }
    }

    /// Simple interface to watch a movie
    pub fn watch_movie(&mut self, movie: &str) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("Starting movie experience...".to_string());
        steps.push(self.popcorn_machine.power_on());
        steps.push(self.popcorn_machine.start_popping());
        steps.push(self.lights.dim(10));
        steps.push(self.projector.power_on());
        steps.push(self.projector.set_input("DVD"));
        steps.push(self.projector.set_wide_screen_mode());
        steps.push(self.sound_system.power_on());
        steps.push(self.sound_system.set_input("DVD"));
        steps.push(self.sound_system.set_surround_sound(true));
        steps.push(self.sound_system.set_volume(70));
        steps.push(self.dvd_player.power_on());
        steps.push(self.dvd_player.insert_disc(movie));
        steps.push(self.dvd_player.play());
        steps.push("Movie is now playing! Enjoy!".to_string());

        steps
    }

    /// Simple interface to end movie
    pub fn end_movie(&mut self) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("Ending movie experience...".to_string());
        steps.push(self.dvd_player.stop());
        steps.push(self.dvd_player.power_off());
        steps.push(self.sound_system.power_off());
        steps.push(self.projector.power_off());
        steps.push(self.lights.turn_on());
        steps.push(self.popcorn_machine.stop_popping());
        steps.push(self.popcorn_machine.power_off());
        steps.push("Movie experience ended. Thanks for watching!".to_string());

        steps
    }

    /// Listen to music with optimized settings
    pub fn listen_to_music(&mut self) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("Setting up music experience...".to_string());
        steps.push(self.lights.dim(30));
        steps.push(self.sound_system.power_on());
        steps.push(self.sound_system.set_input("AUX"));
        steps.push(self.sound_system.set_surround_sound(true));
        steps.push(self.sound_system.set_volume(60));
        steps.push("Music system ready! Connect your device to AUX input.".to_string());

        steps
    }

    /// Gaming setup
    pub fn start_gaming(&mut self) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("Setting up gaming experience...".to_string());
        steps.push(self.lights.dim(40));
        steps.push(self.projector.power_on());
        steps.push(self.projector.set_input("HDMI"));
        steps.push(self.projector.adjust_brightness(90));
        steps.push(self.sound_system.power_on());
        steps.push(self.sound_system.set_input("HDMI"));
        steps.push(self.sound_system.set_surround_sound(true));
        steps.push(self.sound_system.set_volume(80));
        steps.push("Gaming setup complete! Connect your console to HDMI.".to_string());

        steps
    }

    /// Apply a preset configuration
    pub fn apply_preset(&mut self, preset_name: &str) -> Result<Vec<String>, String> {
        let preset = self.presets.get(preset_name)
            .ok_or_else(|| format!("Unknown preset: {}", preset_name))?
            .clone();

        let mut steps = Vec::new();
        steps.push(format!("Applying '{}' preset...", preset_name));

        if self.sound_system.powered_on {
            steps.push(self.sound_system.set_volume(preset.volume));
            steps.push(self.sound_system.set_surround_sound(preset.surround_sound));
        }

        if self.projector.powered_on {
            steps.push(self.projector.adjust_brightness(preset.brightness));
        }

        steps.push(self.lights.dim(preset.light_level));
        steps.push(format!("'{}' preset applied successfully!", preset_name));

        Ok(steps)
    }

    /// Get system status
    pub fn get_status(&self) -> String {
        format!(
            "=== Home Theater Status ===\n\
            DVD Player: {}\n\
            Projector: {}\n\
            Sound System: {}\n\
            Lights: {}%\n\
            Popcorn Machine: {}",
            if self.dvd_player.powered_on { "ON" } else { "OFF" },
            if self.projector.powered_on { "ON" } else { "OFF" },
            if self.sound_system.powered_on { "ON" } else { "OFF" },
            if self.lights.dimmed { self.lights.brightness } else { 100 },
            if self.popcorn_machine.powered_on { "ON" } else { "OFF" }
        )
    }

    /// Emergency shutdown
    pub fn emergency_shutdown(&mut self) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("EMERGENCY SHUTDOWN INITIATED!".to_string());
        steps.push(self.dvd_player.power_off());
        steps.push(self.sound_system.power_off());
        steps.push(self.projector.power_off());
        steps.push(self.popcorn_machine.power_off());
        steps.push(self.lights.turn_on());
        steps.push("All systems safely shut down.".to_string());

        steps
    }

    /// Add custom preset
    pub fn add_preset(&mut self, name: &str, preset: TheaterPreset) -> Result<(), String> {
        if self.presets.contains_key(name) {
            return Err(format!("Preset '{}' already exists", name));
        }
        self.presets.insert(name.to_string(), preset);
        Ok(())
    }

    /// List available presets
    pub fn list_presets(&self) -> Vec<String> {
        self.presets.keys().cloned().collect()
    }
}

impl Default for HomeTheaterFacade {
    fn default() -> Self {
        Self::new()
    }
}

/// Computer subsystem facade example
pub struct ComputerFacade {
    cpu: Cpu,
    memory: Memory,
    hard_drive: HardDrive,
    graphics: Graphics,
}

struct Cpu {
    powered_on: bool,
    load: f32,
}

struct Memory {
    size_gb: u32,
    used_gb: u32,
}

struct HardDrive {
    size_gb: u32,
    used_gb: u32,
    boot_sector_loaded: bool,
}

struct Graphics {
    powered_on: bool,
    resolution: (u32, u32),
}

impl ComputerFacade {
    pub fn new() -> Self {
        ComputerFacade {
            cpu: Cpu { powered_on: false, load: 0.0 },
            memory: Memory { size_gb: 16, used_gb: 0 },
            hard_drive: HardDrive { size_gb: 512, used_gb: 100, boot_sector_loaded: false },
            graphics: Graphics { powered_on: false, resolution: (1920, 1080) },
        }
    }

    /// Simple boot sequence
    pub fn start_computer(&mut self) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("Starting computer...".to_string());

        // Power on CPU
        self.cpu.powered_on = true;
        steps.push("CPU: Powered on and initializing".to_string());

        // Initialize memory
        self.memory.used_gb = 2; // OS overhead
        steps.push(format!("Memory: Initialized {}GB", self.memory.size_gb));

        // Load boot sector
        self.hard_drive.boot_sector_loaded = true;
        steps.push("Hard Drive: Boot sector loaded".to_string());

        // Initialize graphics
        self.graphics.powered_on = true;
        steps.push(format!("Graphics: Display initialized at {}x{}",
                          self.graphics.resolution.0, self.graphics.resolution.1));

        // Set initial CPU load
        self.cpu.load = 15.0;
        steps.push("System: Boot complete, ready for use".to_string());

        steps
    }

    /// Simple shutdown sequence
    pub fn shutdown_computer(&mut self) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("Shutting down computer...".to_string());

        // Save and close applications
        steps.push("System: Saving open documents and closing applications".to_string());

        // Clear memory
        self.memory.used_gb = 0;
        steps.push("Memory: Cleared".to_string());

        // Unmount drives
        self.hard_drive.boot_sector_loaded = false;
        steps.push("Hard Drive: Safely unmounted".to_string());

        // Turn off graphics
        self.graphics.powered_on = false;
        steps.push("Graphics: Display turned off".to_string());

        // Power off CPU
        self.cpu.powered_on = false;
        self.cpu.load = 0.0;
        steps.push("CPU: Powered off".to_string());

        steps.push("Computer shutdown complete".to_string());

        steps
    }

    /// Get system information
    pub fn get_system_info(&self) -> String {
        format!(
            "=== Computer System Info ===\n\
            CPU: {} (Load: {:.1}%)\n\
            Memory: {}/{} GB used\n\
            Storage: {}/{} GB used\n\
            Graphics: {} at {}x{}",
            if self.cpu.powered_on { "Running" } else { "Off" },
            self.cpu.load,
            self.memory.used_gb,
            self.memory.size_gb,
            self.hard_drive.used_gb,
            self.hard_drive.size_gb,
            if self.graphics.powered_on { "Active" } else { "Off" },
            self.graphics.resolution.0,
            self.graphics.resolution.1
        )
    }
}

impl Default for ComputerFacade {
    fn default() -> Self {
        Self::new()
    }
}

/// Smart home facade
pub struct SmartHomeFacade {
    temperature: f32,
    lights_on: bool,
    security_armed: bool,
    music_playing: bool,
    timestamp: u64,
}

impl SmartHomeFacade {
    pub fn new() -> Self {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();

        SmartHomeFacade {
            temperature: 22.0,
            lights_on: false,
            security_armed: false,
            music_playing: false,
            timestamp,
        }
    }

    /// Leaving home scenario
    pub fn leave_home(&mut self) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("Activating 'leaving home' mode...".to_string());

        // Turn off lights
        self.lights_on = false;
        steps.push("Lights: All lights turned off".to_string());

        // Lower temperature
        self.temperature = 18.0;
        steps.push("Thermostat: Temperature lowered to 18°C".to_string());

        // Stop music
        if self.music_playing {
            self.music_playing = false;
            steps.push("Music: Stopped and speakers turned off".to_string());
        }

        // Arm security
        self.security_armed = true;
        steps.push("Security: System armed".to_string());

        steps.push("House is now in away mode. Have a safe trip!".to_string());

        steps
    }

    /// Arriving home scenario
    pub fn arrive_home(&mut self) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("Welcome home! Activating arrival mode...".to_string());

        // Disarm security
        self.security_armed = false;
        steps.push("Security: System disarmed".to_string());

        // Turn on lights
        self.lights_on = true;
        steps.push("Lights: Welcome lighting activated".to_string());

        // Adjust temperature
        self.temperature = 22.0;
        steps.push("Thermostat: Temperature set to comfortable 22°C".to_string());

        // Start music if evening
        let current_hour = (self.timestamp / 3600) % 24;
        if current_hour >= 18 || current_hour <= 6 {
            self.music_playing = true;
            steps.push("Music: Evening playlist started".to_string());
        }

        steps.push("Home is ready and comfortable!".to_string());

        steps
    }

    /// Good night scenario
    pub fn good_night(&mut self) -> Vec<String> {
        let mut steps = Vec::new();

        steps.push("Activating good night mode...".to_string());

        // Turn off lights
        self.lights_on = false;
        steps.push("Lights: All lights turned off".to_string());

        // Lower temperature slightly
        self.temperature = 20.0;
        steps.push("Thermostat: Temperature lowered to 20°C for sleeping".to_string());

        // Stop music
        if self.music_playing {
            self.music_playing = false;
            steps.push("Music: Stopped for peaceful sleep".to_string());
        }

        // Arm security
        self.security_armed = true;
        steps.push("Security: Night mode activated".to_string());

        steps.push("Sweet dreams! House is secured for the night.".to_string());

        steps
    }

    /// Get home status
    pub fn get_home_status(&self) -> String {
        format!(
            "=== Smart Home Status ===\n\
            Temperature: {:.1}°C\n\
            Lights: {}\n\
            Security: {}\n\
            Music: {}",
            self.temperature,
            if self.lights_on { "On" } else { "Off" },
            if self.security_armed { "Armed" } else { "Disarmed" },
            if self.music_playing { "Playing" } else { "Off" }
        )
    }
}

impl Default for SmartHomeFacade {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_home_theater_movie_experience() {
        let mut theater = HomeTheaterFacade::new();
        let steps = theater.watch_movie("The Matrix");

        assert!(!steps.is_empty());
        assert!(steps.iter().any(|s| s.contains("Starting movie")));
        assert!(steps.iter().any(|s| s.contains("The Matrix")));

        let end_steps = theater.end_movie();
        assert!(end_steps.iter().any(|s| s.contains("Ending movie")));
    }

    #[test]
    fn test_theater_presets() {
        let mut theater = HomeTheaterFacade::new();
        let presets = theater.list_presets();

        assert!(presets.contains(&"movie".to_string()));
        assert!(presets.contains(&"music".to_string()));
        assert!(presets.contains(&"gaming".to_string()));

        // Test adding custom preset
        let custom_preset = TheaterPreset {
            volume: 50,
            brightness: 60,
            surround_sound: false,
            light_level: 25,
        };

        assert!(theater.add_preset("custom", custom_preset).is_ok());
        assert!(theater.add_preset("custom", TheaterPreset {
            volume: 50, brightness: 60, surround_sound: false, light_level: 25
        }).is_err()); // Duplicate name
    }

    #[test]
    fn test_computer_facade() {
        let mut computer = ComputerFacade::new();

        let start_steps = computer.start_computer();
        assert!(start_steps.iter().any(|s| s.contains("Starting computer")));
        assert!(start_steps.iter().any(|s| s.contains("Boot complete")));

        let shutdown_steps = computer.shutdown_computer();
        assert!(shutdown_steps.iter().any(|s| s.contains("Shutting down")));
        assert!(shutdown_steps.iter().any(|s| s.contains("shutdown complete")));
    }

    #[test]
    fn test_smart_home_scenarios() {
        let mut home = SmartHomeFacade::new();

        let leave_steps = home.leave_home();
        assert!(leave_steps.iter().any(|s| s.contains("leaving home")));

        let arrive_steps = home.arrive_home();
        assert!(arrive_steps.iter().any(|s| s.contains("Welcome home")));

        let night_steps = home.good_night();
        assert!(night_steps.iter().any(|s| s.contains("good night")));
    }

    #[test]
    fn test_dvd_player_subsystem() {
        let mut player = DvdPlayer::new();

        assert!(player.play().contains("Not powered on"));

        player.power_on();
        assert!(player.play().contains("No disc"));

        player.insert_disc("Test Movie");
        assert!(player.play().contains("Test Movie"));
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Facade Pattern Demo ===");

    // Home Theater Facade
    println!("\n1. Home Theater System:");
    let mut theater = HomeTheaterFacade::new();

    println!("Starting movie night...");
    let movie_steps = theater.watch_movie("Inception");
    for step in &movie_steps {
        println!("  {}", step);
    }

    println!("\n{}", theater.get_status());

    println!("\nEnding movie experience...");
    let end_steps = theater.end_movie();
    for step in end_steps.iter().take(5) { // Show first 5 steps
        println!("  {}", step);
    }

    // Apply preset
    println!("\n2. Using Presets:");
    theater.sound_system.power_on();
    theater.projector.power_on();

    if let Ok(preset_steps) = theater.apply_preset("gaming") {
        for step in preset_steps {
            println!("  {}", step);
        }
    }

    // Computer Facade
    println!("\n3. Computer System:");
    let mut computer = ComputerFacade::new();

    println!("Booting computer...");
    let boot_steps = computer.start_computer();
    for step in boot_steps.iter().take(4) {
        println!("  {}", step);
    }

    println!("\n{}", computer.get_system_info());

    // Smart Home Facade
    println!("\n4. Smart Home Control:");
    let mut home = SmartHomeFacade::new();

    println!("Leaving home...");
    let leave_steps = home.leave_home();
    for step in leave_steps.iter().take(4) {
        println!("  {}", step);
    }

    println!("\n{}", home.get_home_status());

    println!("\nArriving home...");
    let arrive_steps = home.arrive_home();
    for step in arrive_steps.iter().take(4) {
        println!("  {}", step);
    }
}

pub fn main() {
    demo();
}