#include "schedulerWindow.h"
#include <algorithm>

SchedulerWindow::SchedulerWindow(ProcessManager* pm, process_id_t processId)
    : Window(220, 80, 880, 560, "Scheduler Monitor") {
    processManager = pm;
    pid = processId;
    queueScroll = 0;
    tableScroll = 0;
}

void SchedulerWindow::update() {
    Window::update();

    if (!isActive()) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    Rectangle body = { bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 50 };
    Rectangle algorithmCard = { body.x + 10, body.y + 10, body.width - 20, 78 };
    Rectangle queuePanel = { body.x + 10, body.y + 100, body.width * 0.46f - 15, body.height - 110 };
    Rectangle tablePanel = { queuePanel.x + queuePanel.width + 10, body.y + 100, body.width - queuePanel.width - 30, body.height - 110 };

    if (isFocused()) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            if (CheckCollisionPointRec(mouse, queuePanel)) {
                queueScroll -= (int)wheel * 24;
            }
            else if (CheckCollisionPointRec(mouse, tablePanel)) {
                tableScroll -= (int)wheel * 24;
            }
        }
    }

    auto ready = processManager->getReadyQueueSnapshot();
    auto table = processManager->getProcessTable();

    int queueContentHeight = (int)ready.size() * 24;
    int tableContentHeight = (int)table.size() * 24;

    int queueVisible = (int)queuePanel.height - 54;
    int tableVisible = (int)tablePanel.height - 54;

    int queueMaxScroll = std::max(0, queueContentHeight - queueVisible);
    int tableMaxScroll = std::max(0, tableContentHeight - tableVisible);

    if (queueScroll < 0) queueScroll = 0;
    if (queueScroll > queueMaxScroll) queueScroll = queueMaxScroll;
    if (tableScroll < 0) tableScroll = 0;
    if (tableScroll > tableMaxScroll) tableScroll = tableMaxScroll;
}

