#define NEON_SOURCE_ID 12

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "headers/neon.h"
#include "headers/objects.h"
#include "headers/neonio.h"
#include "headers/parser.h"
#include "headers/strings.h"
#include "headers/errors.h"

#if defined(LINUX) || defined(WINDOWS) || defined(MINIMAL_LIBC_RISCV64)
    #include <stdio.h>
#elif defined(TI_EZ80)
    #include <ti/vars.h>
    #include <fileioc.h>
    #include "extern/nio_ce/headers/nspireio.h"
    #include "headers/graphicmodule.h"
#endif


#ifdef LINUX
#include "extern/linenoise.h"
#endif


#ifndef TI_EZ80

    NeStream NeStream_open(char* name, char* mode) {
        FILE* stream = fopen(name, mode);
        if (stream == NULL)
            neon_fail(67, neo_new_str_create(name));
        return stream;
    }

    void NeStream_close(NeStream stream) {
        fclose(stream);
    }

    void NeStream_write(NeStream stream, void* data, size_t size) {
        fwrite(data, 1, size, stream);
    }

    bool NeStream_read(NeStream stream, void* data, size_t size) {
        size_t count = fread(data, 1, size, stream);
        return count == size;
    }


    char* openFile(char* filename)
    {
        FILE* fichier = fopen(filename, "rt");//lit le fichier
        if (fichier == NULL)
        {
            neon_fail(67, neo_new_str_create(filename));
            return NULL;
        }
        
        //on regarde la position actuelle de la tête de fichier
    
        fpos_t pos;
        fgetpos(fichier, &pos);
    
        
        // calcul de la longueur du programme
        size_t longueur = 0;
        while (fgetc(fichier)!=EOF)
        {
            longueur++;
        }
        
        fsetpos(fichier, &pos);//remet la tête de fichier au début
        char* program=neon_malloc(longueur+1);// crée le tableau de caractères qui va contenir le programme
    

        // copie du fichier
        int car;
        for (int index = 0 ; (car=fgetc(fichier)) != EOF ; index++)
        {
            program[index] = (char)car;
        }
        program[longueur] = '\0';
        
        fclose(fichier);

        return program;
    }


    // cette fonction teste l'existence d'un launcher
    bool launcher(char* filename) {
        FILE* fichier = fopen(filename, "rt");//lit le fichier
        if (fichier == NULL) {
            return false;
        }
        fclose(fichier);
        return true;
    }


    void writeFile(char* filename, char* content)
    {
        FILE* fichier = fopen(filename, "w+");
        if (fichier == NULL)
        {
            neon_fail(67, neo_new_str_create(filename));
            return;
        }

        fprintf(fichier, "%s", content);
        fclose(fichier);
        return ;
    }

    #ifndef LINUX
        char* input(char *text) {
            if (strcmp(SEQUENCE_ENTREE, text) == 0) {
                setColor(BLUE);
                printString(SEQUENCE_ENTREE);
                setColor(DEFAULT);
            }
            else {
                printString(text);
            }
            fflush(stdout);
            
            uint8_t capacity = 9;
            char* buffer = neon_malloc(1 << capacity);
            size_t buffer_index = 0;

            while (true) {
                int c = getchar();

                if (c == EOF) {
                    neon_free(buffer);
                    neon_fail(1, NO_ARGS);
                    return "\0";
                }
                else if (c == '\n') {
                    break;
                }

                if (buffer_index >= ((size_t)1 << capacity)) {
                    capacity++;
                    buffer = neon_realloc(buffer, 1 << capacity);
                }
                buffer[buffer_index++] = c;
            }

            buffer[buffer_index] = 0;
            return buffer;
        }
    #else
        char* input(char* text)
        {
            char* str = linenoise(text);

            if (str == NULL) {
                neon_fail(104, NO_ARGS);
                return NULL;
            }
            else if (strlen(str) > 0) { // Ajout de la ligne à l'historique
                linenoiseHistoryAdd(str);
            }
            return str;
        }
    #endif



    char* double_to_str(double number)//nombre en chaine de caractère
    {
        if (isinf(number) && number > 0)
                return strdup(get_infinity());
        else if (isinf(number) && number < 0)
            return addStr("-", get_infinity());
        else if (isnan(number))
            return strdup(get_nan());
        else
        {
            //une fois qu'on a copié le nombre dans la chaine de caracteres, il faut le réallouer de la bonne taille
            char* strNombre = (char*)neon_malloc(310*sizeof(char));//on estime qu'un double ne fait pas plus de 50 caractères de longueur
            int err = snprintf(strNombre, 310, "%lf", number);//converison du nombre
            
            if (err < 0 || (unsigned)err != strlen(strNombre))
            {
                neon_fail(66, NO_ARGS);
                neon_free(strNombre);
                return 0;
            }
            
            removeZeros(strNombre);
                        
            return strNombre;
        }
        
    }
    
    
    void flush(void) {
        fflush(stdout);
    }
    
    
    void printString(char* s)
    {
        printf("%s", s);
    }
    
    
    void setColor(unsigned char color)
    {
        #ifdef COLOR
            if (color == BLUE)
                printf("\033[1;34m"); // bleu et gras
            else if (color == GREEN)
                printf("\033[0;32m"); // vert
            else if (color == RED)
                printf("\033[1;31m"); // met en rouge et gras
            else if (color == PURPLE)
                printf("\033[1;35m"); // met en violet et gras
            else if (color == DEFAULT)
                printf("\033[0;00m");
        #else
        UNUSED_PARAMETER(color);
        #endif
    }
    
    
    void clearConsole(void)
    {
        #if defined(LINUX)
            system("clear");
        #elif defined(WINDOWS)
            system("cls");
        #endif
    }

