#include "boot.h"
#include <algorithm>

BootScreen::BootScreen() {
    messages = {
        "Starting Mini Operating System...",
        "Loading system resources...",
        "Initializing kernel modules...",
        "Starting UI system..."
    };

    timer = 0.0f;
    visibleMessages = 0;
}

void BootScreen::update(float deltaTime) {
    timer += deltaTime;

    // Reveal a new message every 1 second
    if (visibleMessages < messages.size()) {
        if (timer > visibleMessages * 1.0f) {
            visibleMessages++;
        }
    }
}

void BootScreen::draw() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangleGradientV(0, 0, screenW, screenH, Color{ 12, 20, 36, 255 }, Color{ 18, 44, 70, 255 });
    DrawCircle(screenW - 120, 90, 180, Fade(SKYBLUE, 0.10f));
    DrawCircle(140, screenH - 80, 220, Fade(BLUE, 0.15f));

    DrawText("MINI OS", 56, 52, 44, RAYWHITE);
    DrawText("Booting secure shell", 58, 102, 20, Fade(RAYWHITE, 0.70f));

    const int cardX = screenW / 2 - 330;
    const int cardY = screenH / 2 - 120;
    DrawRectangleRounded({ (float)cardX, (float)cardY, 660, 250 }, 0.08f, 16, Fade(BLACK, 0.35f));
    DrawRectangleLinesEx({ (float)cardX, (float)cardY, 660, 250 }, 1.5f, Fade(RAYWHITE, 0.20f));

    int y = cardY + 36;

    for (int i = 0; i < visibleMessages; i++) {
        DrawText(messages[i].c_str(), cardX + 30, y, 22, Color{ 202, 239, 255, 255 });
        y += 30;
    }

    const float progress = std::min(timer / (messages.size() * 1.2f), 1.0f);
    Rectangle progressBack = { (float)cardX + 30, (float)cardY + 192, 600, 16 };
    Rectangle progressFill = { progressBack.x, progressBack.y, progressBack.width * progress, progressBack.height };
    DrawRectangleRounded(progressBack, 0.4f, 16, Fade(RAYWHITE, 0.15f));
    DrawRectangleRounded(progressFill, 0.4f, 16, Color{ 44, 186, 255, 255 });
}


bool BootScreen::isFinished() const {
    return timer > messages.size() * 1.2f;
}