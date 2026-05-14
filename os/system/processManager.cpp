#include "processManager.h"
#include "memory.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#ifndef _WIN32
#include <signal.h>
#include <unistd.h>
using namespace std;
#endif

namespace {
    const char* kBookingFileName = "booking.txt";
    const char* kSystemLogFileName = "system_log.txt";
}

ProcessManager::ProcessManager() {
    nextPID = 1;
    schedulingAlgorithm = SchedulingAlgorithm::FCFS;
    timeQuantum = 1;
    bookingFilePath = kBookingFileName;
    systemLogFilePath = kSystemLogFileName;
    memory = nullptr;
    cpuCores = 0;

#ifndef _WIN32
    // Auto-reap exited children so forked demo processes do not remain as zombies.
    signal(SIGCHLD, SIG_IGN);
#endif

    ensureStorageFiles();
    appendSystemLog("Process Manager initialized");

    initialize();
}

ProcessManager::~ProcessManager() {
    appendSystemLog("Process Manager shutting down");

    for (auto* process : processTable) {
#ifndef _WIN32
        if (process->state != TERMINATED) {
            kill(process->pid, SIGTERM);
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

    ensureStorageFiles();
    appendSystemLog("Process Manager state cleared");
}

void ProcessManager::setSchedulingAlgorithm(SchedulingAlgorithm algorithm) {
    schedulingAlgorithm = algorithm;
    appendSystemLog(string("Scheduler set to ") + (algorithm == SchedulingAlgorithm::RoundRobin ? "Round Robin" : "FCFS"));
}

void ProcessManager::setTimeQuantum(int quantum) {
    timeQuantum = quantum > 0 ? quantum : 1;
    appendSystemLog("Time quantum set to " + to_string(timeQuantum));
}

void ProcessManager::setMemory(Memory* mem) {
    memory = mem;
    appendSystemLog("Memory manager attached");
}

void ProcessManager::setCpuCores(int cores) {
    cpuCores = cores > 0 ? cores : 0;
    appendSystemLog("CPU core budget set to " + to_string(cpuCores));
}

ProcessManager::SchedulingAlgorithm ProcessManager::getSchedulingAlgorithm() const {
    return schedulingAlgorithm;
}

int ProcessManager::getTimeQuantum() const {
    return timeQuantum;
}

const string& ProcessManager::getBookingFilePath() const {
    return bookingFilePath;
}

size_t ProcessManager::countRunningProcesses() const {
    size_t runningCount = 0;
    for (const auto* process : processTable) {
        if (process->state == RUNNING) {
            ++runningCount;
        }
    }
    return runningCount;
}

void ProcessManager::rebuildReadyQueue() {
    queue<Process*> rebuilt;
    for (auto* process : processTable) {
        if (process->state == READY) {
            rebuilt.push(process);
        }
    }
    readyQueue.swap(rebuilt);
}

void ProcessManager::setProcessState(process_id_t pid, ProcessState state, const string& action) {
    for (auto* process : processTable) {
        if (process->pid != pid) {
            continue;
        }

        time_t now = time(nullptr);

        if (process->state == RUNNING && state != RUNNING) {
            closeRunningActivity(pid, now);
        }

        process->state = state;
        addActivity(process->pid, process->name, process->state, action);
        appendSystemLog(action + " | PID " + to_string((long long)process->pid) + " (" + process->name + ")");
        rebuildReadyQueue();
        return;
    }
}

const string& ProcessManager::getSystemLogFilePath() const {
    return systemLogFilePath;
}

Process* ProcessManager::createProcess(string name, int memUsage) {
    if (memUsage < 0) {
        memUsage = 0;
    }

    if (memory != nullptr && memory->getFree() < memUsage) {
        appendSystemLog("Rejected process creation for " + name + " due to insufficient memory");
        return nullptr;
    }

#ifdef _WIN32
    Process* p = new Process(nextPID++, name, NEW);
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

    Process* p = new Process(pid, name, NEW);
#endif

    p->memoryUsage = memUsage;
    if (memory != nullptr) {
        memory->allocate(memUsage);
    }
    processTable.push_back(p);
    addActivity(p->pid, p->name, p->state, "Process created");
    p->state = READY;
    readyQueue.push(p);
    addActivity(p->pid, p->name, p->state, "Process admitted to ready queue");
    if (cpuCores > 0 && countRunningProcesses() >= (size_t)cpuCores) {
        appendSystemLog("CPU saturated while admitting process PID " + to_string((long long)p->pid) + ", queued in READY");
    }
    appendSystemLog("Created process PID " + to_string((long long)p->pid) + " (" + p->name + ")");
    return p;
}

void ProcessManager::terminateProcess(process_id_t pid) {
    for (auto* process : processTable) {
        if (process->pid != pid) {
            continue;
        }

        time_t now = time(nullptr);

        if (process->state == RUNNING) {
            closeRunningActivity(pid, now);
        }

#ifndef _WIN32
        if (process->state != TERMINATED) {
            kill(pid, SIGTERM);
        }
#endif
        if (memory != nullptr && process->memoryUsage > 0) {
            memory->free(process->memoryUsage);
        }
        process->state = TERMINATED;
        addActivity(process->pid, process->name, process->state, "Process terminated");
        appendSystemLog("Terminated process PID " + to_string((long long)process->pid) + " (" + process->name + ")");
        rebuildReadyQueue();
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
    case BLOCKED: return "BLOCKED";
    case WAITING: return "WAITING";
    case TERMINATED: return "TERMINATED";
    default: return "UNKNOWN";
    }
}

void ProcessManager::addActivity(process_id_t pid, const string& name, ProcessState state, const string& action) {
    time_t now = time(nullptr);
    string timestamp = currentTimestamp(now);
    activityLog.push_back(ProcessActivity{ pid, name, state, action, timestamp, now, 0 });

    appendSystemLog(timestamp + " | PID " + to_string((long long)pid) + " | " + name + " | " + getStateText(state) + " | " + action);

    if (activityLog.size() > 250) {
        activityLog.erase(activityLog.begin(), activityLog.begin() + 50);
    }
}

void ProcessManager::closeRunningActivity(process_id_t pid, time_t endTime) {
    for (auto it = activityLog.rbegin(); it != activityLog.rend(); ++it) {
        if (it->pid == pid && it->state == RUNNING && it->endTime == 0) {
            it->endTime = endTime;
            return;
        }
    }
}

string ProcessManager::currentTimestamp(time_t now) const {
    tm* local = localtime(&now);
    ostringstream out;
    out << put_time(local, "%H:%M:%S");
    return out.str();
}

void ProcessManager::ensureStorageFiles() {
    {
        ofstream bookingFile(bookingFilePath, ios::app);
    }

    {
        ofstream logFile(systemLogFilePath, ios::app);
    }
}

void ProcessManager::appendSystemLog(const string& message) const {
    ofstream file(systemLogFilePath, ios::app);
    if (!file.is_open()) {
        return;
    }

    file << message << '\n';
}
Process* ProcessManager::selectNextProcess() {
    if (readyQueue.empty()) {
        return nullptr;
    }

    if (schedulingAlgorithm == SchedulingAlgorithm::FCFS) {
        Process* selected = readyQueue.front();
        readyQueue.pop();
        return selected;
    }
    else if (schedulingAlgorithm == SchedulingAlgorithm::RoundRobin) {
        Process* selected = readyQueue.front();
        readyQueue.pop();
        selected->timeQuantumUsed = 0;
        return selected;
    }

    return nullptr;
}

void ProcessManager::dispatchProcess(Process* process) {
    if (process == nullptr || process->state != READY) {
        return;
    }

    currentRunningProcess = process;
    currentQuantumTick = 0;
    process->state = RUNNING;
    addActivity(process->pid, process->name, RUNNING, "Process dispatched to CPU");
    appendSystemLog("Dispatched PID " + std::to_string((long long)process->pid) + " (" + process->name + ") to CPU");
}

void ProcessManager::preemptCurrentProcess() {
    if (currentRunningProcess == nullptr || currentRunningProcess->state != RUNNING) {
        return;
    }

    std::time_t now = std::time(nullptr);
    closeRunningActivity(currentRunningProcess->pid, now);

    currentRunningProcess->state = READY;
    readyQueue.push(currentRunningProcess);
    addActivity(currentRunningProcess->pid, currentRunningProcess->name, READY, "Preempted after time quantum");
    appendSystemLog("Preempted PID " + std::to_string((long long)currentRunningProcess->pid) + " (" + currentRunningProcess->name + ") - returned to ready queue");

    currentRunningProcess = nullptr;
    currentQuantumTick = 0;
}

void ProcessManager::updateScheduling() {
    currentQuantumTick++;

    if (schedulingAlgorithm == SchedulingAlgorithm::RoundRobin) {
        if (currentRunningProcess != nullptr && currentRunningProcess->state == RUNNING) {
            currentRunningProcess->timeQuantumUsed += currentQuantumTick;

            if (currentRunningProcess->timeQuantumUsed >= timeQuantum) {
                preemptCurrentProcess();
            }
        }
    }

    if (currentRunningProcess == nullptr || currentRunningProcess->state != RUNNING) {
        if (!readyQueue.empty()) {
            Process* nextProcess = selectNextProcess();
            if (nextProcess != nullptr) {
                dispatchProcess(nextProcess);
            }
        }
    }
}

void ProcessManager::update() {
    updateScheduling();
}
