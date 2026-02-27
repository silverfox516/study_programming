/// Template Method Pattern Implementation in Rust
///
/// The Template Method pattern defines the skeleton of an algorithm in a base
/// class and lets subclasses override specific steps without changing the
/// algorithm's structure. In Rust, we use traits to achieve this pattern.

use std::collections::HashMap;
use std::time::{SystemTime, UNIX_EPOCH};

/// Data processing pipeline template
pub trait DataProcessor {
    /// Template method that defines the algorithm structure
    fn process_data(&mut self, input: &str) -> Result<ProcessingResult, String> {
        println!("Starting data processing pipeline...");

        // Step 1: Validate input
        self.validate_input(input)?;

        // Step 2: Parse data
        let parsed_data = self.parse_data(input)?;

        // Step 3: Transform data
        let transformed_data = self.transform_data(parsed_data)?;

        // Step 4: Validate output
        self.validate_output(&transformed_data)?;

        // Step 5: Generate result
        let result = self.generate_result(transformed_data)?;

        println!("Data processing completed successfully");
        Ok(result)
    }

    /// Abstract methods that must be implemented by concrete classes
    fn validate_input(&self, input: &str) -> Result<(), String>;
    fn parse_data(&self, input: &str) -> Result<ParsedData, String>;
    fn transform_data(&mut self, data: ParsedData) -> Result<TransformedData, String>;
    fn validate_output(&self, output: &TransformedData) -> Result<(), String>;
    fn generate_result(&self, data: TransformedData) -> Result<ProcessingResult, String>;

    /// Hook methods with default implementations (can be overridden)
    fn pre_process_hook(&mut self) -> Result<(), String> {
        Ok(()) // Default: do nothing
    }

    fn post_process_hook(&mut self, _result: &ProcessingResult) -> Result<(), String> {
        Ok(()) // Default: do nothing
    }

    fn get_processor_name(&self) -> &str;
}

#[derive(Debug, Clone)]
pub struct ParsedData {
    pub fields: HashMap<String, String>,
    pub metadata: HashMap<String, String>,
}

#[derive(Debug, Clone)]
pub struct TransformedData {
    pub processed_fields: HashMap<String, String>,
    pub computed_values: HashMap<String, f64>,
    pub flags: Vec<String>,
}

#[derive(Debug, Clone)]
pub struct ProcessingResult {
    pub success: bool,
    pub output: String,
    pub metadata: HashMap<String, String>,
    pub processing_time_ms: u64,
    pub processor_name: String,
}

/// CSV data processor
pub struct CsvProcessor {
    delimiter: char,
    has_header: bool,
    required_columns: Vec<String>,
}

impl CsvProcessor {
    pub fn new(delimiter: char, has_header: bool, required_columns: Vec<String>) -> Self {
        CsvProcessor {
            delimiter,
            has_header,
            required_columns,
        }
    }
}

impl DataProcessor for CsvProcessor {
    fn validate_input(&self, input: &str) -> Result<(), String> {
        if input.is_empty() {
            return Err("Input cannot be empty".to_string());
        }

        if !input.contains(self.delimiter) {
            return Err(format!("Input must contain delimiter '{}'", self.delimiter));
        }

        Ok(())
    }

    fn parse_data(&self, input: &str) -> Result<ParsedData, String> {
        let lines: Vec<&str> = input.trim().lines().collect();
        if lines.is_empty() {
            return Err("No data lines found".to_string());
        }

        let mut fields = HashMap::new();
        let mut metadata = HashMap::new();

        let data_start = if self.has_header { 1 } else { 0 };
        let header: Vec<String> = if self.has_header {
            lines[0].split(self.delimiter).map(|s| s.trim().to_string()).collect()
        } else {
            (0..lines[0].split(self.delimiter).count())
                .map(|i| format!("column_{}", i))
                .collect()
        };

        // Check required columns
        for required_col in &self.required_columns {
            if !header.contains(required_col) {
                return Err(format!("Required column '{}' not found", required_col));
            }
        }

        // Parse data rows
        for (row_idx, line) in lines.iter().skip(data_start).enumerate() {
            let values: Vec<&str> = line.split(self.delimiter).collect();

            if values.len() != header.len() {
                return Err(format!(
                    "Row {} has {} columns, expected {}",
                    row_idx + 1,
                    values.len(),
                    header.len()
                ));
            }

            for (col_idx, value) in values.iter().enumerate() {
                let field_key = format!("{}_{}", header[col_idx], row_idx);
                fields.insert(field_key, value.trim().to_string());
            }
        }

        metadata.insert("total_rows".to_string(), (lines.len() - data_start).to_string());
        metadata.insert("total_columns".to_string(), header.len().to_string());
        metadata.insert("has_header".to_string(), self.has_header.to_string());

        Ok(ParsedData { fields, metadata })
    }

