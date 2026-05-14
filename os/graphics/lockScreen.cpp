#include "lockScreen.h"

using namespace std;

void LockScreen::ensureBackground() {
    if (backgroundReady) {
        return;
    }

    const char* candidates[] = {
        "graphics/assets/background.jpg",
        "assets/lock-background.jpg",
        "assets/background.jpg",
        "background.jpg",
        "graphics/assets/background.png",
        "assets/lock-background.png",
        "assets/background.png",
        "background.png"
    };

    for (const char* path : candidates) {
        if (FileExists(path)) {
            backgroundTexture = LoadTexture(path);
            if (backgroundTexture.width > 0 && backgroundTexture.height > 0) {
                backgroundReady = true;
                return;
            }
            // Failed to load valid image data, unload any partial data and try next candidate
            UnloadTexture(backgroundTexture);
        }
    }

    const int w = 1280;
    const int h = 720;
    Image img = GenImageGradientLinear(w, h, 90, Color{ 35, 67, 116, 255 }, Color{ 19, 29, 46, 255 });
    ImageDrawCircle(&img, 960, 140, 210, Fade(Color{ 130, 210, 255, 255 }, 0.45f));
    ImageDrawCircle(&img, 220, 520, 260, Fade(Color{ 84, 160, 235, 255 }, 0.38f));
    ImageDrawRectangle(&img, 0, h - 180, w, 180, Fade(Color{ 3, 10, 26, 255 }, 0.55f));

    backgroundTexture = LoadTextureFromImage(img);
    UnloadImage(img);
    backgroundReady = true;
}

LockScreen::LockScreen() {
    correctPassword = "1234";
    unlocked = false;
    backgroundReady = false;
    errorTimer = 0.0f;
}

LockScreen::~LockScreen() {
    if (backgroundReady) {
        UnloadTexture(backgroundTexture);
    }
}

void LockScreen::update(float dt) {
    errorTimer -= dt;
    if (errorTimer <= 0.0f) {
        errorMessage.clear();
    }

    int key = GetCharPressed();

    while (key > 0) {
        if (key >= 32 && key <= 125 && inputPassword.size() < 24) {
            inputPassword += (char)key;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !inputPassword.empty()) {
        inputPassword.pop_back();
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (inputPassword == correctPassword) {
            unlocked = true;
        }
        else {
            inputPassword.clear();
            errorMessage = "Incorrect password";
            errorTimer = 2.0f;
        }
    }
}

void LockScreen::draw() {
    ensureBackground();

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    if (backgroundReady) {
        Rectangle src = { 0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height };
        Rectangle dst = { 0, 0, (float)screenW, (float)screenH };
        DrawTexturePro(backgroundTexture, src, dst, { 0, 0 }, 0.0f, WHITE);
    }
    else {
        ClearBackground(Color{ 16, 28, 46, 255 });
    }

    // Soft blur-like overlay to de-emphasize the background image.
    for (int i = 0; i < 7; i++) {
        DrawRectangle(-20 + i * 8, -20 + i * 8, screenW + 40 - i * 16, screenH + 40 - i * 16, Fade(BLACK, 0.06f));
    }

    Rectangle card = { (float)screenW / 2.0f - 250, (float)screenH / 2.0f - 220, 500, 430 };
    DrawRectangleRounded(card, 0.08f, 20, Fade(BLACK, 0.46f));
    DrawRectangleLinesEx(card, 1.8f, Fade(RAYWHITE, 0.26f));

    Vector2 avatarCenter = { card.x + card.width / 2.0f, card.y + 96 };
    DrawCircleV(avatarCenter, 44, Color{ 74, 176, 255, 255 });
    DrawCircleLinesV(avatarCenter, 44, Fade(RAYWHITE, 0.75f));
    DrawText("M", (int)avatarCenter.x - 14, (int)avatarCenter.y - 18, 38, RAYWHITE);

    DrawText("Mannan", (int)card.x + 182, (int)card.y + 158, 32, RAYWHITE);
    DrawText("Enter password to unlock", (int)card.x + 145, (int)card.y + 194, 18, Fade(RAYWHITE, 0.72f));

    string hidden(inputPassword.length(), '*');
    Rectangle inputBar = { card.x + 50, card.y + 242, 400, 52 };
    DrawRectangleRounded(inputBar, 0.28f, 14, Fade(Color{ 22, 43, 71, 255 }, 0.82f));
    DrawRectangleLinesEx(inputBar, 1.6f, Fade(Color{ 122, 198, 255, 255 }, 0.85f));

    string shown = hidden.empty() ? "Password" : hidden;
    Color shownColor = hidden.empty() ? Fade(RAYWHITE, 0.50f) : RAYWHITE;
    DrawText(shown.c_str(), (int)inputBar.x + 16, (int)inputBar.y + 15, 22, shownColor);

    DrawText("Press ENTER to unlock", (int)card.x + 160, (int)card.y + 318, 18, Fade(RAYWHITE, 0.66f));

    if (!errorMessage.empty()) {
        DrawText(errorMessage.c_str(), (int)card.x + 160, (int)card.y + 352, 18, Color{ 255, 132, 132, 255 });
    }
}

bool LockScreen::isUnlocked() {
    return unlocked;
}