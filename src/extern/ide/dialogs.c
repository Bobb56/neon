/*
dialogs.c
CEDIT project
Copyright (C) 2021  Michael "Michael0x18"

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
You may use version 2.1 or later only.
*/

#include "headers/dialogs.h"
#include "headers/clipboard.h"
#include "headers/console.h"
#include "headers/libmalloc.h"
#include "headers/state.h"
#include "headers/neonide.h"
#include "headers/editor.h"
#include <stdint.h>
#include <math.h>

#include "../../headers/syntaxhighlighting.h"

bool string_equal(struct estate *state, int24_t pointer, char* str) {
    while (*str && pointer < state->max_buffer_size - 1) {
        if (*str != state->text[pointer])
            return false;

        // Incrementation of the position in the string
        str ++;
        pointer ++;
        if (pointer == state->c1)
            pointer = state->c2 + 1;
    }
    return true;
}




//Does actual searching stufff - with three f's
void perform_search(struct estate *state)
{
    int pointer = 0;
    while (pointer < state->max_buffer_size - (int)strlen(state->search_buffer))
    {
        if (string_equal(state, pointer, state->search_buffer))
        {
            // The pointer is on the left part of the buffer
            if (pointer < state->c1) {
                while (state->c1 > pointer) {
                    cursor_left(state);
                }
            }
            else {
                while (state->c2 < pointer - 1) {
                    cursor_right(state);
                }
            }

            for (unsigned j = 0; j < strlen(state->search_buffer); j++)
            {
                cursor_right_select(state);
            }
            return;
        }

        pointer++;
        if (pointer == state->c1)
            pointer = state->c2 + 1;
    }
}

void show_search_dialog(struct estate *state)
{
    uint8_t previous_alpha_state = state->alpha_state;

    short k = 0;
    int numchars = 0;
    numchars = strlen(state->search_buffer);
    int cx = 52;
    while (true)
    {
        draw_dialog(state, 20, 60, 280, 100);
        gfx_SetColor(state->border_color);
        gfx_HorizLine_NoClip(20, 80, 280);
        //fontlib_SetCursorPosition(115,80);
        fontlib_SetCursorPosition(115, 65);
        fontlib_DrawString("Search");

        fontlib_SetCursorPosition(250, 65);
        if (state->alpha_state == AlphaState_alpha) {
            fontlib_DrawString("alpha");
        }
        else if (state->alpha_state == AlphaState_ALPHA) {
            fontlib_DrawString("ALPHA");
        }

        fontlib_SetCursorPosition(30, 90);
        fontlib_DrawString("Enter string to search for:");
        gfx_SetColor(state->dropshadow_color);
        gfx_Rectangle_NoClip(51, 111, 220, 16);
        gfx_SetColor(state->border_color);
        gfx_Rectangle_NoClip(50, 110, 220, 16);
        fontlib_SetCursorPosition(52, 112);
        fontlib_DrawString(state->search_buffer);
        gfx_VertLine(52 + FONT_WIDTH * numchars, 112, 12);
        cx = 52 + FONT_WIDTH * numchars;
        gfx_BlitBuffer();
        k = ngetchx_xy(state, cx, 112);

        if (k == KEY_CLEAR)
        {
            goto show_search_dialog_return;
        }
        if (k == '\n')
        {
            if (!numchars)
            {
                goto show_search_dialog_return;
            }
            //TODO search for stuff
            perform_search(state);
            goto show_search_dialog_return;
        }
        if (!is_control(k))
        {
            if (numchars < 26)
            {
                state->search_buffer[numchars] = k;
                numchars++;
            }
        }
        if (k == KEY_BS)
        {
            if (numchars)
                state->search_buffer[--numchars] = 0;
        }
    }

show_search_dialog_return:
    state->alpha_state = previous_alpha_state;
    return;
}


void goto_line(struct estate* state, int24_t line_number) {
    if (line_number < 1)
        line_number = 1;

    if (state->lc1 + 1 > line_number) {
        while (state->lc1 + 1 > line_number) {
            cursor_up(state);
        }
    }
    else if (state->lc1 + 1 < line_number) {
        while (state->lc1 + 1 < line_number && state->c2 < state->max_buffer_size - 1) {
            cursor_down(state);
        }
    }
}



void show_goto_dialog(struct estate *state)
{
    uint8_t previous_alpha_state = state->alpha_state;
    state->alpha_state = AlphaState_NoALpha;

    short k = 0;
    int numchars = 0;
    char buffer[6] = {0};
    int cx = 52;
    while (true)
    {
        draw_dialog(state, 20, 60, 280, 100);
        gfx_SetColor(state->border_color);
        gfx_HorizLine_NoClip(20, 80, 280);
        //fontlib_SetCursorPosition(115,80);
        fontlib_SetCursorPosition(95, 65);
        fontlib_DrawString("Goto line");

        fontlib_SetCursorPosition(250, 65);
        if (state->alpha_state == AlphaState_alpha) {
            fontlib_DrawString("alpha");
        }
        else if (state->alpha_state == AlphaState_ALPHA) {
            fontlib_DrawString("ALPHA");
        }

        fontlib_SetCursorPosition(30, 90);
        fontlib_DrawString("Enter line number:");
        gfx_SetColor(state->dropshadow_color);
        gfx_Rectangle_NoClip(51, 111, 220, 16);
        gfx_SetColor(state->border_color);
        gfx_Rectangle_NoClip(50, 110, 220, 16);
        fontlib_SetCursorPosition(52, 112);
        fontlib_DrawString(buffer);
        gfx_VertLine(52 + FONT_WIDTH * numchars, 112, 12);
        cx = 52 + FONT_WIDTH * numchars;
        gfx_BlitBuffer();
        k = ngetchx_xy(state, cx, 112);
        if (k == KEY_CLEAR)
        {
            goto show_goto_dialog_return;
        }
        if (k == '\n')
        {
            if (!numchars)
            {
                goto show_goto_dialog_return;
            }

            int24_t line_number = 0;
            for (uint8_t index = 0 ; index < numchars ; index ++) {
                line_number *= 10;
                line_number += buffer[index] - '0';
            }

            goto_line(state, line_number);
            goto show_goto_dialog_return;
        }
        if (k >= '0' && k <= '9')
        {
            if (numchars < 4)
            {
                buffer[numchars] = k;
                numchars++;
            }
        }
        if (k == KEY_BS)
        {
            if (numchars)
                buffer[--numchars] = 0;
        }
    }
show_goto_dialog_return:
    state->alpha_state = previous_alpha_state;
    return;
}



