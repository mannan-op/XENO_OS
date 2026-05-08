#pragma once

#include "windows.h"
#include "../system/processManager.h"

class LoggerWindow : public Window {
public:
    enum class FilterMode {
        All,
        Running,
        Terminated
    };

private:
    ProcessManager* processManager;
    process_id_t pid;
    FilterMode filterMode;
    std::string searchQuery;
    bool searchFocused;
    int processScroll;
    int activityScroll;

public:
    LoggerWindow(ProcessManager* pm, process_id_t processId);

    void update() override;
    void drawContent() override;
    process_id_t getPid() const;
};
