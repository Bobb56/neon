#include "headers/constants.h"
#include "headers/objects.h"
#define NEON_SOURCE_ID 4

#include <stdlib.h>
#include <string.h>

#include "headers/neonio.h"
#include "headers/errors.h"
#include "headers/neon.h"

//creation du tableau de pointeurs generiques a liberer en cas d’erreur
//static int memory = 0;

static const char* error_messages[NB_ERRORS] = {
    "",// ne doit pas être utilisé
    "",// ne doit pas être utilisé, correspond au code d'erreur d'exit
    "Multiple decimal points in <>.",
    "File <> doesn't exist.",
    "Cannot compute lenghth of <>.",
    "Variable <> is undefined.",
    "Too many arguments when calling user-defined function.\nExpected <> at most, got <> (including automatic assignation of optional arguments).",
    "Insufficient arguments when calling user-defined function.\nExpected at least <> (including automatic assignation of optional arguments), got <> (including automatic assignation of optional arguments).",
    "Call to an undefined function.",
    "Call to an invalid function.",
    "<> cannot be used as a start or an end value of a for loop.\nThe for loop start or end value should be an integer.",
    "Incorrect function definition.",
    "Not enough available memory to continue.",
    "Trying to extract a sub-string of length <> from <> starting at index <>.",
    "Invalid arguments provided in function call.",
    "Trying to index <> whereas it is not indexable.",
    "Cannot index a list or a string with <>. An index must be an integer.",
    "Attempt to index an undefined list.",
    "The item was not found in the list.",
    "Undetermined error, likely a syntax issue.",
    "Cannot evaluate <> as a condition.",
    "return expects only one argument, <> were given.",
    "'For' loop variant should be a variable, not a <>.",
    "<> is an incorrect built-in function name.",
    "Unknown operator <>.",
    "Unknown character <>.",
    "String, list, or other structure not terminated.",
    "Unclosed string at the end of line.",
    "Use of keyword <> in an expression.",
    "Block instruction line not followed by an instruction block (e.g., a lone if() statement).",
    "Improper use of an operator; incompatible types used\nconsecutively or an unknown/incompatible type.",
    "The module <> does not exist.",
    "Unable to deserialize container object because it is incompatible with an already existing container type.",
    "A non optional argument cannot follow optional arguments or unlimited arguments token '...'.",
    "Lone block of instructions.",
    "Incorrect statement definition.",
    "Incorrect number of arguments when calling built-in function <>.\nExpected <> arguments, got <>.",
    "Invalid index provided for inserting in a list.\nThe list's size is <> and the provided index was <>.",
    "Invalid arguments provided in call to <>.\nType help(<>) to see how to use that function.",
    "List or string index out of range.\nAttempt to index element <> whereas the object size is <>.",
    "Cannot perform <> operation on <> and <>.",
    "Incorrect use of parallel. The syntax is: parallel myFunction(arg1, arg2, etc) .",
    "Too many unnamed arguments. This function expects at most <> unnamed arguments.",
    "Cannot perform <> operation on undefined variable or object.",
    "Attempt to dereference <>, which does not exist as a variable.",
    "Euclidean division by zero.",
    "The first argument of format() must be a String, not <>.",
    "Too many arguments in format().\nThe format string specifies <> arguments but <> were given.",
    "Not enough arguments in format().\n<> arguments were given but at least <> were expected.",
    "",
    "",
    "Unsupported types for equality tests: <> and <>",
    "",
    "",
    "",
    "",
    "Attempt to assign the value of an undefined variable to another variable.",
    "Improper use of the '->' operator.",
    "<> operator cannot be applied to <>.",
    "Referencing the variable <>, which is undefined.",
    "Attempt to dereference <>. Only strings can be dereferenced.",
    "",
    "Cannot reverse <>.",
    "Unrecognized color \"<>\". Only \"blue\", \"green\", \"red\", \"purple\" and \"default\" are supported.",
    "Invalid arguments to randint: <> and <>. The randint function must have two positive integers a and b as arguments, with a < b.",
    "",
    "Undefined error.",
    "Can not open file <>.",
    "More closing parentheses, curved brackets or square brackets than opening ones.",
    "local must have at least one argument.",
    "local can only be used in a function or in a method.",
    "Assertion failed.",
    "The '.' operator is only for functions or methods and can only be applied on a variable.",
    "<> is an incorrect hexadecimal or binary number.",
    "", // code 74 spécial pour les appels d'exceptions
    "",
    "except block not preceded by a try block.",
    "try block not followed by an except block.",
    "except expects object of type Exception, not <>.",
    "Attempt to use `in` with <>. `in` can only be calculated with a list.",
    "'>>' is only for containers. Attempt to use '>>' with an object of type <>.",
    "A container field is set but the container is not used.",
    "The container type <> does not have a <> field.",
    "The container type <> already exists but with different fields definition.",
    "Containers must contain at least one field.",
    "Unknown container type <>.",
    "Two different fields cannot have the same name in a container. Field <> was defined multiple times.",
    "Incorrect container definition.",
    "<> not found in <>.",
    "Cannot assign a value or execute a method on <> because it is a literal object.",
    "That list cannot be sorted because <> and <> cannot be compared.",
    "Cannot call a function while dereferencing a container field.",
    "':' outside container definition.",
    "Unknown argument name in user-defined function call.",
    "Multiple assignation to <> in a user-defined function call.",
    "':=' outside function call or function definition.",
    "Bad arguments definition in function definition.",
    "Use of '...' outside function definition.",
    "The module <> is unavailable on the platform <>.",
    "Use of return outside function.",
    "You cannot call a <> in parallel. Only user-defined functions can be called in parallel.",
    "await takes exactly one argument, <> were given.",
    "Atomic time cannot be <>. It must be a positive integer, greater or equal to 1.",
    "Methods cannot be applied to volatile objects such as function or operator results.",
    "Program interrupted.", // code 104 spécial pour intercepter un CTRL-C
    "Cannot assign a value to a string index.",
    "Stack overflow encountered during runtime.",
    "The <> operator has not yet been implemented for containers of type <>.",
    "For loop can only be defined with 2, 3 or 4 arguments as follows : \n for (var, end)\n for (var, start, end)\n for (var, start, end, step).",
    "foreach needs exactly 2 arguments : a variable and a list or a string.",
    "Methods must have at least one argument, and this argument cannot be optional.",
    "for/foreach first argument must be a variable, not a <>.",
    "listComp range parameters must be integers.",
    "<> cannot be converted to an integer.",
    "This function can only receive integer numbers of real numbers as arguments, not <>.",
    "This functionality has not been implemented yet.",
    "Cannot initialize graphics without overriding the container type <>. Please check that all the containers you use respect the specification of graphic containers.",
    "The <> object cannot be drawn because it was not defined correctly.\nPlease see in the documentation how to define correctly a <> object",
    "str cannot be overloaded with a function that does not return a string. The function returned <>.",
    "File system error.",
    "Violation of Neon internal invariants.",
    "Cannot search for the index of <> in <>. The index function can only search for the index of an object in a list or a string in another string.",
    "Unable to deserialize object because of corrupted data."
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
    8,
    4,
    6,
    7,
    3,
    8,
    11,
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
    15,
    18,
    0,
    0,
    0,
    4,
    8,
    4,
    8,
    9,
    0,
    4,
    3,
    3,
    10,
    9,
    4,
    4,
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
    3,
    4,
    5,
    11,
    1,
    0,
    0,
    0,
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
    3,
    13,
    0,
    8,
    5,
    9,
    0,
    0,
    4,
    4,
    0,
    0,
    0,
    15,
    0,
    0,
    0,
    8,
    4,
    14,
    9,
    5,
    15,
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
    1,
    17,
    9,
    18
};