    fn transform_data(&mut self, data: ParsedData) -> Result<TransformedData, String> {
        let mut processed_fields = HashMap::new();
        let mut computed_values = HashMap::new();
        let mut flags = Vec::new();

        // Process each field
        for (key, value) in data.fields {
            // Convert to uppercase and trim
            let processed_value = value.to_uppercase().trim().to_string();
            processed_fields.insert(key.clone(), processed_value.clone());

            // Try to parse as number for statistics
            if let Ok(num_value) = processed_value.parse::<f64>() {
                computed_values.insert(format!("{}_numeric", key), num_value);
            }

            // Add flags for special conditions
            if processed_value.is_empty() {
                flags.push(format!("empty_field_{}", key));
            }
            if processed_value.len() > 100 {
                flags.push(format!("long_field_{}", key));
            }
        }

        // Compute summary statistics
        if !computed_values.is_empty() {
            let sum: f64 = computed_values.values().sum();
            let count = computed_values.len() as f64;
            computed_values.insert("total_sum".to_string(), sum);
            computed_values.insert("average".to_string(), sum / count);
        }

        Ok(TransformedData {
            processed_fields,
            computed_values,
            flags,
        })
    }

    fn validate_output(&self, output: &TransformedData) -> Result<(), String> {
        if output.processed_fields.is_empty() {
            return Err("No processed fields found".to_string());
        }

        // Check for required fields after processing
        for required_col in &self.required_columns {
            let found = output.processed_fields.keys()
                .any(|key| key.starts_with(&format!("{}_", required_col)));

            if !found {
                return Err(format!("Required column '{}' missing from output", required_col));
            }
        }

        Ok(())
    }

    fn generate_result(&self, data: TransformedData) -> Result<ProcessingResult, String> {
        let mut output = String::new();
        output.push_str("CSV Processing Results:\n");
        output.push_str(&format!("Processed {} fields\n", data.processed_fields.len()));
        output.push_str(&format!("Computed {} numeric values\n", data.computed_values.len()));
        output.push_str(&format!("Generated {} flags\n", data.flags.len()));

        if let Some(average) = data.computed_values.get("average") {
            output.push_str(&format!("Average numeric value: {:.2}\n", average));
        }

        let mut metadata = HashMap::new();
        metadata.insert("processed_fields_count".to_string(), data.processed_fields.len().to_string());
        metadata.insert("flags".to_string(), data.flags.join(","));

        Ok(ProcessingResult {
            success: true,
            output,
            metadata,
            processing_time_ms: 0, // Will be set by calling code
            processor_name: self.get_processor_name().to_string(),
        })
    }

    fn get_processor_name(&self) -> &str {
        "CSV Processor"
    }
}

/// JSON data processor
pub struct JsonProcessor {
    required_keys: Vec<String>,
    validate_types: bool,
}

impl JsonProcessor {
    pub fn new(required_keys: Vec<String>, validate_types: bool) -> Self {
        JsonProcessor {
            required_keys,
            validate_types,
        }
    }
}

impl DataProcessor for JsonProcessor {
    fn validate_input(&self, input: &str) -> Result<(), String> {
        if input.is_empty() {
            return Err("Input cannot be empty".to_string());
        }

        let trimmed = input.trim();
        if !trimmed.starts_with('{') || !trimmed.ends_with('}') {
            return Err("Input must be a valid JSON object".to_string());
        }

        Ok(())
    }

