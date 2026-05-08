#include "windows.h"

static float Clampf(float value, float minVal, float maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

Window::Window(float x, float y, float w, float h, string title) {
    bounds = { x, y, w, h };
    this->title = title;
    dragging = false;
    active = true;
    focused = false;
}

void Window::update() {
    if (!active) return;

    Vector2 mouse = GetMousePosition();

    Rectangle titleBar = { bounds.x, bounds.y, bounds.width, 30 };
    Rectangle closeBtn = { bounds.x + bounds.width - 30, bounds.y, 30, 30 };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, closeBtn)) {
        active = false;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, titleBar)) {
        dragging = true;
        dragOffset = { mouse.x - bounds.x, mouse.y - bounds.y };
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        dragging = false;
    }

    if (dragging) {
        bounds.x = Clampf(mouse.x - dragOffset.x, 8.0f, (float)GetScreenWidth() - bounds.width - 8.0f);
        bounds.y = Clampf(mouse.y - dragOffset.y, 8.0f, (float)GetScreenHeight() - bounds.height - 8.0f);
    }
}

void Window::draw() {
    if (!active) return;

    DrawRectangleRounded(bounds, 0.04f, 10, Color{ 16, 20, 28, 244 });
    DrawRectangleLinesEx(bounds, focused ? 2.0f : 1.2f, focused ? Fade(SKYBLUE, 0.8f) : Fade(RAYWHITE, 0.20f));

    DrawRectangleRounded({ bounds.x, bounds.y, bounds.width, 34 }, 0.25f, 10, Color{ 34, 45, 62, 255 });
    DrawRectangle(bounds.x, bounds.y + 20, bounds.width, 14, Color{ 34, 45, 62, 255 });

    DrawText(title.c_str(), bounds.x + 12, bounds.y + 8, 15, RAYWHITE);

    Rectangle closeButton = { bounds.x + bounds.width - 26, bounds.y + 8, 14, 14 };
    DrawRectangleRounded(closeButton, 0.4f, 8, Color{ 244, 100, 100, 255 });
    DrawText("x", bounds.x + bounds.width - 23, bounds.y + 7, 12, WHITE);

    drawContent();
}

bool Window::isActive() { return active; }
void Window::setActive(bool val) { active = val; }
bool Window::isFocused() const { return focused; }
void Window::setFocused(bool val) { focused = val; }
bool Window::containsPoint(Vector2 point) const {
    return CheckCollisionPointRec(point, bounds);
}