#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "headers/constants.h"
#include "headers/parser.h"
#include "headers/wordautomaton.h"
#include "headers/syntaxhighlighting.h"

#ifdef TI_EZ80
#include "extern/ide/headers/libmalloc.h"
#endif

/*
TODO:
- Add new colors in neonio and use those colors
- Highlight syntax when printing result in the console
*/


static SHState sh_state = (SHState) {
    .wa_state = WA_START,
    .word_start = 0,
    .in_word = false,
    .in_long_comment = false,
    .in_short_comment = false,
    .in_string1 = false,
    .in_string2 = false,
    .in_number = false,
    .index = 0,
    .colors = NULL
};

static SHState sh_initial_state;


// Some characters are processes 2 times
// Only characters that mark the end of a type on the previous character
// So characters that set state->colors[state->index] to RESET_COLOR
// and not characters that set state->colors[state->index+1] to RESET_COLOR
void update_state(SHState* state, char c) {
beginning:
    if (state->in_operator) {
        if (!is_sousop(c)) {
            state->in_operator = false;
            state->colors[state->index] = RESET_COLOR;
            // We authorize to process the character again
            goto beginning;
        }
    }
    else if (state->in_number) {
        if (!isdigit(c) && c != 'x' && c != 'b' && c != '.') {
            state->in_number = false;
            state->colors[state->index] = RESET_COLOR;
            // We authorize to process the character again
            goto beginning;
        }
    }
    else if (state->in_short_comment) {
        if (c == '\n') {
            state->in_short_comment = false;
            state->colors[state->index] = RESET_COLOR;
        }
    }
    else if (state->in_long_comment) {
        if (c == '$') {
            state->in_long_comment = false;
            state->colors[state->index+1] = RESET_COLOR;
        }
    }
    else if (state->in_string1) {
        if (c == '\'') {
            state->in_string1 = false;
            state->colors[state->index+1] = RESET_COLOR;
        }
    }
    else if (state->in_string2) {
        if (c == '"') {
            state->in_string2 = false;
            state->colors[state->index+1] = RESET_COLOR;
        }
    }
    else if (state->in_word) {
        if (c != '\'' && c != '_' && !isalnum(c)) {
            state->in_word = false;
            
            uint8_t final_state = wa_get_final(state->wa_state);
            if (final_state == WA_KEYWORD) {
                state->colors[state->word_start] = KEYWORD_COLOR;
                state->colors[state->index] = RESET_COLOR;
            }
            else if (final_state == WA_OPERATOR) {
                state->colors[state->word_start] = OPERATOR_COLOR;
                state->colors[state->index] = RESET_COLOR;
            }
            else if (final_state == WA_CONSTANT) {
                state->colors[state->word_start] = CONSTANT_COLOR;
                state->colors[state->index] = RESET_COLOR;
            }
            // We authorize to process the character again
            goto beginning;
        }
        else {
            state->wa_state = wa_next_state(state->wa_state, c);
        }
    }
    else {
        if (c == '$') {
            state->in_long_comment = true;
            state->colors[state->index] = COMMENT_COLOR;
        }
        else if (c == '#') {
            state->in_short_comment = true;
            state->colors[state->index] = COMMENT_COLOR;
        }
        else if (c == '"') {
            state->in_string2 = true;
            state->colors[state->index] = STRING_COLOR;
        }
        else if (c == '\'' && !state->in_word) {
            state->in_string1 = true;
            state->colors[state->index] = STRING_COLOR;
        }
        else if (c == '_' || isalpha(c)) {
            state->in_word = true;
            state->wa_state = WA_START;
            state->word_start = state->index;
            state->wa_state = wa_next_state(state->wa_state, c);
        }
        else if (isdigit(c)) {
            state->in_number = true;
            state->colors[state->index] = DIGIT_COLOR;
        }
        else if (is_sousop(c)) {
            state->in_operator = true;
            state->colors[state->index] = SPECIAL_CHAR_COLOR;
        }
    }
    state->index++;
}