    fn parse_data(&self, input: &str) -> Result<ParsedData, String> {
        // Simplified JSON parsing (in real implementation, use serde_json)
        let content = input.trim();
        let inner = &content[1..content.len()-1]; // Remove { }

        let mut fields = HashMap::new();
        let mut metadata = HashMap::new();

        // Simple key-value parsing (assumes no nested objects)
        for pair in inner.split(',') {
            let parts: Vec<&str> = pair.split(':').collect();
            if parts.len() != 2 {
                return Err(format!("Invalid JSON pair: {}", pair));
            }

            let key = parts[0].trim().trim_matches('"');
            let value = parts[1].trim().trim_matches('"');

            fields.insert(key.to_string(), value.to_string());
        }

        // Check for required keys
        for required_key in &self.required_keys {
            if !fields.contains_key(required_key) {
                return Err(format!("Required key '{}' not found", required_key));
            }
        }

        metadata.insert("total_keys".to_string(), fields.len().to_string());
        metadata.insert("format".to_string(), "JSON".to_string());

        Ok(ParsedData { fields, metadata })
    }

    fn transform_data(&mut self, data: ParsedData) -> Result<TransformedData, String> {
        let mut processed_fields = HashMap::new();
        let mut computed_values = HashMap::new();
        let mut flags = Vec::new();

        for (key, value) in data.fields {
            // Process string values
            let processed_value = value.trim().to_string();
            processed_fields.insert(key.clone(), processed_value.clone());

            // Type validation and conversion
            if self.validate_types {
                // Try to parse as different types
                if let Ok(num_value) = processed_value.parse::<f64>() {
                    computed_values.insert(format!("{}_as_number", key), num_value);
                    flags.push(format!("numeric_{}", key));
                } else if processed_value.parse::<bool>().is_ok() {
                    flags.push(format!("boolean_{}", key));
                } else {
                    flags.push(format!("string_{}", key));
                }
            }

            // Validate data quality
            if processed_value.is_empty() {
                flags.push(format!("empty_{}", key));
            }
            if processed_value.len() > 255 {
                flags.push(format!("long_value_{}", key));
            }
        }

        // Compute statistics for numeric values
        if !computed_values.is_empty() {
            let values: Vec<f64> = computed_values.values().cloned().collect();
            let sum: f64 = values.iter().sum();
            let count = values.len() as f64;

            computed_values.insert("sum".to_string(), sum);
            computed_values.insert("mean".to_string(), sum / count);

            if let (Some(min), Some(max)) = (
                values.iter().fold(None, |acc, &x| Some(acc.map_or(x, |y| f64::min(y, x)))),
                values.iter().fold(None, |acc, &x| Some(acc.map_or(x, |y| f64::max(y, x))))
            ) {
                computed_values.insert("min".to_string(), min);
                computed_values.insert("max".to_string(), max);
            }
        }

        Ok(TransformedData {
            processed_fields,
            computed_values,
            flags,
        })
    }

    fn validate_output(&self, output: &TransformedData) -> Result<(), String> {
        if output.processed_fields.is_empty() {
            return Err("No processed fields found".to_string());
        }

        // Validate that required keys are present in output
        for required_key in &self.required_keys {
            if !output.processed_fields.contains_key(required_key) {
                return Err(format!("Required key '{}' missing from output", required_key));
            }
        }

        Ok(())
    }

    fn generate_result(&self, data: TransformedData) -> Result<ProcessingResult, String> {
        let mut output = String::new();
        output.push_str("JSON Processing Results:\n");
        output.push_str(&format!("Processed {} fields\n", data.processed_fields.len()));

        if self.validate_types {
            let numeric_count = data.flags.iter().filter(|f| f.starts_with("numeric_")).count();
            let boolean_count = data.flags.iter().filter(|f| f.starts_with("boolean_")).count();
            let string_count = data.flags.iter().filter(|f| f.starts_with("string_")).count();

            output.push_str(&format!("Type distribution: {} numeric, {} boolean, {} string\n",
                            numeric_count, boolean_count, string_count));
        }

        if let Some(mean) = data.computed_values.get("mean") {
            output.push_str(&format!("Mean of numeric values: {:.2}\n", mean));
        }

        let mut metadata = HashMap::new();
        metadata.insert("field_count".to_string(), data.processed_fields.len().to_string());
        metadata.insert("type_validation".to_string(), self.validate_types.to_string());

        Ok(ProcessingResult {
            success: true,
            output,
            metadata,
            processing_time_ms: 0,
            processor_name: self.get_processor_name().to_string(),
        })
    }

