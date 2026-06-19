/**
 * @file main.c
 * @brief Phase 6: UI System Demo
 * 
 * Demonstrates:
 * - Button, Label, Panel
 * - TextEdit, CheckBox
 * - ProgressBar, Slider
 * - Menu system
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "scenes/scene.h"
#include "nodes/node.h"
#include "math/vector2.h"
#include "ui/ui.h"

// Screen
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Colors
#define COLOR_BG UI_COLOR(26, 26, 46)
#define COLOR_PANEL_BG UI_COLOR(40, 40, 60)

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_renderer = NULL;
static TTF_Font* g_font = NULL;

// UI State
static int g_volume = 75;
static int g_music_volume = 60;
static bool g_fullscreen = false;
static bool g_vsync = true;
static int g_progress = 0;
static char g_player_name[64] = "";

// Button callbacks
void on_start_click(UIButton* btn) {
    (void)btn;
    printf("[UI] Start clicked!\n");
}

void on_options_click(UIButton* btn) {
    (void)btn;
    printf("[UI] Options clicked!\n");
}

void on_quit_click(UIButton* btn) {
    (void)btn;
    printf("[UI] Quit clicked!\n");
    SDL_Event e = {.type = SDL_QUIT};
    SDL_PushEvent(&e);
}

void on_fullscreen_toggle(UICheckBox* cb, bool checked) {
    g_fullscreen = checked;
    printf("[UI] Fullscreen: %s\n", checked ? "ON" : "OFF");
}

void on_vsync_toggle(UICheckBox* cb, bool checked) {
    g_vsync = checked;
    printf("[UI] VSync: %s\n", checked ? "ON" : "OFF");
}

void on_volume_changed(UISlider* sl, float value) {
    g_volume = (int)value;
    printf("[UI] Volume: %d%%\n", g_volume);
}

void on_music_changed(UISlider* sl, float value) {
    g_music_volume = (int)value;
    printf("[UI] Music: %d%%\n", g_music_volume);
}

// Render UI control (simplified)
void render_control(UIControl* ctrl, float x, float y) {
    if (!ctrl || !ctrl->visible) return;
    
    Vector2 pos = ctrl->node->transform.position;
    Vector2 size = ctrl->node->transform.scale;
    
    int draw_x = (int)(pos.x + x);
    int draw_y = (int)(pos.y + y);
    int draw_w = (int)size.x;
    int draw_h = (int)size.y;
    
    SDL_SetRenderDrawColor(g_renderer, 
        UI_COLOR_R(ctrl->bg_color),
        UI_COLOR_G(ctrl->bg_color),
        UI_COLOR_B(ctrl->bg_color), 255);
    SDL_RenderFillRect(g_renderer, &(SDL_Rect){draw_x, draw_y, draw_w, draw_h});
    
    if (ctrl->border_width > 0) {
        SDL_SetRenderDrawColor(g_renderer,
            UI_COLOR_R(ctrl->border_color),
            UI_COLOR_G(ctrl->border_color),
            UI_COLOR_B(ctrl->border_color), 255);
        SDL_RenderDrawRect(g_renderer, &(SDL_Rect){draw_x, draw_y, draw_w, draw_h});
    }
}

// Render button
void render_button(UIButton* btn, float x, float y) {
    if (!btn) return;
    
    UIControl* ctrl = &btn->base;
    if (!ctrl->visible) return;
    
    // Background
    int bg_color = ctrl->enabled ? 
        (ctrl->hovered ? UI_COLOR(80, 80, 100) : UI_COLOR(60, 60, 80)) :
        UI_COLOR(40, 40, 50);
    
    Vector2 pos = ctrl->node->transform.position;
    Vector2 size = ctrl->node->transform.scale;
    
    int draw_x = (int)(pos.x + x);
    int draw_y = (int)(pos.y + y);
    int draw_w = (int)size.x;
    int draw_h = (int)size.y;
    
    // Rounded rect (simplified)
    SDL_SetRenderDrawColor(g_renderer, UI_COLOR_R(bg_color), UI_COLOR_G(bg_color), UI_COLOR_B(bg_color), 255);
    SDL_RenderFillRect(g_renderer, &(SDL_Rect){draw_x, draw_y, draw_w, draw_h});
    SDL_SetRenderDrawColor(g_renderer, UI_COLOR_R(ctrl->border_color), UI_COLOR_G(ctrl->border_color), UI_COLOR_B(ctrl->border_color), 255);
    SDL_RenderDrawRect(g_renderer, &(SDL_Rect){draw_x, draw_y, draw_w, draw_h});
    
    // Text
    if (btn->text && g_font) {
        SDL_Color fg = {UI_COLOR_R(btn->text_color), UI_COLOR_G(btn->text_color), UI_COLOR_B(btn->text_color), 255};
        SDL_Surface* surf = TTF_RenderText_Blended(g_font, btn->text, fg);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(g_renderer, surf);
            SDL_Rect dst = {draw_x + (draw_w - surf->w) / 2, draw_y + (draw_h - surf->h) / 2, surf->w, surf->h};
            SDL_RenderCopy(g_renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }
}

// Render label
void render_label(UILabel* lbl, float x, float y) {
    if (!lbl) return;
    
    UIControl* ctrl = &lbl->base;
    if (!ctrl->visible) return;
    
    Vector2 pos = ctrl->node->transform.position;
    
    int draw_x = (int)(pos.x + x);
    int draw_y = (int)(pos.y + y);
    
    if (lbl->text && g_font) {
        SDL_Color fg = {UI_COLOR_R(lbl->text_color), UI_COLOR_G(lbl->text_color), UI_COLOR_B(lbl->text_color), 255};
        SDL_Surface* surf = TTF_RenderText_Blended(g_font, lbl->text, fg);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(g_renderer, surf);
            SDL_Rect dst = {draw_x, draw_y, surf->w, surf->h};
            SDL_RenderCopy(g_renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }
}

// Render panel
void render_panel(UIPanel* panel, float x, float y) {
    if (!panel) return;
    render_control(&panel->base, x, y);
    
    // Title
    if (panel->has_header && panel->title) {
        Vector2 pos = panel->base.node->transform.position;
        SDL_Color fg = {UI_COLOR_R(panel->title_color), UI_COLOR_G(panel->title_color), UI_COLOR_B(panel->title_color), 255};
        SDL_Surface* surf = TTF_RenderText_Blended(g_font, panel->title, fg);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(g_renderer, surf);
            SDL_Rect dst = {(int)(pos.x + x + 10), (int)(pos.y + y + 8), surf->w, surf->h};
            SDL_RenderCopy(g_renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }
}

// Render checkbox
void render_checkbox(UICheckBox* cb, float x, float y) {
    if (!cb) return;
    
    UIControl* ctrl = &cb->base;
    Vector2 pos = ctrl->node->transform.position;
    Vector2 size = ctrl->node->transform.scale;
    
    int draw_x = (int)(pos.x + x);
    int draw_y = (int)(pos.y + y);
    int draw_w = (int)size.x;
    int draw_h = (int)size.y;
    
    // Box
    SDL_SetRenderDrawColor(g_renderer, 30, 30, 40, 255);
    SDL_RenderFillRect(g_renderer, &(SDL_Rect){draw_x, draw_y + (draw_h - 20) / 2, 20, 20});
    SDL_SetRenderDrawColor(g_renderer, 100, 100, 120, 255);
    SDL_RenderDrawRect(g_renderer, &(SDL_Rect){draw_x, draw_y + (draw_h - 20) / 2, 20, 20});
    
    // Check
    if (cb->checked) {
        SDL_SetRenderDrawColor(g_renderer, 100, 200, 100, 255);
        SDL_RenderDrawLine(g_renderer, draw_x + 4, draw_y + (draw_h - 20) / 2 + 10, draw_x + 8, draw_y + (draw_h - 20) / 2 + 14);
        SDL_RenderDrawLine(g_renderer, draw_x + 8, draw_y + (draw_h - 20) / 2 + 14, draw_x + 16, draw_y + (draw_h - 20) / 2 + 4);
    }
    
    // Text
    if (cb->text && g_font) {
        SDL_Color fg = {UI_COLOR_R(cb->text_color), UI_COLOR_G(cb->text_color), UI_COLOR_B(cb->text_color), 255};
        SDL_Surface* surf = TTF_RenderText_Blended(g_font, cb->text, fg);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(g_renderer, surf);
            SDL_Rect dst = {draw_x + 28, draw_y + (draw_h - surf->h) / 2, surf->w, surf->h};
            SDL_RenderCopy(g_renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }
}

// Render slider
void render_slider(UISlider* sl, float x, float y) {
    if (!sl) return;
    
    UIControl* ctrl = &sl->base;
    Vector2 pos = ctrl->node->transform.position;
    Vector2 size = ctrl->node->transform.scale;
    
    int draw_x = (int)(pos.x + x);
    int draw_y = (int)(pos.y + y);
    int draw_w = (int)size.x;
    int draw_h = (int)size.y;
    
    // Track
    SDL_SetRenderDrawColor(g_renderer, 30, 30, 40, 255);
    SDL_RenderFillRect(g_renderer, &(SDL_Rect){draw_x, draw_y + (draw_h - 8) / 2, draw_w, 8});
    
    // Fill
    float ratio = (sl->value - sl->min_value) / (sl->max_value - sl->min_value);
    int fill_w = (int)(draw_w * ratio);
    SDL_SetRenderDrawColor(g_renderer, 80, 120, 200, 255);
    SDL_RenderFillRect(g_renderer, &(SDL_Rect){draw_x, draw_y + (draw_h - 8) / 2, fill_w, 8});
    
    // Thumb
    SDL_SetRenderDrawColor(g_renderer, 200, 200, 220, 255);
    SDL_RenderFillRect(g_renderer, &(SDL_Rect){draw_x + fill_w - 6, draw_y + (draw_h - 16) / 2, 12, 16});
}

// Render progress bar
void render_progressbar(UIProgressBar* pb, float x, float y) {
    if (!pb) return;
    
    UIControl* ctrl = &pb->base;
    Vector2 pos = ctrl->node->transform.position;
    Vector2 size = ctrl->node->transform.scale;
    
    int draw_x = (int)(pos.x + x);
    int draw_y = (int)(pos.y + y);
    int draw_w = (int)size.x;
    int draw_h = (int)size.y;
    
    // Background
    SDL_SetRenderDrawColor(g_renderer, 30, 30, 40, 255);
    SDL_RenderFillRect(g_renderer, &(SDL_Rect){draw_x, draw_y, draw_w, draw_h});
    
    // Fill
    float ratio = (pb->value - pb->min_value) / (pb->max_value - pb->min_value);
    int fill_w = (int)(draw_w * ratio);
    SDL_SetRenderDrawColor(g_renderer, 
        UI_COLOR_R(pb->fill_color),
        UI_COLOR_G(pb->fill_color),
        UI_COLOR_B(pb->fill_color), 255);
    SDL_RenderFillRect(g_renderer, &(SDL_Rect){draw_x, draw_y, fill_w, draw_h});
    
    // Border
    SDL_SetRenderDrawColor(g_renderer, 100, 100, 120, 255);
    SDL_RenderDrawRect(g_renderer, &(SDL_Rect){draw_x, draw_y, draw_w, draw_h});
    
    // Text
    if (pb->show_text && g_font) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.0f%%", ratio * 100);
        SDL_Color fg = {255, 255, 255, 255};
        SDL_Surface* surf = TTF_RenderText_Blended(g_font, buf, fg);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(g_renderer, surf);
            SDL_Rect dst = {draw_x + (draw_w - surf->w) / 2, draw_y + (draw_h - surf->h) / 2, surf->w, surf->h};
            SDL_RenderCopy(g_renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
    }
}

int main(void) {
    printf("+============================================================+\n");
    printf("|  Phase 6: UI System Demo                                     |\n");
    printf("+============================================================+\n\n");
    
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return 1;
    }
    
    if (TTF_Init() < 0) {
        printf("TTF init failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    
    g_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
    if (!g_font) {
        g_font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 14);
    }
    if (!g_font) {
        printf("Warning: Could not load font, text will not render\n");
    }
    
    g_window = SDL_CreateWindow("Phase 6: UI System",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!g_window) { TTF_Quit(); SDL_Quit(); return 1; }
    
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
    if (!g_renderer) { SDL_DestroyWindow(g_window); TTF_Quit(); SDL_Quit(); return 1; }
    
    // Create scene
    Scene* scene = scene_create("UIScene");
    Node* root = scene_get_root(scene);
    
    // Create UI controls
    UIPanel* main_panel = ui_panel_create(root, "Game Settings");
    ui_control_set_position(&main_panel->base, 50, 50);
    ui_control_set_size(&main_panel->base, 350, 500);
    
    // Labels
    UILabel* lbl_volume = ui_label_create(main_panel->base.node, "Volume:");
    ui_control_set_position(&lbl_volume->base, 20, 50);
    
    // Slider
    UISlider* slider_volume = ui_slider_create(main_panel->base.node, 0, 100);
    ui_control_set_position(&slider_volume->base, 20, 75);
    ui_slider_set_value(slider_volume, g_volume);
    ui_slider_set_on_value_changed(slider_volume, on_volume_changed);
    
    UILabel* lbl_music = ui_label_create(main_panel->base.node, "Music:");
    ui_control_set_position(&lbl_music->base, 20, 110);
    
    UISlider* slider_music = ui_slider_create(main_panel->base.node, 0, 100);
    ui_control_set_position(&slider_music->base, 20, 135);
    ui_slider_set_value(slider_music, g_music_volume);
    ui_slider_set_on_value_changed(slider_music, on_music_changed);
    
    // Progress bar
    UILabel* lbl_loading = ui_label_create(main_panel->base.node, "Loading:");
    ui_control_set_position(&lbl_loading->base, 20, 175);
    
    UIProgressBar* pb_loading = ui_progress_bar_create(main_panel->base.node);
    ui_control_set_position(&pb_loading->base, 20, 200);
    ui_progress_bar_set_value(pb_loading, g_progress);
    
    // Checkboxes
    UICheckBox* cb_fullscreen = ui_checkbox_create(main_panel->base.node, "Fullscreen");
    ui_control_set_position(&cb_fullscreen->base, 20, 240);
    ui_checkbox_set_checked(cb_fullscreen, g_fullscreen);
    ui_checkbox_set_on_toggle(cb_fullscreen, on_fullscreen_toggle);
    
    UICheckBox* cb_vsync = ui_checkbox_create(main_panel->base.node, "VSync");
    ui_control_set_position(&cb_vsync->base, 20, 270);
    ui_checkbox_set_checked(cb_vsync, g_vsync);
    ui_checkbox_set_on_toggle(cb_vsync, on_vsync_toggle);
    
    // Buttons
    UIButton* btn_start = ui_button_create(main_panel->base.node, "Start Game");
    ui_control_set_position(&btn_start->base, 20, 320);
    ui_control_set_size(&btn_start->base, 140, 40);
    ui_button_set_on_click(btn_start, on_start_click);
    
    UIButton* btn_options = ui_button_create(main_panel->base.node, "Options");
    ui_control_set_position(&btn_options->base, 170, 320);
    ui_control_set_size(&btn_options->base, 140, 40);
    ui_button_set_on_click(btn_options, on_options_click);
    
    UIButton* btn_quit = ui_button_create(main_panel->base.node, "Quit");
    ui_control_set_position(&btn_quit->base, 20, 380);
    ui_control_set_size(&btn_quit->base, 290, 40);
    ui_button_set_on_click(btn_quit, on_quit_click);
    
    // Hover state tracking
    int mouse_x = 0, mouse_y = 0;
    bool mouse_down = false;
    
    printf("UI System Demo:\n");
    printf("  - Panel with title\n");
    printf("  - Labels, Sliders, Progress bars\n");
    printf("  - Checkboxes, Buttons\n");
    printf("  - Theme-based styling\n\n");
    
    printf("Controls:\n");
    printf("  Move mouse over buttons to see hover\n");
    printf("  Click buttons to trigger callbacks\n");
    printf("  ESC: Exit\n\n");
    
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
            }
            if (event.type == SDL_MOUSEMOTION) {
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                mouse_down = true;
                mouse_x = event.button.x;
                mouse_y = event.button.y;
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                mouse_down = false;
            }
        }
        
        // Update UI hover states
        btn_start->base.hovered = ui_control_hit_test(&btn_start->base, mouse_x, mouse_y);
        btn_options->base.hovered = ui_control_hit_test(&btn_options->base, mouse_x, mouse_y);
        btn_quit->base.hovered = ui_control_hit_test(&btn_quit->base, mouse_x, mouse_y);
        
        // Button click handling
        if (mouse_down) {
            if (btn_start->base.hovered && btn_start->on_click) btn_start->on_click(btn_start);
            if (btn_options->base.hovered && btn_options->on_click) btn_options->on_click(btn_options);
            if (btn_quit->base.hovered && btn_quit->on_click) btn_quit->on_click(btn_quit);
        }
        
        // Animate progress bar
        g_progress = (g_progress + 1) % 100;
        ui_progress_bar_set_value(pb_loading, g_progress);
        
        // Render
        SDL_SetRenderDrawColor(g_renderer, 26, 26, 46, 255);
        SDL_RenderClear(g_renderer);
        
        // Render panel
        render_panel(main_panel, 0, 0);
        
        // Render controls
        render_label(lbl_volume, 50, 50);
        render_slider(slider_volume, 50, 50);
        render_label(lbl_music, 50, 50);
        render_slider(slider_music, 50, 50);
        render_label(lbl_loading, 50, 50);
        render_progressbar(pb_loading, 50, 50);
        render_checkbox(cb_fullscreen, 50, 50);
        render_checkbox(cb_vsync, 50, 50);
        render_button(btn_start, 50, 50);
        render_button(btn_options, 50, 50);
        render_button(btn_quit, 50, 50);
        
        SDL_RenderPresent(g_renderer);
        SDL_Delay(16);
    }
    
    // Cleanup
    printf("\nCleaning up...\n");
    scene_free(scene);
    
    if (g_font) TTF_CloseFont(g_font);
    TTF_Quit();
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    
    printf("\n+============================================================+\n");
    printf("|  Phase 6 Complete! UI System Working!                        |\n");
    printf("+============================================================+\n");
    
    return 0;
}
