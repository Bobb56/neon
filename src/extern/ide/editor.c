/*
 * editor.c
 *
 *  Created on: Jul 25, 2021
 *      Author: michael
 */

#include "headers/libmalloc.h"
#include "headers/neonide.h"
#include "headers/dialogs.h"
#include "headers/editor.h"
#include "headers/state.h"
#include "headers/keys.h"
#include "headers/find.h"
#include <stdint.h>
#include <string.h>

bool is_control(short k)
{
    return (k <= 0) || (k >= 256);
}

void insert_newline(struct estate *state)
{
    //Line split
    int tmp = state->lines[state->lc1] - state->lc_offset;
    state->lines[state->lc1] = state->lc_offset;
    //Create new entry in line buffer.
    state->lc1++;
    state->lines[state->lc1] = tmp;
    state->lc_offset = 0;
}

void insert_char(struct estate *state, char c)
{
    if (state->selection_active)
    {
        bs(state);
    }
    state->saved = false;
    state->text[state->c1] = c;
    state->c1++;
    if (state->text[state->c1 - 1] == '\n')
    {
        insert_newline(state);
    }
    else
    {
        state->lines[state->lc1]++;
        state->lc_offset++;
    }
}

void line_down(struct estate *state)
{
    if (state->lc2 < state->max_buffer_size)
    {
        state->lc2++;
        state->lc1++;
        state->lines[state->lc1] = state->lines[state->lc2];
        state->lc_offset = 0;
    }
}

void line_up(struct estate *state)
{
    if (state->lc1 > 0)
    {
        state->lines[state->lc2] = state->lines[state->lc1];
        state->lc1--;
        state->lc2--;
        state->lc_offset = state->lines[state->lc1];
    }
}

void cursor_left(struct estate *state)
{
    if (state->selection_active)
    {
        state->selection_active = false;
        if (state->selection_anchor > state->c2 + 1)
            return;
        while (state->selection_anchor < state->c1 - 2)
            cursor_left(state);
    }
    if (state->c1 > 0)
    {
        if (state->lc_offset == 0)
        {
            line_up(state);
        }
        else
        {
            //Moving on same line
            state->lc_offset--;
        }
        state->c1--;
        state->text[state->c2] = state->text[state->c1];
        state->c2--;

        if (state->ide_state != IDEState_Editor && state->c1 > 0 && state->text[state->c1 - 1] == 0x1b) {
            cursor_left(state);
        }
    }
}

void cursor_left_select(struct estate *state)
{
    if (!state->selection_active)
    {
        state->selection_active = true;
        state->selection_anchor = state->c2;
    }
    if (state->c1 == state->selection_anchor)
    {
        state->selection_anchor = state->c2;
    }
    if (state->c1 > 0)
    {
        if (state->lc_offset == 0)
        {
            line_up(state);
        }
        else
        {
            //Moving on same line
            state->lc_offset--;
        }
        state->c1--;
        state->text[state->c2] = state->text[state->c1];
        state->c2--;
    }
}

void cursor_right(struct estate *state)
{
    if (state->selection_active)
    {
        state->selection_active = false;
        if (state->selection_anchor < state->c1 - 1)
            return;
        while (state->selection_anchor > state->c2 + 2)
            cursor_right(state);
    }
    if (state->c2 < state->max_buffer_size - 1)
    {
        if (state->lc_offset == state->lines[state->lc1])
        {
            line_down(state);
        }
        else
        {
            state->lc_offset++;
        }
        state->c2++;
        state->text[state->c1] = state->text[state->c2];
        state->c1++;

        if (state->ide_state != IDEState_Editor && state->c2 < state->max_buffer_size - 1 && state->text[state->c2 + 1] == 0x1b) {
            cursor_right(state);
        }
    }
}

void cursor_right_select(struct estate *state)
{
    if (!state->selection_active)
    {
        state->selection_active = true;
        state->selection_anchor = state->c1;
    }
    if (state->c2 == state->selection_anchor)
    {
        state->selection_anchor = state->c1;
    }
    if (state->c2 < state->max_buffer_size - 1)
    {
        if (state->lc_offset == state->lines[state->lc1])
        {
            line_down(state);
        }
        else
        {
            state->lc_offset++;
        }
        state->c2++;
        state->text[state->c1] = state->text[state->c2];
        state->c1++;
    }
}