    fn get_processor_name(&self) -> &str {
        "JSON Processor"
    }
}

/// Abstract document generator template
pub trait DocumentGenerator {
    /// Template method for document generation
    fn generate_document(&mut self, content: &DocumentContent) -> Result<String, String> {
        let start_time = SystemTime::now();

        // Template method steps
        let header = self.generate_header(content)?;
        let body = self.generate_body(content)?;
        let footer = self.generate_footer(content)?;

        let document = self.assemble_document(header, body, footer)?;
        let formatted_document = self.format_document(document)?;

        // Hook for post-processing
        let final_document = self.post_process_document(formatted_document)?;

        let processing_time = start_time.elapsed().unwrap().as_millis();
        println!("Document generated in {}ms using {}", processing_time, self.get_generator_name());

        Ok(final_document)
    }

    /// Abstract methods
    fn generate_header(&self, content: &DocumentContent) -> Result<String, String>;
    fn generate_body(&self, content: &DocumentContent) -> Result<String, String>;
    fn generate_footer(&self, content: &DocumentContent) -> Result<String, String>;
    fn format_document(&self, document: String) -> Result<String, String>;
    fn get_generator_name(&self) -> &str;

    /// Concrete methods with default implementation
    fn assemble_document(&self, header: String, body: String, footer: String) -> Result<String, String> {
        Ok(format!("{}\n\n{}\n\n{}", header, body, footer))
    }

    /// Hook method with default implementation
    fn post_process_document(&self, document: String) -> Result<String, String> {
        Ok(document) // Default: no post-processing
    }
}

#[derive(Debug, Clone)]
pub struct DocumentContent {
    pub title: String,
    pub author: String,
    pub sections: Vec<DocumentSection>,
    pub metadata: HashMap<String, String>,
}

#[derive(Debug, Clone)]
pub struct DocumentSection {
    pub title: String,
    pub content: String,
    pub level: u32,
}

impl DocumentContent {
    pub fn new(title: &str, author: &str) -> Self {
        DocumentContent {
            title: title.to_string(),
            author: author.to_string(),
            sections: Vec::new(),
            metadata: HashMap::new(),
        }
    }

    pub fn add_section(&mut self, title: &str, content: &str, level: u32) {
        self.sections.push(DocumentSection {
            title: title.to_string(),
            content: content.to_string(),
            level,
        });
    }

    pub fn add_metadata(&mut self, key: &str, value: &str) {
        self.metadata.insert(key.to_string(), value.to_string());
    }
}

/// HTML document generator
pub struct HtmlGenerator {
    include_css: bool,
    css_styles: String,
}

impl HtmlGenerator {
    pub fn new(include_css: bool) -> Self {
        let css_styles = if include_css {
            "body { font-family: Arial, sans-serif; margin: 40px; }\n\
             h1 { color: #333; border-bottom: 2px solid #333; }\n\
             h2 { color: #666; }\n\
             h3 { color: #999; }\n\
             .metadata { font-style: italic; color: #777; }".to_string()
        } else {
            String::new()
        };

        HtmlGenerator {
            include_css,
            css_styles,
        }
    }
}

impl DocumentGenerator for HtmlGenerator {
    fn generate_header(&self, content: &DocumentContent) -> Result<String, String> {
        let mut header = String::new();
        header.push_str("<!DOCTYPE html>\n<html>\n<head>\n");
        header.push_str(&format!("  <title>{}</title>\n", content.title));

        if self.include_css {
            header.push_str("  <style>\n");
            header.push_str(&self.css_styles);
            header.push_str("\n  </style>\n");
        }

        header.push_str("</head>\n<body>\n");
        header.push_str(&format!("  <h1>{}</h1>\n", content.title));
        header.push_str(&format!("  <p class=\"metadata\">By: {}</p>\n", content.author));

        Ok(header)
    }