/*
0: SyntaxError
1: FileSystemError
2: UnmeasurableObject
3: Undefined
4: IncorrectFunctionCall
5: MemoryError
6: NonIndexableObject
7: IncorrectIndex
8: OutOfRange
9: IncorrectType
10: DivisionByZero
11: UnknownError
12: AssertionFailed
13: DefinitionError
14: KeyboardInterrupt
15: NotImplemented
16: ExitSignal
17: InternalError
18: DeserializationError
*/


int get_exception_from_code_error(int code_error) {
    return error_codes_exceptions[code_error];
}


void get_error_info_and_reset(int* code, int* source_id, int* line_number, NeList** error_message_arguments) {
    *code = global_env->CODE_ERROR;
    *source_id = global_env->ERROR_NEON_SOURCE_ID;
    *line_number = global_env->ERROR_NEON_LINE_NUMBER;
    *error_message_arguments = global_env->ERROR_MESSAGE_ARGUMENTS;

    global_env->CODE_ERROR = 0;
    global_env->ERROR_NEON_LINE_NUMBER = 0;
    global_env->ERROR_NEON_SOURCE_ID = 0;
    global_env->ERROR_MESSAGE_ARGUMENTS = NULL;
}

void neon_reset_error(void) {
    global_env->CODE_ERROR = 0;
    global_env->ERROR_NEON_LINE_NUMBER = 0;
    global_env->ERROR_NEON_SOURCE_ID = 0;

    if (global_env->ERROR_MESSAGE_ARGUMENTS != NULL) {
        nelist_destroy(global_env->ERROR_MESSAGE_ARGUMENTS);
        global_env->ERROR_MESSAGE_ARGUMENTS = NULL;
    }
}

void neon_set_error(int code_error, int line, int source_id, NeList* error_message_arguments) {
    neon_reset_error();

    global_env->CODE_ERROR = code_error;
    global_env->ERROR_NEON_LINE_NUMBER = line;
    global_env->ERROR_NEON_SOURCE_ID = source_id;
    global_env->ERROR_MESSAGE_ARGUMENTS = error_message_arguments;
}

