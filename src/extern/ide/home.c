/*
 * home.c
 *
 *  Created on: Jul 02, 2026
 *      Author: Raphaël Le Puillandre
 */

#include <string.h>
#include <stdlib.h>
#include <fileioc.h>
#include <time.h>
#include "headers/home.h"
#include "headers/neonide.h"
#include "headers/console.h"
#include "headers/dialogs.h"
#include "headers/editor.h"
#include "headers/state.h"
#include "headers/keys.h"

#define TEXT_Y(i)                   (17 + 15*i)
#define FILES_LIST_SIZE             13

int cmp(const void *a, const void *b)
{
    return strcmp(a, b);
}

int get_files(struct estate* state, char files[][APPVAR_NAME_LENGTH]) {
    int nb_files = 0;

    char* var_name;
    void* vat_ptr = NULL;


    while ((var_name = ti_Detect(&vat_ptr, NEON_DEFAULT_FILE_HEADER)))
    {
        // Check the header again
        ti_var_t var = ti_Open(var_name, "r");
        char buffer[NEON_DEFAULT_FILE_HEADER_SIZE];
        ti_Read(buffer, NEON_DEFAULT_FILE_HEADER_SIZE, 1, var);

        if (strncmp(buffer, NEON_DEFAULT_FILE_HEADER, NEON_DEFAULT_FILE_HEADER_SIZE) == 0) {
            strcpy(files[nb_files++], var_name);
        }
        
        ti_Close(var);
    }

    vat_ptr = NULL;
    while ((var_name = ti_Detect(&vat_ptr, NEON_PLAIN_TEXT_HEADER)))
    {
        strcpy(files[nb_files++], var_name);
    }

    qsort(files, nb_files, APPVAR_NAME_LENGTH, cmp);
    return nb_files;
}


void draw_home_menu(struct estate* state, char files[][APPVAR_NAME_LENGTH], int nb_files, int cursor_position, int start_disp_index) {
    // Draw the background and buttons
    gfx_FillScreen(state->background_color);

    fontlib_SetForegroundColor(state->statusbar_text_color);
    fontlib_SetBackgroundColor(state->text_highlight_color);
    fontlib_SetTransparency(true);
    //Draw statusbars
    gfx_SetColor(state->statusbar_color);
    gfx_FillRectangle_NoClip(0, 0, 320, 12);  //Top
    gfx_FillRectangle_NoClip(1, 228, 62, 12); //Floating segments
    gfx_FillRectangle_NoClip(65, 228, 62, 12);
    gfx_FillRectangle_NoClip(129, 228, 62, 12);
    gfx_FillRectangle_NoClip(193, 228, 62, 12);
    gfx_FillRectangle_NoClip(257, 228, 62, 12);
    //Draw text on segs
    fontlib_SetCursorPosition(20, 228);
    fontlib_DrawString("New");
    fontlib_SetCursorPosition(84, 228);
    fontlib_DrawString("Run");
    fontlib_SetCursorPosition(132, 228);
    fontlib_DrawString("Console");
    fontlib_SetCursorPosition(209, 228);
    fontlib_DrawString("Edit");
    fontlib_SetCursorPosition(272, 228);
    fontlib_DrawString("File");
    //Draw drop shadows
    gfx_SetColor(state->dropshadow_color);
    gfx_VertLine_NoClip(63, 229, 11);
    gfx_VertLine_NoClip(127, 229, 11);
    gfx_VertLine_NoClip(191, 229, 11);
    gfx_VertLine_NoClip(255, 229, 11);
    gfx_VertLine_NoClip(319, 229, 11);
    //Top
    gfx_HorizLine_NoClip(0, 12, 320);

    //Draw top text
    fontlib_SetCursorPosition(1, 0);
    fontlib_DrawString("Neon");

    fontlib_SetCursorPosition(280, 0);
    if (state->alpha_state == AlphaState_alpha) {
        fontlib_DrawString("alpha");
    }
    else if (state->alpha_state == AlphaState_ALPHA) {
        fontlib_DrawString("ALPHA");
    }

    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetBackgroundColor(state->text_highlight_color);
    fontlib_SetTransparency(true);


    // Draw the menu

    if (nb_files == 0) {
        fontlib_SetForegroundColor(state->text_highlight_color);
        fontlib_SetForegroundColor(state->text_color);
        fontlib_SetCursorPosition(90, 120);
        fontlib_DrawString("No Neon script found");
    }
    else {
        int last_index;
        if (nb_files <= FILES_LIST_SIZE)
            last_index = nb_files;
        else {
            last_index = FILES_LIST_SIZE + 1;
        }

        for (int i=0 ; i < last_index ; i++) {
            // display cursor
            if (i == cursor_position) {
                gfx_SetColor(state->focus_color);
                gfx_Rectangle_NoClip(2, TEXT_Y(i)-1, 316, 15);
                gfx_SetColor(state->dropshadow_color);
                gfx_HorizLine_NoClip(3, TEXT_Y(i)-1, 314);
                fontlib_SetForegroundColor(state->focus_color);
            }
            else {
                fontlib_SetForegroundColor(state->text_color);
            }

            fontlib_SetCursorPosition(10, TEXT_Y(i));
            fontlib_DrawString(files[start_disp_index + i]);

            // Print archived status
            uint8_t handle = ti_Open(files[start_disp_index + i], "r");
            fontlib_SetCursorPosition(270, TEXT_Y(i));
            if (i != cursor_position) {
                fontlib_SetForegroundColor(state->dropshadow_color);
            }
            if (ti_IsArchived(handle)) {
                fontlib_DrawString("Flash");
            }
            else {
                fontlib_DrawString("RAM");
            }
            ti_Close(handle);

        }
        fontlib_SetForegroundColor(state->text_color);
    }
}



