/// Flyweight Pattern Implementation in Rust
///
/// The Flyweight pattern uses sharing to support large numbers of fine-grained
/// objects efficiently. It separates intrinsic state (shared) from extrinsic
/// state (context-specific) to minimize memory usage.

use std::collections::HashMap;
use std::rc::Rc;
use std::cell::RefCell;

/// Character flyweight for a text editor
#[derive(Debug, Clone, Hash, Eq, PartialEq)]
pub struct CharacterFlyweight {
    character: char,
    font_family: String,
    font_size: u16,
    style: FontStyle,
}

#[derive(Debug, Clone, Hash, Eq, PartialEq)]
pub enum FontStyle {
    Regular,
    Bold,
    Italic,
    BoldItalic,
}

impl CharacterFlyweight {
    fn new(character: char, font_family: String, font_size: u16, style: FontStyle) -> Self {
        CharacterFlyweight {
            character,
            font_family,
            font_size,
            style,
        }
    }

    /// Render the character with extrinsic state (position, color)
    pub fn render(&self, x: f32, y: f32, color: &str) -> String {
        format!(
            "Rendering '{}' at ({:.1}, {:.1}) with {} {}pt {} in {}",
            self.character,
            x,
            y,
            self.font_family,
            self.font_size,
            format!("{:?}", self.style).to_lowercase(),
            color
        )
    }

    pub fn get_metrics(&self) -> CharacterMetrics {
        // Simulate font metrics calculation
        let base_width = match self.font_size {
            8..=12 => 6.0,
            13..=16 => 8.0,
            17..=24 => 12.0,
            _ => 16.0,
        };

        let width_multiplier = match self.style {
            FontStyle::Bold | FontStyle::BoldItalic => 1.1,
            _ => 1.0,
        };

        CharacterMetrics {
            width: base_width * width_multiplier,
            height: self.font_size as f32 * 1.2,
            baseline: self.font_size as f32 * 0.8,
        }
    }
}

#[derive(Debug, Clone)]
pub struct CharacterMetrics {
    pub width: f32,
    pub height: f32,
    pub baseline: f32,
}

/// Flyweight factory to manage character flyweights
pub struct CharacterFlyweightFactory {
    flyweights: HashMap<CharacterKey, Rc<CharacterFlyweight>>,
}

#[derive(Debug, Clone, Hash, Eq, PartialEq)]
struct CharacterKey {
    character: char,
    font_family: String,
    font_size: u16,
    style: FontStyle,
}

impl CharacterFlyweightFactory {
    pub fn new() -> Self {
        CharacterFlyweightFactory {
            flyweights: HashMap::new(),
        }
    }

    pub fn get_flyweight(
        &mut self,
        character: char,
        font_family: &str,
        font_size: u16,
        style: FontStyle,
    ) -> Rc<CharacterFlyweight> {
        let key = CharacterKey {
            character,
            font_family: font_family.to_string(),
            font_size,
            style: style.clone(),
        };

        if let Some(flyweight) = self.flyweights.get(&key) {
            flyweight.clone()
        } else {
            let flyweight = Rc::new(CharacterFlyweight::new(
                character,
                font_family.to_string(),
                font_size,
                style,
            ));
            self.flyweights.insert(key, flyweight.clone());
            flyweight
        }
    }

    pub fn get_flyweight_count(&self) -> usize {
        self.flyweights.len()
    }

    pub fn get_memory_usage(&self) -> usize {
        // Estimate memory usage (simplified)
        self.flyweights.len() * std::mem::size_of::<CharacterFlyweight>()
    }
}

impl Default for CharacterFlyweightFactory {
    fn default() -> Self {
        Self::new()
    }
}

/// Context that uses flyweights (extrinsic state)
#[derive(Debug, Clone)]
pub struct CharacterContext {
    flyweight: Rc<CharacterFlyweight>,
    x: f32,
    y: f32,
    color: String,
}

impl CharacterContext {
    pub fn new(flyweight: Rc<CharacterFlyweight>, x: f32, y: f32, color: String) -> Self {
        CharacterContext {
            flyweight,
            x,
            y,
            color,
        }
    }

    pub fn render(&self) -> String {
        self.flyweight.render(self.x, self.y, &self.color)
    }

    pub fn move_to(&mut self, x: f32, y: f32) {
        self.x = x;
        self.y = y;
    }

    pub fn set_color(&mut self, color: String) {
        self.color = color;
    }

    pub fn get_character(&self) -> char {
        self.flyweight.character
    }

    pub fn get_metrics(&self) -> CharacterMetrics {
        self.flyweight.get_metrics()
    }
}

