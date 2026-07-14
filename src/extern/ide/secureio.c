/*
 * secureio.c
 *
 *  Created on: Jul 25, 2021
 *      Author: Raphaël Le Puillandre
 */



#include "headers/neonide.h"
#include "headers/editor.h"
#include "headers/home.h"
#include "headers/libmalloc.h"
#include "headers/state.h"
#include "headers/console.h"
#include <stdlib.h>
#include <fileioc.h>
#include <string.h>


uint8_t secureio_Open(struct estate* state, char* name, const char* mode) {
    uint8_t handle = ti_Open(name, mode);
    if (state->text_buffer_handle != 0) {
        editor_update_ptrs(state);
    }
    return handle;
}

void secureio_Close(struct estate* state, uint8_t handle) {
    ti_Close(handle);
    if (state->text_buffer_handle != 0) {
        editor_update_ptrs(state);
    }
}

void secureio_Read(struct estate* state, char* dst, size_t size, uint8_t handle) {
    ti_Read(dst, size, 1, handle);
    if (state->text_buffer_handle != 0) {
        editor_update_ptrs(state);
    }
}

void secureio_Write(struct estate* state, char* dst, size_t size, uint8_t handle) {
    ti_Write(dst, size, 1, handle);
    if (state->text_buffer_handle != 0) {
        editor_update_ptrs(state);
    }
}

void secureio_WriteFile(struct estate *state)
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


    if (state->text_buffer_handle != 0) {
        editor_update_ptrs(state);
    }
    // These writes are safe because the AppVar has already the right size
    ti_Write(state->text, state->c1, 1, var);
    ti_Write(state->text + state->c2 + 1, state->max_buffer_size - (state->c2 + 1), 1, var);

    // Archive file if needed
    ti_SetArchiveStatus(archive, var);

    ti_Close(var);
    state->saved = true;

    if (state->text_buffer_handle != 0) {
        editor_update_ptrs(state);
    }
}


void secureio_Delete(struct estate* state, char* name) {
    ti_Delete(name);
    if (state->text_buffer_handle != 0) {
        editor_update_ptrs(state);
    }
}