#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Old audio player interface
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

// New advanced audio player
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

AdvancedAudioPlayer* create_advanced_audio_player() {
    AdvancedAudioPlayer* player = malloc(sizeof(AdvancedAudioPlayer));
    player->play_vlc = advanced_play_vlc;
    player->play_mp4 = advanced_play_mp4;
    player->play_wav = advanced_play_wav;
    return player;
}

// Adapter
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

// Modern audio player using adapter
typedef struct AudioPlayer {
    OldAudioPlayer* old_player;
    AudioAdapter* adapter;
    void (*play)(struct AudioPlayer* self, const char* audio_type, const char* filename);
} AudioPlayer;

void modern_play(AudioPlayer* self, const char* audio_type, const char* filename) {
    if (strcmp(audio_type, "mp3") == 0) {
        self->old_player->play_mp3(self->old_player, filename);
    } else {
        self->adapter->play(self->adapter, audio_type, filename);
    }
}

AudioPlayer* create_audio_player() {
    AudioPlayer* player = malloc(sizeof(AudioPlayer));
    player->old_player = create_old_audio_player();
    player->adapter = create_audio_adapter();
    player->play = modern_play;
    return player;
}

void destroy_audio_player(AudioPlayer* player) {
    if (player) {
        free(player->old_player);
        free(player->adapter->advanced_player);
        free(player->adapter);
        free(player);
    }
}

int main() {
    AudioPlayer* player = create_audio_player();

    printf("--- Audio Player with Adapter Pattern ---\n");
    player->play(player, "mp3", "song.mp3");
    player->play(player, "mp4", "video.mp4");
    player->play(player, "vlc", "movie.vlc");
    player->play(player, "wav", "sound.wav");
    player->play(player, "avi", "unsupported.avi");

    destroy_audio_player(player);
    return 0;
}