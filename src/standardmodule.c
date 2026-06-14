#define NEON_SOURCE_ID 1

#include <string.h>
#include <time.h>
#include <math.h>

#include "headers/nativefunctions.h"
#include "headers/constants.h"
#include "headers/neonio.h"
#include "headers/standardmodule.h"
#include "headers/dynarrays.h"
#include "headers/gc.h"
#include "headers/objects.h"
#include "headers/operators.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/serialization.h"
#include "headers/strings.h"
#include "headers/syntaxtrees.h"
#include "headers/neon.h"
#include "headers/trees.h"

#ifdef TI_EZ80
#include <sys/rtc.h>
#include <fileioc.h>
#endif

#ifdef LINUX
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#endif

#ifdef WINDOWS
#include <windows.h>
#include <stdio.h>
#endif


NeObj _print_(NeList* args)
{
    for (int i=0 ; i< args->len ; i++)
    {
        if (NEO_TYPE(ARG(i)) == TYPE_STRING)
            printString(neo_to_string(ARG(i)));
        else
            neobject_aff(ARG(i));
        printString(" ");
    }
    newLine();
    
    return neo_none_create();
}



NeObj _input_(NeList* args)
{

    #ifndef LINUX

        for (int i=0 ; i< args->len ; i++)
        {
            if (NEO_TYPE(ARG(i)) == TYPE_STRING)
                printString(neo_to_string(ARG(i)));
            else
                neobject_aff(ARG(i));
            
            
            if (i < args->len - 1)
                printString(" ");
        }

        char *entree = input("");

    #else
        // à cause de linenoise, il faut mettre tout le texte dans une seule chaine de caractères
        char* chaine = strdup("");

        for (int i=0 ; i< args->len ; i++)
        {
            if (NEO_TYPE(ARG(i)) == TYPE_STRING) {
                chaine = addStr2(chaine, neo_to_string(ARG(i)));
            }
            else {
                char* tempstring = neobject_str(ARG(i), true);
                chaine = addStr2(chaine,tempstring);
                neon_free(tempstring);
            }
            
            
            if (i < args->len - 1)
                chaine = addStr2(chaine, " ");
        }

        char* entree = input(chaine);
        neon_free(chaine);

        return_on_error(NEO_VOID);

    #endif
    return neo_str_create(entree);
}




NeObj _nbr_(NeList* args)
{
    char* string = neo_to_string(ARG(0));
    if (is_integer(string)) {
        return neo_integer_create(str_to_int(string));
    }
    else {
        double ret = str_to_double(string);
        return_on_error(NEO_VOID);
        return neo_double_create(ret);
    }
}



NeObj _str_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) == TYPE_STRING)
        return neo_copy(ARG(0));
    return neo_str_create(neobject_str(ARG(0), true));
}


NeObj _len_(NeList* args)
{
  NeObj retour;
    if (NEO_TYPE(ARG(0)) == TYPE_LIST)
    {
        retour = neo_integer_create(neo_list_len(ARG(0)));
    }
    else if (NEO_TYPE(ARG(0)) == TYPE_STRING)
    {
      retour = neo_integer_create(strlen(neo_to_string(ARG(0))));
    }
    else
    {
        neon_fail(4, neo_copy(ARG(0))); // cet objet n'a pas de longueur
        return NEO_VOID;
    }
  
    return retour;
  
}


NeObj _substring_(NeList* args)
{
    char* substring = sub(neo_to_string(ARG(0)),neo_to_integer(ARG(1)),neo_to_integer(ARG(2)));
    return neo_str_create(substring);
}



NeObj _exit_(NeList* args)
{
  neon_fail(1, NO_ARGS);
  return NEO_VOID;
}



NeObj _append_(NeList* args)
{
    neo_list_append(ARG(0), neo_copy(ARG(1)));
    return neo_none_create();
}







NeObj _remove_(NeList* args)
{
    neo_list_remove(ARG(0), neo_to_integer(ARG(1)));
    return neo_none_create();
}





NeObj _insert_(NeList* args)
{
    neo_list_insert(ARG(0), neo_copy(ARG(1)), neo_to_integer(ARG(2)));
    return neo_none_create();
}




NeObj _type_(NeList* args)
{
  char* retour = type(ARG(0));
  
  return neo_str_create(strdup(retour));
  
}


NeObj _reverse_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) == TYPE_STRING)
    {
        char* chaine = neo_to_string(ARG(0));
        int len = strlen(chaine);
        
        char* retour = neon_malloc((len+1)*sizeof(char));
        
        for (int i=0 ; i<len ; i++)
        {
            retour[i] = chaine[len-i-1];
        }
        retour[len] = '\0';
        
        return neo_str_create(retour);
    }
    else if (NEO_TYPE(ARG(0)) == TYPE_LIST) {
        NeList* list = neo_to_list(ARG(0));
        // création de la liste renversée
        NeList* reversed = nelist_reverse(list);
        // ajout de la liste au garbage collector
        return neo_list_convert(reversed);
    }
    else
    {
        neon_fail(62, neo_copy(ARG(0))); // unsupported types for reverse
        return NEO_VOID;
    }
}


NeObj _eval_(NeList* args)
{    
    char* exp = neo_to_string(ARG(0));

    char* sov = global_env->FILENAME;
    global_env->FILENAME = NULL;
    
    TreeBuffer tb = createExpressionTree(exp, false);

    if_error {
        neon_free(sov);
        return NEO_VOID;
    }

    NeObj res = tb_eval_aux(&tb);

    if_error
        neon_free(sov);
    else
        global_env->FILENAME = sov;
    
    TreeBuffer_destroy(&tb);
    return res;
}



NeObj _clear_(NeList* args)
{
    clearConsole();
    return neo_none_create();
}


