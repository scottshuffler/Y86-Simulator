#include <stdio.h>
#include <stdbool.h>
#include "tools.h"

/* 
   Replace the return 0 with the code to perform the required function.
   Do not change the function prototypes.
   Be sure to check for errors in input. 
   Document EACH function with a function header (description, parameters,
   return).
*/

/*
 *Takes an integer and gives the bits between a low and high point.
 *@param low lower bound
 *@param high higher bound
 *@param source integer input
 *@return y bits between bounds
 * */
unsigned int getBits(int low, int high, unsigned int source)
{
    if (low >= 0 && high >= 0 && low <= 31 && low <= 31 && low <= high)
    {
    unsigned int buffer = 31 - high;
    unsigned int x = source << buffer;
    buffer = buffer + low;
    unsigned y = x >> buffer;
    return y;
    }
    return 0;
    
} 

/*
 *Takes an integer and sets the bits between a low and high point to 1
 *@param low lower bound
 *@param high upper bound
 *@param source integer input
 *@return source modified integer
 * */
unsigned int setBits(int low, int high, unsigned int source)
{
    unsigned int test = getBits(low,high,-1);
    test = test << low;
    return source = source | test;
    }

/*
 *Takes an integer and sets the bits between a low and high point to 0
 *@param low lower bound
 *@param high upper bound
 *@param source integer input
 * @return source modified integer
 * */
unsigned int clearBits(int low, int high, unsigned int source)
{
    unsigned int test = getBits(low,high,-1);
    test = test << low;
    return source = ~test & source;
}

/*
 *Sets a given bit in a number to a providede bit value
 *@param bit number position of bit to edit
 *@param bitValue value to set
 *@param source integer input
 *@return retVal modified integer
 * */
unsigned int assignOneBit(int bitNumber, int bitValue, unsigned int source)
{
    unsigned int retVal = source;
    if (bitValue == 1)
    {
        retVal = setBits(bitNumber, bitNumber, source);
    }
    else if (bitValue == 0)
    {
        retVal = clearBits(bitNumber, bitNumber, source);
    }
    return retVal;
}

/*
 *Takes an int and a value(0-3) and returns the byte that coincides with that
 *@param byteNo Number of the byte
 *@param source integer to extract byte from
 *@return specified byte
 * */
unsigned char getByteNumber(int byteNo, unsigned int source)
{
    if (byteNo < 0 || byteNo > 3)
    {
        return 0;
    }
    return getBits(byteNo * 8, ((byteNo + 1) * 8) -1, source);
   
}

/*
 *Takes an int, a value, and a byte number, then replaces the specified byte with the value
 *@param byteNo position of byte to be replaced
 *@param byteValue value to give specified byte
 *@param source integer to be modified
 *@return modified int
 * */
unsigned int putByteNumber(int byteNo, unsigned char byteValue, 
                           unsigned int source)
{
    if (byteNo < 0 || byteNo > 3)
    {
        return source;
    }
    source = clearBits(byteNo * 8, ((byteNo + 1) * 8) -1, source);
    unsigned int thing = byteValue << byteNo * 8;
    return source | thing;

    /*for (int i = 0; i < 8; i++)
    {
        source = assignOneBit(8*byteNo+i,byteValue%2,source);
        byteValue = byteValue >> 1;
    }
    return source;*/
}

/*
 *Takes 4 bytes and builds a word from them
 *@param byte0 first byte
 *@param byte1 second byte
 *@param byte2 third byte
 *@param byte3 fourth byte
 *@return answer all four bytes in order
 * */
unsigned int buildWord(unsigned char byte0, unsigned char byte1, 
                       unsigned char byte2, unsigned char byte3)
{
    unsigned int answer = 0;
    answer = putByteNumber(0, byte0, answer);
    answer = putByteNumber(1, byte1, answer);
    answer = putByteNumber(2, byte2, answer);
    answer = putByteNumber(3, byte3, answer);
    return answer;
}

/*
 *Checks to see if a number is negative
 *@param source number to be checked
 *@return true or false
 * */
bool isNegative(unsigned int source)
{
    return source >> 31;
}

/*
 *Breaks a number in to 4, 8 bit binary digits
 *@param source number to be broken up
 *@param bits[] array to contain bit representation
 * */
void expandBits(unsigned int source, char bits[36])
{
    int x;
    for (x = 35; x >= 0; x--)
    {
        if(x == 35)
            {
                bits[x]='\0';
            }
        else if (x == 8 || x == 17 || x == 26)
            {
                bits[x]=' ';
            }
        else if (source % 2 == 1)
            {
                bits[x] ='1';
                source = source >> 1;
            }
        else
            {
                bits[x] ='0';
                source = source >> 1;
            }
    }
}

/*
 *Sets each byte that the buffer points to, to 0
 *@param buff buffer to be cleared
 *@param size size of buffer
 * */
void clearBuffer(char * buff, int size)
{
    int x;
    for (x = 0; x < size; x++)
        buff[x]=0;
}

