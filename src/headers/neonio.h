#ifndef NEONIO_H
#define NEONIO_H

#ifndef TI_EZ80
#include <stdio.h>
#endif

#include <stdint.h>
#include <stdbool.h>

#include "constants.h"

#ifdef DEBUG
    #include <stdio.h>
    #define debug(...)          printf(__VA_ARGS__)
#else
    #define debug(...)
#endif


#ifdef TI_EZ80
typedef uint8_t NeStream;
#else
typedef FILE* NeStream;
#endif





NeStream NeStream_open(char* name, char* mode);
void NeStream_close(NeStream stream);
void NeStream_write(NeStream stream, void* data, int size);


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