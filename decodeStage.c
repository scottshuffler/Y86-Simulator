#include <stdbool.h>
#include "decodeStage.h"
#include "tools.h"
#include "executeStage.h"
#include "instructions.h"
#include "forwarding.h"
#include "registers.h"
#include "bubbling.h"
//D register holds the input for the decode stage.
//It is only accessible from this file. (static)
static dregister D;
bool e_bubble(bubbleType bubble);
// Function: getDregister
// Description: Returns a copy of the D register
// Params: none
// Returns: dregister
// Modifies: none
dregister getDregister()
{
    return D;
}

// Function: clearDregister
// Description: Returns a copy of the D register
// Params: none
// Returns: none
// Modifies: D
void clearDregister()
{
    clearBuffer((char *) &D, sizeof(D));
    D.stat = AOK;
    D.icode = nop;
}

// Function: updateDregister
// Description: Sets all D register params
// Params: stat, icode, ifun, rA, rB, valC, valP
// Returns: None
// Modifies: stat, icode, ifun, rA, rB, valC, valP
void updateDregister(unsigned int stat, unsigned int icode, unsigned int ifun, unsigned int rA, unsigned int rB, unsigned int valC, unsigned int valP)
{
    D.stat = stat;
    D.icode = icode;
    D.ifun = ifun;
    D.rA = rA;
    D.rB = rB;
    D.valC = valC;
    D.valP = valP;
}

// Function: decodeStage
// Description: decodeStage simutlator for y86 pipe
// Params: forward
// Returns: None
// Modifies: eRegister
void decodeStage(forwardType forward, bubbleType *bubble)
{
    int srcA = getSrcA(D.icode);
    int srcB = getSrcb(D.icode);
    int dstE = getDstE(D.icode);
    int dstM = getDstM(D.icode);
    int valA = selectFwdA(srcA, forward);
    int valB = FwdB(srcB, forward);
    bubble->d_srcA = srcA;
    bubble->d_srcB = srcB;
    bubble->D_icode = D.icode;
    if(e_bubble(*bubble))
    {
        updateEregister(AOK, nop, 0, 0, 0, 0, none, none, none, none);
    }
    else
    {
        updateEregister(D.stat, D.icode, D.ifun, D.valC, valA, valB, dstE, dstM, srcA, srcB);
    }
}

// Function: getSrcA
// Description: Gets the correct icode source
// Params: icode
// Returns: source or none
// Modifies: None
int getSrcA(int icode)
{
    if (icode == rrmovl || icode == rmmovl || icode == OPL || icode == pushl)
    {
        return D.rA;
    }
    if (icode == popl || icode == ret)
    {
        return ESP;
    }
    return none;
}

// Function: getSrcB
// Description: Gets the correct icode source
// Params: icode
// Returns: source or none
// Modifies: None
int getSrcb(int icode)
{
    if (icode == OPL || icode == rmmovl || icode == mrmovl)
    {
       return D.rB;
    }
    if (icode == pushl || icode == popl || icode == call || icode == ret)
    {
        return ESP;
    }
    return none;
}

// Function: getDstE
// Description: Gets the correct icode dest
// Params: icode
// Returns: dest or none
// Modifies: None
int getDstE(int icode)
{
    if (icode == rrmovl || icode == irmovl || icode == OPL)
    {
        return D.rB;
    }
    if (icode == pushl || icode == popl || icode == call || icode == ret)
    {
       return ESP;
    }
    return none;
}

// Function: getDstM
// Description: Gets the correct icode dest
// Params: icode
// Returns: dest or none
// Modifies: None
int getDstM(int icode)
{
    if (icode == mrmovl || icode == popl)
    {
        return D.rA;
    }
    return none;
}

// Function: selectFwdA
// Description: Gets the correct value for fwdA
// Params: icode, forward
// Returns: dest or none
// Modifies: None
int selectFwdA(int srcA,forwardType forward)
{
	if (D.icode == call || D.icode == jxx)
    {
        return D.valP;
    }
    else if (srcA == none)
    {
        return 0;
    }
    else if (srcA == forward.e_dstE)
    {
        return forward.e_valE;
    }
    else if (srcA == forward.M_dstM)
    {
        return forward.m_valM;
    }
    else if(srcA == forward.M_dstE)
    {
        return forward.M_valE;
    }
    else if (srcA == forward.W_dstM)
    {
        return forward.W_valM;
    }
    else if(srcA == forward.W_dstE)
    {
        return forward.W_valE;
    }
    else
    {
        return getRegister(srcA);
    }
}

// Function: FwdB
// Description: Gets the correct value for FwdB
// Params: icode, forward
// Returns: dest or none
// Modifies: None
int FwdB(int srcB, forwardType forward)
{
   // int srcB = getSrcb(icode);
    if (srcB == none)
    {
        return 0;
    }
    else if (srcB == forward.e_dstE)
    {
        return forward.e_valE;
    }
    else if (srcB == forward.M_dstM)
    {
        return forward.m_valM;
    }
    else if (srcB == forward.M_dstE)
    {
        return forward.M_valE;
    }
    else if (srcB == forward.W_dstM)
    {
        return forward.W_valM;
    }
    else if (srcB == forward.W_dstE)
    {
        return forward.W_valE;
    }
    else
    {
        return getRegister(srcB);
    }
}

// Function: e_bubble
// Description: Determines if the execute stage should be bubbled
// Params: bubble
// Returns: true or false
// Modifies: None
bool e_bubble(bubbleType bubble)
{
    if((bubble.E_icode == jxx && !bubble.e_Cnd) || ((bubble.E_icode == mrmovl || bubble.E_icode == popl) 
        && (bubble.E_dstM == bubble.d_srcA || bubble.E_dstM == bubble.d_srcB)))
    {
        return true;
    }
    return false;
}
