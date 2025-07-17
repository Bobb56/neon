#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "headers.c"

extern int CODE_ERROR;



#ifndef TI83PCE



    double* double_allocate(double n)
    {
        double* res = err_malloc(sizeof(double));
        *res = n;
        return res;
    }
    
    
    Number number_default(void)
    {
        //double* ptr = err_malloc(sizeof(double));
        //*ptr = (double)0;
        return NULL;
    }
    
    
    Number number_fromStr(char *string)//convertit une chaîne de caractère en nombre
    {
        if (strcmp(string, "Infini") == 0)
            return double_allocate(INFINITY);
        else if (strcmp(string, "PasUnNombre") == 0)
            return double_allocate(NAN);
        else
        {
            double nombre;
            
            int err=sscanf(string,"%lf",&nombre);
            if (err!=1)
            {
                CODE_ERROR = 66;
                return 0;
            }
            return double_allocate(nombre);
        }
    }
    
    
    
    char* number_toStr(Number number)//nombre en chaine de caractère
    {
        if (isinf(*number) && *number > 0)
                return strdup("Infini");
        else if (isinf(*number) && *number < 0)
            return strdup("-Infini");
        else if (isnan(*number))
            return strdup("PasUnNombre");
        else
        {
            //une fois qu'on a copié le nombre dans la chaine de caracteres, il faut le réallouer de la bonne taille
            char* strNombre = (char*)err_malloc(310*sizeof(char));//on estime qu'un double ne fait pas plus de 50 caractères de longueur
            int err=snprintf(strNombre, 310, "%lf", *number);//converison du nombre
          
            if (err<0 || err!=(int)strlen(strNombre))
            {
              CODE_ERROR = 66;
              err_free(strNombre);
              return 0;
            }
          
            while (strNombre[strlen(strNombre)-1]=='0')//on enlève les zéros inutiles à la fin
            {
              strNombre[strlen(strNombre)-1]='\0';
            }
          
          
            if (strNombre[strlen(strNombre)-1]=='.')//si on a enlevé tous les zéros, on enlève aussi la virgule
            {
              strNombre[strlen(strNombre)-1]='\0';
            }
          
            //ici, faire un realloc avec strlen
          
            return strNombre;
        }
      
    }
    
    
    void number_destroy(Number number)
    {
        if (number != NULL)
            err_free(number);
        return;
    }
    
    Number number_copy(Number number)
    {
        return double_allocate(*number);
    }
    
    
    Number number_add(Number n1, Number n2)
    {
        return double_allocate(*n1 + *n2);
    }
    
    Number number_sub(Number n1, Number n2)
    {
        return double_allocate(*n1 - *n2);
    }
    
    Number number_mul(Number n1, Number n2)
    {
        return double_allocate(*n1 * *n2);
    }
    
    Number number_div(Number n1, Number n2)
    {
        return double_allocate(*n1 / *n2);
    }
    
    Number number_pow(Number n1, Number n2)
    {
        return double_allocate(pow(*n1,*n2));
    }
    
    _Bool number_equal(Number n1, Number n2)
    {
        return *n1 == *n2;
    }
    
    _Bool number_inf(Number n1, Number n2)
    {
        return *n1 < *n2;
    }
    
    _Bool number_infEq(Number n1, Number n2)
    {
        return *n1 <= *n2;
    }
    
    _Bool number_sup(Number n1, Number n2)
    {
        return *n1 > *n2;
    }
    
    _Bool number_supEq(Number n1, Number n2)
    {
        return *n1 >= *n2;
    }

    int number_compare(Number a, Number b)
    {
        if (*a > *b)
            return 1;
        else if (*a < *b)
            return -1;
        else
            return 0;
    }
    
    Number number_minus(Number n1)
    {
        return double_allocate(-*n1);
    }
    
    Number number_mod(Number n1, Number n2)
    {
        return double_allocate((double) ( (long int)*n1 % (long int)*n2 ));
    }
    
    Number number_eucl(Number nb1, Number nb2)
    {
        return double_allocate((double)(*nb1-(long int)*nb1%(long int)*nb2) / *nb2);
    }
    
    void number_aff(Number number)
    {
        if (isinf(*number) && *number > 0)
            printf("Infini");
        else if (isinf(*number) && *number < 0)
            printf("-Infini");
        else if (isnan(*number))
            printf("PasUnNombre");
        else if ((long int)*number == *number)
            printf("%ld", (long int)*number);
        else
            printf("%lf", *number);
    
        return ;
    }
    
    double number_toDouble(Number num)
    {
        return *num;
    }
    
    
    Number number_fromDouble(double num)
    {
        return double_allocate(num);
    }

    Number number_sin(Number a)
    {
        return number_fromDouble(sin(*a));
    }

    Number number_cos(Number a)
    {
        return number_fromDouble(cos(*a));
    }

    Number number_tan(Number a)
    {
        return number_fromDouble(tan(*a));
    }

    Number number_rad(Number a)
    {
        return number_fromDouble(*a/180*PI);
    }

    Number number_deg(Number a)
    {
        return number_fromDouble(*a/PI*180);
    }

    Number number_sqrt(Number a)
    {
        return number_fromDouble(sqrt(*a));
    }

    Number number_ln(Number a)
    {
        return number_fromDouble(log(*a));
    }

    Number number_exp(Number a)
    {
        return number_fromDouble(exp(*a));
    }

    Number number_log(Number a)
    {
        return number_fromDouble(log10(*a));
    }

    Number number_log2(Number a)
    {
        return number_fromDouble(log2(*a));
    }

    Number number_round(Number a,Number b)
    {
        unsigned prec = (unsigned)number_toDouble(b);
        int dec = pow(10,prec);

        return number_fromDouble(((double)round(*a * dec))/((double)dec));
    }

    Number number_abs(Number a)
    {
        if (*a < 0)
            return number_fromDouble(-*a);
        else
            return number_fromDouble(*a);
    }

    Number number_ceil(Number a)
    {
        return number_fromDouble(ceil(*a));
    }

    Number number_floor(Number a)
    {
        return number_fromDouble(floor(*a));
    }

    Number number_randint(Number a, Number b)
    {
        double min = number_toDouble(a);
        double max = number_toDouble(b);

        if (max != (unsigned int)max || min != (unsigned int)min || min >= max)
        {
            CODE_ERROR = 64;// la fonction randint prend en argument un entier positif
            return NULL;
        }

        double result = rand()%(int)(max-min) + min;

        return number_fromDouble(result);
    }

