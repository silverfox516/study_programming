/// State Pattern Implementation in Rust
///
/// The State pattern allows an object to alter its behavior when its internal
/// state changes. The object will appear to change its class. This pattern
/// encapsulates state-specific behavior and makes state transitions explicit.

use std::collections::HashMap;

/// Vending machine example demonstrating state pattern
pub trait VendingMachineState {
    fn insert_coin(&mut self, machine: &mut VendingMachine, amount: u32) -> Result<String, String>;
    fn select_product(&mut self, machine: &mut VendingMachine, product_id: &str) -> Result<String, String>;
    fn dispense(&mut self, machine: &mut VendingMachine) -> Result<String, String>;
    fn cancel_transaction(&mut self, machine: &mut VendingMachine) -> Result<String, String>;
    fn get_state_name(&self) -> &str;
    fn get_available_actions(&self) -> Vec<&str>;
}

#[derive(Debug, Clone)]
pub struct Product {
    pub id: String,
    pub name: String,
    pub price: u32,
    pub stock: u32,
}

impl Product {
    pub fn new(id: &str, name: &str, price: u32, stock: u32) -> Self {
        Product {
            id: id.to_string(),
            name: name.to_string(),
            price,
            stock,
        }
    }
}

pub struct VendingMachine {
    current_state: Box<dyn VendingMachineState>,
    products: HashMap<String, Product>,
    inserted_amount: u32,
    selected_product: Option<String>,
    change_dispenser: u32,
}

impl VendingMachine {
    pub fn new() -> Self {
        let mut machine = VendingMachine {
            current_state: Box::new(IdleState),
            products: HashMap::new(),
            inserted_amount: 0,
            selected_product: None,
            change_dispenser: 1000, // $10.00 in change available
        };

        // Add some products
        machine.add_product(Product::new("A1", "Soda", 150, 10));
        machine.add_product(Product::new("B1", "Chips", 125, 8));
        machine.add_product(Product::new("C1", "Candy", 100, 15));
        machine.add_product(Product::new("D1", "Water", 75, 20));

        machine
    }

    pub fn set_state(&mut self, state: Box<dyn VendingMachineState>) {
        self.current_state = state;
    }

    pub fn insert_coin(&mut self, amount: u32) -> Result<String, String> {
        let mut state = std::mem::replace(&mut self.current_state, Box::new(IdleState));
        let result = state.insert_coin(self, amount);
        self.current_state = state;
        result
    }

    pub fn select_product(&mut self, product_id: &str) -> Result<String, String> {
        let mut state = std::mem::replace(&mut self.current_state, Box::new(IdleState));
        let result = state.select_product(self, product_id);
        self.current_state = state;
        result
    }

    pub fn dispense(&mut self) -> Result<String, String> {
        let mut state = std::mem::replace(&mut self.current_state, Box::new(IdleState));
        let result = state.dispense(self);
        self.current_state = state;
        result
    }

    pub fn cancel_transaction(&mut self) -> Result<String, String> {
        let mut state = std::mem::replace(&mut self.current_state, Box::new(IdleState));
        let result = state.cancel_transaction(self);
        self.current_state = state;
        result
    }

    pub fn add_product(&mut self, product: Product) {
        self.products.insert(product.id.clone(), product);
    }

    pub fn get_product(&self, product_id: &str) -> Option<&Product> {
        self.products.get(product_id)
    }

    pub fn get_product_mut(&mut self, product_id: &str) -> Option<&mut Product> {
        self.products.get_mut(product_id)
    }

    pub fn get_inserted_amount(&self) -> u32 {
        self.inserted_amount
    }

    pub fn add_inserted_amount(&mut self, amount: u32) {
        self.inserted_amount += amount;
    }

    pub fn clear_inserted_amount(&mut self) -> u32 {
        let amount = self.inserted_amount;
        self.inserted_amount = 0;
        amount
    }

    pub fn get_selected_product(&self) -> Option<&String> {
        self.selected_product.as_ref()
    }

    pub fn set_selected_product(&mut self, product_id: Option<String>) {
        self.selected_product = product_id;
    }

    pub fn can_make_change(&self, amount: u32) -> bool {
        self.change_dispenser >= amount
    }

