# Event Sourcing Pattern - Rust Implementation

## 개요

Event Sourcing 패턴은 애플리케이션의 상태 변경을 일련의 이벤트로 저장합니다. 현재 상태를 직접 저장하는 대신, 상태를 변경시킨 모든 이벤트들을 순서대로 저장하고, 현재 상태는 이 이벤트들을 재생하여 복원합니다.

## 구조

```rust
use std::collections::HashMap;
use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};

// Event trait
pub trait Event {
    fn event_type(&self) -> &str;
    fn aggregate_id(&self) -> &str;
    fn version(&self) -> u64;
    fn timestamp(&self) -> DateTime<Utc>;
}

// Aggregate trait
pub trait Aggregate: Clone {
    type Event: Event;

    fn apply_event(&mut self, event: &Self::Event);
    fn get_id(&self) -> &str;
    fn get_version(&self) -> u64;
}

// Account domain events
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AccountEvent {
    AccountCreated {
        account_id: String,
        owner: String,
        initial_balance: f64,
        version: u64,
        timestamp: DateTime<Utc>,
    },
    MoneyDeposited {
        account_id: String,
        amount: f64,
        version: u64,
        timestamp: DateTime<Utc>,
    },
    MoneyWithdrawn {
        account_id: String,
        amount: f64,
        version: u64,
        timestamp: DateTime<Utc>,
    },
}

impl Event for AccountEvent {
    fn event_type(&self) -> &str {
        match self {
            AccountEvent::AccountCreated { .. } => "AccountCreated",
            AccountEvent::MoneyDeposited { .. } => "MoneyDeposited",
            AccountEvent::MoneyWithdrawn { .. } => "MoneyWithdrawn",
        }
    }

    fn aggregate_id(&self) -> &str {
        match self {
            AccountEvent::AccountCreated { account_id, .. } => account_id,
            AccountEvent::MoneyDeposited { account_id, .. } => account_id,
            AccountEvent::MoneyWithdrawn { account_id, .. } => account_id,
        }
    }

    fn version(&self) -> u64 {
        match self {
            AccountEvent::AccountCreated { version, .. } => *version,
            AccountEvent::MoneyDeposited { version, .. } => *version,
            AccountEvent::MoneyWithdrawn { version, .. } => *version,
        }
    }

    fn timestamp(&self) -> DateTime<Utc> {
        match self {
            AccountEvent::AccountCreated { timestamp, .. } => *timestamp,
            AccountEvent::MoneyDeposited { timestamp, .. } => *timestamp,
            AccountEvent::MoneyWithdrawn { timestamp, .. } => *timestamp,
        }
    }
}

// Account aggregate
#[derive(Debug, Clone)]
pub struct Account {
    id: String,
    owner: String,
    balance: f64,
    version: u64,
    created_at: Option<DateTime<Utc>>,
}

impl Account {
    pub fn new() -> Self {
        Self {
            id: String::new(),
            owner: String::new(),
            balance: 0.0,
            version: 0,
            created_at: None,
        }
    }

    pub fn create(account_id: String, owner: String, initial_balance: f64) -> (Self, AccountEvent) {
        let mut account = Account::new();
        let event = AccountEvent::AccountCreated {
            account_id: account_id.clone(),
            owner: owner.clone(),
            initial_balance,
            version: 1,
            timestamp: Utc::now(),
        };

        account.apply_event(&event);
        (account, event)
    }

    pub fn deposit(&self, amount: f64) -> Result<AccountEvent, String> {
        if amount <= 0.0 {
            return Err("Deposit amount must be positive".to_string());
        }

        Ok(AccountEvent::MoneyDeposited {
            account_id: self.id.clone(),
            amount,
            version: self.version + 1,
            timestamp: Utc::now(),
        })
    }

    pub fn withdraw(&self, amount: f64) -> Result<AccountEvent, String> {
        if amount <= 0.0 {
            return Err("Withdrawal amount must be positive".to_string());
        }

        if self.balance < amount {
            return Err("Insufficient funds".to_string());
        }

        Ok(AccountEvent::MoneyWithdrawn {
            account_id: self.id.clone(),
            amount,
            version: self.version + 1,
            timestamp: Utc::now(),
        })
    }

    pub fn balance(&self) -> f64 {
        self.balance
    }

    pub fn owner(&self) -> &str {
        &self.owner
    }
}

impl Aggregate for Account {
    type Event = AccountEvent;

    fn apply_event(&mut self, event: &Self::Event) {
        match event {
            AccountEvent::AccountCreated { account_id, owner, initial_balance, version, timestamp } => {
                self.id = account_id.clone();
                self.owner = owner.clone();
                self.balance = *initial_balance;
                self.version = *version;
                self.created_at = Some(*timestamp);
            }
            AccountEvent::MoneyDeposited { amount, version, .. } => {
                self.balance += amount;
                self.version = *version;
            }
            AccountEvent::MoneyWithdrawn { amount, version, .. } => {
                self.balance -= amount;
                self.version = *version;
            }
        }
    }

    fn get_id(&self) -> &str {
        &self.id
    }

    fn get_version(&self) -> u64 {
        self.version
    }
}

// Event Store
pub trait EventStore<A: Aggregate> {
    fn save_events(&mut self, aggregate_id: &str, events: Vec<A::Event>, expected_version: u64) -> Result<(), String>;
    fn get_events(&self, aggregate_id: &str) -> Result<Vec<A::Event>, String>;
    fn get_events_from_version(&self, aggregate_id: &str, from_version: u64) -> Result<Vec<A::Event>, String>;
}

// In-memory Event Store
pub struct InMemoryEventStore {
    events: HashMap<String, Vec<AccountEvent>>,
}

impl InMemoryEventStore {
    pub fn new() -> Self {
        Self {
            events: HashMap::new(),
        }
    }
}

impl EventStore<Account> for InMemoryEventStore {
    fn save_events(&mut self, aggregate_id: &str, events: Vec<AccountEvent>, expected_version: u64) -> Result<(), String> {
        let existing_events = self.events.get(aggregate_id).map(|e| e.len()).unwrap_or(0) as u64;

        if existing_events != expected_version {
            return Err(format!("Concurrency conflict. Expected version {}, but store has {}",
                              expected_version, existing_events));
        }

        let entry = self.events.entry(aggregate_id.to_string()).or_insert_with(Vec::new);
        entry.extend(events);

        Ok(())
    }

    fn get_events(&self, aggregate_id: &str) -> Result<Vec<AccountEvent>, String> {
        Ok(self.events.get(aggregate_id).cloned().unwrap_or_default())
    }

    fn get_events_from_version(&self, aggregate_id: &str, from_version: u64) -> Result<Vec<AccountEvent>, String> {
        let events = self.get_events(aggregate_id)?;
        Ok(events.into_iter()
           .filter(|e| e.version() > from_version)
           .collect())
    }
}

// Repository using Event Store
pub struct AccountRepository<ES: EventStore<Account>> {
    event_store: ES,
}

impl<ES: EventStore<Account>> AccountRepository<ES> {
    pub fn new(event_store: ES) -> Self {
        Self { event_store }
    }

    pub fn save(&mut self, account: &Account, events: Vec<AccountEvent>) -> Result<(), String> {
        if events.is_empty() {
            return Ok(());
        }

        let expected_version = account.get_version() - events.len() as u64;
        self.event_store.save_events(account.get_id(), events, expected_version)
    }

    pub fn get_by_id(&self, id: &str) -> Result<Option<Account>, String> {
        let events = self.event_store.get_events(id)?;

        if events.is_empty() {
            return Ok(None);
        }

        let mut account = Account::new();
        for event in &events {
            account.apply_event(event);
        }

        Ok(Some(account))
    }
}

// Application Service
pub struct AccountService<ES: EventStore<Account>> {
    repository: AccountRepository<ES>,
}

impl<ES: EventStore<Account>> AccountService<ES> {
    pub fn new(event_store: ES) -> Self {
        Self {
            repository: AccountRepository::new(event_store),
        }
    }

    pub fn create_account(&mut self, account_id: String, owner: String, initial_balance: f64) -> Result<(), String> {
        // 계정이 이미 존재하는지 확인
        if self.repository.get_by_id(&account_id)?.is_some() {
            return Err("Account already exists".to_string());
        }

        let (account, event) = Account::create(account_id, owner, initial_balance);
        self.repository.save(&account, vec![event])
    }

    pub fn deposit(&mut self, account_id: &str, amount: f64) -> Result<(), String> {
        let mut account = self.repository.get_by_id(account_id)?
            .ok_or_else(|| "Account not found".to_string())?;

        let event = account.deposit(amount)?;
        account.apply_event(&event);

        self.repository.save(&account, vec![event])
    }

    pub fn withdraw(&mut self, account_id: &str, amount: f64) -> Result<(), String> {
        let mut account = self.repository.get_by_id(account_id)?
            .ok_or_else(|| "Account not found".to_string())?;

        let event = account.withdraw(amount)?;
        account.apply_event(&event);

        self.repository.save(&account, vec![event])
    }

    pub fn get_account(&self, account_id: &str) -> Result<Option<Account>, String> {
        self.repository.get_by_id(account_id)
    }
}
```

