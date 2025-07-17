#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


extern int CODE_ERROR;

#include "headers.c"


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
        char* program=err_malloc(longueur+1);// crée le tableau de caractères qui va contenir le programme
    

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
            char* var=err_malloc(4001*sizeof(char)); // allocation d'un pointeur pour l'entrée de l'utilisateur (+1 char pour le caractère nul)
            err_memset(var,(char)0,4001*sizeof(char));//initialise le pointeur à '\0' partout
            //on effectue l'entrée
            printf("%s",text);
            
            int err = scanf("%4000[^\n]",var);
            
            cleanStdin();
            
            if (err!=1)
            {
                err_free(var);
                return strdup("");
            }
            
            //crée un deuxième pointeur pour y copier le contenu de l'entrée de la vraie longueur
            char* newVar = err_malloc(sizeof(char)*(strlen(var)+1));//réserve une place de la longueur de l'entrée + 1 pour le caractère nul
            
            void * ptrtest=strcpy(newVar,var);//copie de var dans newVar
            
            err_free(var);
            var=NULL;
                
            if (ptrtest==NULL)
            {
                err_free(newVar);
                CODE_ERROR = 66;
                return NULL;
            }
            
            return newVar;
        }
    #else
        char* input(char* text)
        {
            char* str = linenoise(text);
            // Ajout de la ligne à l'historique
            if (str != NULL)
                linenoiseHistoryAdd(str);

            return str;
        }
    #endif
    
    
    
    void flush(void) {
        fflush(stdin);
    }
    
    
    void printInt(int n)
    {
        printf("%d", n);
    }
    
    
    void printNumber(Number n)
    {
        number_aff(n);
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
        #ifndef WASM
            printf("\e[1;1H\e[2J");
        #else
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
            err_free(string);
            return NULL;
        }


        // transforme string en chaine de caractères
        char* ret = err_malloc(sizeof(char) * (string->len + 1));
        for (int i=0 ; i < string->len ; i++)
            ret[i] = string->data[i];

        ret[string->len] = '\0';

        err_free(string);

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

        
        char* program=err_malloc(longueur+1);// crée le tableau de caractères qui va contenir le programme
    

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
        ti_Write(content, sizeof(char), strlen(content), fichier)
        
        ti_Close(fichier);
    }


    char* input(char *text)
    {
      char* var=err_malloc(4001*sizeof(char)); // allocation d'un pointeur pour l'entrée de l'utilisateur (+1 char pour le caractère nul)
      err_memset(var,(char)0,4001*sizeof(char));//initialise le pointeur à '\0' partout
      //on effectue l'entrée
      nio_printf("%s",text);
    
      if (!nio_getsn(var, 4000))
      {
          CODE_ERROR = 1;
          return NULL;
      }
    
    
    
      //crée un deuxième pointeur pour y copier le contenu de l'entrée de la vraie longueur
      char* newVar = err_malloc(sizeof(char)*(strlen(var)+1));//réserve une place de la longueur de l'entrée + 1 pour le caractère nul
    
      void * ptrtest=strcpy(newVar,var);//copie de var dans newVar
    
      err_free(var);
      var=NULL;
    
      if (ptrtest==NULL)
      {
        err_free(newVar);
        CODE_ERROR = 66;
        return NULL;
      }
    
      return newVar;
    }
    
    
    void flush(void) {
        nio_fflush(&console);
    }

    
    void printInt(int n)
    {
        nio_printf("%d", n);
    }
    
    
    void printNumber(Number n)
    {
        number_aff(n);
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