NeObj _help_(NeList* args) {
    if (args->len == 0) { // affiche l'ensemble des fonctions disponibles
        setColor(GREEN);printString("Neon v");printString(VERSION);setColor(DEFAULT);printString(" for ");
        printString(PLATFORM);printString(", compiled on ");printString(__TIMESTAMP__);newLine();        
        printString("Visit ");setColor(GREEN); printString("https://langage-neon.raphaael.fr"); setColor(DEFAULT);
        printString(" for more information.");newLine();newLine();

        printString("Type help(\"variables\") to display all variables.");newLine();
        printString("Type help(\"modules\") to display all modules.");newLine();
        printString("Type help(\"gc\") to display all the objects tracked by the Garbage Collector");newLine();
        printString("Type help(\"YourModule\") to get help about module YourModule.");newLine();
        printString("Type help(myObject) to display information about object myObject.");newLine();


    }
    // affichage des variables disponibles
    else if (NEO_TYPE(ARG(0)) == TYPE_STRING && strcmp(neo_to_string(ARG(0)), "variables") == 0) { // affiche les variables existantes
        // je sais pas si c'est une bonne idée
        bool bo = false;
        printString("List of all existing variables :");
        newLine();
        for (int i = 0 ; i < global_env->NOMS->len ; i++) {
            if (NEO_TYPE(global_env->ADRESSES->tab[i]) != TYPE_EMPTY) {
                bo=true;
                printString(global_env->NOMS->tab[i]);printString(": ");setColor(GREEN);printString(type(global_env->ADRESSES->tab[i]));setColor(DEFAULT);
                newLine();
            }
        }
        // si on n'a rien affiché, on dit qu'on n'a rien trouvé
        if (!bo) {
            printString("No variable found");
            newLine();
        }
    }
    // affichage des modules disponibles
    else if (NEO_TYPE(ARG(0)) == TYPE_STRING && strcmp(neo_to_string(ARG(0)), "modules") == 0) {
        strlist* modules = get_all_modules();
        // affiche les modules trouvés
        printString("List of all existing modules :");
        newLine();
        for (int i=0 ; i < modules->len ; i++) {
            printString(modules->tab[i]);
            newLine();
        }
        if (modules->len == 0) {
            printString("No module found");
            newLine();
        }
        strlist_destroy(modules, true);
    }
    // affichage des objets du Garbage Collector
    else if (NEO_TYPE(ARG(0)) == TYPE_STRING && strcmp(neo_to_string(ARG(0)), "gc") == 0) {
        print_objects_list();
    }
    // aide à propos d'objets en général
    else if (args->len >= 1) {

        for (int i = 0 ; i < args->len ; i++) { // boucle pour afficher l'aide de chaque objet

            if (NEO_TYPE(ARG(i)) == TYPE_USERFUNC)
            {
                // type userfunc
                UserFunc* fun = neo_to_userfunc(ARG(i));

                if (neo_isMethod(ARG(i)))
                    printString("method ");
                else
                    printString("function ");

                // La raison pour laquelle on n'utilise pas la soft-comparaison entre la fonction et les variables de ADRESSES
                // est que l'opérateur d'égalité peut être surchargé donc peut donner de faux résultats ici
                int index = nelist_index(global_env->ADRESSES, ARG(i));
                if (index == -1) {
                    printString("<anonymous>");
                    neon_reset_error();
                }
                else {
                    printString(global_env->NOMS->tab[index]);
                }

                printString("(");

                bool bo = false;

                for (int i = 0 ; i < fun->nbArgs ; i++)
                {
                    if (fun->unlimited_arguments && i == fun->nbArgs - fun->nbOptArgs && !bo) {
                        printString("...");
                        i --;
                        bo = true;
                    }
                    else {
                        // affichage de la variable fun->args[i]
                        char* nom = get_name(fun->args[i]);
                        printString(nom);

                        if (!neo_is_void(nelist_nth(&fun->opt_args, i))) // expression non vide
                        {
                            printString(" := ");
                            neobject_aff(nelist_nth(&fun->opt_args, i));
                        }
                    }

                    if (i < fun->nbArgs - 1)
                        printString(", ");
                }
                printString(")");
                newLine();

                if (fun->doc != NULL)
                    printString(fun->doc);
                else
                    printString("No documentation available for this function.");
                newLine();
            }
            else if (NEO_TYPE(ARG(i)) == TYPE_BUILTINFUNC)
            {
                Function* fun = neo_to_function(ARG(i));
                printString("Built-in function ");

                // Affichage de la signature

                setColor(BLUE);
                printString((char*)get_function_name(fun->id, fun->module));
                setColor(DEFAULT);

                printString("(");
                
                setColor(GREEN);
                if (fun->nbArgs == -1) {
                    printString("...");
                }
                else
                {
                    for (int i=0 ; i < fun->nbArgs ; i++) {
                        if (fun->typeArgs[i] == TYPE_UNSPECIFIED) {
                            printString("Any");
                        }
                        else {
                            NeObj neo = (NeObj) {.type = fun->typeArgs[i]};
                            printString(type(neo));
                        }

                        if (i + 1 < fun->nbArgs) {
                            setColor(DEFAULT);
                            printString(", ");
                            setColor(GREEN);
                        }
                    }
                }
                setColor(DEFAULT);
                printString(")");
                printString(" -> ");

                setColor(GREEN);

                if (fun->typeRetour == TYPE_NONE) {
                    printString("None");
                }
                else if (fun->typeRetour == TYPE_UNSPECIFIED) {
                    printString("Any");
                }
                else {
                    NeObj neo = (NeObj) {.type = fun->typeRetour};
                    printString(type(neo));
                }
                setColor(DEFAULT);
                
                
                newLine();
                
                if (fun->nbArgs == -1)
                {
                    printString("Illimited number of arguments of type : ");

                    setColor(GREEN);
                    NeObj neo = (NeObj) {.type = fun->typeArgs[0]};
                    printString(type(neo));
                    setColor(DEFAULT);
                }
                else
                {
                    printString("Needs ");
                    printInt(fun->nbArgs);
                    if (fun->nbArgs == 1)
                        printString(" argument");
                    else
                        printString(" arguments");

                    if (fun->nbArgs > 0) {
                        printString(" of type : ");
                        for (int i = 0 ; i < fun->nbArgs ; i++)
                        {
                            setColor(GREEN);
                            NeObj neo = (NeObj) {.type = fun->typeArgs[i]};
                            printString(type(neo));
                            setColor(DEFAULT);

                            if (i + 1 < fun->nbArgs)
                                printString(", ");
                        }
                    }
                    
                }

                printString(".");
                newLine();

                printString("The returned type is : ");

                setColor(GREEN);
                NeObj neo = (NeObj) {.type = fun->typeRetour};
                printString(type(neo));
                setColor(DEFAULT);

                printString(".");
                newLine();
                
                if (fun->help == NULL)
                    printString("No help available for this function.");
                else
                {
                    char* temp = strdup(fun->help); // c'est pas beau, c'est juste pour ne pas avoir le warning du compilateur
                    printString(temp);
                    neon_free(temp);
                    newLine();
                }
            }
            // aide à propos d'un module
            else if (NEO_TYPE(ARG(i)) == TYPE_STRING && is_module(neo_to_string(ARG(0)))) {
                printString("Module ");printString(neo_to_string(ARG(0)));printString(" :");newLine();
                for (int i=0 ; i < global_env->NOMS->len ; i++) {
                    if (has_strict_prefix(global_env->NOMS->tab[i], neo_to_string(ARG(0))) && NEO_TYPE(global_env->ADRESSES->tab[i]) != TYPE_EMPTY) {
                        printString(global_env->NOMS->tab[i]);printString(": ");
                        setColor(GREEN);printString(type(global_env->ADRESSES->tab[i]));setColor(DEFAULT);newLine();
                    }
                }
            }
            // aide à propos d'un type de container
            else if (NEO_TYPE(ARG(i)) == TYPE_STRING && strlist_inList(global_env->CONTAINERS, neo_to_string(ARG(i)))) {
                int index = strlist_index(global_env->CONTAINERS, neo_to_string(ARG(i)));

                printString(neo_to_string(ARG(i))); printString(" is a container type."); 
                newLine();

                printString("The required fields are : ");

                NeList* fields = neo_to_list(global_env->ATTRIBUTES->tab[index]);
                for (int j = 0 ; j < fields->len - 1 ; j++) {
                    printString(neo_to_string(fields->tab[j]));
                    printString(", ");
                }
                printString(neo_to_string(fields->tab[fields->len - 1]));

                newLine();
            }
            // aide à propos d'un objet quelconque
            else {
                printString("Object ");
                neobject_aff(ARG(i));
                printString(" of type ");
                printString(type(ARG(i)));
                newLine();
            }

            // entre les différentes aides
            if (i < args->len - 1) {
                printString("---");
                newLine();
            }
        }
    }

    return neo_none_create();
}