    pub fn dispense_change(&mut self, amount: u32) -> Result<(), String> {
        if self.change_dispenser >= amount {
            self.change_dispenser -= amount;
            Ok(())
        } else {
            Err("Insufficient change available".to_string())
        }
    }

    pub fn get_state_name(&self) -> &str {
        self.current_state.get_state_name()
    }

    pub fn get_available_actions(&self) -> Vec<&str> {
        self.current_state.get_available_actions()
    }

    pub fn get_products(&self) -> &HashMap<String, Product> {
        &self.products
    }

    pub fn get_status(&self) -> VendingMachineStatus {
        VendingMachineStatus {
            state: self.get_state_name().to_string(),
            inserted_amount: self.inserted_amount,
            selected_product: self.selected_product.clone(),
            available_actions: self.get_available_actions().iter().map(|s| s.to_string()).collect(),
            change_available: self.change_dispenser,
        }
    }
}

impl Default for VendingMachine {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug)]
pub struct VendingMachineStatus {
    pub state: String,
    pub inserted_amount: u32,
    pub selected_product: Option<String>,
    pub available_actions: Vec<String>,
    pub change_available: u32,
}

/// Idle State - waiting for coins
pub struct IdleState;

impl VendingMachineState for IdleState {
    fn insert_coin(&mut self, machine: &mut VendingMachine, amount: u32) -> Result<String, String> {
        if amount == 0 {
            return Err("Invalid coin amount".to_string());
        }

        machine.add_inserted_amount(amount);
        machine.set_state(Box::new(HasMoneyState));
        Ok(format!("Inserted {}¢. Total: {}¢", amount, machine.get_inserted_amount()))
    }

    fn select_product(&mut self, _machine: &mut VendingMachine, _product_id: &str) -> Result<String, String> {
        Err("Please insert coins first".to_string())
    }

    fn dispense(&mut self, _machine: &mut VendingMachine) -> Result<String, String> {
        Err("No product selected".to_string())
    }

    fn cancel_transaction(&mut self, _machine: &mut VendingMachine) -> Result<String, String> {
        Ok("No transaction to cancel".to_string())
    }

    fn get_state_name(&self) -> &str {
        "Idle"
    }

    fn get_available_actions(&self) -> Vec<&str> {
        vec!["insert_coin"]
    }
}

/// Has Money State - coins inserted, waiting for product selection
pub struct HasMoneyState;

impl VendingMachineState for HasMoneyState {
    fn insert_coin(&mut self, machine: &mut VendingMachine, amount: u32) -> Result<String, String> {
        if amount == 0 {
            return Err("Invalid coin amount".to_string());
        }

        machine.add_inserted_amount(amount);
        Ok(format!("Inserted {}¢. Total: {}¢", amount, machine.get_inserted_amount()))
    }

    fn select_product(&mut self, machine: &mut VendingMachine, product_id: &str) -> Result<String, String> {
        // Get product information and validate
        let (product_name, product_price, product_stock) = {
            let product = machine.get_product(product_id)
                .ok_or_else(|| format!("Product {} not found", product_id))?;
            (product.name.clone(), product.price, product.stock)
        };

        if product_stock == 0 {
            return Err(format!("Product {} is out of stock", product_name));
        }

        let inserted_amount = machine.get_inserted_amount();
        if inserted_amount < product_price {
            return Err(format!(
                "Insufficient funds. Need {}¢ more",
                product_price - inserted_amount
            ));
        }

        let change_needed = inserted_amount - product_price;
        if change_needed > 0 && !machine.can_make_change(change_needed) {
            return Err("Exact change required".to_string());
        }

        machine.set_selected_product(Some(product_id.to_string()));
        machine.set_state(Box::new(DispensingState));
        Ok(format!("Selected {}. Dispensing...", product_name))
    }

    fn dispense(&mut self, _machine: &mut VendingMachine) -> Result<String, String> {
        Err("Please select a product first".to_string())
    }

    fn cancel_transaction(&mut self, machine: &mut VendingMachine) -> Result<String, String> {
        let amount = machine.clear_inserted_amount();
        machine.set_state(Box::new(IdleState));
        Ok(format!("Transaction cancelled. Returned {}¢", amount))
    }

