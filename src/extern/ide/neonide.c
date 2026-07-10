/*
 * main.c
 *
 *  Created on: Jul 25, 2021
 *      Author: michael
 *      Edited for BOS by Beck
 */


/*
TODO:
- Make NeonIDE as an app and integrate Neon inside
*/

#include "headers/neonide.h"
#include "headers/home.h"
#include "headers/libmalloc.h"
#include "headers/state.h"
#include "headers/console.h"
#include <stdlib.h>


static bool initialize(struct estate *state)
{
    memset(state->search_buffer,0,255);
	//Default is false, if true, files will be archived after writes. Does nothing on BOS.
	state->autoarchive = false;
	//Default is true, if enabled, unsaved file = prompt
	state->saveprompt = true;
	//Default is true, if enabled, used regular expressions in the search box
	state->useregex = true;
	//Default is true, if enabled, blink the cursor slowly
	state->blinkcursor = true;
	//Default is true, if enabled, write files under a different filename, then remove the existing and rename the new file.
	state->backupfiles = true;
	state->multi_lines = 5;
	state->named = false;
	state->lc1 = 0;
	state->lc2 = state->max_buffer_size - 1;
	state->lc_offset = 0;
	state->ls_offset = 0;
	state->c1 = 0;
	state->c2 = state->max_buffer_size - 1;
	state->scr_offset = 0;
	state->scr_line_offset = 0;
	state->history_length = 0;
	state->text_color = 0;
	state->text_highlight_color = 255;
	state->text_selection_color = 0;
	state->text_selection_highlight_color = 31;
	state->background_color = 255;
	state->transparent_color = 255;
	state->statusbar_text_color = 0;
	state->statusbar_color = 20;
	state->border_color = 0;
	state->dropshadow_color = 13;
	state->focus_color = 12;
	state->saved = true;

	state->selection_active = false;
	state->alpha_state = 0;

	state->clipboard_size = 0;
	state->corner_radius = 10;

    state->hide_special_files=1;

	fontlib_SetForegroundColor(state->text_color);
	fontlib_SetTransparency(true);
	fontlib_SetBackgroundColor(state->text_highlight_color);
	
	// Read the parameters from the user configuration file
	parseRC(state);
	return 0;
}

void initialize_editor(struct estate* state) {
	state->max_buffer_size = 16384;
	state->max_lines = 10000;
	state->text = malloc_noheap(state->max_buffer_size);
	state->lines = malloc_noheap(state->max_lines * sizeof(int16_t));
	initialize(state);
}


void initialize_console(struct estate* state) {
	state->max_buffer_size = NUM_LINES * NUM_COLS * 2;
	state->max_lines = NUM_LINES * 2;
	state->text = malloc(state->max_buffer_size);
	state->lines = malloc(state->max_lines * sizeof(int16_t));
	initialize(state);
}

void initialize_void(struct estate* state) {
	state->lines = NULL;
	state->text = NULL;
	state->max_lines = 0;
	state->max_buffer_size = 0;
	initialize(state);
}

void deinit_console(struct estate* state) {
	free(state->lines);
	free(state->text);
}

void deinit_editor(struct estate* state) {
	free_all_noheap();
}