    fn generate_body(&self, content: &DocumentContent) -> Result<String, String> {
        let mut body = String::new();

        for section in &content.sections {
            let heading_tag = format!("h{}", (section.level + 1).min(6));
            body.push_str(&format!("  <{}>{}</{}>\n", heading_tag, section.title, heading_tag));
            body.push_str(&format!("  <p>{}</p>\n", section.content));
        }

        // Add metadata if present
        if !content.metadata.is_empty() {
            body.push_str("  <div class=\"metadata\">\n");
            body.push_str("    <h3>Metadata</h3>\n");
            body.push_str("    <ul>\n");
            for (key, value) in &content.metadata {
                body.push_str(&format!("      <li><strong>{}:</strong> {}</li>\n", key, value));
            }
            body.push_str("    </ul>\n");
            body.push_str("  </div>\n");
        }

        Ok(body)
    }

    fn generate_footer(&self, _content: &DocumentContent) -> Result<String, String> {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();

        Ok(format!(
            "  <footer class=\"metadata\">\n    <p>Generated on {}</p>\n  </footer>\n</body>\n</html>",
            timestamp
        ))
    }

    fn format_document(&self, document: String) -> Result<String, String> {
        // HTML documents are already formatted during generation
        Ok(document)
    }

    fn get_generator_name(&self) -> &str {
        "HTML Generator"
    }
}

/// Markdown document generator
pub struct MarkdownGenerator {
    include_toc: bool,
}

impl MarkdownGenerator {
    pub fn new(include_toc: bool) -> Self {
        MarkdownGenerator { include_toc }
    }

    fn generate_toc(&self, content: &DocumentContent) -> String {
        if !self.include_toc || content.sections.is_empty() {
            return String::new();
        }

        let mut toc = String::new();
        toc.push_str("## Table of Contents\n\n");

        for section in &content.sections {
            let indent = "  ".repeat(section.level as usize);
            let link = section.title.to_lowercase().replace(' ', "-");
            toc.push_str(&format!("{}* [{}](#{})\n", indent, section.title, link));
        }

        toc.push_str("\n");
        toc
    }
}

impl DocumentGenerator for MarkdownGenerator {
    fn generate_header(&self, content: &DocumentContent) -> Result<String, String> {
        let mut header = String::new();
        header.push_str(&format!("# {}\n\n", content.title));
        header.push_str(&format!("*By: {}*\n\n", content.author));

        if self.include_toc {
            header.push_str(&self.generate_toc(content));
        }

        Ok(header)
    }

    fn generate_body(&self, content: &DocumentContent) -> Result<String, String> {
        let mut body = String::new();

        for section in &content.sections {
            let heading_prefix = "#".repeat((section.level + 2) as usize);
            body.push_str(&format!("{} {}\n\n", heading_prefix, section.title));
            body.push_str(&format!("{}\n\n", section.content));
        }

        // Add metadata section
        if !content.metadata.is_empty() {
            body.push_str("## Metadata\n\n");
            for (key, value) in &content.metadata {
                body.push_str(&format!("**{}:** {}\n\n", key, value));
            }
        }

        Ok(body)
    }

    fn generate_footer(&self, _content: &DocumentContent) -> Result<String, String> {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();

        Ok(format!("---\n*Generated on {}*\n", timestamp))
    }

    fn format_document(&self, document: String) -> Result<String, String> {
        // Ensure proper spacing between sections
        let formatted = document.replace("\n\n\n", "\n\n");
        Ok(formatted)
    }

    fn post_process_document(&self, document: String) -> Result<String, String> {
        // Add word count as post-processing
        let word_count = document.split_whitespace().count();
        Ok(format!("{}\n\n*Word count: {}*\n", document, word_count))
    }

    fn get_generator_name(&self) -> &str {
        "Markdown Generator"
    }
}

/// Hook-based template with multiple extension points
pub trait ConfigurableProcessor {
    fn process(&mut self, data: &str) -> Result<String, String> {
        self.before_processing()?;

        let validated_data = self.validate_and_prepare(data)?;

        self.before_main_processing(&validated_data)?;
        let result = self.main_processing(validated_data)?;
        self.after_main_processing(&result)?;

        let final_result = self.finalize_result(result)?;

        self.after_processing(&final_result)?;

        Ok(final_result)
    }

    // Abstract methods
    fn main_processing(&mut self, data: String) -> Result<String, String>;

    // Concrete methods with default implementations
    fn validate_and_prepare(&self, data: &str) -> Result<String, String> {
        if data.is_empty() {
            Err("Input data is empty".to_string())
        } else {
            Ok(data.to_string())
        }
    }

