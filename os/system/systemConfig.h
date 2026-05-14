#pragma once
#include "raylib.h"
#include <string>

using namespace std;

class SystemConfig {
public:
	enum class SchedulingAlgorithm {
		FCFS,
		RoundRobin
	};

private:
	string ramGB;
	string cpuCores;
	string storageGB;
	string timeQuantum;

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