package com.deckflow.engine;

import android.app.Activity;
import android.os.Bundle;
import android.view.WindowManager;
import android.widget.TextView;
import android.graphics.Color;

public class DeckFlowLib extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        TextView view = new TextView(this);
        view.setTextSize(16);
        view.setTextColor(Color.WHITE);
        view.setBackgroundColor(Color.rgb(26, 26, 46));
        view.setPadding(40, 40, 40, 40);
        view.setText(
            "═══════════════════════════════════════════════════\n" +
            "        DECKFLOW ENGINE v1.0.0\n" +
            "═══════════════════════════════════════════════════\n\n" +
            "Phase 1: COMPLETE ✓\n" +
            "  ✓ Scene, Node, Hierarchy (60 tests)\n\n" +
            "Phase 2: COMPLETE ✓\n" +
            "  ✓ SDL2 + Sprite Rendering\n" +
            "  • examples/phase2_sprite/main.c\n\n" +
            "Phase 3: COMPLETE ✓\n" +
            "  ✓ Box2D Physics Integration\n" +
            "  • examples/phase3_physics/main.c\n" +
            "  • Gravity, collisions, rigid bodies\n\n" +
            "github.com/deckflowsuporte-web/deckflow-engine\n"
        );
        setContentView(view);
    }
}
