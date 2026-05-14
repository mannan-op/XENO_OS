#pragma once
#include "raylib.h"
#include "../system/process.h"
#include <string>

using namespace std;

class Window {
protected:
    Rectangle bounds;
    string title;

    bool dragging;
    Vector2 dragOffset;
    bool active;
    bool focused;

public:
    Window(float x, float y, float w, float h, string title);
    virtual void update();
    virtual void draw();

    virtual void drawContent() = 0;

    bool isActive();
    void setActive(bool val);
    bool isFocused() const;
    void setFocused(bool val);
    bool containsPoint(Vector2 point) const;
    virtual process_id_t getPid() const { return 0; }
};