NeObj _randint_(NeList* args)
{
    return neo_integer_create(randint(neo_to_integer(ARG(0)), neo_to_integer(ARG(1))));
}


NeObj _failwith_(NeList* args)
{
    setColor(PURPLE);
    printString(" A fatal error occured");
    
    setColor(DEFAULT);
    printString(" in ");
    printErrSource(global_env->FILENAME, global_env->LINENUMBER);

    setColor(PURPLE);
    printString(" # ");

    setColor(RED);
    printString(neo_to_string(ARG(0)));
    newLine();

    // Exiting immediately
    setColor(PURPLE);
    printString(" Exiting immediately");
    newLine();

    setColor(DEFAULT);

    neon_fail(1, NO_ARGS);
    return NEO_VOID;
}



NeObj _time_(NeList* args)
{
    #ifndef TI_EZ80
        time_t temps = time(NULL);
        double res = (double)temps;
        return neo_integer_create(res);
    #else
        intptr_t t = rtc_Time();
        return neo_integer_create(t);
    #endif
}

NeObj _assert_(NeList* args)
{
    for (int i = 0 ; i < args->len ; i++)
    {
        if (!neo_is_true(ARG(i)))
        {
            neon_fail(71, NO_ARGS);
            return NEO_VOID;
        }
    }
    return neo_none_create();
}



NeObj _output_(NeList* args)
{
    for (int i=0 ; i< args->len ; i++)
    {
        if (NEO_TYPE(ARG(i)) == TYPE_STRING)
            printString(neo_to_string(ARG(i)));
        else
            neobject_aff(ARG(i));
    }
    return neo_none_create();
}


NeObj _chr_(NeList* args)
{
    int n = neo_to_integer(ARG(0));
    char* c = neon_malloc(sizeof(char) * 2);
    c[0] = (char)n;
    c[1] = '\0';
    return neo_str_create(c);
}

NeObj _ord_(NeList* args)
{
    char* c = neo_to_string(ARG(0));
    int n = c[0];

    return neo_integer_create(n);
}