    fn get_state_name(&self) -> &str {
        "HasMoney"
    }

    fn get_available_actions(&self) -> Vec<&str> {
        vec!["insert_coin", "select_product", "cancel_transaction"]
    }
}

/// Dispensing State - product selected, dispensing product and change
pub struct DispensingState;

impl VendingMachineState for DispensingState {
    fn insert_coin(&mut self, _machine: &mut VendingMachine, _amount: u32) -> Result<String, String> {
        Err("Currently dispensing. Please wait".to_string())
    }

    fn select_product(&mut self, _machine: &mut VendingMachine, _product_id: &str) -> Result<String, String> {
        Err("Currently dispensing. Please wait".to_string())
    }

    fn dispense(&mut self, machine: &mut VendingMachine) -> Result<String, String> {
        let product_id = machine.get_selected_product()
            .ok_or("No product selected")?
            .clone();

        let mut result = String::new();

        // Get needed values first
        let inserted_amount = machine.get_inserted_amount();

        // Dispense product
        if let Some(product) = machine.get_product_mut(&product_id) {
            if product.stock > 0 {
                product.stock -= 1;
                let product_name = product.name.clone();
                let product_price = product.price;

                result.push_str(&format!("Dispensed {}", product_name));

                // Calculate and dispense change
                let change = inserted_amount - product_price;
                if change > 0 {
                    machine.dispense_change(change)?;
                    result.push_str(&format!(". Change: {}¢", change));
                }
            } else {
                machine.set_state(Box::new(HasMoneyState));
                return Err("Product out of stock".to_string());
            }
        }

        // Reset machine state
        machine.clear_inserted_amount();
        machine.set_selected_product(None);
        machine.set_state(Box::new(IdleState));

        Ok(result)
    }

    fn cancel_transaction(&mut self, _machine: &mut VendingMachine) -> Result<String, String> {
        Err("Cannot cancel during dispensing".to_string())
    }

    fn get_state_name(&self) -> &str {
        "Dispensing"
    }

    fn get_available_actions(&self) -> Vec<&str> {
        vec!["dispense"]
    }
}

/// Traffic Light State Machine
pub trait TrafficLightState {
    fn next(&mut self, light: &mut TrafficLight) -> Result<String, String>;
    fn get_color(&self) -> TrafficColor;
    fn get_duration(&self) -> u32;
    fn can_transition(&self) -> bool;
}

#[derive(Debug, Clone, PartialEq)]
pub enum TrafficColor {
    Red,
    Yellow,
    Green,
}

pub struct TrafficLight {
    current_state: Box<dyn TrafficLightState>,
    elapsed_time: u32,
    is_pedestrian_requested: bool,
    emergency_mode: bool,
}

impl TrafficLight {
    pub fn new() -> Self {
        TrafficLight {
            current_state: Box::new(RedLightState),
            elapsed_time: 0,
            is_pedestrian_requested: false,
            emergency_mode: false,
        }
    }

    pub fn set_state(&mut self, state: Box<dyn TrafficLightState>) {
        self.current_state = state;
        self.elapsed_time = 0;
    }

    pub fn tick(&mut self) -> Result<String, String> {
        self.elapsed_time += 1;

        if self.emergency_mode {
            self.set_state(Box::new(RedLightState));
            return Ok("Emergency mode: Traffic light set to RED".to_string());
        }

        if self.elapsed_time >= self.current_state.get_duration() {
            let mut state = std::mem::replace(&mut self.current_state, Box::new(RedLightState));
            let result = state.next(self);
            self.current_state = state;
            result
        } else {
            Ok(format!(
                "Current: {:?} ({}s remaining)",
                self.current_state.get_color(),
                self.current_state.get_duration() - self.elapsed_time
            ))
        }
    }

    pub fn request_pedestrian_crossing(&mut self) {
        self.is_pedestrian_requested = true;
    }

    pub fn clear_pedestrian_request(&mut self) {
        self.is_pedestrian_requested = false;
    }

    pub fn is_pedestrian_requested(&self) -> bool {
        self.is_pedestrian_requested
    }

    pub fn set_emergency_mode(&mut self, enabled: bool) {
        self.emergency_mode = enabled;
        if enabled {
            self.set_state(Box::new(RedLightState));
        }
    }