uint8_t show_color_selection_dialog(struct estate *state, uint8_t current_value)
{
    short k = 0;
    uint8_t temp_transparent_color = state->transparent_color;
    short index = current_value;
    while (k != KEY_CLEAR)
    {
        state->transparent_color = temp_transparent_color;
        draw_dialog(state, 20, 20, 280, 200);
        gfx_SetColor(state->border_color);
        gfx_HorizLine_NoClip(20, 40, 280);
        fontlib_SetCursorPosition(110, 25);
        fontlib_SetForegroundColor(state->text_color);
        fontlib_DrawString("Select Color");
        fontlib_SetCursorPosition(31, 42);
        fontlib_DrawString("Selected: ");
        fontlib_DrawUInt(index, 1);
        state->transparent_color = state->background_color;
        const int J_MAX = 8;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                gfx_SetColor(32 * i + j);
                gfx_FillRectangle_NoClip(32 + 8 * j, 72 + 8 * i, 8, 8);
            }
        }
        gfx_SetColor(state->border_color);
        gfx_Rectangle_NoClip(32 + 8 * (index % 32), 72 + 8 * (index >> 5), 8, 8);
        gfx_Rectangle_NoClip(31 + 8 * (index % 32), 71 + 8 * (index >> 5), 10, 10);
        gfx_BlitBuffer();
        k = ngetchx(state);
        if (k == KEY_CLEAR)
        {
            return current_value;
        }
        if (k == KEY_RIGHT)
        {
            index++;
        }
        if (k == KEY_LEFT)
        {
            index--;
        }
        if (k == KEY_UP)
        {
            index -= 256 / J_MAX;
        }
        if (k == KEY_DOWN)
        {
            index += 256 / J_MAX;
        }
        if (k == '\n')
        {
            state->transparent_color = temp_transparent_color;
            return index;
        }
        index += 256;
        index %= 256;
    }
    return current_value;
}



void draw_switch(struct estate *state, int x, int y, bool on){
    gfx_SetColor(on?state->focus_color:state->background_color);
    gfx_FillCircle_NoClip(x+7,y+7,7);
    gfx_FillCircle_NoClip(x+21,y+7,7);
    gfx_SetColor(state->border_color);
    gfx_Circle_NoClip(x+7,y+7,7);
    gfx_Circle_NoClip(x+21,y+7,7);
    gfx_HorizLine_NoClip(x+7,y,14);
    gfx_HorizLine_NoClip(x+7,y+14,14);
    gfx_SetColor(on?state->focus_color:state->background_color);
    gfx_FillRectangle_NoClip(x+7,y+1,14,13);
    gfx_SetColor(state->statusbar_color);
    gfx_FillCircle_NoClip(x+(on?21:7),y+7,7);
    gfx_SetColor(state->border_color);
    gfx_Circle_NoClip(x+(on?21:7),y+7,7);
}


void show_editor_settings_dialog(struct estate *state)
{
    short k = 0;
    short index = 0;
    while (k != KEY_CLEAR)
    {
        draw_dialog(state, 20, 20, 280, 200);
        gfx_SetColor(state->border_color);
        gfx_HorizLine_NoClip(20, 40, 280);
        fontlib_SetCursorPosition(100, 25);
        fontlib_SetForegroundColor(state->text_color);
        fontlib_DrawString("Editor Settings");
        //~~//~~//~~//~~//~~//~~//~~//~~//~~//~~//
        draw_switch(state, 24, 42, state->autoarchive); //autoarchive
        fontlib_SetForegroundColor(index == 0 ? state->focus_color : state->text_color);
        fontlib_SetCursorPosition(24 + 32, 43);
        fontlib_DrawString("Auto archive files");
        draw_switch(state, 24, 60, state->saveprompt); //saveprompt
        fontlib_SetForegroundColor(index == 1 ? state->focus_color : state->text_color);
        fontlib_SetCursorPosition(24 + 32, 61);
        fontlib_DrawString("Show save prompt on exit");
        draw_switch(state, 24, 78, state->useregex); //regex
        fontlib_SetForegroundColor(index == 2 ? state->focus_color : state->text_color);
        fontlib_SetCursorPosition(24 + 32, 79);
        fontlib_DrawString("Use regex in search");
        draw_switch(state, 24, 96, state->blinkcursor); //blinkcursor
        fontlib_SetForegroundColor(index == 3 ? state->focus_color : state->text_color);
        fontlib_SetCursorPosition(24 + 32, 97);
        fontlib_DrawString("Blink cursor");
        draw_switch(state, 24, 96 + 18, state->backupfiles); //backup
        fontlib_SetForegroundColor(index == 4 ? state->focus_color : state->text_color);
        fontlib_SetCursorPosition(24 + 32, 96 + 19);
        fontlib_DrawString("Copy on write");
        draw_switch(state, 24, 96 + 54, state->hide_special_files); //extra_buffer
        fontlib_SetForegroundColor(index == 6 ? state->focus_color : state->text_color);
        fontlib_SetCursorPosition(24 + 32, 96 + 55);
        fontlib_DrawString("Hide special files");
        fontlib_SetForegroundColor(state->text_color);
        fontlib_SetCursorPosition(24, 96 + 55 + 18);
        fontlib_DrawString("Permanent changes can be made");
        fontlib_SetCursorPosition(24, 96 + 55 + 30);
        fontlib_DrawString("by editing NEIDERC");
        //~~//~~//~~//~~//~~//~~//~~//~~//~~//~~//
        gfx_BlitBuffer();
        k = ngetchx(state);
        if (k == KEY_DOWN)
        {
            index++;
            if (index >= 7)
                index = 6;
        }
        if (k == KEY_UP)
        {
            index--;
            if (index < 0)
            {
                index = 0;
            }
        }
        if (k == KEY_RIGHT || k == '\n')
        {
            switch (index)
            {
            case 0:
                state->autoarchive = !(state->autoarchive);
                break;
            case 1:
                state->saveprompt = !(state->saveprompt);
                break;
            case 2:
                state->useregex = !(state->useregex);
                break;
            case 3:
                state->blinkcursor = !(state->blinkcursor);
                break;
            case 4:
                state->backupfiles = !(state->backupfiles);
                break;
            case 5:
                break;
            case 6:
                state->hide_special_files = !(state->hide_special_files);
                break;
            }
        }
    }
}

void show_keybind_dialog(struct estate *state)
{
    //short k = 0;
    draw_dialog(state, 40, 40, 240, 160);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(40, 60, 240);
    fontlib_SetCursorPosition(80, 45);
    fontlib_SetForegroundColor(state->text_color);
    fontlib_DrawString("Keybind Information");
    fontlib_SetCursorPosition(51, 62);
    fontlib_DrawString("Press [CLEAR] to quit.");
    fontlib_SetCursorPosition(51, 74);
    fontlib_DrawString("Hold modifiers like 2nd.");
    fontlib_SetCursorPosition(51, 86);
    fontlib_DrawString("See docs for more info.");
    gfx_BlitBuffer();
    ngetchx(state);
}

