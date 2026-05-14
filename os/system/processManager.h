#pragma once
#include <vector>
#include <queue>
#include <string>
#include <fstream>
#include <ctime>
#include "process.h"

class Memory;

using namespace std;

struct ProcessActivity {
	process_id_t pid;
	string processName;
	ProcessState state;
	string action;
	string timestamp;
	time_t startTime;
	time_t endTime;
};

class ProcessManager {
public:
	enum class SchedulingAlgorithm {
		FCFS,
		RoundRobin
	};

private:
	vector<Process*> processTable;
	queue<Process*> readyQueue;
	vector<ProcessActivity> activityLog;
	string bookingFilePath;
	string systemLogFilePath;
	Memory* memory;
	int cpuCores;

	int nextPID;

	SchedulingAlgorithm schedulingAlgorithm;
	int timeQuantum;
	Process* currentRunningProcess;
	int currentQuantumTick;

	void addActivity(process_id_t pid, const string& name, ProcessState state, const string& action);
	void closeRunningActivity(process_id_t pid, time_t endTime);
	void rebuildReadyQueue();
	size_t countRunningProcesses() const;
	string currentTimestamp(time_t now) const;
	void ensureStorageFiles();
	void appendSystemLog(const string& message) const;
	Process* selectNextProcess();
	void dispatchProcess(Process* process);
	void preemptCurrentProcess();
	void updateScheduling();


	public:
	ProcessManager();
    ~ProcessManager();

	Process* createProcess(string name, int memoryUsage);
	void terminateProcess(process_id_t pid);
	void setProcessState(process_id_t pid, ProcessState state, const string& action);
	void initialize();
	void setMemory(Memory* mem);
	void setCpuCores(int cores);
	void setSchedulingAlgorithm(SchedulingAlgorithm algorithm);
	void setTimeQuantum(int quantum);
	void update();  // Call this regularly to enforce scheduling decisions
	SchedulingAlgorithm getSchedulingAlgorithm() const;
	int getTimeQuantum() const;
	const string& getBookingFilePath() const;
	const string& getSystemLogFilePath() const;

	vector<Process*> getProcessTable();
	vector<Process*> getReadyQueueSnapshot() const;
    vector<ProcessActivity> getActivityLog() const;
    const char* getStateText(ProcessState state) const;
};