#else //------------------------------------------------- PASSAGE A TI_EZ80 ---------------------------------------------


    NeStream NeStream_open(char* name, char* mode) {
        uint8_t stream = ti_Open(name, mode);
        if (stream == 0)
            neon_fail(67, neo_new_str_create(name));
        return stream;
    }

    void NeStream_close(NeStream stream) {
        ti_Close(stream);
    }

    void NeStream_write(NeStream stream, void* data, int size) {
        ti_Write(data, 1, size, stream);
    }

    bool NeStream_read(NeStream stream, void* data, int size) {
        int count = ti_Read(data, 1, size, stream);
        return count == size;
    }

    char* argsAns(void)
    {
        // récupère le contenu de Ans
        uint8_t type;
        string_t* string = os_GetAnsData(&type);

        if (string == NULL)
            return NULL;
        else if (type != OS_TYPE_STR || string->len == 0)
        {
            neon_free(string);
            return NULL;
        }


        // transforme string en chaine de caractères
        char* ret = neon_malloc(sizeof(char) * (string->len + 1));
        for (int i=0 ; i < string->len ; i++)
            ret[i] = string->data[i];

        ret[string->len] = '\0';

        neon_free(string);

        return ret;

    }
    


    char* openFile(char* filename)
    {
        uint8_t fichier = ti_Open(filename, "r"); //ouvre l'AppVar
        if (fichier == 0)
        {
            neon_fail(67, neo_new_str_create(filename));
            return NULL;
        }
        
        //calcul de la longueur de l'AppVar
    
        uint16_t longueur = ti_GetSize(fichier);

        
        char* program = neon_malloc(longueur + 1);// crée le tableau de caractères qui va contenir le programme
    

        // copie du fichier
        ti_Read(program, sizeof(char), longueur, fichier);
        
        program[longueur] = '\0';
        
        ti_Close(fichier);

        // on enlève le préfixe propre aux appvars python s'il y en a un
        if (memcmp(program, "PYCD\x00", 5)==0 || memcmp(program, "NEON\x00", 5)==0)
            memcpy(program, "     ", 5);

        return program;
    }


    // cette fonction teste l'existence d'un launcher
    bool launcher(char* filename) {
        uint8_t fichier = ti_Open(filename, "r"); //ouvre l'AppVar

        if (fichier == 0) {
            ti_Close(fichier);
            return false;
        }

        if (ti_GetSize(fichier) < 5) {
            ti_Close(fichier);
            return false;
        }

        char buffer[5];

        ti_Read(buffer, sizeof(char), 5, fichier);

        ti_Close(fichier);
        return memcmp(buffer, "NEON\x00", 5) == 0 || memcmp(buffer, "#NEON", 5) == 0;
    }


    void writeFile(char* filename, char* content)
    {
        uint8_t fichier = ti_Open(filename, "w"); //ouvre l'AppVar
        if (fichier == 0)
        {
            neon_fail(67, neo_new_str_create(filename));
            return;
        }
        
        //écriture
        ti_Write(content, sizeof(char), strlen(content), fichier);
        
        ti_Close(fichier);
    }


    char* input(char *text)
    {
        char* var = neon_malloc(501*sizeof(char)); // allocation d'un pointeur pour l'entrée de l'utilisateur (+1 char pour le caractère nul)
        
        if (var == NULL) {
            neon_fail(12, NO_ARGS);
            return NULL;
        }
        
        memset(var, (char)0, 501*sizeof(char)); //initialise le pointeur à '\0' partout
        //on effectue l'entrée
        printString(text);
    
        if (!nio_getsn(var, 500))
        {
            neon_fail(1, NO_ARGS);
            return NULL;
        }

        // crée un deuxième pointeur pour y copier le contenu de l'entrée de la vraie longueur
        char* newVar = neon_malloc(sizeof(char)*(strlen(var)+1));//réserve une place de la longueur de l'entrée + 1 pour le caractère nul

        if (newVar == NULL) {
            neon_fail(12, NO_ARGS);
            neon_free(var);
            return NULL;
        }
    
        void* ptrtest = strcpy(newVar, var); //copie de var dans newVar
    
        neon_free(var);
    
        if (ptrtest == NULL) {
            neon_free(newVar);
            neon_fail(66, NO_ARGS);
            return NULL;
        }
    
        return newVar;
    }



    char* double_to_str(double number)//nombre en chaine de caractère
    {
        if (isnan(number)) {
            return strdup(get_nan());
        }
        else if (isinf(number) && number < 0) {
            return addStr("-", get_infinity());
        }
        else if (isinf(number) && number > 0) {
            return strdup(get_infinity());
        }
        else {
            real_t x = os_FloatToReal((float)number);
            char* result = neon_malloc(sizeof(char) * 127);
            os_RealToStr(result, &x, 127, 1, -1);

            // enlève les zéros inutiles à la fin
            removeZeros(result);

            return result;
        }
        
    }
    
    
    void flush(void) {
        nio_fflush(&global_env->console);
    }
    
    
    
    void printString(char* s)
    {
        nio_fputs(s, &global_env->console);
    }
    
    
    
    void setColor(unsigned char color)
    {
        if (color == BLUE)
            nio_color(&global_env->console, NEON_PALETTE_WHITE, NEON_PALETTE_BLUE);
        else if (color == GREEN)
            nio_color(&global_env->console, NEON_PALETTE_WHITE, NEON_PALETTE_GREEN);
        else if (color == RED)
            nio_color(&global_env->console, NEON_PALETTE_WHITE, NEON_PALETTE_RED);
        else if (color == PURPLE)
            nio_color(&global_env->console, NEON_PALETTE_WHITE, NEON_PALETTE_PURPLE);
        else if (color == DEFAULT)
            nio_color(&global_env->console, NEON_PALETTE_WHITE, NEON_PALETTE_BLACK);
    }
    
    
    void clearConsole(void)
    {
        nio_clear(&global_env->console);
    }

