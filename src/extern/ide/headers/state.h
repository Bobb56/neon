/*
 * state.h
 *
 *  Created on: Jul 25, 2021
 *      Author: michael
 */

#ifndef SRC_STATE_H_
#define SRC_STATE_H_

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


/*
 * The default font width and height
 */
#define FONT_WIDTH 		8
#define FONT_HEIGHT 	13

/*
 * The default line spacing
 */
#define LINE_SPACING 14

/*
 * The number of lines that fit on the screen,
 * minus one.
 */
#define NUM_LINES 14

#define NUM_COLS 39

/*
 * The state of the editor.
 */
struct estate {
	int24_t max_lines;
	int24_t max_buffer_size;

	//Number of lines multi movements move by
	int24_t multi_lines;
	//The file name
	char filename[10];
	//true if we are in REPL, false if we are running a program
	bool console;
	//The currently running program
	char running_program[10];
	//Whether the file name is user defined
	bool named;
	//A pointer within the line array. Same line as cursor.
	int24_t lc1;
	//lc1, but line after the cursor
	int24_t lc2;
	//distance of cursor from line start
	int24_t lc_offset;
	//Distance of screen from line start
	int24_t ls_offset;
	//pointer to left of cursor
	int24_t c1;
	//pointer to right of cursor
	int24_t c2;
	//pointer to screen start
	int24_t scr_offset;
	//The line the screen is on
	int24_t scr_line;
	//line offset of scr pos.
	int24_t scr_line_offset;
	//Self explanatory.
	uint8_t text_color;
	uint8_t text_highlight_color;
	uint8_t text_selection_color;
	uint8_t text_selection_highlight_color;
	uint8_t background_color;
	uint8_t transparent_color;
	uint8_t statusbar_color;
	uint8_t statusbar_text_color;
	uint8_t border_color;
	uint8_t dropshadow_color;
	uint8_t focus_color;
    ti_var_t clipboard_file;
	//the radius for rounded corners on dialogs.
	int8_t corner_radius;
	//Cursor x position
	int cx;
	//Cursor y position
	int cy;
	//True if doc has been saved
	bool saved;
	//Size, in num chars, of the clipboard
	int24_t clipboard_size;
	//The first point of selection. The other point is the cursor
	int24_t selection_anchor;
	//True if the selection is active, otherwise false.
	bool selection_active;

	//0: inactive, 1: lower case, 2: uppercase
	uint8_t alpha_state;

	//These are best put at the end of the struct
	//Text buffer
	//char text[16385];
	//Gonna make it 64Kb.
	char* text;
	//Line len buffer
	int24_t* lines;

	// left position of history content
	int24_t history_left[NUM_LINES * 2];
	int24_t history_right[NUM_LINES * 2];
	uint8_t history_length;

	//Data in the clipboard
	//char clipboard_data[10000];
	///////////////////////////Random Editor Settings///////////////////////////
	//Default is false, if true, files will be archived after writes. Does nothing on BOS.
	bool autoarchive;
	//Default is true, if enabled, unsaved file = prompt
	bool saveprompt;
	//Default is true, if enabled, used regular expressions in the search box
	bool useregex;
	//Default is true, if enabled, blink the cursor slowly
	bool blinkcursor;
	//Default is true, if enabled, write files under a different filename, then remove the existing and rename the new file.
	bool backupfiles;
	//Default is true, if enabled, parse ceditrc from /etc/cedit/ceditrc. Otherwise use /home/.ceditrc
    //  files that begin with a dot
    //  Default is true
    bool hide_special_files;
    //For the search text buffer
    char search_buffer[256];
};

#endif /* SRC_STATE_H_ */
