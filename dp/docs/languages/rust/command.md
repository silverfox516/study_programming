# Command Pattern - Rust Implementation

## 개요

Command 패턴은 요청을 객체로 캡슐화하여 클라이언트를 서로 다른 요청, 큐, 로그 요청으로 매개변수화하고 취소 가능한 연산을 지원할 수 있게 합니다.

이 패턴은 요청을 보내는 객체와 요청을 처리하는 객체를 분리하며, 작업을 수행하는 데 필요한 모든 정보를 하나의 객체에 저장합니다. Rust에서는 trait과 소유권 시스템을 활용하여 안전하고 효율적인 구현이 가능합니다.

## 구조

```rust
pub trait Command: std::fmt::Debug {
    fn execute(&mut self) -> Result<String, String>;
    fn undo(&mut self) -> Result<String, String>;
    fn get_description(&self) -> String;
    fn can_undo(&self) -> bool;
}

// 명령 관리자 (Invoker)
pub struct CommandManager {
    history: VecDeque<Box<dyn Command>>,
    undo_stack: Vec<Box<dyn Command>>,
    max_history: usize,
}

// 구체적인 명령
pub struct InsertCommand {
    editor: Option<*mut TextEditor>,
    text: String,
    position: usize,
    executed: bool,
}
```

## Rust 구현

Rust의 Command 구현은 다음과 같은 특징을 가집니다:

### 1. 타입 안전한 명령 인터페이스
```rust
pub trait Command: std::fmt::Debug {
    fn execute(&mut self) -> Result<String, String>;
    fn undo(&mut self) -> Result<String, String>;
    fn get_description(&self) -> String;
    fn can_undo(&self) -> bool;
}
```

### 2. 텍스트 에디터 명령 구현
```rust
impl Command for InsertCommand {
    fn execute(&mut self) -> Result<String, String> {
        if self.executed {
            return Err("Command already executed".to_string());
        }

        if let Some(editor) = self.get_editor() {
            editor.move_cursor(self.position);
            let result = editor.insert_text(&self.text);
            self.executed = true;
            Ok(result)
        } else {
            Err("No editor available".to_string())
        }
    }

    fn undo(&mut self) -> Result<String, String> {
        if !self.executed {
            return Err("Command not executed".to_string());
        }

        if let Some(editor) = self.get_editor() {
            editor.move_cursor(self.position + self.text.len());
            let (_, result) = editor.delete_text(self.text.len());
            self.executed = false;
            Ok(format!("Undone: {}", result))
        } else {
            Err("No editor available".to_string())
        }
    }
}
```

### 3. 매크로 명령 (Composite Command)
```rust
pub struct MacroCommand {
    commands: Vec<Box<dyn Command>>,
    description: String,
    executed: bool,
}

impl Command for MacroCommand {
    fn execute(&mut self) -> Result<String, String> {
        let mut results = Vec::new();
        for command in &mut self.commands {
            match command.execute() {
                Ok(result) => results.push(result),
                Err(e) => {
                    // 실패 시 이전 명령들 취소
                    for prev_command in self.commands.iter_mut().rev() {
                        if prev_command.can_undo() {
                            let _ = prev_command.undo();
                        }
                    }
                    return Err(format!("Macro failed: {}", e));
                }
            }
        }

        self.executed = true;
        Ok(format!("Executed macro: {}", results.join("; ")))
    }
}
```

## 사용 예제

### 1. 텍스트 에디터 명령
```rust
let mut editor = TextEditor::new();
let mut manager = CommandManager::new(10);

// 명령 실행
let insert_cmd = Box::new(InsertCommand::new(&mut editor, "Hello World".to_string()));
match manager.execute_command(insert_cmd) {
    Ok(result) => println!("Executed: {}", result),
    Err(error) => println!("Error: {}", error),
}

println!("Content: '{}'", editor.get_content());

// 실행 취소
if manager.can_undo() {
    manager.undo().unwrap();
}
println!("After undo: '{}'", editor.get_content());

// 다시 실행
if manager.can_redo() {
    manager.redo().unwrap();
}
println!("After redo: '{}'", editor.get_content());
```

### 2. 홈 오토메이션 시스템
```rust
// 스마트 조명 제어
pub struct Light {
    location: String,
    is_on: bool,
    brightness: u8,
}

impl Light {
    pub fn turn_on(&mut self) -> String {
        self.is_on = true;
        self.brightness = 100;
        format!("{} light turned ON", self.location)
    }

    pub fn turn_off(&mut self) -> String {
        self.is_on = false;
        self.brightness = 0;
        format!("{} light turned OFF", self.location)
    }
}

// 조명 켜기 명령
pub struct LightOnCommand {
    light: Option<*mut Light>,
    was_on: bool,
    previous_brightness: u8,
}

impl Command for LightOnCommand {
    fn execute(&mut self) -> Result<String, String> {
        if let Some(light) = self.get_light() {
            self.was_on = light.is_on();
            self.previous_brightness = light.get_brightness();
            Ok(light.turn_on())
        } else {
            Err("No light available".to_string())
        }
    }

    fn undo(&mut self) -> Result<String, String> {
        if let Some(light) = self.get_light() {
            if self.was_on {
                Ok(light.set_brightness(self.previous_brightness))
            } else {
                Ok(light.turn_off())
            }
        } else {
            Err("No light available".to_string())
        }
    }
}
```

