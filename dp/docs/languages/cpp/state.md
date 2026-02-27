# State Pattern - C++ Implementation

## 개요

State 패턴은 객체의 내부 상태가 변할 때 그 동작을 변경할 수 있게 하는 행위 패턴입니다. 상태에 따른 다른 동작을 별도의 상태 클래스로 분리하여 관리합니다.

## 구조

- **Context**: 상태 객체를 참조하고 상태 변경을 처리
- **State**: 다양한 상태들의 공통 인터페이스
- **ConcreteState**: 특정 상태에서의 동작 구현

## C++ 구현

### 1. 자판기 예제

```cpp
#include <iostream>
#include <memory>
#include <string>

// Forward declaration
class VendingMachine;

// State interface
class VendingMachineState {
public:
    virtual ~VendingMachineState() = default;
    virtual void insertCoin(VendingMachine& machine) = 0;
    virtual void selectProduct(VendingMachine& machine) = 0;
    virtual void dispenseProduct(VendingMachine& machine) = 0;
    virtual std::string getStateName() const = 0;
};

// Concrete States
class IdleState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine) override;
    void selectProduct(VendingMachine& machine) override {
        std::cout << "Please insert coin first!" << std::endl;
    }
    void dispenseProduct(VendingMachine& machine) override {
        std::cout << "No product selected!" << std::endl;
    }
    std::string getStateName() const override { return "Idle"; }
};

class HasCoinState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine) override {
        std::cout << "Coin already inserted!" << std::endl;
    }
    void selectProduct(VendingMachine& machine) override;
    void dispenseProduct(VendingMachine& machine) override {
        std::cout << "Please select product first!" << std::endl;
    }
    std::string getStateName() const override { return "HasCoin"; }
};

// Context
class VendingMachine {
private:
    std::unique_ptr<VendingMachineState> currentState;
    int coinCount = 0;
    
public:
    VendingMachine() {
        currentState = std::make_unique<IdleState>();
    }
    
    void setState(std::unique_ptr<VendingMachineState> newState) {
        currentState = std::move(newState);
    }
    
    void insertCoin() {
        currentState->insertCoin(*this);
    }
    
    void selectProduct() {
        currentState->selectProduct(*this);
    }
    
    void dispenseProduct() {
        currentState->dispenseProduct(*this);
    }
    
    std::string getCurrentState() const {
        return currentState->getStateName();
    }
    
    void addCoin() { coinCount++; }
    int getCoinCount() const { return coinCount; }
    void resetCoins() { coinCount = 0; }
};
```

### 2. TCP 연결 예제

```cpp
class TCPConnection;

class TCPState {
public:
    virtual ~TCPState() = default;
    virtual void open(TCPConnection& connection) = 0;
    virtual void close(TCPConnection& connection) = 0;
    virtual void acknowledge(TCPConnection& connection) = 0;
    virtual std::string getStateName() const = 0;
};

class TCPClosed : public TCPState {
public:
    void open(TCPConnection& connection) override;
    void close(TCPConnection& connection) override {
        std::cout << "Connection already closed" << std::endl;
    }
    void acknowledge(TCPConnection& connection) override {
        std::cout << "No connection to acknowledge" << std::endl;
    }
    std::string getStateName() const override { return "Closed"; }
};

class TCPConnection {
private:
    std::unique_ptr<TCPState> state;
    
public:
    TCPConnection() {
        state = std::make_unique<TCPClosed>();
    }
    
    void changeState(std::unique_ptr<TCPState> newState) {
        state = std::move(newState);
    }
    
    void open() { state->open(*this); }
    void close() { state->close(*this); }
    void acknowledge() { state->acknowledge(*this); }
    
    std::string getState() const {
        return state->getStateName();
    }
};
```

## 사용 예제

```cpp
int main() {
    VendingMachine machine;
    
    std::cout << "Current state: " << machine.getCurrentState() << std::endl;
    
    machine.selectProduct(); // Should fail
    machine.insertCoin();
    machine.selectProduct();
    machine.dispenseProduct();
    
    return 0;
}
```

## C++의 장점

1. **스마트 포인터**: 안전한 상태 바꿈
2. **RAII**: 자동 리소스 관리
3. **타입 안전성**: 컴파일 타임 체크

## 적용 상황

1. **네트워크 프로토콜**: TCP/IP 상태 관리
2. **UI 컴포넌트**: 버튼, 입력 필드 상태
3. **게임 개발**: 캐릭터 상태, 게임 모드
4. **자판기**: 쿠인 처리 상태

State 패턴은 복잡한 상태 맨신을 깔끔하게 관리할 수 있게 해주는 유용한 패턴입니다.