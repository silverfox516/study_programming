# Prototype Pattern - Rust Implementation

## 개요

Prototype 패턴은 기존 객체를 복사하여 새로운 객체를 생성하는 방법을 제공합니다. Rust에서는 `Clone` trait을 통해 이 패턴을 구현할 수 있습니다.

## 구조

```rust
#[derive(Debug, Clone)]
pub struct Document {
    title: String,
    content: String,
    metadata: HashMap<String, String>,
    created_at: SystemTime,
}

impl Document {
    pub fn new(title: impl Into<String>, content: impl Into<String>) -> Self {
        Self {
            title: title.into(),
            content: content.into(),
            metadata: HashMap::new(),
            created_at: SystemTime::now(),
        }
    }

    pub fn clone_with_title(&self, new_title: impl Into<String>) -> Self {
        let mut cloned = self.clone();
        cloned.title = new_title.into();
        cloned.created_at = SystemTime::now();
        cloned
    }
}

// 프로토타입 매니저
pub struct DocumentPrototypeManager {
    prototypes: HashMap<String, Document>,
}

impl DocumentPrototypeManager {
    pub fn register(&mut self, key: String, prototype: Document) {
        self.prototypes.insert(key, prototype);
    }

    pub fn create(&self, key: &str) -> Option<Document> {
        self.prototypes.get(key).cloned()
    }
}
```

## 사용 예제

```rust
let original_doc = Document::new("Original", "Original content");

// 기본 복제
let copied_doc = original_doc.clone();

// 제목 변경하여 복제
let modified_doc = original_doc.clone_with_title("Modified Version");

// 프로토타입 매니저 사용
let mut manager = DocumentPrototypeManager::new();
manager.register("template".to_string(), original_doc);

if let Some(new_doc) = manager.create("template") {
    println!("Created document: {}", new_doc.title());
}
```

## 적용 상황

### 1. 복잡한 객체 복제
### 2. 설정 템플릿
### 3. 게임 오브젝트 복제
### 4. 문서 템플릿
### 5. 캐시된 객체 복사