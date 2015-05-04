#include <stdbool.h>
#include "executeStage.h"
#include "tools.h"
#include "instructions.h"
#include "memoryStage.h"
#include "registers.h"
#include "writebackStage.h"
#include "forwarding.h"
#include "status.h"
#include "bubbling.h"
//E register holds the input for the execute stage.
//It is only accessible from this file. (static)
static eregister E;
static int Cnd = 0;
static statusType *status;
int (*array[16])();

bool set_cc(statusType *status);
bool M_bubble(statusType *status);


// Function: getEregister
// Description: Returns a copy of the E register
// Params: none
// Returns: eregister
// Modifies: none
eregister getEregister()
{
    return E;
}

// Function: clearEregister
// Description: Returns a copy of the E register
// Params: none
// Returns: none
// Modifies: E
void clearEregister()
{
  clearBuffer((char *) &E, sizeof(E));
  E.stat = AOK;
  E.icode = nop;
}

// Function: updateEregister
// Description: Sets all values for E register
// Params: stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB
// Returns: none
// Modifies: stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB
void updateEregister(unsigned int stat, unsigned int icode, unsigned int ifun,unsigned int valC, 
    unsigned int valA, unsigned int valB, unsigned int dstE, unsigned int dstM,
    unsigned int srcA, unsigned int srcB)
{
    E.stat = stat;
    E.icode = icode;
    E.ifun = ifun;
    E.valC = valC;
    E.valA = valA;
    E.valB = valB;
    E.dstE = dstE;
    E.dstM = dstM;
    E.srcA = srcA;
    E.srcB = srcB;
}

// Function: execute_unused
// Description: Unused opcode
// Params: None
// Returns: 0
// Modifies: None
int execute_unused()
{
    return 0;
}

// Function: execute_dump
// Description: dump opcode
// Params: None
// Returns: E.valC
// Modifies: None
int execute_dump()
{
    return E.valC;
}

// Function: executeStage
// Description: Simulates the executeStage in the y86 pipe
// Params: None
// Returns: None
// Modifies: M register
void executeStage(forwardType *forward, statusType *stat, bubbleType *bubble)
{
    //forward->e_dstE = E.dstE;
    //(*forward).e_valE = E.valE;
    status = stat;
    int valE = (*array[E.icode])();
    forward->e_valE = valE;

    Cnd = 0;
    if(E.icode != rrmovl && E.icode != jxx)
    {
        Cnd = 0;
    }
    else if (E.ifun == 0)
    {
        Cnd = 1;
    }
    else if(E.ifun == 1 && ((getCC(SF) ^ getCC(OF)) || getCC(ZF)))
    {
        Cnd = 1;
    }
    else if(E.ifun == 2 && (getCC(SF) ^ getCC(OF)))
    {
        Cnd = 1;
    }
    else if(E.ifun == 3 && getCC(ZF))
    {
        Cnd = 1;
    }
    else if(E.ifun == 4 && !getCC(ZF))
    {
        Cnd = 1;
    }
    else if(E.ifun == 5 && !(getCC(SF) ^ getCC(OF)))
    {
         Cnd = 1;
    }
    else if(E.ifun == 6 && (!(getCC(SF) ^ getCC(OF)) && !getCC(ZF)))
    {
        Cnd =  1;
    }
    if(E.icode == rrmovl && Cnd == 0)
    {
        E.dstE = none;
    }
    forward->e_dstE = E.dstE;
    bubble->E_dstM = E.dstM;
    bubble->e_Cnd = Cnd;
    bubble->E_icode = E.icode;
    if(M_bubble(status))
    {
        updateMregister(AOK, nop, 0, 0, 0, none, none);
    }
    else
    {
        updateMregister(E.stat, E.icode, Cnd, valE, E.valA, E.dstE, E.dstM);
    }
}

// Function: initializeFuncPtrArray
// Description: Creates the array of functions
// Params: None
// Returns: None
// Modifies: array
void initializeFuncPtrArray()
{
    int i = 0;
    for (i; i < 16; i ++)
    {
        if (i == dump)
        {
            array[i] = &execute_dump;
        }
        else if (i == irmovl)
        {
            array[i] = &performIrmovl;
        }
        else if (i == OPL)
        {
            array[i] = &performOpl;
        }
        else if (i == mrmovl)
        {
            array[i] = &performMrmovl;
        }
        else if (i == rmmovl)
        {
            array[i] = &performRmmovl;
        }
        else if (i == rrmovl)
        {
            array[i] = &performRrmovl;
        }
        else if (i == popl)
        {
            array[i] = &performPopl;
        }
        else if (i == pushl)
        {
            array[i] = &performPushl;
        }
		else if (i == call)
        {
            array[i] = &performCall;
        }
		else if (i == jxx)
        {
            array[i] = &performJxx;
        }
		else if (i == ret)
        {
            array[i] = &performRet;
        }
        else
        {
            array[i] = &execute_unused;
        }
    }
}

// Function: aluA
// Description: Simulates the ALUA logic
// Params: icode
// Returns: E.valA or E.valC
// Modifies: None
int aluA(int icode)
{
    if(icode == OPL || icode == rrmovl)
    {
        return E.valA;
    }
    else if (icode == irmovl || icode == mrmovl || icode == rmmovl)
    {
        return E.valC;
    }
    else if (icode == call || icode == pushl)
    {
        return -4;
    }
    else if (icode == ret || icode == popl)
    {
        return 4;
    }
}

