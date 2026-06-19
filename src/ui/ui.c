/**
 * @file ui.c
 * @brief UI System Implementation
 */

#include "ui.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Default theme
UITheme g_ui_theme_default = {
    .bg_normal = UI_COLOR(60, 60, 80),
    .bg_hover = UI_COLOR(80, 80, 100),
    .bg_pressed = UI_COLOR(40, 40, 60),
    .bg_disabled = UI_COLOR(40, 40, 50),
    .fg_normal = UI_WHITE,
    .fg_hover = UI_WHITE,
    .fg_pressed = UI_WHITE,
    .fg_disabled = UI_COLOR(128, 128, 128),
    .border = UI_COLOR(100, 100, 120)
};

// ============================================
// BASE CONTROL
// ============================================

UIControl* ui_control_create(Node* parent, const char* name) {
    UIControl* ctrl = (UIControl*)calloc(1, sizeof(UIControl));
    if (!ctrl) return NULL;
    
    ctrl->node = node_create(name, NODE_TYPE_CONTROL);
    if (parent) {
        node_add_child(parent, ctrl->node);
    }
    
    ctrl->min_size_x = 0;
    ctrl->min_size_y = 0;
    ctrl->max_size_x = 0;  // 0 = unlimited
    ctrl->max_size_y = 0;
    ctrl->margin_left = 0;
    ctrl->margin_right = 0;
    ctrl->margin_top = 0;
    ctrl->margin_bottom = 0;
    ctrl->padding_left = 5;
    ctrl->padding_right = 5;
    ctrl->padding_top = 5;
    ctrl->padding_bottom = 5;
    ctrl->bg_color = UI_TRANSPARENT;
    ctrl->border_color = UI_TRANSPARENT;
    ctrl->border_width = 0;
    ctrl->corner_radius = 0;
    ctrl->enabled = true;
    ctrl->visible = true;
    ctrl->focused = false;
    ctrl->hovered = false;
    ctrl->parent = NULL;
    
    return ctrl;
}

void ui_control_free(UIControl* ctrl) {
    if (!ctrl) return;
    if (ctrl->node) {
        node_free(ctrl->node);
    }
    free(ctrl);
}

void ui_control_set_size(UIControl* ctrl, float w, float h) {
    if (!ctrl) return;
    ctrl->node->transform.scale.x = w;
    ctrl->node->transform.scale.y = h;
}

void ui_control_set_position(UIControl* ctrl, float x, float y) {
    if (!ctrl) return;
    ctrl->node->transform.position.x = x;
    ctrl->node->transform.position.y = y;
}

void ui_control_set_min_size(UIControl* ctrl, float w, float h) {
    if (!ctrl) return;
    ctrl->min_size_x = w;
    ctrl->min_size_y = h;
}

void ui_control_set_margin(UIControl* ctrl, float l, float r, float t, float b) {
    if (!ctrl) return;
    ctrl->margin_left = l;
    ctrl->margin_right = r;
    ctrl->margin_top = t;
    ctrl->margin_bottom = b;
}

void ui_control_set_padding(UIControl* ctrl, float l, float r, float t, float b) {
    if (!ctrl) return;
    ctrl->padding_left = l;
    ctrl->padding_right = r;
    ctrl->padding_top = t;
    ctrl->padding_bottom = b;
}

void ui_control_set_bg_color(UIControl* ctrl, int color) {
    if (!ctrl) return;
    ctrl->bg_color = color;
}

void ui_control_set_border(UIControl* ctrl, int color, float width) {
    if (!ctrl) return;
    ctrl->border_color = color;
    ctrl->border_width = width;
}

void ui_control_set_enabled(UIControl* ctrl, bool enabled) {
    if (!ctrl) return;
    ctrl->enabled = enabled;
}

void ui_control_set_visible(UIControl* ctrl, bool visible) {
    if (!ctrl) return;
    ctrl->visible = visible;
    ctrl->node->visible = visible;
}

bool ui_control_hit_test(UIControl* ctrl, float x, float y) {
    if (!ctrl || !ctrl->visible || !ctrl->enabled) return false;
    
    Vector2 pos = ctrl->node->transform.position;
    Vector2 size = ctrl->node->transform.scale;
    
    return x >= pos.x && x <= pos.x + size.x &&
           y >= pos.y && y <= pos.y + size.y;
}

// ============================================
// BUTTON
// ============================================

