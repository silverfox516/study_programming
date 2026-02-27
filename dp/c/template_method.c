#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Abstract Base Class - Data Processor
typedef struct DataProcessor {
    // Template method (final)
    void (*process_data)(struct DataProcessor* self, const char* input);

    // Abstract methods (must be implemented by subclasses)
    void (*read_data)(struct DataProcessor* self, const char* input);
    void (*parse_data)(struct DataProcessor* self);
    void (*validate_data)(struct DataProcessor* self);
    void (*transform_data)(struct DataProcessor* self);
    void (*save_data)(struct DataProcessor* self);

    // Hook methods (can be overridden)
    bool (*should_compress)(struct DataProcessor* self);
    void (*log_processing)(struct DataProcessor* self, const char* step);

    // Cleanup
    void (*destroy)(struct DataProcessor* self);

    // Data storage
    char* raw_data;
    char* processed_data;
    bool is_valid;
    char* processor_name;
} DataProcessor;

// Template method implementation
void data_processor_process_data(DataProcessor* self, const char* input) {
    printf("=== Starting data processing with %s ===\n", self->processor_name);

    self->log_processing(self, "Starting");

    // Step 1: Read the data
    self->read_data(self, input);
    self->log_processing(self, "Data reading");

    // Step 2: Parse the data
    self->parse_data(self);
    self->log_processing(self, "Data parsing");

    // Step 3: Validate the data
    self->validate_data(self);
    self->log_processing(self, "Data validation");

    if (!self->is_valid) {
        printf("Data validation failed. Stopping processing.\n");
        return;
    }

    // Step 4: Transform the data
    self->transform_data(self);
    self->log_processing(self, "Data transformation");

    // Step 5: Optional compression (hook method)
    if (self->should_compress(self)) {
        printf("Compressing data...\n");
    }

    // Step 6: Save the data
    self->save_data(self);
    self->log_processing(self, "Data saving");

    printf("=== Processing completed ===\n\n");
}

// Default hook implementations
bool default_should_compress(DataProcessor* self) {
    return false; // Default: no compression
}

void default_log_processing(DataProcessor* self, const char* step) {
    printf("[%s] %s completed\n", self->processor_name, step);
}

// Concrete Implementation 1 - CSV Processor
typedef struct {
    DataProcessor base;
    char** rows;
    int row_count;
    int max_rows;
} CSVProcessor;

void csv_read_data(DataProcessor* self, const char* input) {
    CSVProcessor* csv = (CSVProcessor*)self;
    printf("Reading CSV data from: %s\n", input);

    // Simulate reading CSV file
    if (self->raw_data) {
        free(self->raw_data);
    }
    self->raw_data = malloc(strlen(input) + 1);
    strcpy(self->raw_data, input);
}

void csv_parse_data(DataProcessor* self) {
    CSVProcessor* csv = (CSVProcessor*)self;
    printf("Parsing CSV data into rows and columns\n");

    // Simulate parsing (in real implementation, would split by commas and newlines)
    csv->rows = malloc(3 * sizeof(char*));
    csv->rows[0] = malloc(strlen("header1,header2,header3") + 1);
    strcpy(csv->rows[0], "header1,header2,header3");
    csv->rows[1] = malloc(strlen("value1,value2,value3") + 1);
    strcpy(csv->rows[1], "value1,value2,value3");
    csv->rows[2] = malloc(strlen("value4,value5,value6") + 1);
    strcpy(csv->rows[2], "value4,value5,value6");
    csv->row_count = 3;
    csv->max_rows = 3;
}

void csv_validate_data(DataProcessor* self) {
    CSVProcessor* csv = (CSVProcessor*)self;
    printf("Validating CSV format and data integrity\n");

    // Simple validation: check if we have data
    self->is_valid = (csv->row_count > 0);
    if (self->is_valid) {
        printf("CSV validation passed: %d rows found\n", csv->row_count);
    } else {
        printf("CSV validation failed: no data rows\n");
    }
}

void csv_transform_data(DataProcessor* self) {
    CSVProcessor* csv = (CSVProcessor*)self;
    printf("Transforming CSV data (converting to JSON format)\n");

    // Simulate transformation
    if (self->processed_data) {
        free(self->processed_data);
    }
    const char* json_data = "[\n  {\"header1\":\"value1\",\"header2\":\"value2\",\"header3\":\"value3\"},\n  {\"header1\":\"value4\",\"header2\":\"value5\",\"header3\":\"value6\"}\n]";
    self->processed_data = malloc(strlen(json_data) + 1);
    strcpy(self->processed_data, json_data);
}

void csv_save_data(DataProcessor* self) {
    printf("Saving processed CSV data to database\n");
    printf("Processed data preview:\n%s\n", self->processed_data);
}

bool csv_should_compress(DataProcessor* self) {
    // CSV files are usually small, don't compress by default
    return false;
}

void csv_destroy(DataProcessor* self) {
    CSVProcessor* csv = (CSVProcessor*)self;

    for (int i = 0; i < csv->row_count; i++) {
        if (csv->rows[i]) {
            free(csv->rows[i]);
        }
    }
    if (csv->rows) {
        free(csv->rows);
    }

    if (self->raw_data) {
        free(self->raw_data);
    }
    if (self->processed_data) {
        free(self->processed_data);
    }
    if (self->processor_name) {
        free(self->processor_name);
    }
    free(csv);
}

