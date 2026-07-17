#include <fontlibc.h>

static const uint8_t font_data[] = {
    #include "fonts/8x13.inc"
};
const fontlib_font_t *font = (fontlib_font_t *)font_data;

/*
static const uint8_t font_bold_data[] = {
    #include "fonts/8x13Boldified.inc"
};
const fontlib_font_t *font_bold = (fontlib_font_t *)font_bold_data;
*/