void show_appearance_settings_dialog(struct estate *state)
{
    short k = 0;
    int index = 0;
    while (k != KEY_CLEAR)
    {
        draw_dialog(state, 20, 20, 280, 200);
        gfx_SetColor(state->border_color);
        gfx_HorizLine_NoClip(20, 40, 280);
        fontlib_SetCursorPosition(84, 25);
        fontlib_SetForegroundColor(state->text_color);
        fontlib_DrawString("Appearance Settings");

        fontlib_SetCursorPosition(45, 45);
        fontlib_SetForegroundColor(
            index == 0 ? state->focus_color : state->text_color);
        fontlib_DrawString("Text color");

        fontlib_SetCursorPosition(45, 57);
        fontlib_SetForegroundColor(
            index == 1 ? state->focus_color : state->text_color);
        fontlib_DrawString("Text highlight color");

        fontlib_SetCursorPosition(45, 69);
        fontlib_SetForegroundColor(
            index == 2 ? state->focus_color : state->text_color);
        fontlib_DrawString("Text selection color");

        fontlib_SetCursorPosition(45, 81);
        fontlib_SetForegroundColor(
            index == 3 ? state->focus_color : state->text_color);
        fontlib_DrawString("Text selection highlight color");

        fontlib_SetCursorPosition(45, 93);
        fontlib_SetForegroundColor(
            index == 4 ? state->focus_color : state->text_color);
        fontlib_DrawString("Background color");

        fontlib_SetCursorPosition(45, 105);
        fontlib_SetForegroundColor(
            index == 5 ? state->focus_color : state->text_color);
        fontlib_DrawString("Transparent color");

        fontlib_SetCursorPosition(45, 117);
        fontlib_SetForegroundColor(
            index == 6 ? state->focus_color : state->text_color);
        fontlib_DrawString("Status bar color");

        fontlib_SetCursorPosition(45, 129);
        fontlib_SetForegroundColor(
            index == 7 ? state->focus_color : state->text_color);
        fontlib_DrawString("Status bar text color");

        fontlib_SetCursorPosition(45, 141);
        fontlib_SetForegroundColor(
            index == 8 ? state->focus_color : state->text_color);
        fontlib_DrawString("Border color");

        fontlib_SetCursorPosition(45, 153);
        fontlib_SetForegroundColor(
            index == 9 ? state->focus_color : state->text_color);
        fontlib_DrawString("Drop shadow color");

        fontlib_SetCursorPosition(45, 165);
        fontlib_SetForegroundColor(
            index == 10 ? state->focus_color : state->text_color);
        fontlib_DrawString("Focus color");

        gfx_SetColor(state->text_color);
        gfx_FillRectangle_NoClip(31, 46, 10, 10);
        gfx_SetColor(state->text_highlight_color);
        gfx_FillRectangle_NoClip(31, 58, 10, 10);
        gfx_SetColor(state->text_selection_color);
        gfx_FillRectangle_NoClip(31, 70, 10, 10);
        gfx_SetColor(state->text_selection_highlight_color);
        gfx_FillRectangle_NoClip(31, 82, 10, 10);
        //sike!
        //gfx_SetColor(state->background_color);
        //gfx_FillRectangle_NoClip(31, 94, 10, 10);
        //Sike!
        //gfx_SetColor(state->transparent_color);
        //gfx_FillRectangle_NoClip(31, 106, 10, 10);
        gfx_SetColor(state->statusbar_color);
        gfx_FillRectangle_NoClip(31, 118, 10, 10);
        gfx_SetColor(state->statusbar_text_color);
        gfx_FillRectangle_NoClip(31, 130, 10, 10);
        gfx_SetColor(state->border_color);
        gfx_FillRectangle_NoClip(31, 142, 10, 10);
        gfx_SetColor(state->dropshadow_color);
        gfx_FillRectangle_NoClip(31, 154, 10, 10);
        gfx_SetColor(state->focus_color);
        gfx_FillRectangle_NoClip(31, 166, 10, 10);

        fontlib_SetForegroundColor(state->text_color);
        fontlib_SetCursorPosition(31, 178);
        fontlib_DrawString("These settings are temporary.");
        fontlib_SetCursorPosition(31, 190);
        fontlib_DrawString("Edit NEIDERC to make changes");
        fontlib_SetCursorPosition(31, 202);
        fontlib_DrawString("permanent.");

        gfx_BlitBuffer();
        k = ngetchx(state);
        if (k == KEY_RIGHT || k == '\n')
        {
            switch (index)
            {
            case 0:
                state->text_color = show_color_selection_dialog(state, state->text_color);
                /*gfx_FillScreen(255);
                fontlib_SetForegroundColor(state->text_color);
                fontlib_SetCursorPosition(0,0);
                fontlib_DrawUInt(state->text_color,1);
                gfx_BlitBuffer();
                ngetchx(state);*/
                break;
            case 1:
                state->text_highlight_color = show_color_selection_dialog(state, state->text_highlight_color);
                break;
            case 2:
                state->text_selection_color = show_color_selection_dialog(state, state->text_selection_color);
                break;
            case 3:
                state->text_selection_highlight_color = show_color_selection_dialog(state, state->text_selection_highlight_color);
                break;
            case 4:
                state->background_color = show_color_selection_dialog(state, state->background_color);
                break;
            case 5:
                state->transparent_color = show_color_selection_dialog(state, state->transparent_color);
                break;
            case 6:
                state->statusbar_color = show_color_selection_dialog(state, state->statusbar_color);
                break;
            case 7:
                state->statusbar_text_color = show_color_selection_dialog(state, state->statusbar_text_color);
                break;
            case 8:
                state->border_color = show_color_selection_dialog(state, state->border_color);
                break;
            case 9:
                state->dropshadow_color = show_color_selection_dialog(state, state->dropshadow_color);
                break;
            case 10:
                state->focus_color = show_color_selection_dialog(state, state->focus_color);
                break;
            }
        }
        if (k == KEY_UP)
        {
            if (index)
            {
                index--;
            }
        }
        if (k == KEY_DOWN)
        {
            if (index < 10)
            {
                index++;
            }
        }
    }
}

