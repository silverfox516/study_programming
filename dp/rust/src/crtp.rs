// CRTP (Curiously Recurring Template Pattern) in Rust
// In Rust, achieved via traits with Self-referencing bounds

// Static polymorphism: trait with default methods using Self
trait Animal: Sized + std::fmt::Debug {
    fn name(&self) -> &str;
    fn sound(&self) -> &str;

    // CRTP-like: default method that uses concrete type info
    fn introduce(&self) {
        println!("I am {} the {:?}, I say '{}'", self.name(), self.type_name(), self.sound());
    }

    fn type_name(&self) -> &'static str {
        std::any::type_name::<Self>()
    }
}

#[derive(Debug)]
struct Dog {
    name: String,
}

impl Animal for Dog {
    fn name(&self) -> &str { &self.name }
    fn sound(&self) -> &str { "Woof!" }
}

#[derive(Debug)]
struct Cat {
    name: String,
}

impl Animal for Cat {
    fn name(&self) -> &str { &self.name }
    fn sound(&self) -> &str { "Meow!" }
}

// CRTP for compile-time mixin: Comparable
trait Comparable: PartialOrd + Sized {
    fn less_than(&self, other: &Self) -> bool {
        self < other
    }
    fn greater_than(&self, other: &Self) -> bool {
        self > other
    }
    fn clamp_to<'a>(&'a self, min: &'a Self, max: &'a Self) -> &'a Self
    where
        Self: PartialOrd,
    {
        if self < min { min }
        else if self > max { max }
        else { self }
    }
}

#[derive(Debug, PartialEq, PartialOrd)]
struct Score(u32);
impl Comparable for Score {}

// CRTP for builder pattern
trait Buildable: Sized {
    fn validate(&self) -> Result<(), String>;

    fn build(self) -> Result<Self, String> {
        self.validate()?;
        Ok(self)
    }
}

struct ServerConfig {
    host: String,
    port: u16,
}

impl Buildable for ServerConfig {
    fn validate(&self) -> Result<(), String> {
        if self.host.is_empty() { return Err("host required".into()); }
        if self.port == 0 { return Err("port must be > 0".into()); }
        Ok(())
    }
}

fn main() {
    // Static polymorphism
    println!("=== Static Polymorphism ===");
    let dog = Dog { name: "Rex".into() };
    let cat = Cat { name: "Luna".into() };
    dog.introduce();
    cat.introduce();

    // Comparable mixin
    println!("\n=== Comparable Mixin ===");
    let s1 = Score(85);
    let s2 = Score(92);
    println!("{:?} < {:?}: {}", s1, s2, s1.less_than(&s2));
    let min = Score(0);
    let max = Score(100);
    let clamped = Score(150).clamp_to(&min, &max);
    println!("Score(150) clamped: {:?}", clamped);

    // Buildable
    println!("\n=== Buildable ===");
    let config = ServerConfig { host: "localhost".into(), port: 8080 };
    match config.build() {
        Ok(c) => println!("Built: {}:{}", c.host, c.port),
        Err(e) => println!("Error: {}", e),
    }
}