UIButton* ui_button_create(Node* parent, const char* text) {
    UIButton* btn = (UIButton*)calloc(1, sizeof(UIButton));
    if (!btn) return NULL;
    
    btn->base.node = node_create("Button", NODE_TYPE_BUTTON);
    if (parent) {
        node_add_child(parent, btn->base.node);
    }
    
    btn->base.bg_color = g_ui_theme_default.bg_normal;
    btn->base.border_color = g_ui_theme_default.border;
    btn->base.border_width = 1;
    btn->base.corner_radius = 4;
    btn->base.node->transform.scale = vector2_new(100, 40);
    
    btn->text = text ? strdup(text) : strdup("Button");
    btn->text_color = g_ui_theme_default.fg_normal;
    btn->text_size = 14;
    btn->text_align = 1;  // center
    btn->on_click = NULL;
    btn->on_pressed = NULL;
    btn->on_released = NULL;
    
    return btn;
}

void ui_button_set_text(UIButton* btn, const char* text) {
    if (!btn) return;
    if (btn->text) free(btn->text);
    btn->text = text ? strdup(text) : strdup("");
}

void ui_button_set_text_color(UIButton* btn, int color) {
    if (!btn) return;
    btn->text_color = color;
}

void ui_button_set_on_click(UIButton* btn, void (*cb)(UIButton*)) {
    if (!btn) return;
    btn->on_click = cb;
}

void ui_button_set_on_pressed(UIButton* btn, void (*cb)(UIButton*)) {
    if (!btn) return;
    btn->on_pressed = cb;
}

void ui_button_set_on_released(UIButton* btn, void (*cb)(UIButton*)) {
    if (!btn) return;
    btn->on_released = cb;
}

// ============================================
// LABEL
// ============================================

UILabel* ui_label_create(Node* parent, const char* text) {
    UILabel* lbl = (UILabel*)calloc(1, sizeof(UILabel));
    if (!lbl) return NULL;
    
    lbl->base.node = node_create("Label", NODE_TYPE_LABEL);
    if (parent) {
        node_add_child(parent, lbl->base.node);
    }
    
    lbl->base.bg_color = UI_TRANSPARENT;
    lbl->base.node->transform.scale = vector2_new(100, 30);
    
    lbl->text = text ? strdup(text) : strdup("");
    lbl->text_color = g_ui_theme_default.fg_normal;
    lbl->text_size = 14;
    lbl->text_align = 0;
    lbl->vertical_align = 1;
    lbl->word_wrap = false;
    lbl->auto_size = true;
    
    return lbl;
}

void ui_label_set_text(UILabel* lbl, const char* text) {
    if (!lbl) return;
    if (lbl->text) free(lbl->text);
    lbl->text = text ? strdup(text) : strdup("");
}

void ui_label_set_text_color(UILabel* lbl, int color) {
    if (!lbl) return;
    lbl->text_color = color;
}

void ui_label_set_align(UILabel* lbl, int align) {
    if (!lbl) return;
    lbl->text_align = align;
}

void ui_label_set_word_wrap(UILabel* lbl, bool wrap) {
    if (!lbl) return;
    lbl->word_wrap = wrap;
}

void ui_label_set_auto_size(UILabel* lbl, bool auto_size) {
    if (!lbl) return;
    lbl->auto_size = auto_size;
}

// ============================================
// PANEL
// ============================================

UIPanel* ui_panel_create(Node* parent, const char* title) {
    UIPanel* panel = (UIPanel*)calloc(1, sizeof(UIPanel));
    if (!panel) return NULL;
    
    panel->base.node = node_create("Panel", NODE_TYPE_PANEL);
    if (parent) {
        node_add_child(parent, panel->base.node);
    }
    
    panel->base.bg_color = g_ui_theme_default.bg_normal;
    panel->base.border_color = g_ui_theme_default.border;
    panel->base.border_width = 1;
    panel->base.corner_radius = 8;
    panel->base.node->transform.scale = vector2_new(300, 200);
    
    panel->has_header = title != NULL;
    panel->title = title ? strdup(title) : NULL;
    panel->title_color = g_ui_theme_default.fg_normal;
    panel->title_size = 16;
    panel->draggable = false;
    panel->resizable = false;
    
    return panel;
}

void ui_panel_set_draggable(UIPanel* panel, bool draggable) {
    if (!panel) return;
    panel->draggable = draggable;
}

void ui_panel_set_resizable(UIPanel* panel, bool resizable) {
    if (!panel) return;
    panel->resizable = resizable;
}

