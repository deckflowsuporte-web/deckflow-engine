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
        view.setTextSize(20);
        view.setTextColor(Color.WHITE);
        view.setBackgroundColor(Color.BLACK);
        view.setPadding(50, 50, 50, 50);
        view.setText(
            "═══════════════════════════════════════\n" +
            "      DECKFLOW ENGINE v1.0.0\n" +
            "═══════════════════════════════════════\n\n" +
            "Phase 1: COMPLETE\n" +
            "  Vector2, Transform2D, Node, Scene\n\n" +
            "Phase 2: SDL2 (PENDING)\n" +
            "Phase 3: Box2D (PENDING)\n\n" +
            "github.com/deckflowsuporte-web/deckflow-engine\n"
        );
        setContentView(view);
    }
}
