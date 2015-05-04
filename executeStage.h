#ifndef EXECUTESTAGE_H
#define EXECUTESTAGE_H
typedef struct
{
    unsigned int stat;
    unsigned int icode;
    unsigned int ifun;
    unsigned int valC;
    unsigned int valA;
    unsigned int valB;
    unsigned int dstE;
    unsigned int dstM;
    unsigned int srcA;
    unsigned int srcB;
} eregister;

//prototypes for functions called from files other than executeStage
eregister getEregister();
void clearEregister();

void initializeFuncPtrArray();
int aluA(int icode);
int aluB(int icode);
int performIrmovl();
int performOpl();
int performRrmovl();
int performCmovxx();
int performMrmovl();
int performRmmovl();
int performPushl();
int performPopl();
int performCall();
int performJxx();
int performRet();

int aluFun(int icode);
void setCond(int tempVal);
#endif