void cursor_up(struct estate *state)
{
    if (state->selection_active)
    {
        cursor_left(state);
    }

    if (state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0) {
        cursor_left(state);
    }

    //if the current line is long enough
    if (state->lc_offset >= NUM_COLS)
    {
        for (int i = 0; i < NUM_COLS; i++)
        {
            cursor_left(state);
        }
    }
    else
    {
        //cache the current offset -- use this later
        int old = state->lc_offset;
        while (state->lc_offset > 0)
        {
            cursor_left(state);
        }
        if (!state->c1)
            return;
        cursor_left(state);
        //on last char of next line
        //now need to move until old

        // return if the end is what you want
        // avoid the case where we end up after the last character of a full physical line
        if (state->lc_offset % NUM_COLS < old && !(state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0))
            return;

        
        int last_physical_line_length = state->lc_offset % NUM_COLS;
        if (last_physical_line_length == 0 && state->lc_offset != 0)
            last_physical_line_length = NUM_COLS;
        
        for (int i = 0; i < last_physical_line_length - old; i++)
        {
            cursor_left(state);
        }
    }
    if (state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0)
        cursor_right(state);
}


void cursor_up_select(struct estate *state)
{
    if (state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0) {
        cursor_left_select(state);
    }

    //if the current line is long enough
    if (state->lc_offset >= NUM_COLS)
    {
        for (int i = 0; i < NUM_COLS; i++)
        {
            cursor_left_select(state);
        }
    }
    else
    {
        //cache the current offset -- use this later
        int old = state->lc_offset;
        while (state->lc_offset > 0)
        {
            cursor_left_select(state);
        }
        if (!state->c1)
            return;
        cursor_left_select(state);
        //on last char of next line
        //now need to move until old

        //return if the end is what you want
        if (state->lc_offset % NUM_COLS < old && !(state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0))
            return;

        
        int last_physical_line_length = state->lc_offset % NUM_COLS;
        if (last_physical_line_length == 0 && state->lc_offset != 0)
            last_physical_line_length = NUM_COLS;
        
        for (int i = 0; i < last_physical_line_length - old; i++)
        {
            cursor_left_select(state);
        }
    }
    if (state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0)
        cursor_right_select(state);
}

void cursor_down(struct estate *state)
{
    if (state->selection_active)
    {
        state->selection_active = 0;
        cursor_down(state);
    }

    if (state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0) {
        cursor_left(state);
    }

    if (state->lines[state->lc1] - state->lc_offset >= NUM_COLS)
    {
        int move = NUM_COLS;
        if (state->lc_offset == 0)
            move++;

        for (int i = 0; i < move; i++)
        {
            cursor_right(state);
        }
    }
    else
    {
        int old = state->lc_offset;
        int oldb = state->lines[state->lc1];
        for (int i = 0; i < oldb - old; i++)
        {
            cursor_right(state);
        }
        cursor_right(state);
        if (state->lines[state->lc1] < old % NUM_COLS)
        {
            for (int i = 0; i < state->lines[state->lc1]; i++)
            {
                cursor_right(state);
            }
        }
        else
        {
            for (int i = 0; i < old % NUM_COLS; i++)
            {
                cursor_right(state);
            }
        }
        if (state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0)
            cursor_right(state);
    }
}

void cursor_down_select(struct estate *state)
{
    if (state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0) {
        cursor_left_select(state);
    }

    if (state->lines[state->lc1] - state->lc_offset >= NUM_COLS)
    {
        int move = NUM_COLS;
        if (state->lc_offset == 0)
            move++;

        for (int i = 0; i < move; i++)
        {
            cursor_right_select(state);
        }
    }
    else
    {
        int old = state->lc_offset;
        int oldb = state->lines[state->lc1];
        for (int i = 0; i < oldb - old; i++)
        {
            cursor_right_select(state);
        }
        cursor_right_select(state);
        if (state->lines[state->lc1] < old % NUM_COLS)
        {
            for (int i = 0; i < state->lines[state->lc1]; i++)
            {
                cursor_right_select(state);
            }
        }
        else
        {
            for (int i = 0; i < old % NUM_COLS; i++)
            {
                cursor_right_select(state);
            }
        }
        if (state->lc_offset > 0 && state->lc_offset % NUM_COLS == 0)
            cursor_right_select(state);
    }
}

