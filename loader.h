#ifndef LOADER_H
#define LOADER_H
unsigned int load (int argc, char * argv[]);
unsigned int validFileName(char * filename);
bool checkLine( char * str);
unsigned int grabAddress(char * str);
bool isAddress(char * str);
bool isData(char * str);
bool isSpaces(int start, int stop, char * str);
bool checkHex(int start, int stop, char * str);
unsigned char grabDataByte(char * str, int start);
bool checkBytes(char *line);
#endif