    fn finalize_result(&self, result: String) -> Result<String, String> {
        Ok(result)
    }

    // Hook methods (empty by default)
    fn before_processing(&mut self) -> Result<(), String> { Ok(()) }
    fn before_main_processing(&self, _data: &str) -> Result<(), String> { Ok(()) }
    fn after_main_processing(&self, _result: &str) -> Result<(), String> { Ok(()) }
    fn after_processing(&self, _result: &str) -> Result<(), String> { Ok(()) }
}

pub struct LoggingProcessor {
    log_entries: Vec<String>,
}

impl LoggingProcessor {
    pub fn new() -> Self {
        LoggingProcessor {
            log_entries: Vec::new(),
        }
    }

    pub fn get_logs(&self) -> &[String] {
        &self.log_entries
    }

    fn log(&mut self, message: &str) {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_millis();
        self.log_entries.push(format!("[{}] {}", timestamp, message));
    }
}

impl Default for LoggingProcessor {
    fn default() -> Self {
        Self::new()
    }
}

impl ConfigurableProcessor for LoggingProcessor {
    fn main_processing(&mut self, data: String) -> Result<String, String> {
        self.log("Starting main processing");

        // Simple transformation: convert to uppercase and add processing info
        let processed = format!("PROCESSED: {}", data.to_uppercase());

        self.log(&format!("Processed {} characters", data.len()));

        Ok(processed)
    }

    fn before_processing(&mut self) -> Result<(), String> {
        self.log("Processing started");
        Ok(())
    }

    fn before_main_processing(&self, data: &str) -> Result<(), String> {
        println!("About to process {} characters", data.len());
        Ok(())
    }

    fn after_main_processing(&self, result: &str) -> Result<(), String> {
        println!("Main processing completed, result length: {}", result.len());
        Ok(())
    }

