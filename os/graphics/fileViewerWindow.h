#pragma once
#include "windows.h"
#include "../system/process.h"
#include <vector>
#include <string>

using namespace std;

class FileViewerWindow : public Window {
private:
    string filePath;
    vector<string> lines;
    int scrollOffset;
    string emptyMessage;
    process_id_t pid;

    void loadFile();

public:
    FileViewerWindow(const string& title, const string& path, const string& emptyMessageText, process_id_t processId);

    void update() override;
    void drawContent() override;
    process_id_t getPid() const;
};
