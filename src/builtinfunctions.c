#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "headers/constants.h"
#include "headers/neonio.h"
#include "headers/builtinfunctions.h"
#include "headers/dynarrays.h"
#include "headers/gc.h"
#include "headers/objects.h"
#include "headers/operators.h"
#include "headers/runtime.h"
#include "headers/errors.h"
#include "headers/strings.h"
#include "headers/syntaxtrees.h"
#include "headers/neon.h"

#ifdef TI_EZ80
#include <sys/rtc.h>
#include "headers/graphics.h"
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

        char *entree=input("");

    #else
        // à cause de linenoise, il faut mettre tout le texte dans une seule chaine de caractères
        char* chaine = strdup("");

        for (int i=0 ; i< args->len ; i++)
        {
            if (NEO_TYPE(ARG(i)) == TYPE_STRING)
                chaine = addStr2(chaine, neo_to_string(ARG(i)));
            else
                chaine = addStr2(chaine,neobject_str(ARG(i)));
            
            
            if (i < args->len - 1)
                chaine = addStr2(chaine, " ");
        }

        char* entree = input(chaine);
        free(chaine);

        if (global_env->CODE_ERROR != 0) {
            return NEO_VOID;
        }

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
        if (global_env->CODE_ERROR != 0)
            return NEO_VOID;
        return neo_double_create(ret);
    }
}



NeObj _str_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) == TYPE_STRING)
        return neo_copy(ARG(0));
    return neo_str_create(neobject_str(ARG(0)));
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
        global_env->CODE_ERROR = 4; // cet objet n'a pas de longueur
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
  global_env->CODE_ERROR = 1;
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
        
        char* retour = malloc((len+1)*sizeof(char));
        
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
        global_env->CODE_ERROR = 62; // unsupported types for reverse
        return NEO_VOID;
    }
}


NeObj _eval_(NeList* args)
{
    Tree* tree = tree_create(NULL, 0, 0);
    
    char* exp = neo_to_string(ARG(0));

    char* sov = global_env->FILENAME;
    global_env->FILENAME = NULL;
    
    createExpressionTree(tree, exp);

    if (global_env->CODE_ERROR != 0) {
        tree_destroy(tree);
        free(sov);
        return NEO_VOID;
    }

    NeObj res = eval_aux(tree);

    if (global_env->CODE_ERROR != 0)
        free(sov);
    else
        global_env->FILENAME = sov;
    
    tree_destroy(tree);
    return res;
}



NeObj _clear_(NeList* args)
{
    clearConsole();
    return neo_none_create();
}