    pub fn is_emergency_mode(&self) -> bool {
        self.emergency_mode
    }

    pub fn get_current_color(&self) -> TrafficColor {
        self.current_state.get_color()
    }

    pub fn get_elapsed_time(&self) -> u32 {
        self.elapsed_time
    }

    pub fn can_cross(&self) -> bool {
        matches!(self.current_state.get_color(), TrafficColor::Green)
    }

    pub fn get_status(&self) -> TrafficLightStatus {
        TrafficLightStatus {
            color: self.current_state.get_color(),
            elapsed_time: self.elapsed_time,
            duration: self.current_state.get_duration(),
            pedestrian_requested: self.is_pedestrian_requested,
            emergency_mode: self.emergency_mode,
            can_cross: self.can_cross(),
        }
    }
}

impl Default for TrafficLight {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug)]
pub struct TrafficLightStatus {
    pub color: TrafficColor,
    pub elapsed_time: u32,
    pub duration: u32,
    pub pedestrian_requested: bool,
    pub emergency_mode: bool,
    pub can_cross: bool,
}

/// Traffic Light States
pub struct RedLightState;

impl TrafficLightState for RedLightState {
    fn next(&mut self, light: &mut TrafficLight) -> Result<String, String> {
        light.set_state(Box::new(GreenLightState));
        Ok("Traffic light changed from RED to GREEN".to_string())
    }

    fn get_color(&self) -> TrafficColor {
        TrafficColor::Red
    }

    fn get_duration(&self) -> u32 {
        30 // 30 seconds
    }

    fn can_transition(&self) -> bool {
        true
    }
}

pub struct YellowLightState;

impl TrafficLightState for YellowLightState {
    fn next(&mut self, light: &mut TrafficLight) -> Result<String, String> {
        light.set_state(Box::new(RedLightState));
        Ok("Traffic light changed from YELLOW to RED".to_string())
    }

    fn get_color(&self) -> TrafficColor {
        TrafficColor::Yellow
    }

    fn get_duration(&self) -> u32 {
        5 // 5 seconds
    }

    fn can_transition(&self) -> bool {
        true
    }
}

pub struct GreenLightState;

impl TrafficLightState for GreenLightState {
    fn next(&mut self, light: &mut TrafficLight) -> Result<String, String> {
        // Check for pedestrian crossing request - extend green time
        if light.is_pedestrian_requested() && light.get_elapsed_time() < 45 {
            light.clear_pedestrian_request();
            return Ok("Pedestrian crossing requested - extending GREEN light".to_string());
        }

        light.set_state(Box::new(YellowLightState));
        Ok("Traffic light changed from GREEN to YELLOW".to_string())
    }

    fn get_color(&self) -> TrafficColor {
        TrafficColor::Green
    }

    fn get_duration(&self) -> u32 {
        45 // 45 seconds
    }

    fn can_transition(&self) -> bool {
        true
    }
}

/// Game Character State Machine
pub trait CharacterState: std::fmt::Debug {
    fn update(&mut self, character: &mut GameCharacter, delta_time: f32) -> Result<String, String>;
    fn handle_input(&mut self, character: &mut GameCharacter, input: GameInput) -> Result<String, String>;
    fn get_state_name(&self) -> &str;
    fn can_transition_to(&self, new_state: &str) -> bool;
}

#[derive(Debug, Clone)]
pub enum GameInput {
    MoveLeft,
    MoveRight,
    Jump,
    Attack,
    Block,
    Idle,
}

#[derive(Debug)]
pub struct GameCharacter {
    current_state: Box<dyn CharacterState>,
    position: (f32, f32),
    velocity: (f32, f32),
    health: u32,
    energy: u32,
    is_grounded: bool,
    facing_right: bool,
    animation_time: f32,
}

impl GameCharacter {
    pub fn new() -> Self {
        GameCharacter {
            current_state: Box::new(IdleCharacterState),
            position: (0.0, 0.0),
            velocity: (0.0, 0.0),
            health: 100,
            energy: 100,
            is_grounded: true,
            facing_right: true,
            animation_time: 0.0,
        }
    }

    pub fn set_state(&mut self, state: Box<dyn CharacterState>) {
        self.current_state = state;
        self.animation_time = 0.0;
    }

