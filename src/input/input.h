/**
 * @file input.h
 * @brief Input System - Keyboard, Mouse, and Touch handling
 */

#ifndef DECKFLOW_INPUT_H
#define DECKFLOW_INPUT_H

#include <stdbool.h>
#include "../math/vector2.h"

// Key codes (SDL compatible)
typedef enum {
    KEY_UNKNOWN = 0,
    KEY_A = 4, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
    KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    KEY_0 = 39, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    KEY_SPACE = 44,
    KEY_LEFT = 80, KEY_RIGHT, KEY_UP, KEY_DOWN,
    KEY_ESCAPE = 41
} KeyCode;

// Input state for a single key
typedef struct {
    bool is_pressed;      // Currently held down
    bool just_pressed;   // Pressed this frame
    bool just_released;   // Released this frame
} InputKey;

// Mouse button
typedef enum {
    MOUSE_LEFT = 1,
    MOUSE_MIDDLE = 2,
    MOUSE_RIGHT = 3
} MouseButton;

// Mouse state
typedef struct {
    int x, y;                    // Position
    int x_delta, y_delta;        // Movement since last frame
    int wheel_x, wheel_y;       // Scroll wheel
    bool buttons[4];             // Button states
    bool just_pressed[4];        // Just pressed this frame
} InputMouse;

// Touch point
typedef struct {
    int id;              // Touch identifier
    Vector2 position;    // Current position
    Vector2 delta;       // Movement since last frame
    bool active;         // Is touch active
} TouchPoint;

// Maximum simultaneous touches
#define MAX_TOUCHES 10

// Input Manager
typedef struct {
    InputKey keys[256];          // Keyboard state
    InputMouse mouse;            // Mouse state
    TouchPoint touches[MAX_TOUCHES];  // Touch points
    int touch_count;             // Active touches
    bool text_input_active;      // Text input mode
    char text_input_buffer[256]; // Typed text
} InputManager;

// Initialize input system
void input_init(void);

// Update input state (call once per frame)
void input_update(void);

// Shutdown input system
void input_shutdown(void);

// Keyboard functions
bool input_key_pressed(KeyCode key);
bool input_key_just_pressed(KeyCode key);
bool input_key_just_released(KeyCode key);

// Mouse functions
bool input_mouse_button_pressed(MouseButton button);
bool input_mouse_button_just_pressed(MouseButton button);
Vector2 input_mouse_position(void);
Vector2 input_mouse_delta(void);
Vector2 input_mouse_wheel(void);

// Touch functions
int input_touch_count(void);
TouchPoint* input_get_touch(int index);
bool input_touch_active(int id);

// Text input
void input_text_start(void);
void input_text_stop(void);
const char* input_get_text(void);
void input_clear_text(void);

// Internal: feed raw events
void input_handle_key_down(int keycode);
void input_handle_key_up(int keycode);
void input_handle_mouse_motion(int x, int y, int xrel, int yrel);
void input_handle_mouse_button(int button, bool pressed, int x, int y);
void input_handle_mouse_wheel(int x, int y);
void input_handle_text_input(const char* text);

#endif // DECKFLOW_INPUT_H
