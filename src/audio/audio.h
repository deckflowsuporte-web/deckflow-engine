/**
 * @file audio.h
 * @brief Audio System - Sound effects and music
 */

#ifndef DECKFLOW_AUDIO_H
#define DECKFLOW_AUDIO_H

#include <stdbool.h>

// Sound types
typedef enum {
    SOUND_TYPE_SFX,      // Sound effects
    SOUND_TYPE_MUSIC     // Music tracks
} SoundType;

// Audio channel
typedef struct AudioChannel AudioChannel;

// Volume settings (0.0 to 1.0)
typedef struct {
    float master;
    float sfx;
    float music;
} AudioVolume;

// Initialize audio system
bool audio_init(void);

// Shutdown audio system
void audio_shutdown(void);

// Update audio (call once per frame)
void audio_update(void);

// Load sound
AudioChannel* audio_load_sfx(const char* filepath);

// Load music
AudioChannel* audio_load_music(const char* filepath);

// Play sound effect
void audio_play_sfx(AudioChannel* sound);

// Play sound effect with volume
void audio_play_sfx_ex(AudioChannel* sound, float volume, int loops);

// Play music
void audio_play_music(AudioChannel* music);

// Play music with volume and loops
void audio_play_music_ex(AudioChannel* music, float volume, int loops);

// Stop music
void audio_stop_music(void);

// Pause/Resume music
void audio_pause_music(void);
void audio_resume_music(void);

// Set volumes
void audio_set_master_volume(float volume);
void audio_set_sfx_volume(float volume);
void audio_set_music_volume(float volume);

// Get volumes
float audio_get_master_volume(void);
float audio_get_sfx_volume(void);
float audio_get_music_volume(void);

// Free sound
void audio_free_sfx(AudioChannel* sound);
void audio_free_music(AudioChannel* music);

// Check if music is playing
bool audio_is_music_playing(void);

// Crossfade music
void audio_crossfade_music(AudioChannel* new_music, int ms_fade_time);

#endif // DECKFLOW_AUDIO_H