    pub fn update(&mut self, delta_time: f32) -> Result<String, String> {
        self.animation_time += delta_time;

        // Update physics
        self.position.0 += self.velocity.0 * delta_time;
        self.position.1 += self.velocity.1 * delta_time;

        // Apply gravity if not grounded
        if !self.is_grounded {
            self.velocity.1 -= 9.8 * delta_time; // Gravity
        }

        // Ground check (simplified)
        if self.position.1 <= 0.0 {
            self.position.1 = 0.0;
            self.velocity.1 = 0.0;
            self.is_grounded = true;
        }

        // Energy recovery
        if self.energy < 100 {
            self.energy = (self.energy + 1).min(100);
        }

        // Temporarily take ownership to avoid borrowing conflicts
        let mut state = std::mem::replace(&mut self.current_state, Box::new(IdleCharacterState));
        let result = state.update(self, delta_time);
        self.current_state = state;
        result
    }

    pub fn handle_input(&mut self, input: GameInput) -> Result<String, String> {
        // Temporarily take ownership to avoid borrowing conflicts
        let mut state = std::mem::replace(&mut self.current_state, Box::new(IdleCharacterState));
        let result = state.handle_input(self, input);
        self.current_state = state;
        result
    }

    pub fn get_state_name(&self) -> &str {
        self.current_state.get_state_name()
    }

    pub fn get_position(&self) -> (f32, f32) {
        self.position
    }

    pub fn set_position(&mut self, position: (f32, f32)) {
        self.position = position;
    }

    pub fn get_velocity(&self) -> (f32, f32) {
        self.velocity
    }

    pub fn set_velocity(&mut self, velocity: (f32, f32)) {
        self.velocity = velocity;
    }

    pub fn get_health(&self) -> u32 {
        self.health
    }

    pub fn take_damage(&mut self, damage: u32) {
        self.health = self.health.saturating_sub(damage);
    }

    pub fn get_energy(&self) -> u32 {
        self.energy
    }

    pub fn consume_energy(&mut self, amount: u32) -> bool {
        if self.energy >= amount {
            self.energy -= amount;
            true
        } else {
            false
        }
    }

    pub fn is_grounded(&self) -> bool {
        self.is_grounded
    }

    pub fn set_grounded(&mut self, grounded: bool) {
        self.is_grounded = grounded;
    }

    pub fn is_facing_right(&self) -> bool {
        self.facing_right
    }

    pub fn set_facing_right(&mut self, facing_right: bool) {
        self.facing_right = facing_right;
    }

    pub fn get_status(&self) -> CharacterStatus {
        CharacterStatus {
            state: self.get_state_name().to_string(),
            position: self.position,
            velocity: self.velocity,
            health: self.health,
            energy: self.energy,
            is_grounded: self.is_grounded,
            facing_right: self.facing_right,
        }
    }
}

impl Default for GameCharacter {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug)]
pub struct CharacterStatus {
    pub state: String,
    pub position: (f32, f32),
    pub velocity: (f32, f32),
    pub health: u32,
    pub energy: u32,
    pub is_grounded: bool,
    pub facing_right: bool,
}

/// Character States
#[derive(Debug)]
pub struct IdleCharacterState;

impl CharacterState for IdleCharacterState {
    fn update(&mut self, character: &mut GameCharacter, _delta_time: f32) -> Result<String, String> {
        // Gradually slow down
        let (vx, vy) = character.get_velocity();
        character.set_velocity((vx * 0.8, vy));

        Ok("Character is idle".to_string())
    }

    fn handle_input(&mut self, character: &mut GameCharacter, input: GameInput) -> Result<String, String> {
        match input {
            GameInput::MoveLeft => {
                character.set_state(Box::new(WalkingCharacterState));
                character.set_facing_right(false);
                character.set_velocity((-5.0, character.get_velocity().1));
                Ok("Started walking left".to_string())
            }
            GameInput::MoveRight => {
                character.set_state(Box::new(WalkingCharacterState));
                character.set_facing_right(true);
                character.set_velocity((5.0, character.get_velocity().1));
                Ok("Started walking right".to_string())
            }
            GameInput::Jump => {
                if character.is_grounded() && character.consume_energy(20) {
                    character.set_state(Box::new(JumpingCharacterState));
                    character.set_velocity((character.get_velocity().0, 8.0));
                    character.set_grounded(false);
                    Ok("Started jumping".to_string())
                } else {
                    Err("Cannot jump (not grounded or insufficient energy)".to_string())
                }
            }
            GameInput::Attack => {
                if character.consume_energy(15) {
                    character.set_state(Box::new(AttackingCharacterState::new()));
                    Ok("Started attacking".to_string())
                } else {
                    Err("Insufficient energy to attack".to_string())
                }
            }
            _ => Ok("No action taken".to_string()),
        }
    }

