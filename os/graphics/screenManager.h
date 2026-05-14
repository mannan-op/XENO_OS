#pragma once
#include "../boot/boot.h"
#include "../system/systemConfig.h"
#include "../system/memory.h"
#include "../system/processManager.h"
#include "desktop.h"
#include "lockScreen.h"
#include "systemModeScreen.h"

enum ScreenState {
	BOOT_SCREEN,
	SYSTEM_MODE_SCREEN,
	DESKTOP_SCREEN,
	LOCK_SCREEN,
	SYSTEM_CONFIG_SCREEN
};

class ScreenManager {
private:
	ScreenState currentScreen;
	BootScreen bootScreen;
	SystemModeScreen systemModeScreen;
	LockScreen lockScreen;
	Desktop* desktopScreen;
	SystemConfig systemConfigScreen;

	Memory* memory;
    ProcessManager* processManager;
	int timeQuantum;
	bool roundRobinEnabled;

public:
	ScreenManager();
	~ScreenManager();

	void update(float dt);
	void draw();
};