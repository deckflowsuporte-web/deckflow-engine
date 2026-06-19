/**
 * @file audio.c
 * @brief Audio System Implementation
 * 
 * Note: This is a stub implementation. For full audio support,
 * integrate SDL_mixer or another audio library.
 */

#include "audio.h"
#include <stdio.h>
#include <stdlib.h>

// Placeholder channel structure
struct AudioChannel {
    char* filepath;
    SoundType type;
    void* data;  // Actual audio data (e.g., Mix_Chunk*, Mix_Music*)
    bool loaded;
};

static AudioVolume g_volume = {1.0f, 1.0f, 1.0f};
static AudioChannel* g_current_music = NULL;
static bool g_initialized = false;
static bool g_paused = false;

bool audio_init(void) {
    if (g_initialized) return true;
    
    // In production, initialize SDL_mixer here:
    // if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;
    
    printf("[Audio] System initialized\n");
    g_initialized = true;
    return true;
}

void audio_shutdown(void) {
    if (!g_initialized) return;
    
    // Stop any playing audio
    if (g_current_music) {
        // Mix_HaltMusic();
    }
    
    // In production:
    // Mix_CloseAudio();
    
    g_initialized = false;
    printf("[Audio] System shutdown\n");
}

void audio_update(void) {
    // Update audio processing if needed
}

AudioChannel* audio_load_sfx(const char* filepath) {
    AudioChannel* channel = (AudioChannel*)malloc(sizeof(AudioChannel));
    if (!channel) return NULL;
    
    channel->filepath = NULL;
    channel->type = SOUND_TYPE_SFX;
    channel->data = NULL;
    channel->loaded = false;
    
    if (filepath) {
        channel->filepath = (char*)malloc(256);
        if (channel->filepath) {
            // In production, load with SDL_mixer:
            // channel->data = Mix_LoadWAV(filepath);
            // channel->loaded = (channel->data != NULL);
            snprintf(channel->filepath, 256, "%s", filepath);
            
            // Stub: mark as loaded
            channel->loaded = true;
            printf("[Audio] Loaded SFX: %s\n", filepath);
        }
    }
    
    return channel;
}

AudioChannel* audio_load_music(const char* filepath) {
    AudioChannel* channel = (AudioChannel*)malloc(sizeof(AudioChannel));
    if (!channel) return NULL;
    
    channel->filepath = NULL;
    channel->type = SOUND_TYPE_MUSIC;
    channel->data = NULL;
    channel->loaded = false;
    
    if (filepath) {
        channel->filepath = (char*)malloc(256);
        if (channel->filepath) {
            // In production, load with SDL_mixer:
            // channel->data = Mix_LoadMUS(filepath);
            // channel->loaded = (channel->data != NULL);
            snprintf(channel->filepath, 256, "%s", filepath);
            
            // Stub: mark as loaded
            channel->loaded = true;
            printf("[Audio] Loaded Music: %s\n", filepath);
        }
    }
    
    return channel;
}

void audio_play_sfx(AudioChannel* sound) {
    if (!sound || !sound->loaded) return;
    
    // In production:
    // Mix_VolumeChunk(data, MIX_MAX_VOLUME * g_volume.master * g_volume.sfx);
    // Mix_PlayChannel(-1, data, 0);
    
    printf("[Audio] Playing SFX: %s\n", sound->filepath ? sound->filepath : "unknown");
}

void audio_play_sfx_ex(AudioChannel* sound, float volume, int loops) {
    (void)loops;
    if (!sound || !sound->loaded) return;
    
    // In production:
    // float vol = MIX_MAX_VOLUME * g_volume.master * g_volume.sfx * volume;
    // Mix_VolumeChunk(data, vol);
    // Mix_PlayChannel(-1, data, loops);
    
    printf("[Audio] Playing SFX: %s (vol=%.1f, loops=%d)\n", 
           sound->filepath ? sound->filepath : "unknown", volume, loops);
}

void audio_play_music(AudioChannel* music) {
    audio_play_music_ex(music, 1.0f, -1);
}

void audio_play_music_ex(AudioChannel* music, float volume, int loops) {
    if (!music) return;
    
    // Stop current music
    if (g_current_music) {
        // Mix_HaltMusic();
    }
    
    g_current_music = music;
    g_paused = false;
    
    // In production:
    // float vol = MIX_MAX_VOLUME * g_volume.master * g_volume.music * volume;
    // Mix_VolumeMusic(vol);
    // Mix_PlayMusic(music->data, loops);
    
    printf("[Audio] Playing Music: %s (vol=%.1f, loops=%d)\n", 
           music->filepath ? music->filepath : "unknown", volume, loops);
}

void audio_stop_music(void) {
    if (g_current_music) {
        // Mix_HaltMusic();
        printf("[Audio] Music stopped\n");
    }
    g_current_music = NULL;
}

void audio_pause_music(void) {
    if (g_current_music && !g_paused) {
        // Mix_PauseMusic();
        g_paused = true;
        printf("[Audio] Music paused\n");
    }
}

void audio_resume_music(void) {
    if (g_current_music && g_paused) {
        // Mix_ResumeMusic();
        g_paused = false;
        printf("[Audio] Music resumed\n");
    }
}

void audio_set_master_volume(float volume) {
    g_volume.master = volume < 0 ? 0 : (volume > 1 ? 1 : volume);
    // In production: Mix_Volume(-1, MIX_MAX_VOLUME * g_volume.master);
}

void audio_set_sfx_volume(float volume) {
    g_volume.sfx = volume < 0 ? 0 : (volume > 1 ? 1 : volume);
    // In production: Mix_Volume(-1, MIX_MAX_VOLUME * g_volume.master * g_volume.sfx);
}

void audio_set_music_volume(float volume) {
    g_volume.music = volume < 0 ? 0 : (volume > 1 ? 1 : volume);
    // In production: Mix_VolumeMusic(MIX_MAX_VOLUME * g_volume.master * g_volume.music);
}

float audio_get_master_volume(void) { return g_volume.master; }
float audio_get_sfx_volume(void) { return g_volume.sfx; }
float audio_get_music_volume(void) { return g_volume.music; }

void audio_free_sfx(AudioChannel* sound) {
    if (!sound) return;
    if (sound->data) {
        // Mix_FreeChunk(sound->data);
    }
    if (sound->filepath) free(sound->filepath);
    free(sound);
}

void audio_free_music(AudioChannel* music) {
    if (!music) return;
    if (music == g_current_music) {
        audio_stop_music();
    }
    if (music->data) {
        // Mix_FreeMusic(music->data);
    }
    if (music->filepath) free(music->filepath);
    free(music);
}

bool audio_is_music_playing(void) {
    // In production: return Mix_PlayingMusic();
    return g_current_music != NULL && !g_paused;
}

void audio_crossfade_music(AudioChannel* new_music, int ms_fade_time) {
    (void)ms_fade_time;
    // In production: Mix_FadeOutMusic(ms_fade_time);
    // Then schedule new_music to start after fade
    audio_play_music(new_music);
}