    fn get_state_name(&self) -> &str {
        "Idle"
    }

    fn can_transition_to(&self, new_state: &str) -> bool {
        matches!(new_state, "Walking" | "Jumping" | "Attacking")
    }
}

#[derive(Debug)]
pub struct WalkingCharacterState;

impl CharacterState for WalkingCharacterState {
    fn update(&mut self, character: &mut GameCharacter, _delta_time: f32) -> Result<String, String> {
        // Check if velocity is too low, transition to idle
        let (vx, _) = character.get_velocity();
        if vx.abs() < 0.1 {
            character.set_state(Box::new(IdleCharacterState));
            return Ok("Stopped walking, now idle".to_string());
        }

        Ok("Character is walking".to_string())
    }

    fn handle_input(&mut self, character: &mut GameCharacter, input: GameInput) -> Result<String, String> {
        match input {
            GameInput::MoveLeft => {
                character.set_facing_right(false);
                character.set_velocity((-5.0, character.get_velocity().1));
                Ok("Walking left".to_string())
            }
            GameInput::MoveRight => {
                character.set_facing_right(true);
                character.set_velocity((5.0, character.get_velocity().1));
                Ok("Walking right".to_string())
            }
            GameInput::Jump => {
                if character.is_grounded() && character.consume_energy(20) {
                    character.set_state(Box::new(JumpingCharacterState));
                    character.set_velocity((character.get_velocity().0, 8.0));
                    character.set_grounded(false);
                    Ok("Jumped while walking".to_string())
                } else {
                    Err("Cannot jump".to_string())
                }
            }
            GameInput::Idle => {
                character.set_state(Box::new(IdleCharacterState));
                Ok("Stopped walking".to_string())
            }
            GameInput::Attack => {
                if character.consume_energy(15) {
                    character.set_state(Box::new(AttackingCharacterState::new()));
                    Ok("Attacked while walking".to_string())
                } else {
                    Err("Insufficient energy to attack".to_string())
                }
            }
            _ => Ok("Continuing to walk".to_string()),
        }
    }

    fn get_state_name(&self) -> &str {
        "Walking"
    }

    fn can_transition_to(&self, new_state: &str) -> bool {
        matches!(new_state, "Idle" | "Jumping" | "Attacking")
    }
}

#[derive(Debug)]
pub struct JumpingCharacterState;

impl CharacterState for JumpingCharacterState {
    fn update(&mut self, character: &mut GameCharacter, _delta_time: f32) -> Result<String, String> {
        if character.is_grounded() {
            let (vx, _) = character.get_velocity();
            if vx.abs() > 0.1 {
                character.set_state(Box::new(WalkingCharacterState));
                Ok("Landed and walking".to_string())
            } else {
                character.set_state(Box::new(IdleCharacterState));
                Ok("Landed and idle".to_string())
            }
        } else {
            Ok("Character is in the air".to_string())
        }
    }

    fn handle_input(&mut self, character: &mut GameCharacter, input: GameInput) -> Result<String, String> {
        match input {
            GameInput::MoveLeft => {
                character.set_facing_right(false);
                let (_, vy) = character.get_velocity();
                character.set_velocity((-3.0, vy)); // Reduced air control
                Ok("Air control: moving left".to_string())
            }
            GameInput::MoveRight => {
                character.set_facing_right(true);
                let (_, vy) = character.get_velocity();
                character.set_velocity((3.0, vy)); // Reduced air control
                Ok("Air control: moving right".to_string())
            }
            GameInput::Attack => {
                if character.consume_energy(25) { // More expensive in air
                    character.set_state(Box::new(AttackingCharacterState::new()));
                    Ok("Air attack!".to_string())
                } else {
                    Err("Insufficient energy for air attack".to_string())
                }
            }
            _ => Ok("Character continues jumping".to_string()),
        }
    }