NeObj _list_comp_(NeList* args)
{

    char* sov_FILENAME = global_env->FILENAME;
    global_env->FILENAME = NULL;

    /* args : indice, debut, fin, pas, condition, valeur */
    char* nom = neo_to_string(ARG(0));

    int index;
    NeObj* i;

    if(!strlist_inList(global_env->NOMS, nom))
    {
        strlist_append(global_env->NOMS, strdup(nom));
        nelist_append(global_env->ADRESSES, neo_empty_create());
        index = global_env->ADRESSES->len - 1;
    }
    else
    {
        index = get_var(nom);
    }

    i = get_absolute_address(index);

    if (NEO_TYPE(ARG(1)) != TYPE_INTEGER || NEO_TYPE(ARG(2)) != TYPE_INTEGER || NEO_TYPE(ARG(3)) != TYPE_INTEGER) {
        if (sov_FILENAME != NULL)
            neon_free(sov_FILENAME);
        neon_fail(112, NO_ARGS);
        return NEO_VOID;
    }
    
    _affect2(i, ARG(1)); // valeur de debut

    // un premier TreeBuffer contenant l'expression de la condition
    TreeBuffer cond = createExpressionTree(neo_to_string(ARG(4)), false);

    if_error {
        TreeBuffer_destroy(&cond);
        if (sov_FILENAME != NULL)
            neon_free(sov_FILENAME);
        return NEO_VOID;
    }

    // un second TreeBuffer contenant l'expression de la valeur
    TreeBuffer val = createExpressionTree(neo_to_string(ARG(5)), false);

    if_error {
        TreeBuffer_destroy(&cond);
        if (sov_FILENAME != NULL)
            neon_free(sov_FILENAME);
        return NEO_VOID;
    }

    NeObj liste = neo_list_create(0);
    NeObj x = NEO_VOID;

    while (neo_to_integer(*i) < neo_to_integer(ARG(2)))
    {
        NeObj bo = tb_eval_aux(&cond);

        if_error {
            TreeBuffer_destroy(&cond);
            TreeBuffer_destroy(&val);
            neobject_destroy(liste);
            if (sov_FILENAME != NULL)
                neon_free(sov_FILENAME);
            return NEO_VOID;
        }


        if (neoIsTrue(bo)) {
            NeObj neo = tb_eval_aux(&val);
            neo_list_append(liste, neo);

            if_error {
                neobject_destroy(bo);
                neobject_destroy(liste);
                TreeBuffer_destroy(&cond);
                TreeBuffer_destroy(&val);
                if (sov_FILENAME != NULL)
                    neon_free(sov_FILENAME);
                return NEO_VOID;
            }
        }

        neobject_destroy(bo);

        // incrémentation de la variable d'indice
        x = _add(*i, ARG(3));

        neobject_destroy(*i);
        *i = x;
    }

    TreeBuffer_destroy(&cond);
    TreeBuffer_destroy(&val);

    global_env->FILENAME = sov_FILENAME;
    
    return liste;
}

/*
Voici comment fonctionnent les exceptions utilisateur
Une exception est un objet qui contient un entier correspondant un numéro d'exception, à l'indice de l'exception
dans le tableau exceptions
Quand une exception utilisateur est lancée, global_env->CODE_ERROR est mis à la valeur négative de l'indice de l'exception,
et EXCEPTION est mis au message d'erreur
De cette manière si global_env->CODE_ERROR est négatif, printError sait que c'est une exception lancée par l'utilisateur
*/
NeObj _create_exception_(NeList* args)
{
    strlist_append(global_env->EXCEPTIONS, strdup(neo_to_string(ARG(0))));
    NeObj e = neo_exception_create(global_env->EXCEPTIONS->len - 1);

    if (strlist_inList(global_env->NOMS,neo_to_string(ARG(0))))
    {
        int index = strlist_index(global_env->NOMS,neo_to_string(ARG(0)));
        _affect2(&global_env->ADRESSES->tab[index], e);
        neobject_destroy(e);
    }
    else
    {
        strlist_append(global_env->NOMS,strdup(neo_to_string(ARG(0)))); // on ajoute le nom à la liste des global_env->NOMS
        nelist_append(global_env->ADRESSES,e); // on ajoute l'objet à la liste des variables
    }

    return neo_none_create();
}


NeObj _raise_(NeList* args) {
    if (NEO_TYPE(ARG(0)) != TYPE_EXCEPTION || NEO_TYPE(ARG(1)) != TYPE_STRING) {
        neon_fail(38, neo_new_const_create("raise"), neo_new_const_create("raise"));
        return NEO_VOID;
    }

    NeList* args_list = nelist_create(args->len - 2);
    for (int i=2 ; i < args->len ; i++) {
        args_list->tab[i-2] = neo_copy(ARG(i));
    }

    neon_raise_user_exception(
        get_exception_code(ARG(0)),
        neo_to_string(ARG(1)),
        args_list
    );
    return NEO_VOID;
}


NeObj _int_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) == TYPE_DOUBLE || NEO_TYPE(ARG(0)) == TYPE_INTEGER) {
        double x = neo_to_double(ARG(0));
        return neo_integer_create(x);
    }
    else if (NEO_TYPE(ARG(0)) == TYPE_STRING && is_integer(neo_to_string(ARG(0))))
        return neo_integer_create(str_to_int(neo_to_string(ARG(0))));
    else {
        neon_fail(113, neo_copy(ARG(0)));
        return NEO_VOID;
    }
    
}


NeObj _index_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) == TYPE_STRING) {
        if (NEO_TYPE(ARG(1)) != TYPE_STRING) {
            neon_fail(121, neo_copy(ARG(1)), neo_copy(ARG(0)));
            return NEO_VOID;
        }
        char* string = neo_to_string(ARG(0));
        char* search = neo_to_string(ARG(1));
        int index = string_index(string, search);
        if (index == -1) {
            neon_fail(88, neo_copy(ARG(1)), neo_copy(ARG(0)));
            return NEO_VOID;
        }
        else {
            return neo_integer_create(index);
        }
    }
    else if (NEO_TYPE(ARG(0)) == TYPE_LIST) {
        int i = nelist_index2(neo_to_list(ARG(0)), ARG(1));

        if (i >= 0)
            return neo_integer_create(i);
        
        return NEO_VOID;
    }
    else {
        neon_fail(121, neo_copy(ARG(1)), neo_copy(ARG(0)));
        return NEO_VOID;
    }
}


NeObj _replace_(NeList* args)
{
    char* str = replace(
        neo_to_string(ARG(0)),
        neo_to_string(ARG(1)),
        neo_to_string(ARG(2))
    );

    return neo_str_create(str);
}


NeObj _count_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) == TYPE_LIST)
    {
        int count = 0;
        NeList* l = neo_to_list(ARG(0));
        for (int i = 0; i < l->len ; i++)
        {
            if (neo_equal(ARG(1), l->tab[i]))
                count++;
        }
        return neo_integer_create(count);
    }
    else if (NEO_TYPE(ARG(0)) == TYPE_STRING)
    {
        return neo_integer_create(count(
            neo_to_string(ARG(0)),
            neo_to_string(ARG(1))
        ));
    }
    else
    {
        neon_fail(38, neo_new_const_create("count"), neo_new_const_create("count"));
        return NEO_VOID;
    }
}