/// Document class that manages many character contexts
pub struct Document {
    characters: Vec<CharacterContext>,
    factory: CharacterFlyweightFactory,
}

impl Document {
    pub fn new() -> Self {
        Document {
            characters: Vec::new(),
            factory: CharacterFlyweightFactory::new(),
        }
    }

    pub fn add_character(
        &mut self,
        character: char,
        x: f32,
        y: f32,
        font_family: &str,
        font_size: u16,
        style: FontStyle,
        color: String,
    ) {
        let flyweight = self.factory.get_flyweight(character, font_family, font_size, style);
        let context = CharacterContext::new(flyweight, x, y, color);
        self.characters.push(context);
    }

    pub fn add_text(
        &mut self,
        text: &str,
        start_x: f32,
        start_y: f32,
        font_family: &str,
        font_size: u16,
        style: FontStyle,
        color: String,
    ) {
        let mut current_x = start_x;

        for character in text.chars() {
            if character == ' ' {
                current_x += font_size as f32 * 0.3; // Space width
                continue;
            }
            if character == '\n' {
                current_x = start_x;
                // Note: Y positioning for newlines would be handled by caller
                continue;
            }

            self.add_character(
                character,
                current_x,
                start_y,
                font_family,
                font_size,
                style.clone(),
                color.clone(),
            );

            // Move x position for next character
            let flyweight = self.factory.get_flyweight(character, font_family, font_size, style.clone());
            let metrics = flyweight.get_metrics();
            current_x += metrics.width;
        }
    }

    pub fn render(&self) -> Vec<String> {
        self.characters.iter().map(|c| c.render()).collect()
    }

    pub fn get_character_count(&self) -> usize {
        self.characters.len()
    }

    pub fn get_flyweight_count(&self) -> usize {
        self.factory.get_flyweight_count()
    }

    pub fn get_memory_efficiency(&self) -> f32 {
        if self.characters.is_empty() {
            return 0.0;
        }

        let total_characters = self.characters.len();
        let unique_flyweights = self.factory.get_flyweight_count();

        // Memory efficiency: fewer flyweights relative to total characters is better
        unique_flyweights as f32 / total_characters as f32
    }

    pub fn find_characters(&self, target: char) -> Vec<(f32, f32)> {
        self.characters
            .iter()
            .filter(|c| c.get_character() == target)
            .map(|c| (c.x, c.y))
            .collect()
    }

    pub fn get_statistics(&self) -> DocumentStatistics {
        let mut char_count = HashMap::new();
        let mut font_usage = HashMap::new();

        for character in &self.characters {
            let ch = character.get_character();
            *char_count.entry(ch).or_insert(0) += 1;

            let font_key = format!("{} {}pt",
                character.flyweight.font_family,
                character.flyweight.font_size);
            *font_usage.entry(font_key).or_insert(0) += 1;
        }

        DocumentStatistics {
            total_characters: self.characters.len(),
            unique_flyweights: self.factory.get_flyweight_count(),
            character_frequency: char_count,
            font_usage,
            memory_efficiency: self.get_memory_efficiency(),
        }
    }
}

impl Default for Document {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug)]
pub struct DocumentStatistics {
    pub total_characters: usize,
    pub unique_flyweights: usize,
    pub character_frequency: HashMap<char, usize>,
    pub font_usage: HashMap<String, usize>,
    pub memory_efficiency: f32,
}

/// Tree flyweight example - for forest simulation
#[derive(Debug, Clone, Hash, Eq, PartialEq)]
pub struct TreeType {
    name: String,
    color: String,
    sprite_data: Vec<u8>, // Simplified sprite representation
}

impl TreeType {
    fn new(name: String, color: String, sprite_data: Vec<u8>) -> Self {
        TreeType {
            name,
            color,
            sprite_data,
        }
    }

    pub fn render(&self, x: f32, y: f32, size: f32) -> String {
        format!(
            "Rendering {} tree ({}) at ({:.1}, {:.1}) with size {:.1} [sprite: {} bytes]",
            self.name,
            self.color,
            x,
            y,
            size,
            self.sprite_data.len()
        )
    }

    pub fn get_name(&self) -> &str {
        &self.name
    }

    pub fn get_color(&self) -> &str {
        &self.color
    }
}

/// Tree flyweight factory
pub struct TreeTypeFactory {
    tree_types: HashMap<String, Rc<TreeType>>,
}

impl TreeTypeFactory {
    pub fn new() -> Self {
        TreeTypeFactory {
            tree_types: HashMap::new(),
        }
    }