// ============================================
// TEXT EDIT
// ============================================

UITextEdit* ui_text_edit_create(Node* parent, const char* placeholder) {
    (void)placeholder;
    UITextEdit* edit = (UITextEdit*)calloc(1, sizeof(UITextEdit));
    if (!edit) return NULL;
    
    edit->base.node = node_create("TextEdit", NODE_TYPE_CONTROL);
    if (parent) {
        node_add_child(parent, edit->base.node);
    }
    
    edit->base.bg_color = UI_COLOR(30, 30, 40);
    edit->base.border_color = g_ui_theme_default.border;
    edit->base.border_width = 1;
    edit->base.corner_radius = 4;
    edit->base.node->transform.scale = vector2_new(200, 30);
    
    edit->text = strdup("");
    edit->text_color = g_ui_theme_default.fg_normal;
    edit->text_size = 14;
    edit->max_length = 256;
    edit->password_mode = false;
    edit->read_only = false;
    edit->numeric_only = false;
    edit->cursor_pos = 0;
    edit->selection_start = -1;
    edit->selection_end = -1;
    
    return edit;
}

void ui_text_edit_set_text(UITextEdit* edit, const char* text) {
    if (!edit) return;
    if (edit->text) free(edit->text);
    edit->text = text ? strdup(text) : strdup("");
    edit->cursor_pos = strlen(edit->text);
}

const char* ui_text_edit_get_text(UITextEdit* edit) {
    return edit ? edit->text : "";
}

void ui_text_edit_set_max_length(UITextEdit* edit, int max_len) {
    if (!edit) return;
    edit->max_length = max_len;
}

void ui_text_edit_set_password_mode(UITextEdit* edit, bool password) {
    if (!edit) return;
    edit->password_mode = password;
}

void ui_text_edit_set_read_only(UITextEdit* edit, bool read_only) {
    if (!edit) return;
    edit->read_only = read_only;
}

// ============================================
// CHECK BOX
// ============================================

UICheckBox* ui_checkbox_create(Node* parent, const char* text) {
    UICheckBox* cb = (UICheckBox*)calloc(1, sizeof(UICheckBox));
    if (!cb) return NULL;
    
    cb->base.node = node_create("CheckBox", NODE_TYPE_CONTROL);
    if (parent) {
        node_add_child(parent, cb->base.node);
    }
    
    cb->base.bg_color = UI_TRANSPARENT;
    cb->base.node->transform.scale = vector2_new(120, 24);
    
    cb->text = text ? strdup(text) : strdup("");
    cb->text_color = g_ui_theme_default.fg_normal;
    cb->text_size = 14;
    cb->checked = false;
    cb->on_toggle = NULL;
    
    return cb;
}

void ui_checkbox_set_checked(UICheckBox* cb, bool checked) {
    if (!cb) return;
    cb->checked = checked;
}

bool ui_checkbox_is_checked(UICheckBox* cb) {
    return cb ? cb->checked : false;
}

void ui_checkbox_set_on_toggle(UICheckBox* cb, void (*cb_fn)(UICheckBox*, bool)) {
    if (!cb) return;
    cb->on_toggle = cb_fn;
}

// ============================================
// PROGRESS BAR
// ============================================

UIProgressBar* ui_progress_bar_create(Node* parent) {
    UIProgressBar* pb = (UIProgressBar*)calloc(1, sizeof(UIProgressBar));
    if (!pb) return NULL;
    
    pb->base.node = node_create("ProgressBar", NODE_TYPE_CONTROL);
    if (parent) {
        node_add_child(parent, pb->base.node);
    }
    
    pb->base.bg_color = g_ui_theme_default.bg_normal;
    pb->base.border_color = g_ui_theme_default.border;
    pb->base.border_width = 1;
    pb->base.node->transform.scale = vector2_new(200, 20);
    
    pb->min_value = 0;
    pb->max_value = 100;
    pb->value = 50;
    pb->fill_color = g_ui_theme_default.bg_hover;
    pb->show_text = true;
    
    return pb;
}

void ui_progress_bar_set_range(UIProgressBar* pb, float min, float max) {
    if (!pb) return;
    pb->min_value = min;
    pb->max_value = max;
}

void ui_progress_bar_set_value(UIProgressBar* pb, float value) {
    if (!pb) return;
    pb->value = value < pb->min_value ? pb->min_value : 
                value > pb->max_value ? pb->max_value : value;
}