//TODO embed color picker in dialog
void show_appearance_settings_dialog_nocolor(struct estate *state)
{
    short k = 0;
    int index = 0;
    while (k != KEY_CLEAR)
    {
        draw_dialog(state, 20, 20, 280, 200);
        gfx_SetColor(state->border_color);
        gfx_HorizLine_NoClip(20, 40, 280);
        fontlib_SetCursorPosition(84, 25);
        fontlib_SetForegroundColor(state->text_color);
        fontlib_DrawString("Appearance Settings");
        fontlib_SetCursorPosition(45, 45);
        fontlib_SetForegroundColor(
            index == 0 ? state->focus_color : state->text_color);
        fontlib_DrawString("Text color");

        fontlib_SetCursorPosition(45, 57);
        fontlib_SetForegroundColor(
            index == 1 ? state->focus_color : state->text_color);
        fontlib_DrawString("Text highlight color");

        fontlib_SetCursorPosition(45, 69);
        fontlib_SetForegroundColor(
            index == 2 ? state->focus_color : state->text_color);
        fontlib_DrawString("Text selection color");

        fontlib_SetCursorPosition(45, 81);
        fontlib_SetForegroundColor(
            index == 3 ? state->focus_color : state->text_color);
        fontlib_DrawString("Text selection highlight color");

        fontlib_SetCursorPosition(45, 93);
        fontlib_SetForegroundColor(
            index == 4 ? state->focus_color : state->text_color);
        fontlib_DrawString("Background color");

        fontlib_SetCursorPosition(45, 105);
        fontlib_SetForegroundColor(
            index == 5 ? state->focus_color : state->text_color);
        fontlib_DrawString("Transparent color");

        fontlib_SetCursorPosition(45, 117);
        fontlib_SetForegroundColor(
            index == 6 ? state->focus_color : state->text_color);
        fontlib_DrawString("Status bar color");

        fontlib_SetCursorPosition(45, 129);
        fontlib_SetForegroundColor(
            index == 7 ? state->focus_color : state->text_color);
        fontlib_DrawString("Status bar text color");

        fontlib_SetCursorPosition(45, 141);
        fontlib_SetForegroundColor(
            index == 8 ? state->focus_color : state->text_color);
        fontlib_DrawString("Border color");

        fontlib_SetCursorPosition(45, 153);
        fontlib_SetForegroundColor(
            index == 9 ? state->focus_color : state->text_color);
        fontlib_DrawString("Drop shadow color");

        fontlib_SetCursorPosition(45, 165);
        fontlib_SetForegroundColor(
            index == 10 ? state->focus_color : state->text_color);
        fontlib_DrawString("Focus color");

        gfx_SetColor(state->text_color);
        gfx_FillRectangle_NoClip(31, 46, 10, 10);
        gfx_SetColor(state->text_highlight_color);
        gfx_FillRectangle_NoClip(31, 58, 10, 10);
        gfx_SetColor(state->text_selection_color);
        gfx_FillRectangle_NoClip(31, 70, 10, 10);
        gfx_SetColor(state->text_selection_highlight_color);
        gfx_FillRectangle_NoClip(31, 82, 10, 10);
        //sike!
        //gfx_SetColor(state->background_color);
        //gfx_FillRectangle_NoClip(31, 94, 10, 10);
        //Sike!
        //gfx_SetColor(state->transparent_color);
        //gfx_FillRectangle_NoClip(31, 106, 10, 10);
        gfx_SetColor(state->statusbar_color);
        gfx_FillRectangle_NoClip(31, 118, 10, 10);
        gfx_SetColor(state->statusbar_text_color);
        gfx_FillRectangle_NoClip(31, 130, 10, 10);
        gfx_SetColor(state->border_color);
        gfx_FillRectangle_NoClip(31, 142, 10, 10);
        gfx_SetColor(state->dropshadow_color);
        gfx_FillRectangle_NoClip(31, 154, 10, 10);
        gfx_SetColor(state->focus_color);
        gfx_FillRectangle_NoClip(31, 166, 10, 10);

        fontlib_SetForegroundColor(state->text_color);
        fontlib_SetCursorPosition(31, 178);
        fontlib_DrawString("These settings are temporary.");
        fontlib_SetCursorPosition(31, 190);
        fontlib_DrawString("Edit NEIDERC to make changes");
        fontlib_SetCursorPosition(31, 202);
        fontlib_DrawString("permanent.");

        gfx_BlitBuffer();
        k = ngetchx(state);
        if (k == KEY_RIGHT || k == '\n')
        {
            show_color_selection_dialog(state, 1);
        }
        if (k == KEY_UP)
        {
            if (index)
            {
                index--;
            }
        }
        if (k == KEY_DOWN)
        {
            if (index < 10)
            {
                index++;
            }
        }
    }
}



/*
 * Draws the Tools menu given the state parameters and the selection index
 */
void console_tools_backend_draw(struct estate *state, int index)
{
    draw_dialog(state, 100, 145, 110, 73);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(100, 165, 110);
    fontlib_SetCursorPosition(125, 150);
    fontlib_SetForegroundColor(state->text_color);
    fontlib_DrawString("Tools");
    fontlib_SetCursorPosition(106, 170);
    if (index == 0)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("1) Copy");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(106, 185);
    if (index == 1)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("2) Cut");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(106, 200);
    if (index == 2)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("3) Paste");
    fontlib_SetForegroundColor(state->text_color);
}



/*
 * Draws the Tools menu given the state parameters and the selection index
 */
void editor_tools_backend_draw(struct estate *state, int index)
{
    draw_dialog(state, 100, 115, 110, 103);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(100, 135, 110);
    fontlib_SetCursorPosition(125, 120);
    fontlib_SetForegroundColor(state->text_color);
    fontlib_DrawString("Tools");
    fontlib_SetCursorPosition(106, 140);
    if (index == 0)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("1) Goto line");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(106, 155);
    if (index == 1)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("2) Search");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(106, 170);
    if (index == 2)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("3) Copy");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(106, 185);
    if (index == 3)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("4) Cut");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(106, 200);
    if (index == 4)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("5) Paste");
}

void show_persistence_dialog(struct estate *state)
{
    //short k = 0;
    draw_dialog(state, 20, 20, 280, 200);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(20, 40, 280);
    fontlib_SetCursorPosition(100, 25);
    fontlib_SetForegroundColor(state->text_color);
    fontlib_DrawString("Editor Settings");
    fontlib_SetCursorPosition(31, 42);
    fontlib_DrawString("Changing settings graphically is");
    fontlib_SetCursorPosition(31, 54);
    fontlib_DrawString("not currently supported.");
    fontlib_SetCursorPosition(31, 66);
    fontlib_DrawString("Please edit NEIDERC instead.");
    gfx_BlitBuffer();
    ngetchx(state);
}

const int n_chars = 14;
static const char chars[] = {
    '#',
    '~',
    '\'',
    '\\',
    '<',
    '>',
    '=',
    '%',
    '&',
    '@',
    '!',
    ';',
    '$',
    '_'
};

void chars_backend_draw(struct estate* state, int line, int col, int n_lines, int n_columns) {
    //70, 70
    draw_dialog(state, 5, 120, 180, 100);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(5, 140, 180);
    fontlib_SetCursorPosition(15, 125);
    fontlib_SetForegroundColor(state->text_color);
    fontlib_DrawString("Character selection");

    const uint8_t space = 16;

    for (int i=0 ; i < n_chars ; i++) {
        int l = i/n_lines;
        int c = i%n_columns;

        fontlib_SetCursorPosition(60 + space * c, 150 + space * l);

        if (l == line && c == col) {
            fontlib_SetTransparency(false);
            fontlib_SetForegroundColor(state->text_highlight_color);
            fontlib_SetBackgroundColor(state->focus_color);
        }
        fontlib_DrawGlyph(chars[i]);
        fontlib_SetForegroundColor(state->text_color);
        fontlib_SetBackgroundColor(state->text_highlight_color);
    }
}


void show_chars_dialog(struct estate* state, void (*draw_background)(struct estate*)) {
    const uint8_t n_lines = 4;
    const uint8_t n_columns = 4;

    int line = 0, col = 0;
    short k = 0;
    while (k != KEY_CLEAR && k != KEY_F2)
    {
        if (k == '\n')
        {
            insert_char(state, chars[line * n_columns + col]);
        }
        else if (k == KEY_BS) {
            bs(state);
        }
        else if (k == KEY_DEL) {
            del(state);
        }
        else if (k == KEY_UP)
        {
            if (line == 0)
                line = n_lines - 1;
            else
                 line--;

            if (line * n_columns + col >= n_chars)
                line--;
        }
        else if (k == KEY_DOWN)
        {
            line++;
            if (line * n_columns + col >= n_chars)
                line = 0;
        }
        else if (k == KEY_LEFT)
        {
            if (col == 0)
                col = n_columns - 1;
            else
                 col--;

            if (line * n_columns + col >= n_chars)
                col = (n_chars - 1)%n_columns;
        }
        else if (k == KEY_RIGHT)
        {
            col++;
            if (col == n_columns)
                col = 0;

            if (line * n_columns + col >= n_chars)
                col = 0;
        }
        draw_background(state);
        chars_backend_draw(state, line, col, n_lines, n_columns);
        gfx_BlitBuffer();
        k = ngetchx(state);
    }
}



