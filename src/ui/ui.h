/**
 * @file ui.h
 * @brief UI System - Buttons, Labels, Panels, Menus
 */

#ifndef DECKFLOW_UI_H
#define DECKFLOW_UI_H

#include <stdbool.h>
#include "../nodes/node.h"

// UI Anchor points
typedef enum {
    UI_ANCHOR_TOP_LEFT,
    UI_ANCHOR_TOP_CENTER,
    UI_ANCHOR_TOP_RIGHT,
    UI_ANCHOR_LEFT_CENTER,
    UI_ANCHOR_CENTER,
    UI_ANCHOR_RIGHT_CENTER,
    UI_ANCHOR_BOTTOM_LEFT,
    UI_ANCHOR_BOTTOM_CENTER,
    UI_ANCHOR_BOTTOM_RIGHT
} UIAnchor;

// UI Size flags
typedef enum {
    UI_SIZE_FIXED = 0,
    UI_SIZE_FILL_H = 1 << 0,
    UI_SIZE_FILL_V = 1 << 1,
    UI_SIZE_EXPAND = 1 << 2
} UISizeFlags;

// UI Theme colors
typedef struct {
    int bg_normal;
    int bg_hover;
    int bg_pressed;
    int bg_disabled;
    int fg_normal;
    int fg_hover;
    int fg_pressed;
    int fg_disabled;
    int border;
} UITheme;

// Default theme
extern UITheme g_ui_theme_default;

// Color helpers
#define UI_COLOR(r,g,b) ((r) | ((g) << 8) | ((b) << 16) | (0xFF << 24))
#define UI_COLOR_RGBA(r,g,b,a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
#define UI_COLOR_R(c) ((c) & 0xFF)
#define UI_COLOR_G(c) (((c) >> 8) & 0xFF)
#define UI_COLOR_B(c) (((c) >> 16) & 0xFF)
#define UI_COLOR_A(c) (((c) >> 24) & 0xFF)

// Common colors
#define UI_WHITE UI_COLOR(255, 255, 255)
#define UI_BLACK UI_COLOR(0, 0, 0)
#define UI_GRAY UI_COLOR(128, 128, 128)
#define UI_DARK_GRAY UI_COLOR(64, 64, 64)
#define UI_LIGHT_GRAY UI_COLOR(200, 200, 200)
#define UI_RED UI_COLOR(200, 80, 80)
#define UI_GREEN UI_COLOR(80, 200, 80)
#define UI_BLUE UI_COLOR(80, 80, 200)
#define UI_YELLOW UI_COLOR(255, 255, 0)
#define UI_ORANGE UI_COLOR(255, 165, 0)
#define UI_PURPLE UI_COLOR(160, 32, 240)
#define UI_TRANSPARENT UI_COLOR_RGBA(0, 0, 0, 0)

// UI Control base (extends Node)
typedef struct UIControl {
    Node* node;
    
    // Layout
    float min_size_x;
    float min_size_y;
    float max_size_x;
    float max_size_y;
    float margin_left;
    float margin_right;
    float margin_top;
    float margin_bottom;
    float padding_left;
    float padding_right;
    float padding_top;
    float padding_bottom;
    
    // Appearance
    int bg_color;
    int border_color;
    int border_width;
    float corner_radius;
    
    // State
    bool enabled;
    bool visible;
    bool focused;
    bool hovered;
    
    // Parent container
    struct UIControl* parent;
} UIControl;

// Button
typedef struct {
    UIControl base;
    
    // Text
    char* text;
    int text_color;
    float text_size;
    int text_align;  // 0=left, 1=center, 2=right
    
    // Callbacks
    void (*on_click)(struct UIButton* btn);
    void (*on_pressed)(struct UIButton* btn);
    void (*on_released)(struct UIButton* btn);
} UIButton;

// Label
typedef struct {
    UIControl base;
    
    char* text;
    int text_color;
    float text_size;
    int text_align;  // 0=left, 1=center, 2=right
    int vertical_align;  // 0=top, 1=center, 2=bottom
    bool word_wrap;
    bool auto_size;  // Size to fit text
} UILabel;

// Panel
typedef struct {
    UIControl base;
    
    bool has_header;
    char* title;
    int title_color;
    float title_size;
    bool draggable;
    bool resizable;
} UIPanel;

// TextEdit
typedef struct {
    UIControl base;
    
    char* text;
    int text_color;
    float text_size;
    int max_length;
    bool password_mode;
    bool read_only;
    bool numeric_only;
    
    // Cursor
    int cursor_pos;
    int selection_start;
    int selection_end;
} UITextEdit;

// CheckBox
typedef struct {
    UIControl base;
    
    char* text;
    int text_color;
    float text_size;
    bool checked;
    
    void (*on_toggle)(struct UICheckBox* cb, bool checked);
} UICheckBox;

// ProgressBar
typedef struct {
    UIControl base;
    
    float min_value;
    float max_value;
    float value;
    int fill_color;
    bool show_text;
} UIProgressBar;

// Slider
typedef struct {
    UIControl base;
    
    float min_value;
    float max_value;
    float value;
    float step;
    
    void (*on_value_changed)(struct UISlider* sl, float value);
} UISlider;

// ScrollBar
typedef struct {
    UIControl base;
    
    float min_value;
    float max_value;
    float value;
    float page_size;
    bool horizontal;
} UIScrollBar;

// Container base
typedef struct UIContainer {
    UIControl base;
    
    // Children
    UIControl** children;
    int child_count;
    int child_capacity;
    
    // Layout
    int layout_type;  // 0=none, 1=vbox, 2=hbox, 3=grid
    float spacing;
    int padding;
} UIContainer;

