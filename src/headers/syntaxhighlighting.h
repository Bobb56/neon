#ifndef SYNTAXHIGHLIGHTING_H
#define SYNTAXHIGHLIGHTING_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "constants.h"

typedef struct SHState {
    uint8_t wa_state;
    bool in_string1;
    bool in_string2;
    bool in_short_comment;
    bool in_long_comment;
    bool in_word;
    bool in_number;
    bool in_operator;
    size_t word_start;
    size_t index;
    uint8_t* colors;
} SHState;


#define KEYWORD_COLOR       BLUE
#define OPERATOR_COLOR      BLUE
#define CONSTANT_COLOR      PURPLE
#define COMMENT_COLOR       GREY
#define STRING_COLOR        GREEN
#define DIGIT_COLOR         ORANGE
#define SPECIAL_CHAR_COLOR  PURPLE
#define RESET_COLOR         DEFAULT


void sh_set_to_initial(void);
void preprocessing_update_state(SHState* state, char c);
void print_highlighted(char* text, size_t length);
void sh_reset_initial_state(void);
void sh_update_initial_state(void);
uint8_t* highlight(char* text, size_t length);
void init_sh_process(size_t text_length);
void update_state(SHState* state, char c);
void close_state(SHState* state);
SHState* sh_get_state_ptr(void);

#endif