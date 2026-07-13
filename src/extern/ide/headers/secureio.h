/*
 * neonide.h
 *
 *  Created on: Jul 25, 2021
 *      Author: michael
 */

#ifndef SRC_SECUREIO_H_
#define SRC_SECUREIO_H_

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
#include "tigcclib.h"
#include "editor.h"
#include "dialogs.h"
#include "gfx.h"
#include "libmalloc.h"

void secureio_WriteFile(struct estate *state);
void secureio_Close(struct estate* state, uint8_t handle);
uint8_t secureio_Open(struct estate* state, char* name, const char* mode);
void secureio_Read(struct estate* state, char* dst, size_t size, uint8_t handle);
void secureio_Write(struct estate* state, char* dst, size_t size, uint8_t handle);
void secureio_Delete(struct estate* state, char* name);

#endif /* SRC_SECUREIO_H_ */