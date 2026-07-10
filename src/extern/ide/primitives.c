#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <fileioc.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphx.h>
#include <keypadc.h>
#include <fontlibc.h>

#include "headers/primitives.h"
#include "headers/neonide.h"
#include "headers/editor.h"
#include "headers/state.h"
#include "headers/gfx.h"
#include "headers/tigcclib.h"


void draw_switch(struct estate *state, int x, int y, bool on){
        gfx_SetColor(on?state->focus_color:state->background_color);
        gfx_FillCircle_NoClip(x+7,y+7,7);
        gfx_FillCircle_NoClip(x+21,y+7,7);
        gfx_SetColor(state->border_color);
        gfx_Circle_NoClip(x+7,y+7,7);
        gfx_Circle_NoClip(x+21,y+7,7);
        gfx_HorizLine_NoClip(x+7,y,14);
        gfx_HorizLine_NoClip(x+7,y+14,14);
        gfx_SetColor(on?state->focus_color:state->background_color);
        gfx_FillRectangle_NoClip(x+7,y+1,14,13);
        gfx_SetColor(state->statusbar_color);
        gfx_FillCircle_NoClip(x+(on?21:7),y+7,7);
        gfx_SetColor(state->border_color);
        gfx_Circle_NoClip(x+(on?21:7),y+7,7);
}