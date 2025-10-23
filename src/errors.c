#include <stddef.h>
#include <stdlib.h>

#include "headers/neonio.h"
#include "headers/errors.h"
#include "headers/neon.h"

//creation du tableau de pointeurs generiques a liberer en cas d’erreur
//static int memory = 0;

static const char* error_messages[NB_ERRORS] = {
    "",// ne doit pas être utilisé
    "",// ne doit pas être utilisé, correspond au code d'erreur d'exit
    "Multiple decimal points in the same number",
    "File doesn't exist",
    "This object has no length",
    "Use of an undefined variable",
    "Too many arguments in user-defined function call",
    "Insufficient arguments in user-defined function call",
    "Call to an undefined function",
    "Call to an invalid function",
    "The for loop start or end value is not an integer",
    "Incorrect function definition",
    "Memory error",
    "Incorrect index for subtree deletion",
    "Invalid arguments provided in function call",
    "This object is not indexable",
    "List index is not a number",
    "Attempt to index an undefined list",
    "List index out of range",
    "Undetermined error, likely a syntax issue",
    "Unevaluable condition due to incorrect type",
    "Multiple arguments for return statement",
    "'For' loop variant is not a variable",
    "Incorrect built-in function name in function call",
    "Unknown operator",
    "Unknown character",
    "String, list, or other structure not terminated",
    "Unclosed string at the end of line",
    "Incompatible keyword used in an expression",
    "Block instruction line not followed by an instruction block (e.g., a lone if() statement)",
    "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
    "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
    "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
    "Improper use of an operator; incompatible types used consecutively or an unknown/incompatible type",
    "Lone block of instructions",
    "Incorrect statement definition",
    "Incorrect number of arguments when calling a built-in function",
    "Index out of range for inserting into a NeList",
    "Index out of range for deleting from a NeList",
    "Index out of range to access in a NeList",
    "Unsupported types for addition",
    "Unsupported types for subtraction",
    "Unsupported types for division",
    "Unsupported types for multiplication",
    "Unsupported types for modulo",
    "Euclidean division by zero",
    "Unsupported types for Euclidean division",
    "Unsupported types for 'and' operator",
    "Unsupported types for 'or' operator",
    "Unsupported types for 'xor' operator",
    "Unsupported types for exponentiation",
    "Unsupported types for equality tests",
    "Unsupported types for <=",
    "Unsupported types for >=",
    "Unsupported types for <",
    "Unsupported types for >",
    "Attempt to assign the value of an undefined variable to another variable",
    "Improper use of the '->' operator",
    "Unsupported types for 'not' operator",
    "Referencing an undefined variable",
    "Dereferencing something other than a string",
    "Unsupported types for unary minus operator",
    "Unsupported types for reverse function",
    "Attempt to use help on a non-function variable",
    "The randint function must have two positive integers a and b as arguments, with a < b",
    "Pointer is not allocated",
    "Undefined error",
    "Can not open file",
    "More closing parentheses, curved brackets or square brackets than opening ones",
    "local must have at least one argument",
    "local can only be used in a function or in a method",
    "Assertion failed",
    "The '.' operator is only for functions or methods and can only be applied on a variable",
    "Incorrect hexadecimal or binary number",
    "", // code 74 spécial pour les appels d'exceptions
    "Unsupported types for implication.",
    "except block not preceded by a try block.",
    "try block not followed by an except block",
    "except expects exceptions",
    "in can only be calculated with a list",
    "'>>' is only for containers",
    "A container field is set but the container is not used",
    "Unknown container field",
    "Same container name but not same fields",
    "Containers must contain at least one field",
    "Unknown container type",
    "Two different fields cannot have the same name in a container",
    "Incorrect container definition",
    "Object not in list",
    "Cannot assign a value or execute a method on a litteral object",
    "That list cannot be sorted",
    "Cannot call a function while dereferencing a container field",
    "':' outside container definition",
    "Unknown argument name in user-defined function call",
    "You have already given a value to this argument in user-defined function call",
    "':=' outside function call or function definition",
    "Bad arguments definition in function definition",
    "Use of '...' outside function definition",
    "Can only set a documentation for a user-defined function or method",
    "Use of return outside function",
    "Can only call a user-defined function in parallel",
    "await takes exactly one argument",
    "Atomic time must be a positive integer, greater or equal to 1",
    "Methods cannot be applied to volatile objects such as function or operators",
    "Program interrupted", // code 104 spécial pour intercepter un CTRL-C
    "Cannot assign a value to a string index",
    "Stack overflow encountered during runtime",
    "This operator has not yet been implemented for this type of container",
    "For loop can only be defined with 2, 3 or 4 arguments as follows : \n for (var, end)\n for (var, start, end)\n for (var, start, end, step)",
    "foreach needs exactly 2 arguments : a variable and a list or a string",
    "Methods must have at least one argument, and this argument cannot be optional",
    "for/foreach first argument must be a variable",
    "listComp range parameters must be integers",
    "This object cannot be converted to an integer",
    "This function can only receive integer numbers of real numbers as arguments",
    "This functionality has not been implemented yet",
    "Cannot initialize graphics without overriding an already defined type",
    "This figure object cannot be drawn because it was not defined correctly",
    "str cannot be overloaded with a function that does not return a string",
    "File system error"
};