void neon_raise_user_exception(int exception_code, char* format, NeList* args) {
    global_env->CODE_ERROR = -exception_code;
    global_env->EXCEPTION = format;
    global_env->ERROR_MESSAGE_ARGUMENTS = args;
}



void segfault(void) {
    volatile int* p = (int*)8;
    volatile int a = *p;
}


void printErrorString(char* format, NeList* error_message_arguments) {
    setColor(RED);
    int format_length = strlen(format);
    int specifier_length = strlen(FORMAT_ARGUMENT_SPECIFIER);

    int argument_index = 0;
    for (int i = 0 ; i < format_length ; i++) {
        // Identificateur d'argument
        if (argument_index < error_message_arguments->len && i < format_length - specifier_length + 1 && strncmp(format + i, FORMAT_ARGUMENT_SPECIFIER, specifier_length) == 0) {
            char* short_repr = neobject_short_repr(error_message_arguments->tab[argument_index], SHORT_REPR_ERR_SIZE, false);
            return_on_error();

            setColor(BLUE); printString(short_repr); setColor(RED);
            neon_free(short_repr);

            // On a consommé un argument
            argument_index += 1;

            // On passe le deuxième pourcentage
            i += specifier_length - 1;
        }
        else if (format[i] == '\n') {
            newLine();
            setColor(PURPLE);
            printString(" # ");
            setColor(RED);
        }
        else {
            const char current_character[2] = {format[i], 0};
            printString((char*)current_character);
        }
    }
    setColor(DEFAULT);
}








void printError(void) {
    int code, line_number, source_id;
    NeList* error_message_arguments;
    get_error_info_and_reset(&code, &source_id, &line_number, &error_message_arguments);

    if (code < 0) // CODE_ERROR négatif = exception lancée par l'utilisateur
    {
        setColor(PURPLE);
        printString(" ");
        printString((char*)global_env->EXCEPTIONS->tab[-code]);

        setColor(DEFAULT);
        printString(" happened in ");
        printErrSource(global_env->FILENAME, global_env->LINENUMBER);
        
        if (strlen(global_env->EXCEPTION) > 0) {
            setColor(PURPLE);
            printString(" # ");

            printErrorString(global_env->EXCEPTION, error_message_arguments);
        }
    }
    else
    {
        int type = get_exception_from_code_error(code);

        // # Exception: Explication de l'erreur
        setColor(PURPLE);
        printString(" ");
        printString((char*)global_env->EXCEPTIONS->tab[type]);

        setColor(DEFAULT);
        printString(" happened in ");
        printErrSource(global_env->FILENAME, global_env->LINENUMBER);
        
        setColor(PURPLE);
        printString(" # ");
        
        printErrorString((char*)error_messages[code], error_message_arguments);

        // # Exact error reference code#sourceid#lineno
        setColor(PURPLE);
        newLine();
        printString(" # ");
        
        setColor(DEFAULT);
        printString("Exact error reference ");

        // Affichage de la référence exacte de l'erreur
        setColor(GREEN);
        printInt(code);
        
        if (source_id != 0) {
            printString("#");
            printInt(source_id);
        }

        if (line_number != 0) {
            printString("#");
            printInt(line_number);
        }

    }
    setColor(DEFAULT);
    newLine();

    nelist_destroy(error_message_arguments);
}


// renvoie l'indice dans la chaîne de caractères correspondant à la ligne demandée
int getFileIndex(char* program, int line) {
    int compt = 1, i = 0;
    while (compt < line && program[i] != '\0') {
        if (program[i] == '\n')
            compt++;
        i++;
    }

    if (program[i] == '\0')
        return -1;

    // passe tous les espaces du début
    while (program[i] == ' ') i++;
    return i;
}



void bp(char* s) {
    volatile int a = 0;
    while (a == 0) {a *= a;}
}



void printErrSource(char* file, int line) {

    if (file != NULL) {
        printString("file ");
        setColor(GREEN); printString(file); setColor(DEFAULT);

        char* program = openFile(file);
        int debut = getFileIndex(program, line);
        if (debut != -1) {
            printString(" at line ");

            setColor(GREEN); printInt(line); setColor(DEFAULT);

            printString(" :");
            newLine();

            setColor(PURPLE);
            printString(" # ");

            setColor(GREEN);
            printString(">> ");

            // marque la fin de la ligne par '\0'
            int i = debut;
            while (program[i] != '\0' && program[i] != '\n') i++;

            program[i] = '\0';

            setColor(DEFAULT);
            printString(program + debut);

            setColor(GREEN);
            printString(" <<");
        }

        free(program);
    }
    else {
        setColor(GREEN);
        printString("standard input");
    }
    setColor(DEFAULT);
    newLine();
}