NeObj _help_(NeList* args)
{
    if (args->len == 0) { // affiche l'ensemble des fonctions disponibles
        setColor(GREEN);printString("Neon v");printString(VERSION);setColor(WHITE);printString(" for ");
        printString(PLATFORM);printString(", compiled on ");printString(__TIMESTAMP__);newLine();        
        printString("Visit ");setColor(GREEN); printString("https://langage-neon.raphaael.fr"); setColor(WHITE);
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
                printString(global_env->NOMS->tab[i]);printString(": ");setColor(GREEN);printString(type(global_env->ADRESSES->tab[i]));setColor(WHITE);
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

            if (NEO_TYPE(ARG(i)) == TYPE_USERFUNC || NEO_TYPE(ARG(i)) == TYPE_USERMETHOD)
            {
                // type userfunc
                UserFunc* fun = neo_to_userfunc(ARG(i));

                if (NEO_TYPE(ARG(i)) == TYPE_USERMETHOD)
                    printString("method ");
                else
                    printString("function ");


                printString(global_env->NOMS->tab[nelist_index2(global_env->ADRESSES,ARG(i))]);

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

                        if (!neo_is_void(fun->opt_args->tab[i])) // expression non vide
                        {
                            printString(" := ");
                            neobject_aff(fun->opt_args->tab[i]);
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
            else if (NEO_TYPE(ARG(i)) == TYPE_FONCTION)
            {
                Function* fun = neo_to_function(ARG(i));
                printString("Built-in function ");

                printString(global_env->NOMS->tab[nelist_index2(global_env->ADRESSES,ARG(i))]);
                newLine();
                
                if (fun->nbArgs == -1)
                {
                    printString("Illimited number of arguments of type : ");

                    NeObj neo = (NeObj) {.type = fun->typeArgs[0]};
                    printString(type(neo));
                }
                else
                {
                    printString("Needs ");
                    printInt(fun->nbArgs);
                    printString(" argument(s)");

                    if (fun->nbArgs > 0) {
                        printString(" of type : ");
                        for (int i = 0 ; i < fun->nbArgs ; i++)
                        {
                            NeObj neo = (NeObj) {.type = fun->typeArgs[i]};
                            printString(type(neo));

                            if (i + 1 < fun->nbArgs)
                                printString(", ");
                        }
                    }
                    
                }

                printString(".");
                newLine();

                printString("The returned type is : ");

                NeObj neo = (NeObj) {.type = fun->typeRetour};
                printString(type(neo));

                printString(".");
                newLine();
                
                if (fun->help == NULL)
                    printString("No help available for this function.");
                else
                {
                    char* temp = strdup(fun->help); // c'est pas beau, c'est juste pour ne pas avoir le warning du compilateur
                    printString(temp);
                    free(temp);
                    newLine();
                }
            }
            // aide à propos d'un module
            else if (NEO_TYPE(ARG(i)) == TYPE_STRING && is_module(neo_to_string(ARG(0)))) {
                printString("Module ");printString(neo_to_string(ARG(0)));printString(" :");newLine();
                for (int i=0 ; i < global_env->NOMS->len ; i++) {
                    if (has_strict_prefix(global_env->NOMS->tab[i], neo_to_string(ARG(0))) && NEO_TYPE(global_env->ADRESSES->tab[i]) != TYPE_EMPTY) {
                        printString(global_env->NOMS->tab[i]);printString(": ");
                        setColor(GREEN);printString(type(global_env->ADRESSES->tab[i]));setColor(WHITE);newLine();
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
    setColor(RED);
    printString(" An error occured: ");
    printString(neo_to_string(ARG(0)));
    newLine();
    setColor(WHITE);
    affLine(global_env->FILENAME, global_env->LINENUMBER);

    global_env->CODE_ERROR = 1;
    
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
            global_env->CODE_ERROR = 71;
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
    char* c = malloc(sizeof(char) * 2);
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
            free(sov_FILENAME);
        global_env->CODE_ERROR = 112;
        return NEO_VOID;
    }
    
    _affect2(i, ARG(1)); // valeur de debut

    Tree* cond = tree_create(NULL, 0, 0);
    Tree* val = tree_create(NULL, 0, 0);

    createExpressionTree(cond, neo_to_string(ARG(4)));

    if (global_env->CODE_ERROR != 0)
    {
        tree_destroy(cond);
        tree_destroy(val);
        if (sov_FILENAME != NULL)
            free(sov_FILENAME);
        return NEO_VOID;
    }

    createExpressionTree(val, neo_to_string(ARG(5)));

    if (global_env->CODE_ERROR != 0)
    {
        tree_destroy(cond);
        tree_destroy(val);
        if (sov_FILENAME != NULL)
            free(sov_FILENAME);
        return NEO_VOID;
    }

    NeObj liste = neo_list_create(0);
    NeObj x = NEO_VOID;

    while (neo_to_integer(*i) < neo_to_integer(ARG(2)))
    {
        NeObj bo = eval_aux(cond);


        if (global_env->CODE_ERROR != 0) {
            tree_destroy(cond);
            tree_destroy(val);
            neobject_destroy(liste);
            if (sov_FILENAME != NULL)
                free(sov_FILENAME);
            return NEO_VOID;
        }


        if (neoIsTrue(bo)) {
            NeObj neo = eval_aux(val);
            neo_list_append(liste, neo);

            if (global_env->CODE_ERROR != 0) {
                neobject_destroy(bo);
                neobject_destroy(liste);
                tree_destroy(cond);
                tree_destroy(val);
                if (sov_FILENAME != NULL)
                    free(sov_FILENAME);
                return NEO_VOID;
            }
        }

        neobject_destroy(bo);

        // incrémentation de la variable d'indice
        x = _add(*i, ARG(3));

        neobject_destroy(*i);
        *i = x;
    }

    tree_destroy(cond);
    tree_destroy(val);

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


NeObj _raise_(NeList* args)
{
    global_env->EXCEPTION = neo_to_string(ARG(1));
    global_env->CODE_ERROR = -get_exception_code(ARG(0));
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
        global_env->CODE_ERROR = 113;
        return NEO_VOID;
    }
    
}


NeObj _index_(NeList* args)
{
    int i = nelist_index2(neo_to_list(ARG(0)), ARG(1));

    if (i >= 0)
        return neo_integer_create(i);
    
    return NEO_VOID;
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
        global_env->CODE_ERROR = 14;
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

    if (global_env->CODE_ERROR != 0)
        return NEO_VOID;
    
    return neo_none_create();
}



NeObj _sort_desc_(NeList* args)
{
    NeList* l = neo_to_list(ARG(0));

    quickSort(l->tab, 0, l->len - 1, neo_compare2);

    if (global_env->CODE_ERROR != 0)
        return NEO_VOID;
    
    return neo_none_create();
}


NeObj _sin_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
        return NEO_VOID;
    }
    return neo_double_create(sin(neo_to_double(ARG(0))));
}


NeObj _cos_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
        return NEO_VOID;
    }
    return neo_double_create(cos(neo_to_double(ARG(0))));
}


NeObj _tan_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
        return NEO_VOID;
    }
    return neo_double_create(tan(neo_to_double(ARG(0))));
}


