# Flyweight Pattern - Rust Implementation

## 개요

Flyweight 패턴은 많은 수의 객체를 효율적으로 지원하기 위해 공유를 사용합니다. 객체의 내재적(intrinsic) 상태를 외재적(extrinsic) 상태와 분리하여 메모리 사용량을 최소화합니다.

## 구조

```rust
use std::collections::HashMap;
use std::rc::Rc;

// Flyweight trait
pub trait Glyph {
    fn render(&self, x: i32, y: i32, font_size: u32) -> String;
    fn get_character(&self) -> char;
}

// Concrete Flyweight
#[derive(Debug)]
pub struct CharacterGlyph {
    character: char, // 내재적 상태 (공유됨)
}

impl CharacterGlyph {
    pub fn new(character: char) -> Self {
        Self { character }
    }
}

impl Glyph for CharacterGlyph {
    fn render(&self, x: i32, y: i32, font_size: u32) -> String {
        format!("Rendering '{}' at ({}, {}) with size {}",
                self.character, x, y, font_size)
    }

    fn get_character(&self) -> char {
        self.character
    }
}

// Flyweight Factory
pub struct GlyphFactory {
    glyphs: HashMap<char, Rc<dyn Glyph>>,
}

impl GlyphFactory {
    pub fn new() -> Self {
        Self {
            glyphs: HashMap::new(),
        }
    }

    pub fn get_glyph(&mut self, character: char) -> Rc<dyn Glyph> {
        if let Some(glyph) = self.glyphs.get(&character) {
            glyph.clone()
        } else {
            let glyph: Rc<dyn Glyph> = Rc::new(CharacterGlyph::new(character));
            self.glyphs.insert(character, glyph.clone());
            println!("Created new glyph for character: '{}'", character);
            glyph
        }
    }

    pub fn get_created_glyphs_count(&self) -> usize {
        self.glyphs.len()
    }
}

// Context - 외재적 상태를 관리
pub struct TextDocument {
    characters: Vec<DocumentCharacter>,
    factory: GlyphFactory,
}

pub struct DocumentCharacter {
    glyph: Rc<dyn Glyph>,
    x: i32,        // 외재적 상태
    y: i32,        // 외재적 상태
    font_size: u32, // 외재적 상태
}

impl TextDocument {
    pub fn new() -> Self {
        Self {
            characters: Vec::new(),
            factory: GlyphFactory::new(),
        }
    }

    pub fn add_character(&mut self, ch: char, x: i32, y: i32, font_size: u32) {
        let glyph = self.factory.get_glyph(ch);
        let doc_char = DocumentCharacter {
            glyph,
            x,
            y,
            font_size,
        };
        self.characters.push(doc_char);
    }

    pub fn render(&self) -> Vec<String> {
        self.characters
            .iter()
            .map(|char| char.glyph.render(char.x, char.y, char.font_size))
            .collect()
    }

    pub fn get_total_characters(&self) -> usize {
        self.characters.len()
    }

    pub fn get_unique_glyphs(&self) -> usize {
        self.factory.get_created_glyphs_count()
    }
}
```

## 사용 예제

```rust
let mut document = TextDocument::new();

// 같은 문자들이 여러 번 사용됨
let text = "Hello World! Hello Rust!";
let mut x = 0;
let y = 100;

for ch in text.chars() {
    document.add_character(ch, x, y, 12);
    x += 10; // 다음 문자 위치
}

println!("Total characters: {}", document.get_total_characters()); // 24
println!("Unique glyphs: {}", document.get_unique_glyphs());      // 훨씬 적은 수

let rendered = document.render();
for line in rendered {
    println!("{}", line);
}
```

## 적용 상황

### 1. 텍스트 에디터의 문자 렌더링
### 2. 게임의 타일/스프라이트 시스템
### 3. 웹 브라우저의 DOM 요소
### 4. 그래픽 라이브러리의 도형 객체
### 5. 캐시 시스템의 공유 데이터