## 사용 예제

```rust
use chrono::Utc;

let event_store = InMemoryEventStore::new();
let mut account_service = AccountService::new(event_store);

// 계정 생성
account_service.create_account(
    "acc-123".to_string(),
    "John Doe".to_string(),
    1000.0
).unwrap();

// 입금
account_service.deposit("acc-123", 500.0).unwrap();

// 출금
account_service.withdraw("acc-123", 200.0).unwrap();

// 현재 상태 조회
if let Ok(Some(account)) = account_service.get_account("acc-123") {
    println!("Account Balance: {}", account.balance()); // 1300.0
    println!("Account Owner: {}", account.owner());
    println!("Account Version: {}", account.get_version());
}
```

## Event Sourcing의 장점

### 1. 완전한 감사 추적
- 모든 변경사항이 이벤트로 기록됨

### 2. 시간 여행
- 특정 시점의 상태로 복원 가능

### 3. 이벤트 기반 아키텍처
- 다른 시스템과의 통합 용이

### 4. 버그 분석
- 문제가 발생한 정확한 시점과 원인 파악 가능

## 적용 상황

### 1. 금융 시스템 (거래 내역)
### 2. 전자상거래 (주문 처리)
### 3. 게임 (플레이어 진행상황)
### 4. 협업 도구 (문서 편집 이력)
### 5. IoT 데이터 수집