#endif

void removeZeros(char* string) {
    int len = strlen(string);

    while (string[len - 1] == '0') { // on enlève les zéros inutiles à la fin
        string[len - 1] = '\0';
        len--;
    }
    
    
    if (string[len - 1] == '.') { // si on a enlevés tous les zéros, on enlève aussi la virgule
        string[len - 1] = '\0';
    }

    return;
}


double str_to_double(char *string)//convertit une chaîne de caractère en nombre
{
    if (strcmp(string, get_infinity()) == 0)
        return INFINITY;
    else if (string[0] == '-' && strcmp(string + 1, get_infinity()) == 0)
        return -INFINITY;
    else if (strcmp(string, get_nan()) == 0)
        return NAN;
    else
    {
        return atof(string);
    }
}





int unitCharToInt(char* whole_number, char car, int base)
{
    if (car != '0' && car != '1' && base == 2) // le binaire n'autorise que 0 ou 1
        neon_fail(73, neo_new_str_create(whole_number));
    
    if (isdigit(car))
        return car - '0';
    else if (car >= 'a' && car <= 'f')
        return car - 'a' + 10;
    else if (car >= 'A' && car <= 'F')
        return car - 'A' + 10;

    neon_fail(73, neo_new_str_create(whole_number));
    return 0;
}



