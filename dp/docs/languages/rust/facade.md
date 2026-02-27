# Facade Pattern - Rust Implementation

## 개요

Facade 패턴은 복잡한 하위 시스템에 대한 단순한 인터페이스를 제공합니다. 하위 시스템의 복잡성을 숨기고 클라이언트가 시스템을 쉽게 사용할 수 있도록 합니다.

## 구조

```rust
pub struct HomeTheaterFacade {
    amplifier: Amplifier,
    dvd_player: DvdPlayer,
    projector: Projector,
    lights: Lights,
    screen: Screen,
}

impl HomeTheaterFacade {
    pub fn watch_movie(&mut self, movie: &str) -> Vec<String> {
        let mut actions = Vec::new();

        actions.push("Getting ready to watch a movie...".to_string());
        actions.push(self.lights.dim(10));
        actions.push(self.screen.down());
        actions.push(self.projector.on());
        actions.push(self.amplifier.on());
        actions.push(self.dvd_player.on());
        actions.push(self.dvd_player.play(movie));

        actions
    }

    pub fn end_movie(&mut self) -> Vec<String> {
        let mut actions = Vec::new();

        actions.push("Shutting movie theater down...".to_string());
        actions.push(self.dvd_player.stop());
        actions.push(self.dvd_player.off());
        actions.push(self.amplifier.off());
        actions.push(self.projector.off());
        actions.push(self.screen.up());
        actions.push(self.lights.on());

        actions
    }
}
```

## 적용 상황

### 1. 홈 시어터 시스템
### 2. 컴퓨터 API 래퍼
### 3. 데이터베이스 접근 계층
### 4. 웹 서비스 클라이언트
### 5. 복잡한 라이브러리 래퍼