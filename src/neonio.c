#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "headers/neonio.h"
#include "headers/linenoise.h"

#ifndef LINUX
#include "headers/strings.h"
#endif



extern int CODE_ERROR;



#ifndef TI83PCE



    char* openFile(char* filename)
    {
        FILE* fichier = fopen(filename, "rt");//lit le fichier
        if (fichier == NULL)
        {
            CODE_ERROR = 67;
            return NULL;
        }
        
        //on regarde la position actuelle de la tête de fichier
    
        fpos_t pos;
        fgetpos(fichier, &pos);
    
        
        // calcul de la longueur du programme
        int longueur = 0;
        while (fgetc(fichier)!=EOF)
        {
            longueur++;
        }
        
        fsetpos(fichier, &pos);//remet la tête de fichier au début
        char* program=malloc(longueur+1);// crée le tableau de caractères qui va contenir le programme
    

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


    void writeFile(char* filename, char* content)
    {
        FILE* fichier = fopen(filename, "w+");
        if (fichier == NULL)
        {
            CODE_ERROR = 67;
            return;
        }

        fprintf(fichier, "%s", content);
        fclose(fichier);
        return ;
    }

    #ifndef LINUX
        char* input(char *text)
        {
            char* var=malloc(4001*sizeof(char)); // allocation d'un pointeur pour l'entrée de l'utilisateur (+1 char pour le caractère nul)
            memset(var,(char)0,4001*sizeof(char));//initialise le pointeur à '\0' partout
            //on effectue l'entrée
            printf("%s",text);
            
            int err = scanf("%4000[^\n]",var);
            
            cleanStdin();
            
            if (err != 1)
            {
                free(var);
                CODE_ERROR = 104;
                return NULL;
            }
            
            //crée un deuxième pointeur pour y copier le contenu de l'entrée de la vraie longueur
            char* newVar = malloc(sizeof(char)*(strlen(var)+1));//réserve une place de la longueur de l'entrée + 1 pour le caractère nul
            
            void * ptrtest=strcpy(newVar,var);//copie de var dans newVar
            
            free(var);
            var=NULL;
                
            if (ptrtest==NULL)
            {
                free(newVar);
                CODE_ERROR = 66;
                return NULL;
            }
            
            return newVar;
        }
    #else
        char* input(char* text)
        {
            char* str = linenoise(text);

            if (str == NULL) {
                CODE_ERROR = 104;
                return NULL;
            }
            else if (strlen(str) > 0) { // Ajout de la ligne à l'historique
                linenoiseHistoryAdd(str);
            }
            return str;
        }
    #endif

    bool is_integer(char* string) {
        int i = 0;
        if (string[0] == '-')
            i++;

        while (string[i] != '\0') {
            if (string[i] < '0' || string[i] > '9')
                return false;
            i++;
        }
        return true;
    }


    // cette fonction compte le nombre de chiffres d'un entier
    int nbdigits(intptr_t n) {
        int count = 0;
        while (n > 0) {
            n -= n%10;
            n /= 10;
            count ++;
        }
        return count;
    }




    intptr_t str_to_int(char* string) {
        bool negative = (string[0] == '-');

        if (negative)
            string += 1;

        intptr_t n = 0;
        int i = 0;

        while (string[i] != '\0') {
            n *= 10;
            n += string[i++] - '0';
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
            int lenstr = nbdigits(number) + 1;

            if (number < 0)
                lenstr++;

            char* strNombre = malloc(sizeof(char) * lenstr);
            int i = lenstr - 1;
            strNombre[i--] = '\0';

            while (number > 0) {
                strNombre[i--] = number%10 + '0';
                number -= number%10;
                number /= 10;
            }

            if (number < 0)
                strNombre[0] = '-';

            return strNombre;
        }
    }
    

    double str_to_double(char *string)//convertit une chaîne de caractère en nombre
    {
        if (strcmp(string, "Infinity") == 0)
            return INFINITY;
        else if (strcmp(string, "-Infinity") == 0)
            return -INFINITY;
        else if (strcmp(string, "NaN") == 0)
            return NAN;
        else
        {
            double nombre;
            
            int err = sscanf(string,"%lf",&nombre);
            if (err!=1)
            {
                CODE_ERROR = 66;
                return 0;
            }
            return nombre;
        }
    }



    char* double_to_str(double number)//nombre en chaine de caractère
    {
        if (isinf(number) && number > 0)
                return strdup("Infinity");
        else if (isinf(number) && number < 0)
            return strdup("-Infinity");
        else if (isnan(number))
            return strdup("NaN");
        else
        {
            //une fois qu'on a copié le nombre dans la chaine de caracteres, il faut le réallouer de la bonne taille
            char* strNombre = (char*)malloc(310*sizeof(char));//on estime qu'un double ne fait pas plus de 50 caractères de longueur
            int err = snprintf(strNombre, 310, "%lf", number);//converison du nombre
            
            if (err<0 || err!=(int)strlen(strNombre))
            {
                CODE_ERROR = 66;
                free(strNombre);
                return 0;
            }
            
            while (strNombre[strlen(strNombre)-1]=='0')//on enlève les zéros inutiles à la fin
            {
                strNombre[strlen(strNombre)-1]='\0';
            }
            
            
            if (strNombre[strlen(strNombre)-1]=='.')//si on a enlevés tous les zéros, on enlève aussi la virgule
            {
                strNombre[strlen(strNombre)-1]='\0';
            }
            
            //ici, faire un realloc avec strlen
            
            return strNombre;
        }
        
    }
    
    
    void flush(void) {
        fflush(stdin);
    }
    
    
    void printInt(int n)
    {
        printf("%d", n);
    }
    
    
    void printDouble(double x)
    {
        printf("%lf", x);
    }
    
    void printString(char* s)
    {
        printf("%s", s);
    }
    
    void newLine(void)
    {
        printf("\n");
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
            else if (color == WHITE)
                printf("\033[0;00m");
        #endif
    }
    
    void printBack(void)
    {
        printf("\b");
    }
    
    void clearConsole(void)
    {
        #if defined(LINUX)
            int result = system("clear");
        #elif defined(WINDOWS)
            int result = system("cls");
        #elif defined(WASM)
            printf("T5#sD9@jPzQ7fH*2m$1\n");
        #endif
    }

#else //-------------------------------------------------PASSAGE A TI83PCE---------------------------------------------

    #include "nio_ce.h"
    #include <ti/vars.h>
    #include <fileioc.h>

    extern nio_console console;



    char* argsAns(void)
    {
        // récupère le contenu de Ans
        uint8_t type;
        string_t* string = os_GetAnsData(&type);

        if (string == NULL)
            return NULL;
        else if (type != OS_TYPE_STR || string->len == 0)
        {
            free(string);
            return NULL;
        }


        // transforme string en chaine de caractères
        char* ret = malloc(sizeof(char) * (string->len + 1));
        for (int i=0 ; i < string->len ; i++)
            ret[i] = string->data[i];

        ret[string->len] = '\0';

        free(string);

        return ret;

    }
    


    char* openFile(char* filename)
    {
        uint8_t fichier = ti_Open(filename, "r"); //ouvre l'AppVar
        if (fichier == 0)
        {
            CODE_ERROR = 67;
            return NULL;
        }
        
        //calcul de la longueur de l'AppVar
    
        uint16_t longueur = ti_GetSize(fichier);

        
        char* program=malloc(longueur+1);// crée le tableau de caractères qui va contenir le programme
    

        // copie du fichier
        ti_Read(program, sizeof(char), longueur, fichier);
        
        program[longueur] = '\0';
        
        ti_Close(fichier);

        return program;
    }



    void writeFile(char* filename, char* content)
    {
        uint8_t fichier = ti_Open(filename, "w"); //ouvre l'AppVar
        if (fichier == 0)
        {
            CODE_ERROR = 67;
            return;
        }
        
        //écriture
        ti_Write(content, sizeof(char), strlen(content), fichier);
        
        ti_Close(fichier);
    }


    char* input(char *text)
    {
        char* var=malloc(4001*sizeof(char)); // allocation d'un pointeur pour l'entrée de l'utilisateur (+1 char pour le caractère nul)
        memset(var,(char)0,4001*sizeof(char));//initialise le pointeur à '\0' partout
        //on effectue l'entrée
        nio_printf("%s",text);
    
        if (!nio_getsn(var, 4000))
        {
            CODE_ERROR = 1;
            return NULL;
        }
    
    
    
        //crée un deuxième pointeur pour y copier le contenu de l'entrée de la vraie longueur
        char* newVar = malloc(sizeof(char)*(strlen(var)+1));//réserve une place de la longueur de l'entrée + 1 pour le caractère nul
    
        void * ptrtest=strcpy(newVar,var);//copie de var dans newVar
    
        free(var);
        var=NULL;
    
        if (ptrtest==NULL)
        {
            free(newVar);
            CODE_ERROR = 66;
            return NULL;
        }
    
        return newVar;
    }


    double str_to_double(char *string)//convertit une chaîne de caractère en nombre
    {
        if (strcmp(string, "Infinity") == 0)
            return INFINITY;
        else if (strcmp(string, "-Infinity") == 0)
            return -INFINITY;
        else if (strcmp(string, "NaN") == 0)
            return NAN;
        else
        {
            double nombre;
            
            int err = sscanf(string,"%lf",&nombre);
            if (err!=1)
            {
                CODE_ERROR = 66;
                return 0;
            }
            return nombre;
        }
    }



    char* double_to_str(double number)//nombre en chaine de caractère
    {
        if (isinf(number) && number > 0)
                return strdup("Infinity");
        else if (isinf(number) && number < 0)
            return strdup("-Infinity");
        else if (isnan(number))
            return strdup("NaN");
        else
        {
            //une fois qu'on a copié le nombre dans la chaine de caracteres, il faut le réallouer de la bonne taille
            char* strNombre = (char*)malloc(310*sizeof(char));//on estime qu'un double ne fait pas plus de 50 caractères de longueur
            int err = snprintf(strNombre, 310, "%lf", number);//converison du nombre
            
            if (err<0 || err!=(int)strlen(strNombre))
            {
                CODE_ERROR = 66;
                free(strNombre);
                return 0;
            }
            
            while (strNombre[strlen(strNombre)-1]=='0')//on enlève les zéros inutiles à la fin
            {
                strNombre[strlen(strNombre)-1]='\0';
            }
            
            
            if (strNombre[strlen(strNombre)-1]=='.')//si on a enlevés tous les zéros, on enlève aussi la virgule
            {
                strNombre[strlen(strNombre)-1]='\0';
            }
            
            //ici, faire un realloc avec strlen
            
            return strNombre;
        }
        
    }
    
    
    void flush(void) {
        nio_fflush(&console);
    }

    
    void printInt(int n)
    {
        nio_printf("%d", n);
    }
    
    
    void printDouble(double n)
    {
        real_t x = os_FloatToReal((float)num)
        char* result = malloc(sizeof(char) * 127);
        os_RealToStr(result, &x, 127, 1, -1);
        nio_printf("%s", result);
    }
    
    void printString(char* s)
    {
        nio_printf("%s", s);
    }
    
    void newLine(void)
    {
        nio_printf("\n");
    }
    
    
    void setColor(unsigned char color)
    {
        if (color == BLUE)
            nio_color(&console, NIO_COLOR_BLACK, NIO_COLOR_LIGHTBLUE);
        else if (color == GREEN)
            nio_color(&console, NIO_COLOR_BLACK, NIO_COLOR_GREEN);
        else if (color == RED)
            nio_color(&console, NIO_COLOR_BLACK, NIO_COLOR_RED);
        else if (color == WHITE)
            nio_color(&console, NIO_COLOR_BLACK, NIO_COLOR_WHITE);
    }
    
    void printBack(void)
    {
        nio_printf("\b");
    }
    
    void clearConsole(void)
    {
        nio_clear(&console);
    }






#endif
