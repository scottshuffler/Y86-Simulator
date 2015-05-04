/* names: Devin Sink and Scott Shuffler 
 * userids: sinkdb and shufflers
 * descrption: routines that simulate the primary memory of the Y86
 * */
#include <stdio.h>
#include <stdbool.h>
#include "tools.h"
#include "memory.h"


static unsigned int memory[MEMSIZE];

/* fetch
 * gets the value at a specific address
 * @param address address to be accessed
 * @param memError boolean value for errors in memory
 * @return memory[Address] data at the address
 * */
int fetch(int address, bool * memError)
{
    if (address >= 0 && address <= MEMSIZE)
    {
        *memError = 0;
        return memory[address];
    }
    else
    {
        *memError = 1;
        return 0;
    }
}

/* store
 * stores given value at given address
 * @param address location for valule to be stored
 * @param value value to be stored at given address
 * @param memError boolean value for errors in memory
 * */
void store(int address, unsigned int value, bool * memError)
{
    if (address >= 0 && address <= MEMSIZE)
    {
        *memError = 0;
        memory[address] = value;
    }
    else
    {
        *memError = 1;
    }

}

/* getByte
 * returns the byte at given address
 * @param byteAddress address user wants to access
 * @param memError boolean value for errors in memory
 * @return getByteNumber 
 * */
unsigned char getByte(int byteAddress, bool * memError)
{
    unsigned int word = 0;
    if (byteAddress >= 0 && byteAddress <= (MEMSIZE << 2))
    {
        *memError = 0;
        word = fetch(byteAddress >> 2, memError);
        return getByteNumber(byteAddress % 4, word);
    }
    else
    {
        *memError = 1;
        return 0;
    }
}

/* putByte
 * stores a value in a byte address
 * @param byteAddress location for data to be stored
 * @param value value to be stored at given location
 * @param memError boolean value for memory errors
 * */
void putByte(int byteAddress, unsigned char value, bool * memError)
{
    unsigned int word = 0;
    if (byteAddress >= 0 && byteAddress <= (MEMSIZE << 2))
    {
        *memError = 0;
        word = fetch((byteAddress - byteAddress % 4) >> 2, memError);
        store((byteAddress - (byteAddress % 4)) >> 2, putByteNumber(byteAddress % 4, value, word) , memError);
    }
    else
    {
        *memError = 1;
    }

}

/*  getWord
 * returns word at given address
 * @param byteAddress address to be accessed
 * @param memError boolean value for memory errors
 * @return fetch data contained within byte address
 * */
unsigned int getWord(int byteAddress, bool * memError)
{
    if (byteAddress >= 0 && byteAddress <= MEMSIZE && (byteAddress % 4 == 0))
    {
        *memError = 0;
        return fetch(byteAddress >> 2, memError);
    }
    else
    {
        *memError = 1;
        return 0;
    }

}

/* putWord
 * stores word at given address
 * @param byteAddress address for data to be stored
 * @param value value to be stored at given location
 * @param memError boolnea vlue for memory errors
 * */
void putWord(int byteAddress, unsigned int value, bool * memError)
{
    if (byteAddress >= 0 && byteAddress <= MEMSIZE && (byteAddress % 4 == 0))
    {
        *memError = 0;
        store(byteAddress >> 2, value, memError);
    }
    else
    {
        *memError = 1;
    }
}

/* clearMemory
 * sets all contents of memory to be zero
 * */
void clearMemory()
{
    clearBuffer((char *)memory,MEMSIZE << 2);
}