bool is_integer(char* string) {
    int i = 0;
    if (string[0] == '-')
        i++;

    if (string[i] != '\0' && string[i+1] != '\0' && !isdigit(string[i+1])) {
        if (string[i] == '0' && string[i+1] == 'b') {
            // nombre binaire
            i += 2; // on passe les deux caractères
            while (string[i] != '\0') {
                if (string[i] < '0' || string[i] > '1')
                    return false;
                i++;
            }
            return true;
        }
        else if (string[i] == '0' && string[i+1] == 'x') {
            // nombre hexa
            i += 2; // on passe les deux caractères
            while (string[i] != '\0') {
                if (!((string[i] >= 'a' && string[i] <= 'f') || (string[i] >= 'A' && string[i] <= 'F') || (string[i] >= '0' && string[i] <= '9')))
                    return false;
                i++;
            }
            return true;
        }
    }
    else {
        // nombre décimal
        while (string[i] != '\0') {
            if (string[i] < '0' || string[i] > '9')
                return false;
            i++;
        }
        return true;
    }
    return false;
}


// cette fonction compte le nombre de chiffres d'un entier
size_t nbdigits(intptr_t n) {
    size_t count = 0;
    while (n > 0) {
        n -= n%10;
        n /= 10;
        count ++;
    }
    return count;
}






// on peut supposer que la chaîne de caractère représente bien un nombre entier
intptr_t str_to_int(char* string) {
    bool negative = (string[0] == '-');

    if (negative)
        string += 1;

    intptr_t n = 0;

    if (string[0] != '\0' && string[1] != '\0' && !isdigit(string[1])) {
        if (string[0] == '0' && string[1] == 'b') {
            // nombre binaire
            int i = 2;
            while (string[i] != '\0') {
                n *= 2;
                n += unitCharToInt(string, string[i++], 2);
                return_on_error(0);
            }
        }
        else if (string[0] == '0' && string[1] == 'x') {
            // nombre hexa
            int i = 2;
            while (string[i] != '\0') {
                n *= 16;
                n += unitCharToInt(string, string[i++], 16);
                return_on_error(0);
            }
        }
    }
    else {
        // nombre décimal
        int i = 0;
        while (string[i] != '\0') {
            n *= 10;
            n += unitCharToInt(string, string[i++], 10);
            return_on_error(0);
        }
    }

    if (negative)
        return -n;
    else
        return n;
}



char* int_to_str(intptr_t number)//nombre en chaine de caractère
{
    if (number == 0) {
        return strdup("0");
    }
    else {
        // on se souvient de si le nombre est négatif ou pas, et on le met en valeur absolue
        bool neg = number < 0;
        if (neg)
            number = -number;
        
        size_t lenstr = nbdigits(number) + 1;

        if (neg)
            lenstr++;

        char* strNombre = neon_malloc(sizeof(char) * lenstr);
        int i = lenstr - 1;
        strNombre[i--] = '\0';
        

        while (number > 0) {
            strNombre[i--] = number%10 + '0';
            number -= number%10;
            number /= 10;
        }

        if (neg)
            strNombre[0] = '-';

        return strNombre;
    }
}


void printDouble(double x)
{
    char* xstr = double_to_str(x);
    printString(xstr);
    neon_free(xstr);
}


void printInt(intptr_t n) {
    char* nstr = int_to_str(n);
    printString(nstr);
    neon_free(nstr);
}

void newLine(void) {
    printString("\n");
}

void neon_pause(char* text) {
    //flush();
    char* res = input(text);
    if (res != NULL)
        neon_free(res);
}
