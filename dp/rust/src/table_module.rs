// Table Module Pattern in Rust
// One class handles all business logic for a database table

use std::collections::HashMap;

#[derive(Debug, Clone)]
struct Row {
    fields: HashMap<String, String>,
}

struct TableModule {
    name: String,
    rows: Vec<Row>,
    next_id: u32,
}

impl TableModule {
    fn new(name: &str) -> Self {
        TableModule {
            name: name.to_string(),
            rows: Vec::new(),
            next_id: 1,
        }
    }

    fn insert(&mut self, mut fields: HashMap<String, String>) -> u32 {
        let id = self.next_id;
        self.next_id += 1;
        fields.insert("id".to_string(), id.to_string());
        println!("[{}] INSERT id={}", self.name, id);
        self.rows.push(Row { fields });
        id
    }

    fn find_by_id(&self, id: u32) -> Option<&Row> {
        let id_str = id.to_string();
        self.rows.iter().find(|r| r.fields.get("id") == Some(&id_str))
    }

    fn find_by(&self, field: &str, value: &str) -> Vec<&Row> {
        self.rows
            .iter()
            .filter(|r| r.fields.get(field).map(|v| v.as_str()) == Some(value))
            .collect()
    }

    fn update(&mut self, id: u32, field: &str, value: &str) -> bool {
        let id_str = id.to_string();
        if let Some(row) = self.rows.iter_mut().find(|r| r.fields.get("id") == Some(&id_str)) {
            row.fields.insert(field.to_string(), value.to_string());
            println!("[{}] UPDATE id={}: {}={}", self.name, id, field, value);
            true
        } else {
            false
        }
    }

    fn delete(&mut self, id: u32) -> bool {
        let id_str = id.to_string();
        let len_before = self.rows.len();
        self.rows.retain(|r| r.fields.get("id") != Some(&id_str));
        let removed = self.rows.len() < len_before;
        if removed {
            println!("[{}] DELETE id={}", self.name, id);
        }
        removed
    }

    fn count(&self) -> usize {
        self.rows.len()
    }
}

fn make_fields(pairs: &[(&str, &str)]) -> HashMap<String, String> {
    pairs.iter().map(|(k, v)| (k.to_string(), v.to_string())).collect()
}

fn main() {
    let mut users = TableModule::new("users");

    let id1 = users.insert(make_fields(&[("name", "Alice"), ("role", "admin")]));
    let id2 = users.insert(make_fields(&[("name", "Bob"), ("role", "user")]));
    let _id3 = users.insert(make_fields(&[("name", "Charlie"), ("role", "user")]));

    // Find
    if let Some(row) = users.find_by_id(id1) {
        println!("\nFound: {:?}", row.fields);
    }

    // Query
    let normal_users = users.find_by("role", "user");
    println!("\nUsers with role='user': {}", normal_users.len());

    // Update
    users.update(id2, "role", "admin");

    // Delete
    users.delete(id1);
    println!("\nTotal users: {}", users.count());
}
