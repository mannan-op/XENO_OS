#pragma once
#include <vector>
#include <queue>
#include <string>
#include "process.h"

using namespace std;

struct ProcessActivity {
	process_id_t pid;
	string processName;
	ProcessState state;
	string action;
	string timestamp;
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

	int nextPID;

	SchedulingAlgorithm schedulingAlgorithm;
	int timeQuantum;

	void addActivity(process_id_t pid, const string& name, ProcessState state, const string& action);
	string currentTimestamp() const;


	public:
	ProcessManager();
    ~ProcessManager();

	Process* createProcess(string name, int memoryUsage);
	void terminateProcess(process_id_t pid);
	void initialize();
	void setSchedulingAlgorithm(SchedulingAlgorithm algorithm);
	void setTimeQuantum(int quantum);
	SchedulingAlgorithm getSchedulingAlgorithm() const;
	int getTimeQuantum() const;

	vector<Process*> getProcessTable();
	vector<Process*> getReadyQueueSnapshot() const;
    vector<ProcessActivity> getActivityLog() const;
    const char* getStateText(ProcessState state) const;
};