
/*
 * This line editing library for Neon is based on https://github.com/deqart/deadline
 * ------------------------------
 * Copyright (c) 2023, Baran Avci
 *
 * Under the MIT License
 * ------------------------------
 *
 * Some parts of this file are inspired from https://github.com/antirez/linenoise
 * ------------------------------------------------------------------------
 * Copyright (c) 2010-2023, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2010-2013, Pieter Noordhuis <pcnoordhuis at gmail dot com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------
 *
 */

#define NEON_SOURCE_ID 24

#include "../headers/constants.h"
#ifdef LINUX

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "../headers/errors.h"
#include "../headers/neonio.h"
#include "../headers/neon.h"

#include "syntaxhighlighting.h"

//VT100 escape sequences
#define CTRL_D                  4
#define CTRL_C                  3
#define CTRL_DEL                8
#define TAB                     9
#define END_OF_LINE             13
#define BACKSPACE               127
#define LEFT_ARROW              0x445b1b
#define RIGHT_ARROW             0x435b1b
#define UP_ARROW                0x415b1b
#define DOWN_ARROW              0x425b1b
#define DEL                     0x7e335b1b
#define CTRL_RIGHT              0x43353b315b1b
#define CTRL_LEFT               0x44353b315b1b

#define TO_KEY(key)   (*(uintptr_t*)key)


#define clear_cursor_down() fputs("\033[J", stdout)
#define save_cursor_state() fputs("\0337", stdout)
#define restore_cursor_state() fputs("\0338", stdout)
#define move_cursor_to(x, y) printf("\033[%d;%dH", y, x)

#define IS_TRAILING_BYTE(b) ((b & 0xc0) == 0x80)

static struct {
    int width;
    int height;
} term;

static struct {
    int x;
    int y;
} cursor;

static void get_cursor_pos(int *x, int *y)
{
    char buf[16] = {0};
    fputs("\033[6n", stdout);
    fflush(stdout);

    int ch = 0;
    for (int i = 0; i < 15 && ch != 'R'; ++i)
    {
        //TODO: replace with a std libc function
        read(STDIN_FILENO, &ch, 1);
        buf[i] = ch;
    }
    *y = atoi(&buf[2]);
    *x = atoi(&buf[strcspn(buf, ";")+1]);
}

static void get_term_size(void)
{
    struct winsize ws;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    term.width = ws.ws_col;
    term.height = ws.ws_row;
}

static void move_left(void)
{
    if (cursor.x == 1 && term.height != 1)
    {
        cursor.x = term.width;
        cursor.y--;
        return;
    }

    cursor.x--;
}

static void move_right(void)
{
    if (cursor.x == term.width)
    {
        if (cursor.y == term.height)
        {
            putchar('\n');
            restore_cursor_state();
            int new_x = 0, new_y = 0;
            get_cursor_pos(&new_x, &new_y);
            move_cursor_to(new_x, new_y-1);
            save_cursor_state();
            fflush(stdout);
            cursor.y--;
        }
        cursor.x = 1;
        cursor.y++;
        return;
    }

    cursor.x++;
}

static inline void re_pos_cursor(size_t line_size)
{
    restore_cursor_state();
    get_cursor_pos(&cursor.x, &cursor.y);
    for (size_t i = 0; i < line_size; ++i)
        move_right();
}

static inline size_t utf8len(const char *str)
{
    size_t len = 0;
    while (*str)
    {
        if (!IS_TRAILING_BYTE(*str))
            len++;
        str++;
    }
    return len;
}

static volatile sig_atomic_t re_render = false;
static struct termios new_set, old_set;

static void sig_handler(int sig)
{
    if (sig == SIGWINCH)
        re_render = true;
}


