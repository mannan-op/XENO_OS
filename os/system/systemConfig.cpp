#include "systemConfig.h"
#include <algorithm>
using namespace std;

namespace {
    enum class SetupField {
        RAM,
        CPU,
        Storage,
        Scheduler,
        Quantum,
        Done
    };

    void DrawInputCard(const char* label, const string& value, Rectangle rect, bool active) {
        DrawText(label, (int)rect.x, (int)rect.y - 24, 17, Fade(RAYWHITE, 0.82f));
        DrawRectangleRounded(rect, 0.22f, 10, active ? Fade(Color{ 47, 88, 138, 255 }, 0.72f) : Fade(BLACK, 0.28f));
        DrawRectangleLinesEx(rect, 1.6f, active ? Color{ 88, 185, 255, 255 } : Fade(RAYWHITE, 0.16f));

        string shown = value.empty() ? "Type value" : value;
        DrawText(shown.c_str(), (int)rect.x + 14, (int)rect.y + 14, 20, value.empty() ? Fade(RAYWHITE, 0.42f) : RAYWHITE);
    }

    void DrawOption(Rectangle rect, const char* title, const char* subtitle, bool selected) {
        DrawRectangleRounded(rect, 0.18f, 12, selected ? Color{ 66, 156, 220, 255 } : Color{ 23, 33, 46, 255 });
        DrawRectangleLinesEx(rect, 1.2f, selected ? Fade(RAYWHITE, 0.34f) : Fade(RAYWHITE, 0.14f));
        DrawText(title, (int)rect.x + 16, (int)rect.y + 14, 20, RAYWHITE);
        DrawText(subtitle, (int)rect.x + 16, (int)rect.y + 42, 13, Fade(RAYWHITE, 0.68f));
    }

    void DrawActionButton(Rectangle rect, const char* text, Color fill) {
        DrawRectangleRounded(rect, 0.30f, 12, fill);
        DrawRectangleLinesEx(rect, 1.2f, Fade(RAYWHITE, 0.18f));
        DrawText(text, (int)rect.x + 28, (int)rect.y + 14, 18, RAYWHITE);
    }
}

SystemConfig::SystemConfig() {
    step = 0;
    algorithm = SchedulingAlgorithm::FCFS;
}

void SystemConfig::update(float deltaTime) {
    (void)deltaTime;

    SetupField field = (step == 0) ? SetupField::RAM :
        (step == 1) ? SetupField::CPU :
        (step == 2) ? SetupField::Storage :
        (step == 3) ? SetupField::Scheduler :
        (step == 4) ? SetupField::Quantum : SetupField::Done;

    if (field == SetupField::RAM || field == SetupField::CPU || field == SetupField::Storage || field == SetupField::Quantum) {
        int key = GetCharPressed();
        string* current = nullptr;

        if (field == SetupField::RAM) current = &ramGB;
        else if (field == SetupField::CPU) current = &cpuCores;
        else if (field == SetupField::Storage) current = &storageGB;
        else if (field == SetupField::Quantum) current = &timeQuantum;

        while (key > 0) {
            if (key >= '0' && key <= '9' && current->size() < 4) {
                *current += (char)key;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !current->empty()) {
            current->pop_back();
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (field == SetupField::Quantum) {
                step = 5;
            }
            else {
                step++;
            }
        }
    }

    if (field == SetupField::Scheduler) {
        Vector2 mouse = GetMousePosition();
        Rectangle fcfsBtn = { 246, 392, 140, 54 };
        Rectangle rrBtn = { 404, 392, 180, 54 };
        Rectangle continueBtn = { 338, 468, 156, 44 };

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
            else if (CheckCollisionPointRec(mouse, continueBtn)) {
                step = (algorithm == SchedulingAlgorithm::RoundRobin) ? 4 : 5;
                if (algorithm == SchedulingAlgorithm::RoundRobin && timeQuantum.empty()) {
                    timeQuantum = "1";
                }
            }
        }

        if (IsKeyPressed(KEY_ONE)) {
            algorithm = SchedulingAlgorithm::FCFS;
            step = 5;
        }
        else if (IsKeyPressed(KEY_TWO)) {
            algorithm = SchedulingAlgorithm::RoundRobin;
            if (timeQuantum.empty()) {
                timeQuantum = "1";
            }
            step = 4;
        }
        else if (IsKeyPressed(KEY_ENTER)) {
            step = (algorithm == SchedulingAlgorithm::RoundRobin) ? 4 : 5;
            if (algorithm == SchedulingAlgorithm::RoundRobin && timeQuantum.empty()) {
                timeQuantum = "1";
            }
        }
    }
}

