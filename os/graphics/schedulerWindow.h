#pragma once

#include "windows.h"
#include "../system/processManager.h"
#include <vector>

class SchedulerWindow : public Window {
private:
    ProcessManager* processManager;
    process_id_t pid;
    int queueScroll;
    int tableScroll;

public:
    SchedulerWindow(ProcessManager* pm, process_id_t processId);

    void update() override;
    void drawContent() override;
    process_id_t getPid() const;
};
