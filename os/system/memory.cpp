#include "memory.h"

Memory::Memory(int totalRAM) {
    this->totalRAM = totalRAM;
    this->usedMemory = 0;
    this->freeMemory = totalRAM;

    // Simulate RAM as blocks (1 block = 1 GB)
    ramBlocks.resize(totalRAM, 0); // 0 = free, 1 = used
}

void Memory::allocate(int amount) {
    int allocated = 0;

    for (int i = 0; i < ramBlocks.size() && allocated < amount; i++) {
        if (ramBlocks[i] == 0) {
            ramBlocks[i] = 1;
            allocated++;
        }
    }

    usedMemory += allocated;
    freeMemory = totalRAM - usedMemory;
}

void Memory::free(int amount) {
    int freed = 0;

    for (int i = 0; i < ramBlocks.size() && freed < amount; i++) {
        if (ramBlocks[i] == 1) {
            ramBlocks[i] = 0;
            freed++;
        }
    }

    usedMemory -= freed;
    if (usedMemory < 0) usedMemory = 0;

    freeMemory = totalRAM - usedMemory;
}

int Memory::getTotal() { return totalRAM; }
int Memory::getUsed() { return usedMemory; }
int Memory::getFree() { return freeMemory; }