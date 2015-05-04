#include <stdbool.h>
#include "memoryStage.h"
#include "tools.h"
#include "writebackStage.h"
#include "forwarding.h"
#include "instructions.h"
#include "status.h"
#include "bubbling.h"
//M register holds the input for the memory stage.
//It is only accessible from this file. (static)
static mregister M;
static bool memerror = false;
static int valM = 0;
bool w_stall(statusType status);
// Function: getMregister
// Description: Returns a copy of the M register
// Params: None
// Returns: mregister
// Modifies: None
mregister getMregister()
{
    return M;
}

// Function: clearMregister
// Description: Returns a copy of the M register
// Params: None
// Returns: None
// Modifies: M
void clearMregister()
{
  clearBuffer((char *) &M, sizeof(M));
  M.stat = AOK;
  M.icode = nop;
}

// Function: updateMregister
// Description: Updates the M register with passed in values
// Params: stat, icode, Cnd, valE, valA, dstE, dstM
// Returns: None
// Modifies: stat, icode, Cnd, valE, valA, dstE, dstM
void updateMregister(int stat, int icode, int Cnd, int valE, int valA, int dstE, int dstM)
{
    M.stat = stat;
    M.icode = icode;
    M.Cnd = Cnd;
    M.valE = valE;
    M.valA = valA;
    M.dstE = dstE;
    M.dstM = dstM;
}

// Function: memoryStage
// Description: Simulates the memorystage of the y86 pipe
// Params: None
// Returns: None
// Modifies: W register
void memoryStage(forwardType *forward, statusType *status, bubbleType *bubble)
{
    if (memWrite(M.icode))
    {
        putWord(mem_addr(M.icode), M.valA, &memerror);
    }
    //valM = 0;
    if (memRead(M.icode))
    {
        int addr = mem_addr(M.icode);
        /*int byte1 = getByte(addr+0,&memerror);
        int byte2 = getByte(addr+1,&memerror);
        int byte3 = getByte(addr+2,&memerror);
        int byte4 = getByte(addr+3,&memerror);
        valM = buildWord(byte1, byte2, byte3, byte4);*/
        valM = getWord(addr, &memerror);
    }
    if (memerror)
    {
        M.stat = ADR;
    }
    forward->M_Cnd = M.Cnd;
    forward->M_valA = M.valA;
    forward->M_icode = M.icode;
    forward->M_dstE = M.dstE;
    forward->M_valE = M.valE;
    forward->M_dstM = M.dstM;
    forward->m_valM = valM;
    status->M_stat = M.stat;
    bubble->M_icode = M.icode;
    if (!w_stall(*status))
    {
        updateWregister(M.stat, M.icode, M.valE, valM, M.dstE, M.dstM);
    }
}

// Function: memWrite
// Description: Determines if it will write to memory
// Params: icode
// Returns: true or false
// Modifies: None
bool memWrite (int icode)
{
    //needs pushl and calll
    if (icode == rmmovl || icode == pushl || icode == call)
    {
        return true;
    }
    return false;
}

// Function: memRead
// Description: Determines if it will read to memory
// Params: icode
// Returns: true or false
// Modifies: None
bool memRead(int icode)
{
    //pop and return as well
    if (icode == mrmovl || icode == ret || icode == popl)
    {
        return true;
    }
    return false;
}

// Function: mem_addr
// Description: Returns the address if needed
// Params: icode
// Returns: address
// Modifies: None
int mem_addr(int icode)
{
    if (icode == rmmovl || icode == mrmovl || icode == pushl || icode == call)
    {
        return M.valE;
    }
    else if(icode == popl || icode ==  ret)
    {
        return M.valA;
    }
}

// Function: w_stall
// Description: Determines if the writeback stage should be stalled
// Params: statusType status
// Returns: true or false
// Modifies: None
bool w_stall(statusType status)
{
    if (status.W_stat == ADR || status.W_stat == INS || status.W_stat == HLT)
    {
        return true;
    }
    return false;
}