NeObj _list_(NeList* args)
{
    char* s = neo_to_string(ARG(0));
    NeList* l = nelist_create(strlen(s));
    for (int i=0 ; i < l->len ; i++)
        l->tab[i] = neo_str_create(charToString(s[i]));
    
    return neo_list_convert(l);
}




void swap(NeObj* a, NeObj* b) {
    NeObj tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

// Fonction pour partitionner le tableau en utilisant le dernier élément comme pivot
int partition(NeObj* arr, int low, int high, int (*compare) (NeObj,NeObj)) {
    NeObj pivot = arr[high];  // Choix du pivot
    int i = (low - 1);  // Index du plus petit élément

    for (int j = low; j <= high - 1; j++) {
        // Si l'élément courant est plus petit que le pivot
        if (compare(arr[j], pivot) < 0) {
            i++;  // Incrémenter l'index du plus petit élément
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}



// Fonction principale de tri rapide
void quickSort(NeObj arr[], int low, int high, int (*compare) (NeObj,NeObj)) {
    if (low < high) {
        // Obtention de l'index de partition
        int pi = partition(arr, low, high, compare);

        // Trier les éléments avant et après la partition
        quickSort(arr, low, pi - 1, compare);
        quickSort(arr, pi + 1, high, compare);
    }
}


NeObj _sort_asc_(NeList* args)
{
    NeList* l = neo_to_list(ARG(0));

    quickSort(l->tab, 0, l->len - 1, neo_compare);

    return_on_error(NEO_VOID);
    
    return neo_none_create();
}



NeObj _sort_desc_(NeList* args)
{
    NeList* l = neo_to_list(ARG(0));

    quickSort(l->tab, 0, l->len - 1, neo_compare2);

    return_on_error(NEO_VOID);
    
    return neo_none_create();
}


NeObj _sin_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    return neo_double_create(sin(neo_to_double(ARG(0))));
}


NeObj _cos_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    return neo_double_create(cos(neo_to_double(ARG(0))));
}


NeObj _tan_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    return neo_double_create(tan(neo_to_double(ARG(0))));
}


NeObj _deg_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    double angle = neo_to_double(ARG(0));
    return neo_double_create(angle/PI*180.f);
}


NeObj _rad_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    double angle = neo_to_double(ARG(0));
    return neo_double_create(angle/180.f*PI);
}


NeObj _sqrt_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    return neo_double_create(sqrt(neo_to_double(ARG(0))));
}


NeObj _ln_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    return neo_double_create(log(neo_to_double(ARG(0))));;
}


NeObj _exp_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    return neo_double_create(exp(neo_to_double(ARG(0))));
}


NeObj _log_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    return neo_double_create(log10(neo_to_double(ARG(0))));
}


NeObj _log2_(NeList* args)
{
    if (!is_number(ARG(0))) {
        neon_fail(114, neo_new_str_create(type(ARG(0))));
        return NEO_VOID;
    }
    return neo_double_create(log2(neo_to_double(ARG(0))));
}


NeObj _round_(NeList* args)
{
    double x = neo_to_double(ARG(0));
    x *= pow(10, (double)neo_to_integer(ARG(1)));
    return neo_double_create((int)x);
}


NeObj _abs_(NeList* args)
{
    double x = neo_to_double(ARG(0));
    if (x < 0)
        x = -x;
    return neo_double_create(x);
}


NeObj _ceil_(NeList* args)
{
    return neo_double_create(ceil(neo_to_double(ARG(0))));
}


NeObj _floor_(NeList* args)
{
    return neo_double_create(floor(neo_to_double(ARG(0))));
}



NeObj _readFile_(NeList* args)
{
    char* res = openFile(neo_to_string(ARG(0)));

    return_on_error(NEO_VOID);
    
    return neo_str_create(res);
}



NeObj _writeFile_(NeList* args)
{
    writeFile(neo_to_string(ARG(0)),neo_to_string(ARG(1)));

    return_on_error(NEO_VOID);
    
    return neo_none_create();
}


NeObj _setFunctionDoc_(NeList* args) {
    UserFunc* fun = neo_to_userfunc(ARG(0));

    if (fun->doc != NULL)
        neon_free(fun->doc);

    fun->doc = strdup(neo_to_string(ARG(1)));
    return neo_none_create();
}


NeObj _setAtomicTime_(NeList* args) {
    if (neo_to_integer(ARG(0)) < 1) {
        neon_fail(102, neo_copy(ARG(0)));
        return NEO_VOID;
    }

    global_env->ATOMIC_TIME = neo_to_integer(ARG(0));
    global_env->atomic_counter = 0;
    return neo_none_create();
}

NeObj _copy_(NeList* args) {
    NeObj copy = neo_deepcopy(ARG(0));
    return copy;
}



NeObj _load_namespace_(NeList* args) {
    char* prefix = neo_to_string(ARG(0));
    for (int i=0 ; i < global_env->NOMS->len ; i++) {
        if (has_strict_prefix(global_env->NOMS->tab[i], prefix)) {
            char* without_prefix = remove_prefix(global_env->NOMS->tab[i], prefix);

            if (strlist_inList(global_env->NOMS, without_prefix)) {
                _affect2(&global_env->ADRESSES->tab[strlist_index(global_env->NOMS, without_prefix)], global_env->ADRESSES->tab[i]);
                neon_free(without_prefix);
            }
            else {
                strlist_append(global_env->NOMS, without_prefix);
                nelist_append(global_env->ADRESSES, neo_copy(global_env->ADRESSES->tab[i]));
            }
        }
    }
    return neo_none_create();
}

NeObj _gc_(NeList* args) {
    gc_mark_and_sweep();
    return neo_none_create();
}


NeObj _setColor_(NeList* args) {
    char* color = neo_to_string(ARG(0));

    if (strcmp(color, "blue") == 0) {
        setColor(BLUE);
    }
    else if (strcmp(color, "default") == 0) {
        setColor(DEFAULT);
    }
    else if (strcmp(color, "green") == 0) {
        setColor(GREEN);
    }
    else if (strcmp(color, "red") == 0) {
        setColor(RED);
    }
    else if (strcmp(color, "purple") == 0) {
        setColor(PURPLE);
    }
    else {
        neon_fail(63, neo_new_str_create(color));
    }
    return neo_none_create();
}


