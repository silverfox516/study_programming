# Template Method Pattern

## 개요
Template Method 패턴은 상위 클래스에서 알고리즘의 골격을 정의하고, 하위 클래스에서 알고리즘의 구체적인 단계를 구현하는 행위 패턴입니다.

## C 언어 구현
```c
typedef struct DataProcessor {
    void (*read_data)(struct DataProcessor* self);
    void (*process_data)(struct DataProcessor* self);
    void (*write_data)(struct DataProcessor* self);
    void (*template_method)(struct DataProcessor* self);  // 템플릿 메소드
} DataProcessor;

// 템플릿 메소드 - 알고리즘의 골격 정의
void data_processor_template_method(DataProcessor* self) {
    printf("=== 데이터 처리 시작 ===\n");
    self->read_data(self);
    self->process_data(self);
    self->write_data(self);
    printf("=== 데이터 처리 완료 ===\n\n");
}

// CSV 프로세서
typedef struct CSVProcessor {
    DataProcessor base;
} CSVProcessor;

void csv_read_data(DataProcessor* self) {
    printf("CSV 파일에서 데이터 읽기\n");
}

void csv_process_data(DataProcessor* self) {
    printf("CSV 데이터 처리 (쉼표로 분리)\n");
}

void csv_write_data(DataProcessor* self) {
    printf("처리된 CSV 데이터 저장\n");
}

CSVProcessor* create_csv_processor() {
    CSVProcessor* processor = malloc(sizeof(CSVProcessor));
    processor->base.read_data = csv_read_data;
    processor->base.process_data = csv_process_data;
    processor->base.write_data = csv_write_data;
    processor->base.template_method = data_processor_template_method;
    return processor;
}

// XML 프로세서
typedef struct XMLProcessor {
    DataProcessor base;
} XMLProcessor;

void xml_read_data(DataProcessor* self) {
    printf("XML 파일에서 데이터 읽기\n");
}

void xml_process_data(DataProcessor* self) {
    printf("XML 데이터 처리 (태그 파싱)\n");
}

void xml_write_data(DataProcessor* self) {
    printf("처리된 XML 데이터 저장\n");
}

XMLProcessor* create_xml_processor() {
    XMLProcessor* processor = malloc(sizeof(XMLProcessor));
    processor->base.read_data = xml_read_data;
    processor->base.process_data = xml_process_data;
    processor->base.write_data = xml_write_data;
    processor->base.template_method = data_processor_template_method;
    return processor;
}
```

## 사용 예제
```c
DataProcessor* csv_processor = (DataProcessor*)create_csv_processor();
DataProcessor* xml_processor = (DataProcessor*)create_xml_processor();

csv_processor->template_method(csv_processor);
xml_processor->template_method(xml_processor);
```

## 적용 상황
- **알고리즘 프레임워크**: 공통 알고리즘 골격과 변형 가능한 단계
- **데이터 처리 파이프라인**: 읽기-처리-쓰기의 공통 패턴
- **게임 레벨 로더**: 맵 로딩의 공통 절차
- **테스트 프레임워크**: 설정-실행-정리의 공통 패턴