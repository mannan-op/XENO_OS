#include "process.h"
using namespace std;

Process::Process(process_id_t p, const string& n, ProcessState s)
    : pid(p), name(n), state(s), memoryUsage(0), timeQuantumUsed(0) {
}