// Specification Pattern in Rust
// Composable business rules as objects

trait Specification<T> {
    fn is_satisfied_by(&self, item: &T) -> bool;
}

// Concrete specifications
struct MinAge(u32);
struct MaxPrice(f64);
struct InCategory(String);

#[derive(Debug)]
struct Product {
    name: String,
    price: f64,
    category: String,
    min_age: u32,
}

impl Specification<Product> for MinAge {
    fn is_satisfied_by(&self, item: &Product) -> bool {
        item.min_age >= self.0
    }
}

impl Specification<Product> for MaxPrice {
    fn is_satisfied_by(&self, item: &Product) -> bool {
        item.price <= self.0
    }
}

impl Specification<Product> for InCategory {
    fn is_satisfied_by(&self, item: &Product) -> bool {
        item.category == self.0
    }
}

// Composite specifications
struct AndSpec<A, B>(A, B);
struct OrSpec<A, B>(A, B);
struct NotSpec<A>(A);

impl<T, A: Specification<T>, B: Specification<T>> Specification<T> for AndSpec<A, B> {
    fn is_satisfied_by(&self, item: &T) -> bool {
        self.0.is_satisfied_by(item) && self.1.is_satisfied_by(item)
    }
}

impl<T, A: Specification<T>, B: Specification<T>> Specification<T> for OrSpec<A, B> {
    fn is_satisfied_by(&self, item: &T) -> bool {
        self.0.is_satisfied_by(item) || self.1.is_satisfied_by(item)
    }
}

impl<T, A: Specification<T>> Specification<T> for NotSpec<A> {
    fn is_satisfied_by(&self, item: &T) -> bool {
        !self.0.is_satisfied_by(item)
    }
}

fn filter<'a, T>(items: &'a [T], spec: &dyn Specification<T>) -> Vec<&'a T> {
    items.iter().filter(|item| spec.is_satisfied_by(item)).collect()
}

fn main() {
    let products = vec![
        Product { name: "Toy Car".into(), price: 15.0, category: "toys".into(), min_age: 3 },
        Product { name: "Laptop".into(), price: 999.0, category: "electronics".into(), min_age: 0 },
        Product { name: "Board Game".into(), price: 25.0, category: "toys".into(), min_age: 8 },
        Product { name: "Phone Case".into(), price: 10.0, category: "electronics".into(), min_age: 0 },
    ];

    // Simple spec
    let cheap = MaxPrice(30.0);
    println!("Cheap products (<=30):");
    for p in filter(&products, &cheap) {
        println!("  {} (${:.2})", p.name, p.price);
    }

    // Composite: cheap AND toys
    let cheap_toys = AndSpec(MaxPrice(30.0), InCategory("toys".into()));
    println!("\nCheap toys:");
    for p in filter(&products, &cheap_toys) {
        println!("  {} (${:.2})", p.name, p.price);
    }

    // NOT electronics
    let not_electronics = NotSpec(InCategory("electronics".into()));
    println!("\nNon-electronics:");
    for p in filter(&products, &not_electronics) {
        println!("  {}", p.name);
    }
}
