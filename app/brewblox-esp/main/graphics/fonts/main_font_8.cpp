/*******************************************************************************
 * Size: 8 px
 * Bpp: 4
 * Opts: --no-compress --no-prefilter --bpp 4 --size 8 --font Roboto-Medium.ttf -r 0x20-0x7F,0xB0,0x2022 --font MaterialIcons-Regular.ttf -r 0xe1ba,0xe1da,0xe328 --format lvgl -o main_font_8.cpp --force-fast-kern-format
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef MAIN_FONT_8
#define MAIN_FONT_8 1
#endif

#if MAIN_FONT_8

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x69, 0x68, 0x58, 0x35, 0x47,

    /* U+0022 "\"" */
    0x97, 0x38, 0x62,

    /* U+0023 "#" */
    0x1, 0x89, 0x13, 0xba, 0xd6, 0x8, 0x2a, 0x5,
    0xda, 0xc3, 0xa, 0x54, 0x0,

    /* U+0024 "$" */
    0x0, 0x50, 0x0, 0x8e, 0x50, 0x5b, 0x1e, 0x1,
    0xd6, 0x0, 0x1, 0x6d, 0x7, 0xa3, 0xe1, 0x7,
    0xd4, 0x0, 0x2, 0x0,

    /* U+0025 "%" */
    0x68, 0x80, 0x30, 0x48, 0x68, 0x10, 0x0, 0x6a,
    0x92, 0x2, 0x7a, 0x28, 0x3, 0x8, 0xa4,

    /* U+0026 "&" */
    0xc, 0xc6, 0x0, 0x1d, 0x77, 0x0, 0x2c, 0xd1,
    0x50, 0x96, 0x6e, 0x80, 0x3c, 0xac, 0x90,

    /* U+0027 "'" */
    0xa0, 0x80,

    /* U+0028 "(" */
    0x3, 0x60, 0xb0, 0x48, 0x7, 0x60, 0x66, 0x3,
    0x90, 0xa, 0x10, 0x14,

    /* U+0029 ")" */
    0x71, 0x2, 0xa0, 0xb, 0x10, 0x94, 0xa, 0x30,
    0xc0, 0x48, 0x5, 0x0,

    /* U+002A "*" */
    0x8, 0x10, 0x7d, 0xa3, 0x38, 0xa0, 0x0, 0x0,

    /* U+002B "+" */
    0x3, 0x90, 0x0, 0x3b, 0x0, 0x9d, 0xfd, 0x20,
    0x3b, 0x0,

    /* U+002C "," */
    0x75, 0x91, 0x0,

    /* U+002D "-" */
    0x8b, 0x30,

    /* U+002E "." */
    0x47,

    /* U+002F "/" */
    0x0, 0xb0, 0x56, 0xb, 0x2, 0x90, 0x92, 0x5,
    0x0,

    /* U+0030 "0" */
    0x2c, 0xb8, 0x8, 0x60, 0xe0, 0x95, 0xd, 0x28,
    0x70, 0xe0, 0x1c, 0xb8, 0x0,

    /* U+0031 "1" */
    0x18, 0xe2, 0x2f, 0x0, 0xf0, 0xf, 0x0, 0xf0,

    /* U+0032 "2" */
    0x3c, 0xc9, 0x5, 0x30, 0xe0, 0x0, 0x87, 0x0,
    0x97, 0x0, 0x7e, 0xbb, 0x20,

    /* U+0033 "3" */
    0x3c, 0xc8, 0x3, 0x21, 0xe0, 0x5, 0xd8, 0x3,
    0x20, 0xe0, 0x4c, 0xc9, 0x0,

    /* U+0034 "4" */
    0x0, 0xba, 0x0, 0x7b, 0xa0, 0x3a, 0x5a, 0xa,
    0xbc, 0xe3, 0x0, 0x5a, 0x0,

    /* U+0035 "5" */
    0x1f, 0xcc, 0x3, 0xeb, 0x70, 0x1, 0xd, 0x24,
    0x50, 0xd2, 0x1b, 0xb9, 0x0,

    /* U+0036 "6" */
    0x7, 0xb4, 0x4, 0xeb, 0x70, 0x88, 0xe, 0x17,
    0x80, 0xd2, 0x1b, 0xc8, 0x0,

    /* U+0037 "7" */
    0x8b, 0xbf, 0x20, 0x3, 0xb0, 0x0, 0xb3, 0x0,
    0x4b, 0x0, 0xc, 0x30, 0x0,

    /* U+0038 "8" */
    0x2c, 0xc9, 0x6, 0x90, 0xe0, 0x1e, 0xd8, 0x8,
    0x70, 0xe0, 0x3c, 0xca, 0x0,

    /* U+0039 "9" */
    0x3c, 0xc6, 0x9, 0x60, 0xe0, 0x2b, 0x9f, 0x0,
    0x3, 0xd0, 0x9, 0xb2, 0x0,

    /* U+003A ":" */
    0x57, 0x0, 0x0, 0x57,

    /* U+003B ";" */
    0x65, 0x0, 0x0, 0x66, 0x82,

    /* U+003C "<" */
    0x2, 0x97, 0x9c, 0x40, 0x29, 0xc4, 0x0, 0x2,

    /* U+003D "=" */
    0x5b, 0xba, 0x0, 0x0, 0x5b, 0xba,

    /* U+003E ">" */
    0x69, 0x30, 0x3, 0xbb, 0x4c, 0x93, 0x20, 0x0,

    /* U+003F "?" */
    0x5c, 0xd4, 0x31, 0x78, 0x4, 0xb0, 0x4, 0x20,
    0x8, 0x30,

    /* U+0040 "@" */
    0x3, 0x98, 0x88, 0x2, 0xa1, 0x99, 0x37, 0x73,
    0xa1, 0xb0, 0x99, 0x1c, 0xb, 0xa, 0x47, 0x98,
    0xb9, 0x40, 0x79, 0x86, 0x0,

    /* U+0041 "A" */
    0x0, 0xf5, 0x0, 0x6, 0xac, 0x0, 0xd, 0x1c,
    0x20, 0x4e, 0xbd, 0x90, 0xb4, 0x0, 0xe1,

    /* U+0042 "B" */
    0x6e, 0xbc, 0x26, 0x90, 0x96, 0x6d, 0xbe, 0x26,
    0x90, 0x68, 0x6e, 0xbd, 0x30,

    /* U+0043 "C" */
    0xa, 0xcc, 0x47, 0x90, 0x29, 0x96, 0x0, 0x7,
    0x90, 0x28, 0xa, 0xcc, 0x40,

    /* U+0044 "D" */
    0x6e, 0xcb, 0x16, 0x90, 0x6a, 0x69, 0x3, 0xc6,
    0x90, 0x6a, 0x6e, 0xcb, 0x10,

    /* U+0045 "E" */
    0x6e, 0xcc, 0x36, 0x90, 0x0, 0x6e, 0xba, 0x6,
    0x90, 0x0, 0x6e, 0xbb, 0x30,

    /* U+0046 "F" */
    0x6e, 0xcc, 0x26, 0x90, 0x0, 0x6e, 0xb9, 0x6,
    0x90, 0x0, 0x69, 0x0, 0x0,

    /* U+0047 "G" */
    0xa, 0xcc, 0x56, 0x90, 0x16, 0x96, 0x2b, 0xc6,
    0xa0, 0xf, 0x9, 0xcc, 0x80,

    /* U+0048 "H" */
    0x69, 0x0, 0xe1, 0x69, 0x0, 0xe1, 0x6e, 0xbb,
    0xf1, 0x69, 0x0, 0xe1, 0x69, 0x0, 0xe1,

    /* U+0049 "I" */
    0x5a, 0x5a, 0x5a, 0x5a, 0x5a,

    /* U+004A "J" */
    0x0, 0x1e, 0x0, 0x1e, 0x0, 0x1e, 0x62, 0x2d,
    0x5d, 0xd6,

    /* U+004B "K" */
    0x69, 0xa, 0x70, 0x69, 0xa8, 0x0, 0x6f, 0xf2,
    0x0, 0x6a, 0x4d, 0x0, 0x69, 0x7, 0xa0,

    /* U+004C "L" */
    0x69, 0x0, 0x6, 0x90, 0x0, 0x69, 0x0, 0x6,
    0x90, 0x0, 0x6e, 0xbb, 0x10,

    /* U+004D "M" */
    0x6f, 0x10, 0x1f, 0x66, 0xd8, 0x8, 0xd6, 0x68,
    0xd0, 0xd8, 0x66, 0x88, 0xb7, 0x86, 0x69, 0x1f,
    0x19, 0x60,

    /* U+004E "N" */
    0x6d, 0x0, 0xe1, 0x6e, 0xa0, 0xe1, 0x69, 0xa5,
    0xe1, 0x69, 0x1d, 0xf1, 0x69, 0x4, 0xf1,

    /* U+004F "O" */
    0xa, 0xcc, 0x40, 0x69, 0x1, 0xe0, 0x96, 0x0,
    0xe1, 0x69, 0x1, 0xe0, 0x9, 0xcc, 0x40,

    /* U+0050 "P" */
    0x6e, 0xcd, 0x56, 0x90, 0x4c, 0x6e, 0xcc, 0x46,
    0x90, 0x0, 0x69, 0x0, 0x0,

    /* U+0051 "Q" */
    0xa, 0xcc, 0x40, 0x79, 0x1, 0xe0, 0x96, 0x0,
    0xe1, 0x79, 0x1, 0xe0, 0xa, 0xce, 0x60, 0x0,
    0x2, 0x90,

    /* U+0052 "R" */
    0x6e, 0xbc, 0x36, 0x90, 0x79, 0x6e, 0xcd, 0x16,
    0x90, 0xe1, 0x69, 0x7, 0x90,

    /* U+0053 "S" */
    0x2c, 0xbb, 0x17, 0x90, 0x43, 0x7, 0xb8, 0x6,
    0x30, 0x97, 0x2b, 0xbc, 0x20,

    /* U+0054 "T" */
    0x9c, 0xfc, 0x80, 0x1e, 0x0, 0x1, 0xe0, 0x0,
    0x1e, 0x0, 0x1, 0xe0, 0x0,

    /* U+0055 "U" */
    0x87, 0x4, 0xb8, 0x70, 0x4b, 0x87, 0x4, 0xb7,
    0x80, 0x5a, 0x1b, 0xbc, 0x30,

    /* U+0056 "V" */
    0xb5, 0x2, 0xe0, 0x5a, 0x8, 0x70, 0xd, 0xd,
    0x10, 0x8, 0x9a, 0x0, 0x1, 0xf4, 0x0,

    /* U+0057 "W" */
    0xa4, 0xf, 0x13, 0xc6, 0x84, 0xd5, 0x67, 0x2c,
    0x95, 0xaa, 0x30, 0xdc, 0xb, 0xc0, 0xa, 0x90,
    0x8b, 0x0,

    /* U+0058 "X" */
    0x7a, 0x9, 0x80, 0xc7, 0xc0, 0x4, 0xf5, 0x0,
    0xc8, 0xd0, 0x7a, 0x9, 0x80,

    /* U+0059 "Y" */
    0xa6, 0x8, 0x81, 0xd1, 0xd0, 0x7, 0xd5, 0x0,
    0x1f, 0x0, 0x1, 0xf0, 0x0,

    /* U+005A "Z" */
    0x8c, 0xcf, 0x60, 0x5, 0xb0, 0x3, 0xd1, 0x1,
    0xd2, 0x0, 0x9e, 0xbb, 0x60,

    /* U+005B "[" */
    0x6c, 0x17, 0x70, 0x77, 0x7, 0x70, 0x77, 0x7,
    0x70, 0x77, 0x6, 0xc1,

    /* U+005C "\\" */
    0xb3, 0x0, 0x4a, 0x0, 0xd, 0x10, 0x6, 0x80,
    0x0, 0xd0, 0x0, 0x52,

    /* U+005D "]" */
    0xb8, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0xb8,

    /* U+005E "^" */
    0x5, 0x10, 0x1c, 0x80, 0x93, 0xb0,

    /* U+005F "_" */
    0xab, 0xb6,

    /* U+0060 "`" */
    0x4a, 0x0,

    /* U+0061 "a" */
    0x3b, 0xb6, 0x19, 0x9d, 0x96, 0x2d, 0x4d, 0xbe,

    /* U+0062 "b" */
    0x87, 0x0, 0x8, 0xcc, 0x90, 0x87, 0xd, 0x28,
    0x70, 0xd2, 0x8c, 0xc9, 0x0,

    /* U+0063 "c" */
    0x2c, 0xc7, 0x95, 0x6, 0x95, 0x5, 0x2c, 0xb6,

    /* U+0064 "d" */
    0x0, 0xf, 0x2, 0xdb, 0xf0, 0x95, 0xf, 0x9,
    0x50, 0xf0, 0x2c, 0xaf, 0x0,

    /* U+0065 "e" */
    0x1b, 0xb8, 0x8, 0xca, 0xc0, 0x97, 0x2, 0x2,
    0xcb, 0x90,

    /* U+0066 "f" */
    0x1d, 0x99, 0xe6, 0x2c, 0x2, 0xc0, 0x2c, 0x0,

    /* U+0067 "g" */
    0x2d, 0xbe, 0x9, 0x50, 0xe0, 0x96, 0xe, 0x2,
    0xca, 0xf0, 0x2a, 0xb8, 0x0,

    /* U+0068 "h" */
    0x86, 0x0, 0x8c, 0xc9, 0x87, 0xf, 0x86, 0xf,
    0x86, 0xf,

    /* U+0069 "i" */
    0x56, 0x78, 0x78, 0x78, 0x78,

    /* U+006A "j" */
    0x6, 0x50, 0x77, 0x7, 0x70, 0x77, 0x7, 0x73,
    0xd4,

    /* U+006B "k" */
    0x87, 0x0, 0x8, 0x78, 0x90, 0x8d, 0xb0, 0x8,
    0xbd, 0x20, 0x87, 0x4c, 0x0,

    /* U+006C "l" */
    0x78, 0x78, 0x78, 0x78, 0x78,

    /* U+006D "m" */
    0x8c, 0xca, 0xad, 0x38, 0x70, 0xf0, 0x87, 0x87,
    0xf, 0x7, 0x78, 0x70, 0xf0, 0x77,

    /* U+006E "n" */
    0x8c, 0xca, 0x87, 0xf, 0x86, 0xf, 0x86, 0xf,

    /* U+006F "o" */
    0x2b, 0xc8, 0x9, 0x50, 0xc2, 0x95, 0xc, 0x22,
    0xbb, 0x80,

    /* U+0070 "p" */
    0x8c, 0xb9, 0x8, 0x70, 0xd1, 0x87, 0xd, 0x18,
    0xcc, 0x90, 0x87, 0x0, 0x0,

    /* U+0071 "q" */
    0x2d, 0xaf, 0x9, 0x50, 0xf0, 0x96, 0xf, 0x2,
    0xca, 0xf0, 0x0, 0xf, 0x0,

    /* U+0072 "r" */
    0x0, 0x8, 0xd8, 0x87, 0x8, 0x70, 0x87, 0x0,

    /* U+0073 "s" */
    0x3c, 0xc6, 0x6b, 0x43, 0x34, 0x8a, 0x4c, 0xc6,

    /* U+0074 "t" */
    0x4a, 0xb, 0xe4, 0x4a, 0x4, 0xa0, 0x1d, 0x50,

    /* U+0075 "u" */
    0x86, 0xf, 0x86, 0xf, 0x87, 0xf, 0x3d, 0xaf,

    /* U+0076 "v" */
    0xb3, 0x4a, 0x58, 0x94, 0xd, 0xd0, 0x9, 0x90,

    /* U+0077 "w" */
    0xb2, 0x87, 0x4a, 0x76, 0xbc, 0x85, 0x2d, 0x8a,
    0xd1, 0xd, 0x34, 0xc0,

    /* U+0078 "x" */
    0x88, 0x88, 0xc, 0xd0, 0xd, 0xd0, 0x87, 0x79,

    /* U+0079 "y" */
    0xb3, 0x5a, 0x59, 0xa3, 0xc, 0xc0, 0x9, 0x60,
    0x7b, 0x0,

    /* U+007A "z" */
    0x7b, 0xe9, 0x3, 0xd1, 0x1d, 0x20, 0x9e, 0xb8,

    /* U+007B "{" */
    0x0, 0x0, 0x85, 0xe, 0x1, 0xd0, 0xb7, 0x1,
    0xd0, 0xe, 0x0, 0x85, 0x0, 0x0,

    /* U+007C "|" */
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x11,

    /* U+007D "}" */
    0x0, 0x9, 0x40, 0x3a, 0x2, 0xc0, 0xc, 0x52,
    0xc0, 0x3a, 0x9, 0x40, 0x0, 0x0,

    /* U+007E "~" */
    0x1b, 0x80, 0xa6, 0x45, 0xc5,

    /* U+00B0 "°" */
    0x38, 0x33, 0x83,

    /* U+2022 "•" */
    0x29, 0x14, 0xe2,

    /* U+E1BA "" */
    0x6, 0xcf, 0xfc, 0x60, 0x9f, 0xff, 0xff, 0xf9,
    0x1d, 0xff, 0xff, 0xd1, 0x3, 0xff, 0xff, 0x20,
    0x0, 0x5f, 0xf5, 0x0, 0x0, 0x8, 0x80, 0x0,
    0x0, 0x0, 0x0, 0x0,

    /* U+E1DA "" */
    0x12, 0x0, 0x0, 0x0, 0xd, 0x4d, 0xfc, 0x60,
    0x9f, 0xe5, 0xef, 0xf9, 0x1d, 0xfe, 0x5e, 0xd1,
    0x2, 0xff, 0xe5, 0x20, 0x0, 0x5f, 0xfb, 0x20,
    0x0, 0x8, 0x80, 0x50, 0x0, 0x0, 0x0, 0x0,

    /* U+E328 "" */
    0x0, 0x0, 0x10, 0x0, 0x0, 0x8a, 0x70, 0x0,
    0x2, 0x2, 0x0, 0x0, 0xa, 0x0, 0x8a, 0xaa,
    0xe9, 0xf, 0x88, 0xae, 0xf2, 0xdf, 0xff, 0xff,
    0x10
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 32, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 34, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 42, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 8, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 21, .adv_w = 73, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 41, .adv_w = 94, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 82, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 71, .adv_w = 22, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 73, .adv_w = 45, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 85, .adv_w = 45, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 97, .adv_w = 57, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 105, .adv_w = 71, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 115, .adv_w = 28, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 118, .adv_w = 42, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 120, .adv_w = 36, .box_w = 2, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 51, .box_w = 3, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 130, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 73, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 164, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 177, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 190, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 203, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 216, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 242, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 255, .adv_w = 34, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 259, .adv_w = 30, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 264, .adv_w = 65, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 272, .adv_w = 72, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 278, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 286, .adv_w = 62, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 296, .adv_w = 115, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 317, .adv_w = 85, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 332, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 345, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 358, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 371, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 384, .adv_w = 70, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 397, .adv_w = 87, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 410, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 425, .adv_w = 36, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 430, .adv_w = 71, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 440, .adv_w = 81, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 455, .adv_w = 69, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 468, .adv_w = 112, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 486, .adv_w = 91, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 501, .adv_w = 88, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 516, .adv_w = 82, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 529, .adv_w = 88, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 547, .adv_w = 80, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 560, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 573, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 586, .adv_w = 83, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 599, .adv_w = 83, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 614, .adv_w = 113, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 632, .adv_w = 81, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 645, .adv_w = 78, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 658, .adv_w = 77, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 671, .adv_w = 35, .box_w = 3, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 683, .adv_w = 54, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 695, .adv_w = 35, .box_w = 2, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 703, .adv_w = 55, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 709, .adv_w = 58, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 711, .adv_w = 41, .box_w = 3, .box_h = 1, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 713, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 721, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 734, .adv_w = 67, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 742, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 755, .adv_w = 69, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 765, .adv_w = 45, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 773, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 786, .adv_w = 71, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 796, .adv_w = 33, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 801, .adv_w = 32, .box_w = 3, .box_h = 6, .ofs_x = -1, .ofs_y = -1},
    {.bitmap_index = 810, .adv_w = 67, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 823, .adv_w = 33, .box_w = 2, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 828, .adv_w = 111, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 842, .adv_w = 71, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 850, .adv_w = 73, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 860, .adv_w = 72, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 873, .adv_w = 73, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 886, .adv_w = 45, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 894, .adv_w = 66, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 902, .adv_w = 43, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 910, .adv_w = 71, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 918, .adv_w = 63, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 926, .adv_w = 95, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 938, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 946, .adv_w = 62, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 956, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 964, .adv_w = 43, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 978, .adv_w = 32, .box_w = 2, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 985, .adv_w = 43, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 999, .adv_w = 85, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 1004, .adv_w = 49, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1007, .adv_w = 45, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 1010, .adv_w = 128, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1038, .adv_w = 128, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1070, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x1f72, 0xe10a, 0xe12a, 0xe278
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 176, .range_length = 57977, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 5, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 1, 0, 2, 0, 0, 0, 0,
    2, 3, 0, 0, 0, 4, 0, 4,
    5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 6, 7, 8, 9, 10, 11,
    0, 12, 12, 13, 14, 15, 12, 12,
    9, 16, 17, 18, 0, 19, 13, 20,
    21, 22, 23, 24, 25, 0, 0, 0,
    0, 0, 26, 27, 28, 0, 29, 30,
    0, 31, 0, 0, 32, 0, 31, 31,
    33, 27, 0, 34, 0, 35, 0, 36,
    37, 38, 36, 39, 40, 0, 0, 0,
    0, 0, 0, 0, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 1, 0, 2, 0, 0, 0, 3,
    2, 0, 4, 5, 0, 6, 7, 6,
    8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    9, 0, 10, 0, 11, 0, 0, 0,
    11, 0, 0, 12, 0, 0, 0, 0,
    11, 0, 11, 0, 13, 14, 15, 16,
    17, 18, 19, 20, 0, 0, 21, 0,
    0, 0, 22, 0, 23, 23, 23, 24,
    23, 0, 0, 0, 0, 0, 25, 25,
    26, 25, 23, 27, 28, 29, 30, 31,
    32, 33, 31, 34, 0, 0, 35, 0,
    0, 36, 0, 0, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, -4, 0, -1, -6, 0, -6,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1,
    2, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -18, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -15,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, 0, 0, 0, -5, 0, -1, 0,
    0, -7, -1, -5, -2, 0, -9, 0,
    0, 0, 0, 0, -3, -1, 0, 0,
    -1, -1, -3, -2, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, -1,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, -8, 0, 0,
    0, -1, 0, 0, 0, -5, 0, -1,
    0, -1, -3, -1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0,
    0, 0, -1, -1, 0, -1, 0, 0,
    0, -1, -2, -1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -17, 0, 0,
    0, -12, 0, -13, 0, 1, 0, 0,
    0, 0, 0, 0, 0, -2, -1, 0,
    0, -1, -2, 0, 0, -1, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0,
    0, -2, 0, 0, 0, 1, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -10, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, -2, 0, 0,
    0, -1, -2, -4, 0, 0, 0, 0,
    0, -18, 0, 0, 0, 0, 0, 0,
    0, 1, -4, 0, 0, -13, -1, -13,
    -6, 0, -17, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, -8, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -25, 0, 0, 0, -11, 0, -11,
    0, 0, 0, 0, 0, -3, 0, -2,
    0, -1, -1, 0, 0, -1, 0, 0,
    1, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, -2,
    -1, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -1, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, -16, -17, 0,
    0, -7, -2, -13, -1, 1, 0, 1,
    1, 0, 1, 0, 0, -10, -6, 0,
    -6, -13, -4, -5, 0, -4, -5, -3,
    -5, -4, 0, 0, 0, 0, 0, 1,
    0, -13, -10, 0, 0, -5, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, -3, -3, 0, 0, -3, -2, 0,
    0, -2, -1, 0, 0, 0, 1, 0,
    0, 0, 0, 1, 0, -9, -4, 0,
    0, -3, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, -2, -2, 0,
    0, -2, -1, 0, 0, -1, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, -10, 0, 0, 0, -2, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, -2, 0, 0, -1, 0, 0,
    0, -1, -2, 0, 0, 0, 0, 0,
    0, 0, -2, 1, -3, -14, -9, 0,
    0, -9, -2, -6, -1, 1, -6, 1,
    1, 1, 1, 0, 1, -4, -4, -1,
    -2, -4, -2, -4, -1, -2, -1, 0,
    -1, -2, 1, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 0, 0, -1, 0, 0,
    0, -1, -2, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -1, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    -1, -1, 0, 0, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, 0, 0, 0, 0, 1, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    -1, -1, 0, 0, 0, 1, 0, 0,
    0, -11, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -1, 1, 0, -2, 0, 0,
    3, 0, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0,
    0, -10, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, -1, 1, 0, -1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -8, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -1, 0,
    0, -1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -1,
    0, 0, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 40,
    .right_class_cnt     = 36,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 4,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t main_font_8 = {
#else
lv_font_t main_font_8 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 10,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = -1,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if MAIN_FONT_8*/

