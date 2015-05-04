#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "loader.h"
#include "memory.h"

static bool memError = false;


/*  load
 * Checks if a file is a valid file type and if it is checks the contents
 * to be sure its in the correct format
 * @param argc the count of arguments passed
 * @param argv is the array of arguments passed
 * @return fetch returns a value to signify pass or fail 
 * */
unsigned int load (int argc, char * argv[])
{
    if (argc != 2 )
    {
    	printf("Usage: yess filename.yo\n");
    	return 0;
    }
    if(!validFileName(argv[1]))
    {
	  	printf("Usage: yess filename.yo\n");
        return 0;
    }
    FILE * fp = fopen(argv[1],"r");
    char buffer[150];
    int line = 1;
    int lastAddress = 0;
    bool flag = true;
    while (fgets(buffer, sizeof(buffer), fp)!= NULL )
    {
        if (!checkLine(buffer))
        {
            printf("Error on line %d\n", line);
            printf("%s\n",buffer);
            fclose(fp);
            return 0;
        }
        else if (!isSpaces(0, 21, buffer))
        {
            int address = grabAddress(buffer);
			
			if (!flag)
                {
                    printf("Error on line %d\n", line);
                    printf(buffer);
                    printf("\n");
                    fclose(fp);
                    return 0;
                }
			
            if(address < lastAddress)
            {
                printf("Error on line %d\n", line);
                printf(buffer);
                fclose(fp);
                return 0;
            }
            int current  = 9;
            int bytes = 0;
            while(buffer[current] != ' ')
            {
                bytes++;
                current++;
            }
            bytes = bytes / 2;
            if (bytes > 0)
            {
                int currentByte = 0;
                int index = 9;
                unsigned char byte;
                while(currentByte < bytes)
                {
                    byte = grabDataByte(buffer, index);
                    putByte(address, byte, &memError);
                    if(memError)
                    {
                        fclose(fp);
                        return 0;
                    }
                    index = index + 2;
                    currentByte++;
                    address++;
                }
            }
            lastAddress = address;
	    flag = checkBytes(buffer);
        }
        line++;
    }
    fclose(fp);
    return 1;
}

/* validFileName
 * checks a file to see if the extension is correct
 * and that it does in fact exist
 * @param fileName char pointer that holds the file name
 * @return true or false depending on success or failure 
 * */
unsigned int validFileName(char * filename)
{
    char *dot = strrchr(filename, '.');
	if (dot && !strcmp(dot, ".yo"))
	{
		return true;
	}
	return false;
}

/* checkLine
 * checks the line to make sure it's in the correct format
 * *str holds the line to be examined
 * @return true or false depending on success or failure
 * */
bool checkLine(char * str)
{
    if((isSpaces(0, 21, str))) 
    {
    	return true;
    }
    else if((!isSpaces(0, 1, str)) ||
            (!isAddress(str)) ||
            (!isData(str)) ||
            (!isSpaces(8,8,str)) ||
            (str[22] != '|'))
    {
        return false;
    }
    return true;
}

/* grabAddress
 * Grabs the current lines address
 * *str holds the line to be examined
 * @return returns the address as an unsigned int
 * */
unsigned int grabAddress(char * str)
{
    char array[3];
    array[0] = str[4];
    array[1] = str[5];
    array[2] = str[6];
    return (unsigned int)strtol(array, NULL, 16);
}

/* isAddress
 * checks to  see if the address is in a valid format
 * *str holds the line to be examined
 * @return true or false depending on success or failure
 * */
bool isAddress(char * str)
{
    if ((str[2] != '0') ||
            (str[3] != 'x')||
            (!checkHex(4, 6, str)) ||
            (str[7] != ':'))
    {
        return false;
    }
    return true;
}

/* isData
 * checks a file to see if there are an even number of hex values after the address
 * @param *str holds the line to be examined
 * @return true or false depending on success or failure 
 * */
bool isData(char * str)
{
    int current = 9;
    while(str[current] != ' ')
    {
        if (!checkHex(current, current, str))
        {
            return false;
        }
        current++;
    }
    if(!isSpaces(current, 21, str) || current % 2 == 0)
    {
        return false;
    }
    return true;
}

/* isSpaces
 * checks to see if the range between start and stop are spaces
 * @param start holds the value to start at
 * @param stop holds the value to stop at
 * @param *str holds the line to be examined
 * @return true or false depending on success or failure
 * */
bool isSpaces(int start, int stop, char * str)
{
    int i;
    for (i = start; i <= stop; i++)
    {
        if (str[i] != ' ')
        {
            return false;
        }
    }
    return true;
}

/* checkHex
 * checks to see if the range between start and stop are hex values
 * @param start holds the value to start at
 * @param stop holds the value to stop at
 * @param *str holds the line to be examined
 * @return true or false depending on success or failure
 * */
bool checkHex(int start, int stop, char * str)
{
    int i;
    for (i = start; i<= stop; i++)
    {
        if (!isdigit(str[i]) && (str[i] > 'f' || str[i] < 'a'))
        {
            return false;
        }
    }
    return true;
}
/* isSpaces
 * returns the byte starting at that address
 * @param start holds the value to start at
 * @param *str holds the line to be examined
 * @return returns the byte of datae
 * */
unsigned char grabDataByte(char * str, int start)
{
    char byte[3] = {str[start], str[start+1], '\0'};
    return (unsigned char)strtol(byte, NULL, 16);
}

/* checkBytes
 * returns true or false depending if the line has the correct number of bytes
 * @param buffer holds the value of the line you're checking
 * @return true or false depending if the number of bytes are correct 
 * */
bool checkBytes(char *buffer)
{
    if((checkHex(9, 10,buffer) && isSpaces(17, 21,buffer)) || isSpaces(9, 21,buffer))
    {
        return true;
    }
    if((buffer[9] == '0' || buffer[9] == '1' || buffer[9] == '9') && (checkHex(9, 10,buffer) && isSpaces(11, 21,buffer)))
    {
        return true;
    }
    if((buffer[9] == '2' || buffer[9] == '6' || buffer[9] == 'a'|| buffer[9] == 'b') && (checkHex(9, 12,buffer) && isSpaces(13, 21,buffer)))
    {
        return true;
    }
    if((buffer[9] == '7' || buffer[9] == '8' || buffer[9] == 'c') && (checkHex(9, 18,buffer) && isSpaces(19, 21,buffer)))
    {
        return true;
    }
    if((buffer[9] == '3' || buffer[9] == '4' || buffer[9] == '5') && (checkHex(9, 20,buffer) && isSpaces(21, 21,buffer)))
    {
        return true;
    }
    return false;

}