DataProcessor* csv_processor_create() {
    CSVProcessor* csv = malloc(sizeof(CSVProcessor));

    // Set up function pointers
    csv->base.process_data = data_processor_process_data;
    csv->base.read_data = csv_read_data;
    csv->base.parse_data = csv_parse_data;
    csv->base.validate_data = csv_validate_data;
    csv->base.transform_data = csv_transform_data;
    csv->base.save_data = csv_save_data;
    csv->base.should_compress = csv_should_compress;
    csv->base.log_processing = default_log_processing;
    csv->base.destroy = csv_destroy;

    // Initialize data
    csv->base.raw_data = NULL;
    csv->base.processed_data = NULL;
    csv->base.is_valid = false;
    csv->base.processor_name = malloc(strlen("CSVProcessor") + 1);
    strcpy(csv->base.processor_name, "CSVProcessor");

    csv->rows = NULL;
    csv->row_count = 0;
    csv->max_rows = 0;

    return (DataProcessor*)csv;
}

// Concrete Implementation 2 - XML Processor
typedef struct {
    DataProcessor base;
    char* root_element;
    int element_count;
} XMLProcessor;

void xml_read_data(DataProcessor* self, const char* input) {
    printf("Reading XML data from: %s\n", input);

    if (self->raw_data) {
        free(self->raw_data);
    }
    self->raw_data = malloc(strlen(input) + 1);
    strcpy(self->raw_data, input);
}

void xml_parse_data(DataProcessor* self) {
    XMLProcessor* xml = (XMLProcessor*)self;
    printf("Parsing XML structure and extracting elements\n");

    // Simulate XML parsing
    xml->root_element = malloc(strlen("root") + 1);
    strcpy(xml->root_element, "root");
    xml->element_count = 5;
}

void xml_validate_data(DataProcessor* self) {
    XMLProcessor* xml = (XMLProcessor*)self;
    printf("Validating XML against schema\n");

    // Simple validation
    self->is_valid = (xml->element_count > 0 && xml->root_element != NULL);
    if (self->is_valid) {
        printf("XML validation passed: %d elements found\n", xml->element_count);
    } else {
        printf("XML validation failed\n");
    }
}

void xml_transform_data(DataProcessor* self) {
    printf("Transforming XML data (flattening structure)\n");

    if (self->processed_data) {
        free(self->processed_data);
    }
    const char* flattened = "element1=value1,element2=value2,element3=value3";
    self->processed_data = malloc(strlen(flattened) + 1);
    strcpy(self->processed_data, flattened);
}

void xml_save_data(DataProcessor* self) {
    printf("Saving processed XML data to file system\n");
    printf("Processed data: %s\n", self->processed_data);
}

bool xml_should_compress(DataProcessor* self) {
    XMLProcessor* xml = (XMLProcessor*)self;
    // Compress if we have many elements
    return xml->element_count > 10;
}

void xml_log_processing(DataProcessor* self, const char* step) {
    XMLProcessor* xml = (XMLProcessor*)self;
    printf("[%s] %s completed (elements: %d)\n", self->processor_name, step, xml->element_count);
}

void xml_destroy(DataProcessor* self) {
    XMLProcessor* xml = (XMLProcessor*)self;

    if (xml->root_element) {
        free(xml->root_element);
    }
    if (self->raw_data) {
        free(self->raw_data);
    }
    if (self->processed_data) {
        free(self->processed_data);
    }
    if (self->processor_name) {
        free(self->processor_name);
    }
    free(xml);
}

DataProcessor* xml_processor_create() {
    XMLProcessor* xml = malloc(sizeof(XMLProcessor));

    // Set up function pointers
    xml->base.process_data = data_processor_process_data;
    xml->base.read_data = xml_read_data;
    xml->base.parse_data = xml_parse_data;
    xml->base.validate_data = xml_validate_data;
    xml->base.transform_data = xml_transform_data;
    xml->base.save_data = xml_save_data;
    xml->base.should_compress = xml_should_compress;
    xml->base.log_processing = xml_log_processing; // Custom logging
    xml->base.destroy = xml_destroy;

    // Initialize data
    xml->base.raw_data = NULL;
    xml->base.processed_data = NULL;
    xml->base.is_valid = false;
    xml->base.processor_name = malloc(strlen("XMLProcessor") + 1);
    strcpy(xml->base.processor_name, "XMLProcessor");

    xml->root_element = NULL;
    xml->element_count = 0;

    return (DataProcessor*)xml;
}

// Client code
int main() {
    printf("=== Template Method Pattern Demo - Data Processing ===\n\n");

    // Create processors
    DataProcessor* csv_processor = csv_processor_create();
    DataProcessor* xml_processor = xml_processor_create();

    // Process CSV data
    printf("Processing CSV file:\n");
    csv_processor->process_data(csv_processor, "data.csv");

    // Process XML data
    printf("Processing XML file:\n");
    xml_processor->process_data(xml_processor, "data.xml");

    // Demonstrate invalid data handling
    printf("Processing invalid data:\n");
    DataProcessor* csv_processor2 = csv_processor_create();
    csv_processor2->process_data(csv_processor2, "empty.csv");
    // The validation will fail and stop processing

    // Cleanup
    csv_processor->destroy(csv_processor);
    xml_processor->destroy(xml_processor);
    csv_processor2->destroy(csv_processor2);

    printf("=== Template Method Benefits ===\n");
    printf("1. Code reuse: Common algorithm structure is shared\n");
    printf("2. Flexibility: Each subclass can customize specific steps\n");
    printf("3. Control: Template method controls the overall flow\n");
    printf("4. Extension: Hook methods allow optional customization\n");

    return 0;
}