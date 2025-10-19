/**
 * @file ce.c
 * @author Adrien "Adriweb" Bertrand
 * @version 1.0
 *
 * @section LICENSE
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
 *
 * @section DESCRIPTION
 *
 * Functions for CE platform
 */

#include "headers/platform.h"
#include "headers/nspireio.h"

bool any_key_pressed(void)
{
    return kb_AnyKey();
}

void wait_key_pressed(void)
{
    while (!any_key_pressed());
}

bool nio_scrbuf_init()
{
    gfx_Begin();
    return true;
}

void nio_scrbuf_clear()
{
    gfx_ZeroScreen();
}

void nio_scrbuf_free()
{
    gfx_End();
}



void nio_set_global_color(uint8_t color)
{
    gfx_SetColor(color);
}

void nio_vram_pixel_set(unsigned int x, unsigned int y)
{
    gfx_SetPixel(x, (uint8_t)y);
}

void nio_vram_fill(unsigned x, unsigned y, unsigned w, unsigned h)
{
    // Hack: Round up to the screen width to cover the pixels between the last char and the edge.
    if(x + w == (unsigned)(SCREEN_WIDTH/NIO_CHAR_WIDTH) * NIO_CHAR_WIDTH)
        w = SCREEN_WIDTH - x;

    gfx_FillRectangle_NoClip(x, (uint8_t) y, w, (uint8_t) h);
}

void nio_vram_scroll(unsigned x, unsigned y, unsigned w, unsigned h, unsigned scroll) {
    if (!scroll) return;
    gfx_ShiftUp((uint8_t) scroll);

    // Hack: Round up to the screen width to cover the pixels between the last char and the edge.
    if(x + w == (unsigned)(SCREEN_WIDTH/NIO_CHAR_WIDTH) * NIO_CHAR_WIDTH)
        w = SCREEN_WIDTH - x;

// TODO: support non-full scroll
/*
    unsigned int r = h - scroll;

    uint16_t *p1 = (uint16_t*)gfx_vram+x+SCREEN_WIDTH*y;
    uint16_t *p2 = p1 + SCREEN_WIDTH*scroll;
    size_t n = sizeof(uint16_t)*w;
    for (unsigned int i = 0; i < r; ++i, p1 += SCREEN_WIDTH, p2 += SCREEN_WIDTH)
        memmove(p1, p2, n);
 */

    nio_vram_fill(x, y + h - scroll, w, scroll);
}

void nio_vram_draw(void)
{
    // we could do gfx_Blit...
}

unsigned int nio_cursor_clock(void) {
    return (unsigned int) rtc_Time();
}

unsigned nio_time_get() {
    return (unsigned int) rtc_Time();
}

unsigned char nio_ascii_get(uint8_t* adaptive_cursor_state)
{
    static bool second = false, alpha = false;
    sk_key_t key = os_GetCSC();

    if (key == sk_2nd) {
        second = (bool)!second;
    }
    if (key == sk_Alpha) {
        alpha = (bool)!alpha;
    }

    if (alpha && second) {
        *adaptive_cursor_state = 2; // uppercase alpha cursor
    } else if (alpha) {
        *adaptive_cursor_state = 3; // lowercase alpha cursor
    } else if (second) {
        *adaptive_cursor_state = 1; // arrow cursor
    } else {
        *adaptive_cursor_state = 0; // block cursor
    }

    switch (key)
    {
        case sk_Mode  : return 0;
        case sk_Clear : return NIO_KEY_ESC;

        case sk_DecPnt: return (unsigned char) (alpha ?                 ':'  : '.');
        case sk_Chs   : return (unsigned char) (alpha ?                 '?'  : '-');
        case sk_0     : return (unsigned char) (alpha ?                 ' '  : '0');
        case sk_1     : return (unsigned char) (alpha ? (second ? 'Y' : 'y') : '1');
        case sk_2     : return (unsigned char) (alpha ? (second ? 'Z' : 'z') : '2');
        case sk_3     : return (unsigned char) (alpha ?                 '@'  : '3');
        case sk_4     : return (unsigned char) (alpha ? (second ? 'T' : 't') : '4');
        case sk_5     : return (unsigned char) (alpha ? (second ? 'U' : 'u') : '5');
        case sk_6     : return (unsigned char) (alpha ? (second ? 'V' : 'v') : '6');
        case sk_7     : return (unsigned char) (alpha ? (second ? 'O' : 'o') : '7');
        case sk_8     : return (unsigned char) (alpha ? (second ? 'P' : 'p') : '8');
        case sk_9     : return (unsigned char) (alpha ? (second ? 'Q' : 'q') : '9');

        case sk_Math  : return (unsigned char) (alpha ? (second ? 'A' : 'a') :  0 );
        case sk_Apps  : return (unsigned char) (alpha ? (second ? 'B' : 'b') :  0 );
        case sk_Prgm  : return (unsigned char) (alpha ? (second ? 'C' : 'c') :  0 );
        case sk_Recip : return (unsigned char) (alpha ? (second ? 'D' : 'd') : '\'');
        case sk_Sin   : return (unsigned char) (alpha ? (second ? 'E' : 'e') :  0 );
        case sk_Cos   : return (unsigned char) (alpha ? (second ? 'F' : 'f') :  0 );
        case sk_Tan   : return (unsigned char) (alpha ? (second ? 'G' : 'g') :  0 );
        case sk_Square: return (unsigned char) (alpha ? (second ? 'I' : 'i') : ';');
        case sk_Comma : return (unsigned char) (alpha ? (second ? 'J' : 'j') : ',');
        case sk_LParen: return (unsigned char) (alpha ? (second ? 'K' : 'k') : (second ? '{' : '('));
        case sk_RParen: return (unsigned char) (alpha ? (second ? 'L' : 'l') : (second ? '}' : ')'));
        case sk_Log   : return (unsigned char) (alpha ? (second ? 'N' : 'n') : '%');
        case sk_Ln    : return (unsigned char) (alpha ? (second ? 'S' : 's') : '<');
        case sk_Store : return (unsigned char) (alpha ? (second ? 'X' : 'x') : '>');

        case sk_Add   : return (unsigned char) (alpha ?                 '"'  : '+');
        case sk_Sub   : return (unsigned char) (alpha ? (second ? 'W' : 'w') : (second ? ']' : '-'));
        case sk_Mul   : return (unsigned char) (alpha ? (second ? 'R' : 'r') : (second ? '[' : '*'));
        case sk_Div   : return (unsigned char) (alpha ? (second ? 'M' : 'm') : '/');
        case sk_Power : return (unsigned char) (alpha ? (second ? 'H' : 'h') : '^');

        case sk_Vars  : return '#';
        case sk_Yequ  : return '=';
        case sk_Del   : return '\b';
        case sk_Enter : return '\n';
        case sk_Up    : return NIO_KEY_UP;
        case sk_Down  : return NIO_KEY_DOWN;

        case sk_GraphVar: return 'X'; // why not

        default: return 0; // no key pressed
    }
}

char* nio_get_back_buffer()
{
    return (char*) (gfx_vram + SCREEN_WIDTH*SCREEN_HEIGHT); // back buffer since 8bpp
}