NeObj _init_(NeList* args) {
    for (int i=0 ; i < args->len ; i++) {
        char* moduleName = neo_to_string(ARG(i));

        if (strcmp(moduleName, "graphics") == 0) {
            init_module(GraphicModule, global_env);
        }
        else {
            neon_fail(31, neo_new_str_create(moduleName));
            return NEO_VOID;
        }
    }
    return neo_none_create();
}



// détecte les fichiers commençant par une certaine chaîne de caractères
NeObj _detectFiles_(NeList* args) {
    NeObj files = neo_list_create(0);
    char* prefix = neo_to_string(ARG(0));

    #if defined(TI_EZ80)

    void* vat_ptr = NULL;
    char* var_name;

    while ((var_name = ti_Detect(&vat_ptr, prefix))) {
        neo_list_append(files, neo_str_create(strdup(var_name)));
    }

    #elif defined(LINUX)

    // this code mostly comes from ChatGPT
    int prefix_len = strlen(prefix);
    char* buffer = neon_malloc(prefix_len);

    DIR *rep;
    struct dirent *entree;
    struct stat infos;

    rep = opendir(".");
    if (rep == NULL) {
        neon_free(buffer);
        neon_fail(119, NO_ARGS);
        return NEO_VOID;
    }

    while ((entree = readdir(rep)) != NULL) {
        // Ignore "." et ".."
        if (strcmp(entree->d_name, ".") == 0 || strcmp(entree->d_name, "..") == 0)
            continue;

        // Obtenir les informations sur le fichier
        if (stat(entree->d_name, &infos) == 0) {
            if (S_ISREG(infos.st_mode)) {
                // on ouvre le fichier pour vérifier le préfixe
                FILE* file = fopen(entree->d_name, "r");

                if (file == NULL) {
                    neon_fail(119, NO_ARGS);
                    neobject_destroy(files);
                    neon_free(buffer);
                    return NEO_VOID;
                }
                fread(buffer, 1, prefix_len, file);

                if (strncmp(buffer, prefix, prefix_len) == 0)
                    neo_list_append(files, neo_str_create(strdup(entree->d_name)));

                fclose(file);
            }
        } else {
            neobject_destroy(files);
            neon_free(buffer);
            neon_fail(119, NO_ARGS);
            return NEO_VOID;
        }
    }

    closedir(rep);
    neon_free(buffer);

    #elif defined(WINDOWS)
    int prefix_len = strlen(prefix);
    char* buffer = neon_malloc(prefix_len);

    WIN32_FIND_DATA fichier;
    HANDLE handle = FindFirstFile(".\\*", &fichier);

    if (handle == INVALID_HANDLE_VALUE) {
        neon_fail(119, NO_ARGS);
        neobject_destroy(files);
        neon_free(buffer);
        return NEO_VOID;
    }

    do {
        if (strcmp(fichier.cFileName, ".") != 0 && strcmp(fichier.cFileName, "..") != 0) {

            // on ouvre le fichier pour vérifier le préfixe
            FILE* file = fopen(fichier.cFileName, "r");

            if (file == NULL) {
                neon_fail(119, NO_ARGS);
                neobject_destroy(files);
                neon_free(buffer);
                return NEO_VOID;
            }
            fread(buffer, 1, prefix_len, file);

            if (strncmp(buffer, prefix, prefix_len) == 0)
                neo_list_append(files, neo_str_create(strdup(fichier.cFileName)));

            fclose(file);
        }
    } while (FindNextFile(handle, &fichier));

    FindClose(handle);

    neon_free(buffer);

    #endif

    return files;
}


NeObj _safeExec_(NeList* args) {
    NeonEnv* global_env_sov = NeonEnv_set(NeonEnv_init());

    // définition de la liste des arguments
    update__name__(strdup(neo_to_string(ARG(0)))); // nom du fichier actuel
    variable_append(global_env, "__args__", neo_copy(ARG(1)));

    execFile(neo_to_string(ARG(0)));

    NeonEnv_destroy(global_env);
    NeonEnv_set(global_env_sov);

    return neo_none_create();
}

NeObj _bin_(NeList* args) {
    return neo_str_create(decToBin(neo_to_integer(ARG(0))));
}

NeObj _hex_(NeList* args) {
    return neo_str_create(decToHex(neo_to_integer(ARG(0))));
}

NeObj _serialize_(NeList* args) {
    NeStream stream = NeStream_open(neo_to_string(ARG(0)), "w+");
    return_on_error(NEO_VOID);
    neobject_serialize(stream, ARG(1));
    NeStream_close(stream);
    return neo_none_create();
}


NeObj _deserialize_(NeList* args) {
    NeStream stream = NeStream_open(neo_to_string(ARG(0)), "r");
    return_on_error(NEO_VOID);
    NeObj neo = neobject_deserialize(stream);
    NeStream_close(stream);
    return_on_error(NEO_VOID);
    return neo;
}