    fn get_state_name(&self) -> &str {
        "Jumping"
    }

    fn can_transition_to(&self, new_state: &str) -> bool {
        matches!(new_state, "Idle" | "Walking" | "Attacking")
    }
}

#[derive(Debug)]
pub struct AttackingCharacterState {
    attack_timer: f32,
}

impl AttackingCharacterState {
    fn new() -> Self {
        AttackingCharacterState {
            attack_timer: 0.0,
        }
    }
}

impl Default for AttackingCharacterState {
    fn default() -> Self {
        Self::new()
    }
}

impl CharacterState for AttackingCharacterState {
    fn update(&mut self, character: &mut GameCharacter, delta_time: f32) -> Result<String, String> {
        self.attack_timer += delta_time;

        if self.attack_timer >= 0.5 { // Attack duration
            let (vx, _) = character.get_velocity();
            if vx.abs() > 0.1 {
                character.set_state(Box::new(WalkingCharacterState));
                Ok("Attack finished, now walking".to_string())
            } else {
                character.set_state(Box::new(IdleCharacterState));
                Ok("Attack finished, now idle".to_string())
            }
        } else {
            Ok("Character is attacking".to_string())
        }
    }

    fn handle_input(&mut self, _character: &mut GameCharacter, input: GameInput) -> Result<String, String> {
        match input {
            GameInput::Attack => Ok("Already attacking".to_string()),
            _ => Ok("Cannot act while attacking".to_string()),
        }
    }

    fn get_state_name(&self) -> &str {
        "Attacking"
    }