    pub fn get_tree_type(&mut self, name: &str, color: &str) -> Rc<TreeType> {
        let key = format!("{}_{}", name, color);

        if let Some(tree_type) = self.tree_types.get(&key) {
            tree_type.clone()
        } else {
            // Simulate loading sprite data
            let sprite_data = vec![0u8; 1024]; // 1KB sprite data
            let tree_type = Rc::new(TreeType::new(
                name.to_string(),
                color.to_string(),
                sprite_data,
            ));
            self.tree_types.insert(key, tree_type.clone());
            tree_type
        }
    }

    pub fn get_created_types(&self) -> Vec<String> {
        self.tree_types.keys().cloned().collect()
    }

    pub fn get_memory_usage(&self) -> usize {
        self.tree_types.len() * (std::mem::size_of::<TreeType>() + 1024) // Include sprite data
    }
}

impl Default for TreeTypeFactory {
    fn default() -> Self {
        Self::new()
    }
}

/// Tree context (extrinsic state)
#[derive(Debug, Clone)]
pub struct Tree {
    tree_type: Rc<TreeType>,
    x: f32,
    y: f32,
    size: f32,
}

impl Tree {
    pub fn new(tree_type: Rc<TreeType>, x: f32, y: f32, size: f32) -> Self {
        Tree {
            tree_type,
            x,
            y,
            size,
        }
    }

    pub fn render(&self) -> String {
        self.tree_type.render(self.x, self.y, self.size)
    }

    pub fn get_position(&self) -> (f32, f32) {
        (self.x, self.y)
    }

    pub fn get_type_name(&self) -> &str {
        self.tree_type.get_name()
    }
}

/// Forest that manages many trees
pub struct Forest {
    trees: Vec<Tree>,
    factory: Rc<RefCell<TreeTypeFactory>>,
}

impl Forest {
    pub fn new() -> Self {
        Forest {
            trees: Vec::new(),
            factory: Rc::new(RefCell::new(TreeTypeFactory::new())),
        }
    }

    pub fn plant_tree(&mut self, x: f32, y: f32, name: &str, color: &str, size: f32) {
        let tree_type = self.factory.borrow_mut().get_tree_type(name, color);
        let tree = Tree::new(tree_type, x, y, size);
        self.trees.push(tree);
    }

    pub fn render_forest(&self) -> Vec<String> {
        self.trees.iter().map(|tree| tree.render()).collect()
    }

    pub fn get_tree_count(&self) -> usize {
        self.trees.len()
    }

    pub fn get_unique_types(&self) -> usize {
        self.factory.borrow().tree_types.len()
    }

    pub fn get_memory_savings(&self) -> f32 {
        if self.trees.is_empty() {
            return 0.0;
        }

        let total_trees = self.trees.len();
        let unique_types = self.get_unique_types();

        // Calculate memory savings compared to storing full tree data for each tree
        let with_flyweight = unique_types * 1024 + total_trees * std::mem::size_of::<Tree>();
        let without_flyweight = total_trees * (std::mem::size_of::<Tree>() + 1024);

        ((without_flyweight - with_flyweight) as f32 / without_flyweight as f32) * 100.0
    }

    pub fn find_trees_by_type(&self, name: &str) -> Vec<(f32, f32)> {
        self.trees
            .iter()
            .filter(|tree| tree.get_type_name() == name)
            .map(|tree| tree.get_position())
            .collect()
    }

    pub fn get_forest_statistics(&self) -> ForestStatistics {
        let mut type_count = HashMap::new();
        let mut total_area = 0.0;

        for tree in &self.trees {
            let type_name = tree.get_type_name().to_string();
            *type_count.entry(type_name).or_insert(0) += 1;
            total_area += tree.size * tree.size; // Simplified area calculation
        }

        ForestStatistics {
            total_trees: self.trees.len(),
            unique_types: self.get_unique_types(),
            type_distribution: type_count,
            total_area,
            memory_savings: self.get_memory_savings(),
        }
    }
}

impl Default for Forest {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug)]
pub struct ForestStatistics {
    pub total_trees: usize,
    pub unique_types: usize,
    pub type_distribution: HashMap<String, usize>,
    pub total_area: f32,
    pub memory_savings: f32,
}

/// Particle system flyweight example
#[derive(Debug, Clone, Hash, Eq, PartialEq)]
pub struct ParticleType {
    color: (u8, u8, u8),
    texture: String,
    size: u8,
}

impl ParticleType {
    fn new(color: (u8, u8, u8), texture: String, size: u8) -> Self {
        ParticleType {
            color,
            texture,
            size,
        }
    }

