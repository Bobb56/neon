#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "state.h"

int console_handle_key(struct estate *state, short k);
void start_console(void);
void run_neon_program(char* name);

void neonide_print_string(char* string);
char* neonide_input(char* prompt);
void neonide_set_color(uint8_t color);
void neonide_clear(void);
void neonide_flush(void);

#endif