float ui_progress_bar_get_value(UIProgressBar* pb) {
    return pb ? pb->value : 0;
}

void ui_progress_bar_set_fill_color(UIProgressBar* pb, int color) {
    if (!pb) return;
    pb->fill_color = color;
}

void ui_progress_bar_show_text(UIProgressBar* pb, bool show) {
    if (!pb) return;
    pb->show_text = show;
}

// ============================================
// SLIDER
// ============================================

UISlider* ui_slider_create(Node* parent, float min, float max) {
    UISlider* sl = (UISlider*)calloc(1, sizeof(UISlider));
    if (!sl) return NULL;
    
    sl->base.node = node_create("Slider", NODE_TYPE_CONTROL);
    if (parent) {
        node_add_child(parent, sl->base.node);
    }
    
    sl->base.bg_color = g_ui_theme_default.bg_normal;
    sl->base.node->transform.scale = vector2_new(200, 20);
    
    sl->min_value = min;
    sl->max_value = max;
    sl->value = (min + max) / 2;
    sl->step = 0;
    sl->on_value_changed = NULL;
    
    return sl;
}

void ui_slider_set_value(UISlider* sl, float value) {
    if (!sl) return;
    float v = value < sl->min_value ? sl->min_value : 
              value > sl->max_value ? sl->max_value : value;
    if (sl->step > 0) {
        v = ((int)((v - sl->min_value) / sl->step)) * sl->step + sl->min_value;
    }
    sl->value = v;
}

float ui_slider_get_value(UISlider* sl) {
    return sl ? sl->value : 0;
}

void ui_slider_set_on_value_changed(UISlider* sl, void (*cb)(UISlider*, float)) {
    if (!sl) return;
    sl->on_value_changed = cb;
}

// ============================================
// BOX CONTAINER
// ============================================

UIBox* ui_box_create(Node* parent, bool vertical) {
    UIBox* box = (UIBox*)calloc(1, sizeof(UIBox));
    if (!box) return NULL;
    
    box->base.base.node = node_create(vertical ? "VBox" : "HBox", NODE_TYPE_CONTROL);
    if (parent) {
        node_add_child(parent, box->base.base.node);
    }
    
    box->base.base.bg_color = UI_TRANSPARENT;
    box->base.child_capacity = 16;
    box->base.children = (UIControl**)calloc(box->base.child_capacity, sizeof(UIControl*));
    box->base.layout_type = vertical ? 1 : 2;
    box->base.spacing = 5;
    box->base.padding = 0;
    box->expand_children = false;
    
    return box;
}

void ui_box_add_child(UIBox* box, UIControl* child) {
    if (!box || !child) return;
    
    if (box->base.child_count >= box->base.child_capacity) {
        box->base.child_capacity *= 2;
        box->base.children = (UIControl**)realloc(box->base.children, 
            sizeof(UIControl*) * box->base.child_capacity);
    }
    
    box->base.children[box->base.child_count++] = child;
    child->parent = &box->base;
}

void ui_box_set_spacing(UIBox* box, float spacing) {
    if (!box) return;
    box->base.spacing = spacing;
}

void ui_box_set_expand_children(UIBox* box, bool expand) {
    if (!box) return;
    box->expand_children = expand;
}

// ============================================
// THEME
// ============================================

static UITheme g_current_theme;

void ui_theme_set_default(UITheme theme) {
    g_current_theme = theme;
}

UITheme ui_theme_get_default(void) {
    return g_current_theme;
}

void ui_theme_apply_button(UIButton* btn, UITheme theme) {
    if (!btn) return;
    btn->base.bg_color = theme.bg_normal;
    btn->base.border_color = theme.border;
    btn->text_color = theme.fg_normal;
}

void ui_theme_apply_label(UILabel* lbl, UITheme theme) {
    if (!lbl) return;
    lbl->text_color = theme.fg_normal;
}

// ============================================
// RENDERING & INPUT (Stubs)
// ============================================

void ui_render(UIControl* root, void* renderer, void* camera) {
    (void)root; (void)renderer; (void)camera;
    // In production, this would render all UI controls
    // using SDL_RenderFillRect, SDL_RenderDrawLine, etc.
}

void ui_handle_input(UIControl* root, float mouse_x, float mouse_y, bool mouse_down) {
    (void)root; (void)mouse_x; (void)mouse_y; (void)mouse_down;
    // In production, this would handle mouse hover, click, etc.
}
