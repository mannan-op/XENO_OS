#include "raylib.h"
#include "graphics/screenManager.h"

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "Xeno OS");

    ScreenManager manager;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        manager.update(dt);

        BeginDrawing();
        manager.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}