# Adapter Pattern

## 개요
Adapter 패턴은 호환되지 않는 인터페이스 때문에 함께 동작할 수 없는 클래스들이 함께 작동하도록 해주는 구조 패턴입니다. 기존 클래스의 인터페이스를 다른 인터페이스로 변환하여 클라이언트가 기대하는 인터페이스와 호환되도록 만듭니다.

## 구조
- **Target (AudioPlayer)**: 클라이언트가 사용하는 도메인에 특화된 인터페이스 정의
- **Adaptee (AdvancedAudioPlayer)**: 적응되어야 하는 기존 인터페이스
- **Adapter (AudioAdapter)**: Target과 Adaptee 사이의 인터페이스를 맞춰주는 역할
- **Client**: Target 인터페이스를 사용하는 객체

## C 언어 구현

### 기존 오디오 플레이어 (Target)
```c
// 기존 오디오 플레이어 - MP3만 지원
typedef struct OldAudioPlayer {
    void (*play_mp3)(struct OldAudioPlayer* self, const char* filename);
} OldAudioPlayer;

void old_play_mp3(OldAudioPlayer* self, const char* filename) {
    printf("Playing MP3 file: %s\n", filename);
}

OldAudioPlayer* create_old_audio_player() {
    OldAudioPlayer* player = malloc(sizeof(OldAudioPlayer));
    player->play_mp3 = old_play_mp3;
    return player;
}
```

### 고급 오디오 플레이어 (Adaptee)
```c
// 고급 오디오 플레이어 - VLC, MP4, WAV 지원
typedef struct AdvancedAudioPlayer {
    void (*play_vlc)(struct AdvancedAudioPlayer* self, const char* filename);
    void (*play_mp4)(struct AdvancedAudioPlayer* self, const char* filename);
    void (*play_wav)(struct AdvancedAudioPlayer* self, const char* filename);
} AdvancedAudioPlayer;

void advanced_play_vlc(AdvancedAudioPlayer* self, const char* filename) {
    printf("Playing VLC file: %s\n", filename);
}

void advanced_play_mp4(AdvancedAudioPlayer* self, const char* filename) {
    printf("Playing MP4 file: %s\n", filename);
}

void advanced_play_wav(AdvancedAudioPlayer* self, const char* filename) {
    printf("Playing WAV file: %s\n", filename);
}
```

### 어댑터 구현
```c
// 어댑터 - 고급 플레이어의 기능을 통합 인터페이스로 제공
typedef struct AudioAdapter {
    AdvancedAudioPlayer* advanced_player;
    void (*play)(struct AudioAdapter* self, const char* audio_type, const char* filename);
} AudioAdapter;

void adapter_play(AudioAdapter* self, const char* audio_type, const char* filename) {
    if (strcmp(audio_type, "vlc") == 0) {
        self->advanced_player->play_vlc(self->advanced_player, filename);
    } else if (strcmp(audio_type, "mp4") == 0) {
        self->advanced_player->play_mp4(self->advanced_player, filename);
    } else if (strcmp(audio_type, "wav") == 0) {
        self->advanced_player->play_wav(self->advanced_player, filename);
    } else {
        printf("Invalid media. %s format not supported\n", audio_type);
    }
}

AudioAdapter* create_audio_adapter() {
    AudioAdapter* adapter = malloc(sizeof(AudioAdapter));
    adapter->advanced_player = create_advanced_audio_player();
    adapter->play = adapter_play;
    return adapter;
}
```

### 통합 오디오 플레이어
```c
// 모든 형식을 지원하는 현대적 오디오 플레이어
typedef struct AudioPlayer {
    OldAudioPlayer* old_player;
    AudioAdapter* adapter;
    void (*play)(struct AudioPlayer* self, const char* audio_type, const char* filename);
} AudioPlayer;

void modern_play(AudioPlayer* self, const char* audio_type, const char* filename) {
    if (strcmp(audio_type, "mp3") == 0) {
        // 기존 플레이어 사용
        self->old_player->play_mp3(self->old_player, filename);
    } else {
        // 어댑터를 통해 고급 플레이어 사용
        self->adapter->play(self->adapter, audio_type, filename);
    }
}
```

## 사용 예제
```c
int main() {
    AudioPlayer* player = create_audio_player();

    printf("--- Audio Player with Adapter Pattern ---\n");

    // 기존 MP3 기능 사용
    player->play(player, "mp3", "song.mp3");

    // 어댑터를 통한 새로운 형식 지원
    player->play(player, "mp4", "video.mp4");
    player->play(player, "vlc", "movie.vlc");
    player->play(player, "wav", "sound.wav");

    // 지원하지 않는 형식
    player->play(player, "avi", "unsupported.avi");

    destroy_audio_player(player);
    return 0;
}
```

출력:
```
--- Audio Player with Adapter Pattern ---
Playing MP3 file: song.mp3
Playing MP4 file: video.mp4
Playing VLC file: movie.vlc
Playing WAV file: sound.wav
Invalid media. avi format not supported
```

## 장점과 단점

### 장점
- **기존 코드 재사용**: 기존 코드를 수정하지 않고 새로운 인터페이스에서 사용 가능
- **인터페이스 통합**: 서로 다른 인터페이스를 가진 클래스들을 하나의 통합된 인터페이스로 사용
- **레거시 시스템 통합**: 기존 레거시 시스템을 새로운 시스템과 연결
- **단일 책임 원칙**: 인터페이스 변환 로직을 별도의 클래스로 분리

### 단점
- **복잡성 증가**: 새로운 인터페이스와 클래스들이 추가됨
- **간접적 호출**: 어댑터를 거쳐야 하므로 직접 호출보다 약간의 성능 오버헤드
- **이해도 저하**: 코드의 전체적인 복잡성이 증가하여 이해하기 어려워질 수 있음

## 적용 상황
- **레거시 시스템 통합**: 기존 시스템을 새로운 시스템과 연결해야 할 때
- **서드파티 라이브러리 통합**: 외부 라이브러리의 인터페이스를 자신의 인터페이스에 맞춰야 할 때
- **데이터 형식 변환**: 서로 다른 데이터 형식 간의 변환이 필요할 때
- **API 호환성**: 버전이 다른 API들 간의 호환성을 제공해야 할 때
- **다중 미디어 처리**: 다양한 형식의 파일을 통일된 인터페이스로 처리해야 할 때

이 패턴은 특히 C 언어에서 함수 포인터를 활용하여 다양한 인터페이스를 통합하고, 기존 코드의 재사용성을 높이는 데 매우 유용합니다.