int handle_key(struct estate *state, short k)
{
	if (!is_control(k))
	{
		insert_char(state, k);
	}
	else
	{
		switch (k)
		{
		case KEY_SUP:
			cursor_up_select(state);
			break;
		case KEY_SDOWN:
			cursor_down_select(state);
			break;
		case KEY_SLEFT:
			cursor_left_select(state);
			break;
		case KEY_SRIGHT:
			cursor_right_select(state);
			break;
		case KEY_LEFT: //left
			cursor_left(state);
			break;
		case KEY_RIGHT: //right
			cursor_right(state);
			break;
		case KEY_DOWN: //down
			cursor_down(state);
			break;
		case KEY_UP: //up
			cursor_up(state);
			break;
		case KEY_BS: //backspace
			bs(state);
			break;
		case KEY_DEL: //delete
			del(state);
			break;
		case KEY_SAVE_AS:
			draw_editor(state);
			gfx_SwapDraw();
			if (!show_save_dialog(state))
				write_file(state);
			break;
		case KEY_SAVE: //save
			insert_char(state, '=');
			break;
		case KEY_WSLEFT:
			cursor_to_left_word_select(state);
			break;
		case KEY_WSRIGHT:
			cursor_to_right_word_select(state);
			break;
		case KEY_WSDOWN:
			cursor_multi_down_select(state);
			break;
		case KEY_WSUP:
			cursor_multi_up_select(state);
			break;
		case KEY_WLEFT: //2nd-left
			cursor_to_left_word(state);
			break;
		case KEY_WRIGHT: //2nd-right
			cursor_to_right_word(state);
			break;
		case KEY_WUP: //2nd-up
			cursor_multi_up(state);
			break;
		case KEY_WDOWN: //2nd-down
			cursor_multi_down(state);
			break;
		case KEY_LSLEFT:
			cursor_to_l_start_select(state);
			break;
		case KEY_LSRIGHT:
			cursor_to_l_end_select(state);
			break;
		case KEY_LSUP:
			cursor_to_start_select(state);
			break;
		case KEY_LSDOWN:
			cursor_to_end_select(state);
			break;
		case KEY_LLEFT: //meta-left
			cursor_to_l_start(state);
			break;
		case KEY_LRIGHT: //meta-right
			cursor_to_l_end(state);
			break;
		case KEY_LUP: //meta-up
			cursor_to_start(state);
			break;
		case KEY_LDOWN: //meta-down
			cursor_to_end(state);
			break;
		case KEY_F1:
			draw_editor(state);
            gfx_SwapDraw();
            int res = show_options_dialog(state);
            draw_editor(state);
            gfx_SwapDraw();
            if (res != 0)
                return -1;
            break;
        case KEY_F2:
            draw_editor(state);
            gfx_SwapDraw();
            show_chars_dialog(state, draw_editor);
            draw_editor(state);
            gfx_SwapDraw();
            break;
        case KEY_F3:
            draw_editor(state);
            gfx_SwapDraw();
            show_editor_tools_dialog(state);
            draw_editor(state);
            gfx_SwapDraw();
            break;
        case KEY_F4:
            draw_editor(state);
            gfx_SwapDraw();
            bool result = state->saved || show_unsaved_dialog(state);
            draw_editor(state);
            gfx_SwapDraw();
            if (result)
                return -1;
            break;
        case KEY_F5:
            draw_editor(state);
            gfx_SwapDraw();
            result = state->saved || show_unsaved_dialog(state);
            draw_editor(state);
            gfx_SwapDraw();
            if (result) {
                state->ide_goto = IDEState_RunningProgram;
                state->ide_go_back = IDEState_Editor;
                return -1;
            }
            break;
        case KEY_OPEN:
            draw_editor(state);
            gfx_SwapDraw();
            open_file(state);
            break;
        case KEY_STATE:
            break;
        }
    }
    return 0;
}

void cursor_to_start_select(struct estate *state)
{
    while (state->c1)
    {
        cursor_left_select(state);
    }
}
void cursor_to_end_select(struct estate *state)
{
    while (state->c2 < state->max_buffer_size - 1)
    {
        cursor_right_select(state);
    }
}
void cursor_multi_up_select(struct estate *state)
{
    for (int i = 0; i < 5; i++)
    {
        cursor_up_select(state);
    }
}
void cursor_multi_down_select(struct estate *state)
{
    for (int i = 0; i < 5; i++)
    {
        cursor_down_select(state);
    }
}
void cursor_to_l_start_select(struct estate *state)
{
    while (state->lc_offset)
    {
        cursor_left_select(state);
    }
}
void cursor_to_l_end_select(struct estate *state)
{
    while (state->lc_offset < state->lines[state->lc1])
    {
        cursor_right_select(state);
    }
}

