// Tag Dispatch Pattern in Rust
// Use marker types (zero-sized) to select behavior at compile time

use std::marker::PhantomData;

// Tags (zero-sized types)
struct Json;
struct Xml;
struct Csv;

// Serializer parameterized by format tag
struct Serializer<Format> {
    _format: PhantomData<Format>,
}

trait Serialize {
    fn serialize(&self, data: &[(&str, &str)]) -> String;
}

impl Serialize for Serializer<Json> {
    fn serialize(&self, data: &[(&str, &str)]) -> String {
        let fields: Vec<String> = data
            .iter()
            .map(|(k, v)| format!("\"{}\":\"{}\"", k, v))
            .collect();
        format!("{{{}}}", fields.join(","))
    }
}

impl Serialize for Serializer<Xml> {
    fn serialize(&self, data: &[(&str, &str)]) -> String {
        let fields: Vec<String> = data
            .iter()
            .map(|(k, v)| format!("<{}>{}</{}>", k, v, k))
            .collect();
        format!("<record>{}</record>", fields.join(""))
    }
}

impl Serialize for Serializer<Csv> {
    fn serialize(&self, data: &[(&str, &str)]) -> String {
        let header: Vec<&str> = data.iter().map(|(k, _)| *k).collect();
        let values: Vec<&str> = data.iter().map(|(_, v)| *v).collect();
        format!("{}\n{}", header.join(","), values.join(","))
    }
}

fn make_serializer<F>() -> Serializer<F> {
    Serializer { _format: PhantomData }
}

// Tag dispatch for sorting strategy
struct Ascending;
struct Descending;

fn sort_with<T: Ord + Clone, Tag>(_tag: PhantomData<Tag>, data: &mut Vec<T>)
where
    Sorter<Tag>: SortStrategy<T>,
{
    Sorter(PhantomData).sort(data);
}

struct Sorter<Tag>(PhantomData<Tag>);

trait SortStrategy<T> {
    fn sort(&self, data: &mut Vec<T>);
}

impl<T: Ord> SortStrategy<T> for Sorter<Ascending> {
    fn sort(&self, data: &mut Vec<T>) { data.sort(); }
}

impl<T: Ord> SortStrategy<T> for Sorter<Descending> {
    fn sort(&self, data: &mut Vec<T>) { data.sort_by(|a, b| b.cmp(a)); }
}

fn main() {
    let data = &[("name", "Alice"), ("age", "30"), ("city", "Seoul")];

    println!("=== Tag Dispatch: Serialization ===");
    let json = make_serializer::<Json>();
    let xml = make_serializer::<Xml>();
    let csv = make_serializer::<Csv>();

    println!("JSON: {}", json.serialize(data));
    println!("XML:  {}", xml.serialize(data));
    println!("CSV:\n{}", csv.serialize(data));

    // Sorting with tag dispatch
    println!("\n=== Tag Dispatch: Sorting ===");
    let mut nums = vec![3, 1, 4, 1, 5, 9, 2, 6];
    sort_with::<_, Ascending>(PhantomData, &mut nums);
    println!("Ascending:  {:?}", nums);

    sort_with::<_, Descending>(PhantomData, &mut nums);
    println!("Descending: {:?}", nums);
}
