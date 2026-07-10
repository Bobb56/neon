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
#include "headers/tigcclib.h"

#define TEXT_Y(i)                   (14 + 14*i)

int cmp(const void *a, const void *b)
{
    const char *const *sa = a;
    const char *const *sb = b;
    return strcmp(*sa, *sb);
}

int get_files(char* files[]) {
    int nb_files = 0;

    char* var_name;
    void* vat_ptr = NULL;


    while ((var_name = ti_Detect(&vat_ptr, "NEON\x00")))
    {
        files[nb_files++] = strdup(var_name);
    }

    vat_ptr = NULL;
    while ((var_name = ti_Detect(&vat_ptr, "#NEON")))
    {
        files[nb_files++] = strdup(var_name);
    }

    qsort(files, nb_files, sizeof(char*), cmp);
    return nb_files;
}





void home_menu(void) {
    struct estate state_struct;
    struct estate* state = &state_struct;
    initialize_void(state);

    char* files[128];
    int nb_files;


    int start_disp_index = 0; // indice dans la liste d'éléments de l'élément affiché le plus haut
    int cursor_position = 0; // indice du curseur sur l'écran en teme de nombre d'items
    uint8_t vertical_limit = 15;

    bool refresh = true;
    bool reload_files = true;
    short key = 0;
    while (key != KEY_CLEAR) {
        if (reload_files) {
            nb_files = get_files(files);
            // If the cursor was on the last file and a file was deleted, we need to reajust its position
            if (cursor_position + start_disp_index + 1 >= nb_files) {
                if (nb_files < vertical_limit) {
                    cursor_position = nb_files - 1;
                    start_disp_index = 0;
                }
                else {
                    cursor_position = vertical_limit - 1;
                    start_disp_index = nb_files - vertical_limit;
                }
            }
            refresh = true;
        }

        if (refresh) {
            gfx_BlitScreen(); // copy the screen to the buffer
            gfx_SetDrawBuffer(); // and then draw on the buffer

            refresh = false;

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
            fontlib_SetCursorPosition(0, 0);
            fontlib_DrawString("Neon");

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
                if (nb_files < vertical_limit)
                    last_index = nb_files;
                else {
                    last_index = vertical_limit;
                }

                for (int i=0 ; i < last_index ; i++) {
                    // display cursor
                    if (i == cursor_position) {
                        fontlib_SetForegroundColor(state->focus_color);
                    }
                    else {
                        fontlib_SetForegroundColor(state->text_color);
                    }

                    fontlib_SetCursorPosition(3, TEXT_Y(i));
                    fontlib_DrawString(files[start_disp_index + i]);

                }
                fontlib_SetForegroundColor(state->text_color);
            }

            // copy the buffer to the secreen
            gfx_BlitBuffer();
            gfx_SetDrawScreen();
        }

        key = ngetchx();

        if (nb_files > 0 && (key == KEY_DOWN || key == KEY_RIGHT)) { // next item
            if (cursor_position + start_disp_index + 1 >= nb_files) {
                cursor_position = 0;
                start_disp_index = 0;
            }
            else if (cursor_position + 1 < vertical_limit) {
                cursor_position++;
            }
            else {
                start_disp_index++;
            }
            refresh = true;
        }
        else if (nb_files > 0 && (key == KEY_UP || key == KEY_LEFT)) { // previous item
            if (cursor_position > 0) {
                cursor_position--;
            }
            else if (start_disp_index != 0) {
                start_disp_index--;
            }
            else {
                if (nb_files < vertical_limit) {
                    cursor_position = nb_files - 1;
                    start_disp_index = 0;
                }
                else {
                    cursor_position = vertical_limit - 1;
                    start_disp_index = nb_files - vertical_limit;
                }
            }
            refresh = true;
        }
        else if (nb_files > 0 && key == '\n') {
            // TODO: Execute Neon file
            reload_files = true;
        }
        else if (key == KEY_F1) {
            gfx_SwapDraw();
            char* name = show_create_dialog(state);
            gfx_SwapDraw();
            if (name != NULL) {
                uint8_t handle = ti_Open(name, "w");
                if (handle != 0) {
                    ti_Write("NEON\x00", 5, 1, handle);
                    ti_Close(handle);
                    reload_files = true;
                }
            }
        }
        else if (key == KEY_F2) {
            gfx_SwapDraw();
            show_unimplemented_dialog(state);
            gfx_SwapDraw();
        }
        else if (key == KEY_F3) {
            gfx_SwapDraw();
            start_using_console(NULL);
            gfx_SwapDraw();
        }
        else if (key == KEY_F4) {
            launch_editor(files[start_disp_index + cursor_position]);
            refresh = true;
        }
        else if (key == KEY_F5) {
            gfx_SwapDraw();
            show_file_menu_dialog(state, files[start_disp_index + cursor_position]);
            gfx_SwapDraw();
            reload_files = true;
        }
    }
}