void cursor_to_left_word_select(struct estate *state)
{
    while(state->c1 && (state->text[state->c1-1]=='\n' || state->text[state->c1-1]==' ')){
        cursor_left_select(state);
    }
    while(state->c1 && (state->text[state->c1-1]!='\n' && state->text[state->c1-1]!=' ')){
        cursor_left_select(state);
    }
}

void cursor_to_right_word_select(struct estate *state)
{
    while(state->c2<state->max_buffer_size - 1 && (state->text[state->c2+1]=='\n' || state->text[state->c2+1]==' ')){
        cursor_right_select(state);
    }
    while(state->c2<state->max_buffer_size - 1 && (state->text[state->c2+1]!='\n' && state->text[state->c2+1]!=' ')){
        cursor_right_select(state);
    }
}



void initialize_editor(struct estate* state) {
	state->max_buffer_size = 16384;
	state->max_lines = 10000;
	state->text = malloc_noheap(state->max_buffer_size);
	state->lines = malloc_noheap(state->max_lines * sizeof(int16_t));
	initialize(state);

    state->ide_state = IDEState_Editor;
}


void deinit_editor(struct estate* state) {
    free_all_noheap();
	state->ide_state = IDEState_Other;
}


void draw_editor(struct estate *state)
{
    draw_text_area(state);
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
    fontlib_SetCursorPosition(4, 228);
    fontlib_DrawString("Options");
    fontlib_SetCursorPosition(76, 228);
    fontlib_DrawString("# $ %");
    fontlib_SetCursorPosition(140, 228);
    fontlib_DrawString("Tools");
    fontlib_SetCursorPosition(209, 228);
    fontlib_DrawString("Home");
    fontlib_SetCursorPosition(276, 228);
    fontlib_DrawString("Run");
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
    fontlib_DrawString("Line: ");
    fontlib_DrawInt(state->lc1 + 1, 4);

    
    if (!state->saved) {
        fontlib_SetCursorPosition(104, 0);
        fontlib_DrawGlyph('*');
    }
    
    fontlib_SetCursorPosition(120, 0);
    fontlib_DrawString(state->named ? state->filename : "New script");

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
}

void launch_editor(struct estate* state, char* filename) {
    initialize_editor(state);
    
    strcpy(state->filename, filename);
    state->named = true;

    load_text(state);
    editor_mainloop(state);

    deinit_editor(state);
}

void editor_mainloop(struct estate *state)
{
    short k = 0;
    while (true)
    {
        draw_editor(state);
        gfx_SwapDraw();
        k = ngetchx_xy(state, state->cx, state->cy);
        if (k == KEY_CLEAR)
        {
            //Rely on short circuit
            //First do this to remove other dialog artifacts
            draw_editor(state);
            gfx_SwapDraw();
            if (state->saved || show_unsaved_dialog(state)) {
                break;
            }
        }
        int res = handle_key(state, k);
        
        if (res != 0)
            break;
    }
}

void bs(struct estate *state)
{
    state->saved = false;
    if (state->selection_active)
    {
        state->selection_active = false;
        while (state->selection_anchor < state->c1)
        { //if selection is to left
            bs(state);
        }
        while (state->selection_anchor > state->c2)
        { //if selection is to left
            del(state);
        }
    }
    else if (state->c1)
    {
        if (state->lc_offset == 0)
        {
            state->lc1--; //Delete line
            state->lc_offset = state->lines[state->lc1];
            state->lines[state->lc1] += state->lines[state->lc1 + 1];
        }
        else
        {
            state->lc_offset--; //else go back
            state->lines[state->lc1]--;
        }
        state->c1--;
    }
}