static void init_deadline(void)
{
    static bool inited = false;

    if (!inited) {
        // Sets a handler function for detecting when the term window is resized
        struct sigaction sigact = {
            .sa_handler = sig_handler,
            .sa_flags = 0
        };

        sigaction(SIGWINCH, &sigact, NULL);


        tcgetattr(STDIN_FILENO, &old_set);


        // Put term into raw mode
        new_set = old_set;  /* modify the original mode */
        /* input modes: no break, no CR to NL, no parity check, no strip char,
        * no start/stop output control. */
        new_set.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        /* output modes - disable post processing */
        new_set.c_oflag &= ~(OPOST);
        /* control modes - set 8 bit chars */
        new_set.c_cflag |= (CS8);
        /* local modes - choing off, canonical off, no extended functions,
        * no signal chars (^Z,^C) */
        new_set.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        /* control chars - set return condition: min number of bytes and timer.
        * We want read to return every single byte, without timeout. */
        new_set.c_cc[VMIN] = 1; new_set.c_cc[VTIME] = 0; /* 1 byte, no timer */

        /* put term in raw mode after flushing */
        tcsetattr(STDIN_FILENO,TCSAFLUSH,&new_set);

        inited = true;
    }

    tcsetattr(STDIN_FILENO, TCSADRAIN, &new_set);
}

static void print_prompt(const char* prompt) {
    // Rewrite the prompt, with blue coloring if it is SEQUENCE_ENTREE
    if (strcmp(prompt, SEQUENCE_ENTREE) == 0) {
        setColor(BLUE);
        fputs(prompt, stdout);
        setColor(DEFAULT);
    }
    else {
        fputs(prompt, stdout);
    }
}

static void write_line(const char *prompt, char *buffer, size_t size)
{
    restore_cursor_state();
    clear_cursor_down();

    print_prompt(prompt);

    if (global_env->syntax_highlighting_on)
        print_highlighted(buffer, size);
    else
        fwrite(buffer, 1, size, stdout);
    move_cursor_to(cursor.x, cursor.y);
}



static void update_current_line_in_history(char* buffer, size_t size) {
    if (global_env->history->len == 0) {
        strlist_append(global_env->history, strndup(buffer, size));
    }
    else if (strcmp(global_env->history->tab[global_env->history->len - 1], buffer) != 0) {
        free(global_env->history->tab[global_env->history->len - 1]);
        global_env->history->tab[global_env->history->len - 1] = strndup(buffer, size);
    }
}