#else

    #include <ti/real.h>

    real_t* real_allocate(real_t number)
    {
        real_t* ret = err_malloc(sizeof(real_t));
        *ret = os_RealCopy(&number);
        return ret;
    }

    
    Number number_default(void)
    {
        //double* ptr = err_malloc(sizeof(double));
        //*ptr = (double)0;
        return NULL;
    }
    
    
    Number number_fromStr(char *string)//convertit une chaîne de caractère en nombre
    {
        return real_allocate(os_StrToReal(string, NULL));
    }
    
    
    
    char* number_toStr(Number number)//nombre en chaine de caractère
    {
        char* result = err_malloc(sizeof(char) * 127);
        os_RealToStr(result, number, 127, 1, -1);
        char* ret = strdup(result);
        err_free(result);
        return ret;
    }
    
    
    void number_destroy(Number number)
    {
        if (number != NULL)
            err_free(number);
        return;
    }
    
    Number number_copy(Number number)
    {
        return real_allocate(*number);
    }
    
    
    Number number_add(Number n1, Number n2)
    {
        return real_allocate(os_RealAdd(n1,n2));
    }
    
    Number number_sub(Number n1, Number n2)
    {
        return real_allocate(os_RealSub(n1,n2));
    }

    Number number_mul(Number n1, Number n2)
    {
        return real_allocate(os_RealMul(n1,n2));
    }
    
    Number number_div(Number n1, Number n2)
    {
        return real_allocate(os_RealDiv(n1,n2));
    }
    
    Number number_pow(Number n1, Number n2)
    {
        return real_allocate(os_RealPow(n1,n2));
    }
    
    _Bool number_equal(Number n1, Number n2)
    {
        return os_RealCompare(n1,n2) == 0;
    }
    
    _Bool number_inf(Number n1, Number n2)
    {
        return os_RealCompare(n1,n2) < 0;
    }
    
    _Bool number_infEq(Number n1, Number n2)
    {
        return os_RealCompare(n1,n2) <= 0;
    }
    
    _Bool number_sup(Number n1, Number n2)
    {
        return os_RealCompare(n1,n2) > 0;
    }
    
    _Bool number_supEq(Number n1, Number n2)
    {
        return os_RealCompare(n1,n2) >= 0;
    }

    int number_compare(Number a, Number b)
    {
        return os_RealCompare(a,b);
    }
    
    Number number_minus(Number n1)
    {
        return real_allocate(os_RealNeg(n1));
    }
    
    Number number_mod(Number n1, Number n2)
    {
        return real_allocate(os_RealMod(n1, n2));
    }
    
    Number number_eucl(Number nb1, Number nb2)
    {
        int n1 = (int)os_RealToFloat(nb1);
        int n2 = (int)os_RealToFloat(nb2);
        int div = (n1 - n1 % n2) / n2;
        return real_allocate(os_FloatToReal((float)div));
    }
    
    void number_aff(Number number)
    {
        char* str = number_toStr(number);
        printString(str);
        err_free(str);
        return ;
    }
    
    double number_toDouble(Number num)
    {
        return (double)os_RealToFloat(num);
    }
    
    
    Number number_fromDouble(double num)
    {
        return real_allocate(os_FloatToReal((float)num));
    }


    Number number_sin(Number a)
    {
        return real_allocate(os_RealSinRad(a));
    }

    Number number_cos(Number a)
    {
        return real_allocate(os_RealCosRad(a));
    }

    Number number_tan(Number a)
    {
        return real_allocate(os_RealTanRad(a));
    }

    Number number_rad(Number a)
    {
        return real_allocate(os_RealDegToRad(a));
    }

    Number number_deg(Number a)
    {
        return real_allocate(os_RealRadToDeg(a));
    }

    Number number_sqrt(Number a)
    {
        return real_allocate(os_RealSqrt(a));
    }

    Number number_ln(Number a)
    {
        return real_allocate(os_RealLog(a));
    }

    Number number_exp(Number a)
    {
        return real_allocate(os_RealExp(a));
    }

    Number number_log(Number a)
    {
        Number ln10 = number_fromDouble(2.302585093);
        Number lna = number_ln(a);
        Number b = number_div(lna,ln10);
        number_destroy(ln10);number_destroy(lna);
        return b;
    }

    Number number_log2(Number a)
    {
        Number ln2 = number_fromDouble(0.693147181);
        Number lna = number_ln(a);
        Number b = number_div(lna,ln2);
        number_destroy(ln2);number_destroy(lna);
        return b;
    }

    Number number_round(Number a,Number b)
    {
        char prec = (char)number_toDouble(b);
        return real_allocate(os_RealRound(a,prec));
    }

    Number number_abs(Number a)
    {
        Number _a = number_minus(a);
        Number res = real_allocate(os_RealMax(a, _a));
        number_destroy(_a);
        return res;
    }

    Number number_floor(Number a)
    {
        return real_allocate(os_RealFloor(a));
    }

    Number number_ceil(Number a)
    {
        Number _1 = number_fromDouble((double)1);
        Number a2 = number_floor(a);
        Number res = real_allocate(os_RealAdd(a2, _1));
        number_destroy(_1), number_destroy(a2);
        return res;
    }


    Number number_randint(Number a, Number b)
    {
        return real_allocate(os_RealRandInt(a, b));
    }
    

#endif