NeObj _deg_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
        return NEO_VOID;
    }
    double angle = neo_to_double(ARG(0));
    return neo_double_create(angle/PI*180.f);
}


NeObj _rad_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
        return NEO_VOID;
    }
    double angle = neo_to_double(ARG(0));
    return neo_double_create(angle/180.f*PI);
}


NeObj _sqrt_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
        return NEO_VOID;
    }
    return neo_double_create(sqrt(neo_to_double(ARG(0))));
}


NeObj _ln_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
        return NEO_VOID;
    }
    return neo_double_create(log(neo_to_double(ARG(0))));;
}


NeObj _exp_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
        return NEO_VOID;
    }
    return neo_double_create(exp(neo_to_double(ARG(0))));
}


NeObj _log_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
        return NEO_VOID;
    }
    return neo_double_create(log10(neo_to_double(ARG(0))));
}


NeObj _log2_(NeList* args)
{
    if (NEO_TYPE(ARG(0)) != TYPE_INTEGER && NEO_TYPE(ARG(0)) != TYPE_DOUBLE) {
        global_env->CODE_ERROR = 114;
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

    if (global_env->CODE_ERROR != 0)
        return NEO_VOID;
    
    return neo_str_create(res);
}



NeObj _writeFile_(NeList* args)
{
    writeFile(neo_to_string(ARG(0)),neo_to_string(ARG(1)));

    if (global_env->CODE_ERROR != 0)
        return NEO_VOID;
    
    return neo_none_create();
}


NeObj _setFunctionDoc_(NeList* args) {
    if (NEO_TYPE(ARG(0)) != TYPE_USERFUNC && NEO_TYPE(ARG(0)) != TYPE_USERMETHOD) {
        global_env->CODE_ERROR = 98;
        return NEO_VOID;
    }
    UserFunc* fun = neo_to_userfunc(ARG(0));

    if (fun->doc != NULL)
        free(fun->doc);

    fun->doc = strdup(neo_to_string(ARG(1)));
    return neo_none_create();
}


NeObj _setAtomicTime_(NeList* args) {
    global_env->ATOMIC_TIME = neo_to_integer(ARG(0));
    global_env->atomic_counter = 0;

    if (global_env->ATOMIC_TIME < 1)
    {
        global_env->CODE_ERROR = 102;
        return NEO_VOID;
    }
    return neo_none_create();
}

NeObj _copy_(NeList* args) {
    NeObj copy = neo_deepcopy(ARG(0));
    return copy;
}



NeObj _load_namespace_(NeList* args) {
    char* namespace = neo_to_string(ARG(0));
    char* prefix = addStr(namespace, "~"); // le préfixe que l'on cherche
    for (int i=0 ; i < global_env->NOMS->len ; i++) {
        if (has_strict_prefix(global_env->NOMS->tab[i], prefix)) {
            char* without_prefix = remove_prefix(global_env->NOMS->tab[i], prefix);

            if (strlist_inList(global_env->NOMS, without_prefix)) {
                _affect2(&global_env->ADRESSES->tab[strlist_index(global_env->NOMS, without_prefix)], global_env->ADRESSES->tab[i]);
                free(without_prefix);
            }
            else {
                strlist_append(global_env->NOMS, without_prefix);
                nelist_append(global_env->ADRESSES, neo_copy(global_env->ADRESSES->tab[i]));
            }
        }
    }
    free(prefix);
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
    else if (strcmp(color, "white") == 0) {
        setColor(WHITE);
    }
    else if (strcmp(color, "green") == 0) {
        setColor(GREEN);
    }
    else if (strcmp(color, "red") == 0) {
        setColor(RED);
    }
    return neo_none_create();
}


NeObj _initGraphics_(NeList* args) {
    #if !defined(TI_EZ80)
    global_env->CODE_ERROR = 115;
    return NEO_VOID;
    #else

    // fonction pour charger en mémoire les fonctions graphiques définies dans graphics.c et définir les types de container utilisés
    initGraphics();

    #endif

    return neo_none_create();
}