char *readline(const char *prompt)
{
    size_t prompt_len = utf8len(prompt);
    char *buffer = malloc(256);
    if (buffer == NULL) return NULL;
    memset(buffer, 0, 256);
    
    size_t size = 256;
    size_t pos = 0; // This is the position of the last written byte in buffer
    size_t diff = 0; // This is the offset between the pos and the actual position of the cursor

    strlist_append(global_env->history, strdup(""));
    size_t history_index = 0;
    bool history_navigating = false;

    init_deadline();

    save_cursor_state();
    print_prompt(prompt);
    get_cursor_pos(&cursor.x, &cursor.y);
    while (true)
    {
        fflush(stdout);

        uint8_t key[8] = {0};
        read(STDIN_FILENO, key, 6);
        
        //printf("char: \n\n\n%ld\n\n\n", TO_KEY(key));

        // Handle history navigating start and end
        if (TO_KEY(key) == UP_ARROW || TO_KEY(key) == DOWN_ARROW) {
            if (!history_navigating) {
                history_navigating = true;

                update_current_line_in_history(buffer, pos);
                history_index = global_env->history->len - 1;
            }
        }
        // If it's a key that will modify the content of the buffer
        else if (TO_KEY(key) != RIGHT_ARROW && TO_KEY(key) != LEFT_ARROW) {
            if (history_navigating) {
                history_navigating = false;
            }
        }


        switch (TO_KEY(key))
        {
            case BACKSPACE:
            {
                if (pos - diff > 0) {
                    move_left();

                    size_t rm_amount = 1;
                    while (IS_TRAILING_BYTE(buffer[pos-diff-rm_amount]))
                        rm_amount++;

                    for (size_t i = 0; i < rm_amount; ++i)
                    {
                        pos--;
                        for (size_t i = pos-diff; i < pos; ++i)
                            buffer[i] = buffer[i+1];
                    }
                    write_line(prompt, buffer, pos);
                }
                break;
            }
            case DEL: {
                if (pos > 0 && diff > 0) {
                    size_t rm_amount = 1;
                    while (pos-diff+rm_amount <= pos && IS_TRAILING_BYTE(buffer[pos-diff+rm_amount]))
                        rm_amount++;

                    for (size_t i = 0; i < rm_amount; ++i)
                    {
                        for (size_t i = pos-diff; i < pos; ++i)
                            buffer[i] = buffer[i+1];
                    }
                    write_line(prompt, buffer, pos);
                }
                break;
            }
            case CTRL_D: {
                neon_fail(1, NO_ARGS);
                free(buffer);
                buffer = "\0";
                goto readline_exit;
            }
            case CTRL_C: {
                free(buffer);
                buffer = NULL;
                goto readline_exit;
            }
            case END_OF_LINE: {
                buffer[pos] = '\0';
                update_current_line_in_history(buffer, pos);

                // We don't add the line if it is empty or the same as the previous
                size_t history_length = global_env->history->len;
                bool remove_last_line = false;

                // We check if it's the same as the previous
                if (history_length >= 2) {
                    char* last_line = global_env->history->tab[history_length-1];
                    char* last_last_line = global_env->history->tab[history_length-2];
                    if (strcmp(last_last_line, last_line) == 0) {
                        remove_last_line = true;
                    }
                }
                // We check if it's empty
                if (strlen(global_env->history->tab[history_length - 1]) == 0) {
                    remove_last_line = true;
                }
                // And remove it if needed
                if (remove_last_line)
                    strlist_remove(global_env->history, history_length - 1, true);

                goto readline_exit;
            }
            case RIGHT_ARROW: {
                if (diff > 0) {
                    diff--;
                    while (diff > 0 && buffer[pos-diff] && IS_TRAILING_BYTE(buffer[pos-diff]))
                        diff--;
                    move_right();
                    move_cursor_to(cursor.x, cursor.y);
                }
                break;
            }
            case LEFT_ARROW: {
                if (pos - diff > 0) {
                    diff++;
                    while (diff < pos && IS_TRAILING_BYTE(buffer[pos-diff]))
                        diff++;
                    move_left();
                    move_cursor_to(cursor.x, cursor.y);
                }
                break;
            }

            case UP_ARROW: {
                if (history_index > 0) {
                    history_index--;
                    strcpy(buffer, global_env->history->tab[history_index]);
                    pos = strlen(buffer);
                    diff = 0;

                    re_render = true;
                }
                break;
            }

            case DOWN_ARROW: {
                if (history_index + 1 < global_env->history->len) {
                    history_index++;
                    strcpy(buffer, global_env->history->tab[history_index]);
                    pos = strlen(buffer);
                    diff = 0;

                    re_render = true;
                }
                break;
            }

            case TAB: {
                const size_t tab_size = 2;
                
                // Resize buffer if needed
                if (pos + tab_size >= size) {
                    while (pos + tab_size >= size) {
                        size <<= 1;
                    }
                    buffer = realloc(buffer, size);
                        if (buffer == NULL) goto readline_exit;
                }

                // Move characters if we insert characters
                for (size_t i = pos + tab_size - 1 ; i < pos-diff ; i--) {
                    buffer[i] = buffer[i-4];
                }

                for (size_t i=0 ; i < tab_size ; i++) {
                    buffer[pos-diff] = ' ';
                    pos++;
                    move_right();
                }

                write_line(prompt, buffer, pos);
                break;
            }

            case CTRL_RIGHT:
            case CTRL_LEFT:
            case CTRL_DEL:
                break;

            default: {

                // If unknown escape sequence, quit
                if ((TO_KEY(key) & 127) == 0x1b)
                    break;

                for (int i=0 ; key[i] != 0 && i < 6; i++) {
                    int ch = key[i];

                    if (!IS_TRAILING_BYTE(ch))
                        move_right(); // for added characters

                    if (ch == '\n') break;

                    for (size_t i = pos; pos-diff < i; --i)
                        buffer[i] = buffer[i-1];
                    buffer[pos-diff] = ch;
                    pos++;
                    if (pos == size)
                    {
                        size <<= 1;
                        buffer = realloc(buffer, size);
                        if (buffer == NULL) goto readline_exit;
                    }

                    write_line(prompt, buffer, pos);
                }
                break;
            }
        }

        get_term_size();

        if (re_render)
        {
            re_render = false;
            re_pos_cursor(utf8len(buffer)+prompt_len);
            write_line(prompt, buffer, pos);
            continue;
        }
    }

readline_exit:
    tcsetattr(STDIN_FILENO, TCSANOW, &old_set);
    putchar('\n');
    return buffer;
}


#endif