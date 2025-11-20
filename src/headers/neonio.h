#ifndef NEONIO_H
#define NEONIO_H

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

#ifdef DEBUG
    #include <stdio.h>
    #define debug(...)          printf(__VA_ARGS__)
#else
    #define debug(...)
#endif

void removeZeros(char* string);
bool is_integer(char* string);
intptr_t str_to_int(char* string);
char* int_to_str(intptr_t number);
char* double_to_str(double number);
double str_to_double(char *string);
char* input(char*);
void printInt(intptr_t n);
void printDouble(double n);
void flush(void);
void printString(char* s);
void newLine(void);
void setColor(uint8_t color);
void printBack(void);
void clearConsole(void);
bool launcher(char* filename);
char* openFile(char* filename);
void writeFile(char* filename, char* content);
void neon_pause(char* text);

#ifdef TI_EZ80
char* argsAns(void);
#endif

#endif