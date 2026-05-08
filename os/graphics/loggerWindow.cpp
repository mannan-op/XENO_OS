#include "loggerWindow.h"
#include <algorithm>
#include <cctype>
#include <vector>

namespace {
    std::string ToLower(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
            return (char)std::tolower(c);
        });
        return value;
    }

    bool ContainsInsensitive(const std::string& value, const std::string& needle) {
        if (needle.empty()) {
            return true;
        }
        return ToLower(value).find(ToLower(needle)) != std::string::npos;
    }

    bool MatchesFilter(ProcessState state, LoggerWindow::FilterMode mode) {
        switch (mode) {
        case LoggerWindow::FilterMode::All:
            return true;
        case LoggerWindow::FilterMode::Running:
            return state != TERMINATED;
        case LoggerWindow::FilterMode::Terminated:
            return state == TERMINATED;
        default:
            return true;
        }
    }

    void DrawFilterButton(Rectangle rect, const char* text, bool active) {
        DrawRectangleRounded(rect, 0.35f, 8, active ? Color{ 73, 155, 215, 255 } : Color{ 34, 47, 62, 255 });
        DrawRectangleLinesEx(rect, 1.0f, active ? Fade(RAYWHITE, 0.30f) : Fade(RAYWHITE, 0.14f));
        DrawText(text, (int)rect.x + 10, (int)rect.y + 5, 12, RAYWHITE);
    }
}

LoggerWindow::LoggerWindow(ProcessManager* pm, process_id_t processId)
    : Window(200, 70, 860, 560, "Process Logger") {
    processManager = pm;
    pid = processId;
    filterMode = FilterMode::All;
    searchFocused = false;
    processScroll = 0;
    activityScroll = 0;
}

void LoggerWindow::update() {
    Window::update();

    if (!isActive()) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    Rectangle body = { bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 50 };
    Rectangle header = { body.x + 10, body.y + 10, body.width - 20, 96 };
    Rectangle processPanel = { body.x + 10, body.y + 116, body.width - 20, 186 };
    Rectangle activityPanel = { body.x + 10, body.y + 314, body.width - 20, body.height - 324 };
    Rectangle searchBox = { header.x + 14, header.y + 48, 240, 28 };
    Rectangle allBtn = { header.x + header.width - 236, header.y + 14, 60, 24 };
    Rectangle runningBtn = { header.x + header.width - 170, header.y + 14, 70, 24 };
    Rectangle terminatedBtn = { header.x + header.width - 94, header.y + 14, 82, 24 };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        bool clickedSearch = CheckCollisionPointRec(mouse, searchBox);
        bool clickedButton = false;

        if (CheckCollisionPointRec(mouse, allBtn)) {
            filterMode = FilterMode::All;
            clickedButton = true;
        }
        else if (CheckCollisionPointRec(mouse, runningBtn)) {
            filterMode = FilterMode::Running;
            clickedButton = true;
        }
        else if (CheckCollisionPointRec(mouse, terminatedBtn)) {
            filterMode = FilterMode::Terminated;
            clickedButton = true;
        }

        searchFocused = clickedSearch;
        if (clickedSearch || clickedButton || CheckCollisionPointRec(mouse, processPanel) || CheckCollisionPointRec(mouse, activityPanel)) {
            setFocused(true);
        }
    }

    if (isFocused()) {
        if (searchFocused) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125 && searchQuery.size() < 32) {
                    searchQuery += (char)key;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !searchQuery.empty()) {
                searchQuery.pop_back();
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                searchFocused = false;
            }
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            if (CheckCollisionPointRec(mouse, processPanel)) {
                processScroll -= (int)wheel * 24;
            }
            else if (CheckCollisionPointRec(mouse, activityPanel)) {
                activityScroll -= (int)wheel * 24;
            }
        }
    }

    auto processTable = processManager->getProcessTable();
    std::vector<const Process*> filteredProcesses;
    for (const auto* p : processTable) {
        if (!MatchesFilter(p->state, filterMode)) {
            continue;
        }

        if (!ContainsInsensitive(std::to_string((long long)p->pid), searchQuery) &&
            !ContainsInsensitive(p->name, searchQuery)) {
            continue;
        }

        filteredProcesses.push_back(p);
    }

    auto log = processManager->getActivityLog();
    std::vector<const ProcessActivity*> filteredLog;
    for (const auto& entry : log) {
        if (!MatchesFilter(entry.state, filterMode)) {
            continue;
        }

        if (!ContainsInsensitive(std::to_string((long long)entry.pid), searchQuery) &&
            !ContainsInsensitive(entry.processName, searchQuery)) {
            continue;
        }

        filteredLog.push_back(&entry);
    }

    int processRowHeight = 24;
    int processVisibleHeight = (int)processPanel.height - 84;
    int processContentHeight = (int)filteredProcesses.size() * processRowHeight;
    int processMaxScroll = std::max(0, processContentHeight - processVisibleHeight);
    if (processScroll < 0) processScroll = 0;
    if (processScroll > processMaxScroll) processScroll = processMaxScroll;

    int logRowHeight = 22;
    int logVisibleHeight = (int)activityPanel.height - 46;
    int logContentHeight = (int)filteredLog.size() * logRowHeight;
    int logMaxScroll = std::max(0, logContentHeight - logVisibleHeight);
    if (activityScroll < 0) activityScroll = 0;
    if (activityScroll > logMaxScroll) activityScroll = logMaxScroll;
}