void options_backend_draw(struct estate *state, int index)
{
    draw_dialog(state, 7, 93, 200, 125);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(7, 113, 200);
    fontlib_SetCursorPosition(45, 98);
    fontlib_SetForegroundColor(state->text_color);
    fontlib_DrawString("NeonIDE options");
    fontlib_SetCursorPosition(14, 118);
    if (index == 0)
    {
        fontlib_SetForegroundColor(state->focus_color);
        //fontlib_DrawString(">");
    }
    fontlib_DrawString("1) Save file");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(14, 133);
    if (index == 1)
    {
        fontlib_SetForegroundColor(state->focus_color);
        //fontlib_DrawString(">");
    }
    fontlib_DrawString("2) Save file as");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(14, 148);
    if (index == 2)
    {
        fontlib_SetForegroundColor(state->focus_color);
        //fontlib_DrawString(">");
    }
    fontlib_DrawString("3) Editor settings");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(14, 163);
    if (index == 3)
    {
        fontlib_SetForegroundColor(state->focus_color);
        //fontlib_DrawString(">");
    }
    fontlib_DrawString("4) Appearance Settings");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(14, 178);
    if (index == 4)
    {
        fontlib_SetForegroundColor(state->focus_color);
        //fontlib_DrawString(">");
    }
    fontlib_DrawString("5) Persistence Settings");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(14, 193);
    if (index == 5)
    {
        fontlib_SetForegroundColor(state->focus_color);
        //fontlib_DrawString(">");
    }
    fontlib_DrawString("6) Close editor");
}


int show_options_dialog(struct estate *state)
{
    short k = 0;
    int index = 0;
    while (k != KEY_CLEAR && k != KEY_F1)
    {
        if (k == '\n' || k == KEY_RIGHT || (k >= '1' && k <= '6'))
        {
            if (k >= '1' && k <= '6') {
                index = k - '1';
            }

            switch (index)
            {
            case 0:
                write_file(state);
                return 0;
            case 1:
                draw_editor(state);
                gfx_SwapDraw();
                if (!show_save_dialog(state)) {
                    write_file(state);
                    return 0;
                }
                break;
            case 2:
                show_editor_settings_dialog(state);
                return 0;
            case 3:
                show_appearance_settings_dialog(state);
                return 0;
            case 4:
                show_persistence_dialog(state);
                return 0;
            case 5:
                draw_editor(state);
                gfx_SwapDraw();
                bool result = state->saved || show_unsaved_dialog(state);
                draw_editor(state);
                gfx_SwapDraw();
                if (result)
                    return -1;
                break;
                break;
            }
        }
        if (k == KEY_UP)
        {
            if (index)
            {
                index--;
            }
        }
        if (k == KEY_DOWN)
        {
            if (index < 5)
            {
                index++;
            }
        }
        options_backend_draw(state, index);

        gfx_BlitBuffer();
        k = ngetchx(state);
    }
    return 0;
}



void show_console_tools_dialog(struct estate *state)
{
    short k = 0;
    int index = 0;
    while (k != KEY_CLEAR && k != KEY_F3)
    {
        if (k == '\n' || k == KEY_RIGHT || (k >= '1' && k <= '3'))
        {
            if (k >= '1' && k <= '3')
                index = k - '1';

            switch (index)
            {
            case 0:
                cb_copy(state);
                return;
            case 1:
                cb_cut(state);
                return;
            case 2:
                cb_paste(state);
                return;
            }
        }
        if (k == KEY_UP)
        {
            if (index)
            {
                index--;
            }
        }
        if (k == KEY_DOWN)
        {
            if (index < 2)
            {
                index++;
            }
        }
        console_tools_backend_draw(state, index);

        gfx_BlitBuffer();
        k = ngetchx(state);
    }
}




void show_editor_tools_dialog(struct estate *state)
{
    short k = 0;
    int index = 0;
    while (k != KEY_CLEAR && k != KEY_F3)
    {
        if (k == '\n' || k == KEY_RIGHT || (k >= '1' && k <= '5'))
        {
            if (k >= '1' && k <= '5')
                index = k - '1';

            switch (index)
            {
            case 0:
                show_goto_dialog(state);
                return;
            case 1:
                show_search_dialog(state);
                return;
            case 2:
                cb_copy(state);
                return;
            case 3:
                cb_cut(state);
                return;
            case 4:
                cb_paste(state);
                return;
            }
        }
        if (k == KEY_UP)
        {
            if (index)
            {
                index--;
            }
        }
        if (k == KEY_DOWN)
        {
            if (index < 4)
            {
                index++;
            }
        }
        editor_tools_backend_draw(state, index);

        gfx_BlitBuffer();
        k = ngetchx(state);
    }
}

void draw_dialog(struct estate *state, int x, int y, int w, int h)
{
    gfx_SetColor(state->dropshadow_color);
    gfx_FillCircle_NoClip(5 + x + state->corner_radius,
                          5 + y + state->corner_radius, state->corner_radius);
    gfx_FillCircle_NoClip(5 + x + w - state->corner_radius - 1,
                          5 + y + state->corner_radius, state->corner_radius);
    gfx_FillCircle_NoClip(5 + x + state->corner_radius,
                          5 + y + h - state->corner_radius - 1, state->corner_radius);
    gfx_FillCircle_NoClip(5 + x + w - state->corner_radius - 1,
                          5 + y + h - state->corner_radius - 1, state->corner_radius);
    gfx_FillRectangle_NoClip(5 + x + state->corner_radius, 5 + y,
                             w - state->corner_radius - state->corner_radius, h);
    gfx_FillRectangle_NoClip(5 + x, 5 + y + state->corner_radius, w,
                             h - state->corner_radius - state->corner_radius);

    gfx_SetColor(state->border_color);
    gfx_Circle_NoClip(x + state->corner_radius, y + state->corner_radius,
                      state->corner_radius);
    gfx_Circle_NoClip(x + w - state->corner_radius - 1,
                      y + state->corner_radius, state->corner_radius);
    gfx_Circle_NoClip(x + state->corner_radius,
                      y + h - state->corner_radius - 1, state->corner_radius);
    gfx_Circle_NoClip(x + w - state->corner_radius - 1,
                      y + h - state->corner_radius - 1, state->corner_radius);
    gfx_Rectangle_NoClip(x + state->corner_radius, y,
                         w - state->corner_radius - state->corner_radius, h);
    gfx_Rectangle_NoClip(x, y + state->corner_radius, w,
                         h - state->corner_radius - state->corner_radius);

    gfx_SetColor(state->background_color);
    gfx_FillCircle_NoClip(x + state->corner_radius, y + state->corner_radius,
                          state->corner_radius - 1);
    gfx_FillCircle_NoClip(x + w - state->corner_radius - 1,
                          y + state->corner_radius, state->corner_radius - 1);
    gfx_FillCircle_NoClip(x + state->corner_radius,
                          y + h - state->corner_radius - 1, state->corner_radius - 1);
    gfx_FillCircle_NoClip(x + w - state->corner_radius - 1,
                          y + h - state->corner_radius - 1, state->corner_radius - 1);
    gfx_FillRectangle_NoClip(x + state->corner_radius + 1, y + 1,
                             w - state->corner_radius - state->corner_radius + 2, h - 2);
    gfx_FillRectangle_NoClip(x + 1, y + state->corner_radius - 1, w - 2,
                             h - state->corner_radius - state->corner_radius + 2);
}


