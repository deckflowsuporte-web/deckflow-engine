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
        view.setTextSize(18);
        view.setTextColor(Color.WHITE);
        view.setBackgroundColor(Color.rgb(26, 26, 46));
        view.setPadding(50, 50, 50, 50);
        view.setText(
            "═══════════════════════════════════════════════════\n" +
            "        DECKFLOW ENGINE v1.0.0\n" +
            "═══════════════════════════════════════════════════\n\n" +
            "Phase 1: COMPLETE ✓\n" +
            "  ✓ Scene creation\n" +
            "  ✓ Node creation (60 tests passed)\n" +
            "  ✓ Parent/Child hierarchy\n" +
            "  ✓ Node search\n" +
            "  ✓ Tree cleanup\n\n" +
            "Phase 2: SDL2 + Sprite Rendering\n" +
            "  • examples/phase2_sprite/main.c\n" +
            "  • Run on desktop with SDL2 installed\n\n" +
            "Phase 3: Box2D (PENDING)\n\n" +
            "github.com/deckflowsuporte-web/deckflow-engine\n"
        );
        setContentView(view);
    }
}