void LoggerWindow::drawContent() {
    Rectangle body = { bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 50 };
    DrawRectangleRounded(body, 0.04f, 10, Color{ 12, 17, 24, 255 });

    Rectangle header = { body.x + 10, body.y + 10, body.width - 20, 96 };
    Rectangle processPanel = { body.x + 10, body.y + 116, body.width - 20, 186 };
    Rectangle activityPanel = { body.x + 10, body.y + 314, body.width - 20, body.height - 324 };
    Rectangle searchBox = { header.x + 14, header.y + 48, 240, 28 };
    Rectangle allBtn = { header.x + header.width - 236, header.y + 14, 60, 24 };
    Rectangle runningBtn = { header.x + header.width - 170, header.y + 14, 70, 24 };
    Rectangle terminatedBtn = { header.x + header.width - 94, header.y + 14, 82, 24 };

    DrawRectangleRounded(header, 0.08f, 10, Color{ 21, 33, 47, 255 });
    DrawRectangleLinesEx(header, 1.0f, Fade(RAYWHITE, 0.14f));
    DrawText("Live process telemetry", (int)header.x + 14, (int)header.y + 8, 20, RAYWHITE);
    DrawText("Filter apps, inspect states, and search by PID or process name", (int)header.x + 14, (int)header.y + 34, 16, Fade(RAYWHITE, 0.72f));

    auto processTable = processManager->getProcessTable();
    int runningCount = 0;
    int terminatedCount = 0;
    for (const auto* p : processTable) {
        if (p->state == TERMINATED) terminatedCount++;
        else runningCount++;
    }

    Rectangle stat1 = { header.x + header.width - 236, header.y + 44, 68, 34 };
    Rectangle stat2 = { header.x + header.width - 160, header.y + 44, 68, 34 };
    Rectangle stat3 = { header.x + header.width - 84, header.y + 44, 68, 34 };
    DrawRectangleRounded(stat1, 0.2f, 8, Color{ 24, 44, 64, 255 });
    DrawRectangleRounded(stat2, 0.2f, 8, Color{ 28, 64, 74, 255 });
    DrawRectangleRounded(stat3, 0.2f, 8, Color{ 60, 37, 48, 255 });
    DrawText(TextFormat("%d", (int)processTable.size()), (int)stat1.x + 22, (int)stat1.y + 7, 16, RAYWHITE);
    DrawText("all", (int)stat1.x + 18, (int)stat1.y + 20, 10, Fade(RAYWHITE, 0.66f));
    DrawText(TextFormat("%d", runningCount), (int)stat2.x + 22, (int)stat2.y + 7, 16, RAYWHITE);
    DrawText("live", (int)stat2.x + 15, (int)stat2.y + 20, 10, Fade(RAYWHITE, 0.66f));
    DrawText(TextFormat("%d", terminatedCount), (int)stat3.x + 22, (int)stat3.y + 7, 16, RAYWHITE);
    DrawText("done", (int)stat3.x + 12, (int)stat3.y + 20, 10, Fade(RAYWHITE, 0.66f));

    DrawText("Filter", (int)header.x + 14, (int)header.y + 14, 14, Fade(RAYWHITE, 0.60f));
    DrawFilterButton(allBtn, "ALL", filterMode == FilterMode::All);
    DrawFilterButton(runningBtn, "RUNNING", filterMode == FilterMode::Running);
    DrawFilterButton(terminatedBtn, "TERMINATED", filterMode == FilterMode::Terminated);

    DrawText("Search", (int)searchBox.x, (int)searchBox.y - 16, 13, Fade(RAYWHITE, 0.60f));
    DrawRectangleRounded(searchBox, 0.30f, 8, searchFocused ? Color{ 20, 41, 66, 255 } : Color{ 26, 34, 45, 255 });
    DrawRectangleLinesEx(searchBox, 1.0f, searchFocused ? Fade(SKYBLUE, 0.8f) : Fade(RAYWHITE, 0.18f));
    std::string searchShown = searchQuery.empty() ? "Type PID or name..." : searchQuery;
    if (searchFocused && ((int)(GetTime() * 2.0f) % 2 == 0)) {
        searchShown += "_";
    }
    DrawText(searchShown.c_str(), (int)searchBox.x + 10, (int)searchBox.y + 6, 13, searchQuery.empty() ? Fade(RAYWHITE, 0.40f) : RAYWHITE);

    DrawRectangleRounded(processPanel, 0.04f, 8, Color{ 22, 31, 43, 255 });
    DrawRectangleRounded(activityPanel, 0.04f, 8, Color{ 18, 25, 37, 255 });
    DrawRectangleLinesEx(processPanel, 1.0f, Fade(RAYWHITE, 0.16f));
    DrawRectangleLinesEx(activityPanel, 1.0f, Fade(RAYWHITE, 0.16f));

    DrawText("Running Process Table", (int)processPanel.x + 12, (int)processPanel.y + 8, 18, RAYWHITE);
    DrawText("Activity Log", (int)activityPanel.x + 12, (int)activityPanel.y + 8, 18, RAYWHITE);

    DrawText("PID", (int)processPanel.x + 14, (int)processPanel.y + 38, 16, Fade(RAYWHITE, 0.72f));
    DrawText("Name", (int)processPanel.x + 120, (int)processPanel.y + 38, 16, Fade(RAYWHITE, 0.72f));
    DrawText("State", (int)processPanel.x + 340, (int)processPanel.y + 38, 16, Fade(RAYWHITE, 0.72f));
    DrawText("Memory", (int)processPanel.x + 520, (int)processPanel.y + 38, 16, Fade(RAYWHITE, 0.72f));

    auto processTableVisible = processManager->getProcessTable();
    std::vector<const Process*> filteredProcesses;
    for (const auto* p : processTableVisible) {
        if (!MatchesFilter(p->state, filterMode)) {
            continue;
        }
        if (!ContainsInsensitive(std::to_string((long long)p->pid), searchQuery) &&
            !ContainsInsensitive(p->name, searchQuery)) {
            continue;
        }
        filteredProcesses.push_back(p);
    }

    int processRowHeight = 24;
    int processVisibleHeight = (int)processPanel.height - 84;
    int processContentHeight = (int)filteredProcesses.size() * processRowHeight;
    int processMaxScroll = std::max(0, processContentHeight - processVisibleHeight);
    if (processScroll < 0) processScroll = 0;
    if (processScroll > processMaxScroll) processScroll = processMaxScroll;

    BeginScissorMode((int)processPanel.x + 2, (int)processPanel.y + 34, (int)processPanel.width - 20, (int)processPanel.height - 42);
    int processY = (int)processPanel.y + 58 - processScroll;
    for (const auto* p : filteredProcesses) {
        DrawText(TextFormat("%lld", (long long)p->pid), (int)processPanel.x + 14, processY, 16, Color{ 153, 224, 255, 255 });
        DrawText(p->name.c_str(), (int)processPanel.x + 120, processY, 16, Color{ 219, 236, 255, 255 });
        DrawText(processManager->getStateText(p->state), (int)processPanel.x + 340, processY, 16, Color{ 255, 218, 142, 255 });
        DrawText(TextFormat("%d MB", p->memoryUsage), (int)processPanel.x + 520, processY, 16, Color{ 190, 255, 200, 255 });
        processY += processRowHeight;
    }
    EndScissorMode();

    if (filteredProcesses.empty()) {
        DrawText("No matching processes", (int)processPanel.x + 14, (int)processPanel.y + 72, 14, Fade(RAYWHITE, 0.55f));
    }

    if (processMaxScroll > 0) {
        DrawRectangle((int)processPanel.x + (int)processPanel.width - 10, (int)processPanel.y + 36, 2, processVisibleHeight, Fade(RAYWHITE, 0.10f));
        float thumbH = std::max(24.0f, ((float)processVisibleHeight / std::max(1, processContentHeight)) * processVisibleHeight);
        float thumbY = processPanel.y + 36 + ((float)processScroll / std::max(1, processMaxScroll)) * (processVisibleHeight - thumbH);
        DrawRectangleRounded({ processPanel.x + processPanel.width - 12, thumbY, 6, thumbH }, 0.4f, 8, Color{ 108, 194, 232, 255 });
    }

    auto log = processManager->getActivityLog();
    std::vector<const ProcessActivity*> filteredLog;
    for (const auto& entry : log) {
        if (!MatchesFilter(entry.state, filterMode)) {
            continue;
        }
        if (!ContainsInsensitive(std::to_string((long long)entry.pid), searchQuery) &&
            !ContainsInsensitive(entry.processName, searchQuery)) {
            continue;
        }
        filteredLog.push_back(&entry);
    }

    int logRowHeight = 22;
    int logVisibleHeight = (int)activityPanel.height - 46;
    int logContentHeight = (int)filteredLog.size() * logRowHeight;
    int logMaxScroll = std::max(0, logContentHeight - logVisibleHeight);
    if (activityScroll < 0) activityScroll = 0;
    if (activityScroll > logMaxScroll) activityScroll = logMaxScroll;

    BeginScissorMode((int)activityPanel.x + 2, (int)activityPanel.y + 32, (int)activityPanel.width - 4, (int)activityPanel.height - 38);
    int logY = (int)activityPanel.y + 36 - activityScroll;
    for (const auto* entry : filteredLog) {
        std::string line = entry->timestamp + " | PID " + std::to_string((long long)entry->pid) +
            " | " + entry->processName + " | " + processManager->getStateText(entry->state) +
            " | " + entry->action;
        DrawText(line.c_str(), (int)activityPanel.x + 12, logY, 15, Color{ 204, 244, 219, 255 });
        logY += logRowHeight;
    }
    EndScissorMode();

    if (filteredLog.empty()) {
        DrawText("No matching log entries", (int)activityPanel.x + 12, (int)activityPanel.y + 36, 14, Fade(RAYWHITE, 0.55f));
    }

    if (logMaxScroll > 0) {
        DrawRectangle((int)activityPanel.x + (int)activityPanel.width - 10, (int)activityPanel.y + 36, 2, logVisibleHeight, Fade(RAYWHITE, 0.10f));
        float thumbH = std::max(24.0f, ((float)logVisibleHeight / std::max(1, logContentHeight)) * logVisibleHeight);
        float thumbY = activityPanel.y + 36 + ((float)activityScroll / std::max(1, logMaxScroll)) * (logVisibleHeight - thumbH);
        DrawRectangleRounded({ activityPanel.x + activityPanel.width - 12, thumbY, 6, thumbH }, 0.4f, 8, Color{ 108, 194, 232, 255 });
    }

    DrawText("Click search then type PID or process name. Use mouse wheel over each panel.",
        (int)activityPanel.x + 12, (int)activityPanel.y + (int)activityPanel.height - 22, 14, Fade(RAYWHITE, 0.50f));
}

process_id_t LoggerWindow::getPid() const {
    return pid;
}