    pub fn render(&self, x: f32, y: f32, velocity_x: f32, velocity_y: f32, age: f32) -> String {
        format!(
            "Particle at ({:.1}, {:.1}) vel:({:.1}, {:.1}) age:{:.1}s - {} texture RGB({},{},{}) size:{}",
            x, y, velocity_x, velocity_y, age,
            self.texture, self.color.0, self.color.1, self.color.2, self.size
        )
    }
}

pub struct ParticleSystem {
    particles: Vec<Particle>,
    particle_types: HashMap<ParticleTypeKey, Rc<ParticleType>>,
}

#[derive(Debug, Clone, Hash, Eq, PartialEq)]
struct ParticleTypeKey {
    color: (u8, u8, u8),
    texture: String,
    size: u8,
}

#[derive(Debug, Clone)]
pub struct Particle {
    particle_type: Rc<ParticleType>,
    x: f32,
    y: f32,
    velocity_x: f32,
    velocity_y: f32,
    age: f32,
}

impl ParticleSystem {
    pub fn new() -> Self {
        ParticleSystem {
            particles: Vec::new(),
            particle_types: HashMap::new(),
        }
    }

    pub fn create_particle(
        &mut self,
        x: f32,
        y: f32,
        velocity_x: f32,
        velocity_y: f32,
        color: (u8, u8, u8),
        texture: &str,
        size: u8,
    ) {
        let key = ParticleTypeKey {
            color,
            texture: texture.to_string(),
            size,
        };

        let particle_type = if let Some(existing) = self.particle_types.get(&key) {
            existing.clone()
        } else {
            let new_type = Rc::new(ParticleType::new(color, texture.to_string(), size));
            self.particle_types.insert(key, new_type.clone());
            new_type
        };

        let particle = Particle {
            particle_type,
            x,
            y,
            velocity_x,
            velocity_y,
            age: 0.0,
        };

        self.particles.push(particle);
    }

    pub fn update(&mut self, delta_time: f32) {
        for particle in &mut self.particles {
            particle.x += particle.velocity_x * delta_time;
            particle.y += particle.velocity_y * delta_time;
            particle.age += delta_time;
        }

        // Remove old particles
        self.particles.retain(|p| p.age < 10.0);
    }

    pub fn render(&self) -> Vec<String> {
        self.particles
            .iter()
            .map(|p| p.particle_type.render(p.x, p.y, p.velocity_x, p.velocity_y, p.age))
            .collect()
    }

    pub fn get_particle_count(&self) -> usize {
        self.particles.len()
    }

    pub fn get_type_count(&self) -> usize {
        self.particle_types.len()
    }
}

impl Default for ParticleSystem {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_character_flyweight_factory() {
        let mut factory = CharacterFlyweightFactory::new();

        let char_a1 = factory.get_flyweight('A', "Arial", 12, FontStyle::Regular);
        let char_a2 = factory.get_flyweight('A', "Arial", 12, FontStyle::Regular);

        // Same flyweight should be returned
        assert!(Rc::ptr_eq(&char_a1, &char_a2));
        assert_eq!(factory.get_flyweight_count(), 1);

        let char_b = factory.get_flyweight('B', "Arial", 12, FontStyle::Regular);
        assert_eq!(factory.get_flyweight_count(), 2);
        assert!(!Rc::ptr_eq(&char_a1, &char_b));
    }

    #[test]
    fn test_document_efficiency() {
        let mut doc = Document::new();

        // Add repeated text
        doc.add_text("AAA BBB AAA", 0.0, 0.0, "Arial", 12, FontStyle::Regular, "black".to_string());

        // Should have many characters but few flyweights
        assert!(doc.get_character_count() > doc.get_flyweight_count());
        assert!(doc.get_memory_efficiency() < 1.0);

        let positions = doc.find_characters('A');
        assert_eq!(positions.len(), 6); // 6 'A' characters
    }

    #[test]
    fn test_forest_memory_savings() {
        let mut forest = Forest::new();

        // Plant many trees of few types
        for i in 0..100 {
            let x = (i % 10) as f32;
            let y = (i / 10) as f32;

            let tree_type = if i % 3 == 0 { "Oak" } else { "Pine" };
            let color = if i % 2 == 0 { "Green" } else { "DarkGreen" };

            forest.plant_tree(x, y, tree_type, color, 1.0);
        }

        assert_eq!(forest.get_tree_count(), 100);
        assert!(forest.get_unique_types() < 10); // Much fewer unique types
        assert!(forest.get_memory_savings() > 0.0);
    }

