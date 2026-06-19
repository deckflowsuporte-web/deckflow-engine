# 🎮 DeckFlow Engine

**Motor de jogos 2D mobile-first para Android, inspirado no design da Godot Engine.**

[![Build Android APK](https://github.com/deckflowsuporte-web/deckflow-engine/actions/workflows/android.yml/badge.svg)](https://github.com/deckflowsuporte-web/deckflow-engine/actions/workflows/android.yml)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

---

## 📱 Downloads

| Tipo | Status |
|------|--------|
| **Debug APK** | ✅ Build automático via GitHub Actions |
| **Release APK** | ✅ Build automático via GitHub Actions |

### 📥 Download Rápido

1. Vá em **[Actions](https://github.com/deckflowsuporte-web/deckflow-engine/actions)**
2. Clique no workflow mais recente
3. Baixe o APK em **Artifacts**

---

## 🚀 Funcionalidades

### ✅ Implementado

| Sistema | Descrição |
|---------|-----------|
| **Gameplay** | Platformer funcional com física |
| **Player** | Personagem com controles touch |
| **Moedas** | Sistema de coleta com pontuação |
| **Vidas** | 3 vidas com respawn |
| **Câmera** | Follow suave do player |
| **Touch Controls** | ◀ ▶ ⬆ botões na tela |

### 🎯 Roadmap

| Fase | Status | Descrição |
|------|--------|-----------|
| Phase 1 | ✅ | Scene/Node System |
| Phase 2 | ✅ | SDL2 Rendering |
| Phase 3 | ✅ | Box2D Physics |
| Phase 4 | ✅ | Input, Camera, Audio, Particles |
| Phase 5 | ✅ | Physics + Scene Integration |
| Phase 6 | ✅ | UI System |
| Phase 7 | ✅ | Animation System |
| Phase 8 | ✅ | **Game Demo - Platformer** |
| Phase 9 | ✅ | **Android NDK Integration** |

---

## 🕹️ Como Jogar

1. **Instale o APK** no Android
2. **Abra o app** - O jogo inicia automaticamente
3. **Controles:**
   - ◀ = Mover esquerda
   - ▶ = Mover direita  
   - ⬆ = Pular
   - 🔄 = Reiniciar

**Objetivo:** Coletar todas as 5 moedas douradas! 💰

---

## 💻 Desenvolvimento

### Build Local

```bash
# Clone o repo
git clone https://github.com/deckflowsuporte-web/deckflow-engine.git
cd deckflow-engine

# Build APK
cd app
./gradlew assembleDebug

# APK em:
# app/build/outputs/apk/debug/app-debug.apk
```

### Requisitos

- Java 17+
- Android SDK 34
- Gradle 8.4+

---

## 📂 Estrutura do Projeto

```
deckflow-engine/
├── src/                    # Engine C (multi-plataforma)
│   ├── math/              # Vector2, Transform2D
│   ├── nodes/             # Sistema de entidades
│   ├── scenes/            # Gerenciamento de cenas
│   ├── physics/           # Box2D integration
│   ├── input/             # Entrada (touch/teclado)
│   ├── camera/            # Camera2D
│   ├── audio/             # Áudio
│   ├── particle/          # Partículas
│   ├── tilemap/           # Tile maps
│   ├── ui/                # UI components
│   └── animation/         # Animações
│
├── app/                   # Wrapper Android
│   ├── src/main/
│   │   ├── java/          # Interface Java
│   │   ├── jni/           # Código nativo Android
│   │   └── cpp/           # CMake build
│   └── build.gradle
│
├── examples/              # Demos de cada fase
│   ├── phase1_test/
│   ├── phase2_sprite/
│   ├── phase3_physics/
│   ├── phase4_demo/
│   ├── phase5_physics/
│   ├── phase6_ui/
│   ├── phase7_animation/
│   └── game_demo/
│
├── .github/
│   └── workflows/
│       └── android.yml    # CI/CD - Build APK automático
│
├── CMakeLists.txt
└── README.md
```

---

## 🔧 CI/CD - GitHub Actions

O projeto usa **GitHub Actions** para build automático do APK a cada push.

```yaml
# .github/workflows/android.yml
on:
  push:
    branches: [ master, main ]
  workflow_dispatch:

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-java@v4
        with: { java-version: '17' }
      - uses: android-actions/setup-android@v3
      - run: ./gradlew assembleDebug
      - uses: actions/upload-artifact@v4
        with: { path: app/build/outputs/apk/debug/*.apk }
```

---

## 🎨 Screenshots

```
┌────────────────────────────────────┐
│  🎮 DECKFLOW ENGINE      SCORE: 0 │
├────────────────────────────────────┤
│                                    │
│     🌟                              │
│   ▓▓▓▓▓         🌟                │
│          ▓▓▓▓▓                    │
│   🌟          ▓▓▓▓▓    🌟          │
│        ▓▓▓▓▓         ▓▓▓▓▓       │
│                      🌟            │
│  🟢                      ▓▓▓▓▓     │
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │
├────────────────────────────────────┤
│   [◀]   [▶]   [⬆ JUMP]   [🔄]    │
└────────────────────────────────────┘
```

---

## 📊 Estatísticas

| Métrica | Valor |
|---------|-------|
| **Linhas de código** | ~6000+ |
| **Fases implementadas** | 9 |
| **Exemplos** | 8 |
| **Plataformas** | Android, Desktop |
| **Linguagem** | C11 + Java |

---

## 🤝 Contribuir

1. Fork o repo
2. Crie uma branch (`git checkout -b feature/nova-feature`)
3. Commit (`git commit -m 'Add nova feature'`)
4. Push (`git push origin feature/nova-feature`)
5. Abra um Pull Request

---

## 📄 Licença

MIT License - Veja [LICENSE](LICENSE)

---

## 👨‍💻 Autor

**DeckFlow Suporte Web**

- 📧 Email: deckflow.suporte@gmail.com
- 🌐 GitHub: [deckflowsuporte-web](https://github.com/deckflowsuporte-web)

---

<p align="center">
  Feito com ❤️ para a comunidade de jogos indie
</p>
