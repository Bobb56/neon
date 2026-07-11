/*
 * main.c
 *
 *  Created on: Jul 25, 2021
 *      Author: michael
 *      Edited for BOS by Beck
 */


/*
TODO:
- Stack overflow avec les programmes un peu gros sur TI_EZ80 (apples.ne)
- relaunch.ne sur TI_EZ80: boucle infinie ou crash
- RAM reset quand on lance launcher dans LAUNCHER dans l'application
- Handle archived scripts
- Add the ability of running a program directly from the editor
- Add the ability to edit a program from the console that runs it
- KEY_CLEAR -> KEY_STO
- Update is_control
- Make an inventory of all key bindings and keep/change some
*/

#include "headers/neonide.h"
#include "headers/home.h"
#include "headers/libmalloc.h"
#include "headers/state.h"
#include "headers/console.h"
#include <stdlib.h>


bool initialize(struct estate *state)
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
	state->running_program = false;

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


void initialize_void(struct estate* state) {
	state->lines = NULL;
	state->text = NULL;
	state->max_lines = 0;
	state->max_buffer_size = 0;
	initialize(state);
}
