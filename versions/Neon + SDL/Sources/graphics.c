#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "headers.c"

extern int CODE_ERROR;
extern int MAX_HEIGHT;
extern int MAX_WIDTH;

#if defined(LINUX) || defined(WINDOWS10) || defined(WINDOWS11)

#include <SDL2/SDL.h>

extern SDL_Window *WINDOW;
extern SDL_Renderer* RENDERER;



// fonctions graphiques utilisant SDL

NeObject* _enableGraphics_(NeList* args)
{
    SDL_Rect bounds;
    SDL_GetDisplayBounds(0, &bounds);
    MAX_HEIGHT = bounds.h;
    MAX_WIDTH = bounds.w;

    // fonction qui affiche une fenêtre en plein écran, début des graphiques
    WINDOW = SDL_CreateWindow("Neon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MAX_WIDTH, MAX_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
    
    if(NULL == WINDOW)
    {
        CODE_ERROR = 93;
        return NULL;
    }

    RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_SOFTWARE);

    if(NULL == RENDERER)
    {
        CODE_ERROR = 93;
        return NULL;
    }

    if(SDL_SetRenderDrawColor(RENDERER, 255, 255, 255, 255) < 0 || SDL_RenderClear(RENDERER) < 0)
    {
        CODE_ERROR = 93;
        return NULL;
    }

    SDL_RenderPresent(RENDERER);


    return neo_none_create();
}





NeObject* _disableGraphics_(NeList* args)
{
    // fonction qui affiche une fenêtre en plein écran, début des graphiques
    if (WINDOW != NULL)
    {
        SDL_DestroyWindow(WINDOW);
        WINDOW = NULL;
    }
    if (RENDERER != NULL)
    {
        SDL_DestroyRenderer(RENDERER);
        RENDERER = NULL;
    }

    return neo_none_create();
}


NeObject* _sleep_(NeList* args)
{
    SDL_Delay((long int)number_toDouble(neo_to_nb(args->tab[0])));
    return neo_none_create();
}

NeObject* _setPixel_(NeList* args)
{

    if (RENDERER == NULL || WINDOW == NULL)
    {
        CODE_ERROR = 94;
        return NULL;
    }


    int x = number_toDouble(neo_to_nb(args->tab[0]));
    int y = number_toDouble(neo_to_nb(args->tab[1]));

    int r = number_toDouble(neo_to_nb(args->tab[2]));
    int g = number_toDouble(neo_to_nb(args->tab[3]));
    int b = number_toDouble(neo_to_nb(args->tab[4]));


    SDL_SetRenderDrawColor(RENDERER, r, g, b, 255);


    SDL_RenderDrawPoint(RENDERER, x, y);

    return neo_none_create();
}


NeObject* _refreshScreen_(NeList* args)
{
    if (RENDERER == NULL || WINDOW == NULL)
    {
        CODE_ERROR = 94;
        return NULL;
    }
    SDL_RenderPresent(RENDERER);
    return neo_none_create();
}



NeObject* _getScreenSize_(NeList* args)
{
    if (RENDERER == NULL || WINDOW == NULL)
    {
        CODE_ERROR = 94;
        return NULL;
    }

    NeList* l = nelist_create(2);
    l->tab[0] = neo_nb_create(number_fromDouble((double)MAX_WIDTH));
    l->tab[1] = neo_nb_create(number_fromDouble((double)MAX_HEIGHT));
    return neo_list_convert(l);
}




#endif