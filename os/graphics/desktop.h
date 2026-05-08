#pragma once
#include "raylib.h"
#include "terminalWindow.h"
#include "loggerWindow.h"
#include "schedulerWindow.h"
#include "../system/processManager.h"

class Desktop {
private:
    Rectangle terminalIcon;
    Rectangle loggerIcon;
    Rectangle schedulerIcon;
    vector<Window*> windows;

    Memory* memory;
    ProcessManager* processManager;
    int cpu;
    int storage;

public:
    Desktop(Memory* mem, ProcessManager* pm, int cpu, int storage);
    ~Desktop();

    void update();
    void draw();
};