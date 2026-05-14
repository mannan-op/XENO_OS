#include "fileViewerWindow.h"
#include <fstream>
#include <algorithm>
using namespace std;

FileViewerWindow::FileViewerWindow(const string& title, const string& path, const string& emptyMessageText, process_id_t processId)
    : Window(240, 90, 860, 540, title) {
    filePath = path;
    emptyMessage = emptyMessageText;
    scrollOffset = 0;
    pid = processId;
    loadFile();
}

void FileViewerWindow::loadFile() {
    lines.clear();

    ifstream file(filePath);
    if (!file.is_open()) {
        lines.push_back("Unable to open file: " + filePath);
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            lines.push_back(" ");
        }
        else {
            lines.push_back(line);
        }
    }

    if (lines.empty()) {
        lines.push_back(emptyMessage);
    }
}

void FileViewerWindow::update() {
    Window::update();

    if (!isActive()) {
        return;
    }

    loadFile();

    Vector2 mouse = GetMousePosition();
    Rectangle body = { bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 50 };
    Rectangle contentPanel = { body.x + 10, body.y + 44, body.width - 20, body.height - 56 };

    if (isFocused()) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f && CheckCollisionPointRec(mouse, contentPanel)) {
            scrollOffset -= (int)wheel * 24;
        }
    }

    int visibleHeight = (int)contentPanel.height - 20;
    int contentHeight = (int)lines.size() * 22;
    int maxScroll = max(0, contentHeight - visibleHeight);

    if (scrollOffset < 0) scrollOffset = 0;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;
}

void FileViewerWindow::drawContent() {
    Rectangle body = { bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 50 };
    DrawRectangleRounded(body, 0.04f, 10, Color{ 12, 17, 24, 255 });

    Rectangle header = { body.x + 10, body.y + 10, body.width - 20, 28 };
    Rectangle contentPanel = { body.x + 10, body.y + 44, body.width - 20, body.height - 56 };

    DrawRectangleRounded(header, 0.12f, 8, Color{ 25, 35, 50, 255 });
    DrawRectangleLinesEx(header, 1.0f, Fade(RAYWHITE, 0.14f));
    DrawText(filePath.c_str(), (int)header.x + 12, (int)header.y + 6, 14, Fade(RAYWHITE, 0.85f));

    DrawRectangleRounded(contentPanel, 0.04f, 8, Color{ 18, 25, 37, 255 });
    DrawRectangleLinesEx(contentPanel, 1.0f, Fade(RAYWHITE, 0.12f));

    int contentHeight = (int)lines.size() * 22;
    int visibleHeight = (int)contentPanel.height - 20;
    int maxScroll = max(0, contentHeight - visibleHeight);

    BeginScissorMode((int)contentPanel.x + 2, (int)contentPanel.y + 2, (int)contentPanel.width - 4, (int)contentPanel.height - 4);

    int y = (int)contentPanel.y + 10 - scrollOffset;
    for (const auto& line : lines) {
        DrawText(line.c_str(), (int)contentPanel.x + 12, y, 16, Color{ 204, 244, 219, 255 });
        y += 22;
    }

    EndScissorMode();

    if (maxScroll > 0) {
        DrawRectangle((int)contentPanel.x + (int)contentPanel.width - 10, (int)contentPanel.y + 8, 2, visibleHeight, Fade(RAYWHITE, 0.10f));
        float thumbH = max(24.0f, ((float)visibleHeight / max(1, contentHeight)) * visibleHeight);
        float thumbY = contentPanel.y + 8 + ((float)scrollOffset / max(1, maxScroll)) * (visibleHeight - thumbH);
        DrawRectangleRounded({ contentPanel.x + contentPanel.width - 12, thumbY, 6, thumbH }, 0.4f, 8, Color{ 108, 194, 232, 255 });
    }
}

process_id_t FileViewerWindow::getPid() const {
    return pid;
}
