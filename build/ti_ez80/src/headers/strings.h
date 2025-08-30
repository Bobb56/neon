#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>
#include <stdint.h>

#include "constants.h"
#include "dynarrays.h"

intptr_t binToDec(char* chaine, int debut, int longueur);
intptr_t hexToDec(char* chaine, int debut, int longueur);
bool isFull(char* string);
bool isString(char* string, char* test, int size);
char* inputCode(char*);
char* sub(char* string,int debut,int fin);//a liberer après
char* replace(char*, char*, char*);//a liberer après
void cleanStdin(void);
char* addStr(char* str1, char* str2);//a liberer après
int count(char* string, char* search);
char* subReplace(char* string, int len, int debut, int longueur, char* remplacement, int len_remplacement);
int sindex (char* string, char car);
char* traitementString(char* string);//a liberer apres
char* traitementStringInverse(char* string);//a liberer apres
char* addStr2(char* str1, char* str2);
char* charToString(char car);
char* sandwich(char* string, char car);
bool strToBool(char* string);
bool isidentifier(char c);
bool has_strict_prefix(char* string, char* prefix);
char* remove_prefix(char* string, char* prefix);
char* get_prefix(char* string);
strlist* get_all_modules(void);
bool is_module(char* module);
int function_module(char* module, char* function);


#endif