void SchedulerWindow::drawContent() {
    Rectangle body = { bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 50 };
    DrawRectangleRounded(body, 0.04f, 10, Color{ 11, 16, 24, 255 });

    Rectangle algorithmCard = { body.x + 10, body.y + 10, body.width - 20, 78 };
    Rectangle queuePanel = { body.x + 10, body.y + 100, body.width * 0.46f - 15, body.height - 110 };
    Rectangle tablePanel = { queuePanel.x + queuePanel.width + 10, body.y + 100, body.width - queuePanel.width - 30, body.height - 110 };

    DrawRectangleRounded(algorithmCard, 0.08f, 10, Color{ 22, 33, 47, 255 });
    DrawRectangleLinesEx(algorithmCard, 1.0f, Fade(RAYWHITE, 0.16f));

    std::string algorithmName = processManager->getSchedulingAlgorithm() == ProcessManager::SchedulingAlgorithm::RoundRobin ? "Round Robin" : "FCFS";
    DrawText("Scheduler Monitor", (int)algorithmCard.x + 14, (int)algorithmCard.y + 8, 20, RAYWHITE);
    DrawText(("Algorithm: " + algorithmName).c_str(), (int)algorithmCard.x + 14, (int)algorithmCard.y + 34, 18, Color{ 160, 223, 255, 255 });
    DrawText(TextFormat("Time Quantum: %d", processManager->getTimeQuantum()), (int)algorithmCard.x + 320, (int)algorithmCard.y + 34, 18, Color{ 255, 230, 160, 255 });

    DrawRectangleRounded(queuePanel, 0.04f, 8, Color{ 18, 26, 38, 255 });
    DrawRectangleRounded(tablePanel, 0.04f, 8, Color{ 19, 29, 43, 255 });
    DrawRectangleLinesEx(queuePanel, 1.0f, Fade(RAYWHITE, 0.16f));
    DrawRectangleLinesEx(tablePanel, 1.0f, Fade(RAYWHITE, 0.16f));

    DrawText("Ready Queue", (int)queuePanel.x + 12, (int)queuePanel.y + 8, 18, RAYWHITE);
    DrawText("Process Table", (int)tablePanel.x + 12, (int)tablePanel.y + 8, 18, RAYWHITE);

    auto ready = processManager->getReadyQueueSnapshot();
    auto table = processManager->getProcessTable();

    DrawText("Order", (int)queuePanel.x + 14, (int)queuePanel.y + 38, 16, Fade(RAYWHITE, 0.72f));
    DrawText("PID", (int)queuePanel.x + 90, (int)queuePanel.y + 38, 16, Fade(RAYWHITE, 0.72f));
    DrawText("Name", (int)queuePanel.x + 160, (int)queuePanel.y + 38, 16, Fade(RAYWHITE, 0.72f));

    DrawText("PID", (int)tablePanel.x + 14, (int)tablePanel.y + 38, 16, Fade(RAYWHITE, 0.72f));
    DrawText("Name", (int)tablePanel.x + 90, (int)tablePanel.y + 38, 16, Fade(RAYWHITE, 0.72f));
    DrawText("State", (int)tablePanel.x + 295, (int)tablePanel.y + 38, 16, Fade(RAYWHITE, 0.72f));
    DrawText("Memory", (int)tablePanel.x + 425, (int)tablePanel.y + 38, 16, Fade(RAYWHITE, 0.72f));

    int queueVisible = (int)queuePanel.height - 54;
    int tableVisible = (int)tablePanel.height - 54;

    BeginScissorMode((int)queuePanel.x + 2, (int)queuePanel.y + 34, (int)queuePanel.width - 4, queueVisible);
    int queueY = (int)queuePanel.y + 58 - queueScroll;
    for (int i = 0; i < (int)ready.size(); ++i) {
        const Process* p = ready[i];
        DrawText(TextFormat("%d", i + 1), (int)queuePanel.x + 14, queueY, 16, Color{ 98, 229, 153, 255 });
        DrawText(TextFormat("%lld", (long long)p->pid), (int)queuePanel.x + 90, queueY, 16, Color{ 153, 224, 255, 255 });
        DrawText(p->name.c_str(), (int)queuePanel.x + 160, queueY, 16, RAYWHITE);
        queueY += 24;
    }
    EndScissorMode();

    BeginScissorMode((int)tablePanel.x + 2, (int)tablePanel.y + 34, (int)tablePanel.width - 4, tableVisible);
    int tableY = (int)tablePanel.y + 58 - tableScroll;
    for (const auto* p : table) {
        DrawText(TextFormat("%lld", (long long)p->pid), (int)tablePanel.x + 14, tableY, 16, Color{ 153, 224, 255, 255 });
        DrawText(p->name.c_str(), (int)tablePanel.x + 90, tableY, 16, RAYWHITE);
        DrawText(processManager->getStateText(p->state), (int)tablePanel.x + 295, tableY, 16, Color{ 255, 218, 142, 255 });
        DrawText(TextFormat("%d MB", p->memoryUsage), (int)tablePanel.x + 425, tableY, 16, Color{ 190, 255, 200, 255 });
        tableY += 24;
    }
    EndScissorMode();

    if (!ready.empty()) {
        DrawText("Scroll inside each panel to browse all entries", (int)queuePanel.x + 14, (int)queuePanel.y + (int)queuePanel.height - 22, 14, Fade(RAYWHITE, 0.50f));
    }
    else {
        DrawText("Ready queue is empty", (int)queuePanel.x + 14, (int)queuePanel.y + 66, 14, Fade(RAYWHITE, 0.55f));
    }

    if (ready.size() * 24 > queueVisible) {
        DrawRectangle((int)queuePanel.x + (int)queuePanel.width - 10, (int)queuePanel.y + 36, 2, queueVisible, Fade(RAYWHITE, 0.10f));
        float thumbH = std::max(24.0f, ((float)queueVisible / std::max(1, (int)ready.size() * 24)) * queueVisible);
        float thumbY = queuePanel.y + 36 + ((float)queueScroll / std::max(1, (int)ready.size() * 24 - queueVisible)) * (queueVisible - thumbH);
        DrawRectangleRounded({ queuePanel.x + queuePanel.width - 12, thumbY, 6, thumbH }, 0.4f, 8, Color{ 108, 194, 232, 255 });
    }

    if (table.size() * 24 > tableVisible) {
        DrawRectangle((int)tablePanel.x + (int)tablePanel.width - 10, (int)tablePanel.y + 36, 2, tableVisible, Fade(RAYWHITE, 0.10f));
        float thumbH = std::max(24.0f, ((float)tableVisible / std::max(1, (int)table.size() * 24)) * tableVisible);
        float thumbY = tablePanel.y + 36 + ((float)tableScroll / std::max(1, (int)table.size() * 24 - tableVisible)) * (tableVisible - thumbH);
        DrawRectangleRounded({ tablePanel.x + tablePanel.width - 12, thumbY, 6, thumbH }, 0.4f, 8, Color{ 108, 194, 232, 255 });
    }
}

process_id_t SchedulerWindow::getPid() const {
    return pid;
}
