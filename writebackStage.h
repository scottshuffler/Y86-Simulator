#ifndef WRITEBACKSTAGE_H
#define WRITEBACKSTAGE_H
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int valE;
    unsigned int valM;
    unsigned int dstE;
    unsigned int dstM;
} wregister;

//prototypes for functions called from files other than writebackStage
wregister getWregister();
void clearWregister();
#endif


