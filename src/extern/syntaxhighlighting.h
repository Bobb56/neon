#ifndef SYNTAXHIGHLIGHTING_H
#define SYNTAXHIGHLIGHTING_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct SHState {
    uint8_t wa_state;
    bool in_string1;
    bool in_string2;
    bool in_short_comment;
    bool in_long_comment;
    bool in_word;
    bool in_number;
    bool in_operator;
    uint8_t word_start;
    size_t index;
    uint8_t* colors;
} SHState;


#define KEYWORD_COLOR       1
#define OPERATOR_COLOR      2
#define COMMENT_COLOR       3
#define STRING_COLOR        4
#define DIGIT_COLOR         5
#define SPECIAL_CHAR_COLOR  6
#define RESET_COLOR         7


void print_highlighted(char* text, size_t length);

#endif