/*
Makes the state consistent for coloring a string inside a bigger string by
processing the previous part of the string
*/
void update_state_from_beginning(struct estate* state) {
    static int last_scr_offset = -1;
    
    if (last_scr_offset != state->scr_offset) {
        // We process the entire beginning of the buffer
        sh_reset_initial_state();
        int i=0;
        while (i < state->scr_offset) {
            if (i == state->c1) {
                i = state->c2 + 1;
            }
            preprocessing_update_state(sh_get_state_ptr(), state->text[i]);
            i++;
        }
        last_scr_offset = state->scr_offset;
    }
}



uint8_t* editor_highlight(struct estate* state) {
    // Make the state consistent with the text currently on the screen
    update_state_from_beginning(state);
    // Initialize a highlighting process for highlighting from the current state
    init_sh_process(NUM_LINES * NUM_COLS);

    // Loop over the characters
    int24_t i = state->scr_offset;
    int8_t row = 0;
    int8_t col = 0;
    int24_t cp = 0;

    //Iterate buffer
    while (i < state->max_buffer_size && (cp < state->max_buffer_size - state->c2 + state->c1) && row < NUM_LINES + 1)
    {
        if (i == state->c1)
        {
            i = state->c2 + 1;
            if (i >= state->max_buffer_size)
                break;
        }

        if (state->text[i] == '\n')
        {
            update_state(sh_get_state_ptr(), '\n');
            row++;
            col = 0;
            i++;
            cp++;
            continue;
        }

        // We create a new line only if we truly need to display characters so
        // that if the last character of a line is \n there will be only one new line
        if (col >= NUM_COLS) {
            if (row == NUM_LINES)
                break;

            col = 0;
            row++;
        }
        update_state(sh_get_state_ptr(), state->text[i]);
        i++;
        col++;
        cp++;
    }
    close_state(sh_get_state_ptr());
    return sh_get_state_ptr()->colors;
}


void draw_text_area(struct estate* state) {
    //uint8_t* colors;
beginning:

    //colors = editor_highlight(state);
    gfx_FillScreen(state->background_color);

    // Number of pixels between the left border and the first character of a line 
    const uint8_t left_offset = 2;
    
    //Initialize temporary variables
    uint8_t text_color = state->text_color;
    int24_t i = state->scr_offset;
    int8_t row = 0;
    int8_t col = 0;
    int24_t cp = 0;
    bool drawn = false;
    uint8_t current_text_color = state->text_color;

    //Start drawing
    fontlib_SetForegroundColor(text_color);
    fontlib_SetCursorPosition(left_offset, LINE_SPACING);

    //Iterate buffer
    while (i < state->max_buffer_size && (cp < state->max_buffer_size - state->c2 + state->c1) && row < NUM_LINES + 1)
    {

        //if (colors[cp] != NO_COLOR) {
        //    current_text_color = translate_color(colors[cp]);
        //}

        if (i == state->c1)
        {
            gfx_VertLine_NoClip(left_offset + FONT_WIDTH * col, LINE_SPACING * row + LINE_SPACING, LINE_SPACING);
            state->cx = left_offset + FONT_WIDTH * col;
            state->cy = LINE_SPACING * row + LINE_SPACING;

            i = state->c2 + 1;
            drawn = true;
            if (i >= state->max_buffer_size)
                break;
        }

        if (state->text[i] == '\n')
        {
            row++;
            col = 0;
            i++;
            cp++;
            fontlib_SetCursorPosition(left_offset, LINE_SPACING * row + LINE_SPACING);
            continue;
        }
        if (state->selection_active && ((state->selection_anchor <= i && i < state->c1) || (state->selection_anchor >= i && i > state->c2)))
        {
            fontlib_SetForegroundColor(state->text_selection_color);
            fontlib_SetBackgroundColor(state->text_selection_highlight_color);
            fontlib_SetTransparency(false);
        }
        else
        {
            fontlib_SetForegroundColor(current_text_color);
            fontlib_SetBackgroundColor(state->text_highlight_color);
            fontlib_SetTransparency(true);
        }

        // We create a new line only if we truly need to display characters so
        // that if the last character of a line is \n there will be only one new line
        if (col >= NUM_COLS) {
            if (row == NUM_LINES)
                break;

            col = 0;
            row++;
            fontlib_SetCursorPosition(left_offset, LINE_SPACING * row + LINE_SPACING);
        }
        fontlib_DrawGlyph(state->text[i]);
        i++;
        col++;
        cp++;
    }

    if (!drawn)
    {
        if (state->c1 < state->scr_offset)
        {
            scroll_up(state);
        }
        else
        {
            scroll_down(state);
        }
        goto beginning;
    }
    //free_noheap(colors);
}




int min(int a, int b)
{
    return a < b ? a : b;
}

bool is_special(char *s)
{
    if (s[0] == '.')
        return true;
    if (0 == strcmp("Cesium", s))
        return true;
    if (0 == strcmp("CLIPDATA", s))
        return true;
    if (0 == strcmp("DrMono", s))
        return true;
    if (0 == strcmp("FATDRVCE", s))
        return true;
    if (0 == strcmp("FONTLIBC", s))
        return true;
    if (0 == strcmp("GRAPHX", s))
        return true;
    if (0 == strcmp("FILEIOC", s))
        return true;
    if (0 == strcmp("ICEHOOKS", s))
        return true;
    if (0 == strcmp("LibLoad", s))
        return true;
    if (0 == strcmp("USBDRVCE", s))
        return true;
    return false;
}