void list_next_item(int* cursor_position, int* start_disp_index, int nb_files) {
    if (*cursor_position + *start_disp_index + 1 >= nb_files) {
        *cursor_position = 0;
        *start_disp_index = 0;
    }
    else if (*cursor_position < FILES_LIST_SIZE) {
        (*cursor_position)++;
    }
    else {
        (*start_disp_index)++;
    }
}


void list_prev_item(int* cursor_position, int* start_disp_index, int nb_files) {
    if (*cursor_position > 0) {
        (*cursor_position)--;
    }
    else if (*start_disp_index != 0) {
        (*start_disp_index)--;
    }
    else {
        if (nb_files <= FILES_LIST_SIZE) {
            *cursor_position = nb_files - 1;
            *start_disp_index = 0;
        }
        else {
            *cursor_position = FILES_LIST_SIZE;
            *start_disp_index = nb_files - FILES_LIST_SIZE - 1;
        }
    }
}



void home_menu(void) {
    gfx_BlitScreen(); // copy the screen to the buffer
    gfx_SetDrawBuffer(); // and then draw on the buffer
    static struct estate state;
    initialize_void(&state);

    static char files[64][APPVAR_NAME_LENGTH];
    int nb_files;


    int start_disp_index = 0; // indice dans la liste d'éléments de l'élément affiché le plus haut
    int cursor_position = 0; // indice du curseur sur l'écran en teme de nombre d'items

    state.alpha_state = AlphaState_alpha;
    bool reload_files = true;
    short key = 0;
    while (key != KEY_CLEAR) {

        // Go in the right IDE section if needed
        if (state.ide_goto == IDEState_Editor) {
            state.ide_goto = state.ide_go_back;
            state.ide_go_back = IDEState_Other;
            launch_editor(&state, state.filename);
            continue;
        }
        else if (state.ide_goto == IDEState_RunningProgram) {
            state.ide_goto = state.ide_go_back;
            state.ide_go_back = IDEState_Other;
            run_neon_program(&state, state.filename);
            continue;
        }

        if (reload_files) {
            nb_files = get_files(&state, files);
            // If the cursor was on the last file and a file was deleted, we need to reajust its position
            if (cursor_position + start_disp_index + 1 >= nb_files) {
                if (nb_files <= FILES_LIST_SIZE) {
                    cursor_position = nb_files - 1;
                    start_disp_index = 0;
                }
                else {
                    cursor_position = FILES_LIST_SIZE;
                    start_disp_index = nb_files - FILES_LIST_SIZE - 1;
                }
            }
        }

        draw_home_menu(&state, files, nb_files, cursor_position, start_disp_index);
        gfx_SwapDraw();

        // Go in the right IDE section if needed
        if (state.ide_goto == IDEState_Editor) {
            state.ide_goto = state.ide_go_back;
            state.ide_go_back = IDEState_Other;
            launch_editor(&state, state.filename);
            state.alpha_state = AlphaState_alpha;
            continue;
        }
        else if (state.ide_goto == IDEState_RunningProgram) {
            state.ide_goto = state.ide_go_back;
            state.ide_go_back = IDEState_Other;
            run_neon_program(&state, state.filename);
            state.alpha_state = AlphaState_alpha;
            continue;
        }

        // Otherwise wait for a key
        key = ngetchx(&state);

        if (key == KEY_F1) {
            draw_home_menu(&state, files, nb_files, cursor_position, start_disp_index);
            gfx_SwapDraw();
            char* name = show_create_dialog(&state);
            if (name != NULL) {
                ti_var_t var = ti_Open(name, "w");
                if (var != 0) {
                    ti_Write(NEON_DEFAULT_FILE_HEADER, NEON_DEFAULT_FILE_HEADER_SIZE, 1, var);
                    ti_Close(var);
                    reload_files = true;
                }
            }
        }
        else if (key == KEY_F3) {
            draw_home_menu(&state, files, nb_files, cursor_position, start_disp_index);
            gfx_SwapDraw();
            start_console(&state);
        }
        else if (nb_files > 0) {
            if (key == KEY_DOWN || key == KEY_RIGHT) { // next item
                list_next_item(&cursor_position, &start_disp_index, nb_files);
            }
            else if (key == KEY_UP || key == KEY_LEFT) { // previous item
                list_prev_item(&cursor_position, &start_disp_index, nb_files);
            }
            else if (key == KEY_F2 || key == '\n') {
                run_neon_program(&state, files[start_disp_index + cursor_position]);
                reload_files = true;
            }
            else if (key == KEY_F4) {
                launch_editor(&state, files[start_disp_index + cursor_position]);
            }
            else if (key == KEY_F5) {
                draw_home_menu(&state, files, nb_files, cursor_position, start_disp_index);
                gfx_SwapDraw();
                show_file_menu_dialog(&state, files[start_disp_index + cursor_position]);
                reload_files = true;
            }
            else if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) {
                int initial_cursor_position = cursor_position;
                int initial_start_disp_index = start_disp_index;

                do {
                    list_next_item(&cursor_position, &start_disp_index, nb_files);
                } while (files[cursor_position + start_disp_index][0] != key && !(cursor_position == initial_cursor_position && start_disp_index == initial_start_disp_index));
            }
        }
    }
}