void del(struct estate *state)
{
    state->saved = false;
    if (state->selection_active)
    {
        state->selection_active = false;
        while (state->selection_anchor < state->c1)
        { //if selection is to left
            bs(state);
        }
        while (state->selection_anchor > state->c2)
        { //if selection is to left
            del(state);
        }
    }
    else if (state->c2 < state->max_buffer_size - 1)
    {
        if (state->lc_offset == state->lines[state->lc1])
        {
            state->lc2++;
            state->lines[state->lc1] += state->lines[state->lc2 + 1];
        }
        else
        {
            state->lines[state->lc1]--;
        }
        state->c2++;
    }
}

void scroll_up(struct estate *state)
{
    state->scr_offset = state->c1 - state->lc_offset % NUM_COLS;
    state->scr_line = state->lc1;
    state->scr_line_offset = state->lc_offset - state->lc_offset % NUM_COLS;
}

void scroll_down(struct estate *state)
{
    if (!state->selection_active)
    {
        int pre_offset = state->lc_offset;
        for (int i = 0; i < NUM_LINES; i++)
        {
            cursor_up(state);
        }
        scroll_up(state);
        for (int i = 0; i < NUM_LINES; i++)
        {
            cursor_down(state);
        }
        while (state->lc_offset < pre_offset)
        {
            cursor_right(state);
        }
        while (state->lc_offset > pre_offset)
        {
            cursor_left(state);
        }
    }
    else
    {
        int pre_offset = state->lc_offset;
        for (int i = 0; i < NUM_LINES; i++)
        {
            cursor_up_select(state);
        }
        scroll_up(state);
        for (int i = 0; i < NUM_LINES; i++)
        {
            cursor_down_select(state);
        }
        while (state->lc_offset < pre_offset)
        {
            cursor_right_select(state);
        }
        while (state->lc_offset > pre_offset)
        {
            cursor_left_select(state);
        }
    }
}

void cursor_to_start(struct estate *state)
{
    while (state->c1)
    {
        cursor_left(state);
    }
}

void cursor_to_end(struct estate *state)
{
    while (state->c2 < state->max_buffer_size - 1)
    {
        cursor_right(state);
    }
}

void cursor_to_left_word(struct estate *state)
{
    while(state->c1 && (state->text[state->c1-1]=='\n' || state->text[state->c1-1]==' ')){
        cursor_left(state);
    }
    while(state->c1 && (state->text[state->c1-1]!='\n' && state->text[state->c1-1]!=' ')){
        cursor_left(state);
    }
}

void cursor_to_right_word(struct estate *state)
{
    while(state->c2<state->max_buffer_size - 1 && (state->text[state->c2+1]=='\n' || state->text[state->c2+1]==' ')) {
        cursor_right(state);
    }
    while(state->c2<state->max_buffer_size - 1 && (state->text[state->c2+1]!='\n' && state->text[state->c2+1]!=' ')) {
        cursor_right(state);
    }
}

void cursor_multi_up(struct estate *state)
{
    for (int i = 0; i < 5; i++)
    {
        cursor_up(state);
    }
}

void cursor_multi_down(struct estate *state)
{
    for (int i = 0; i < 5; i++)
    {
        cursor_down(state);
    }
}

void cursor_to_l_start(struct estate *state)
{
    while (state->lc_offset)
    {
        cursor_left(state);
    }
}

void cursor_to_l_end(struct estate *state)
{
    while (state->lc_offset < state->lines[state->lc1])
    {
        cursor_right(state);
    }
}

void load_text(struct estate *state)
{
	ti_var_t var = ti_Open(state->filename, "r");
	if (var == 0) {
		return; //no error out
	}

	char buffer[NEON_DEFAULT_FILE_HEADER_SIZE];
	ti_Read(buffer, NEON_DEFAULT_FILE_HEADER_SIZE, 1, var);
	if (strncmp(buffer, NEON_DEFAULT_FILE_HEADER, NEON_DEFAULT_FILE_HEADER_SIZE) != 0) {
		for (int i=0 ; i < NEON_DEFAULT_FILE_HEADER_SIZE ; i++) {
			insert_char(state, buffer[i]);
		}
	}

    char c;
    while ((c = ti_GetC(var)) != EOF)
    {
        insert_char(state, c);
    }

    cursor_to_start(state);

	ti_Close(var);
	state->saved = true;
}

