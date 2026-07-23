#include "headers/libmalloc.h"
#define NEON_SOURCE_ID 100
#include "headers/console.h"
#include "headers/neonide.h"
#include "headers/editor.h"
#include "headers/state.h"
#include "headers/dialogs.h"
#include "headers/keys.h"
#include <string.h>

#include "../../headers/neon.h"
#include "../../headers/errors.h"
#include "../../headers/graphicmodule.h"

// Number of chars written between each console flush
#define FLUSH_FREQUENCY 100

// A global variable that points to the current console
// that is used by neonide_print and neonide_input
// Needs to be initialized by a console before using
// neonide_print and neonide_input and to be deinit after
// see -> void start_console(void) { ... }
//       -> void run_neon_program(char* name) { ... }
static struct estate* global_console_state = NULL;

#define INPUT_MAX_LINES        2


void console_scroll_down(struct estate* state) {
    int col = 0;
    while (state->text[state->scr_offset] != '\n' && col < NUM_COLS) {
        if (state->text[state->scr_offset] == 0x1b) {
            state->scr_offset++;
            state->scr_line_offset++;
        }
        else {
            col++;
        }
        state->scr_offset++;
        state->scr_line_offset++;
    }

    if (state->text[state->scr_offset] == '\n') {
        state->scr_offset++;
        state->scr_line++;
        state->scr_line_offset = 0;
    }
}




void pointer_left(struct estate* state, int* pointer) {
    (*pointer)--;
    if (*pointer == state->c2) {
        *pointer = state->c1 - 1;
    }
}

void line_left(struct estate* state, int* line) {
    (*line)--;
    if (*line == state->lc2) {
        *line = state->lc1;
    }
}

void console_scroll_up(struct estate* state) {
    pointer_left(state, &state->scr_offset);
    state->scr_line_offset--;

    if (state->text[state->scr_offset] == '\n') {
        pointer_left(state, &state->scr_offset);
        line_left(state, &state->scr_line);
        state->scr_line_offset = state->lines[state->scr_line] - 2;
    }

    int col = 0;
    while (state->text[state->scr_offset] != '\n' && col < NUM_COLS) {
        if (state->scr_offset > 0 && state->text[state->scr_offset - 1] == 0x1b) {
            pointer_left(state, &state->scr_offset);
            state->scr_line_offset--;
        }
        else {
            col++;
        }

        if (state->scr_offset == 0)
            break;

        pointer_left(state, &state->scr_offset);
        state->scr_line_offset--;
    }

    if (state->text[state->scr_offset] == '\n') {
        state->scr_offset++;
        state->scr_line_offset = 0;
    }
}

void console_draw_text_area(struct estate* state) {
beginning:
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

    //Start drawing
    fontlib_SetForegroundColor(text_color);
    fontlib_SetCursorPosition(left_offset, LINE_SPACING);

    //Iterate buffer
    while (i < state->max_buffer_size && (cp < state->max_buffer_size - state->c2 + state->c1) && row < NUM_LINES + 1)
    {
        fontlib_SetForegroundColor(text_color);
        fontlib_SetBackgroundColor(state->text_highlight_color);
        fontlib_SetTransparency(true);
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
            fontlib_SetForegroundColor(text_color);
            fontlib_SetBackgroundColor(state->text_highlight_color);
            fontlib_SetTransparency(true);
        }

        if (state->text[i] == 0x1b) {
            if (i < state->max_buffer_size - 1)
                text_color = state->text[i+1];
            i+=2;
        }
        else {
            // We create a new line only if we truly need to display characters so
            // that if the last character of a line is \n there will be only one new line
            if (col >= NUM_COLS) {
                col = 0;
                row++;
                fontlib_SetCursorPosition(left_offset, LINE_SPACING * row + LINE_SPACING);
            }

            fontlib_DrawGlyph(state->text[i]);
            i+=1;
            col++;
        }
        cp++;
    }

    if (!drawn)
    {
        if (state->c1 < state->scr_offset)
        {
            console_scroll_up(state);
        }
        else
        {
            console_scroll_down(state);
        }
        goto beginning;
    }
}