### 3. 리모컨 제어기
```rust
pub struct RemoteControl {
    on_commands: Vec<Option<Box<dyn Command>>>,
    off_commands: Vec<Option<Box<dyn Command>>>,
    last_command: Option<Box<dyn Command>>,
}

impl RemoteControl {
    pub fn set_command(
        &mut self,
        slot: usize,
        on_command: Box<dyn Command>,
        off_command: Box<dyn Command>,
    ) -> Result<(), String> {
        if slot >= self.slots {
            return Err("Invalid slot".to_string());
        }

        self.on_commands[slot] = Some(on_command);
        self.off_commands[slot] = Some(off_command);
        Ok(())
    }

    pub fn press_undo_button(&mut self) -> Result<String, String> {
        if let Some(mut command) = self.last_command.take() {
            command.undo()
        } else {
            Err("No command to undo".to_string())
        }
    }
}
```

### 4. 명령 큐 처리
```rust
pub struct CommandQueue {
    queue: VecDeque<QueuedCommand>,
    processing: bool,
}

pub struct QueuedCommand {
    command: Box<dyn Command>,
    priority: u8,
    scheduled_time: Option<SystemTime>,
    retry_count: u8,
    max_retries: u8,
}

impl CommandQueue {
    pub fn enqueue_command(
        &mut self,
        command: Box<dyn Command>,
        priority: u8,
        scheduled_time: Option<SystemTime>,
        max_retries: u8,
    ) {
        let queued_command = QueuedCommand {
            command,
            priority,
            scheduled_time,
            retry_count: 0,
            max_retries,
        };

        // 우선순위에 따른 삽입
        let insert_pos = self.queue
            .iter()
            .position(|cmd| cmd.priority < priority)
            .unwrap_or(self.queue.len());

        self.queue.insert(insert_pos, queued_command);
    }
}
```

## Rust의 장점

### 1. 메모리 안전성
- 소유권 시스템으로 명령 객체의 안전한 관리
- 댕글링 포인터나 메모리 누수 문제 방지

### 2. 타입 안전성
```rust
// 컴파일 타임에 명령 인터페이스 검증
pub trait Command: std::fmt::Debug {
    fn execute(&mut self) -> Result<String, String>;
    fn undo(&mut self) -> Result<String, String>;
}
```

### 3. 에러 처리
```rust
// Result를 사용한 명시적 에러 처리
match manager.execute_command(command) {
    Ok(result) => println!("Success: {}", result),
    Err(error) => println!("Error: {}", error),
}
```

### 4. 패턴 매칭
```rust
let resolution = match request.issue_type {
    IssueType::Technical => "Technical resolution",
    IssueType::Billing => "Billing resolution",
    _ => "General resolution",
};
```

### 5. 제로 코스트 추상화
- trait 시스템으로 성능 오버헤드 없는 추상화
- 컴파일 타임 최적화

## 적용 상황

### 1. 텍스트/코드 에디터
- Undo/Redo 기능 구현
- 매크로 명령 지원
- 명령 히스토리 관리

### 2. GUI 애플리케이션
```rust
trait UICommand {
    fn execute(&self, context: &mut UIContext) -> Result<(), UIError>;
    fn undo(&self, context: &mut UIContext) -> Result<(), UIError>;
}

struct ButtonClickCommand {
    button_id: String,
    action: Box<dyn Fn(&mut UIContext)>,
}
```

### 3. 데이터베이스 트랜잭션
- 트랜잭션 롤백 지원
- 배치 작업 처리
- 명령 로깅 및 감사

### 4. 게임 시스템
```rust
trait GameCommand {
    fn execute(&self, game_state: &mut GameState) -> CommandResult;
    fn undo(&self, game_state: &mut GameState) -> CommandResult;
}

struct MoveCommand {
    entity_id: EntityId,
    from: Position,
    to: Position,
}
```

### 5. 네트워크 작업
- 비동기 명령 실행
- 재시도 로직 구현
- 명령 우선순위 처리

### 6. IoT 장치 제어
```rust
trait DeviceCommand {
    fn execute(&self, device: &mut Device) -> Result<String, DeviceError>;
    fn get_device_id(&self) -> DeviceId;
}

struct TemperatureCommand {
    device_id: DeviceId,
    target_temperature: f32,
}
```

### 7. 워크플로우 엔진
- 작업 단계별 실행
- 실패 시 보상 작업
- 작업 스케줄링

### 8. 로깅 및 감사 시스템
```rust
struct AuditableCommand<T: Command> {
    inner: T,
    user_id: String,
    timestamp: SystemTime,
    audit_log: Arc<Mutex<AuditLog>>,
}

impl<T: Command> Command for AuditableCommand<T> {
    fn execute(&mut self) -> Result<String, String> {
        let result = self.inner.execute();
        self.audit_log.lock().unwrap().log_execution(&self.user_id, &result);
        result
    }
}
```

이 패턴은 Rust의 소유권 모델과 타입 시스템과 잘 어울리며, 안전하고 확장 가능한 명령 기반 아키텍처를 구축할 수 있게 해줍니다.