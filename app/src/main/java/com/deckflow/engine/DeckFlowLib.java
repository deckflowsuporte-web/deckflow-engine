package com.deckflow.engine;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class DeckFlowLib extends Activity {

    // Game State
    private GameView gameView;
    private boolean gameRunning = false;
    private int score = 0;
    private int coins = 0;
    private int lives = 3;

    // Player
    private float playerX, playerY;
    private float playerVelX = 0, playerVelY = 0;
    private float playerSize = 50;
    private boolean onGround = false;
    private boolean facingRight = true;

    // World
    private List<Platform> platforms = new ArrayList<>();
    private List<Coin> coinList = new ArrayList<>();
    private float cameraY = 0;

    // Touch
    private boolean touchLeft = false;
    private boolean touchRight = false;
    private boolean touchJump = false;

    // Physics
    private static final float GRAVITY = 1500;
    private static final float JUMP_FORCE = -600;
    private static final float MOVE_SPEED = 300;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Main layout
        LinearLayout mainLayout = new LinearLayout(this);
        mainLayout.setOrientation(LinearLayout.VERTICAL);
        mainLayout.setBackgroundColor(Color.rgb(30, 30, 60));

        // Header
        LinearLayout header = new LinearLayout(this);
        header.setBackgroundColor(Color.rgb(20, 20, 40));
        header.setPadding(20, 15, 20, 15);
        header.setOrientation(LinearLayout.HORIZONTAL);

        TextView title = new TextView(this);
        title.setText("🎮 DECKFLOW ENGINE");
        title.setTextSize(20);
        title.setTextColor(Color.WHITE);

        TextView scoreText = new TextView(this);
        scoreText.setText("SCORE: 0");
        scoreText.setTextSize(16);
        scoreText.setTextColor(Color.YELLOW);
        scoreText.setPadding(50, 0, 0, 0);

        TextView livesText = new TextView(this);
        livesText.setText("❤️ 3");
        livesText.setTextSize(16);
        livesText.setTextColor(Color.RED);
        livesText.setPadding(30, 0, 0, 0);

        header.addView(title);
        header.addView(scoreText);
        header.addView(livesText);

        // Game Canvas
        gameView = new GameView(this);

        // Controls layout
        LinearLayout controls = new LinearLayout(this);
        controls.setBackgroundColor(Color.rgb(20, 20, 40));
        controls.setPadding(20, 20, 20, 40);

        // Left buttons
        Button btnLeft = createButton("◀");
        btnLeft.setOnTouchListener((v, event) -> {
            touchLeft = event.getAction() == MotionEvent.ACTION_DOWN;
            return true;
        });

        Button btnRight = createButton("▶");
        btnRight.setOnTouchListener((v, event) -> {
            touchRight = event.getAction() == MotionEvent.ACTION_DOWN;
            return true;
        });

        // Jump button
        Button btnJump = createButton("⬆ JUMP");
        btnJump.setTextSize(18);
        btnJump.setOnTouchListener((v, event) -> {
            touchJump = event.getAction() == MotionEvent.ACTION_DOWN;
            return true;
        });

        // Restart button
        Button btnRestart = createButton("🔄");
        btnRestart.setOnClickListener(v -> restartGame());

        controls.addView(btnLeft);
        controls.addView(btnRight);
        controls.addView(btnJump);
        controls.addView(btnRestart);

        // Info text
        TextView info = new TextView(this);
        info.setText("Colete todas as moedas! 💰");
        info.setTextSize(14);
        info.setTextColor(Color.GRAY);
        info.setPadding(20, 10, 20, 10);

        mainLayout.addView(header);
        mainLayout.addView(gameView, new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT, 0, 1));
        mainLayout.addView(controls);
        mainLayout.addView(info);

        setContentView(mainLayout);

        // Initialize game
        initGame();
        startGame();
    }

    private Button createButton(String text) {
        Button btn = new Button(this);
        btn.setText(text);
        btn.setTextSize(24);
        btn.setBackgroundColor(Color.rgb(60, 60, 90));
        btn.setTextColor(Color.WHITE);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(0, 120, 1);
        params.setMargins(10, 0, 10, 0);
        btn.setLayoutParams(params);
        return btn;
    }

    private void initGame() {
        // Player start position
        playerX = 100;
        playerY = 300;
        playerVelX = 0;
        playerVelY = 0;
        score = 0;
        coins = 0;
        lives = 3;
        cameraY = 0;

        // Create platforms
        platforms.clear();
        platforms.add(new Platform(0, 550, 800, 50)); // Ground
        platforms.add(new Platform(100, 450, 150, 20));
        platforms.add(new Platform(300, 350, 150, 20));
        platforms.add(new Platform(500, 250, 150, 20));
        platforms.add(new Platform(200, 150, 150, 20));
        platforms.add(new Platform(450, 100, 150, 20));

        // Create coins
        coinList.clear();
        coinList.add(new Coin(175, 400));
        coinList.add(new Coin(375, 300));
        coinList.add(new Coin(575, 200));
        coinList.add(new Coin(275, 100));
        coinList.add(new Coin(525, 50));
    }

    private void restartGame() {
        initGame();
        gameRunning = true;
    }

    private void startGame() {
        gameRunning = true;
        new Thread(() -> {
            while (gameRunning) {
                update();
                gameView.postInvalidate();
                try {
                    Thread.sleep(16); // 60 FPS
                } catch (InterruptedException e) {}
            }
        }).start();
    }

    private void update() {
        float dt = 0.016f;

        // Horizontal movement
        if (touchLeft) {
            playerVelX = -MOVE_SPEED;
            facingRight = false;
        } else if (touchRight) {
            playerVelX = MOVE_SPEED;
            facingRight = true;
        } else {
            playerVelX *= 0.8f;
        }

        // Jump
        if (touchJump && onGround) {
            playerVelY = JUMP_FORCE;
            onGround = false;
        }

        // Gravity
        playerVelY += GRAVITY * dt;

        // Apply velocity
        playerX += playerVelX * dt;
        playerY += playerVelY * dt;

        // World bounds
        if (playerX < 0) playerX = 0;
        if (playerX > 750) playerX = 750;

        // Fall death
        if (playerY > 700) {
            lives--;
            playerY = 100;
            playerVelY = 0;
            if (lives <= 0) {
                lives = 3;
                coins = 0;
                for (Coin c : coinList) c.collected = false;
            }
        }

        // Platform collision
        onGround = false;
        for (Platform p : platforms) {
            if (playerVelY > 0 &&
                playerX + playerSize/2 > p.x &&
                playerX - playerSize/2 < p.x + p.width &&
                playerY + playerSize/2 > p.y &&
                playerY + playerSize/2 < p.y + p.height + playerVelY * dt) {
                playerY = p.y - playerSize/2;
                playerVelY = 0;
                onGround = true;
            }
        }

        // Coin collection
        for (Coin c : coinList) {
            if (!c.collected) {
                float dx = playerX - c.x;
                float dy = playerY - c.y;
                if (Math.sqrt(dx*dx + dy*dy) < 40) {
                    c.collected = true;
                    coins++;
                    score += 100;
                }
            }
        }

        // Camera follow
        float targetCameraY = playerY - 250;
        cameraY += (targetCameraY - cameraY) * 0.1f;
        if (cameraY > 0) cameraY = 0;
    }

    // Classes
    class Platform {
        float x, y, width, height;
        Platform(float x, float y, float w, float h) {
            this.x = x; this.y = y; this.width = w; this.height = h;
        }
    }

    class Coin {
        float x, y;
        boolean collected = false;
        Coin(float x, float y) { this.x = x; this.y = y; }
    }

    // Game View
    class GameView extends View {
        Paint paint = new Paint();

        public GameView(android.content.Context context) {
            super(context);
            setBackgroundColor(Color.rgb(60, 100, 180)); // Sky blue
        }

        @Override
        protected void onDraw(Canvas canvas) {
            super.onDraw(canvas);

            // Draw platforms
            paint.setColor(Color.rgb(120, 80, 50)); // Brown
            for (Platform p : platforms) {
                canvas.drawRect(p.x, p.y - cameraY, p.x + p.width, p.y + p.height - cameraY, paint);
            }

            // Draw grass on platforms
            paint.setColor(Color.rgb(80, 160, 80)); // Green
            for (Platform p : platforms) {
                canvas.drawRect(p.x, p.y - cameraY, p.x + p.width, p.y + 5 - cameraY, paint);
            }

            // Draw coins
            for (Coin c : coinList) {
                if (!c.collected) {
                    paint.setColor(Color.rgb(255, 215, 0)); // Gold
                    canvas.drawCircle(c.x, c.y - cameraY, 15, paint);
                    paint.setColor(Color.WHITE);
                    canvas.drawCircle(c.x - 4, c.y - 4 - cameraY, 4, paint);
                }
            }

            // Draw player
            int playerColor = Color.rgb(80, 200, 120); // Green
            paint.setColor(playerColor);
            canvas.drawRoundRect(
                playerX - playerSize/2,
                playerY - playerSize/2 - cameraY,
                playerX + playerSize/2,
                playerY + playerSize/2 - cameraY,
                10, 10, paint
            );

            // Player face
            paint.setColor(Color.WHITE);
            float eyeX = facingRight ? playerX + 5 : playerX - 15;
            canvas.drawCircle(eyeX, playerY - 10 - cameraY, 6, paint);
            canvas.drawCircle(eyeX + 15, playerY - 10 - cameraY, 6, paint);

            paint.setColor(Color.BLACK);
            canvas.drawCircle(eyeX + 2, playerY - 10 - cameraY, 3, paint);
            canvas.drawCircle(eyeX + 17, playerY - 10 - cameraY, 3, paint);

            // Draw score
            paint.setColor(Color.WHITE);
            paint.setTextSize(20);
            canvas.drawText("💰 " + coins + "/5  ⭐ " + score, 20, 35, paint);

            // Lives
            paint.setColor(Color.RED);
            canvas.drawText("❤️ " + lives, 650, 35, paint);

            // Instructions
            if (playerY < 50) {
                paint.setColor(Color.WHITE);
                paint.setTextSize(24);
                canvas.drawText("🎮 Use os botões para jogar!", 200, 300, paint);
            }
        }
    }
}
