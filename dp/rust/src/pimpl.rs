// Pimpl (Pointer to Implementation) Pattern in Rust
// Hide implementation details behind a Box<dyn Trait> or opaque struct

// In Rust, this is idiomatically achieved with trait objects or modules

mod engine {
    // Private implementation — not exposed to users
    struct EngineImpl {
        cylinders: u32,
        horsepower: u32,
    }

    // Public interface
    pub struct Engine {
        inner: Box<EngineImpl>,
    }

    impl Engine {
        pub fn new(cylinders: u32, horsepower: u32) -> Self {
            Engine {
                inner: Box::new(EngineImpl { cylinders, horsepower }),
            }
        }

        pub fn description(&self) -> String {
            format!("{}cyl {}hp engine", self.inner.cylinders, self.inner.horsepower)
        }

        pub fn start(&self) {
            println!("[Engine] Starting {} ({}hp)", self.inner.cylinders, self.inner.horsepower);
        }

        pub fn stop(&self) {
            println!("[Engine] Stopping");
        }
    }
}

// Trait-based pimpl (more idiomatic Rust)
trait Renderer {
    fn render(&self, content: &str);
    fn clear(&self);
}

struct OpenGLRenderer;
impl Renderer for OpenGLRenderer {
    fn render(&self, content: &str) {
        println!("[OpenGL] Rendering: {}", content);
    }
    fn clear(&self) {
        println!("[OpenGL] Clear buffer");
    }
}

struct Widget {
    renderer: Box<dyn Renderer>,  // pimpl via trait object
    name: String,
}

impl Widget {
    fn new(name: &str, renderer: Box<dyn Renderer>) -> Self {
        Widget { renderer, name: name.to_string() }
    }

    fn draw(&self) {
        self.renderer.render(&self.name);
    }
}

fn main() {
    // Opaque struct pimpl
    println!("=== Opaque Struct ===");
    let engine = engine::Engine::new(4, 150);
    println!("{}", engine.description());
    engine.start();
    engine.stop();

    // Trait object pimpl
    println!("\n=== Trait Object ===");
    let widget = Widget::new("Button", Box::new(OpenGLRenderer));
    widget.draw();
}
