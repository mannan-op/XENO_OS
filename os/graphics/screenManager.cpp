#include "screenManager.h"

ScreenManager::ScreenManager() {
    currentScreen = BOOT_SCREEN;
    memory = nullptr;
    processManager = new ProcessManager();
    desktopScreen = nullptr;
    timeQuantum = 1;
    roundRobinEnabled = false;
}

ScreenManager::~ScreenManager() {
    delete memory;
    delete processManager;
    delete desktopScreen;
}

void ScreenManager::update(float deltaTime) {
    if (currentScreen == BOOT_SCREEN) {
        bootScreen.update(deltaTime);

        if (bootScreen.isFinished()) {
            currentScreen = SYSTEM_CONFIG_SCREEN;
        }
    }
    else if (currentScreen == SYSTEM_CONFIG_SCREEN) {
        systemConfigScreen.update(deltaTime);

        if (systemConfigScreen.isFinished()) {

			int ram = systemConfigScreen.getRAM();
			int cpu = systemConfigScreen.getCPU();
			int storage = systemConfigScreen.getStorage();
            timeQuantum = systemConfigScreen.getTimeQuantum();
            roundRobinEnabled = systemConfigScreen.getSchedulingAlgorithm() == SystemConfig::SchedulingAlgorithm::RoundRobin;
            processManager->setSchedulingAlgorithm(roundRobinEnabled ? ProcessManager::SchedulingAlgorithm::RoundRobin : ProcessManager::SchedulingAlgorithm::FCFS);
            processManager->setTimeQuantum(timeQuantum);

			memory = new Memory(ram);
            desktopScreen = new Desktop(memory, processManager, cpu, storage);
            currentScreen = LOCK_SCREEN;
        }
    }

    else if (currentScreen == LOCK_SCREEN) {
        lockScreen.update(deltaTime);

        if (lockScreen.isUnlocked()) {
            currentScreen = DESKTOP_SCREEN;
        }
    }

    else if (currentScreen == DESKTOP_SCREEN) {
        if (desktopScreen != nullptr)
            desktopScreen->update();
    }
}

void ScreenManager::draw() {
    if (currentScreen == BOOT_SCREEN) {
        bootScreen.draw();
    }
    else if (currentScreen == SYSTEM_CONFIG_SCREEN) {
        systemConfigScreen.draw();
    }

    else if (currentScreen == LOCK_SCREEN) {
        lockScreen.draw();
    }

    else if (currentScreen == DESKTOP_SCREEN) {
        if (desktopScreen != nullptr)
            desktopScreen->draw();
    }
}