void SystemConfig::draw() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    SetupField field = (step == 0) ? SetupField::RAM :
        (step == 1) ? SetupField::CPU :
        (step == 2) ? SetupField::Storage :
        (step == 3) ? SetupField::Scheduler :
        (step == 4) ? SetupField::Quantum : SetupField::Done;

    DrawRectangleGradientV(0, 0, screenW, screenH, Color{ 12, 22, 38, 255 }, Color{ 7, 12, 22, 255 });
    DrawCircle(110, 100, 180, Fade(SKYBLUE, 0.10f));
    DrawCircle(screenW - 150, screenH - 90, 250, Fade(BLUE, 0.10f));

    Rectangle card = { (float)screenW / 2.0f - 340, (float)screenH / 2.0f - 250, 680, 500 };
    DrawRectangleRounded(card, 0.06f, 18, Fade(BLACK, 0.42f));
    DrawRectangleLinesEx(card, 1.4f, Fade(RAYWHITE, 0.18f));

    Rectangle header = { card.x + 28, card.y + 22, card.width - 56, 96 };
    DrawText("Hardware Setup", (int)header.x, (int)header.y, 34, RAYWHITE);
    DrawText("Initialize your virtual machine and choose how processes should be scheduled", (int)header.x, (int)header.y + 40, 17, Fade(RAYWHITE, 0.72f));

    Rectangle progress = { header.x, header.y + 66, header.width, 10 };
    DrawRectangleRounded(progress, 0.5f, 8, Fade(RAYWHITE, 0.08f));
    DrawRectangleRounded({ progress.x, progress.y, progress.width * min(step / 5.0f, 1.0f), progress.height }, 0.5f, 8, Color{ 82, 178, 255, 255 });

    const char* stepTitles[] = { "Memory", "CPU", "Storage", "Scheduler", "Quantum" };
    for (int i = 0; i < 5; ++i) {
        Rectangle dot = { card.x + 40 + i * 116.0f, card.y + 138, 96, 28 };
        DrawRectangleRounded(dot, 0.5f, 8, i < step ? Color{ 66, 156, 220, 255 } : Color{ 32, 44, 58, 255 });
        DrawText(stepTitles[i], (int)dot.x + 14, (int)dot.y + 6, 14, RAYWHITE);
    }

    if (field == SetupField::RAM || field == SetupField::CPU || field == SetupField::Storage) {
        DrawText("Press ENTER to confirm each value", (int)card.x + 28, (int)card.y + 182, 16, Fade(RAYWHITE, 0.68f));
        DrawInputCard("RAM (GB)", ramGB, { card.x + 28, card.y + 214, 300, 50 }, field == SetupField::RAM);
        DrawInputCard("CPU Cores", cpuCores, { card.x + 352, card.y + 214, 300, 50 }, field == SetupField::CPU);
        DrawInputCard("Storage (GB)", storageGB, { card.x + 28, card.y + 294, 300, 50 }, field == SetupField::Storage);
        DrawText("Type numbers only. Backspace edits, Enter advances.", (int)card.x + 28, (int)card.y + 364, 15, Fade(RAYWHITE, 0.52f));
    }

    if (field == SetupField::Scheduler) {
        DrawText("5. Scheduler Initialization", (int)card.x + 28, (int)card.y + 188, 20, Color{ 172, 226, 255, 255 });
        DrawText("Pick a scheduling algorithm. You can also press 1 for FCFS or 2 for Round Robin.", (int)card.x + 28, (int)card.y + 214, 16, Fade(RAYWHITE, 0.66f));

        DrawOption({ card.x + 106, card.y + 276, 210, 84 }, "FCFS", "First come, first served", algorithm == SchedulingAlgorithm::FCFS);
        DrawOption({ card.x + 360, card.y + 276, 210, 84 }, "Round Robin", "Uses time quantum", algorithm == SchedulingAlgorithm::RoundRobin);

        DrawActionButton({ card.x + 262, card.y + 386, 156, 46 }, "Continue", Color{ 68, 146, 214, 255 });
        DrawText("Press Enter to accept the current choice.", (int)card.x + 28, (int)card.y + 448, 15, Fade(RAYWHITE, 0.55f));
    }

    if (field == SetupField::Quantum) {
        DrawText("Round Robin requires a time quantum", (int)card.x + 28, (int)card.y + 188, 20, Color{ 172, 226, 255, 255 });
        DrawText("Enter how many time slices each process should run before rotating.", (int)card.x + 28, (int)card.y + 214, 16, Fade(RAYWHITE, 0.66f));

        DrawInputCard("Time Quantum", timeQuantum, { card.x + 28, card.y + 268, 300, 50 }, true);
        DrawText("Press ENTER to finish setup.", (int)card.x + 28, (int)card.y + 344, 15, Fade(RAYWHITE, 0.58f));
    }

    if (field == SetupField::Done) {
        DrawText("Setup complete", (int)card.x + 28, (int)card.y + 214, 28, Color{ 98, 229, 153, 255 });
        DrawText("Press ENTER to continue to lock screen.", (int)card.x + 28, (int)card.y + 258, 18, Fade(RAYWHITE, 0.70f));
    }
}

bool SystemConfig::isFinished() const {
    return step >= 5;
}

int SystemConfig::getRAM() {
    return ramGB.empty() ? 0 : stoi(ramGB);
}

int SystemConfig::getCPU() {
    return cpuCores.empty() ? 0 : stoi(cpuCores);
}

int SystemConfig::getStorage() {
    return storageGB.empty() ? 0 : stoi(storageGB);
}

SystemConfig::SchedulingAlgorithm SystemConfig::getSchedulingAlgorithm() const {
    return algorithm;
}

int SystemConfig::getTimeQuantum() {
    if (timeQuantum.empty()) {
        return 1;
    }
    return stoi(timeQuantum);
}