/*
This function does the same job than update_state but in order to preprocess
the beginning of a string before coloring the next characters it
*/
void preprocessing_update_state(SHState* state, char c) {
beginning:
    if (state->in_operator) {
        if (!is_sousop(c)) {
            state->in_operator = false;
            // We authorize to process the character again
            goto beginning;
        }
    }
    else if (state->in_number) {
        if (!isdigit(c) && c != 'x' && c != 'b' && c != '.') {
            state->in_number = false;
            // We authorize to process the character again
            goto beginning;
        }
    }
    else if (state->in_short_comment) {
        if (c == '\n') {
            state->in_short_comment = false;
        }
    }
    else if (state->in_long_comment) {
        if (c == '$') {
            state->in_long_comment = false;
        }
    }
    else if (state->in_string1) {
        if (c == '\'') {
            state->in_string1 = false;
        }
    }
    else if (state->in_string2) {
        if (c == '"') {
            state->in_string2 = false;
        }
    }
    else if (state->in_word) {
        if (c != '\'' && c != '_' && !isalnum(c)) {
            state->in_word = false;
            // We authorize to process the character again
            goto beginning;
        }
        else {
            state->wa_state = wa_next_state(state->wa_state, c);
        }
    }
    else {
        if (c == '$') {
            state->in_long_comment = true;
        }
        else if (c == '#') {
            state->in_short_comment = true;
        }
        else if (c == '"') {
            state->in_string2 = true;
        }
        else if (c == '\'' && !state->in_word) {
            state->in_string1 = true;
        }
        else if (c == '_' || isalpha(c)) {
            state->in_word = true;
            state->wa_state = WA_START;
            state->wa_state = wa_next_state(state->wa_state, c);
        }
        else if (isdigit(c)) {
            state->in_number = true;
        }
        else if (is_sousop(c)) {
            state->in_operator = true;
        }
    }
}



void close_state(SHState* state) {
    if (state->in_word) {
        uint8_t final_state = wa_get_final(state->wa_state);
        if (final_state == WA_KEYWORD) {
            state->colors[state->word_start] = KEYWORD_COLOR;
            state->colors[state->index] = RESET_COLOR;
        }
        else if (final_state == WA_OPERATOR) {
            state->colors[state->word_start] = OPERATOR_COLOR;
            state->colors[state->index] = RESET_COLOR;
        }
        else if (final_state == WA_CONSTANT) {
            state->colors[state->word_start] = CONSTANT_COLOR;
            state->colors[state->index] = RESET_COLOR;
        }
    }
}

// We initialize a new syntax highlighting process
// while keeping the last parameters
void init_sh_process(size_t text_length) {
    sh_state = sh_initial_state;
    #ifdef TI_EZ80
        sh_state.colors = malloc_noheap(text_length + 1);
    #else
        sh_state.colors = malloc(text_length + 1);
    #endif

    memset(sh_state.colors, NO_COLOR, text_length);

    sh_state.index = 0;

    if (sh_state.in_word) {
        sh_state.word_start = 0;
    }
    else if (sh_state.in_string1 || sh_state.in_string2) {
        sh_state.colors[0] = STRING_COLOR;
    }
    else if (sh_state.in_long_comment || sh_state.in_short_comment) {
        sh_state.colors[0] = COMMENT_COLOR;
    }
    else if (sh_state.in_number) {
        sh_state.colors[0] = DIGIT_COLOR;
    }
    else if (sh_state.in_operator) {
        sh_state.colors[0] = SPECIAL_CHAR_COLOR;
    }
}

void sh_reset_initial_state(void) {
    sh_initial_state = (SHState) {
        .wa_state = WA_START,
        .word_start = 0,
        .in_word = false,
        .in_long_comment = false,
        .in_short_comment = false,
        .in_string1 = false,
        .in_string2 = false,
        .in_number = false,
        .index = 0,
        .colors = NULL
    };
}

SHState* sh_get_state_ptr(void) {
    return &sh_state;
}

void sh_update_initial_state(void) {
    sh_initial_state = sh_state;
}

void sh_set_to_initial(void) {
    sh_state = sh_initial_state;
}


uint8_t* highlight(char* text, size_t length) {
    init_sh_process(length);
    for (size_t i=0 ; i < length ; i++) {
        update_state(&sh_state, text[i]);
    }
    close_state(&sh_state);
    return sh_state.colors;
}