    fn can_transition_to(&self, new_state: &str) -> bool {
        matches!(new_state, "Idle" | "Walking")
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_vending_machine_states() {
        let mut machine = VendingMachine::new();

        // Initial state
        assert_eq!(machine.get_state_name(), "Idle");

        // Insert coin
        machine.insert_coin(150).unwrap();
        assert_eq!(machine.get_state_name(), "HasMoney");

        // Select product
        machine.select_product("A1").unwrap();
        assert_eq!(machine.get_state_name(), "Dispensing");

        // Dispense
        machine.dispense().unwrap();
        assert_eq!(machine.get_state_name(), "Idle");
    }

    #[test]
    fn test_traffic_light_states() {
        let mut light = TrafficLight::new();

        assert_eq!(light.get_current_color(), TrafficColor::Red);

        // Simulate time passing
        for _ in 0..30 {
            light.tick().unwrap();
        }

        // Should change to green
        light.tick().unwrap();
        assert_eq!(light.get_current_color(), TrafficColor::Green);
    }

    #[test]
    fn test_game_character_states() {
        let mut character = GameCharacter::new();

        assert_eq!(character.get_state_name(), "Idle");

        // Start walking
        character.handle_input(GameInput::MoveRight).unwrap();
        assert_eq!(character.get_state_name(), "Walking");

        // Jump while walking
        character.handle_input(GameInput::Jump).unwrap();
        assert_eq!(character.get_state_name(), "Jumping");
    }

    #[test]
    fn test_vending_machine_insufficient_funds() {
        let mut machine = VendingMachine::new();

        machine.insert_coin(50).unwrap(); // Not enough for any product
        let result = machine.select_product("A1"); // Costs 150¢

        assert!(result.is_err());
        assert!(result.unwrap_err().contains("Insufficient funds"));
    }

    #[test]
    fn test_character_energy_consumption() {
        let mut character = GameCharacter::new();

        // Drain energy
        for _ in 0..7 { // Each attack consumes 15 energy
            if character.get_energy() >= 15 {
                character.handle_input(GameInput::Attack).unwrap();
                character.update(0.6).unwrap(); // Wait for attack to finish
            }
        }

        // Should not be able to attack with low energy
        let result = character.handle_input(GameInput::Attack);
        assert!(result.is_err());
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== State Pattern Demo ===");

    // Vending Machine Example
    println!("\n1. Vending Machine State Machine:");
    let mut vending_machine = VendingMachine::new();

    println!("Available products:");
    for product in vending_machine.get_products().values() {
        println!("  {}: {} - {}¢ (stock: {})", product.id, product.name, product.price, product.stock);
    }

    println!("\nVending machine simulation:");
    println!("Initial state: {}", vending_machine.get_state_name());

    // Simulate a purchase
    let actions = [
        ("Insert 50¢", 0),
        ("Insert 100¢", 1),
        ("Select Soda (A1)", 2),
        ("Dispense", 3),
    ];

    for (description, action_type) in actions.iter() {
        println!("\nAction: {}", description);
        let result = match action_type {
            0 => vending_machine.insert_coin(50),
            1 => vending_machine.insert_coin(100),
            2 => vending_machine.select_product("A1"),
            3 => vending_machine.dispense(),
            _ => Err("Unknown action".to_string()),
        };

        match result {
            Ok(result_msg) => {
                println!("  Result: {}", result_msg);
                let status = vending_machine.get_status();
                println!("  State: {} | Inserted: {}¢ | Selected: {:?}",
                        status.state, status.inserted_amount, status.selected_product);
            }
            Err(error) => println!("  Error: {}", error),
        }
    }

    // Traffic Light Example
    println!("\n2. Traffic Light State Machine:");
    let mut traffic_light = TrafficLight::new();

    println!("Simulating traffic light for 2 minutes:");
    for second in 1..=120 {
        match traffic_light.tick() {
            Ok(status) => {
                if second % 10 == 0 || status.contains("changed") {
                    println!("  {}s: {}", second, status);
                }
            }
            Err(error) => println!("  {}s: Error - {}", second, error),
        }

        // Request pedestrian crossing at 30 seconds
        if second == 30 {
            traffic_light.request_pedestrian_crossing();
            println!("  {}s: Pedestrian crossing requested", second);
        }

        // Emergency mode at 90 seconds
        if second == 90 {
            traffic_light.set_emergency_mode(true);
            println!("  {}s: Emergency mode activated", second);
        }

        // Clear emergency at 100 seconds
        if second == 100 {
            traffic_light.set_emergency_mode(false);
            println!("  {}s: Emergency mode cleared", second);
        }
    }

    // Game Character Example
    println!("\n3. Game Character State Machine:");
    let mut character = GameCharacter::new();

    println!("Character initial status:");
    let status = character.get_status();
    println!("  State: {} | Position: ({:.1}, {:.1}) | Health: {} | Energy: {}",
            status.state, status.position.0, status.position.1, status.health, status.energy);

    let game_actions = [
        (GameInput::MoveRight, "Move right"),
        (GameInput::Jump, "Jump"),
        (GameInput::Attack, "Attack"),
        (GameInput::MoveLeft, "Move left"),
        (GameInput::Idle, "Stop moving"),
        (GameInput::Attack, "Attack"),
        (GameInput::Jump, "Jump"),
    ];

    println!("\nGame simulation:");
    for (input, description) in game_actions.iter() {
        println!("\nInput: {}", description);

        match character.handle_input(input.clone()) {
            Ok(result) => println!("  Action: {}", result),
            Err(error) => println!("  Failed: {}", error),
        }

        // Update character (simulate frame)
        for _ in 0..10 { // Simulate 10 frames
            match character.update(0.016) { // 60 FPS
                Ok(update_result) => {
                    if !update_result.contains("is") { // Skip repetitive status messages
                        println!("  Update: {}", update_result);
                    }
                }
                Err(error) => println!("  Update error: {}", error),
            }
        }

        let status = character.get_status();
        println!("  Status: {} | Pos: ({:.1}, {:.1}) | Energy: {}",
                status.state, status.position.0, status.position.1, status.energy);
    }

    // State Pattern Benefits
    println!("\n4. State Pattern Benefits:");
    println!("  ✅ Encapsulates state-specific behavior");
    println!("  ✅ Makes state transitions explicit");
    println!("  ✅ Eliminates large conditional statements");
    println!("  ✅ Allows objects to change behavior at runtime");
    println!("  ✅ Makes adding new states easier");
    println!("  ✅ Promotes single responsibility principle");
    println!("  ✅ Enables complex state machines with clear transitions");
}

pub fn main() {
    demo();
}