// Function: aluB
// Description: Simulates the ALUB logic
// Params: icode
// Returns: E.valB or 0
// Modifies: None
int aluB(int icode)
{
    if (icode == OPL || icode == rmmovl || icode == mrmovl || icode == call || icode == pushl || icode == popl || icode == ret)
    {
        return E.valB;
    }
    if (icode == rrmovl || icode == irmovl)
    {
        return 0;
    }
}

// Function: performIrmovl
// Description: Simulates the performIrmovl logic
// Params: None
// Returns: E.valC
// Modifies: None
int performIrmovl()
{
    return E.valC;
}

// Function: performRrmovl
// Description: Simulates the performRrmovl logic
// Params: None
// Returns: E.valA
// Modifies: None
int performRrmovl()
{
    return E.valA;
}

// Function: performMrmovl
// Description: Simulates the performMrmovl logic
// Params: None
// Returns: E.valB + E.valC
// Modifies: None
int performMrmovl()
{
    return E.valB + E.valC;
}

// Function: performRmmovl
// Description: Simulates the performRmmovl logic
// Params: None
// Returns: E.valB + E.valc
// Modifies: None
int performRmmovl()
{
    return E.valB + E.valC;
}
// Function: performOpl
// Description: Simulates the performOpl logic
// Params: None
// Returns: tempValue
// Modifies: Condition codes
int performOpl()
{
    int ifunVal = aluFun(E.icode);
    int tempValue = 0;
    if (ifunVal == 0)
    {
        tempValue = E.valA + E.valB;
    }
    else if (ifunVal == 1)
    {
        tempValue = aluB(E.icode) - aluA(E.icode);
    }
    else if (ifunVal == 2)
    {
        tempValue = aluA(E.icode) & aluB(E.icode);
    }
    else if (ifunVal == 3)
    {
        tempValue = aluA(E.icode) ^ aluB(E.icode);
    }
    if(set_cc(status))
    {
        setCond(tempValue);
    }
    return tempValue;
}

// Function: aluFun
// Description: Simulates the aluFun logic
// Params: icode
// Returns: E.ifun
// Modifies: None
int aluFun(int icode)
{
    if (icode == OPL)
    {
        return E.ifun;
    }
    //return 0;
}

// Function: setCond
// Description: Simulates the setCond logic
// Params: tempVal
// Returns: None
// Modifies: Condition codes
void setCond(int tempVal)
{
    if (tempVal == 0)
    {
        setCC(ZF,1);
    }
    else
    {
        setCC(ZF,0);
    }
    if (getBits(31,31,tempVal))
    {
        setCC(SF,1);
    }
    else
    {
        setCC(SF,0);
    }
    if ((((aluA(E.icode) > 0 && aluB(E.icode) > 0 && tempVal < 0)
            || (aluA(E.icode) < 0 && aluB(E.icode) < 0 && tempVal >= 0))
            && (aluFun(E.icode) == 0 || aluFun(E.icode) == 2)) ||
            (((aluA(E.icode) > 0 && aluB(E.icode) < 0 && tempVal >= 0)
            || (aluA(E.icode) < 0 && aluB(E.icode) > 0 && tempVal < 0))
            && (aluFun(E.icode) == 1)))
    {
        setCC(OF,1);
    }
    else
    {
        setCC(OF,0);
    }
}

// Function: performPushl
// Description: Simulates a push operation
// Params: None
// Returns: E.valB - 4
// Modifies: None
int performPushl()
{
    return E.valB - 4;
}

// Function performPopl
// Description: Simulates a pop operation
// Params: None
// Returns: E.valB + 4
// Modifies: None
int performPopl()
{
    return E.valB + 4;
}

// Function performCall
// Description: Simulates the call operation
// Params: None
// Returns: E.valB - 4
// Modifies: None
int performCall()
{
    return E.valB - 4;
}

// Function performRet
// Description: Simulates the return operation
// Params: None
// Returns: E.valB + 4
// Modifies: None
int performRet()
{
    return E.valB + 4;
}

// Function: performJxx
// Description: Simulates the conditional jump operations
// Params: None
// Returns: E.valA
// Modifies: None
int performJxx()
{
    return E.valA;
}

// Function M_bubble
// Description: Determines if the memoryStage needs to be bubbled
// Params: statusType *status
// Returns: true or false
// Modifies: None
bool M_bubble(statusType *status)
{
    if(status->M_stat == ADR || status->M_stat == INS || status->M_stat == HLT)
    {
        return true;
    }
    if(status->W_stat == ADR || status->W_stat == INS || status->W_stat == HLT)
    {
        return true;
    }
    return false;
}

// Function set_cc
// Description: Determines if condition codes need to be set
// Params: statusType * status
// Returns: true or false
// Modifies: None
bool set_cc(statusType *status)
{
    if (E.icode == OPL)
    {
        if(status->M_stat != ADR && status->M_stat != INS && status->M_stat != HLT)
        {
            if(status->W_stat != ADR && status->W_stat != INS && status->W_stat != HLT)
            {
                return true;
            }
        }
    }
    return false;
}
