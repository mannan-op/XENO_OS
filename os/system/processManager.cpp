#include "processManager.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#ifndef _WIN32
#include <signal.h>
#include <unistd.h>
#endif

ProcessManager::ProcessManager() {
    nextPID = 1;
    schedulingAlgorithm = SchedulingAlgorithm::FCFS;
    timeQuantum = 1;

#ifndef _WIN32
    // Auto-reap exited children so forked demo processes do not remain as zombies.
    signal(SIGCHLD, SIG_IGN);
#endif

    initialize(); 
}

ProcessManager::~ProcessManager() {
    for (auto* process : processTable) {
#ifndef _WIN32
        if (process->state != TERMINATED) {
            kill(process->pid, SIGTERM);
            process->state = TERMINATED;
            addActivity(process->pid, process->name, process->state, "Terminated on shutdown");
        }
#endif
        delete process;
    }
    processTable.clear();
}

void ProcessManager::initialize() {
    for (auto* process : processTable) {
        delete process;
    }
    processTable.clear();

    while (!readyQueue.empty()) {
        readyQueue.pop();
    }

    activityLog.clear();

    nextPID = 1;
}

void ProcessManager::setSchedulingAlgorithm(SchedulingAlgorithm algorithm) {
    schedulingAlgorithm = algorithm;
}

void ProcessManager::setTimeQuantum(int quantum) {
    timeQuantum = quantum > 0 ? quantum : 1;
}

ProcessManager::SchedulingAlgorithm ProcessManager::getSchedulingAlgorithm() const {
    return schedulingAlgorithm;
}

int ProcessManager::getTimeQuantum() const {
    return timeQuantum;
}

Process* ProcessManager::createProcess(string name, int memUsage) {

#ifdef _WIN32
    Process* p = new Process(nextPID++, name, READY);
    p->state = RUNNING;
#else
    pid_t pid = fork();
    if (pid < 0) {
        return nullptr;
    }

    if (pid == 0) {
        // Keep child alive until app/window requests termination.
        while (true) {
            pause();
        }
        _exit(0);
    }

    Process* p = new Process(pid, name, RUNNING);
#endif
    p->memoryUsage = memUsage;

    processTable.push_back(p);
    readyQueue.push(p);
    addActivity(p->pid, p->name, p->state, "Process created");
    return p;
}

void ProcessManager::terminateProcess(process_id_t pid) {
    for (auto* process : processTable) {
        if (process->pid != pid) {
            continue;
        }

#ifndef _WIN32
        if (process->state != TERMINATED) {
            kill(pid, SIGTERM);
        }
#endif
        process->state = TERMINATED;
        addActivity(process->pid, process->name, process->state, "Process terminated");
        return;
    }
}

vector<Process*> ProcessManager::getProcessTable() {
    return processTable;
}

vector<Process*> ProcessManager::getReadyQueueSnapshot() const {
    queue<Process*> copy = readyQueue;
    vector<Process*> ready;

    while (!copy.empty()) {
        ready.push_back(copy.front());
        copy.pop();
    }

    return ready;
}

vector<ProcessActivity> ProcessManager::getActivityLog() const {
    return activityLog;
}

const char* ProcessManager::getStateText(ProcessState state) const {
    switch (state) {
    case NEW: return "NEW";
    case READY: return "READY";
    case RUNNING: return "RUNNING";
    case WAITING: return "WAITING";
    case TERMINATED: return "TERMINATED";
    default: return "UNKNOWN";
    }
}

void ProcessManager::addActivity(process_id_t pid, const string& name, ProcessState state, const string& action) {
    activityLog.push_back({ pid, name, state, action, currentTimestamp() });

    if (activityLog.size() > 250) {
        activityLog.erase(activityLog.begin(), activityLog.begin() + 50);
    }
}

string ProcessManager::currentTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    std::ostringstream out;
    out << std::put_time(local, "%H:%M:%S");
    return out.str();
}