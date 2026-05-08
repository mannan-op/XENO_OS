#pragma once
#include <string>
#ifdef _WIN32
#include <windows.h>
using process_id_t = DWORD;
#else
#include <sys/types.h>
using process_id_t = pid_t;
#endif


using namespace std;

enum ProcessState {
	NEW,
	READY,
	RUNNING,
	WAITING,
	TERMINATED
};

class Process {
public:
	process_id_t pid;
	string name;
	ProcessState state;
	
	int memoryUsage;

	Process(process_id_t p, const string& n, ProcessState s);
};