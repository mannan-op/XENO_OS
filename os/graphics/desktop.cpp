#include "desktop.h"
#include <algorithm>


Desktop::Desktop(Memory* mem, ProcessManager* pm, int cpuCores, int storageGB) {
    terminalIcon = { 80, 120, 118, 118 };
    loggerIcon = { 80, 360, 118, 118 };
    schedulerIcon = { 250, 120, 118, 118 };

    memory = mem;
    processManager = pm;
    cpu = cpuCores;
    storage = storageGB;
}

Desktop::~Desktop() {
    for (auto* win : windows) {
        delete win;
    }
    windows.clear();
}

void Desktop::update() {
    Vector2 mouse = GetMousePosition();
    bool clickedWindow = false;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = (int)windows.size() - 1; i >= 0; --i) {
            if (!windows[i]->isActive()) {
                continue;
            }

            if (windows[i]->containsPoint(mouse)) {
                clickedWindow = true;

                Window* selected = windows[i];
                windows.erase(windows.begin() + i);
                windows.push_back(selected);

                for (auto* win : windows) {
                    win->setFocused(false);
                }
                selected->setFocused(true);
                break;
            }
        }

        if (!clickedWindow && CheckCollisionPointRec(mouse, terminalIcon)) {
            Process* terminalProcess = processManager->createProcess("Terminal", 128);
            process_id_t terminalPid = terminalProcess ? terminalProcess->pid : 0;
            for (auto* win : windows) {
                win->setFocused(false);
            }

            Window* newTerminal = new TerminalWindow(memory, cpu, storage, terminalPid);
            newTerminal->setFocused(true);
            windows.push_back(newTerminal);
        }

        if (!clickedWindow && CheckCollisionPointRec(mouse, loggerIcon)) {
            Process* loggerProcess = processManager->createProcess("Logger", 96);
            process_id_t loggerPid = loggerProcess ? loggerProcess->pid : 0;

            for (auto* win : windows) {
                win->setFocused(false);
            }

            Window* logger = new LoggerWindow(processManager, loggerPid);
            logger->setFocused(true);
            windows.push_back(logger);
        }

        if (!clickedWindow && CheckCollisionPointRec(mouse, schedulerIcon)) {
            Process* schedulerProcess = processManager->createProcess("Scheduler", 96);
            process_id_t schedulerPid = schedulerProcess ? schedulerProcess->pid : 0;

            for (auto* win : windows) {
                win->setFocused(false);
            }

            Window* scheduler = new SchedulerWindow(processManager, schedulerPid);
            scheduler->setFocused(true);
            windows.push_back(scheduler);
        }
    }

    // Update all windows
    for (auto* win : windows) {
        win->update();
    }

    windows.erase(std::remove_if(windows.begin(), windows.end(), [this](Window* win) {
        if (win->isActive()) {
            return false;
        }

        TerminalWindow* terminal = dynamic_cast<TerminalWindow*>(win);
        if (terminal != nullptr) {
            processManager->terminateProcess(terminal->getPid());
        }

        LoggerWindow* logger = dynamic_cast<LoggerWindow*>(win);
        if (logger != nullptr) {
            processManager->terminateProcess(logger->getPid());
        }

        SchedulerWindow* scheduler = dynamic_cast<SchedulerWindow*>(win);
        if (scheduler != nullptr) {
            processManager->terminateProcess(scheduler->getPid());
        }

        delete win;
        return true;
    }), windows.end());
}