bool show_open_dialog(struct estate *state)
{
    const int num_rows = 13;
    short k = 0;
    //Selected index
    int index = 0;
    //index on screen
    int sindex = 0;
    //index of start of screen
    int scrstart = 0;
    char *var_name;
    void *search_pos = NULL;
    int i = 0;
    void *arr;
    os_MemChk(&arr);
    //array is blocked into 24s first 8 chars are the title, next 16 are the first characters of the file
    while ((var_name = ti_Detect(&search_pos, NULL)) != NULL)
    {
        if (state->hide_special_files)
        {
            if (is_special(var_name))
                continue;
        }
        strncpy((char *)arr + 24 * i, var_name, 8);
        ti_var_t slot1 = ti_Open(var_name, "r");
        for (int j = 0; j < 16; j++)
        {
            char c = ti_GetC(slot1);
            if (c == EOF)
            {
                c = '\0';
            }
            if (c == '\n')
                c = ' ';
            //Save the char
            *((char *)arr + 24 * i + 8 + j) = c;
        }
        ti_Close(slot1);
        i++;
    }
    int numfiles = i;
    while (k != KEY_CLEAR)
    {
        draw_dialog(state, 20, 20, 280, 200);
        gfx_SetColor(state->border_color);
        gfx_HorizLine_NoClip(20, 40, 280);
        fontlib_SetCursorPosition(84, 25);
        fontlib_SetForegroundColor(state->text_color);
        fontlib_DrawString("Open File (filename)");
        fontlib_SetCursorPosition(45, 205);
        fontlib_DrawString("Press enter to open");
        gfx_SetColor(state->statusbar_color);
        gfx_Rectangle_NoClip(25, 45, 270, 160);
        gfx_SetColor(state->border_color);
        gfx_Rectangle_NoClip(26, 46, 270, 160);
        for (i = 0; i < min(num_rows, numfiles); i++)
        {
            if (i == sindex)
            {
                fontlib_SetForegroundColor(state->focus_color);
                gfx_SetColor(state->statusbar_color);
                gfx_FillRectangle_NoClip(27, 46 + 12 * i, 268, 12);
            }
            else
            {
                fontlib_SetForegroundColor(state->text_color);
            }
            //Set initial cursor position
            fontlib_SetCursorPosition(32, 45 + 12 * i);
            //Draw the name
            fontlib_DrawStringL(arr + 24 * (i + scrstart), 8);
            //Draw separator
            fontlib_SetCursorPosition(32 + 8 * 8, 12 * i + 45);
            fontlib_DrawString(" - ");
            //Draw file preview.
            fontlib_DrawStringL(arr + 24 * (i + scrstart) + 8, 16);
        }
        gfx_BlitBuffer();
        k = ngetchx(state);
        if (k == KEY_UP)
        {
            index--;
            if (index < 0)
                index = 0;
            sindex--;
            if (sindex < 0)
            {
                sindex = 0;
                scrstart--;
                if (scrstart < 0)
                {
                    scrstart = 0;
                }
            }
        }
        if (k == KEY_DOWN)
        {
            index++;
            if (index >= numfiles)
                index = numfiles;
            sindex++;
            if (sindex >= num_rows)
            {
                sindex = num_rows - 1;
                scrstart++;
                if (scrstart >= numfiles - num_rows)
                {
                    if (numfiles - num_rows > 0)
                    {
                        scrstart = numfiles - num_rows;
                    }
                    else
                    {
                        scrstart = 0;
                    }
                }
            }
            //Edge case - no trap
            if (sindex >= numfiles)
            {
                sindex = numfiles - 1;
            }
        }
        if (k == '\n')
        {
            if (state->saved || show_unsaved_dialog(state))
            {
                strncpy(state->filename, (char *)arr + 24 * index, 8);
                deinit_editor(state);
                initialize_editor(state);
                parseRC(state);
                load_text(state);
                state->named = true;
                return true;
            }
        }
    }
    return false;
}

//==========================//END_OF_OPEN_FUNCTIONS//==========================//


char* show_create_dialog(struct estate *state)
{
    uint8_t previous_alpha_state = state->alpha_state;
    state->alpha_state = AlphaState_alpha;

    short k = 0;
    int numchars = 0;
    static char buffer[10] = {0};

    int cx = 52;
    while (true)
    {
        draw_dialog(state, 20, 60, 280, 100);
        gfx_SetColor(state->border_color);
        gfx_HorizLine_NoClip(20, 80, 280);
        //fontlib_SetCursorPosition(115,80);
        fontlib_SetCursorPosition(80, 65);
        fontlib_DrawString("Choose a file name");

        fontlib_SetCursorPosition(250, 65);
        if (state->alpha_state == AlphaState_alpha) {
            fontlib_DrawString("alpha");
        }
        else if (state->alpha_state == AlphaState_ALPHA) {
            fontlib_DrawString("ALPHA");
        }

        fontlib_SetCursorPosition(30, 90);
        fontlib_DrawString("Enter file name:");
        gfx_SetColor(state->dropshadow_color);
        gfx_Rectangle_NoClip(51, 111, 220, 16);
        gfx_SetColor(state->border_color);
        gfx_Rectangle_NoClip(50, 110, 220, 16);
        fontlib_SetCursorPosition(52, 112);
        fontlib_DrawString(buffer);
        gfx_VertLine(52 + FONT_WIDTH * numchars, 112, 12);
        cx = 52 + FONT_WIDTH * numchars;
        gfx_BlitBuffer();

        k = ngetchx_xy(state, cx, 112);

        if (k == KEY_CLEAR)
        {
            state->alpha_state = previous_alpha_state;
            return NULL;
        }
        if (numchars && k == '\n')
        {
            // Check if the file already exists
            uint8_t test = ti_Open(buffer, "r");
            if (test == 0) {
                state->alpha_state = previous_alpha_state;
                return buffer;
            }
            ti_Close(test);
            alert(state, "Error", "This file already exists");
        }
        else if (!is_control(k))
        {
            if (numchars < 8)
            {
                buffer[numchars] = k;
                numchars++;
            }
        }
        else if (k == KEY_BS)
        {
            if (numchars)
                buffer[--numchars] = 0;
        }
    }
    state->alpha_state = previous_alpha_state;
    return NULL;
}



// true means canceled
bool show_save_dialog(struct estate *state)
{
    uint8_t previous_alpha_state = state->alpha_state;
    state->alpha_state = AlphaState_alpha;

    bool ret_value;
    short k = 0;
    int numchars = 0;
    char buffer[9];
    memset(buffer, 0, 9);

    int cx = 52;
    while (true)
    {
        draw_dialog(state, 20, 60, 280, 100);
        gfx_SetColor(state->border_color);
        gfx_HorizLine_NoClip(20, 80, 280);
        //fontlib_SetCursorPosition(115,80);
        fontlib_SetCursorPosition(115, 65);
        fontlib_DrawString("Save File");

        fontlib_SetCursorPosition(250, 65);
        if (state->alpha_state == AlphaState_alpha) {
            fontlib_DrawString("alpha");
        }
        else if (state->alpha_state == AlphaState_ALPHA) {
            fontlib_DrawString("ALPHA");
        }

        fontlib_SetCursorPosition(30, 90);
        fontlib_DrawString("Enter file name:");
        gfx_SetColor(state->dropshadow_color);
        gfx_Rectangle_NoClip(51, 111, 220, 16);
        gfx_SetColor(state->border_color);
        gfx_Rectangle_NoClip(50, 110, 220, 16);
        fontlib_SetCursorPosition(52, 112);
        fontlib_DrawString(buffer);
        gfx_VertLine(52 + FONT_WIDTH * numchars, 112, 12);
        cx = 52 + FONT_WIDTH * numchars;
        gfx_BlitBuffer();
        k = ngetchx_xy(state, cx, 112);
        if (k == KEY_CLEAR)
        {
            state->alpha_state = previous_alpha_state;
            ret_value = true;
            goto show_save_dialog_return;
        }
        if (k == '\n')
        {
            if (!numchars)
            {
                state->alpha_state = previous_alpha_state;
                ret_value = true;
                goto show_save_dialog_return;
            }
            state->named = true;
            strncpy(state->filename, buffer, 8);
            state->alpha_state = previous_alpha_state;
            ret_value = false;
            goto show_save_dialog_return;
        }
        if (!is_control(k))
        {
            if (numchars < 8)
            {
                buffer[numchars] = k;
                numchars++;
            }
        }
        if (k == KEY_BS)
        {
            if (numchars)
                buffer[--numchars] = 0;
        }
    }
show_save_dialog_return:
    state->alpha_state = previous_alpha_state;
    return ret_value;
}