    fn after_processing(&self, result: &str) -> Result<(), String> {
        println!("Processing completed successfully");
        println!("Final result preview: {}...", &result[..result.len().min(50)]);
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_csv_processor() {
        let mut processor = CsvProcessor::new(',', true, vec!["name".to_string()]);
        let input = "name,age,city\nJohn,30,NYC\nJane,25,LA";

        let result = processor.process_data(input);
        assert!(result.is_ok());

        let result = result.unwrap();
        assert!(result.success);
        assert!(result.output.contains("CSV Processing Results"));
    }

    #[test]
    fn test_json_processor() {
        let mut processor = JsonProcessor::new(vec!["name".to_string()], true);
        let input = r#"{"name": "John", "age": "30", "active": "true"}"#;

        let result = processor.process_data(input);
        assert!(result.is_ok());

        let result = result.unwrap();
        assert!(result.success);
        assert!(result.output.contains("JSON Processing Results"));
    }

    #[test]
    fn test_html_generator() {
        let mut generator = HtmlGenerator::new(true);
        let mut content = DocumentContent::new("Test Document", "John Doe");
        content.add_section("Introduction", "This is the introduction.", 1);
        content.add_metadata("version", "1.0");

        let result = generator.generate_document(&content);
        assert!(result.is_ok());

        let html = result.unwrap();
        assert!(html.contains("<html>"));
        assert!(html.contains("Test Document"));
        assert!(html.contains("John Doe"));
    }

    #[test]
    fn test_markdown_generator() {
        let mut generator = MarkdownGenerator::new(true);
        let mut content = DocumentContent::new("Test Document", "Jane Doe");
        content.add_section("Overview", "This is an overview.", 1);

        let result = generator.generate_document(&content);
        assert!(result.is_ok());

        let markdown = result.unwrap();
        assert!(markdown.contains("# Test Document"));
        assert!(markdown.contains("Jane Doe"));
        assert!(markdown.contains("## Table of Contents"));
    }

    #[test]
    fn test_logging_processor() {
        let mut processor = LoggingProcessor::new();
        let result = processor.process("hello world");

        assert!(result.is_ok());
        assert!(result.unwrap().contains("PROCESSED: HELLO WORLD"));
        assert!(!processor.get_logs().is_empty());
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Template Method Pattern Demo ===");

    // Data Processing Pipeline
    println!("\n1. Data Processing Pipeline:");

    // CSV Processing
    println!("\nCSV Data Processing:");
    let mut csv_processor = CsvProcessor::new(',', true, vec!["name".to_string(), "age".to_string()]);
    let csv_input = "name,age,city,salary\nJohn,30,NYC,75000\nJane,25,LA,65000\nBob,35,Chicago,80000";

    match csv_processor.process_data(csv_input) {
        Ok(result) => {
            println!("✅ {}", result.output);
            println!("Metadata: {:?}", result.metadata);
        }
        Err(error) => println!("❌ CSV processing failed: {}", error),
    }

    // JSON Processing
    println!("\nJSON Data Processing:");
    let mut json_processor = JsonProcessor::new(vec!["name".to_string()], true);
    let json_input = r#"{"name": "Alice", "age": "28", "department": "Engineering", "salary": "90000", "active": "true"}"#;

    match json_processor.process_data(json_input) {
        Ok(result) => {
            println!("✅ {}", result.output);
            println!("Processor: {}", result.processor_name);
        }
        Err(error) => println!("❌ JSON processing failed: {}", error),
    }

    // Document Generation
    println!("\n2. Document Generation:");

    // Create document content
    let mut content = DocumentContent::new("Software Design Patterns", "Claude Assistant");
    content.add_section("Introduction", "Design patterns are reusable solutions to commonly occurring problems in software design.", 1);
    content.add_section("Template Method", "The Template Method pattern defines the skeleton of an algorithm.", 2);
    content.add_section("Benefits", "This pattern promotes code reuse and provides a clear structure.", 2);
    content.add_section("Implementation", "In Rust, we use traits to implement the Template Method pattern.", 2);
    content.add_metadata("version", "1.0");
    content.add_metadata("language", "Rust");
    content.add_metadata("patterns_covered", "Template Method");

    // HTML Generation
    println!("\nHTML Document Generation:");
    let mut html_generator = HtmlGenerator::new(true);
    match html_generator.generate_document(&content) {
        Ok(html_doc) => {
            println!("✅ HTML document generated ({} characters)", html_doc.len());
            println!("Preview:");
            println!("{}", &html_doc[..html_doc.len().min(300)]);
            println!("...");
        }
        Err(error) => println!("❌ HTML generation failed: {}", error),
    }

    // Markdown Generation
    println!("\nMarkdown Document Generation:");
    let mut md_generator = MarkdownGenerator::new(true);
    match md_generator.generate_document(&content) {
        Ok(md_doc) => {
            println!("✅ Markdown document generated ({} characters)", md_doc.len());
            println!("Preview:");
            println!("{}", &md_doc[..md_doc.len().min(400)]);
            println!("...");
        }
        Err(error) => println!("❌ Markdown generation failed: {}", error),
    }

    // Configurable Processor with Hooks
    println!("\n3. Configurable Processor with Hooks:");
    let mut logging_processor = LoggingProcessor::new();

    match logging_processor.process("Hello Template Method Pattern!") {
        Ok(result) => {
            println!("✅ Processing result: {}", result);
            println!("\nProcessing logs:");
            for log_entry in logging_processor.get_logs() {
                println!("  {}", log_entry);
            }
        }
        Err(error) => println!("❌ Processing failed: {}", error),
    }

    // Template Method Pattern Benefits
    println!("\n4. Template Method Pattern Benefits:");
    println!("  ✅ Defines algorithm structure in base class");
    println!("  ✅ Allows subclasses to override specific steps");
    println!("  ✅ Promotes code reuse through inheritance");
    println!("  ✅ Enforces algorithm structure while allowing customization");
    println!("  ✅ Provides hook methods for optional extensions");
    println!("  ✅ Follows the Hollywood Principle ('Don't call us, we'll call you')");
    println!("  ✅ Makes algorithms more maintainable and extensible");

    println!("\n5. Common Use Cases:");
    println!("  • Data processing pipelines");
    println!("  • Document generation systems");
    println!("  • Testing frameworks");
    println!("  • Compilation processes");
    println!("  • Game development (game loops, AI behavior)");
    println!("  • Web request handling");
    println!("  • Database ORM frameworks");
}

pub fn main() {
    demo();
}