void Desktop::draw() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangleGradientV(0, 0, screenW, screenH, Color{ 23, 43, 72, 255 }, Color{ 9, 19, 35, 255 });
    DrawCircle(screenW - 180, 120, 200, Fade(SKYBLUE, 0.10f));
    DrawCircle(120, screenH - 110, 230, Fade(BLUE, 0.12f));

    Rectangle statusCard = { 24, 18, 330, 86 };
    DrawRectangleRounded(statusCard, 0.14f, 16, Fade(BLACK, 0.34f));
    DrawRectangleLinesEx(statusCard, 1.4f, Fade(RAYWHITE, 0.20f));
    DrawText("Mini OS Desktop", 38, 34, 26, RAYWHITE);
    DrawText(TextFormat("CPU: %d cores | RAM: %dGB | Disk: %dGB", cpu, memory->getTotal(), storage), 38, 68, 17, Fade(RAYWHITE, 0.70f));

    Rectangle iconCard = { terminalIcon.x - 14, terminalIcon.y - 14, terminalIcon.width + 28, terminalIcon.height + 52 };
    DrawRectangleRounded(iconCard, 0.22f, 14, Fade(BLACK, 0.30f));
    DrawRectangleLinesEx(iconCard, 1.2f, Fade(RAYWHITE, 0.20f));

    DrawRectangleRounded(terminalIcon, 0.20f, 14, Color{ 31, 46, 80, 255 });
    DrawRectangle(terminalIcon.x + 16, terminalIcon.y + 18, terminalIcon.width - 32, terminalIcon.height - 38, Color{ 15, 18, 29, 255 });
    DrawRectangle(terminalIcon.x + 21, terminalIcon.y + 24, terminalIcon.width - 42, 6, Color{ 58, 88, 140, 255 });
    DrawText("_", terminalIcon.x + 30, terminalIcon.y + 42, 22, Color{ 113, 237, 182, 255 });
    DrawText("Terminal", terminalIcon.x + 16, terminalIcon.y + terminalIcon.height + 10, 18, RAYWHITE);

    Rectangle loggerCard = { loggerIcon.x - 14, loggerIcon.y - 14, loggerIcon.width + 28, loggerIcon.height + 52 };
    DrawRectangleRounded(loggerCard, 0.22f, 14, Fade(BLACK, 0.30f));
    DrawRectangleLinesEx(loggerCard, 1.2f, Fade(RAYWHITE, 0.20f));

    DrawRectangleRounded(loggerIcon, 0.20f, 14, Color{ 35, 59, 75, 255 });
    DrawRectangle(loggerIcon.x + 16, loggerIcon.y + 18, loggerIcon.width - 32, loggerIcon.height - 38, Color{ 16, 24, 33, 255 });
    for (int i = 0; i < 4; ++i) {
        DrawRectangle(loggerIcon.x + 24, loggerIcon.y + 28 + i * 14, loggerIcon.width - 48, 6, Color{ 88, 180, 220, 255 });
    }
    DrawText("Logger", loggerIcon.x + 26, loggerIcon.y + loggerIcon.height + 10, 18, RAYWHITE);

    Rectangle schedulerCard = { schedulerIcon.x - 14, schedulerIcon.y - 14, schedulerIcon.width + 28, schedulerIcon.height + 52 };
    DrawRectangleRounded(schedulerCard, 0.22f, 14, Fade(BLACK, 0.30f));
    DrawRectangleLinesEx(schedulerCard, 1.2f, Fade(RAYWHITE, 0.20f));

    DrawRectangleRounded(schedulerIcon, 0.20f, 14, Color{ 55, 44, 82, 255 });
    DrawRectangle(schedulerIcon.x + 16, schedulerIcon.y + 18, schedulerIcon.width - 32, schedulerIcon.height - 38, Color{ 26, 20, 40, 255 });
    DrawRectangle(schedulerIcon.x + 26, schedulerIcon.y + 30, 18, 36, Color{ 110, 169, 255, 255 });
    DrawRectangle(schedulerIcon.x + 48, schedulerIcon.y + 42, 18, 24, Color{ 143, 237, 182, 255 });
    DrawRectangle(schedulerIcon.x + 70, schedulerIcon.y + 24, 18, 42, Color{ 255, 218, 142, 255 });
    DrawText("Scheduler", schedulerIcon.x + 14, schedulerIcon.y + schedulerIcon.height + 10, 18, RAYWHITE);

    Rectangle dock = { (float)screenW / 2.0f - 230, (float)screenH - 72, 460, 48 };
    DrawRectangleRounded(dock, 0.5f, 20, Fade(BLACK, 0.33f));
    DrawRectangleLinesEx(dock, 1.0f, Fade(RAYWHITE, 0.18f));
    DrawText("Applications", (int)dock.x + 162, (int)dock.y + 14, 18, Fade(RAYWHITE, 0.78f));

    for (auto* win : windows) {
        if (win->isActive())
            win->draw();
    }
}