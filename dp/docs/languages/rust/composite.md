# Composite Pattern - Rust Implementation

## 개요

Composite 패턴은 객체들을 트리 구조로 구성하여 부분-전체 계층을 나타낼 수 있게 합니다. 클라이언트가 개별 객체와 객체들의 조합을 동일하게 다룰 수 있도록 해줍니다.

이 패턴은 파일 시스템, GUI 컴포넌트, 그래픽 객체 등에서 계층적 구조를 다룰 때 유용합니다. Rust에서는 trait, Rc, RefCell을 활용하여 안전하고 효율적인 트리 구조를 구현할 수 있습니다.

## 구조

```rust
// 공통 인터페이스
pub trait FileSystemComponent {
    fn name(&self) -> &str;
    fn size(&self) -> u64;
    fn display(&self, indent: usize) -> String;
    fn is_composite(&self) -> bool;

    // 조합 객체만 구현하는 메서드들
    fn add_child(&mut self, child: Rc<RefCell<dyn FileSystemComponent>>) -> Result<(), String> {
        Err("Operation not supported".to_string())
    }
    fn remove_child(&mut self, name: &str) -> Result<(), String> {
        Err("Operation not supported".to_string())
    }
}

// 잎(Leaf) 컴포넌트
pub struct File {
    name: String,
    size: u64,
    content: String,
}

// 조합(Composite) 컴포넌트
pub struct Directory {
    name: String,
    children: HashMap<String, Rc<RefCell<dyn FileSystemComponent>>>,
}
```

## Rust 구현

Rust의 Composite 구현은 다음과 같은 특징을 가집니다:

### 1. 공통 인터페이스 정의
```rust
pub trait FileSystemComponent {
    fn name(&self) -> &str;
    fn size(&self) -> u64;
    fn display(&self, indent: usize) -> String;
    fn is_composite(&self) -> bool;

    // 기본 구현으로 오류 반환 (잎 컴포넌트용)
    fn add_child(&mut self, _child: Rc<RefCell<dyn FileSystemComponent>>) -> Result<(), String> {
        Err("Operation not supported".to_string())
    }
}
```

### 2. 파일 (Leaf) 구현
```rust
impl FileSystemComponent for File {
    fn name(&self) -> &str {
        &self.name
    }

    fn size(&self) -> u64 {
        self.size
    }

    fn display(&self, indent: usize) -> String {
        format!("{}{} ({} bytes)", "  ".repeat(indent), self.name, self.size)
    }

    fn is_composite(&self) -> bool {
        false
    }
}
```

### 3. 디렉토리 (Composite) 구현
```rust
impl FileSystemComponent for Directory {
    fn name(&self) -> &str {
        &self.name
    }

    fn size(&self) -> u64 {
        // 모든 자식의 크기를 합산
        self.children
            .values()
            .map(|child| child.borrow().size())
            .sum()
    }

    fn display(&self, indent: usize) -> String {
        let mut result = format!("{}{}/", "  ".repeat(indent), self.name);

        for child in self.children.values() {
            result.push('\n');
            result.push_str(&child.borrow().display(indent + 1));
        }

        result
    }

    fn is_composite(&self) -> bool {
        true
    }

    fn add_child(&mut self, child: Rc<RefCell<dyn FileSystemComponent>>) -> Result<(), String> {
        let child_name = child.borrow().name().to_string();
        if self.children.contains_key(&child_name) {
            return Err(format!("Item '{}' already exists", child_name));
        }

        self.children.insert(child_name, child);
        Ok(())
    }
}
```

## 사용 예제

### 1. 파일 시스템 예제
```rust
let fs = FileSystemManager::new();
let root = fs.get_root();

{
    let mut root_borrowed = root.borrow_mut();

    // 파일 생성
    root_borrowed.create_file("readme.txt", "Project documentation").unwrap();
    root_borrowed.create_file("main.rs", "fn main() {}").unwrap();

    // 디렉토리 생성
    let src_dir = root_borrowed.create_directory("src").unwrap();
    let docs_dir = root_borrowed.create_directory("docs").unwrap();

    // 하위 디렉토리에 파일 추가
    {
        let mut src_borrowed = src_dir.borrow_mut();
        src_borrowed.create_file("lib.rs", "pub mod utils;").unwrap();
        src_borrowed.create_file("utils.rs", "pub fn helper() {}").unwrap();
    }
}

// 전체 구조 출력
println!("{}", fs.display_tree());
println!("Total size: {} bytes", fs.calculate_total_size());
```

### 2. 그래픽 객체 예제
```rust
pub trait Graphic {
    fn draw(&self) -> String;
    fn bounds(&self) -> (f64, f64, f64, f64);
}

// 기본 도형들
pub struct Circle {
    x: f64,
    y: f64,
    radius: f64,
}

pub struct Rectangle {
    x: f64,
    y: f64,
    width: f64,
    height: f64,
}

// 그래픽 그룹 (조합)
pub struct GraphicsGroup {
    graphics: Vec<Box<dyn Graphic>>,
}

impl Graphic for GraphicsGroup {
    fn draw(&self) -> String {
        let mut result = String::from("Group:");
        for (i, graphic) in self.graphics.iter().enumerate() {
            result.push_str(&format!("\n  {}: {}", i, graphic.draw()));
        }
        result
    }

    fn bounds(&self) -> (f64, f64, f64, f64) {
        if self.graphics.is_empty() {
            return (0.0, 0.0, 0.0, 0.0);
        }

        // 모든 자식의 경계를 계산하여 전체 경계 결정
        let first_bounds = self.graphics[0].bounds();
        let mut min_x = first_bounds.0;
        let mut min_y = first_bounds.1;
        let mut max_x = first_bounds.0 + first_bounds.2;
        let mut max_y = first_bounds.1 + first_bounds.3;

        for graphic in &self.graphics[1..] {
            let bounds = graphic.bounds();
            min_x = min_x.min(bounds.0);
            min_y = min_y.min(bounds.1);
            max_x = max_x.max(bounds.0 + bounds.2);
            max_y = max_y.max(bounds.1 + bounds.3);
        }

        (min_x, min_y, max_x - min_x, max_y - min_y)
    }
}
```

