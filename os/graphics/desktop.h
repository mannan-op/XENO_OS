#pragma once
#include "raylib.h"
#include "terminalWindow.h"
#include "loggerWindow.h"
#include "schedulerWindow.h"
#include "fileViewerWindow.h"
#include "railwayReservationWindow.h"
#include "../system/processManager.h"
#include <vector>
using namespace std;

class Desktop {
private:
    Rectangle terminalIcon;
    Rectangle loggerIcon;
    Rectangle schedulerIcon;
    Rectangle bookingIcon;
    Rectangle systemLogIcon;
    vector<Window*> windows;

    Memory* memory;
    ProcessManager* processManager;
    int cpu;
    int storage;
    Texture2D backgroundTexture;
    bool backgroundReady;

    void ensureBackground();

public:
    Desktop(Memory* mem, ProcessManager* pm, int cpu, int storage);
    ~Desktop();

    void update();
    void draw();
};