static const int error_codes_exceptions[NB_ERRORS] = {
    0,// ne doit pas être utilisé
    16,//ExitSignal
    0,
    1,
    2,
    3,
    4,
    4,
    3,
    4,
    0,
    0,
    5,
    11,
    4,
    6,
    7,
    3,
    8,
    0,
    9,
    0,
    9,
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    8,
    8,
    8,
    9,
    9,
    9,
    9,
    9,
    10,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    9,
    3,
    9,
    9,
    3,
    9,
    9,
    9,
    9,
    9,
    5,
    11,
    1,
    0,
    0,
    9,
    12,
    9,
    0,
    0,
    9,
    0,
    0,
    9,
    9,
    9,
    5,
    3,
    13,
    13,
    13,
    13,
    0,
    8,
    4,
    9,
    0,
    0,
    4,
    4,
    0,
    0,
    0,
    9,
    0,
    0,
    0,
    8,
    4,
    14,
    9,
    5,
    9,
    0,
    0,
    0,
    0,
    9,
    9,
    9,
    15,
    13,
    9,
    9,
    1
};


int get_exception_from_code_error(int code_error) {
    return error_codes_exceptions[code_error];
}

/*
void* neon_malloc(size_t size) {
    memory += size;
    return malloc(size);
}

void* neon_realloc(void* ptr, size_t size) {
    memory += size;
    return realloc(ptr, size);
}

void neon_free(void* ptr) {
    free(ptr);
}
*/
/*
int allocatedMem(void) {
    return memory;
}


void segfault(void) {
    volatile int* p = (int*)8;
    volatile int a = *p;
}*/

void printError(int code)
{
    if (code < 0) // CODE_ERROR négatif = exception lancée par l'utilisateur
    {
        setColor(RED);
        printString(" ");
        printString((char*)global_env->EXCEPTIONS->tab[-code]);
        printString(": ");
        printString(global_env->EXCEPTION);
    }
    else
    {
        int type = get_exception_from_code_error(code);

        setColor(RED);

        printString(" ");
        printString((char*)global_env->EXCEPTIONS->tab[type]);
        printString(": ");
        
        printString((char*)error_messages[code]);
        setColor(GREEN); printString(" (");setColor(DEFAULT);printInt(code);setColor(GREEN);printString(")");
    }
    setColor(DEFAULT);
    newLine();
    affLine(global_env->FILENAME, global_env->LINENUMBER);
}




void affLine(char* file, int line) {
    setColor(RED);
    printString(" # ");
    setColor(DEFAULT);

    if (file != NULL) {
        printString("Error happened in file ");
        setColor(GREEN); printString(file); setColor(DEFAULT);
        printString(" at line ");

        setColor(GREEN); printInt(line); setColor(DEFAULT);

        printString(" :");
        newLine();

        setColor(RED);
        printString(" # ");
        setColor(DEFAULT);

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