/*
 * dialogs.h
 *
 *  Created on: Jul 25, 2021
 *      Author: michael
 */

#ifndef SRC_DIALOGS_H_
#define SRC_DIALOGS_H_

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

#include "neonide.h"
#include "editor.h"
#include "state.h"
#include "keys.h"

bool string_equal(struct estate *state, int24_t pointer, char* str);

/*
 * Displays the editor settings:
 * tabstop, preserve clipboard on exit,
 * multi_lines
 */
void show_editor_settings_dialog(struct estate *);

void show_search_dialog(struct estate *);

void show_appearance_settings_dialog(struct estate *);

/*
 * Draws the main menu
 * The main menu will open various sub-dialogs for additional settings/options.
 * They will open on top of each other.
 * Returns -1 if need to exit editor
 */
int show_options_dialog(struct estate *);

/*
 * The backend drawing for the menu. Used to restore state after being
 * damaged by submenus, and for smaller drawing routines.
 */
void options_backend_draw(struct estate *, int);

void show_console_tools_dialog(struct estate *);
void console_tools_backend_draw(struct estate *, int);
void show_editor_tools_dialog(struct estate *);
void editor_tools_backend_draw(struct estate *, int);
void show_chars_dialog(struct estate* state, void (*draw_background)(struct estate*));
void chars_backend_draw(struct estate* state, int line, int col, int n_lines, int n_columns);
char* show_create_dialog(struct estate *state);
void show_file_menu_dialog(struct estate *state, char* filename);
void alert(struct estate *state, char* title, char* text);
void show_unimplemented_dialog(struct estate *state);
bool show_confirm_dialog(struct estate *state);

void draw_text_area(struct estate* state);

/*
 * Draws a dialog with an empty background
 * params x,y,width,height
 */
void draw_dialog(struct estate *, int, int, int, int);

/*
 * Draws the open file dialog
 * returns true if a new file was picked, false otherwise
 */
bool show_open_dialog(struct estate *);

/*
 * Draws the save file dialog
 */
bool show_save_dialog(struct estate *);
/*
 * Draws the about dialog
 */
void show_about_dialog(struct estate *);

//returns true if discard
bool show_unsaved_dialog(struct estate *);

#endif /* SRC_DIALOGS_H_ */