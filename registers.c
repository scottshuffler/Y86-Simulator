/* names: Devin Sink and Scott Shuffler
 * userids: sinkdb and shuffleres
 * description: routines for simulating the registers in the Y86 
 * */

#include <stdio.h>
#include <stdbool.h>
#include "tools.h"
#include "registers.h"

static unsigned int CC;
static unsigned int registers[REGSIZE];

/* getRegister
 * Retreives the value stored within a specifric register
 * @param regNum Number of register to be accessed
 * @return registers[getNum] value contained in register
 * */
unsigned int getRegister(int regNum)
{
    if (regNum >= 0 && regNum < REGSIZE)
    {
        return registers[regNum];
    }
    else
    {
        return 0;
    }
}

/* setResgister
 * assigns a value to a specific register
 * @param regNum register to be manipulated
 * @param regValue value to be placed in register
 * */
void setRegister(int regNum, unsigned int regValue)
{
    if (regNum >= 0 && regNum < REGSIZE)
    {
        registers[regNum] = regValue;
    }

}

/* clearRegister
 * goes through register array and clears all values
 * */
void clearRegisters()
{
    int i;
    for(i = 0; i < REGSIZE; i++)
    {
        registers[i] = 0;
    }

}

/* setCC
 * sets condition codes
 * int bitNumber bit to be assigned
 * int value value to assign to above bit
 * */
void setCC(unsigned int bitNumber, unsigned int value)
{
    if (bitNumber == OF || bitNumber == ZF || bitNumber == SF)
    {
        CC = assignOneBit(bitNumber, value, CC);
    }
}

/* getCC
 * gets the value of the condition code on a certain bit
 * @param bitNumber bit to check condition code on
 * @return getBits value of that bit
 * */
unsigned int getCC(unsigned int bitNumber)
{
   if (bitNumber == OF || bitNumber == ZF || bitNumber == SF)
    {
        return getBits(bitNumber, bitNumber, CC);
    }
    else
    {
        return 0;
    }
}
