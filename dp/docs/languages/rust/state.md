# State Pattern - Rust Implementation

## 개요

State 패턴은 객체의 내부 상태가 변경될 때 객체가 행동을 바꿀 수 있게 해줍니다. 객체가 클래스를 바꾼 것처럼 보이게 합니다.

## 구조

```rust
pub trait VendingMachineState {
    fn insert_coin(&mut self, context: &mut VendingMachine) -> Result<String, String>;
    fn select_item(&mut self, context: &mut VendingMachine, item: &str) -> Result<String, String>;
    fn dispense(&mut self, context: &mut VendingMachine) -> Result<String, String>;
    fn get_state_name(&self) -> &str;
}

pub struct VendingMachine {
    state: Box<dyn VendingMachineState>,
    inventory: HashMap<String, u32>,
    balance: u32,
    selected_item: Option<String>,
}
```

## Rust 구현

### 1. 상태 인터페이스
```rust
pub trait VendingMachineState {
    fn insert_coin(&mut self, context: &mut VendingMachine) -> Result<String, String>;
    fn select_item(&mut self, context: &mut VendingMachine, item: &str) -> Result<String, String>;
    fn dispense(&mut self, context: &mut VendingMachine) -> Result<String, String>;
    fn get_state_name(&self) -> &str;
}
```

### 2. 구체적인 상태들
```rust
pub struct IdleState;

impl VendingMachineState for IdleState {
    fn insert_coin(&mut self, context: &mut VendingMachine) -> Result<String, String> {
        context.add_balance(100);
        context.set_state(Box::new(HasMoneyState));
        Ok("Coin inserted. Please select an item.".to_string())
    }

    fn select_item(&mut self, _context: &mut VendingMachine, _item: &str) -> Result<String, String> {
        Err("Please insert coin first".to_string())
    }

    fn dispense(&mut self, _context: &mut VendingMachine) -> Result<String, String> {
        Err("No item selected".to_string())
    }

    fn get_state_name(&self) -> &str {
        "Idle"
    }
}

pub struct HasMoneyState;

impl VendingMachineState for HasMoneyState {
    fn insert_coin(&mut self, context: &mut VendingMachine) -> Result<String, String> {
        context.add_balance(100);
        Ok("Additional coin inserted".to_string())
    }

    fn select_item(&mut self, context: &mut VendingMachine, item: &str) -> Result<String, String> {
        if let Some(&quantity) = context.inventory.get(item) {
            if quantity > 0 {
                context.selected_item = Some(item.to_string());
                context.set_state(Box::new(DispensingState));
                Ok(format!("Item '{}' selected", item))
            } else {
                Err(format!("Item '{}' is out of stock", item))
            }
        } else {
            Err(format!("Item '{}' not found", item))
        }
    }

    fn dispense(&mut self, _context: &mut VendingMachine) -> Result<String, String> {
        Err("Please select an item first".to_string())
    }

    fn get_state_name(&self) -> &str {
        "HasMoney"
    }
}
```

## 사용 예제

```rust
let mut vending_machine = VendingMachine::new();

// 동전 삽입
vending_machine.insert_coin().unwrap();

// 아이템 선택
vending_machine.select_item("Cola").unwrap();

// 아이템 배출
vending_machine.dispense().unwrap();
```

## 적용 상황

### 1. 자판기 시스템
### 2. 게임 캐릭터 상태
### 3. 네트워크 연결 상태
### 4. 주문 처리 상태
### 5. 문서 워크플로우 상태