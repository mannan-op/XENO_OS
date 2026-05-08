#include "systemConfig.h"
#include <algorithm>

namespace {
    void DrawField(const char* label, const std::string& value, int x, int y, bool active) {
        Rectangle field = { (float)x, (float)y, 420, 48 };
        Color frame = active ? Color{ 74, 177, 255, 255 } : Fade(RAYWHITE, 0.28f);
        Color fill = active ? Fade(Color{ 42, 83, 130, 255 }, 0.55f) : Fade(BLACK, 0.25f);

        DrawText(label, x, y - 24, 18, Fade(RAYWHITE, 0.8f));
        DrawRectangleRounded(field, 0.22f, 10, fill);
        DrawRectangleLinesEx(field, 1.8f, frame);

        std::string shown = value.empty() ? "Type value" : value;
        Color textColor = value.empty() ? Fade(RAYWHITE, 0.45f) : RAYWHITE;
        DrawText(shown.c_str(), x + 14, y + 14, 20, textColor);
    }

    void DrawChoiceButton(const char* label, Rectangle rect, bool selected) {
        DrawRectangleRounded(rect, 0.22f, 8, selected ? Color{ 70, 147, 214, 255 } : Color{ 35, 47, 63, 255 });
        DrawRectangleLinesEx(rect, 1.2f, selected ? Fade(RAYWHITE, 0.34f) : Fade(RAYWHITE, 0.16f));
        DrawText(label, (int)rect.x + 18, (int)rect.y + 12, 18, RAYWHITE);
    }
}


SystemConfig::SystemConfig() {
    step = 0;
    algorithm = SchedulingAlgorithm::FCFS;
}

void SystemConfig::update(float deltaTime) {
    if (step <= 2) {
        int key = GetCharPressed();

        // Accept number input only
        if (key >= '0' && key <= '9') {
            if (step == 0) ramGB += (char)key;
            else if (step == 1) cpuCores += (char)key;
            else if (step == 2) storageGB += (char)key;
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (step == 0 && !ramGB.empty()) ramGB.pop_back();
            else if (step == 1 && !cpuCores.empty()) cpuCores.pop_back();
            else if (step == 2 && !storageGB.empty()) storageGB.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER)) {
            step++;
        }
    }
    else if (step == 3) {
        Vector2 mouse = GetMousePosition();

        Rectangle fcfsBtn = { 234, 250, 150, 52 };
        Rectangle rrBtn = { 414, 250, 150, 52 };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, fcfsBtn)) {
                algorithm = SchedulingAlgorithm::FCFS;
                step = 5;
            }
            else if (CheckCollisionPointRec(mouse, rrBtn)) {
                algorithm = SchedulingAlgorithm::RoundRobin;
                if (timeQuantum.empty()) {
                    timeQuantum = "1";
                }
                step = 4;
            }
        }
    }
    else if (step == 4) {
        int key = GetCharPressed();
        if (key >= '0' && key <= '9' && timeQuantum.size() < 4) {
            timeQuantum += (char)key;
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !timeQuantum.empty()) {
            timeQuantum.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER)) {
            step++;
        }
    }
}

void SystemConfig::draw() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangleGradientV(0, 0, screenW, screenH, Color{ 15, 26, 44, 255 }, Color{ 6, 15, 28, 255 });
    DrawCircle(100, 100, 160, Fade(SKYBLUE, 0.10f));
    DrawCircle(screenW - 120, screenH - 80, 240, Fade(BLUE, 0.12f));

    Rectangle card = { (float)screenW / 2.0f - 280, (float)screenH / 2.0f - 220, 560, 430 };
    DrawRectangleRounded(card, 0.06f, 18, Fade(BLACK, 0.45f));
    DrawRectangleLinesEx(card, 1.5f, Fade(RAYWHITE, 0.20f));

    DrawText("Hardware Setup", (int)card.x + 34, (int)card.y + 28, 34, RAYWHITE);
    DrawText("Configure your virtual machine resources", (int)card.x + 34, (int)card.y + 68, 18, Fade(RAYWHITE, 0.70f));

    Rectangle progress = { card.x + 34, card.y + 102, card.width - 68, 8 };
    DrawRectangleRounded(progress, 0.5f, 8, Fade(RAYWHITE, 0.10f));
    DrawRectangleRounded({ progress.x, progress.y, progress.width * std::min(step / 5.0f, 1.0f), progress.height }, 0.5f, 8, Color{ 74, 177, 255, 255 });

    DrawField("RAM (GB)", ramGB, (int)card.x + 34, (int)card.y + 126, step == 0);
    DrawField("CPU Cores", cpuCores, (int)card.x + 34, (int)card.y + 202, step == 1);
    DrawField("Storage (GB)", storageGB, (int)card.x + 34, (int)card.y + 278, step == 2);

    if (step < 3) {
        DrawText("Press ENTER to move to the next field", (int)card.x + 34, (int)card.y + 352, 18, Fade(RAYWHITE, 0.68f));
    }

    if (step == 3) {
        DrawText("5. Scheduler Initialization", (int)card.x + 34, (int)card.y + 350, 18, Fade(RAYWHITE, 0.80f));
        DrawText("Select the scheduling algorithm for process execution", (int)card.x + 34, (int)card.y + 374, 16, Fade(RAYWHITE, 0.65f));

        DrawChoiceButton("FCFS", { (float)card.x + 118, (float)card.y + 244, 138, 52 }, algorithm == SchedulingAlgorithm::FCFS);
        DrawChoiceButton("Round Robin", { (float)card.x + 264, (float)card.y + 244, 174, 52 }, algorithm == SchedulingAlgorithm::RoundRobin);

        DrawText("Click one option to continue", (int)card.x + 34, (int)card.y + 406, 16, Fade(RAYWHITE, 0.65f));
    }

    if (step == 4) {
        DrawText("Initialize Time Quantum (Round Robin)", (int)card.x + 34, (int)card.y + 350, 18, Fade(RAYWHITE, 0.80f));
        DrawField("Time Quantum", timeQuantum, (int)card.x + 34, (int)card.y + 374, true);

        DrawText("Press ENTER to finish setup", (int)card.x + 34, (int)card.y + 406, 16, Fade(RAYWHITE, 0.65f));
    }

    if (step >= 5) {
        DrawText("Initialization complete. Press ENTER.",
            (int)card.x + 34, (int)card.y + 352, 20, Color{ 98, 229, 153, 255 });
    }
}

bool SystemConfig::isFinished() const {
    return step >= 5;
}

int SystemConfig::getRAM() {
    return ramGB.empty() ? 0 : std::stoi(ramGB);
}

int SystemConfig::getCPU() {
    return cpuCores.empty() ? 0 : std::stoi(cpuCores);
}

int SystemConfig::getStorage() {
    return storageGB.empty() ? 0 : std::stoi(storageGB);
}

SystemConfig::SchedulingAlgorithm SystemConfig::getSchedulingAlgorithm() const {
    return algorithm;
}

int SystemConfig::getTimeQuantum() {
    if (timeQuantum.empty()) {
        return 1;
    }
    return std::stoi(timeQuantum);
}