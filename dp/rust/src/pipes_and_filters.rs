// Pipes and Filters Pattern in Rust
// Chain processing steps using iterators / closures

type Filter<T> = Box<dyn Fn(Vec<T>) -> Vec<T>>;

struct Pipeline<T> {
    filters: Vec<Filter<T>>,
}

impl<T: 'static> Pipeline<T> {
    fn new() -> Self {
        Pipeline { filters: Vec::new() }
    }

    fn add_filter<F>(mut self, f: F) -> Self
    where
        F: Fn(Vec<T>) -> Vec<T> + 'static,
    {
        self.filters.push(Box::new(f));
        self
    }

    fn execute(self, input: Vec<T>) -> Vec<T> {
        self.filters.into_iter().fold(input, |data, filter| filter(data))
    }
}

// Text processing pipeline
fn main() {
    // Numeric pipeline
    println!("=== Numeric Pipeline ===");
    let result = Pipeline::new()
        .add_filter(|nums: Vec<i32>| {
            println!("[Filter] Remove negatives");
            nums.into_iter().filter(|&n| n >= 0).collect()
        })
        .add_filter(|nums| {
            println!("[Filter] Double values");
            nums.into_iter().map(|n| n * 2).collect()
        })
        .add_filter(|nums| {
            println!("[Filter] Keep < 100");
            nums.into_iter().filter(|&n| n < 100).collect()
        })
        .add_filter(|mut nums| {
            println!("[Filter] Sort");
            nums.sort();
            nums
        })
        .execute(vec![42, -5, 10, 80, 3, -1, 55, 25]);

    println!("Result: {:?}", result);

    // String pipeline
    println!("\n=== Text Pipeline ===");
    let texts = vec![
        "  Hello World  ".to_string(),
        "RUST programming".to_string(),
        "  ".to_string(),
        "pipes AND filters".to_string(),
    ];

    let processed = Pipeline::new()
        .add_filter(|v: Vec<String>| {
            println!("[Filter] Trim");
            v.into_iter().map(|s| s.trim().to_string()).collect()
        })
        .add_filter(|v| {
            println!("[Filter] Remove empty");
            v.into_iter().filter(|s| !s.is_empty()).collect()
        })
        .add_filter(|v| {
            println!("[Filter] Lowercase");
            v.into_iter().map(|s| s.to_lowercase()).collect()
        })
        .execute(texts);

    println!("Result: {:?}", processed);
}
