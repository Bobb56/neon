/*
 * main.c
 *
 *  Created on: Jul 25, 2021
 *      Author: michael
 *      Edited for BOS by Beck
 */

/*
TODO:
- Fix consistency of Tools/Chars menus in console
- Fix problems when editing a program from the console, running programs from the editor
- neon_fail augmente la taille du contexte des fonctions, cause des stack overflows sur TI_EZ80
- RAM reset quand on lance l'éditeur de temps en temps
- KEY_CLEAR -> KEY_STO
- Update is_control
- Make an inventory of all key bindings and keep/change some
*/

#include "headers/neonide.h"
#include "headers/editor.h"
#include "headers/home.h"
#include "headers/libmalloc.h"
#include "headers/state.h"
#include "headers/console.h"
#include "headers/secureio.h"
#include <stdlib.h>
#include <fileioc.h>
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

    state->text_buffer_handle = 0;
    state->lines_array_handle = 0;

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


uint8_t create_buffer(struct estate* state, size_t size) {
    static uint8_t counter = 0;

    char name[9] = {0};
    strcpy(name, "NIDEBF");
    name[7] = counter%26 + 'a';
    name[6] = counter/26 + 'a';

    uint8_t slot = secureio_Open(state, name, "w");
    ti_Resize(size, slot);
    return slot;
}

void delete_buffer(struct estate* state, uint8_t slot) {
    char buffer[10];
    ti_GetName(buffer, slot);
    secureio_Close(state, slot);
    secureio_Delete(state, buffer);
}

