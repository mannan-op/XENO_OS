#pragma once
#include <vector>

class Memory {
private:
    int totalRAM;      // in GB
    int usedMemory;    // in GB
    int freeMemory;    // in GB

    std::vector<int> ramBlocks; // simulate memory blocks

public:
    Memory(int totalRAM);

    void allocate(int amount);
    void free(int amount);

    int getTotal();
    int getUsed();
    int getFree();
};