void write_file(struct estate *state)
{
	// We always write a header, except if the file starts with #NEON
	bool need_neon_header = !string_equal(state, 0, NEON_PLAIN_TEXT_HEADER);

	// Compute the size of the resulting file
	int24_t fullsize = state->c1 + (state->max_buffer_size - (state->c2 + 1));

	if (need_neon_header)
		fullsize += 5;

	bool current_archive_status = false;

	// Check if we will archive the saved file
	bool archive = false;
	ti_var_t var = ti_Open(state->filename, "r");
	if (var != 0) {
		archive = ti_IsArchived(var);
		ti_Close(var);
	}

	var = ti_Open(state->filename, "w");
	//makes saving a lot faster due to only needing to resize the variable once
	ti_Resize(fullsize, var);

	if (need_neon_header) {
		ti_Write(NEON_DEFAULT_FILE_HEADER, NEON_DEFAULT_FILE_HEADER_SIZE, 1, var);
	}

	ti_Write(state->text, state->c1, 1, var);
	ti_Write(state->text + state->c2 + 1, state->max_buffer_size - (state->c2 + 1), 1, var);

	// Archive file if needed
	ti_SetArchiveStatus(archive, var);

	ti_Close(var);
	state->saved = true;
}



/**
Parse the NEIDERC file.
*/
void parseRC(struct estate *state)
{
    FILE *f = fopen("NEIDERC", "r");
    if (!f)
        return;
    char buffer[256];
    while (!feof(f))
    {
        fgets(buffer, 256, f);
        if (0 == strncmp(buffer, "TC:", 3))
        {
            int val = atoi(buffer + 3);
            if (val < 256 && val >= 0)
            {
                state->text_color = val;
            }
        }
        if (0 == strncmp(buffer, "THC:", 4))
        {
            int val = atoi(buffer + 4);
            if (val < 256 && val >= 0)
            {
                state->text_highlight_color = val;
            }
        }
        if (0 == strncmp(buffer, "TSC:", 4))
        {
            int val = atoi(buffer + 4);
            if (val < 256 && val >= 0)
            {
                state->text_selection_color = val;
            }
        }
        if (0 == strncmp(buffer, "TSHC:", 5))
        {
            int val = atoi(buffer + 5);
            if (val < 256 && val >= 0)
            {
                state->text_selection_highlight_color = val;
            }
        }
        if (0 == strncmp(buffer, "BGC:", 4))
        {
            int val = atoi(buffer + 4);
            if (val < 256 && val >= 0)
            {
                state->background_color = val;
            }
        }
        if (0 == strncmp(buffer, "TRC:", 4))
        {
            int val = atoi(buffer + 4);
            if (val < 256 && val >= 0)
            {
                state->transparent_color = val;
            }
        }
        if (0 == strncmp(buffer, "SBC:", 4))
        {
            int val = atoi(buffer + 4);
            if (val < 256 && val >= 0)
            {
                state->statusbar_color = val;
            }
        }
        if (0 == strncmp(buffer, "SBTC:", 5))
        {
            int val = atoi(buffer + 5);
            if (val < 256 && val >= 0)
            {
                state->statusbar_text_color = val;
            }
        }
        if (0 == strncmp(buffer, "TSHC:", 5))
        {
            int val = atoi(buffer + 5);
            if (val < 256 && val >= 0)
            {
                state->text_selection_highlight_color = val;
            }
        }
        if (0 == strncmp(buffer, "BC:", 3))
        {
            int val = atoi(buffer + 3);
            if (val < 256 && val >= 0)
            {
                state->border_color = val;
            }
        }
        if (0 == strncmp(buffer, "DSC:", 4))
        {
            int val = atoi(buffer + 4);
            if (val < 256 && val >= 0)
            {
                state->dropshadow_color = val;
            }
        }
        if (0 == strncmp(buffer, "FC:", 3))
        {
            int val = atoi(buffer + 3);
            if (val < 256 && val >= 0)
            {
                state->focus_color = val;
            }
        }
        if (0 == strncmp(buffer, "AA:", 3))
        {
            int val = atoi(buffer + 3);
            if (val)
            {
                state->autoarchive = 1;
            }
        }
        if (0 == strncmp(buffer, "CB:", 3))
        {
            int val = atoi(buffer + 3);
            if (val)
            {
                state->blinkcursor = 1;
            }
        }
        if (0==strncmp(buffer, "HSF:",4)){
            int val = atoi(buffer + 4);
            if(val==0)
                state->hide_special_files=0;
        }
    }
    fclose(f);
}