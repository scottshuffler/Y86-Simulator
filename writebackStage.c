#include <stdbool.h>
#include "writebackStage.h"
#include "tools.h"
#include "dump.h"
#include "instructions.h"
#include "forwarding.h"
#include "registers.h"
#include "status.h"
//W register holds the input for the writeback stage.
//It is only accessible from this file. (static)
static wregister W;

// Function: getWregister
// Description: Returns a copy of the W register
// Params: none
// Returns: wregister
// Modifies: none
wregister getWregister()
{
    return W;
}

// Function: clearWregister
// Description: Returns a copy of the W register
// Params: none
// Returns: none
// Modifies: W
void clearWregister()
{
  clearBuffer((char *) &W, sizeof(W));
  W.stat = AOK;
  W.icode = nop;
}

// Function: updateWregister
// Description: Returns a copy of the W register
// Params: stat, icode, valE, valM, dstE, dstM
// Returns: None
// Modifies: stat, icode, valE, valM, dstE, dstM
void updateWregister(int stat, int icode, int valE, int valM, int dstE, int dstM)
{
	W.stat = stat;
	W.icode = icode;
	W.valE = valE;
	W.valM = valM;
	W.dstE = dstE;
	W.dstM = dstM;
}

// Function: writebackStage
// Description: Simulates the writebackStage of the y86 pipe
// Params: *forward, *status
// Returns: Bool
// Modifies: None
bool writebackStage(forwardType *forward, statusType *status)
{
    forward->W_dstE = W.dstE;
    forward->W_valE = W.valE;
    forward->W_valM = W.valM;
    forward->W_dstM = W.dstM;
    status->W_stat = W.stat;
	forward->W_icode = W.icode;
    //setRegister(W.dstE, W.valE);
    //setRegister(W.dstM, W.valM);
    /*if (W.icode == mrmovl || W.icode == popl)
    {
        setRegister(W.dstM, W.valM);
    }
    if(W.icode == OPL || W.icode == rrmovl || W.icode == irmovl || W.icode == popl || W.icode == pushl)
    {
        setRegister(W.dstE, W.valE);
    }*/
    if(W.dstM == none)
    {
        W.valM = 0;
    }
    
    if (W.stat == HLT || W.stat == INS || W.stat == ADR)
    {
	    if(W.stat == INS)
        {
            printf("Invalid instruction\n");
            dumpProgramRegisters();
            dumpProcessorRegisters();
            dumpMemory();
        }
        if(W.stat == ADR)
        {
            printf("Invalid memory address\n");
            dumpProgramRegisters();
            dumpProcessorRegisters();
            dumpMemory();
        }
        return true;
    }
    else
    {
    setRegister(W.dstE, W.valE);
    setRegister(W.dstM, W.valM);
    if (W.icode == dump)
        {
            int temp_array[3] = {getBits(0,0,W.valE),getBits(1,1,W.valE),getBits(2,2,W.valE)};
            if (temp_array[0])
            {
                dumpProgramRegisters();
            }
            if (temp_array[1])
            {
                dumpProcessorRegisters();
            }
            if (temp_array[2])
            {
                dumpMemory();
            }
        }
        return false;
    }
}
