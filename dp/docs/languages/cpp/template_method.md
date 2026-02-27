# Template Method Pattern - C++ Implementation

## 개요

Template Method 패턴은 알고리즘의 골격을 정의하고, 하위 클래스에서 알고리즘의 특정 단계를 재정의할 수 있게 하는 행위 패턴입니다. 알고리즘의 구조는 유지하면서 특정 단계의 구현을 변경할 수 있습니다.

## 구조

- **AbstractClass**: 템플릿 메서드와 추상 단계들을 정의
- **ConcreteClass**: 추상 단계들의 구체적 구현을 제공

## C++ 구현

### 1. 데이터 처리 템플릿

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

class DataProcessor {
public:
    // Template Method
    void processData() {
        loadData();
        if (validateData()) {
            processCore();
            saveResults();
        } else {
            handleError();
        }
    }

protected:
    // Abstract methods (pure virtual)
    virtual void loadData() = 0;
    virtual bool validateData() = 0;
    virtual void processCore() = 0;
    virtual void saveResults() = 0;
    
    // Hook method (optional override)
    virtual void handleError() {
        std::cout << "Default error handling" << std::endl;
    }
    
    virtual ~DataProcessor() = default;
};

class CSVProcessor : public DataProcessor {
protected:
    void loadData() override {
        std::cout << "Loading CSV data..." << std::endl;
    }
    
    bool validateData() override {
        std::cout << "Validating CSV format..." << std::endl;
        return true;
    }
    
    void processCore() override {
        std::cout << "Processing CSV data..." << std::endl;
    }
    
    void saveResults() override {
        std::cout << "Saving CSV results..." << std::endl;
    }
};
```

### 2. 게임 캐릭터 AI 템플릿

```cpp
class AICharacter {
public:
    // Template Method
    void takeTurn() {
        if (canAct()) {
            analyzeEnvironment();
            selectAction();
            executeAction();
            updateState();
        }
    }

protected:
    virtual bool canAct() const {
        return true; // Default implementation
    }
    
    virtual void analyzeEnvironment() = 0;
    virtual void selectAction() = 0;
    virtual void executeAction() = 0;
    
    virtual void updateState() {
        std::cout << "Updating character state..." << std::endl;
    }
};

class WarriorAI : public AICharacter {
protected:
    void analyzeEnvironment() override {
        std::cout << "Warrior analyzing for nearby enemies..." << std::endl;
    }
    
    void selectAction() override {
        std::cout << "Warrior choosing melee attack..." << std::endl;
    }
    
    void executeAction() override {
        std::cout << "Warrior executing sword strike!" << std::endl;
    }
};
```

## 사용 예제

```cpp
int main() {
    auto csvProcessor = std::make_unique<CSVProcessor>();
    csvProcessor->processData();
    
    auto warrior = std::make_unique<WarriorAI>();
    warrior->takeTurn();
    
    return 0;
}
```

## C++의 장점

1. **가상 함수**: 자연스러운 다형성 지원
2. **접근 제어**: protected로 구현 세부사항 보호
3. **순수 가상 함수**: 강제 구현으로 안전성 보장

## 적용 상황

1. **프레임워크 설계**: 공통 알고리즘 골격 제공
2. **데이터 처리**: ETL 파이프라인
3. **게임 개발**: AI, 애니메이션 시스템
4. **컴파일러**: 코드 생성 단계들

Template Method 패턴은 코드 재사용성을 높이고 일관된 알고리즘 구조를 유지하는 핵심 패턴입니다.