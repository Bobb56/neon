#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "wordautomaton.h"
#include "../headers/parser.h"

/*
TODO:
- Add new colors in neonio and use those colors
- Highlight syntax when printing result in the console
*/


#include "syntaxhighlighting.h"


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
    else if (state->in_word && c != '\'' && c != '_' && !isalnum(c)) {
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
        // We authorize to process the character again
        goto beginning;
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
            if (!state->in_word) {
                state->in_word = true;
                state->wa_state = WA_START;
                state->word_start = state->index;
            }

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
    }
}


SHState init_sh_process(size_t text_length) {
    SHState state = (SHState) {
        .wa_state = WA_START,
        .word_start = 0,
        .in_word = false,
        .in_long_comment = false,
        .in_short_comment = false,
        .in_string1 = false,
        .in_string2 = false,
        .in_number = false,
        .index = 0,
        .colors = malloc(text_length+2)
    };
    memset(state.colors, 0, text_length);
    return state;
}


uint8_t* highlight(char* text, size_t length) {
    SHState sh_state = init_sh_process(length);
    for (size_t i=0 ; i < length ; i++) {
        update_state(&sh_state, text[i]);
    }
    close_state(&sh_state);
    return sh_state.colors;
}


void print_highlighted(char* text, size_t length) {
    uint8_t* colors = highlight(text, length);

    for (size_t i=0 ; i < length ; i++) {
        switch (colors[i]) {
            case OPERATOR_COLOR:
            case KEYWORD_COLOR:
                printf("\033[1;34m");
                break;
            case STRING_COLOR:
            case COMMENT_COLOR:
                printf("\033[0;32m");
                break;
            case SPECIAL_CHAR_COLOR:
                printf("\033[0;35m");
                break;
            case DIGIT_COLOR:
                printf("\033[0;31m");
                break;
            case RESET_COLOR:
                printf("\033[0;00m");
                break;
        }

        putchar(text[i]);
    }
    printf("\033[0;00m");
    free(colors);
}