    #[test]
    fn test_particle_system() {
        let mut system = ParticleSystem::new();

        // Create particles with overlapping types
        system.create_particle(0.0, 0.0, 1.0, 1.0, (255, 0, 0), "fire", 5);
        system.create_particle(1.0, 1.0, 1.0, 1.0, (255, 0, 0), "fire", 5);
        system.create_particle(2.0, 2.0, 1.0, 1.0, (0, 255, 0), "smoke", 3);

        assert_eq!(system.get_particle_count(), 3);
        assert_eq!(system.get_type_count(), 2); // fire and smoke types

        system.update(1.0);
        assert_eq!(system.get_particle_count(), 3); // All particles still alive
    }

    #[test]
    fn test_document_statistics() {
        let mut doc = Document::new();
        doc.add_text("Hello World!", 0.0, 0.0, "Arial", 12, FontStyle::Regular, "black".to_string());

        let stats = doc.get_statistics();
        assert!(stats.total_characters > 0);
        assert!(stats.unique_flyweights > 0);
        assert!(stats.character_frequency.contains_key(&'l')); // 'l' appears 3 times
        assert_eq!(stats.character_frequency[&'l'], 3);
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Flyweight Pattern Demo ===");

    // Document with character flyweights
    println!("\n1. Text Document with Character Flyweights:");
    let mut document = Document::new();

    document.add_text(
        "Hello World! Hello Rust!",
        10.0,
        50.0,
        "Arial",
        14,
        FontStyle::Regular,
        "black".to_string(),
    );

    document.add_text(
        "Hello World! Hello Rust!",
        10.0,
        80.0,
        "Arial",
        14,
        FontStyle::Bold,
        "blue".to_string(),
    );

    println!("Document characters: {}", document.get_character_count());
    println!("Unique flyweights: {}", document.get_flyweight_count());
    println!("Memory efficiency: {:.2}%", document.get_memory_efficiency() * 100.0);

    let stats = document.get_statistics();
    println!("\nCharacter frequency:");
    for (ch, count) in stats.character_frequency.iter().take(5) {
        if *ch != ' ' {
            println!("  '{}': {}", ch, count);
        }
    }

    // Forest simulation
    println!("\n2. Forest Simulation with Tree Flyweights:");
    let mut forest = Forest::new();

    // Plant a diverse forest
    let tree_types = ["Oak", "Pine", "Birch", "Maple"];
    let colors = ["Green", "DarkGreen", "LightGreen"];

    for i in 0..50 {
        let x = (i % 10) as f32 * 10.0;
        let y = (i / 10) as f32 * 10.0;
        let tree_type = tree_types[i % tree_types.len()];
        let color = colors[i % colors.len()];
        let size = 1.0 + (i % 3) as f32;

        forest.plant_tree(x, y, tree_type, color, size);
    }

    let forest_stats = forest.get_forest_statistics();
    println!("Total trees planted: {}", forest_stats.total_trees);
    println!("Unique tree types: {}", forest_stats.unique_types);
    println!("Memory savings: {:.1}%", forest_stats.memory_savings);

    println!("\nTree distribution:");
    for (tree_type, count) in &forest_stats.type_distribution {
        println!("  {}: {}", tree_type, count);
    }

    // Particle system
    println!("\n3. Particle System with Flyweights:");
    let mut particle_system = ParticleSystem::new();

    // Create explosion effect
    for i in 0..20 {
        let angle = (i as f32) * 0.314; // Rough approximation of pi/10
        let velocity_x = angle.cos() * 2.0;
        let velocity_y = angle.sin() * 2.0;

        let color = if i % 3 == 0 {
            (255, 100, 0) // Orange
        } else {
            (255, 200, 0) // Yellow
        };

        particle_system.create_particle(
            0.0, 0.0,
            velocity_x, velocity_y,
            color, "fire", 3
        );
    }

    println!("Particles created: {}", particle_system.get_particle_count());
    println!("Particle types: {}", particle_system.get_type_count());

    // Simulate some time
    particle_system.update(1.0);
    println!("After 1 second: {} particles", particle_system.get_particle_count());

    // Show a few particle renders
    let renders = particle_system.render();
    println!("\nSample particle renders:");
    for render in renders.iter().take(3) {
        println!("  {}", render);
    }

    println!("\n4. Memory Usage Comparison:");
    println!("Without Flyweight Pattern:");
    println!("  - Each character stores its own font data");
    println!("  - Each tree stores its own sprite data");
    println!("  - Each particle stores its own texture data");
    println!("  - Memory usage grows linearly with object count");

    println!("\nWith Flyweight Pattern:");
    println!("  - Intrinsic state (font, sprite, texture) shared among objects");
    println!("  - Only extrinsic state (position, color) stored per object");
    println!("  - Significant memory savings with many similar objects");
    println!("  - Memory usage grows much slower than object count");
}

pub fn main() {
    demo();
}