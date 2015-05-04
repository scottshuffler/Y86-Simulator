#include <stdio.h>
#include <stdbool.h>
#include "tools.h"
#include "memory.h"
#include "dump.h"
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"

void initialize();

int main(int argv, char * args[])
{
    initialize();
	bool loadError = !load(argv, args);
    if (loadError) {

        dumpMemory();
        exit(0);
    }
    forwardType forward;
    statusType status;
    bubbleType bubble;
    int clockCount = 0;
    bool stop = false;
    while (stop == false ) {
        stop = writebackStage(&forward, &status);
        memoryStage(&forward, &status, &bubble);
        executeStage(&forward, &status, &bubble);
        decodeStage(forward, &bubble);
        fetchStage(forward, bubble);
        clockCount++;
    }
    printf("\nTotal clock cycles = %d\n",clockCount);
}

void initialize()
{
    initializeFuncPtrArray();
    clearMemory();
    clearRegisters();
    clearFregister();
    clearDregister();
    clearEregister();
    clearMregister();
    clearWregister();
}


