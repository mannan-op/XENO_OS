#pragma once

#include "raylib.h"
#include <string>
#include <vector>
using namespace std;

class SystemModeScreen {
private:
    vector<string> messages;
    float timer;
    int visibleMessages;

public:
    SystemModeScreen();

    void update(float dt);
    void draw();
    bool isFinished() const;
};