void draw_console(struct estate *state)
{
    gfx_SetDrawBuffer();
    // Moves scr_offset on the beginning of the line of the cursor
    // Works in the use cases but could not work in all situations
    /*if (state->scr_offset >= state->c1 && state->scr_offset <= state->c2) {
        state->scr_offset = state->c1;
        
        while (state->scr_offset > 0 && state->text[state->scr_offset] != '\n') {
            state->scr_offset--;
        }


        if (state->text[state->scr_offset] == '\n') {
            state->scr_line_offset = 0;
            state->scr_offset++;
        }
    }*/

    console_draw_text_area(state);
    
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
    fontlib_SetCursorPosition(16, 228);
    fontlib_DrawString("Home");
    fontlib_SetCursorPosition(76, 228);
    fontlib_DrawString("# $ %");
    fontlib_SetCursorPosition(140, 228);
    fontlib_DrawString("Tools");

    if (state->ide_state != IDEState_RunningProgram) {
        fontlib_SetForegroundColor(state->dropshadow_color);
    }
    fontlib_SetCursorPosition(209, 228);
    fontlib_DrawString("Edit");
    fontlib_SetForegroundColor(state->text_color);

    if (state->ide_state != IDEState_RunningProgram) {
        fontlib_SetForegroundColor(state->dropshadow_color);
    }
    fontlib_SetCursorPosition(260, 228);
    fontlib_DrawString("Restart");

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
    

    fontlib_SetCursorPosition(121, 0);
    if (state->ide_state == IDEState_Console)
        fontlib_DrawString("Console");
    else if (state->ide_state == IDEState_RunningProgram)
         fontlib_DrawString(state->filename);


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



void initialize_console(struct estate* state, char* name) {
    state->max_buffer_size = NUM_LINES * NUM_COLS * 2;
    state->max_lines = NUM_LINES * 2;
    state->text = neon_malloc(state->max_buffer_size);
    state->lines = neon_malloc(state->max_lines * sizeof(int16_t));
    initialize(state);

    if (name != NULL) {
        state->ide_state = IDEState_RunningProgram;
        strcpy(state->filename, name);
    }
    else {
        state->ide_state = IDEState_Console;
    }

    draw_console(state);
    gfx_SwapDraw();
}


void deinit_console(struct estate* state) {
    neon_free(state->text);
    neon_free(state->lines);
    state->ide_state = IDEState_Other;
}



// Removes `nb_lines` lines of text on top of the console buffer
// This operation is invisible to the screen
// Crashes if not enough hidden lines to erase
void console_scroll(struct estate* state, int nb_lines) {
    cursor_to_end(state);

    uint8_t line_number = 0;
    int index = -1;
    uint8_t col_count = 0;
    uint8_t history_erase = 0;
    uint8_t real_line_count = 0; // Counts the number of \n that we will erase
    while (line_number < nb_lines) {
        index++;

        // Count how many history line we will delete
        if (index >= state->history_left[history_erase]) {
            history_erase++;
        }

        if (state->text[index] == 0x1b) {
            index+=1;
            continue;
        }

        if (state->text[index] == '\n') {
            col_count = 0;
            line_number++;
            real_line_count++;
        }
        else if (col_count == NUM_COLS - 1) {
            col_count = 0;
            line_number++;
        }

        if (state->text[index] != '\n') {
            col_count++;
        }
    }

    state->c1 -= index;
    state->lc1 -= real_line_count;
    state->scr_offset -= index;
    memmove(state->text, state->text + index, state->max_buffer_size - index);

    // Remove history lines that do not exist anymore in the buffer
    // and update the indices according to the new layout of the console
    for (int i=0 ; i < state->history_length - history_erase ; i++) {
        state->history_left[i] = state->history_left[i + history_erase] - index;
        state->history_right[i] = state->history_right[i + history_erase] - index;
    }
    state->history_length -= history_erase;
}

void scroll_if_needed(struct estate* state) {
    if (state->c2 - state->c1 < NUM_COLS * INPUT_MAX_LINES || state->lc1 > state->max_lines - INPUT_MAX_LINES) {
        console_scroll(state, state->max_lines - NUM_LINES - INPUT_MAX_LINES);
    }
}


void neonide_print_string(char* text) {
    if (!global_console_state)
        return;

    // increments each character displayed
    static uint8_t counter = 0;

    scroll_if_needed(global_console_state);
    cursor_to_end(global_console_state);

    while (*text) {
        insert_char(global_console_state, *text);

        // Flush the console at \n and every FLUSH_FREQUENCY characters
        counter = (counter + 1)%FLUSH_FREQUENCY;
        if (*text == '\n' || counter == 0) {
            neonide_flush();
        }
        text++;
    }
}


void history_navigate(struct estate* state) {
    if (state->history_length == 0)
        return;

    cursor_to_end(state);
    int index = state->history_length;
    short k = KEY_UP;
    while (true) {
        if (k == KEY_UP && index > 0) {
            index--;
            while (state->c1 > state->history_right[index])
                cursor_left(state);

            while (state->c1 > state->history_left[index])
                cursor_left_select(state);
        }
        else if (k == KEY_DOWN && index < state->history_length - 1) {
            index++;
            while (state->c1 < state->history_left[index])
                cursor_right(state);

            while (state->c1 < state->history_right[index])
                cursor_right_select(state);
        }
        else if (k == KEY_DOWN && index == state->history_length - 1) {
            cursor_to_end(state);
            return;
        }
        else if (k == '\n') {
            cursor_to_end(state);
            while (state->c1 > state->last_prompt_start)
                bs(state);

            for (int i=state->history_left[index] ; i < state->history_right[index] ; i++) {
                insert_char(state, state->text[i]);
            }
            return;
        }
        else if (k == KEY_CLEAR) {
            cursor_to_end(state);
            return;
        }

        draw_console(state);
        gfx_SwapDraw();
        k = ngetchx(state);
    }
}

void neonide_flush(void) {
    if (!global_console_state)
        return;

    draw_console(global_console_state);
    gfx_SwapDraw();
}


void neonide_clear(void) {
    if (!global_console_state)
        return;

    global_console_state->lc1 = 0;
    global_console_state->lc2 = global_console_state->max_buffer_size - 1;
    global_console_state->lc_offset = 0;
    global_console_state->c1 = 0;
    global_console_state->c2 = global_console_state->max_buffer_size - 1;
    global_console_state->scr_offset = 0;
    global_console_state->scr_line_offset = 0;
}


uint8_t translate_color(uint8_t neon_color_constant) {
    switch (neon_color_constant) {
        case BLUE:
            return NEON_PALETTE_BLUE;
        case GREEN:
            return NEON_PALETTE_GREEN;
        case RED:
            return NEON_PALETTE_RED;
        case PURPLE:
            return NEON_PALETTE_PURPLE;
        case GREY:
            return NEON_PALETTE_GREY;
        case ORANGE:
            return NEON_PALETTE_ORANGE;
        default:
            return NEON_PALETTE_BLACK;
    }
}

void neonide_set_color(uint8_t color) {
    if (!global_console_state)
        return;

    scroll_if_needed(global_console_state);
    cursor_to_end(global_console_state);
    insert_char(global_console_state, 0x1b);
    insert_char(global_console_state, translate_color(color));
}

void console_cursor_left(struct estate *state)
{
    if (state->selection_active)
    {
        state->selection_active = false;
        if (state->selection_anchor > state->c2 + 1)
            return;
        while (state->selection_anchor < state->c1 - 2)
            cursor_left(state);
    }
    if (state->c1 > state->last_prompt_start)
    {
        if (state->lc_offset == 0)
        {
            //Moving on same line
            state->lc_offset--;
        }
        state->c1--;
        state->text[state->c2] = state->text[state->c1];
        state->c2--;
    }
}



void console_cursor_left_select(struct estate *state)
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
    if (state->c1 > state->last_prompt_start)
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


void console_bs(struct estate *state)
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
    else if (state->c1 > state->last_prompt_start)
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



char* neonide_input(char* prompt) {
    if (!global_console_state)
        return NULL;

    if (strcmp(prompt, SEQUENCE_ENTREE) == 0) {
        neonide_set_color(BLUE);
        neonide_print_string(SEQUENCE_ENTREE);
        neonide_set_color(DEFAULT);
    }
    else {
        neonide_print_string(prompt);
    }

    // We need the cursor to be at the end - this is currently done by neonide_print_string
    struct estate *state = global_console_state;

    short k = 0;
    state->last_prompt_start = state->c1;

    while (true) {
        draw_console(state);
        gfx_SwapDraw();

        k = ngetchx_xy(state, state->cx, state->cy);

        if (!is_control(k))
        {
            if (k == '\n') {
                cursor_to_end(state);

                // Add content to history
                if (state->c1 != state->last_prompt_start) {
                    state->history_left[state->history_length] = state->last_prompt_start;
                    state->history_right[state->history_length] = state->c1;
                    state->history_length++;
                }

                char* buffer = neon_malloc(state->c1 - state->last_prompt_start + 1);
                for (int i=0 ; i + state->last_prompt_start < state->c1 ; i++) {
                    buffer[i] = state->text[i + state->last_prompt_start];
                }
                buffer[state->c1 - state->last_prompt_start] = '\0';

                insert_char(state, '\n');
                return buffer;
            }
            else {
                insert_char(state, k);
            }
        }
        else
        {
            switch (k) {
                case KEY_F1:
                case KEY_CLEAR:
                    neon_fail(1, NO_ARGS);
                    return NULL;
                case KEY_SLEFT:
                    console_cursor_left_select(state);
                    break;
                case KEY_SRIGHT:
                    cursor_right_select(state);
                    break;
                case KEY_LEFT: //left
                    console_cursor_left(state);
                    break;
                case KEY_RIGHT: //right
                    cursor_right(state);
                    break;
                case KEY_DOWN: //down
                    break;
                case KEY_UP: //up
                    history_navigate(state);
                    break;
                case KEY_BS: //backspace
                    console_bs(state);
                    break;
                case KEY_DEL: //delete
                    del(state);
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
                case KEY_F2:
                    draw_console(state);
                    gfx_SwapDraw();
                    show_chars_dialog(state, draw_console);
                    draw_console(state);
                    gfx_SwapDraw();
                    break;
                case KEY_F3:
                    draw_console(state);
                    gfx_SwapDraw();
                    show_console_tools_dialog(state);
                    draw_console(state);
                    gfx_SwapDraw();
                    break;
                case KEY_F4:
                    if (state->ide_state == IDEState_RunningProgram) {
                        state->ide_goto = IDEState_Editor;
                        neon_fail(1, NO_ARGS);
                        return NULL;
                    }
                    break;
                case KEY_F5:
                    if (state->ide_state == IDEState_RunningProgram) {
                        state->ide_goto = IDEState_RunningProgram;
                        neon_fail(1, NO_ARGS);
                        return NULL;
                    }
                    break;
                case KEY_OPEN:
                    break;
                case KEY_STATE:
                    break;
            }
        }
    }
}



void start_console(struct estate* state)
{
    global_console_state = state;
    initialize_console(state, NULL);
    neonInit();
    run_interactive();
    neonExit();

    gfx_SetDrawBuffer();
    deinit_console(state);
    global_console_state = NULL;
}


void run_neon_program(struct estate* state, char* name) {
    // Initialize the global console
    global_console_state = state;
    initialize_console(state, name);

    neonInit();
    NeObj l = neo_list_create(0);
    variable_append(global_env, "__args__", l);

    execFile(name);
    if_error {
        neon_reset_error();
        goto function_end;
    }

    terminal();

function_end:
    neonExit();
    gfx_SetDrawBuffer();
    deinit_console(state);
    global_console_state = NULL;
}