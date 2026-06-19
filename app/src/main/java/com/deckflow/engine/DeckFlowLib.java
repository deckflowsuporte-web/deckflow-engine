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
        view.setTextSize(14);
        view.setTextColor(Color.WHITE);
        view.setBackgroundColor(Color.rgb(26, 26, 46));
        view.setPadding(30, 30, 30, 30);
        view.setText(
            "═══════════════════════════════════════════════════\n" +
            "       DECKFLOW ENGINE v1.0.0\n" +
            "═══════════════════════════════════════════════════\n\n" +
            "Phase 1: COMPLETE ✓  Scene, Node, Hierarchy\n" +
            "Phase 2: COMPLETE ✓  SDL2 + Sprite\n" +
            "Phase 3: COMPLETE ✓  Box2D Physics\n" +
            "Phase 4: COMPLETE ✓  Input, Camera, Audio\n" +
            "         COMPLETE ✓  Tilemap, Particles\n" +
            "Phase 5: COMPLETE ✓  Physics + Scene\n" +
            "Phase 6: COMPLETE ✓  UI System\n" +
            "Phase 7: COMPLETE ✓  Animation System\n" +
            "Phase 8: COMPLETE ✓  GAME DEMO!\n\n" +
            "github.com/deckflowsuporte-web/deckflow-engine\n"
        );
        setContentView(view);
    }
}