NeObj _format_(NeList* args) {
    if (args->len < 1) {
        neon_fail(38, neo_new_const_create("format"), neo_new_const_create("format"));
        return NEO_VOID;
    }
    else if (NEO_TYPE(ARG(0)) != TYPE_STRING) {
        neon_fail(49, neo_copy(ARG(0)));
        return NEO_VOID;
    }

    char* format = neo_to_string(ARG(0));

    int format_length = strlen(format);
    int specifier_length = strlen(FORMAT_ARGUMENT_SPECIFIER);

    // Précalcul de tous les arguments et calcul d'un majorant de la longueur totale de la nouvelle chaîne
    int cumulated_length = format_length;
    strlist* arguments = strlist_create(args->len - 1);
    for (int i=1 ; i < args->len ; i++) {
        if (NEO_TYPE(ARG(i)) == TYPE_STRING)
            arguments->tab[i-1] = strdup(neo_to_string(ARG(i)));
        else
            arguments->tab[i-1] = neobject_str(ARG(i), true);

        cumulated_length += strlen(arguments->tab[i-1]);
    }

    // Écriture des arguments et du reste de la chaîne de format directement dans la chaîne finale
    char* final_string = neon_malloc(sizeof(char) * (cumulated_length + 1));
    int final_string_index = 0;

    int argument_index = 0;

    for (int i = 0 ; i < format_length ; i++) {
        // Identificateur d'argument
        if (i < format_length - specifier_length + 1 && strncmp(format + i, FORMAT_ARGUMENT_SPECIFIER, specifier_length) == 0) {
            // Check if we stilla have enough have arguments
            if (argument_index == arguments->len) {
                neon_fail(48, neo_integer_create(arguments->len), neo_integer_create(argument_index + 1));
                strlist_destroy(arguments, true);
                neon_free(final_string);
                return neo_none_create();
            }

            // Recopie de l'argument n°argument_index dans la chaîne finale
            for (int j=0 ; arguments->tab[argument_index][j] != '\0' ; j++)
                final_string[final_string_index++] = arguments->tab[argument_index][j];

            // On a consommé un argument
            argument_index += 1;

            // On passe le deuxième pourcentage
            i += specifier_length - 1;
        }
        else {
            final_string[final_string_index++] = format[i];
        }
    }
    final_string[final_string_index] = '\0';

    strlist_destroy(arguments, true);


    if (argument_index < args->len - 1) {
        neon_free(final_string);
        neon_fail(47, neo_integer_create(argument_index), neo_integer_create(args->len-1));
        return neo_none_create();
    }


    return neo_str_create(final_string);
}


NeObj _hash_(NeList* args) {
    return neo_integer_create(neo_hash(ARG(0)));
}



NeObj (*builtinfunctions_pointers[NBBUILTINFUNC])(NeList*) = {
    _print_,
    _input_,
    _nbr_,
    _str_,
    _len_,
    _substring_,
    _exit_,
    _append_,
    _remove_,
    _insert_,
    _type_,
    _reverse_,
    _eval_,
    _clear_,
    _help_,
    _randint_,
    _failwith_,
    _time_,
    _assert_,
    _output_,
    _chr_,
    _ord_,
    _list_comp_,
    _create_exception_,
    _raise_,
    _int_,
    _index_,
    _replace_,
    _count_,
    _list_,
    _sort_asc_,
    _sort_desc_,
    _sin_,
    _cos_,
    _tan_,
    _deg_,
    _rad_,
    _sqrt_,
    _ln_,
    _exp_,
    _log_,
    _log2_,
    _round_,
    _abs_,
    _ceil_,
    _floor_,
    _readFile_,
    _writeFile_,
    _setFunctionDoc_,
    _setAtomicTime_,
    _copy_,
    _load_namespace_,
    _gc_,
    _setColor_,
    _init_,
    _detectFiles_,
    _safeExec_,
    _bin_,
    _hex_,
    _serialize_,
    _deserialize_,
    _format_,
    _hash_
};



const char* const builtinfunctions_names[] = {
    "print",
    "input",
    "nbr",
    "str",
    "len",
    "sub",
    "exit",
    "append",
    "remove",
    "insert",
    "type",
    "reverse",
    "eval",
    "clear",
    "help",
    "randint",
    "failwith",
    "time",
    "assert",
    "output",
    "chr",
    "ord",
    "listComp",
    "createException",
    "raise",
    "int",
    "index",
    "replace",
    "count",
    "list",
    "sortAsc",
    "sortDesc",
    "sin",
    "cos",
    "tan",
    "deg",
    "rad",
    "sqrt",
    "ln",
    "exp",
    "log",
    "log2",
    "round",
    "abs",
    "ceil",
    "floor",
    "readFile",
    "writeFile",
    "setFunctionDoc",
    "setAtomicTime",
    "copy",
    "loadNamespace",
    "gc",
    "setColor",
    "init",
    "detectFiles",
    "safeExec",
    "bin",
    "hex",
    "serialize",
    "deserialize",
    "format",
    "hash"
};



