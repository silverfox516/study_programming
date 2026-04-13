// Type Erasure Pattern in Rust
// Hide concrete types behind trait objects for runtime polymorphism

use std::fmt;

// Type-erased drawable — any type that implements Draw
trait Draw: fmt::Debug {
    fn draw(&self);
    fn area(&self) -> f64;
}

#[derive(Debug)]
struct Circle {
    radius: f64,
}

impl Draw for Circle {
    fn draw(&self) { println!("  Drawing circle (r={})", self.radius); }
    fn area(&self) -> f64 { std::f64::consts::PI * self.radius * self.radius }
}

#[derive(Debug)]
struct Rectangle {
    width: f64,
    height: f64,
}

impl Draw for Rectangle {
    fn draw(&self) { println!("  Drawing rect ({}x{})", self.width, self.height); }
    fn area(&self) -> f64 { self.width * self.height }
}

#[derive(Debug)]
struct Triangle {
    base: f64,
    height: f64,
}

impl Draw for Triangle {
    fn draw(&self) { println!("  Drawing triangle (b={}, h={})", self.base, self.height); }
    fn area(&self) -> f64 { 0.5 * self.base * self.height }
}

// Canvas holds type-erased shapes
struct Canvas {
    shapes: Vec<Box<dyn Draw>>,
}

impl Canvas {
    fn new() -> Self { Canvas { shapes: Vec::new() } }

    fn add(&mut self, shape: impl Draw + 'static) {
        self.shapes.push(Box::new(shape));
    }

    fn draw_all(&self) {
        for shape in &self.shapes {
            shape.draw();
        }
    }

    fn total_area(&self) -> f64 {
        self.shapes.iter().map(|s| s.area()).sum()
    }
}

// Type erasure with closures (fn pointer style)
struct Callback {
    _name: String,
    func: Box<dyn Fn(i32) -> i32>,
}

fn main() {
    println!("=== Type-Erased Canvas ===");
    let mut canvas = Canvas::new();
    canvas.add(Circle { radius: 5.0 });
    canvas.add(Rectangle { width: 3.0, height: 4.0 });
    canvas.add(Triangle { base: 6.0, height: 3.0 });

    canvas.draw_all();
    println!("Total area: {:.2}", canvas.total_area());

    // Type-erased callbacks
    println!("\n=== Type-Erased Callbacks ===");
    let callbacks: Vec<Callback> = vec![
        Callback { _name: "double".into(), func: Box::new(|x| x * 2) },
        Callback { _name: "square".into(), func: Box::new(|x| x * x) },
        Callback { _name: "negate".into(), func: Box::new(|x| -x) },
    ];

    let input = 5;
    for cb in &callbacks {
        println!("  {}({}) = {}", cb._name, input, (cb.func)(input));
    }
}
