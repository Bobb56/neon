#include <stdlib.h>

#include "headers/neonio.h"
#include "headers/dynarrays.h"
#include "headers/printerror.h"
#include "headers/neon.h"

//creation du tableau de pointeurs generiques a liberer en cas d’erreur

int indent = 0;


//creation du tableau de pointeurs generiques a liberer en cas d’erreur
extern strlist ERRORS_MESSAGES;
extern strlist exceptions;
extern intlist exceptions_err;



void printError(int code)
{
    if (code < 0) // CODE_ERROR négatif = exception lancée par l'utilisateur
    {
        setColor(RED);
        printString(" ");
        printString(exceptions.tab[-code]);
        printString(": ");
        printString(global_env->EXCEPTION);
    }
    else
    {
        int type = exceptions_err.tab[code];

        setColor(RED);

        printString(" ");
        printString(exceptions.tab[type]);
        printString(": ");
        
        printString(ERRORS_MESSAGES.tab[code]);
        setColor(GREEN); printString(" (");setColor(WHITE);printInt(code);setColor(GREEN);printString(")");
    }
    setColor(WHITE);
    newLine();
    affLine(global_env->FILENAME, global_env->LINENUMBER);
}




void affLine(char* file, int line) {
    setColor(RED);
    printString(" # ");
    setColor(WHITE);

    if (file != NULL) {
        printString("Error happened in file ");
        setColor(GREEN); printString(file); setColor(WHITE);
        printString(" at line ");

        setColor(GREEN); printInt(line); setColor(WHITE);

        printString(" :");
        newLine();

        setColor(RED);
        printString(" # ");
        setColor(WHITE);

        char* program = openFile(file);
        int compt = 1, i = 0;

        // cherche le début de la ligne
        while (compt < line && program[i] != '\0') {
            if (program[i] == '\n')
                compt++;
            i++;
        }

        // passe tous les espaces du début
        while (program[i] == ' ') i++;

        // marque la fin de la ligne
        int debut = i;
        while (program[i] != '\0' && program[i] != '\n')
            i++;

        program[i] = '\0';

        printString(program + debut);
        free(program);
    }
    else {
        printString("Error happened in standard input");
    }
    newLine();
}