### 3. 그래픽 조합 사용
```rust
let mut main_group = GraphicsGroup::new();

// 개별 도형 추가
main_group.add(Box::new(Circle::new(10.0, 10.0, 5.0)));
main_group.add(Box::new(Rectangle::new(20.0, 20.0, 15.0, 10.0)));

// 중첩 그룹 생성
let mut sub_group = GraphicsGroup::new();
sub_group.add(Box::new(Circle::new(50.0, 50.0, 3.0)));
sub_group.add(Box::new(Rectangle::new(60.0, 45.0, 8.0, 12.0)));

// 하위 그룹을 메인 그룹에 추가
main_group.add(Box::new(sub_group));

// 전체 그룹 그리기 및 경계 계산
println!("{}", main_group.draw());
let bounds = main_group.bounds();
println!("Overall bounds: ({}, {}) {}x{}", bounds.0, bounds.1, bounds.2, bounds.3);
```

## Rust의 장점

### 1. 메모리 안전성
```rust
use std::rc::{Rc, Weak};
use std::cell::RefCell;

// 참조 카운팅과 내부 가변성으로 안전한 트리 구조
pub struct Directory {
    children: HashMap<String, Rc<RefCell<dyn FileSystemComponent>>>,
    parent: Option<Weak<RefCell<Directory>>>, // 순환 참조 방지
}
```

### 2. 타입 안전성
- 컴파일 타임에 trait 구현 검증
- 잘못된 메서드 호출 방지

### 3. 소유권 관리
```rust
// Rc로 공유 소유권, RefCell로 내부 가변성
let child = Rc::new(RefCell::new(File::new("test.txt", "content")));
directory.add_child(child.clone()).unwrap();
```

### 4. 에러 처리
```rust
// Result를 사용한 명시적 에러 처리
fn add_child(&mut self, child: Rc<RefCell<dyn FileSystemComponent>>) -> Result<(), String> {
    if self.children.contains_key(&child_name) {
        return Err(format!("Item '{}' already exists", child_name));
    }

    self.children.insert(child_name, child);
    Ok(())
}
```

### 5. 기본 구현 활용
```rust
pub trait FileSystemComponent {
    // 필수 메서드들
    fn name(&self) -> &str;
    fn size(&self) -> u64;

    // 기본 구현 (잎 컴포넌트용)
    fn add_child(&mut self, _child: Rc<RefCell<dyn FileSystemComponent>>) -> Result<(), String> {
        Err("Operation not supported".to_string())
    }
}
```

## 적용 상황

### 1. 파일 시스템
- 파일과 디렉토리의 통일된 처리
- 재귀적 크기 계산 및 탐색
- 파일 시스템 네비게이션

### 2. GUI 컴포넌트
```rust
trait Widget {
    fn render(&self) -> String;
    fn handle_event(&mut self, event: Event) -> bool;
    fn get_bounds(&self) -> Rectangle;
}

struct Panel {
    children: Vec<Box<dyn Widget>>,
}

struct Button {
    label: String,
    bounds: Rectangle,
}
```

### 3. 문서 구조
- HTML DOM 트리
- XML 문서 구조
- 마크다운 AST

### 4. 그래픽 렌더링
```rust
trait Drawable {
    fn draw(&self, context: &mut DrawContext);
    fn get_bounds(&self) -> BoundingBox;
    fn transform(&mut self, matrix: &TransformMatrix);
}

struct Scene {
    objects: Vec<Box<dyn Drawable>>,
}
```

### 5. 조직도/계층구조
```rust
trait OrganizationUnit {
    fn get_name(&self) -> &str;
    fn get_employee_count(&self) -> usize;
    fn get_budget(&self) -> f64;
}

struct Department {
    name: String,
    sub_departments: Vec<Box<dyn OrganizationUnit>>,
    employees: Vec<Employee>,
}
```

### 6. 수식 트리
```rust
trait Expression {
    fn evaluate(&self) -> f64;
    fn to_string(&self) -> String;
}

struct BinaryOperation {
    left: Box<dyn Expression>,
    right: Box<dyn Expression>,
    operator: Operator,
}

struct Constant {
    value: f64,
}
```

### 7. 메뉴 시스템
```rust
trait MenuItem {
    fn execute(&self) -> Result<(), MenuError>;
    fn get_label(&self) -> &str;
    fn is_enabled(&self) -> bool;
}

struct Menu {
    label: String,
    items: Vec<Box<dyn MenuItem>>,
}

struct MenuAction {
    label: String,
    action: Box<dyn Fn() -> Result<(), MenuError>>,
}
```

이 패턴은 Rust의 trait system과 소유권 모델과 잘 어울리며, 안전하고 효율적인 계층적 구조를 구축할 수 있게 해줍니다. 특히 Rc와 RefCell을 통한 안전한 공유 참조 관리가 큰 장점입니다.