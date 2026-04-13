// Unit of Work Pattern in Rust
// Tracks changes to entities and commits them as a single transaction

use std::collections::HashMap;

#[derive(Debug, Clone)]
struct Entity {
    id: u32,
    name: String,
    version: u32,
}

#[derive(Debug)]
enum Change {
    Insert(Entity),
    Update(Entity),
    Delete(u32),
}

struct UnitOfWork {
    changes: Vec<Change>,
    identity_map: HashMap<u32, Entity>,
}

impl UnitOfWork {
    fn new() -> Self {
        UnitOfWork {
            changes: Vec::new(),
            identity_map: HashMap::new(),
        }
    }

    fn register_new(&mut self, entity: Entity) {
        println!("[UoW] Register NEW: {:?}", entity);
        self.identity_map.insert(entity.id, entity.clone());
        self.changes.push(Change::Insert(entity));
    }

    fn register_dirty(&mut self, entity: Entity) {
        println!("[UoW] Register DIRTY: {:?}", entity);
        self.identity_map.insert(entity.id, entity.clone());
        self.changes.push(Change::Update(entity));
    }

    fn register_deleted(&mut self, id: u32) {
        println!("[UoW] Register DELETE: id={}", id);
        self.identity_map.remove(&id);
        self.changes.push(Change::Delete(id));
    }

    fn commit(&mut self) -> Result<(), String> {
        println!("\n[UoW] Committing {} changes...", self.changes.len());
        for change in &self.changes {
            match change {
                Change::Insert(e) => println!("  INSERT: {} ({})", e.name, e.id),
                Change::Update(e) => println!("  UPDATE: {} (v{})", e.name, e.version),
                Change::Delete(id) => println!("  DELETE: id={}", id),
            }
        }
        self.changes.clear();
        println!("[UoW] Committed successfully.");
        Ok(())
    }

    fn rollback(&mut self) {
        println!("[UoW] Rolling back {} changes", self.changes.len());
        self.changes.clear();
    }
}

fn main() {
    let mut uow = UnitOfWork::new();

    uow.register_new(Entity { id: 1, name: "Alice".into(), version: 1 });
    uow.register_new(Entity { id: 2, name: "Bob".into(), version: 1 });
    uow.register_dirty(Entity { id: 1, name: "Alice Updated".into(), version: 2 });
    uow.register_deleted(2);

    uow.commit().unwrap();
}
