# Template Method Pattern - Rust Implementation

## 개요

Template Method 패턴은 알고리즘의 구조를 정의하고 하위 클래스에서 알고리즘의 특정 단계를 재정의할 수 있게 합니다. 알고리즘의 구조는 그대로 유지하면서 특정 단계들을 서브클래스에서 재정의할 수 있습니다.

## 구조

```rust
pub trait DataProcessor {
    // 템플릿 메서드
    fn process_data(&mut self) -> Result<String, String> {
        let mut results = Vec::new();

        results.push(self.read_data()?);
        results.push(self.validate_data()?);
        results.push(self.transform_data()?);
        results.push(self.save_data()?);

        Ok(results.join(" -> "))
    }

    // 구체적인 단계들 (서브클래스에서 구현)
    fn read_data(&mut self) -> Result<String, String>;
    fn validate_data(&mut self) -> Result<String, String>;
    fn transform_data(&mut self) -> Result<String, String>;
    fn save_data(&mut self) -> Result<String, String>;
}

pub struct CsvDataProcessor {
    filename: String,
    data: Vec<String>,
}

impl DataProcessor for CsvDataProcessor {
    fn read_data(&mut self) -> Result<String, String> {
        self.data = vec!["row1,data1".to_string(), "row2,data2".to_string()];
        Ok(format!("Read CSV data from {}", self.filename))
    }

    fn validate_data(&mut self) -> Result<String, String> {
        if self.data.is_empty() {
            return Err("No data to validate".to_string());
        }
        Ok("CSV data validated".to_string())
    }

    fn transform_data(&mut self) -> Result<String, String> {
        self.data = self.data.iter()
            .map(|row| row.to_uppercase())
            .collect();
        Ok("CSV data transformed".to_string())
    }

    fn save_data(&mut self) -> Result<String, String> {
        Ok(format!("CSV data saved: {} rows", self.data.len()))
    }
}
```

## 사용 예제

```rust
let mut csv_processor = CsvDataProcessor::new("data.csv");
match csv_processor.process_data() {
    Ok(result) => println!("Processing result: {}", result),
    Err(error) => println!("Processing failed: {}", error),
}

let mut json_processor = JsonDataProcessor::new("data.json");
match json_processor.process_data() {
    Ok(result) => println!("Processing result: {}", result),
    Err(error) => println!("Processing failed: {}", error),
}
```

## 적용 상황

### 1. 데이터 처리 파이프라인
### 2. 게임 AI 행동 패턴
### 3. 웹 요청 처리
### 4. 컴파일러의 단계별 처리
### 5. 테스트 프레임워크