// VBox/HBox
typedef struct {
    UIContainer base;
    
    bool expand_children;
} UIBox;

// Grid
typedef struct {
    UIContainer base;
    
    int columns;
    float row_height;
} UIGrid;

// Menu Item
typedef struct {
    char* text;
    char* shortcut;
    bool enabled;
    bool checked;
    void (*on_click)(void);
    
    // Submenu
    struct UIMenu* submenu;
} UIMenuItem;

// Menu
typedef struct UIMenu {
    UIControl base;
    
    UIMenuItem* items;
    int item_count;
    
    int hovered_index;
} UIMenu;

// MenuBar
typedef struct {
    UIControl base;
    
    UIMenu** menus;
    int menu_count;
    int hovered_index;
} UIMenuBar;

// ============================================
// CONTROL FUNCTIONS
// ============================================

// Base control
UIControl* ui_control_create(Node* parent, const char* name);
void ui_control_free(UIControl* ctrl);
void ui_control_set_size(UIControl* ctrl, float w, float h);
void ui_control_set_position(UIControl* ctrl, float x, float y);
void ui_control_set_min_size(UIControl* ctrl, float w, float h);
void ui_control_set_margin(UIControl* ctrl, float l, float r, float t, float b);
void ui_control_set_padding(UIControl* ctrl, float l, float r, float t, float b);
void ui_control_set_bg_color(UIControl* ctrl, int color);
void ui_control_set_border(UIControl* ctrl, int color, float width);
void ui_control_set_enabled(UIControl* ctrl, bool enabled);
void ui_control_set_visible(UIControl* ctrl, bool visible);

// Hit test
bool ui_control_hit_test(UIControl* ctrl, float x, float y);

// ============================================
// BUTTON
// ============================================

UIButton* ui_button_create(Node* parent, const char* text);
void ui_button_set_text(UIButton* btn, const char* text);
void ui_button_set_text_color(UIButton* btn, int color);
void ui_button_set_on_click(UIButton* btn, void (*cb)(UIButton*));
void ui_button_set_on_pressed(UIButton* btn, void (*cb)(UIButton*));
void ui_button_set_on_released(UIButton* btn, void (*cb)(UIButton*));

// ============================================
// LABEL
// ============================================

UILabel* ui_label_create(Node* parent, const char* text);
void ui_label_set_text(UILabel* lbl, const char* text);
void ui_label_set_text_color(UILabel* lbl, int color);
void ui_label_set_align(UILabel* lbl, int align);
void ui_label_set_word_wrap(UILabel* lbl, bool wrap);
void ui_label_set_auto_size(UILabel* lbl, bool auto_size);

// ============================================
// PANEL
// ============================================

UIPanel* ui_panel_create(Node* parent, const char* title);
void ui_panel_set_draggable(UIPanel* panel, bool draggable);
void ui_panel_set_resizable(UIPanel* panel, bool resizable);

// ============================================
// TEXT EDIT
// ============================================

UITextEdit* ui_text_edit_create(Node* parent, const char* placeholder);
void ui_text_edit_set_text(UITextEdit* edit, const char* text);
void ui_text_edit_set_max_length(UITextEdit* edit, int max_len);
void ui_text_edit_set_password_mode(UITextEdit* edit, bool password);
void ui_text_edit_set_read_only(UITextEdit* edit, bool read_only);
const char* ui_text_edit_get_text(UITextEdit* edit);

// ============================================
// CHECK BOX
// ============================================

UICheckBox* ui_checkbox_create(Node* parent, const char* text);
void ui_checkbox_set_checked(UICheckBox* cb, bool checked);
bool ui_checkbox_is_checked(UICheckBox* cb);
void ui_checkbox_set_on_toggle(UICheckBox* cb, void (*cb_fn)(UICheckBox*, bool));

// ============================================
// PROGRESS BAR
// ============================================

UIProgressBar* ui_progress_bar_create(Node* parent);
void ui_progress_bar_set_range(UIProgressBar* pb, float min, float max);
void ui_progress_bar_set_value(UIProgressBar* pb, float value);
float ui_progress_bar_get_value(UIProgressBar* pb);
void ui_progress_bar_set_fill_color(UIProgressBar* pb, int color);
void ui_progress_bar_show_text(UIProgressBar* pb, bool show);

// ============================================
// SLIDER
// ============================================

UISlider* ui_slider_create(Node* parent, float min, float max);
void ui_slider_set_value(UISlider* sl, float value);
float ui_slider_get_value(UISlider* sl);
void ui_slider_set_on_value_changed(UISlider* sl, void (*cb)(UISlider*, float));

// ============================================
// BOX CONTAINER
// ============================================

UIBox* ui_box_create(Node* parent, bool vertical);
void ui_box_add_child(UIBox* box, UIControl* child);
void ui_box_set_spacing(UIBox* box, float spacing);
void ui_box_set_expand_children(UIBox* box, bool expand);

// ============================================
// MENU BAR
// ============================================

UIMenuBar* ui_menu_bar_create(Node* parent);
void ui_menu_bar_add_menu(UIMenuBar* mb, const char* name);
void ui_menu_add_item(UIMenu* menu, const char* text, const char* shortcut, void (*on_click)(void));

// ============================================
// THEME
// ============================================

void ui_theme_set_default(UITheme theme);
UITheme ui_theme_get_default(void);
void ui_theme_apply_button(UIButton* btn, UITheme theme);
void ui_theme_apply_label(UILabel* lbl, UITheme theme);

// ============================================
// RENDERING
// ============================================

void ui_render(UIControl* root, void* renderer, void* camera);
void ui_handle_input(UIControl* root, float mouse_x, float mouse_y, bool mouse_down);

#endif // DECKFLOW_UI_H
