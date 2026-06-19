/**
 * @file input.c
 * @brief Input System Implementation
 */

#include "input.h"
#include <string.h>
#include <stdlib.h>

static InputManager g_input = {0};

void input_init(void) {
    memset(&g_input, 0, sizeof(InputManager));
    g_input.mouse.wheel_x = 0;
    g_input.mouse.wheel_y = 0;
}

void input_update(void) {
    // Clear "just pressed/released" states for next frame
    for (int i = 0; i < 256; i++) {
        g_input.keys[i].just_pressed = false;
        g_input.keys[i].just_released = false;
    }
    
    // Clear mouse deltas and wheel
    g_input.mouse.x_delta = 0;
    g_input.mouse.y_delta = 0;
    g_input.mouse.wheel_x = 0;
    g_input.mouse.wheel_y = 0;
    
    // Clear touch deltas
    for (int i = 0; i < MAX_TOUCHES; i++) {
        if (g_input.touches[i].active) {
            g_input.touches[i].delta = vector2_zero();
        }
    }
}

void input_shutdown(void) {
    memset(&g_input, 0, sizeof(InputManager));
}

bool input_key_pressed(KeyCode key) {
    if (key < 0 || key >= 256) return false;
    return g_input.keys[key].is_pressed;
}

bool input_key_just_pressed(KeyCode key) {
    if (key < 0 || key >= 256) return false;
    return g_input.keys[key].just_pressed;
}

bool input_key_just_released(KeyCode key) {
    if (key < 0 || key >= 256) return false;
    return g_input.keys[key].just_released;
}

bool input_mouse_button_pressed(MouseButton button) {
    if (button < 1 || button > 3) return false;
    return g_input.mouse.buttons[button];
}

bool input_mouse_button_just_pressed(MouseButton button) {
    if (button < 1 || button > 3) return false;
    return g_input.mouse.just_pressed[button];
}

Vector2 input_mouse_position(void) {
    return vector2_new((float)g_input.mouse.x, (float)g_input.mouse.y);
}

Vector2 input_mouse_delta(void) {
    return vector2_new((float)g_input.mouse.x_delta, (float)g_input.mouse.y_delta);
}

Vector2 input_mouse_wheel(void) {
    return vector2_new((float)g_input.mouse.wheel_x, (float)g_input.mouse.wheel_y);
}

int input_touch_count(void) {
    return g_input.touch_count;
}

TouchPoint* input_get_touch(int index) {
    if (index < 0 || index >= MAX_TOUCHES) return NULL;
    if (!g_input.touches[index].active) return NULL;
    return &g_input.touches[index];
}

bool input_touch_active(int id) {
    for (int i = 0; i < MAX_TOUCHES; i++) {
        if (g_input.touches[i].active && g_input.touches[i].id == id) {
            return true;
        }
    }
    return false;
}

void input_text_start(void) {
    g_input.text_input_active = true;
    g_input.text_input_buffer[0] = '\0';
}

void input_text_stop(void) {
    g_input.text_input_active = false;
}

const char* input_get_text(void) {
    return g_input.text_input_buffer;
}

void input_clear_text(void) {
    g_input.text_input_buffer[0] = '\0';
}

// Internal event handlers
void input_handle_key_down(int keycode) {
    if (keycode < 0 || keycode >= 256) return;
    if (!g_input.keys[keycode].is_pressed) {
        g_input.keys[keycode].just_pressed = true;
    }
    g_input.keys[keycode].is_pressed = true;
}

void input_handle_key_up(int keycode) {
    if (keycode < 0 || keycode >= 256) return;
    g_input.keys[keycode].is_pressed = false;
    g_input.keys[keycode].just_released = true;
}

void input_handle_mouse_motion(int x, int y, int xrel, int yrel) {
    g_input.mouse.x = x;
    g_input.mouse.y = y;
    g_input.mouse.x_delta += xrel;
    g_input.mouse.y_delta += yrel;
}

void input_handle_mouse_button(int button, bool pressed, int x, int y) {
    if (button < 1 || button > 3) return;
    g_input.mouse.buttons[button] = pressed;
    g_input.mouse.just_pressed[button] = pressed;
    g_input.mouse.x = x;
    g_input.mouse.y = y;
}

void input_handle_mouse_wheel(int x, int y) {
    g_input.mouse.wheel_x += x;
    g_input.mouse.wheel_y += y;
}

void input_handle_text_input(const char* text) {
    if (!g_input.text_input_active) return;
    size_t len = strlen(g_input.text_input_buffer);
    size_t text_len = strlen(text);
    if (len + text_len < sizeof(g_input.text_input_buffer) - 1) {
        strcat(g_input.text_input_buffer, text);
    }
}
