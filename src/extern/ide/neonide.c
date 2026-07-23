/*
 * neonide.c
 *
 *      Author: Raphaël Le Puillandre
 */

/*
TODO:
- Make sure that 0x1b is taken in account everywhere in the terminal
- Fix problems with alpha locking keys like digits sometimes
- Try to increase a bit the size of the console history
- KEY_CLEAR -> KEY_STO && KEY_STO insert an '='
- Update is_control
- Make an inventory of all key bindings and keep/change some
- Improve search with circular search
- Indentation support ?
*/

#include "headers/neonide.h"
#include "headers/editor.h"
#include "headers/font.h"
#include "headers/home.h"
#include "headers/libmalloc.h"
#include "headers/state.h"
#include "headers/console.h"
#include <stdlib.h>
#include <fileioc.h>
#include <fontlibc.h>
#include <string.h>


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
    state->multi_lines = NUM_LINES;
    state->named = false;
    state->lc1 = 0;
    state->lc2 = state->max_buffer_size - 1;
    state->lc_offset = 0;
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
    state->ide_state = IDEState_Other;
    state->ide_goto = IDEState_Other;
    state->ide_go_back = IDEState_Other;

    state->selection_active = false;
    state->alpha_state = AlphaState_NoALpha;

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

void set_normal_font(void) {
    fontlib_SetFont(font, 0);
}

// No bold font for now
void set_bold_font(void) {
    fontlib_SetFont(font, 0);
}


void launch_neonide(void) {
	set_normal_font();
	home_menu();

	clean_memory();
}
