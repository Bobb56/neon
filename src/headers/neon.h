#ifndef NEON_H
#define NEON_H

#include "objects.h"


/*MAIN.C*/
void printRes(NeObj res);
void execFile(char* filename);
void importFile(char* filename);
void startMessage(void);
void terminal (void);
void neonInit(void);
void neonExit(void);


#endif