static const Function builtinfunctions[] = {
    (Function) {
        .help = "Displays arguments in the terminal",
        .nbArgs = -1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Displays the given argument and prompts the user to input in the terminal. Returns the entered value.",
        .nbArgs = -1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_STRING
    },
    (Function) {
        .help = "Converts a string into a number.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_UNSPECIFIED
    },
    (Function) {
        .help = "Converts any object into a string.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_STRING
    },
    (Function) {
        .help = "Returns the length of an object.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "Performs substring extraction. sub(\"hello\", 1, 4) returns \"ell\".",
        .nbArgs = 3,
        .typeArgs = (int[]){TYPE_STRING, TYPE_INTEGER, TYPE_INTEGER},
        .typeRetour = TYPE_STRING
    },
    (Function) {
        .help = "Exits the Neon interpreter.",
        .nbArgs = 0,
        .typeArgs = NULL,
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Adds an element to a list.",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_LIST, TYPE_UNSPECIFIED},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Removes an element from a list.",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_LIST, TYPE_INTEGER},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Inserts an element into a list.",
        .nbArgs = 3,
        .typeArgs = (int[]){TYPE_LIST, TYPE_UNSPECIFIED, TYPE_INTEGER},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Returns the type of an object.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_STRING
    },
    (Function) {
        .help = "Reverses the characters in a string or reverses a list. Returns the reversed object without modifying the original one",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_UNSPECIFIED
    },
    (Function) {
        .help = "Evaluates the given string of characters and returns the result.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_STRING
    },
    (Function) {
        .help = "Clears the terminal.",
        .nbArgs = 0,
        .typeArgs = NULL,
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "This function displays all kinds of information on objects and modules.\nType help() for more information.",
        .nbArgs = -1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Returns a random integer between two integers.",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_INTEGER, TYPE_INTEGER},
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "Displays the provided text in the terminal and exits the program.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Returns the time elapsed since 1970 in seconds.",
        .nbArgs = 0,
        .typeArgs = NULL,
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "Stops the program if at least 1 assertion has failed.",
        .nbArgs = -1,
        .typeArgs = (int[]){TYPE_BOOL},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Puts the given arguments on the terminal without spaces or \\n.",
        .nbArgs = -1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Returns the ASCII character associated to an integer.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_INTEGER},
        .typeRetour = TYPE_STRING
    },
    (Function) {
        .help = "Returns the ASCII value of the given character.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "Arguments : index, start, end, step, condition, value\nEvaluates value for all indices between start and end according to step, and adds it to the list if condition is true.\nindex must be the name of the variable used, and condition and value must be string expressions.",
        .nbArgs = 6,
        .typeArgs = (int[]){TYPE_STRING, TYPE_INTEGER, TYPE_INTEGER, TYPE_INTEGER, TYPE_STRING, TYPE_STRING},
        .typeRetour = TYPE_LIST
    },
    (Function) {
        .help = "Creates an Exception object with the given name.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_EXCEPTION
    },
    (Function) {
        .help = "The function needs at least 2 paramteters :\n - The first parameter is an object of type Exception\n - The second parameter is a string. Every \"<>\" in that string will be replaced by the arguments after, similarly to the behavior of format()\nExample: raise(DivisionByZero, \"You tried to divide <> by <>\", 5, 0) will raise the exception DivisionByZero and display \"You tried to divide 5 by 0\"",
        .nbArgs = -1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Converts an object into an integer",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "Returns the index of an element in a list or the index of a string in another string",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_UNSPECIFIED, TYPE_UNSPECIFIED},
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "replace(str, str1, str2) returns a string in which all occurrences of str1 have been replaced by str2 in str.",
        .nbArgs = 3,
        .typeArgs = (int[]){TYPE_STRING, TYPE_STRING, TYPE_STRING},
        .typeRetour = TYPE_STRING
    },
    (Function) {
        .help = "Counts the number of occurrences in a list or a string.",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_UNSPECIFIED, TYPE_UNSPECIFIED},
        .typeRetour = TYPE_INTEGER
    },
    (Function) {
        .help = "Separates every character of a string in a list.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_LIST
    },
    (Function) {
        .help = "Sorts a list in ascending order.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_LIST},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Sorts a list in descending order.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_LIST},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "sin: Sine function",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "cos: Cosine function",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "tan: Tangent function",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "deg: Convert angle from radians to degrees",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "rad: Convert angle from degrees to radians",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "sqrt: Square root function",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "ln: Natural logarithm (base e)",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "exp: Exponential function (e^x)",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "log: Logarithm (base 10)",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "log2: Logarithm (base 2)",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "round: Round to the nearest real number with the given precision",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_DOUBLE, TYPE_INTEGER},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "abs: Absolute value",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_DOUBLE},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "ceil: Ceiling function (smallest integer greater than or equal to)",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_DOUBLE},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "floor: Floor function (largest integer less than or equal to)",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_DOUBLE},
        .typeRetour = TYPE_DOUBLE
    },
    (Function) {
        .help = "Returns the content of the file whose name was given",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_STRING
    },
    (Function) {
        .help = "Writes the given string in the file whose name was given. The syntax is writeFile(name, content)",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_STRING, TYPE_STRING},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Sets a string documentation for a user-defined function or method",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_USERFUNC, TYPE_STRING},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Time to allow for each process before switching",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_INTEGER},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Performs a deep copy of an object, preserving the pointer dependencies",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_UNSPECIFIED
    },
    (Function) {
        .help = "Loads any variable from a given namespace without the namespace prefix",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Calls the Garbage Collector",
        .nbArgs = 0,
        .typeArgs = NULL,
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Changes the writing text color in console if available.\nColors: red, green, blue and white",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Initializes a native module",
        .nbArgs = -1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_NONE
    },
    (Function) {
        .help = "Returns a list of all AppVars starting by a specific string",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_LIST
    },
    (Function) {
        .help = "Executes a Neon file with the given arguments in a separate environment",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_STRING, TYPE_LIST},
        .typeRetour = TYPE_NONE
    },
    {
        .help = "Converts an integer into its binary representation",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_INTEGER},
        .typeRetour = TYPE_STRING
    },
    {
        .help = "Converts an integer into its hexa-decimal representation",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_INTEGER},
        .typeRetour = TYPE_STRING
    },
    {
        .help = "Writes a Neon object into a file",
        .nbArgs = 2,
        .typeArgs = (int[]){TYPE_STRING, TYPE_UNSPECIFIED},
        .typeRetour = TYPE_NONE
    },
    {
        .help = "Reads a Neon object from a file created by the serialize function",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_STRING},
        .typeRetour = TYPE_UNSPECIFIED
    },
    {
        .help = "Formats a string by filling in the `<>` markers by the given arguments.\nExample: format(\"<> + <> = <>\", 1, 2, 3) returns \"1 + 2 = 3\"",
        .nbArgs = -1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_STRING
    },
    {
        .help = "Returns an integer indentifying any object.",
        .nbArgs = 1,
        .typeArgs = (int[]){TYPE_UNSPECIFIED},
        .typeRetour = TYPE_INTEGER
    }
};



NeObj get_standardfunction(int id) {
    neon_assert(id >= 0 && id < NBBUILTINFUNC, NEO_VOID);

    Function f = builtinfunctions[id];
    return neo_fun_create(id, StandardModule, f.help, f.nbArgs, f.typeArgs, f.typeRetour);
}

const char* get_standardfunction_name(int id) {
    neon_assert(id >= 0 && id < NBBUILTINFUNC, NULL);

    return builtinfunctions_names[id];
}

NeObj call_standardfunction(int id, NeList* list) {
    neon_assert(id >= 0 && id < NBBUILTINFUNC, NEO_VOID);

    return builtinfunctions_pointers[id](list);
}


void init_standardmodule(NeonEnv* env) {
    for (int i = 0 ; i < NBBUILTINFUNC ; i++) {
        strlist_append(env->NOMS, strdup(get_standardfunction_name(i)));
        nelist_append(env->ADRESSES, get_standardfunction(i));
    }
    return;
}