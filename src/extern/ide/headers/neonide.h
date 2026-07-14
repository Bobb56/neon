/*
 * neonide.h
 *
 *  Created on: Jul 25, 2021
 *      Author: michael
 */

#ifndef SRC_CEDIT_H_
#define SRC_CEDIT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <fileioc.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphx.h>
#include <keypadc.h>
#include <fontlibc.h>

#include "state.h"
#include "keys.h"
#include "editor.h"
#include "dialogs.h"
#include "gfx.h"
#include "libmalloc.h"


#define NEONIDE_VERSION_STRING          "NeonIDE 1.0"

#define NEON_PLAIN_TEXT_HEADER          "#NEON"
#define NEON_DEFAULT_FILE_HEADER        "NEON\x00"
#define NEON_DEFAULT_FILE_HEADER_SIZE   5

bool initialize(struct estate *state);
void initialize_void(struct estate *state);

uint8_t create_buffer(size_t size);
void delete_buffer(uint8_t slot);

/*
 * Parse the NEIDERC file, if it exists, and do stuff.
 */
void parse_rc(struct estate *);

#endif /* SRC_CEDIT_H_ */