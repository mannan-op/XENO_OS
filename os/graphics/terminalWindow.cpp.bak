#include "terminalWindow.h"
#include <algorithm>

TerminalWindow::TerminalWindow(Memory* mem, int cpu, int storage, process_id_t processId)
    : Window(150, 120, 500, 300, "Terminal")
{
    memory = mem;
    pid = processId;
    cpuCores = cpu;
    storageGB = storage;

    output.push_back("MiniOS Terminal ready.");
    output.push_back("PID: " + std::to_string((long long)pid));
    output.push_back("Try: help, show ram, show cpu, show storage, top, clear");
}

void TerminalWindow::update() {
    Window::update();

    if (!isActive() || !isFocused()) return;

    int key = GetCharPressed();

    while (key > 0) {
        if (key >= 32 && key <= 125 && input.size() < 64) {
            input += (char)key;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !input.empty()) {
        input.pop_back();
    }

    if (IsKeyPressed(KEY_ENTER)) {
        executeCommand(input);
        input.clear();
    }
}
void TerminalWindow::executeCommand(const std::string& cmd) {

    if (cmd.empty()) {
        return;
    }

    if (cmd == "help") {
        output.push_back("Available: show kernel, show user, show ram, show cpu");
        output.push_back("Available: show storage, show pid, top, clear, help");
    }

    else if (cmd == "show kernel") {
        output.push_back("Kernel: MiniOS v1.0");
    }

    else if (cmd == "show user") {
        output.push_back("User: Admin");
    }

    else if (cmd == "show ram") {
        output.push_back("Total RAM: " + std::to_string(memory->getTotal()) + " GB");
        output.push_back("Used RAM: " + std::to_string(memory->getUsed()) + " GB");
        output.push_back("Free RAM: " + std::to_string(memory->getFree()) + " GB");
    }

    else if (cmd == "show cpu") {
        output.push_back("CPU Cores: " + std::to_string(cpuCores));
    }

    else if (cmd == "show storage") {
        output.push_back("Storage: " + std::to_string(storageGB) + " GB");
    }

    else if (cmd == "show pid") {
        output.push_back("PID: " + std::to_string((long long)pid));
    }

    else if (cmd == "top") {
        int fakeLoad = rand() % 100;

        output.push_back("CPU Usage: " + std::to_string(fakeLoad) + "%");
        output.push_back("Used RAM: " + std::to_string(memory->getUsed()) + " GB");
    }

    else if (cmd == "clear") {
        output.clear();
    }

    else {
        output.push_back("Unknown command: " + cmd);
    }

    if (output.size() > 120) {
        output.erase(output.begin(), output.begin() + 30);
    }
}

void TerminalWindow::drawContent() {
    Rectangle terminalArea = { bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 52 };
    DrawRectangleRounded(terminalArea, 0.03f, 8, Color{ 11, 15, 22, 255 });

    const int lineHeight = 18;
    const int maxVisibleLines = (int)((terminalArea.height - 48) / lineHeight);
    int start = std::max(0, (int)output.size() - maxVisibleLines);
    int y = (int)terminalArea.y + 10;

    for (int i = start; i < (int)output.size(); ++i) {
        DrawText(output[i].c_str(), (int)terminalArea.x + 8, y, 16, Color{ 124, 252, 175, 255 });
        y += lineHeight;
    }

    Rectangle promptBar = { terminalArea.x + 8, terminalArea.y + terminalArea.height - 32, terminalArea.width - 16, 22 };
    DrawRectangleRounded(promptBar, 0.3f, 8, Color{ 33, 43, 57, 255 });

    std::string promptText = "> " + input;
    if (((int)(GetTime() * 2.0) % 2) == 0) {
        promptText += "_";
    }
    DrawText(promptText.c_str(), (int)promptBar.x + 8, (int)promptBar.y + 3, 15, Color{ 255, 221, 125, 255 });
}

process_id_t TerminalWindow::getPid() const {
    return pid;
}