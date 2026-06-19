# Phase 6: UI System

## 📋 Visão Geral

A Phase 6 implementa um sistema completo de UI (User Interface) para o motor de jogo:

| Componente | Descrição |
|------------|-----------|
| **Button** | Botões clicáveis com callbacks |
| **Label** | Texto estático |
| **Panel** | Container com título |
| **TextEdit** | Campo de texto editável |
| **CheckBox** | Caixa de seleção |
| **ProgressBar** | Barra de progresso |
| **Slider** | Controle deslizante |
| **Box** | Container VBox/HBox |

## 🏗️ Arquitetura

```
┌─────────────────────────────────────────────────────────────┐
│                     UIControl (base)                         │
│  - position, size                                           │
│  - margins, padding                                         │
│  - colors, borders                                         │
│  - enabled, visible, hovered                               │
└─────────────────────────────────────────────────────────────┘
         │
    ┌────┴────┬──────────┬───────────┬──────────┐
    ▼         ▼          ▼           ▼          ▼
┌───────┐ ┌──────┐ ┌────────┐ ┌─────────┐ ┌────────┐
│Button │ │Label │ │Panel   │ │TextEdit │ │CheckBox│
│       │ │      │ │        │ │         │ │        │
│text   │ │text  │ │title   │ │text     │ │checked │
│on_click│ │      │ │draggable│ │cursor  │ │on_toggle│
└───────┘ └──────┘ └────────┘ └─────────┘ └────────┘
```

## 📦 API de Controles

### Button
```c
UIButton* btn = ui_button_create(parent, "Click Me");
ui_control_set_position(&btn->base, 100, 100);
ui_control_set_size(&btn->base, 120, 40);
ui_button_set_on_click(btn, my_callback);

void my_callback(UIButton* btn) {
    printf("Button clicked: %s\n", btn->text);
}
```

### Label
```c
UILabel* lbl = ui_label_create(parent, "Score: 0");
ui_label_set_text(lbl, "Score: 100");
ui_label_set_text_color(lbl, UI_COLOR(255, 255, 0));
```

### Panel
```c
UIPanel* panel = ui_panel_create(parent, "Settings");
ui_panel_set_draggable(panel, true);
```

### CheckBox
```c
UICheckBox* cb = ui_checkbox_create(parent, "Fullscreen");
ui_checkbox_set_on_toggle(cb, on_toggle);

void on_toggle(UICheckBox* cb, bool checked) {
    printf("Fullscreen: %s\n", checked ? "ON" : "OFF");
}
```

### Slider
```c
UISlider* sl = ui_slider_create(parent, 0, 100);
ui_slider_set_value(sl, 50);
ui_slider_set_on_value_changed(sl, on_changed);

void on_changed(UISlider* sl, float value) {
    printf("Volume: %.0f%%\n", value);
}
```

### ProgressBar
```c
UIProgressBar* pb = ui_progress_bar_create(parent);
ui_progress_bar_set_range(pb, 0, 100);
ui_progress_bar_set_value(pb, 75);
ui_progress_bar_show_text(pb, true);
```

## 🎨 Cores e Temas

```c
// Cores predefinidas
UI_WHITE, UI_BLACK, UI_GRAY
UI_RED, UI_GREEN, UI_BLUE
UI_YELLOW, UI_ORANGE, UI_PURPLE

// Criar cor RGB
int color = UI_COLOR(r, g, b);  // 0-255

// Cor com alpha
int color = UI_COLOR_RGBA(r, g, b, a);

// Extrair componentes
int r = UI_COLOR_R(color);
int g = UI_COLOR_G(color);
int b = UI_COLOR_B(color);
```

## 📐 Layout

```c
// VBox (vertical)
UIBox* vbox = ui_box_create(parent, true);  // vertical = true
ui_box_add_child(vbox, label1);
ui_box_add_child(vbox, label2);
ui_box_set_spacing(vbox, 10);

// HBox (horizontal)
UIBox* hbox = ui_box_create(parent, false);  // vertical = false
```

## 🖱️ Hit Testing

```c
// Verificar se mouse está sobre controle
if (ui_control_hit_test(&btn->base, mouse_x, mouse_y)) {
    // Mouse sobre o botão
}

// Atualizar estados
ctrl->hovered = ui_control_hit_test(ctrl, mouse_x, mouse_y);
```

## 🔄 Padrão de Uso

```c
// 1. Criar controles
UIButton* btn = ui_button_create(root, "Start");
ui_control_set_position(&btn->base, 100, 100);

// 2. Configurar callbacks
ui_button_set_on_click(btn, on_start_click);

// 3. No game loop - input
int mx, my;
SDL_GetMouseState(&mx, &my);
btn->base.hovered = ui_control_hit_test(&btn->base, mx, my);

if (mouse_clicked && btn->base.hovered) {
    btn->on_click(btn);
}

// 4. No game loop - render
render_button(btn, offset_x, offset_y);
```

## 📝 Demo

O demo `phase6_ui/main.c` demonstra:
- Painel com título
- Labels e Sliders
- Progress bar animada
- Checkboxes
- Botões com callbacks

### Controles
- Mover mouse sobre botões para ver hover
- Clicar botões para callbacks
- ESC para sair

## 🚀 Próximos Passos

1. **ScrollView**: Área com scroll
2. **ListView**: Lista de itens
3. **TreeView**: Árvore hierárquica
4. **ContextMenu**: Menu de contexto
5. **Tooltip**: Dicas ao passar mouse
6. **RichText**: Texto com formatação
