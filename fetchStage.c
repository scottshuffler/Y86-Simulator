#include <stdbool.h>
#include "fetchStage.h"
#include "tools.h"
#include "instructions.h"
#include "decodeStage.h"
#include "memory.h"
#include "forwarding.h"
#include "bubbling.h"
//F register holds the input for the fetch stage.
//It is only accessible from this file. (static)
static fregister F;
static bool memError;// = false;
static int valC;
static int valP;

bool instr_valid(int icode);
bool d_bubble(bubbleType bubble);
bool d_stall(bubbleType bubble);
bool f_stall(bubbleType bubble);



// Function: getFregister
// Description: Returns a copy of the F REGISTER
// Params: none
// Returns: fregister
// Modifies: none
fregister getFregister()
{
    return F;
}

// Function: clearFregister
// Description: Returns a copy of the F register
// Params: none
// Returns: none
// Modifies: F
void clearFregister()
{
    clearBuffer((char *) &F, sizeof(F));
}

// Function: predictPC
// Description: Returns the approriate F.predPC
// Params: icode
// Returns: predPC
// Modifies: Adds to predPC
unsigned int predictPC(int icode, int predPC)
{
    if (icode == dump || icode == jxx || icode == call)
    {
        predPC = predPC + 5;
    }
    else if (icode == irmovl || icode == mrmovl || icode == rmmovl)
    {
        predPC = predPC +6;
    }
    else if (icode == OPL || icode == rrmovl || icode == popl || icode == pushl)
    {
        predPC = predPC +2;
    }
    else
    {
        predPC = predPC +1;
    }
	return predPC;
}

// Function: f_icode
// Description: Returns the approriate icode for the instruction
// Params: instruction
// Returns: icode or none
// Modifies: None
int f_icode(int instruction)
{
    if (memError)
    {
        return none;
    }
    else
    {
        return getBits(4,7,instruction);
    }
}

// Function: f_stat
// Description: Returns the approriate status for the instruction
// Params: icode
// Returns: status code
// Modifies: None
int f_stat(int icode)
{
    if (memError)
    {
        return ADR;
    }
    if(!instr_valid(icode))
    {
        return INS;
    }
    if (icode == halt)
    {
        return HLT;
    }
    else
    {
        return AOK;
    }
}

// Function: need_valC
// Description: Returns wether or not a valC is needed for an icode
// Params: icode
// Returns: bool
// Modifies: None
bool need_valC(int icode)
{
    if (icode == irmovl || icode == rmmovl || icode == mrmovl || icode == jxx || icode == call || icode == dump)
    {
        return true;
    }
    return false;
}

// Function: instr_valid
// Description: Returns wether or not the icode is valid
// Params: icode
// Returns: bool
// Modifies: None
bool instr_valid(int icode)
{
   // if (icode >= 0 && icode <= 0xC)
    if(icode == nop || icode == halt || icode == rrmovl || icode == irmovl || icode == rmmovl || icode == mrmovl || icode == OPL || icode == jxx || icode == call || icode == ret || icode == pushl || icode == popl || icode == dump)
    {
        return true;
    }
    return false;
}

// Function: updateFregister
// Description: Updates predPC
// Params: predPC
// Returns: None
// Modifies: predPC
void updateFregister(int predPC)
{
    F.predPC = predPC;
}

// Function: selectPC
// Description: Returns the predPC
// Params: predPC
// Returns: predPC
// Modifies: None
unsigned int selectPC(int predPC, forwardType forward)
{
    if(forward.M_icode == jxx && !forward.M_Cnd)
    {
        return forward.M_valA;
    }
    else if(forward.W_icode == ret)
    {
        return forward.W_valM;
    }
    else
    {
        return predPC;
    }
}

// Function: need_regids
// Description: Returns wether or not a register is needed for an icode
// Params: icode
// Returns: bool
// Modifies: None
bool need_regids(int icode)
{
    if (icode == irmovl || icode == pushl || icode == popl || icode == mrmovl || icode == rrmovl || icode == rmmovl || icode == OPL)
    {
        return true;
    }
    return false;
}

