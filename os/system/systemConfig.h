#pragma once
#include "raylib.h"
#include <string>

class SystemConfig {
public:
	enum class SchedulingAlgorithm {
		FCFS,
		RoundRobin
	};

private:
	std::string ramGB;
	std::string cpuCores;
	std::string storageGB;
	std::string timeQuantum;

	SchedulingAlgorithm algorithm;

	int step;

public:

	SystemConfig();

	void update(float dt);
	void draw();

	bool isFinished() const;

	int getRAM();
	int getCPU();
	int getStorage();
	SchedulingAlgorithm getSchedulingAlgorithm() const;
	int getTimeQuantum();
};