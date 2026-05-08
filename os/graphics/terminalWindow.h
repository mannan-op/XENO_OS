#pragma once
#include "windows.h"
#include <vector>
#include <string>
#include "../system/memory.h"
#include "../system/process.h"

using namespace std;

class TerminalWindow : public Window {
private:
    string input;
    vector<string> output;

    Memory* memory;
    process_id_t pid;
    int cpuCores;
    int storageGB;

public:
    TerminalWindow(Memory* mem, int cpu, int storage, process_id_t processId);

    void update() override;
    void drawContent() override;

    void executeCommand(const string& cmd);
    process_id_t getPid() const;
};