// Function: fetchStage
// Description: fetchStage for the y86 pipe
// Params: None
// Returns: None
// Modifies: None
void fetchStage(forwardType forward, bubbleType bubble)
{
    unsigned int f_pc = selectPC(F.predPC, forward);
    int instruction = getByte(f_pc,&memError);
    int ifun = getBits(0,3,instruction);
    if (memError)
    {
        ifun = none;
    }
    int icode = f_icode(instruction);
    int stat = f_stat(icode);
	int rA = none;
    int rB = none;
	if (need_regids(icode))
    {
        /*rA = getBits(4,7,f_pc);
        rB = getBits(0,3,f_pc);*/
		unsigned char regByte = getByte(f_pc+1, &memError);
        if (icode == OPL || icode == mrmovl || icode == rrmovl || icode == rmmovl || icode == pushl || icode == popl) {
            rA = getBits(4, 7, regByte);
            rB = getBits(0, 3, regByte);
        }
        if (icode == irmovl) {
            rA = 15;
            rB = getBits(0, 3, regByte);
        }
    }
    valC = 0;
    if (need_valC(icode))
    {
		int addr_pc = f_pc;
		if (icode == irmovl || icode == mrmovl || icode == rmmovl)
		{
			addr_pc++;
		}
        int byte1 = getByte(addr_pc+1,&memError);
        int byte2 = getByte(addr_pc+2,&memError);
        int byte3 = getByte(addr_pc+3,&memError);
        int byte4 = getByte(addr_pc+4,&memError);
        valC = buildWord(byte1, byte2, byte3, byte4);
    }
    //valP = predictPC(icode);
	valP = predictPC(icode, f_pc);
    if(!f_stall(bubble))
    {
	    F.predPC = predictFPC(icode);
    }
    if(d_bubble(bubble))
    {
        updateDregister(AOK, nop, 0, none, none, 0, 0);
    }
    else if(!d_stall(bubble))
    {
        updateDregister(stat, icode, ifun, rA, rB, valC, valP);
    }
    //updateFregister(valP);
    //F.predPC = valP;
	//valP = predictPC(icode);
}

// Function: predictFPC
// Description: 
// Params: icode
// Returns: valC or valP
// Modifies: None
int predictFPC (int icode)
{
	if(icode == jxx || icode == call)
    {
        return valC; 
    }
    else
    {
        return valP;
    }
}

// Function: f_stall
// Description: Determines if the fetch stage should be stalled
// Params: bubbleType bubble
// Returns: true or false
// Modifies: None
bool f_stall(bubbleType bubble)
{
    if (bubble.E_icode == mrmovl || bubble.E_icode == popl)
    {
        if (bubble.E_dstM == bubble.d_srcA || bubble.E_dstM == bubble.d_srcB 
            || bubble.D_icode == ret || bubble.E_icode == ret || bubble.M_icode == ret)
        {
            return true;
        }
    }
    return false;
}

// Function: d_stall
// Description: Determines if the decode stage should be stalled
// Params: bubbleType bubble
// Returns: true or false
// Modifies: None
bool d_stall(bubbleType bubble)
{
    if (bubble.E_icode == mrmovl || bubble.E_icode == popl)
    {
        if (bubble.E_dstM == bubble.d_srcA || bubble.E_dstM == bubble.d_srcB)
        {
            return true;
        }
    }
    return false;

}

// Function: d_bubble
// Description: Determines if the decode stage should be bubbled
// Params: bubbleType bubble
// Returns: true or false
// Modifies: None
bool d_bubble(bubbleType bubble)
{
    if(bubble.E_icode == jxx && !bubble.e_Cnd)
    {
        return true;
    }
    if (!((bubble.E_icode == mrmovl || bubble.E_icode == popl) && (bubble.E_dstM == bubble.d_srcA || bubble.E_dstM == bubble.d_srcB))
            && (bubble.D_icode == ret || bubble.E_icode == ret || bubble.M_icode == ret))
    {
        return true;
    }
    return false;
}