void show_about_dialog(struct estate *state)
{
    draw_dialog(state, 60, 60, 200, 120);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(60, 80, 200);
    //fontlib_SetCursorPosition(115,80);
    fontlib_SetCursorPosition(105, 65);
    fontlib_DrawString("About NeonIDE");
    fontlib_SetCursorPosition(80, 120);
    fontlib_DrawString(NEONIDE_VERSION_STRING);
    gfx_BlitBuffer();
    ngetchx(state);
}


void show_unimplemented_dialog(struct estate *state)
{
    draw_dialog(state, 60, 60, 200, 120);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(60, 80, 200);
    //fontlib_SetCursorPosition(115,80);
    fontlib_SetCursorPosition(105, 65);
    fontlib_DrawString("Unimplemented");
    fontlib_SetCursorPosition(65, 85);
    fontlib_DrawString("Please wait the next");
    fontlib_SetCursorPosition(65, 97);
    fontlib_DrawString("major update of Neon for");
    fontlib_SetCursorPosition(65, 109);
    fontlib_DrawString("this feature to be added");

    fontlib_SetCursorPosition(65, 129);
    fontlib_DrawString("For now you need to use");
    fontlib_SetCursorPosition(65, 141);
    fontlib_DrawString("NeonIDE and the Neon app");
    gfx_BlitBuffer();
    ngetchx(state);
}



void alert(struct estate *state, char* title, char* text)
{
    draw_dialog(state, 60, 90, 200, 60);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(60, 110, 200);
    //fontlib_SetCursorPosition(115,80);
    fontlib_SetCursorPosition(105, 95);
    fontlib_DrawString(title);
    fontlib_SetCursorPosition(65, 120);
    fontlib_DrawString(text);
    gfx_BlitBuffer();
    ngetchx(state);
}



void confirm_backend_draw(struct estate *state, int index)
{
    draw_dialog(state, 110, 85, 105, 60);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(110, 105, 105);
    fontlib_SetCursorPosition(115, 90);
    fontlib_DrawString("Are you sure:");

    fontlib_SetCursorPosition(114, 110);
    if (index == 0)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("1) Yes");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(114, 125);
    if (index == 1)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("2) No");
    fontlib_SetForegroundColor(state->text_color);
}

bool show_confirm_dialog(struct estate *state)
{
    short k = 0;
    int index = 0;
    while (k != KEY_CLEAR)
    {
        if (k == '\n' || k == KEY_RIGHT || (k >= '1' && k <= '2'))
        {
            if (k >= '1' && k <= '2')
                index = k - '1';

            switch (index)
            {
            case 0:
                return true;
            case 1:
                return false;
            }
        }
        if (k == KEY_UP)
        {
            if (index)
            {
                index--;
            }
        }
        if (k == KEY_DOWN)
        {
            if (index < 1)
            {
                index++;
            }
        }
        confirm_backend_draw(state, index);

        gfx_BlitBuffer();
        k = ngetchx(state);
    }
    return false;
}


/*
 * Draws the Tools menu given the state parameters and the selection index
 */
void file_menu_backend_draw(struct estate *state, int index, char* filename)
{
    //x+135, y+80
    draw_dialog(state, 195, 125, 110, 90);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(195, 145, 110);
    fontlib_SetCursorPosition(200, 130);
    fontlib_DrawString("File:");

    fontlib_SetCursorPosition(199, 150);
    if (index == 0)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("1) Duplicate");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(199, 165);
    if (index == 1)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("2) Rename");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(199, 180);
    if (index == 2)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("3) Delete");
    fontlib_SetForegroundColor(state->text_color);

    fontlib_SetCursorPosition(199, 195);
    if (index == 3)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    uint8_t handle = ti_Open(filename, "r");
    if (ti_IsArchived(handle))
        fontlib_DrawString("4) Unarchive");
    else
        fontlib_DrawString("4) Archive");
    ti_Close(handle);
    fontlib_SetForegroundColor(state->text_color);
}

void show_file_menu_dialog(struct estate *state, char* filename)
{
    short k = 0;
    int index = 0;
    while (k != KEY_CLEAR && k != KEY_F5)
    {
        if (k == '\n' || k == KEY_RIGHT || (k >= '1' && k <= '4'))
        {
            if (k >= '1' && k <= '4')
                index = k - '1';

            switch (index)
            {
                case 0: {
                    char* new_name = show_create_dialog(state);
                    if (new_name != NULL) {
                        uint8_t source = ti_Open(filename, "r");
                        uint8_t dest = ti_Open(new_name, "w");
                        char c;
                        while ((c = ti_GetC(source)) != EOF) {
                            ti_Write(&c, 1, 1, dest);
                        }
                        ti_Close(source);
                        ti_Close(dest);
                    }
                    return;
                }
                case 1: {
                    char* new_name = show_create_dialog(state);
                    if (new_name != NULL) {
                        ti_Rename(filename, new_name);
                    }
                    return;
                }
                case 2:
                    if (show_confirm_dialog(state)) {
                        ti_Delete(filename);
                        return;
                    }
                    break;
                
                case 3:
                {
                    uint8_t handle = ti_Open(filename, "r");
                    if (ti_IsArchived(handle))
                        ti_SetArchiveStatus(false, handle);
                    else
                        ti_SetArchiveStatus(true, handle);
                    ti_Close(handle);
                    return;
                }
            }
        }
        if (k == KEY_UP)
        {
            if (index)
            {
                index--;
            }
        }
        if (k == KEY_DOWN)
        {
            if (index < 2)
            {
                index++;
            }
        }
        file_menu_backend_draw(state, index, filename);

        gfx_BlitBuffer();
        k = ngetchx(state);
    }
}




void unsaved_backend_draw(struct estate *state, int index)
{
    draw_dialog(state, 60, 60, 200, 75);
    gfx_SetColor(state->border_color);
    gfx_HorizLine_NoClip(60, 80, 200);
    fontlib_SetCursorPosition(65, 65);
    fontlib_DrawString("You have unsaved changes:");

    fontlib_SetCursorPosition(64, 85);
    if (index == 0)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("1) Save");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(64, 100);
    if (index == 1)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("2) Discard changes");
    fontlib_SetForegroundColor(state->text_color);
    fontlib_SetCursorPosition(64, 115);
    if (index == 2)
    {
        fontlib_SetForegroundColor(state->focus_color);
    }
    fontlib_DrawString("3) Cancel");
    fontlib_SetForegroundColor(state->text_color);
}

bool show_unsaved_dialog(struct estate *state)
{
    short k = 0;
    int index = 0;
    while (k != KEY_CLEAR)
    {
        if (k == '\n' || k == KEY_RIGHT || (k >= '1' && k <= '3'))
        {
            if (k >= '1' && k <= '3')
                index = k - '1';

            switch (index)
            {
                case 0:
                    write_file(state);
                    return true;
                case 1:
                    return true;
                case 2:
                    return false;
            }
        }
        if (k == KEY_UP)
        {
            if (index)
            {
                index--;
            }
        }
        if (k == KEY_DOWN)
        {
            if (index < 2)
            {
                index++;
            }
        }
        unsaved_backend_draw(state, index);

        gfx_BlitBuffer();
        k = ngetchx(state);
    }
    return false;
}