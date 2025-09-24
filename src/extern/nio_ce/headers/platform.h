/**
 * @file platform.h
 * @author Julian Mackeben aka compu <compujuckel@googlemail.com>
 *       + Adrien "Adriweb" Bertrand
 * @version 3.1
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
 * Decide which header to use...
 */

// Includes for the different platforms here.

#ifndef PLATFORM_H
#define PLATFORM_H

#define NIO_CHAR_WIDTH 6
#define NIO_CHAR_HEIGHT 8

// Put headers required for our platform here
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <sys/rtc.h>
#include <ti/getcsc.h>
#include <graphx.h>
#include <keypadc.h>

// Fullscreen definitions

#define NIO_MAX_ROWS 30
#define NIO_MAX_COLS 53

#define SCREEN_WIDTH    GFX_LCD_WIDTH
#define SCREEN_HEIGHT   GFX_LCD_HEIGHT

void wait_key_pressed(void); // blocks until a key is pressed
bool any_key_pressed(void); // non-blocking, TRUE if any key pressed

/** Initializes double buffering.
*/
bool nio_scrbuf_init();

/** Clears the screen buffer.
*/
void nio_scrbuf_clear();

/** Frees the screenbuffer and restores the screen to its original state.
*/
void nio_scrbuf_free();

char* nio_get_back_buffer();

// These functions are the same on all platforms...

void nio_set_global_color(unsigned int color);
void nio_vram_pixel_set(unsigned int x, unsigned int y);
void nio_vram_fill(unsigned x, unsigned y, unsigned w, unsigned h);
void nio_vram_draw(void);
void nio_vram_scroll(unsigned x, unsigned y, unsigned w, unsigned h, unsigned scroll);
unsigned int nio_cursor_clock(void);
unsigned char nio_ascii_get(uint8_t* adaptive_cursor_state);

#endif
