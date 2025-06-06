#define _CRT_SECURE_NO_WARNINGS
#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#ifdef _WIN32
#include "Windows.h"
#else
#include <unistd.h>
#endif
#include <time.h>
#include <SDL.h>
#include <stdbool.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

// Classic VGA 8x16 Font (256 characters, 16 bytes each)
// Each byte represents a row, MSB is the leftmost pixel.
static const unsigned char vga_font_8x16[256][16] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //   0 Null
    {0x7E, 0xFF, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0xFF, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //   1 White Smiley Face
    {0x7E, 0xFF, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0xFF, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, //   2 Black Smiley Face
    {0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x10, 0x10, 0x38, 0x7C, 0xFE, 0xFE, 0xFE, 0x6C, 0x00}, //   3 Black Heart Suit
    {0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x10, 0x10, 0x38, 0x7C, 0xFE, 0xFE, 0xFE, 0x6C, 0xFF}, //   4 Black Diamond Suit
    {0x38, 0x7C, 0x7C, 0x38, 0xFC, 0xFE, 0x7E, 0x10, 0x10, 0x7E, 0xFE, 0xFC, 0x38, 0x7C, 0x7C, 0x38}, //   5 Black Club Suit
    {0x10, 0x38, 0x7C, 0xFE, 0x10, 0x10, 0x10, 0xFE, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x00, 0x00}, //   6 Black Spade Suit
    {0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //   7 Bullet
    {0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF, 0xFF}, //   8 Inverse Bullet
    {0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //   9 White Circle
    {0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, //  10 Black Circle
    {0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  11 Male Sign
    {0x38, 0x7C, 0x38, 0x10, 0x7C, 0xFE, 0x7C, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  12 Female Sign
    {0x3C, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x00, 0x7E, 0x7E, 0x00, 0x18, 0x18, 0x18, 0x18}, //  13 Eighth Note
    {0x3C, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x18, 0x00, 0x7E, 0x7E, 0x7E, 0x60, 0x60, 0x60, 0x60}, //  14 Beamed Eighth Notes
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, //  15 White Sun with Rays (*) Actually solid block fill
    {0x00, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  16 Black Right Pointing Pointer
    {0x00, 0x08, 0x1C, 0x3E, 0x7F, 0x3E, 0x1C, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  17 Black Left Pointing Pointer
    {0x18, 0x3C, 0x7E, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, //  18 Up Down Arrow
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00}, //  19 Double Exclamation Mark !!
    {0x00, 0x3C, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0xD8, 0xD8, 0xC0, 0xC0, 0xD8, 0x7E, 0x18, 0x00, 0x00}, //  20 Paragraph Sign
    {0x00, 0x00, 0xFC, 0xFE, 0x06, 0x06, 0x06, 0xFE, 0xFC, 0x06, 0x06, 0x06, 0xFE, 0xFC, 0x00, 0x00}, //  21 Section Sign
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x18, 0x3C, 0x7E, 0xFF, 0x00}, //  22 Lower Half Block Rectangle
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x3C, 0x7E, 0xFF}, //  23 Up Down Arrow With Base
    {0x18, 0x3C, 0x7E, 0xFF, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  24 Upwards Arrow
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00}, //  25 Downwards Arrow
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00}, //  26 Rightwards Arrow
    {0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  27 Leftwards Arrow
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF}, //  28 Right Angle (*) Actually Lower Half Inverse Block
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, //  29 Left Right Arrow
    {0x00, 0x00, 0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00}, //  30 Black Up Pointing Triangle
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18, 0x3C, 0x7E, 0xFF, 0x00}, //  31 Black Down Pointing Triangle
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  32 Space
    {0x30, 0x78, 0x78, 0x30, 0x30, 0x30, 0x30, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  33 !
    {0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  34 "
    {0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  35 #
    {0x18, 0x3C, 0x60, 0x38, 0x0C, 0x78, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  36 $
    {0x00, 0xC6, 0xC6, 0x0C, 0x18, 0x30, 0x60, 0x60, 0xC6, 0x8C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, //  37 %
    {0x38, 0x6C, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  38 &
    {0x60, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  39 '
    {0x0C, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  40 (
    {0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  41 )
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  42 *
    {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  43 +
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00}, //  44 ,
    {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  45 -
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  46 .
    {0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  47 /
    {0x3C, 0x66, 0xC6, 0xCE, 0xDE, 0xEE, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  48 0
    {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  49 1
    {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  50 2
    {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x06, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  51 3
    {0x0C, 0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  52 4
    {0xFE, 0x60, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  53 5
    {0x3C, 0x66, 0xC0, 0xFC, 0xC6, 0xC6, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  54 6
    {0xFE, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  55 7
    {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  56 8
    {0x3C, 0x66, 0xC6, 0xC6, 0x7E, 0x06, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  57 9
    {0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  58 :
    {0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  59 ;
    {0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  60 <
    {0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  61 =
    {0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  62 >
    {0x3C, 0x66, 0x0C, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  63 ?
    {0x3C, 0x66, 0xC6, 0xC0, 0xDE, 0xCE, 0xC6, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  64 @
    {0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  65 A
    {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x66, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  66 B
    {0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  67 C
    {0x78, 0x6C, 0x66, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  68 D
    {0x7E, 0xC0, 0xC0, 0xFC, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  69 E
    {0xFE, 0xC0, 0xC0, 0xFC, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  70 F
    {0x3C, 0x66, 0xC0, 0xC0, 0xCE, 0xC6, 0x66, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  71 G
    {0x66, 0x66, 0x66, 0xFE, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  72 H
    {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  73 I
    {0x0E, 0x06, 0x06, 0x06, 0x06, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  74 J
    {0xC6, 0xC6, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  75 K
    {0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  76 L
    {0xC6, 0xEE, 0xFE, 0xD6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  77 M
    {0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  78 N
    {0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  79 O
    {0xFC, 0x66, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  80 P
    {0x7C, 0xC6, 0xC6, 0xC6, 0xD6, 0xDE, 0x7C, 0x0E, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  81 Q
    {0xFC, 0x66, 0x66, 0x6C, 0x7C, 0x6C, 0x66, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  82 R
    {0x3C, 0x66, 0x60, 0x38, 0x0C, 0x06, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  83 S
    {0xFE, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  84 T
    {0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  85 U
    {0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  86 V
    {0xC6, 0xC6, 0xC6, 0xD6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  87 W
    {0xC6, 0x6C, 0x38, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  88 X
    {0xC6, 0xC6, 0x6C, 0x38, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  89 Y
    {0xFE, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  90 Z
    {0x7E, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  91 [
    {0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  92 \ Backslash
    {0x7E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  93 ]
    {0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  94 ^ Caret
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00}, //  95 _ Underscore
    {0x30, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  96 ` Grave Accent
    {0x00, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  97 a
    {0xC0, 0xC0, 0xC0, 0xFC, 0xC6, 0xC6, 0xC6, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  98 b
    {0x00, 0x00, 0x7C, 0xC6, 0xC0, 0xC0, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //  99 c
    {0x06, 0x06, 0x06, 0x7E, 0xC6, 0xC6, 0xC6, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 100 d
    {0x00, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 101 e
    {0x1E, 0x30, 0x30, 0x7C, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 102 f
    {0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0xCC, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 103 g
    {0xC0, 0xC0, 0xC0, 0xFC, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 104 h
    {0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 105 i
    {0x0C, 0x00, 0x0C, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 106 j
    {0xC0, 0xC0, 0xC0, 0xCC, 0xD8, 0xF0, 0xD8, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 107 k
    {0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 108 l
    {0x00, 0x00, 0xEC, 0xFE, 0xD6, 0xD6, 0xD6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 109 m
    {0x00, 0x00, 0xDC, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 110 n
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 111 o
    {0x00, 0x00, 0xDC, 0xC6, 0xC6, 0xDC, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 112 p
    {0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0x0C, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 113 q
    {0x00, 0x00, 0xDC, 0x6C, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 114 r
    {0x00, 0x00, 0x7C, 0xC0, 0x78, 0x0C, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 115 s
    {0x10, 0x10, 0x7C, 0x10, 0x10, 0x10, 0x16, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 116 t
    {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 117 u
    {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 118 v
    {0x00, 0x00, 0xC6, 0xC6, 0xD6, 0xD6, 0xFE, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 119 w
    {0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 120 x
    {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0x7E, 0x06, 0x0C, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 121 y
    {0x00, 0x00, 0xFE, 0x18, 0x30, 0x60, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 122 z
    {0x0E, 0x18, 0x18, 0x70, 0x18, 0x18, 0x18, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 123 {
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 124 | Vertical Bar
    {0x70, 0x18, 0x18, 0x0E, 0x18, 0x18, 0x18, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 125 }
    {0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 126 ~ Tilde
    {0x3C, 0x66, 0x7E, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 127 House / Delete
    {0x00, 0x00, 0x7C, 0xC6, 0xC0, 0xC0, 0xC6, 0x7C, 0x00, 0x0E, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00}, // 128 Ç C cedilla
    {0x6C, 0x6C, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 129 ü u umlaut
    {0x18, 0x30, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 130 é e acute
    {0x6C, 0x6C, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 131 â a circumflex
    {0x6C, 0x6C, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 132 ä a umlaut
    {0x18, 0x30, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 133 à a grave
    {0x18, 0x3C, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 134 å a ring
    {0x18, 0x3C, 0x18, 0x7C, 0xC6, 0xC0, 0xC0, 0xC6, 0x7C, 0x00, 0x0E, 0x0C, 0x18, 0x00, 0x00, 0x00}, // 135 ç c cedilla
    {0x10, 0x38, 0x6C, 0xC6, 0xC0, 0xC0, 0xC0, 0xFC, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00}, // 136 ê e circumflex
    {0x6C, 0x6C, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 137 ë e umlaut
    {0x18, 0x30, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 138 è e grave
    {0x6C, 0x6C, 0x00, 0x70, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 139 ï i umlaut
    {0x10, 0x38, 0x6C, 0xC6, 0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00, 0x00}, // 140 î i circumflex
    {0x18, 0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 141 ì i grave
    {0x6C, 0x6C, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 142 Ä A umlaut
    {0x18, 0x3C, 0x00, 0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00}, // 143 Å A ring
    {0x18, 0x30, 0x00, 0x7E, 0xC0, 0xC0, 0xFC, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00}, // 144 É E acute
    {0x00, 0x00, 0x78, 0xCC, 0xC0, 0xC0, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 145 æ ae ligature
    {0x00, 0x00, 0xFE, 0x6C, 0x6C, 0x7C, 0x60, 0x60, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 146 Æ AE ligature
    {0x10, 0x38, 0x6C, 0xC6, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 147 ô o circumflex
    {0x6C, 0x6C, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 148 ö o umlaut
    {0x18, 0x30, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 149 ò o grave
    {0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 150 û u circumflex
    {0x18, 0x30, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 151 ù u grave
    {0x6C, 0x6C, 0x00, 0xC6, 0xC6, 0xC6, 0x7E, 0x06, 0x0C, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 152 ÿ y umlaut
    {0x6C, 0x6C, 0x00, 0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00}, // 153 Ö O umlaut
    {0x6C, 0x6C, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 154 Ü U umlaut
    {0x18, 0x3C, 0x60, 0xE0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 155 ø o slash (*) Cent sign
    {0x78, 0xCC, 0xC0, 0xFC, 0x0C, 0x0C, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 156 £ Pound sign
    {0x00, 0xC6, 0xC6, 0xC6, 0xFE, 0x0C, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 157 Ø O slash (*) Yen sign
    {0x00, 0x7C, 0xC6, 0xC0, 0xC0, 0xC0, 0x7C, 0x06, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 158 Pt Peseta sign
    {0x30, 0x78, 0xCC, 0xCC, 0x78, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 159 ƒ Florin sign
    {0x18, 0x30, 0x00, 0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00}, // 160 á a acute
    {0x18, 0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 161 í i acute
    {0x18, 0x30, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 162 ó o acute
    {0x18, 0x30, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 163 ú u acute
    {0x00, 0x00, 0xDC, 0xC6, 0xC6, 0xD6, 0xF6, 0xE6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 164 ñ n tilde
    {0x00, 0x00, 0xDC, 0xC6, 0xC6, 0xD6, 0xF6, 0xE6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 165 Ñ N tilde
    {0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 166 ª Feminine Ordinal Indicator
    {0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 167 º Masculine Ordinal Indicator
    {0x3C, 0x66, 0x0C, 0x18, 0x18, 0x00, 0x18, 0x18, 0x0C, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00}, // 168 ¿ Inverted question mark
    {0x00, 0x00, 0xFE, 0x0C, 0x0C, 0x0C, 0x0C, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 169 ® Registered Trade Mark Sign (*) Reversed Not Sign
    {0x00, 0x00, 0xFE, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 170 ¬ Not sign
    {0x00, 0xFE, 0xC6, 0x8C, 0x18, 0x32, 0x66, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 171 ½ One half
    {0x00, 0xFE, 0xC6, 0x8E, 0x1E, 0x3E, 0x7E, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 172 ¼ One quarter
    {0x30, 0x78, 0x78, 0x30, 0x30, 0x00, 0x30, 0x30, 0x78, 0x78, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00}, // 173 ¡ Inverted Exclamation Mark
    {0x0C, 0x18, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60, 0xC0, 0xC0, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00}, // 174 « Left Guillemet
    {0x30, 0x18, 0x18, 0x18, 0x0C, 0x0C, 0x06, 0x06, 0x03, 0x03, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00}, // 175 » Right Guillemet
    {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF}, // 176 Light Shade (25%)
    {0xDB, 0x6D, 0xDB, 0x6D, 0xDB, 0x6D, 0xDB, 0x6D, 0xDB, 0x6D, 0xDB, 0x6D, 0xDB, 0x6D, 0xDB, 0x6D}, // 177 Medium Shade (50%)
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 178 Dark Shade (75%) (*) Actually Full Block
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 179 │ Box Drawings Light Vertical
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 180 ┤ Box Drawings Light Vertical and Left
    {0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x7E, 0x3C, 0x18}, // 181 Á A acute
    {0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x6C, 0x38, 0x10, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00}, // 182 Â A circumflex
    {0x18, 0x30, 0x60, 0xC0, 0xC0, 0xFE, 0x6C, 0x38, 0x10, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00}, // 183 À A grave
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 184 © Copyright Sign (*) Top Half Integral
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 185 ╣ Box Drawings Double Vertical and Left
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // 186 ║ Box Drawings Double Vertical
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}, // 187 ╗ Box Drawings Double Down and Left
    {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 188 ╝ Box Drawings Double Up and Left
    {0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 189 ¢ Cent Sign
    {0x7E, 0xFF, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xFF, 0x7E, 0x00, 0x00, 0x00}, // 190 ¥ Yen Sign
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60}, // 191 ┐ Box Drawings Light Down and Left
    {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 192 └ Box Drawings Light Up and Right
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 193 ┴ Box Drawings Light Up and Horizontal
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 194 ┬ Box Drawings Light Down and Horizontal
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60}, // 195 ├ Box Drawings Light Vertical and Right
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 196 ─ Box Drawings Light Horizontal
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 197 ┼ Box Drawings Light Vertical and Horizontal
    {0x6C, 0x6C, 0xFE, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 198 ã a tilde
    {0x6C, 0x6C, 0xFE, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 199 Ã A tilde
    {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0xFF, 0xFF, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}, // 200 ╚ Box Drawings Double Up and Right
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}, // 201 ╔ Box Drawings Double Down and Right
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // 202 ╩ Box Drawings Double Up and Horizontal
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 203 ╦ Box Drawings Double Down and Horizontal
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // 204 ╠ Box Drawings Double Vertical and Right
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // 205 ═ Box Drawings Double Horizontal
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // 206 ╬ Box Drawings Double Vertical and Horizontal
    {0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 207 õ o tilde
    {0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 208 Õ O tilde
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0xC6, 0xC6, 0x60, 0x30, 0x60, 0xC6, 0x7E, 0x00}, // 209 Ð Icelandic eth (lowercase)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x7E, 0x00}, // 210 Ð Icelandic eth (uppercase)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xC6, 0xC6, 0xDC, 0xC0, 0xC0, 0xC0, 0xFE, 0x00}, // 211 Þ Icelandic thorn (lowercase)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0xC0, 0x00}, // 212 Þ Icelandic thorn (uppercase)
    {0x6C, 0x6C, 0x00, 0xC6, 0xC6, 0x6C, 0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00}, // 213 Ý Y acute
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 214 Ö O Umlaut (*) Full Block
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 215 µ Micro Sign (*) Upper Half Block
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x7C, 0x7C, 0x38, 0xFC, 0xFE, 0x7E, 0x10, 0x10}, // 216 ¶ Pilcrow Sign
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 217 ┘ Box Drawings Light Up and Left
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 218 ┌ Box Drawings Light Down and Right
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 219 █ Full Block
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 220 ▄ Lower Half Block
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF}, // 221 ▌ Left Half Block
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00}, // 222 ▐ Right Half Block
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF}, // 223 ▀ Upper Half Block
    {0x38, 0x6C, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 224 α Greek Alpha
    {0x00, 0x00, 0xFC, 0xC6, 0xC6, 0xFC, 0xC6, 0xC6, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 225 ß German Sharp s
    {0x00, 0x00, 0x00, 0xFF, 0x38, 0x6C, 0x6C, 0x6C, 0x38, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 226 Γ Greek Gamma
    {0x00, 0x00, 0x18, 0x18, 0xFE, 0x18, 0x18, 0x18, 0xFE, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // 227 π Greek Pi
    {0x00, 0x00, 0xFE, 0xC6, 0x8C, 0x18, 0x32, 0x66, 0xC6, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 228 Σ Greek Sigma
    {0x00, 0x00, 0x7E, 0xC6, 0xC0, 0xC0, 0xC0, 0xFE, 0x06, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00}, // 229 σ Greek sigma
    {0x00, 0x00, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00}, // 230 µ Micro Sign
    {0x00, 0x00, 0xFE, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // 231 τ Greek tau
    {0x00, 0x00, 0xFE, 0xC6, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0xC6, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00}, // 232 Φ Greek Phi
    {0x00, 0x00, 0x7E, 0x18, 0x18, 0x7E, 0xC6, 0xC6, 0xC6, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 233 Θ Greek Theta
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x7C, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00}, // 234 Ω Greek Omega
    {0x00, 0x00, 0x7C, 0xC6, 0xC6, 0x7C, 0x18, 0x30, 0x60, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 235 δ Greek delta
    {0x00, 0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00}, // 236 ∞ Infinity
    {0x00, 0x00, 0xC6, 0xEE, 0xFE, 0xFE, 0xFE, 0xDE, 0xCE, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00}, // 237 φ Greek phi
    {0x00, 0x00, 0x7C, 0xC6, 0xFE, 0xFE, 0xC0, 0xC0, 0xC6, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 238 ε Greek epsilon
    {0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 239 ∩ Intersection
    {0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0xFE, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 240 ≡ Identical To
    {0x00, 0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 241 ± Plus-Minus Sign
    {0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 242 ≥ Greater-Than or Equal To
    {0x00, 0x00, 0x06, 0x06, 0x06, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 243 ≤ Less-Than or Equal To
    {0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 244 ⌠ Top Half Integral
    {0x7E, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 245 ⌡ Bottom Half Integral
    {0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00}, // 246 ÷ Division Sign
    {0x00, 0x00, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0xC6, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 247 ≈ Almost Equal To
    {0x00, 0x18, 0x3C, 0x00, 0x3C, 0x18, 0x3C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 248 ° Degree Sign
    {0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 249 ∙ Bullet Operator
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 250 · Middle Dot
    {0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x60, 0xFE, 0x60, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // 251 √ Square Root
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0xC0, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00, 0x00, 0x00}, // 252 ⁿ Superscript n
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x06, 0x06, 0x06, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00}, // 253 ² Superscript Two
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 254 ■ Black Square
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 255 Non-breaking space (same as space)
};

#define CPU_VERSION 5
#define MEMORY_SIZE (65536 * 1024)
#define NUM_REGISTERS 32
#define NUM_F_REGISTERS 16
#define MAX_PROGRAM_SIZE 65536
#define MAX_LINE_LENGTH 512
#define DEFAULT_SCREEN_WIDTH 256
#define DEFAULT_SCREEN_HEIGHT 192
#define MAX_SCREEN_DIM 1024
#define PALETTE_SIZE 16
#define MAX_DEFINES 4096

typedef enum {
    MOV, ADD, SUB, MUL, DIV, INTR, NOP, HLT, NOT, AND, OR, XOR, SHL, SHR, JMP,
    CMP, JMPNE, JMPH, JMPL, NEG, INC, DEC, XCHG, CLR, PUSH, POP, CALL, RET, ROL,
    ROR, STRMOV, RND, JMPE, MOD, POW, ABS, LOOP, LOAD, STORE, TEST,
    LEA, PUSHF, POPF, SETF, CLRF, BT, BSET, BCLR, BTOG,
    STRCMP, STRLEN, STRCPY, MEMCPY, MEMSET, CPUID, BSWAP, SAR, RVD,
    INC_MEM, DEC_MEM, JMPO, JMPNO, JMPHE, JMPLE,
    FMOV, FADD, FSUB, FMUL, FDIV, FCMP, FABS, FNEG, FSQRT, CVTIF, CVTFI,
    FLOAD, FSTORE, FINC, FDEC, FPUSH, FPOP, FCLR, FXCHG, FPOW,
    CALLH, CALLL, CALLE, CALLNE, CALLO, CALLNO, CALLHE, CALLLE,
    IF, ELSE, END, WHILE, BREAK, CONTINUE,
    INVALID_INST
} InstructionType;

typedef struct {
    char name[64];
    char value[64];
} DefineEntry;

#define MAX_PREPROC_DEPTH 64

typedef struct {
    bool condition_met;
    bool parent_active;
    bool seen_else;
} PreprocessorState;

typedef struct {
    char label_name[64];
    int line_number;
} LabelEntry;

#define FPU_EPSILON 1e-9
#define FLAG_ZERO   0x01
#define FLAG_GREATER 0x02
#define FLAG_LESS   0x04
#define FLAG_OVERFLOW 0x08

//Null
#define INT_NOP             0x00 // No operation

// Console Input/Output
#define INT_PRINT_REG0      0x01 // Print integer in R0
#define INT_PRINT_STRING    0x02 // Print string from memory address in R0 until null terminator
#define INT_PRINT_NEWLINE   0x03 // Print a newline character
#define INT_GET_CHAR        0x04 // Keyboard input, reads a character into R0
#define INT_GET_STRING      0x05 // Read string input into memory at R0, max length R1
#define INT_PRINT_HEX_REG0  0x06 // Print R0 as hexadecimal
#define INT_CLEAR_SCREEN_OS 0x07 // Clear console screen
#define INT_SET_CONSOLE_COLOR 0x08 // Set console text color (R0=FG, R1=BG [Win32 codes])
#define INT_RESET_CONSOLE_COLOR 0x09 // Reset console color to default
#define INT_GOTOXY          0x0A // Move console cursor (R0=Col, R1=Row [0-based])
#define INT_GETXY           0x0B // Get console cursor pos (R0=Col, R1=Row) [Win32 only reliable]
#define INT_GET_CONSOLE_SIZE 0x0C // Get console dimensions (R0=Width, R1=Height)
#define INT_SET_CONSOLE_TITLE 0x0D // Set console window title (R0=title_addr)
#define INT_PRINT_FREG0 0x0E // Print floating point in F0

// Graphics Output / Control
#define INT_DRAW_PIXEL      0x10 // Draw pixel at (R0, R1) with palette color index R2
#define INT_CLEAR_GFX_SCREEN 0x11 // Clear graphics screen with palette color index R0
#define INT_SCREEN_ON       0x12 // Enable automatic graphics screen updates
#define INT_SCREEN_OFF      0x13 // Disable automatic graphics screen updates
#define INT_SET_RESOLUTION  0x14 // Change screen resolution to R0 x R1
#define INT_GET_PIXEL       0x15 // Get pixel color index at (R0, R1) into R0 (-1 if invalid)
#define INT_DRAW_STRING_GFX 0x16 // Draw string (R0=X, R1=Y, R2=StrAddr, R3=ColorIdx)
#define INT_BLIT            0x17 // Blit from Mem (R0=DX, R1=DY, R2=SrcAddr, R3=SW, R4=SH)
#define INT_GET_SCREEN_SIZE 0x18 // Get current GFX screen dimensions (R0=Width, R1=Height)
#define INT_UPDATE_GFX_SCREEN 0x19 // Manually update the screen with current pixel buffer contents

// Audio Control
#define INT_SPEAKER_ON      0x20 // Turn the virtual speaker on
#define INT_SPEAKER_OFF     0x21 // Turn the virtual speaker off
#define INT_SET_FREQ        0x22 // Set speaker frequency from R0 (Hz)
#define INT_SET_VOLUME      0x23 // Set speaker volume from R0 (0-100)
#define INT_SPEAKER_SLEEP   0x24 // Make speaker silent for R0 milliseconds

// Time / Date / Sleep
#define INT_SLEEP_MS        0x30 // Sleep for milliseconds specified in R0
#define INT_GET_TIME        0x31 // Get current time into R0 (H), R1 (M), R2 (S)
#define INT_GET_TICKS       0x32 // Get SDL Ticks (ms since init) into R0
#define INT_GET_DATETIME    0x33 // Get Date/Time (R0=Y, R1=M, R2=D, R3=h, R4=m, R5=s)

// CPU / System / Memory Control
#define INT_RESET_CPU       0x40 // Full CPU reset (state, memory, registers, flags, ip)
#define INT_SYSTEM_SHUTDOWN 0x41 // Requests the emulator session to end
#define INT_GET_MEMORY_SIZE 0x42 // Get configured MEMORY_SIZE into R
#define INT_BREAKPOINT      0x43 // Trigger host debugger breakpoint (if attached and set)
#define INT_DUMP_REGISTERS  0x44 // Print all register values, IP, SP, Flags
#define INT_DUMP_MEMORY     0x45 // Dump memory from R0 for length R1

// Input Device State
#define INT_GET_KEY_STATE   0x50 // Get state of key (SDL_Scancode in R0). R0=1 if pressed, 0 if not.
#define INT_WAIT_FOR_KEY    0x51 // Waits for a key press, returns SDL_Scancode in R0.
#define INT_GET_MOUSE_STATE 0x52 // Get mouse state: R0=X, R1=Y, R2=ButtonMask (1=L, 2=M, 4=R)

// Disk I/O
#define INT_DISK_READ       0x60 // Read sectors (R0=Sector#, R1=MemAddr, R2=NumSectors) -> R0=Status
#define INT_DISK_WRITE      0x61 // Write sectors (R0=Sector#, R1=MemAddr, R2=NumSectors) -> R0=Status
#define INT_DISK_INFO       0x62 // Get disk info -> R0=TotalSectors, R1=SectorSize
#define INT_DISK_FORMAT     0x63 // Format (zero-fill) the entire disk -> R0=Status

// Extended Graphics Output / Control
#define INT_DRAW_LINE         0x70 // Draw line (R0=x1, R1=y1, R2=x2, R3=y2, R4=color_idx)
#define INT_DRAW_RECT_FILLED  0x71 // Draw filled rectangle (R0=x, R1=y, R2=w, R3=h, R4=color_idx)
#define INT_DRAW_RECT_HOLLOW  0x72 // Draw hollow rectangle (R0=x, R1=y, R2=w, R3=h, R4=color_idx)
#define INT_DRAW_CIRCLE_FILLED 0x73 // Draw filled circle (R0=center_x, R1=center_y, R2=radius, R3=color_idx)
#define INT_DRAW_CIRCLE_HOLLOW 0x74 // Draw hollow circle (R0=center_x, R1=center_y, R2=radius, R3=color_idx)
#define INT_DRAW_TRIANGLE_FILLED 0x76 // Draw filled triangle (R0=x1, R1=y1, R2=x2, R3=y2, R4=x3, R5=y3, R6=color_idx)
#define INT_DRAW_TRIANGLE_HOLLOW 0x77 // Draw hollow triangle (R0=x1, R1=y1, R2=x2, R3=y2, R4=x3, R5=y3, R6=color_idx)

#define DISK_IMAGE_FILENAME "disk.img"
#define DISK_IMAGE_SIZE_BYTES (16 * 1024 * 1024) // 16 MB
#define DISK_SECTOR_SIZE 512

typedef struct {
    int registers[NUM_REGISTERS];
    double f_registers[NUM_F_REGISTERS];
    int memory[MEMORY_SIZE];
    int ip;
    int flags;
    int sp;

    SDL_AudioSpec audioSpec;
    SDL_AudioDeviceID audioDevice;
    double frequency;
    double volume;
    int speaker_on;
    int sleep_duration;
    Uint32 sleep_start_time;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int screen_width;
    int screen_height;
    Uint32* pixels;
    SDL_Color palette[PALETTE_SIZE];
    int screen_on;

    bool shutdown_requested;

    FILE* disk_image_fp;
    long long disk_image_size;
    int disk_sector_size;

} VirtualCPU;


bool init_cpu(VirtualCPU* cpu);
void cleanup_cpu(VirtualCPU* cpu);
bool init_sdl(VirtualCPU* cpu);
void cleanup_sdl(VirtualCPU* cpu);

InstructionType parseInstruction(const char* instruction);
int loadProgram(const char* filename, char* program[], int max_size);

void execute(VirtualCPU* cpu, char* program[], int program_size, bool debug_mode);
void interrupt(VirtualCPU* cpu, int interrupt_id);
void updateScreen(VirtualCPU* cpu);

void mov(VirtualCPU* cpu, int reg1, int value);
void add(VirtualCPU* cpu, int reg1, int reg2);
void sub(VirtualCPU* cpu, int reg1, int reg2);
void mul(VirtualCPU* cpu, int reg1, int reg2);
void divi(VirtualCPU* cpu, int reg1, int reg2);
void nop(VirtualCPU* cpu);
void hlt(VirtualCPU* cpu, bool* running_flag);
void not_op(VirtualCPU* cpu, int reg1);
void and_op(VirtualCPU* cpu, int reg1, int reg2);
void or_op(VirtualCPU* cpu, int reg1, int reg2);
void xor_op(VirtualCPU* cpu, int reg1, int reg2);
void shl(VirtualCPU* cpu, int reg1, int count);
void shr(VirtualCPU* cpu, int reg1, int count);
void jmp(VirtualCPU* cpu, int line_number);
void cmp(VirtualCPU* cpu, int reg1, int reg2);
void neg(VirtualCPU* cpu, int reg1);
void inc(VirtualCPU* cpu, int reg1);
void dec(VirtualCPU* cpu, int reg1);
void xchg(VirtualCPU* cpu, int reg1, int reg2);
void clr(VirtualCPU* cpu, int reg1);
void push(VirtualCPU* cpu, int reg1);
void pop(VirtualCPU* cpu, int reg1);
void call(VirtualCPU* cpu, int line_number);
void ret(VirtualCPU* cpu);
void rol(VirtualCPU* cpu, int reg1, int count);
void ror(VirtualCPU* cpu, int reg1, int count);
void rnd(VirtualCPU* cpu, int reg1);
void mod_op(VirtualCPU* cpu, int reg1, int reg2);
void pow_op(VirtualCPU* cpu, int reg1, int reg2);
void abs_op(VirtualCPU* cpu, int reg1);
void loop_op(VirtualCPU* cpu, int counter_reg, int target_line);
void load_op(VirtualCPU* cpu, int dest_reg, int addr_src_reg);
void store_op(VirtualCPU* cpu, int val_src_reg, int addr_dest_reg);
void test_op(VirtualCPU* cpu, int reg1, int reg2);
void lea_op(VirtualCPU* cpu, int dest_reg, int base_reg, int offset);
void pushf_op(VirtualCPU* cpu);
void popf_op(VirtualCPU* cpu);
void loope_op(VirtualCPU* cpu, int counter_reg, int target_line);
void loopne_op(VirtualCPU* cpu, int counter_reg, int target_line);
void setf_op(VirtualCPU* cpu, int flag_mask);
void clrf_op(VirtualCPU* cpu, int flag_mask);
void bt_op(VirtualCPU* cpu, int reg1, int bit_index);
void bset_op(VirtualCPU* cpu, int reg1, int bit_index);
void bclr_op(VirtualCPU* cpu, int reg1, int bit_index);
void btog_op(VirtualCPU* cpu, int reg1, int bit_index);
void strcmp_op(VirtualCPU* cpu, int reg_addr1, int reg_addr2);
void strlen_op(VirtualCPU* cpu, int reg_dest, int reg_addr);
void strcpy_op(VirtualCPU* cpu, int reg_dest_addr, int reg_src_addr);
void memcpy_op(VirtualCPU* cpu, int dest_addr_reg, int src_addr_reg, int len_reg);
void memset_op(VirtualCPU* cpu, int dest_addr_reg, int val_reg, int len_reg);
void cpuid_op(VirtualCPU* cpu, int dest_reg);
void bswap_op(VirtualCPU* cpu, int reg);
void sar_op(VirtualCPU* cpu, int reg, int count);
void rvd_op(VirtualCPU* cpu, int reg);
void inc_mem_op(VirtualCPU* cpu, int addr_reg);
void dec_mem_op(VirtualCPU* cpu, int addr_reg);
void loopo_op(VirtualCPU* cpu, int counter_reg, int target_line);
void loopno_op(VirtualCPU* cpu, int counter_reg, int target_line);
void eli_op(VirtualCPU* cpu);
void dli_op(VirtualCPU* cpu);
void fmov_reg(VirtualCPU* cpu, int dest_freg, int src_freg);
void fmov_imm(VirtualCPU* cpu, int dest_freg, double value);
void fadd(VirtualCPU* cpu, int dest_freg, int src_freg);
void fsub(VirtualCPU* cpu, int dest_freg, int src_freg);
void fmul(VirtualCPU* cpu, int dest_freg, int src_freg);
void fdiv(VirtualCPU* cpu, int dest_freg, int src_freg);
void fcmp(VirtualCPU* cpu, int freg1, int freg2);
void fabs_op(VirtualCPU* cpu, int freg);
void fneg_op(VirtualCPU* cpu, int freg);
void fsqrt(VirtualCPU* cpu, int freg);
void cvtif(VirtualCPU* cpu, int dest_freg, int src_reg);
void cvtfi(VirtualCPU* cpu, int dest_reg, int src_freg);
void fload(VirtualCPU* cpu, int dest_freg, int addr_src_reg);
void fstore(VirtualCPU* cpu, int addr_dest_reg, int src_freg);
void finc(VirtualCPU* cpu, int freg);
void fdec(VirtualCPU* cpu, int freg);
void fpush(VirtualCPU* cpu, int freg);
void fpop(VirtualCPU* cpu, int freg);
void fclr(VirtualCPU* cpu, int freg);
void fxchg(VirtualCPU* cpu, int freg1, int freg2);
void fpow_op(VirtualCPU* cpu, int dest_freg, int src_freg);

void audioCallback(void* userdata, Uint8* stream, int len);
void draw_pixel_gfx(VirtualCPU* cpu, int x, int y, int color_index);
void draw_line_gfx(VirtualCPU* cpu);
void draw_rect_filled_gfx(VirtualCPU* cpu);
void draw_rect_hollow_gfx(VirtualCPU* cpu);
void draw_circle_filled_gfx(VirtualCPU* cpu);
void draw_circle_hollow_gfx(VirtualCPU* cpu);

bool isValidReg(int reg) {
    return reg >= 0 && reg < NUM_REGISTERS;
}
bool isValidFReg(int reg) {
    return reg >= 0 && reg < NUM_F_REGISTERS;
}
bool isValidMem(int addr) {
    return addr >= 0 && addr < MEMORY_SIZE;
}

char* strdup_portable(const char* s) {
    if (s == NULL) {
        return NULL;
    }
#ifdef _MSC_VER
    return _strdup(s);
#else
    size_t len = strlen(s) + 1;
    char* new_s = malloc(len);
    if (new_s == NULL) {
        return NULL;
    }
    memcpy(new_s, s, len);
    return new_s;
#endif
}

void draw_pixel_gfx(VirtualCPU* cpu, int x, int y, int color_index) {
    if (!cpu->pixels) return;

    if (x >= 0 && x < cpu->screen_width && y >= 0 && y < cpu->screen_height &&
        color_index >= 0 && color_index < PALETTE_SIZE)
    {
        SDL_Color palette_color = cpu->palette[color_index];
        Uint32 color = ((Uint32)palette_color.a << 24) |
            ((Uint32)palette_color.r << 16) |
            ((Uint32)palette_color.g << 8) |
            ((Uint32)palette_color.b);
        cpu->pixels[y * cpu->screen_width + x] = color;
    }
    else {

    }
}

void draw_line_gfx(VirtualCPU* cpu) {
    int x1 = cpu->registers[0];
    int y1 = cpu->registers[1];
    int x2 = cpu->registers[2];
    int y2 = cpu->registers[3];
    int color_index = cpu->registers[4];

    if (color_index < 0 || color_index >= PALETTE_SIZE) color_index = 0;

    if (x1 > x2) {
        int tmpx = x1; x1 = x2; x2 = tmpx;
        int tmpy = y1; y1 = y2; y2 = tmpy;
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    int current_x = x1;
    int current_y = y1;

    while (true) {
        draw_pixel_gfx(cpu, current_x, current_y, color_index);

        if (current_x == x2 && current_y == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            current_x += sx;
        }
        if (e2 < dx) {
            err += dx;
            current_y += sy;
        }

        if (abs(current_x - x1) > MAX_SCREEN_DIM + MAX_SCREEN_DIM ||
            abs(current_y - y1) > MAX_SCREEN_DIM + MAX_SCREEN_DIM) {
            fprintf(stderr, "Warning DRAW_LINE: Potential infinite loop detected for line from (%d,%d) to (%d,%d) at line %d. Aborting drawing.\n",
                cpu->registers[0], cpu->registers[1], cpu->registers[2], cpu->registers[3], cpu->ip + 1);
            break;
        }
    }

    if (cpu->screen_on == 1) {
        updateScreen(cpu);
    }
}

void draw_line_gfx_impl(VirtualCPU* cpu, int x1, int y1, int x2, int y2, int color_index) {
    if (x1 == x2 && y1 == y2) {
        draw_pixel_gfx(cpu, x1, y1, color_index);
        return;
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    int current_x = x1;
    int current_y = y1;

    int step_count = 0;
    const int max_steps = cpu->screen_width + cpu->screen_height + 100;

    while (true) {
        draw_pixel_gfx(cpu, current_x, current_y, color_index);

        if (current_x == x2 && current_y == y2) break;

        step_count++;
        if (step_count > max_steps) {
            fprintf(stderr, "Warning draw_line_gfx_impl: Exceeded max steps (%d) for line from (%d,%d) to (%d,%d) at line %d. Aborting drawing.\n",
                max_steps, x1, y1, x2, y2, cpu->ip + 1);
            break;
        }

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            current_x += sx;
        }
        if (e2 < dx) {
            err += dx;
            current_y += sy;
        }
    }
}

// Draws a filled rectangle
void draw_rect_filled_gfx(VirtualCPU* cpu) {
    int x = cpu->registers[0];
    int y = cpu->registers[1];
    int w = cpu->registers[2];
    int h = cpu->registers[3];
    int color_index = cpu->registers[4];

    if (w <= 0 || h <= 0) return;
    if (color_index < 0 || color_index >= PALETTE_SIZE) color_index = 0;

    int start_x = (x < 0) ? 0 : x;
    int start_y = (y < 0) ? 0 : y;
    int end_x = (x + w > cpu->screen_width) ? cpu->screen_width : x + w;
    int end_y = (y + h > cpu->screen_height) ? cpu->screen_height : y + h;

    if (start_x >= end_x || start_y >= end_y) return;

    for (int current_y = start_y; current_y < end_y; ++current_y) {
        for (int current_x = start_x; current_x < end_x; ++current_x) {
            draw_pixel_gfx(cpu, current_x, current_y, color_index);
        }
    }

    if (cpu->screen_on == 1) {
        updateScreen(cpu);
    }
}

void draw_rect_hollow_gfx(VirtualCPU* cpu) {
    int x = cpu->registers[0];
    int y = cpu->registers[1];
    int w = cpu->registers[2];
    int h = cpu->registers[3];
    int color_index = cpu->registers[4];

    if (w <= 0 || h <= 0) return;
    if (color_index < 0 || color_index >= PALETTE_SIZE) color_index = 0;

    int r0_orig = cpu->registers[0];
    int r1_orig = cpu->registers[1];
    int r2_orig = cpu->registers[2];
    int r3_orig = cpu->registers[3];
    int r4_orig = cpu->registers[4];

    cpu->registers[0] = x;
    cpu->registers[1] = y;
    cpu->registers[2] = x + w - 1;
    cpu->registers[3] = y;
    cpu->registers[4] = color_index;
    draw_line_gfx(cpu);

    cpu->registers[0] = x;
    cpu->registers[1] = y + h - 1;
    cpu->registers[2] = x + w - 1;
    cpu->registers[3] = y + h - 1;
    cpu->registers[4] = color_index;
    draw_line_gfx(cpu);

    cpu->registers[0] = x;
    cpu->registers[1] = y;
    cpu->registers[2] = x;
    cpu->registers[3] = y + h - 1;
    cpu->registers[4] = color_index;
    draw_line_gfx(cpu);

    cpu->registers[0] = x + w - 1;
    cpu->registers[1] = y;
    cpu->registers[2] = x + w - 1;
    cpu->registers[3] = y + h - 1;
    cpu->registers[4] = color_index;
    draw_line_gfx(cpu);

    cpu->registers[0] = r0_orig;
    cpu->registers[1] = r1_orig;
    cpu->registers[2] = r2_orig;
    cpu->registers[3] = r3_orig;
    cpu->registers[4] = r4_orig;
}

void draw_circle_points(VirtualCPU* cpu, int cx, int cy, int x, int y, int color_index) {
    draw_pixel_gfx(cpu, cx + x, cy + y, color_index);
    draw_pixel_gfx(cpu, cx - x, cy + y, color_index);
    draw_pixel_gfx(cpu, cx + x, cy - y, color_index);
    draw_pixel_gfx(cpu, cx - x, cy - y, color_index);
    draw_pixel_gfx(cpu, cx + y, cy + x, color_index);
    draw_pixel_gfx(cpu, cx - y, cy + x, color_index);
    draw_pixel_gfx(cpu, cx + y, cy - x, color_index);
    draw_pixel_gfx(cpu, cx - y, cy - x, color_index);
}

// Draws a hollow circle using the Midpoint Circle Algorithm
void draw_circle_hollow_gfx(VirtualCPU* cpu) {
    int cx = cpu->registers[0];
    int cy = cpu->registers[1];
    int radius = cpu->registers[2];
    int color_index = cpu->registers[3];

    if (radius <= 0) {
        if (radius == 0) draw_pixel_gfx(cpu, cx, cy, color_index);
        return;
    }
    if (color_index < 0 || color_index >= PALETTE_SIZE) color_index = 0;

    int x = 0;
    int y = radius;
    int p = 1 - radius;

    draw_circle_points(cpu, cx, cy, x, y, color_index);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        }
        else {
            y--;
            p += 2 * (x - y) + 1;
        }
        draw_circle_points(cpu, cx, cy, x, y, color_index);
    }
}

void draw_circle_filled_gfx(VirtualCPU* cpu) {
    int cx = cpu->registers[0];
    int cy = cpu->registers[1];
    int radius = cpu->registers[2];
    int color_index = cpu->registers[3];

    if (radius <= 0) {
        if (radius == 0) draw_pixel_gfx(cpu, cx, cy, color_index);
        return;
    }
    if (color_index < 0 || color_index >= PALETTE_SIZE) color_index = 0;

    for (int y = -radius; y <= radius; ++y) {
        int dy_sq = y * y;
        int radius_sq = radius * radius;
        int dx_sq = radius_sq - dy_sq;

        if (dx_sq >= 0) {
            int dx = round(sqrt(dx_sq));

            int start_x = cx - dx;
            int end_x = cx + dx;
            int current_y = cy + y;

            for (int current_x = start_x; current_x <= end_x; ++current_x) {
                draw_pixel_gfx(cpu, current_x, current_y, color_index);
            }
        }
    }
}

long long triangle_signed_area_test(long long p1x, long long p1y, long long p2x, long long p2y, long long ptx, long long pty) {
    return (p2x - p1x) * (pty - p1y) - (ptx - p1x) * (p2y - p1y);
}

void draw_triangle_filled_gfx(VirtualCPU* cpu) {
    int x1 = cpu->registers[0]; int y1 = cpu->registers[1];
    int x2 = cpu->registers[2]; int y2 = cpu->registers[3];
    int x3 = cpu->registers[4]; int y3 = cpu->registers[5];
    int color_index = cpu->registers[6];

    int min_x = SDL_min(x1, SDL_min(x2, x3));
    int max_x = SDL_max(x1, SDL_max(x2, x3));
    int min_y = SDL_min(y1, SDL_min(y2, y3));
    int max_y = SDL_max(y1, SDL_max(y2, y3));

    min_x = SDL_max(min_x, 0);
    min_y = SDL_max(min_y, 0);
    max_x = SDL_min(max_x, cpu->screen_width - 1);
    max_y = SDL_min(max_y, cpu->screen_height - 1);

    for (int py = min_y; py <= max_y; ++py) {
        for (int px = min_x; px <= max_x; ++px) {
            long long d1 = triangle_signed_area_test(x1, y1, x2, y2, px, py);
            long long d2 = triangle_signed_area_test(x2, y2, x3, y3, px, py);
            long long d3 = triangle_signed_area_test(x3, y3, x1, y1, px, py);

            bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
            bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

            bool is_inside = !(has_neg && has_pos);

            if (is_inside) {
                draw_pixel_gfx(cpu, px, py, color_index);
            }
        }
    }
}

void draw_triangle_hollow_gfx(VirtualCPU* cpu) {
    int x1 = cpu->registers[0]; int y1 = cpu->registers[1];
    int x2 = cpu->registers[2]; int y2 = cpu->registers[3];
    int x3 = cpu->registers[4]; int y3 = cpu->registers[5];
    int color_index = cpu->registers[6];

    draw_line_gfx_impl(cpu, x1, y1, x2, y2, color_index);
    draw_line_gfx_impl(cpu, x2, y2, x3, y3, color_index);
    draw_line_gfx_impl(cpu, x3, y3, x1, y1, color_index);
}

int parse_condition_operands(const char* line_ptr, char* reg1_str, char* op_str, char* val_str, int max_val_len, int line_num) {
    reg1_str[0] = '\0'; op_str[0] = '\0'; val_str[0] = '\0';

    while (*line_ptr && (isspace((unsigned char)*line_ptr) || *line_ptr == ',')) line_ptr++;
    if (!*line_ptr) { fprintf(stderr, "Error parsing condition: Missing first operand at line %d\n", line_num); return -1; }

    char* reg1_start = line_ptr;
    char* reg1_end = line_ptr;
    while (*reg1_end && !isspace((unsigned char)*reg1_end) && *reg1_end != ',') reg1_end++;
    if (reg1_end == reg1_start) { fprintf(stderr, "Error parsing condition: Cannot parse first operand at line %d\n", line_num); return -1; }
    size_t reg1_len = reg1_end - reg1_start;
    if (reg1_len >= 64) { fprintf(stderr, "Error parsing condition: First operand too long at line %d\n", line_num); return -1; }
    strncpy(reg1_str, reg1_start, reg1_len);
    reg1_str[reg1_len] = '\0';
    line_ptr = reg1_end;

    while (*line_ptr && (isspace((unsigned char)*line_ptr) || *line_ptr == ',')) line_ptr++;
    if (!*line_ptr) { fprintf(stderr, "Error parsing condition: Missing operator at line %d\n", line_num); return -1; }

    char* op_start = line_ptr;
    char* op_end = line_ptr;
    if (strncmp(op_end, "==", 2) == 0) op_end += 2;
    else if (strncmp(op_end, "!=", 2) == 0) op_end += 2;
    else if (strncmp(op_end, ">=", 2) == 0) op_end += 2;
    else if (strncmp(op_end, "<=", 2) == 0) op_end += 2;
    else if (strncmp(op_end, ">", 1) == 0) op_end += 1;
    else if (strncmp(op_end, "<", 1) == 0) op_end += 1;
    else { fprintf(stderr, "Error parsing condition: Invalid operator at line %d\n", line_num); return -1; }
    size_t op_len = op_end - op_start;
    if (op_len >= 10) { fprintf(stderr, "Error parsing condition: Operator too long at line %d\n", line_num); return -1; }
    strncpy(op_str, op_start, op_len);
    op_str[op_len] = '\0';
    line_ptr = op_end;

    while (*line_ptr && (isspace((unsigned char)*line_ptr) || *line_ptr == ',')) line_ptr++;
    if (!*line_ptr) { fprintf(stderr, "Error parsing condition: Missing second operand at line %d\n", line_num); return -1; }

    char* val_start = line_ptr;
    strncpy(val_str, val_start, max_val_len - 1);
    val_str[max_val_len - 1] = '\0';
    char* val_end = val_str + strlen(val_str) - 1;
    while (val_end >= val_str && isspace((unsigned char)*val_end)) val_end--;
    *(val_end + 1) = '\0';

    return 0;
}

int evaluate_condition(VirtualCPU* cpu, const char* reg_str, const char* op_str, const char* val_str, int line_num) {
    int reg_idx = -1;
    if (sscanf(reg_str, "R%d", &reg_idx) != 1 || !isValidReg(reg_idx)) {
        fprintf(stderr, "Error evaluating condition: Invalid register '%s' at line %d\n", reg_str, line_num);
        return -1;
    }
    int value1 = cpu->registers[reg_idx];

    int value2;
    int reg2_idx = -1;
    if (sscanf(val_str, "R%d", &reg2_idx) == 1 && isValidReg(reg2_idx)) {
        value2 = cpu->registers[reg2_idx];
    }
    else {
        char* endptr;
        long parsed_val = strtol(val_str, &endptr, 0);
        if (val_str == endptr || *endptr != '\0') {
            double float_val;
            if (sscanf(val_str, "%lf", &float_val) == 1 && *endptr == '\0') {
                fprintf(stderr, "Error evaluating condition: Floating point immediate '%s' is not supported for integer comparisons at line %d\n", val_str, line_num);
                return -1;
            }

            fprintf(stderr, "Error evaluating condition: Invalid second operand '%s' (must be register or integer immediate) at line %d\n", val_str, line_num);
            return -1;
        }
        value2 = (int)parsed_val;
        if (parsed_val > INT_MAX || parsed_val < INT_MIN) {
        }
    }

    if (strcmp(op_str, "==") == 0) return (value1 == value2);
    if (strcmp(op_str, "!=") == 0) return (value1 != value2);
    if (strcmp(op_str, ">") == 0)  return (value1 > value2);
    if (strcmp(op_str, "<") == 0)  return (value1 < value2);
    if (strcmp(op_str, ">=") == 0) return (value1 >= value2);
    if (strcmp(op_str, "<=") == 0) return (value1 <= value2);

    fprintf(stderr, "Error evaluating condition: Unknown operator '%s' at line %d\n", op_str, line_num);
    return -1;
}

int find_matching_end(char* program[], int program_size, int start_ip) {
    InstructionType start_type = INVALID_INST;
    if (start_ip >= 0 && start_ip < program_size && program[start_ip]) {
        char op_str[MAX_LINE_LENGTH];
        char* scan_ptr = program[start_ip];
        while (*scan_ptr && isspace((unsigned char)*scan_ptr)) scan_ptr++;
        int scan_op_idx = 0;
        while (*scan_ptr && !isspace((unsigned char)*scan_ptr) && *scan_ptr != ',' && scan_op_idx < sizeof(op_str) - 1) {
            op_str[scan_op_idx++] = toupper((unsigned char)*scan_ptr++);
        }
        op_str[scan_op_idx] = '\0';
        start_type = parseInstruction(op_str);
        if (start_type != IF && start_type != WHILE) {
            fprintf(stderr, "Error (Internal): find_matching_end called on IP %d with non-block start instruction.\n", start_ip + 1);
            return -1;
        }
    }
    else {
        fprintf(stderr, "Error (Internal): find_matching_end called with invalid start_ip %d.\n", start_ip);
        return -1;
    }


    int nested_level = 0;
    int scan_ip = start_ip + 1;

    while (scan_ip < program_size) {
        if (!program[scan_ip]) { scan_ip++; continue; }

        char scan_op_str_buf[MAX_LINE_LENGTH];
        char* scan_ptr = program[scan_ip];
        while (*scan_ptr && isspace((unsigned char)*scan_ptr)) scan_ptr++;
        int scan_op_idx = 0;
        while (*scan_ptr && !isspace((unsigned char)*scan_ptr) && *scan_ptr != ',' && scan_op_idx < sizeof(scan_op_str_buf) - 1) {
            scan_op_str_buf[scan_op_idx++] = toupper((unsigned char)*scan_ptr++);
        }
        scan_op_str_buf[scan_op_idx] = '\0';

        InstructionType scanned_inst = parseInstruction(scan_op_str_buf);

        if (scanned_inst == IF || scanned_inst == WHILE) {
            nested_level++;
        }
        else if (scanned_inst == END) {
            if (nested_level > 0) {
                nested_level--;
            }
            else {
                return scan_ip;
            }
        }
        scan_ip++;
    }
    return -1;
}

int find_enclosing_while_ip(char* program[], int program_size, int current_ip) {
    int nested_depth = 0;
    int scan_ip = current_ip - 1;

    while (scan_ip >= 0) {
        if (!program[scan_ip]) { scan_ip--; continue; }

        char scan_op_str_buf[MAX_LINE_LENGTH];
        char* scan_ptr = program[scan_ip];
        while (*scan_ptr && isspace((unsigned char)*scan_ptr)) scan_ptr++;
        int scan_op_idx = 0;
        while (*scan_ptr && !isspace((unsigned char)*scan_ptr) && *scan_ptr != ',' && scan_op_idx < sizeof(scan_op_str_buf) - 1) {
            scan_op_str_buf[scan_op_idx++] = toupper((unsigned char)*scan_ptr++);
        }
        scan_op_str_buf[scan_op_idx] = '\0';

        InstructionType scanned_inst = parseInstruction(scan_op_str_buf);

        if (scanned_inst == END) {
            nested_depth++;
        }
        else if (scanned_inst == IF || scanned_inst == WHILE) {
            if (nested_depth > 0) {
                nested_depth--;
            }
            else {
                if (scanned_inst == WHILE) {
                    return scan_ip;
                }
            }
        }
        scan_ip--;
    }
    return -1;
}

bool initialize_disk_image(VirtualCPU* cpu) {
    if (cpu->disk_image_fp != NULL) {
        fclose(cpu->disk_image_fp);
        cpu->disk_image_fp = NULL;
    }

    FILE* fp = fopen(DISK_IMAGE_FILENAME, "rb+");

    if (fp == NULL) {
        printf("Disk image '%s' not found, creating new 16MB file...\n", DISK_IMAGE_FILENAME);
        fp = fopen(DISK_IMAGE_FILENAME, "wb+");
        if (fp == NULL) {
            perror("Error creating disk image file");
            fprintf(stderr, "Failed to create '%s'. Disk operations will fail.\n", DISK_IMAGE_FILENAME);
            return false;
        }

        char* zero_buffer = (char*)calloc(DISK_SECTOR_SIZE, 1);
        if (!zero_buffer) {
            fprintf(stderr, "Failed to allocate zero buffer for disk init.\n");
            fclose(fp);
            return false;
        }
        long long bytes_written = 0;
        size_t write_result;
        fseek(fp, 0, SEEK_SET);
        printf("Writing zeros to disk image (this may take a moment)...\n");
        while (bytes_written < DISK_IMAGE_SIZE_BYTES) {
            size_t bytes_to_write = DISK_SECTOR_SIZE;
            if (bytes_written + bytes_to_write > DISK_IMAGE_SIZE_BYTES) {
                bytes_to_write = DISK_IMAGE_SIZE_BYTES - bytes_written;
                if (bytes_to_write == 0) break;
            }
            write_result = fwrite(zero_buffer, 1, bytes_to_write, fp);
            if (write_result != bytes_to_write) {
                perror("Error writing zeros to new disk image file");
                fprintf(stderr, "Failed to fully initialize '%s'. Disk operations may fail.\n", DISK_IMAGE_FILENAME);
                free(zero_buffer);
                fclose(fp);
                return false;
            }
            bytes_written += write_result;
        }
        printf("Disk image created and zeroed.\n");
        free(zero_buffer);
        fflush(fp);
        fseek(fp, 0, SEEK_SET);
    }

    fseek(fp, 0, SEEK_END);
    long long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size < 0) {
        perror("Error getting disk image file size");
        fclose(fp);
        return false;
    }

    cpu->disk_image_fp = fp;
    cpu->disk_image_size = file_size;
    cpu->disk_sector_size = DISK_SECTOR_SIZE;

    printf("Disk image '%s' opened. Size: %lld bytes (%lld sectors).\n",
        DISK_IMAGE_FILENAME, cpu->disk_image_size, cpu->disk_image_size / cpu->disk_sector_size);

    if (cpu->disk_image_size != DISK_IMAGE_SIZE_BYTES) {
        fprintf(stderr, "Warning: Disk image size (%lld bytes) differs from expected (%d bytes).\n",
            cpu->disk_image_size, DISK_IMAGE_SIZE_BYTES);
    }


    return true;
}

bool init_cpu(VirtualCPU* cpu) {
    if (!cpu) return false;

    memset(cpu->registers, 0, sizeof(cpu->registers));
    memset(cpu->f_registers, 0, sizeof(cpu->f_registers));
    memset(cpu->memory, 0, sizeof(cpu->memory));
    cpu->ip = 0;
    cpu->flags = 0;
    cpu->sp = MEMORY_SIZE;
    cpu->shutdown_requested = false;

    cpu->audioDevice = 0;
    cpu->frequency = 440.0;
    cpu->volume = 0.5;
    cpu->speaker_on = 0;
    cpu->sleep_duration = 0;
    cpu->sleep_start_time = 0;

    cpu->window = NULL;
    cpu->renderer = NULL;
    cpu->texture = NULL;
    cpu->screen_width = DEFAULT_SCREEN_WIDTH;
    cpu->screen_height = DEFAULT_SCREEN_HEIGHT;
    cpu->screen_on = 0;

    cpu->pixels = (Uint32*)malloc(cpu->screen_width * cpu->screen_height * sizeof(Uint32));
    if (!cpu->pixels) {
        fprintf(stderr, "Error: Failed to allocate pixel buffer\n");
        return false;
    }
    memset(cpu->pixels, 0, cpu->screen_width * cpu->screen_height * sizeof(Uint32));

    // Index 0: Black color (R: 0, G: 0, B: 0, A: 255)
    cpu->palette[0] = (SDL_Color){ 0, 0, 0, 255 };

    // Index 1: Blue color (R: 0, G: 0, B: 170, A: 255)
    cpu->palette[1] = (SDL_Color){ 0, 0, 170, 255 };

    // Index 2: Green color (R: 0, G: 170, B: 0, A: 255)
    cpu->palette[2] = (SDL_Color){ 0, 170, 0, 255 };

    // Index 3: Cyan color (R: 0, G: 170, B: 170, A: 255)
    cpu->palette[3] = (SDL_Color){ 0, 170, 170, 255 };

    // Index 4: Red color (R: 170, G: 0, B: 0, A: 255)
    cpu->palette[4] = (SDL_Color){ 170, 0, 0, 255 };

    // Index 5: Magenta color (R: 170, G: 0, B: 170, A: 255)
    cpu->palette[5] = (SDL_Color){ 170, 0, 170, 255 };

    // Index 6: Orange color (R: 170, G: 85, B: 0, A: 255)
    cpu->palette[6] = (SDL_Color){ 170, 85, 0, 255 };

    // Index 7: Light gray color (R: 170, G: 170, B: 170, A: 255)
    cpu->palette[7] = (SDL_Color){ 170, 170, 170, 255 };

    // Index 8: Dark gray color (R: 85, G: 85, B: 85, A: 255)
    cpu->palette[8] = (SDL_Color){ 85, 85, 85, 255 };

    // Index 9: Light blue color (R: 85, G: 85, B: 255, A: 255)
    cpu->palette[9] = (SDL_Color){ 85, 85, 255, 255 };

    // Index 10: Light green color (R: 85, G: 255, B: 85, A: 255)
    cpu->palette[10] = (SDL_Color){ 85, 255, 85, 255 };

    // Index 11: Light cyan color (R: 85, G: 255, B: 255, A: 255)
    cpu->palette[11] = (SDL_Color){ 85, 255, 255, 255 };

    // Index 12: Light red color (R: 255, G: 85, B: 85, A: 255)
    cpu->palette[12] = (SDL_Color){ 255, 85, 85, 255 };

    // Index 13: Light magenta color (R: 255, G: 85, B: 255, A: 255)
    cpu->palette[13] = (SDL_Color){ 255, 85, 255, 255 };

    // Index 14: Yellow color (R: 255, G: 255, B: 85, A: 255)
    cpu->palette[14] = (SDL_Color){ 255, 255, 85, 255 };

    // Index 15: White color (R: 255, G: 255, B: 255, A: 255)
    cpu->palette[15] = (SDL_Color){ 255, 255, 255, 255 };

    cpu->disk_image_fp = NULL;
    cpu->disk_image_size = 0;
    cpu->disk_sector_size = 512;

    srand(time(NULL));

    return true;
}

void cleanup_cpu(VirtualCPU* cpu) {
    if (cpu) {
        free(cpu->pixels);
        cpu->pixels = NULL;

        if (cpu->disk_image_fp) {
            printf("Closing disk image '%s'.\n", DISK_IMAGE_FILENAME);
            fclose(cpu->disk_image_fp);
            cpu->disk_image_fp = NULL;
        }
    }
}

bool init_sdl(VirtualCPU* cpu) {
    if (!cpu) return false;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s", SDL_GetError());
        return false;
    }

    cpu->window = SDL_CreateWindow("Virtual CPU Screen",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        cpu->screen_width, cpu->screen_height,
        SDL_WINDOW_SHOWN);
    if (!cpu->window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    cpu->renderer = SDL_CreateRenderer(cpu->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!cpu->renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_DestroyWindow(cpu->window);
        SDL_Quit();
        return false;
    }

    cpu->texture = SDL_CreateTexture(cpu->renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
        cpu->screen_width, cpu->screen_height);
    if (!cpu->texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTexture Error: %s", SDL_GetError());
        SDL_DestroyRenderer(cpu->renderer);
        SDL_DestroyWindow(cpu->window);
        SDL_Quit();
        return false;
    }

    SDL_AudioSpec desiredSpec, obtainedSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = 44100;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 512;
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = cpu;

    cpu->audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &obtainedSpec, 0);
    if (cpu->audioDevice == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_OpenAudioDevice Error: %s", SDL_GetError());
        SDL_DestroyTexture(cpu->texture);
        SDL_DestroyRenderer(cpu->renderer);
        SDL_DestroyWindow(cpu->window);
        SDL_Quit();
        return false;
    }
    cpu->audioSpec = obtainedSpec;

    SDL_PauseAudioDevice(cpu->audioDevice, 0);

    return true;
}

void cleanup_sdl(VirtualCPU* cpu) {
    if (cpu) {
        if (cpu->audioDevice > 0) {
            SDL_CloseAudioDevice(cpu->audioDevice);
        }
        if (cpu->texture) {
            SDL_DestroyTexture(cpu->texture);
        }
        if (cpu->renderer) {
            SDL_DestroyRenderer(cpu->renderer);
        }
        if (cpu->window) {
            SDL_DestroyWindow(cpu->window);
        }
    }
    SDL_Quit();
}

#ifdef _WIN32
void enable_virtual_terminal_processing_if_needed() {
    static bool initialized = false;
    if (initialized) return;

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return;
    }

    if (!(dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, dwMode)) {
            fprintf(stderr, "Warning: Failed to enable virtual terminal processing for console.\n");
        }
        else {
            initialized = true;
        }
    }
    else {
        initialized = true;
    }
}
#else
void enable_virtual_terminal_processing_if_needed() {}
#endif

InstructionType parseInstruction(const char* instruction) {
    if (strcasecmp(instruction, "MOV") == 0) return MOV;
    if (strcasecmp(instruction, "ADD") == 0) return ADD;
    if (strcasecmp(instruction, "SUB") == 0) return SUB;
    if (strcasecmp(instruction, "MUL") == 0) return MUL;
    if (strcasecmp(instruction, "DIV") == 0) return DIV;
    if (strcasecmp(instruction, "INT") == 0) return INTR;
    if (strcasecmp(instruction, "NOP") == 0) return NOP;
    if (strcasecmp(instruction, "HLT") == 0) return HLT;
    if (strcasecmp(instruction, "NOT") == 0) return NOT;
    if (strcasecmp(instruction, "AND") == 0) return AND;
    if (strcasecmp(instruction, "OR") == 0) return OR;
    if (strcasecmp(instruction, "XOR") == 0) return XOR;
    if (strcasecmp(instruction, "SHL") == 0) return SHL;
    if (strcasecmp(instruction, "SHR") == 0) return SHR;
    if (strcasecmp(instruction, "JMP") == 0) return JMP;
    if (strcasecmp(instruction, "CMP") == 0) return CMP;
    if (strcasecmp(instruction, "JMPNE") == 0) return JMPNE;
    if (strcasecmp(instruction, "JMPH") == 0) return JMPH;
    if (strcasecmp(instruction, "JMPL") == 0) return JMPL;
    if (strcasecmp(instruction, "NEG") == 0) return NEG;
    if (strcasecmp(instruction, "INC") == 0) return INC;
    if (strcasecmp(instruction, "DEC") == 0) return DEC;
    if (strcasecmp(instruction, "XCHG") == 0) return XCHG;
    if (strcasecmp(instruction, "CLR") == 0) return CLR;
    if (strcasecmp(instruction, "PUSH") == 0) return PUSH;
    if (strcasecmp(instruction, "POP") == 0) return POP;
    if (strcasecmp(instruction, "CALL") == 0) return CALL;
    if (strcasecmp(instruction, "RET") == 0) return RET;
    if (strcasecmp(instruction, "ROL") == 0) return ROL;
    if (strcasecmp(instruction, "ROR") == 0) return ROR;
    if (strcasecmp(instruction, "STRMOV") == 0) return STRMOV;
    if (strcasecmp(instruction, "RND") == 0) return RND;
    if (strcasecmp(instruction, "JMPE") == 0) return JMPE;
    if (strcasecmp(instruction, "POW") == 0) return POW;
    if (strcasecmp(instruction, "MOD") == 0) return MOD;
    if (strcasecmp(instruction, "ABS") == 0) return ABS;
    if (strcasecmp(instruction, "LOOP") == 0) return LOOP;
    if (strcasecmp(instruction, "LOAD") == 0) return LOAD;
    if (strcasecmp(instruction, "STORE") == 0) return STORE;
    if (strcasecmp(instruction, "TEST") == 0) return TEST;
    if (strcasecmp(instruction, "LEA") == 0) return LEA;
    if (strcasecmp(instruction, "PUSHF") == 0) return PUSHF;
    if (strcasecmp(instruction, "POPF") == 0) return POPF;
    if (strcasecmp(instruction, "SETF") == 0) return SETF;
    if (strcasecmp(instruction, "CLRF") == 0) return CLRF;
    if (strcasecmp(instruction, "BT") == 0) return BT;
    if (strcasecmp(instruction, "BSET") == 0) return BSET;
    if (strcasecmp(instruction, "BCLR") == 0) return BCLR;
    if (strcasecmp(instruction, "BTOG") == 0) return BTOG;
    if (strcasecmp(instruction, "STRCMP") == 0) return STRCMP;
    if (strcasecmp(instruction, "STRLEN") == 0) return STRLEN;
    if (strcasecmp(instruction, "STRCPY") == 0) return STRCPY;
    if (strcasecmp(instruction, "MEMCPY") == 0) return MEMCPY;
    if (strcasecmp(instruction, "MEMSET") == 0) return MEMSET;
    if (strcasecmp(instruction, "CPUID") == 0) return CPUID;
    if (strcasecmp(instruction, "BSWAP") == 0) return BSWAP;
    if (strcasecmp(instruction, "SAR") == 0) return SAR;
    if (strcasecmp(instruction, "RVD") == 0) return RVD;
    if (strcasecmp(instruction, "INCMEM") == 0) return INC_MEM;
    if (strcasecmp(instruction, "DECMEM") == 0) return DEC_MEM;
    if (strcasecmp(instruction, "JMPO") == 0) return JMPO;
    if (strcasecmp(instruction, "JMPNO") == 0) return JMPNO;
    if (strcasecmp(instruction, "JMPHE") == 0) return JMPHE;
    if (strcasecmp(instruction, "JMPLE") == 0) return JMPLE;
    if (strcasecmp(instruction, "FMOV") == 0) return FMOV;
    if (strcasecmp(instruction, "FADD") == 0) return FADD;
    if (strcasecmp(instruction, "FSUB") == 0) return FSUB;
    if (strcasecmp(instruction, "FMUL") == 0) return FMUL;
    if (strcasecmp(instruction, "FDIV") == 0) return FDIV;
    if (strcasecmp(instruction, "FCMP") == 0) return FCMP;
    if (strcasecmp(instruction, "FABS") == 0) return FABS;
    if (strcasecmp(instruction, "FNEG") == 0) return FNEG;
    if (strcasecmp(instruction, "FSQRT") == 0) return FSQRT;
    if (strcasecmp(instruction, "CVTIF") == 0) return CVTIF;
    if (strcasecmp(instruction, "CVTFI") == 0) return CVTFI;
    if (strcasecmp(instruction, "FLOAD") == 0) return FLOAD;
    if (strcasecmp(instruction, "FSTORE") == 0) return FSTORE;
    if (strcasecmp(instruction, "FINC") == 0) return FINC;
    if (strcasecmp(instruction, "FDEC") == 0) return FDEC;
    if (strcasecmp(instruction, "FPUSH") == 0) return FPUSH;
    if (strcasecmp(instruction, "FPOP") == 0) return FPOP;
    if (strcasecmp(instruction, "FCLR") == 0) return FCLR;
    if (strcasecmp(instruction, "FXCHG") == 0) return FXCHG;
    if (strcasecmp(instruction, "FPOW") == 0) return FPOW;
    if (strcasecmp(instruction, "CALLH") == 0) return CALLH;
    if (strcasecmp(instruction, "CALLL") == 0) return CALLL;
    if (strcasecmp(instruction, "CALLE") == 0) return CALLE;
    if (strcasecmp(instruction, "CALLNE") == 0) return CALLNE;
    if (strcasecmp(instruction, "CALLO") == 0) return CALLO;
    if (strcasecmp(instruction, "CALLNO") == 0) return CALLNO;
    if (strcasecmp(instruction, "CALLHE") == 0) return CALLHE;
    if (strcasecmp(instruction, "CALLLE") == 0) return CALLLE;
    if (strcasecmp(instruction, "IF") == 0) return IF;
    if (strcasecmp(instruction, "ELSE") == 0) return ELSE;
    if (strcasecmp(instruction, "END") == 0) return END;
    if (strcasecmp(instruction, "WHILE") == 0) return WHILE;
    if (strcasecmp(instruction, "BREAK") == 0) return BREAK;
    if (strcasecmp(instruction, "CONTINUE") == 0) return CONTINUE;
    return INVALID_INST;
}

int loadProgram(const char* filename, char* program[], int max_size, int* out_start_ip) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open program file '%s'\n", filename);
        return -1;
    }

    char buffer[MAX_LINE_LENGTH];
    char** temp_program = (char**)malloc(max_size * sizeof(char*));
    if (!temp_program) {
        fprintf(stderr, "Error: Memory allocation failed for temp_program.\n");
        fclose(file);
        return -2;
    }
    memset(temp_program, 0, max_size * sizeof(char*));
    int temp_line_count = 0;
    int final_program_size = 0;

    DefineEntry* define_map = NULL;
    int define_count = 0;
    define_map = malloc(MAX_DEFINES * sizeof(DefineEntry));
    if (define_map == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for define_map.\n");
        free(temp_program);
        fclose(file);
        return -2;
    }

    LabelEntry* label_map = NULL;
    label_map = malloc(max_size * sizeof(LabelEntry));
    if (!label_map) {
        fprintf(stderr, "Error: Memory allocation failed for label_map.\n");
        free(define_map);
        free(temp_program);
        fclose(file);
        return -2;
    }
    int label_count = 0;
    int main_label_line = -1;

    PreprocessorState preproc_stack[MAX_PREPROC_DEPTH];
    int preproc_stack_ptr = -1;
    bool currently_active = true;

    if (out_start_ip) {
        *out_start_ip = -1;
    }
    else {
        fprintf(stderr, "Error: Output start IP pointer is NULL in loadProgram.\n");
        if (label_map) free(label_map);
        if (define_map) free(define_map);
        if (temp_program) free(temp_program);
        fclose(file);
        return -12;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL && temp_line_count < max_size) {
        buffer[strcspn(buffer, "\n")] = 0;
        char* comment_start = strchr(buffer, ';');
        if (comment_start != NULL) {
            *comment_start = '\0';
        }
        char* start = buffer;
        while (isspace((unsigned char)*start)) start++;
        char* end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';

        if (strncmp(start, "#ifdef", 6) == 0 && isspace((unsigned char)start[6])) {
            char symbol_name[64];
            if (sscanf(start + 6, " %63s", symbol_name) == 1) {
                if (preproc_stack_ptr + 1 >= MAX_PREPROC_DEPTH) {
                    fprintf(stderr, "Error: Exceeded maximum preprocessor nesting depth (%d) at line (approx) %d\n", MAX_PREPROC_DEPTH, temp_line_count + final_program_size + 1);
                    goto load_error_cleanup;
                }
                bool parent_is_active = (preproc_stack_ptr >= 0) ? preproc_stack[preproc_stack_ptr].condition_met && preproc_stack[preproc_stack_ptr].parent_active : true;
                bool defined = false;
                for (int k = 0; k < define_count; ++k) {
                    if (strcmp(define_map[k].name, symbol_name) == 0) {
                        defined = true;
                        break;
                    }
                }
                bool current_condition_met = defined;
                preproc_stack_ptr++;
                preproc_stack[preproc_stack_ptr].condition_met = current_condition_met;
                preproc_stack[preproc_stack_ptr].parent_active = parent_is_active;
                preproc_stack[preproc_stack_ptr].seen_else = false;
                currently_active = preproc_stack[preproc_stack_ptr].condition_met && preproc_stack[preproc_stack_ptr].parent_active;
            }
            else {
                fprintf(stderr, "Warning: Malformed #ifdef at line (approx) %d: '%s'\n", temp_line_count + final_program_size + 1, start);
            }
            continue;
        }
        else if (strncmp(start, "#ifndef", 7) == 0 && isspace((unsigned char)start[7])) {
            char symbol_name[64];
            if (sscanf(start + 7, " %63s", symbol_name) == 1) {
                if (preproc_stack_ptr + 1 >= MAX_PREPROC_DEPTH) {
                    fprintf(stderr, "Error: Exceeded maximum preprocessor nesting depth (%d) at line (approx) %d\n", MAX_PREPROC_DEPTH, temp_line_count + final_program_size + 1);
                    goto load_error_cleanup;
                }
                bool parent_is_active = (preproc_stack_ptr >= 0) ? preproc_stack[preproc_stack_ptr].condition_met && preproc_stack[preproc_stack_ptr].parent_active : true;
                bool defined = false;
                for (int k = 0; k < define_count; ++k) {
                    if (strcmp(define_map[k].name, symbol_name) == 0) {
                        defined = true;
                        break;
                    }
                }
                bool current_condition_met = !defined;
                preproc_stack_ptr++;
                preproc_stack[preproc_stack_ptr].condition_met = current_condition_met;
                preproc_stack[preproc_stack_ptr].parent_active = parent_is_active;
                preproc_stack[preproc_stack_ptr].seen_else = false;
                currently_active = preproc_stack[preproc_stack_ptr].condition_met && preproc_stack[preproc_stack_ptr].parent_active;
            }
            else {
                fprintf(stderr, "Warning: Malformed #ifndef at line (approx) %d: '%s'\n", temp_line_count + final_program_size + 1, start);
            }
            continue;
        }
        else if (strcmp(start, "#else") == 0) {
            if (preproc_stack_ptr < 0) {
                fprintf(stderr, "Error: Stray #else without matching #if/#ifdef/#ifndef at line (approx) %d\n", temp_line_count + final_program_size + 1);
                goto load_error_cleanup;
            }
            if (preproc_stack[preproc_stack_ptr].seen_else) {
                fprintf(stderr, "Error: Multiple #else directives for the same #if/#ifdef/#ifndef at line (approx) %d\n", temp_line_count + final_program_size + 1);
                goto load_error_cleanup;
            }
            preproc_stack[preproc_stack_ptr].seen_else = true;
            preproc_stack[preproc_stack_ptr].condition_met = !preproc_stack[preproc_stack_ptr].condition_met;
            currently_active = preproc_stack[preproc_stack_ptr].condition_met && preproc_stack[preproc_stack_ptr].parent_active;
            continue;
        }
        else if (strcmp(start, "#endif") == 0) {
            if (preproc_stack_ptr < 0) {
                fprintf(stderr, "Error: Stray #endif without matching #if/#ifdef/#ifndef at line (approx) %d\n", temp_line_count + final_program_size + 1);
                goto load_error_cleanup;
            }
            preproc_stack_ptr--;
            currently_active = (preproc_stack_ptr >= 0) ? (preproc_stack[preproc_stack_ptr].condition_met && preproc_stack[preproc_stack_ptr].parent_active) : true;
            continue;
        }

        if (!currently_active) {
            continue;
        }

        if (strncmp(start, "#define", 7) == 0 && isspace((unsigned char)start[7])) {
            char define_name[64];
            char define_value[64];
            int scan_count = sscanf(start + 7, " %63s %63s", define_name, define_value);
            if (scan_count == 2) {
                if (define_count < MAX_DEFINES) {
                    bool duplicate = false;
                    for (int k = 0; k < define_count; ++k) {
                        if (strcmp(define_map[k].name, define_name) == 0) {
                            fprintf(stderr, "Warning: Duplicate define for '%s' ignored (line approx %d).\n", define_name, temp_line_count + final_program_size + 1);
                            duplicate = true;
                            break;
                        }
                    }
                    if (!duplicate) {
                        strncpy(define_map[define_count].name, define_name, sizeof(define_map[0].name) - 1);
                        define_map[define_count].name[sizeof(define_map[0].name) - 1] = '\0';
                        strncpy(define_map[define_count].value, define_value, sizeof(define_map[0].value) - 1);
                        define_map[define_count].value[sizeof(define_map[0].value) - 1] = '\0';
                        define_count++;
                    }
                }
                else {
                    fprintf(stderr, "Error: Maximum number of defines (%d) exceeded.\n", MAX_DEFINES);
                }
            }
            else {
                fprintf(stderr, "Warning: Invalid #define format: '%s'. Skipping.\n", start);
            }
            continue;
        }

        if (strncmp(start, "#warning", 8) == 0 && (isspace((unsigned char)start[8]) || start[8] == '\0')) {
            char* warning_msg = start + 8;
            while (isspace((unsigned char)*warning_msg)) warning_msg++;
            fprintf(stderr, "Warning: %s (at line approx %d)\n", warning_msg, temp_line_count + final_program_size + 1);
            continue;
        }

        if (strncmp(start, "#error", 6) == 0 && (isspace((unsigned char)start[6]) || start[6] == '\0')) {
            char* error_msg = start + 6;
            while (isspace((unsigned char)*error_msg)) error_msg++;
            fprintf(stderr, "Error: %s (encountered at line approx %d)\n", error_msg, temp_line_count + final_program_size + 1);
            goto load_error_cleanup;
        }

        if (strlen(start) == 0) {
            continue;
        }

        if (temp_line_count >= max_size) {
            fprintf(stderr, "Error: Processed program exceeds maximum size of %d lines (check for infinite loops or large expansions).\n", max_size);
            goto load_error_cleanup;
        }

        temp_program[temp_line_count] = strdup_portable(start);
        if (!temp_program[temp_line_count]) {
            fprintf(stderr, "Error: Memory allocation failed during program load.\n");
            goto load_error_cleanup;
        }
        temp_line_count++;
    }

    if (preproc_stack_ptr != -1) {
        fprintf(stderr, "Error: Unterminated #if/#ifdef/#ifndef directive(s) at end of file.\n");
        goto load_error_cleanup;
    }

    fclose(file);
    file = NULL;

    for (int i = 0; i < temp_line_count; i++) {
        char* original_line = temp_program[i];
        if (!original_line) continue;

        char processed_line[MAX_LINE_LENGTH * 2] = { 0 };
        char token_buffer[MAX_LINE_LENGTH];
        char* current_pos = original_line;
        bool line_changed = false;

        char op_check[10];
        sscanf(original_line, "%9s", op_check);
        bool is_strmov = (strcasecmp(op_check, "STRMOV") == 0);
        char* quote_start = is_strmov ? strchr(original_line, '"') : NULL;
        char* first_token_end = strpbrk(original_line, " \t,");

        if (first_token_end) {
            strncpy(processed_line, original_line, first_token_end - original_line);
            processed_line[first_token_end - original_line] = '\0';
            current_pos = first_token_end;
        }
        else {
            strcpy(processed_line, original_line);
            current_pos = original_line + strlen(original_line);
        }

        while (*current_pos != '\0') {
            char* separator_start = current_pos;
            while (*current_pos != '\0' && (isspace((unsigned char)*current_pos) || *current_pos == ',')) {
                current_pos++;
            }
            if (current_pos > separator_start) {
                strncat(processed_line, separator_start, current_pos - separator_start);
            }
            if (*current_pos == '\0') break;

            if (quote_start && current_pos >= quote_start) {
                char* quote_end = strrchr(current_pos, '"');
                if (quote_end && quote_end > current_pos) {
                    strncat(processed_line, current_pos, quote_end - current_pos + 1);
                    current_pos = quote_end + 1;
                }
                else {
                    strcat(processed_line, current_pos);
                    current_pos += strlen(current_pos);
                }
                continue;
            }

            char* token_start = current_pos;
            int token_len = 0;
            while (*current_pos != '\0' && !isspace((unsigned char)*current_pos) && *current_pos != ',') {
                current_pos++;
                token_len++;
            }
            if (token_len == 0) break;
            if (token_len >= MAX_LINE_LENGTH) {
                fprintf(stderr, "Error: Token too long near '%.*s' on processed line %d\n", 20, token_start, i + 1);
                strncpy(token_buffer, token_start, MAX_LINE_LENGTH - 1);
                token_buffer[MAX_LINE_LENGTH - 1] = '\0';
                strcat(processed_line, token_buffer);
                continue;
            }
            strncpy(token_buffer, token_start, token_len);
            token_buffer[token_len] = '\0';

            if ((token_buffer[0] == 'r' || token_buffer[0] == 'f') && isdigit((unsigned char)token_buffer[1])) {
                bool all_digits = true;
                for (int k = 1; token_buffer[k] != '\0'; ++k) {
                    if (!isdigit((unsigned char)token_buffer[k])) {
                        all_digits = false;
                        break;
                    }
                }
                if (all_digits) {
                    token_buffer[0] = toupper((unsigned char)token_buffer[0]);
                    line_changed = true;
                    strcat(processed_line, token_buffer);
                    continue;
                }
            }

            bool replaced = false;
            if (!((token_buffer[0] == 'R' || token_buffer[0] == 'r') && isdigit((unsigned char)token_buffer[1])) &&
                !((token_buffer[0] == 'F' || token_buffer[0] == 'f') && isdigit((unsigned char)token_buffer[1])) &&
                !(isdigit((unsigned char)token_buffer[0]) || (token_buffer[0] == '-' && isdigit((unsigned char)token_buffer[1]))) &&
                !(token_buffer[0] == '0' && (token_buffer[1] == 'x' || token_buffer[1] == 'X')))
            {
                for (int j = 0; j < define_count; j++) {
                    if (strcmp(token_buffer, define_map[j].name) == 0) {
                        strcat(processed_line, define_map[j].value);
                        replaced = true;
                        line_changed = true;
                        break;
                    }
                }
            }

            if (!replaced) {
                strcat(processed_line, token_buffer);
            }
        }

        if (line_changed) {
            free(temp_program[i]);
            temp_program[i] = strdup_portable(processed_line);
            if (!temp_program[i]) {
                fprintf(stderr, "Error: Memory allocation failed during define substitution.\n");
                goto load_error_cleanup;
            }
        }
    }

    final_program_size = 0;
    for (int i = 0; i < temp_line_count; i++) {
        char* line = temp_program[i];
        if (!line) continue;

        char* colon_pos = NULL;
        bool in_quotes = false;

        for (char* p = line; *p; ++p) {
            if (*p == '"') {
                in_quotes = !in_quotes;
            }
            else if (*p == ':' && !in_quotes) {
                colon_pos = p;
                break;
            }
        }

        if (colon_pos != NULL) {
            *colon_pos = '\0';
            char* label_start = line;
            while (isspace((unsigned char)*label_start)) label_start++;
            char* label_end = label_start + strlen(label_start) - 1;
            while (label_end > label_start && isspace((unsigned char)*label_end)) label_end--;
            *(label_end + 1) = '\0';

            if (strlen(label_start) > 0 && strlen(label_start) < sizeof(label_map[0].label_name)) {
                bool duplicate = false;
                for (int k = 0; k < label_count; ++k) {
                    if (strcmp(label_map[k].label_name, label_start) == 0) {
                        fprintf(stderr, "Error: Duplicate label '%s' found near processed line %d\n", label_start, i + 1);
                        duplicate = true;
                        break;
                    }
                }
                if (duplicate) {
                    free(temp_program[i]);
                    temp_program[i] = NULL;
                    goto load_error_cleanup;
                }

                strcpy(label_map[label_count].label_name, label_start);
                label_map[label_count].line_number = final_program_size + 1;
                label_count++;

                char* code_after_label = colon_pos + 1;
                while (isspace((unsigned char)*code_after_label)) code_after_label++;
                if (strlen(code_after_label) > 0) {
                    if (final_program_size < max_size) {
                        program[final_program_size] = strdup_portable(code_after_label);
                        if (!program[final_program_size]) {
                            fprintf(stderr, "Error: Memory allocation failed for code after label.\n");
                            goto load_error_cleanup;
                        }
                        final_program_size++;
                    }
                    else {
                        fprintf(stderr, "Error: Program size exceeded while processing code after label.\n");
                        goto load_error_cleanup;
                    }
                }
            }
            else if (strlen(label_start) > 0) {
                fprintf(stderr, "Warning: Label '%s' near processed line %d is too long or invalid. Ignored.\n", label_start, i + 1);
            }

            free(temp_program[i]);
            temp_program[i] = NULL;

        }
        else {
            if (final_program_size < max_size) {
                program[final_program_size++] = temp_program[i];
                temp_program[i] = NULL;
            }
            else {
                fprintf(stderr, "Error: Program size exceeded during label processing.\n");
                goto load_error_cleanup;
            }
        }
    }

    bool main_found = false;
    for (int k = 0; k < label_count; ++k) {
        if (strcmp(label_map[k].label_name, "_main") == 0) {
            main_label_line = label_map[k].line_number;
            main_found = true;
            break;
        }
    }

    if (!main_found) {
        fprintf(stderr, "Error: Entry point label '_main:' not found in '%s'. Cannot start execution.\n", filename);
        goto load_error_cleanup;
    }

    for (int i = 0; i < final_program_size; i++) {
        char* instruction = program[i];
        if (!instruction) continue;
        char op[10];
        char operand1_str[MAX_LINE_LENGTH] = { 0 };
        char operand2_str[MAX_LINE_LENGTH] = { 0 };

        int scan_count = sscanf(instruction, "%9s %[^,], %s", op, operand1_str, operand2_str);
        if (scan_count < 1) {
            scan_count = sscanf(instruction, "%9s %s", op, operand1_str);
            if (scan_count < 1) continue;
        }

        InstructionType inst = parseInstruction(op);
        char* label_operand_str = NULL;

        if (inst == JMP || inst == JMPNE || inst == JMPH || inst == JMPL || inst == CALL || inst == JMPE || inst == JMPO || inst == JMPNO || inst == JMPHE || inst == JMPLE ||
            inst == CALLH || inst == CALLL || inst == CALLE || inst == CALLNE || inst == CALLO || inst == CALLNO || inst == CALLHE || inst == CALLLE)
        {
            if (sscanf(instruction, "%*s %s", operand1_str) == 1) {
                label_operand_str = operand1_str;
            }
        }
        else if (inst == LOOP) {
            char reg_operand_str[64];
            if (sscanf(instruction, "%*s %63[^,], %s", reg_operand_str, operand2_str) == 2) {
                label_operand_str = operand2_str;
            }
        }

        if (label_operand_str != NULL && strlen(label_operand_str) > 0 &&
            !isdigit((unsigned char)label_operand_str[0]) &&
            !(label_operand_str[0] == '-' && isdigit((unsigned char)label_operand_str[1])) &&
            !((label_operand_str[0] == 'R' || label_operand_str[0] == 'r') && isdigit((unsigned char)label_operand_str[1])) &&
            !((label_operand_str[0] == 'F' || label_operand_str[0] == 'f') && isdigit((unsigned char)label_operand_str[1])))
        {
            int target_line = -1;
            for (int j = 0; j < label_count; j++) {
                if (strcmp(label_map[j].label_name, label_operand_str) == 0) {
                    target_line = label_map[j].line_number;
                    break;
                }
            }

            if (target_line != -1) {
                char new_instruction[MAX_LINE_LENGTH];
                if (inst == LOOP) {
                    char reg_operand_str[64];
                    sscanf(instruction, "%*s %63[^,]", reg_operand_str);
                    snprintf(new_instruction, sizeof(new_instruction), "%s %s, %d", op, reg_operand_str, target_line);
                }
                else {
                    snprintf(new_instruction, sizeof(new_instruction), "%s %d", op, target_line);
                }

                free(program[i]);
                program[i] = strdup_portable(new_instruction);
                if (!program[i]) {
                    fprintf(stderr, "Error: Memory allocation failed during label resolution.\n");
                    goto load_error_cleanup;
                }
            }
            else {
                fprintf(stderr, "Error: Label '%s' not found (used in final line %d: '%s')\n", label_operand_str, i + 1, instruction);
                goto load_error_cleanup;
            }
        }
        else if (label_operand_str == NULL && (inst == JMP || inst == JMPNE || inst == JMPH || inst == JMPL || inst == CALL || inst == JMPE || inst == JMPO || inst == JMPNO || inst == JMPHE || inst == JMPLE || inst == CALLH || inst == CALLL || inst == CALLE || inst == CALLNE || inst == CALLO || inst == CALLNO || inst == CALLHE || inst == CALLLE || inst == LOOP)) {
            char operand_buf[MAX_LINE_LENGTH];
            if (sscanf(instruction, "%*s %s", operand_buf) != 1 || strlen(operand_buf) == 0) {
                fprintf(stderr, "Error: Missing or invalid label/line number operand for %s instruction at final line %d: '%s'\n", op, i + 1, instruction);
                goto load_error_cleanup;
            }

            if (!isdigit((unsigned char)operand_buf[0]) && !(operand_buf[0] == '-' && isdigit((unsigned char)operand_buf[1])) &&
                !((operand_buf[0] == 'R' || operand_buf[0] == 'r') && isdigit((unsigned char)operand_buf[1]))) {
                fprintf(stderr, "Error: Expected label or line number for %s, found '%s' at final line %d: '%s'\n", op, operand_buf, i + 1, instruction);
                goto load_error_cleanup;
            }
        }
    }

    goto load_success;

load_error_cleanup:
    if (file) fclose(file);
    if (define_map) free(define_map);
    if (label_map) free(label_map);
    if (temp_program) {
        for (int i = 0; i < temp_line_count; ++i) {
            if (temp_program[i]) free(temp_program[i]);
        }
        free(temp_program);
    }
    if (program) {
        for (int i = 0; i < final_program_size; ++i) {
            if (program[i]) free(program[i]);
            program[i] = NULL;
        }
    }
    if (out_start_ip) {
        *out_start_ip = -1;
    }
    return -11;

load_success:
    if (define_map) free(define_map);
    if (label_map) free(label_map);
    if (temp_program) {
        for (int i = 0; i < temp_line_count; ++i) {
            if (temp_program[i]) free(temp_program[i]);
        }
        free(temp_program);
    }
    *out_start_ip = main_label_line - 1;
    return final_program_size;
}


void updateScreen(VirtualCPU* cpu) {
    if (!cpu || !cpu->texture || !cpu->renderer || !cpu->pixels) {
        return;
    }
    if (SDL_UpdateTexture(cpu->texture, NULL, cpu->pixels, cpu->screen_width * sizeof(Uint32)) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "SDL_UpdateTexture failed: %s", SDL_GetError());
        return;
    }
    if (SDL_RenderCopy(cpu->renderer, cpu->texture, NULL, NULL) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "SDL_RenderCopy failed: %s", SDL_GetError());
        return;
    }
    SDL_RenderPresent(cpu->renderer);
}


void interrupt(VirtualCPU* cpu, int interrupt_id) {
    switch (interrupt_id) {
    case INT_NOP: // 0x00
        nop(cpu);
        break;

    case INT_PRINT_REG0: // 0x01
        printf("%d\n", cpu->registers[0]);
        break;
    case INT_PRINT_STRING: // 0x02
    {
        int addr = cpu->registers[0];
        if (addr < 0 || addr >= MEMORY_SIZE) {
            fprintf(stderr, "Error (INT 0x02): Invalid memory address in R0: %d\n", addr);
            break;
        }
        int count = 0;
        while (addr < MEMORY_SIZE && cpu->memory[addr] != 0 && count < MEMORY_SIZE) {
            putchar(cpu->memory[addr]);
            addr++;
            count++;
        }
        putchar('\n');
    }
    break;
    case INT_PRINT_NEWLINE: // 0x03
        putchar('\n');
        break;
    case INT_GET_CHAR: // 0x04
    {
        int ch = getchar();
        cpu->registers[0] = ch;
    }
    break;
    case INT_GET_STRING: // 0x05
    {
        int buffer_addr = cpu->registers[0];
        int max_length = cpu->registers[1];
        char input_buffer[MAX_LINE_LENGTH];

        if (buffer_addr < 0 || buffer_addr >= MEMORY_SIZE) {
            fprintf(stderr, "Error (INT 0x05): Invalid buffer address in R0: %d\n", buffer_addr);
            break;
        }
        if (max_length <= 0) {
            fprintf(stderr, "Warning (INT 0x05): Invalid max length in R1: %d. Reading nothing.\n", max_length);
            if (buffer_addr < MEMORY_SIZE) cpu->memory[buffer_addr] = 0;
            break;
        }
        if (max_length >= MAX_LINE_LENGTH) {
            max_length = MAX_LINE_LENGTH - 1;
        }
        if (buffer_addr + max_length >= MEMORY_SIZE) {
            max_length = MEMORY_SIZE - buffer_addr - 1;
            if (max_length <= 0) {
                fprintf(stderr, "Error (INT 0x05): Buffer address and length exceed memory bounds.\n");
                if (buffer_addr < MEMORY_SIZE) cpu->memory[buffer_addr] = 0;
                break;
            }
        }

        fflush(stdout);

        if (fgets(input_buffer, max_length + 1, stdin) != NULL) {
            input_buffer[strcspn(input_buffer, "\n")] = 0;
            size_t len = strlen(input_buffer);
            for (size_t i = 0; i < len; i++) {
                if (buffer_addr + i < MEMORY_SIZE) {
                    cpu->memory[buffer_addr + i] = input_buffer[i];
                }
                else {
                    break;
                }
            }
            if (buffer_addr + len < MEMORY_SIZE) {
                cpu->memory[buffer_addr + len] = 0;
            }
            else if (buffer_addr + max_length < MEMORY_SIZE) {
                cpu->memory[buffer_addr + max_length] = 0;
            }
        }
        else {
            fprintf(stderr, "Error reading input for INT 0x05.\n");
            if (buffer_addr < MEMORY_SIZE) cpu->memory[buffer_addr] = 0;
        }
    }
    break;
    case INT_PRINT_HEX_REG0: // 0x06
        printf("0x%X\n", cpu->registers[0]);
        break;
    case INT_CLEAR_SCREEN_OS: // 0x07
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        break;
    case INT_SET_CONSOLE_COLOR: // 0x08
    {
        int fg_color = cpu->registers[0];
        int bg_color = cpu->registers[1];

#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            fg_color &= 0x0F;
            bg_color &= 0x0F;
            WORD attribute = (WORD)((bg_color << 4) | fg_color);
            SetConsoleTextAttribute(hConsole, attribute);
        }
        else {
            fprintf(stderr, "Warning (INT 0x08): Could not get console handle (Win32).\n");
        }
#else
        int ansi_fg, ansi_bg;
        if (fg_color >= 0 && fg_color <= 7) ansi_fg = 30 + fg_color;
        else if (fg_color >= 8 && fg_color <= 15) ansi_fg = 90 + (fg_color - 8);
        else ansi_fg = 39;

        if (bg_color >= 0 && bg_color <= 7) ansi_bg = 40 + bg_color;
        else if (bg_color >= 8 && bg_color <= 15) ansi_bg = 100 + (bg_color - 8);
        else ansi_bg = 49;

        printf("\033[%d;%dm", ansi_fg, ansi_bg);
        fflush(stdout);
#endif
    }
    break;
    case INT_RESET_CONSOLE_COLOR: // 0x09
    {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        WORD saved_attributes = 7;
        if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
            saved_attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }
        if (hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleTextAttribute(hConsole, saved_attributes);
        }
        else {
            fprintf(stderr, "Warning (INT 0x09): Could not get console handle (Win32).\n");
        }
#else
        printf("\033[0m");
        fflush(stdout);
#endif
    }
    break;
    case INT_GOTOXY: // 0x0A
    {
        int col = cpu->registers[0];
        int row = cpu->registers[1];

#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            COORD coord = { (SHORT)col, (SHORT)row };
            if (coord.X < 0) coord.X = 0;
            if (coord.Y < 0) coord.Y = 0;
            SetConsoleCursorPosition(hConsole, coord);
        }
        else {
            fprintf(stderr, "Warning (INT 0x0A): Could not get console handle (Win32).\n");
        }
#else
        printf("\033[%d;%dH", row + 1, col + 1);
        fflush(stdout);
#endif
    }
    break;
    case INT_GETXY: // 0x0B
    {
        int col_reg = 0;
        int row_reg = 1;

        cpu->registers[col_reg] = -1;
        cpu->registers[row_reg] = -1;

#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
            cpu->registers[col_reg] = consoleInfo.dwCursorPosition.X;
            cpu->registers[row_reg] = consoleInfo.dwCursorPosition.Y;
        }
        else {
            fprintf(stderr, "Warning (INT 0x0B): Failed to get console buffer info (Win32).\n");
        }
#else
        fprintf(stderr, "Warning (INT 0x0B): INT_GETXY is not reliably implemented on non-Windows platforms.\n");
#endif
    }
    break;
    case INT_GET_CONSOLE_SIZE: // 0x0C
    {
        int width_reg = 0;
        int height_reg = 1;

        cpu->registers[width_reg] = -1;
        cpu->registers[height_reg] = -1;

#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
            cpu->registers[width_reg] = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
            cpu->registers[height_reg] = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
        }
        else {
            fprintf(stderr, "Warning (INT 0x0C): Failed to get console buffer info (Win32).\n");
        }
#else
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
            cpu->registers[width_reg] = ws.ws_col;
            cpu->registers[height_reg] = ws.ws_row;
        }
        else {
            perror("ioctl TIOCGWINSZ");
            fprintf(stderr, "Warning (INT 0x0C): Failed to get console size (ioctl).\n");
        }
#endif
    }
    break;
    case INT_SET_CONSOLE_TITLE: // 0x0D
    {
        int title_addr_reg = 0;
        int title_addr = cpu->registers[title_addr_reg];

        if (!isValidMem(title_addr)) {
            fprintf(stderr, "Error (INT 0x0D): Invalid memory address for title string (0x%X).\n", title_addr);
            break;
        }

        char title_buf[256];
        int idx = 0;
        while (isValidMem(title_addr + idx) && idx < sizeof(title_buf) - 1) {
            char c = (char)cpu->memory[title_addr + idx];
            title_buf[idx] = c;
            if (c == 0) break;
            idx++;
        }
        title_buf[idx] = 0;

        if (idx == sizeof(title_buf) - 1 && title_buf[idx] != 0) {
            fprintf(stderr, "Warning (INT 0x0D): Title string too long or not null-terminated.\n");
        }

#ifdef _WIN32
        if (!SetConsoleTitle(title_buf)) {
            fprintf(stderr, "Warning (INT 0x0D): SetConsoleTitle failed (Win32).\n");
        }
#else
        printf("\033]0;%s\007", title_buf);
        fflush(stdout);
#endif
    }
    break;
    case INT_PRINT_FREG0: // 0x0E
        if (!isValidFReg(0)) {
            fprintf(stderr, "Error (INT 0x0E): F0 is not a valid F register.\n");
            break;
        }
        printf("%f\n", cpu->f_registers[0]);
        break;

    case INT_DRAW_PIXEL: // 0x10
    {
        int x = cpu->registers[0];
        int y = cpu->registers[1];
        int color_index = cpu->registers[2];
        draw_pixel_gfx(cpu, x, y, color_index);
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
    }
    break;
    case INT_CLEAR_GFX_SCREEN: // 0x11
    {
        int color_index = cpu->registers[0];
        if (!cpu->pixels) break;
        if (color_index >= 0 && color_index < PALETTE_SIZE) {
            SDL_Color palette_color = cpu->palette[color_index];
            Uint32 clear_color = ((Uint32)palette_color.a << 24) |
                ((Uint32)palette_color.r << 16) |
                ((Uint32)palette_color.g << 8) |
                ((Uint32)palette_color.b);
            for (int i = 0; i < cpu->screen_width * cpu->screen_height; i++) {
                cpu->pixels[i] = clear_color;
            }
        }
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
    }
    break;
    case INT_SCREEN_ON: // 0x12
        cpu->screen_on = 1;
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        break;
    case INT_SCREEN_OFF: // 0x13
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        cpu->screen_on = 0;
        break;
    case INT_SET_RESOLUTION: // 0x14
    {
        int new_width = cpu->registers[0];
        int new_height = cpu->registers[1];

        if (new_width > 0 && new_height > 0 &&
            new_width <= MAX_SCREEN_DIM && new_height <= MAX_SCREEN_DIM &&
            (new_width != cpu->screen_width || new_height != cpu->screen_height))
        {
            fprintf(stdout, "Attempting to change resolution to %dx%d\n", new_width, new_height);

            Uint32* new_pixels = (Uint32*)realloc(cpu->pixels, new_width * new_height * sizeof(Uint32));
            if (!new_pixels) {
                fprintf(stderr, "Error (INT 0x14): Failed to reallocate pixel buffer for new resolution.\n");
                break;
            }
            cpu->pixels = new_pixels;
            cpu->screen_width = new_width;
            cpu->screen_height = new_height;
            memset(cpu->pixels, 0, cpu->screen_width * cpu->screen_height * sizeof(Uint32));

            SDL_DestroyTexture(cpu->texture);
            cpu->texture = SDL_CreateTexture(cpu->renderer, SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                cpu->screen_width, cpu->screen_height);
            if (!cpu->texture) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTexture Error (resolution change): %s", SDL_GetError());
            }

            SDL_SetWindowSize(cpu->window, cpu->screen_width, cpu->screen_height);
        }
        else if (new_width <= 0 || new_height <= 0 || new_width > MAX_SCREEN_DIM || new_height > MAX_SCREEN_DIM) {
            fprintf(stderr, "Warning (INT 0x14): Invalid screen resolution requested: %dx%d. Max is %dx%d.\n",
                new_width, new_height, MAX_SCREEN_DIM, MAX_SCREEN_DIM);
        }
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
    }
    break;
    case INT_GET_PIXEL: // 0x15
    {
        int x = cpu->registers[0];
        int y = cpu->registers[1];
        int result_reg = 0;

        cpu->registers[result_reg] = -1;

        if (!cpu->pixels) {
            fprintf(stderr, "Error (INT 0x15): Pixel buffer not initialized.\n");
            break;
        }

        if (x >= 0 && x < cpu->screen_width && y >= 0 && y < cpu->screen_height) {
            Uint32 target_pixel = cpu->pixels[y * cpu->screen_width + x];
            for (int i = 0; i < PALETTE_SIZE; ++i) {
                SDL_Color p_color = cpu->palette[i];
                Uint32 palette_pixel = ((Uint32)p_color.a << 24) |
                    ((Uint32)p_color.r << 16) |
                    ((Uint32)p_color.g << 8) |
                    ((Uint32)p_color.b);
                if (target_pixel == palette_pixel) {
                    cpu->registers[result_reg] = i;
                    break;
                }
            }
        }
    }
    break;
    case INT_DRAW_STRING_GFX: // 0x16
    {
        int x = cpu->registers[0];
        int y = cpu->registers[1];
        int str_addr = cpu->registers[2];
        int color_idx = cpu->registers[3];
        Uint32 target_color;

        if (!cpu->pixels) break;

        if (color_idx < 0 || color_idx >= PALETTE_SIZE) {
            fprintf(stderr, "Warning (INT 0x16): Invalid color index %d. Defaulting to 0.\n", color_idx);
            color_idx = 0;
        }
        SDL_Color palette_color = cpu->palette[color_idx];
        target_color = ((Uint32)palette_color.a << 24) |
            ((Uint32)palette_color.r << 16) |
            ((Uint32)palette_color.g << 8) |
            ((Uint32)palette_color.b);


        if (!isValidMem(str_addr)) {
            fprintf(stderr, "Error (INT 0x16): Invalid string start address 0x%X\n", str_addr);
            break;
        }

        int current_addr = str_addr;
        int current_x = x;
        while (isValidMem(current_addr)) {
            unsigned char c = (unsigned char)cpu->memory[current_addr];
            if (c == 0) break;

            const unsigned char* char_data = vga_font_8x16[c];

            for (int row = 0; row < 16; ++row) {
                unsigned char row_data = char_data[row];
                int screen_y = y + row;

                if (screen_y < 0 || screen_y >= cpu->screen_height) {
                    continue;
                }

                for (int col = 0; col < 8; ++col) {
                    if ((row_data >> (7 - col)) & 1) {
                        int screen_x = current_x + col;

                        if (screen_x >= 0 && screen_x < cpu->screen_width) {
                            cpu->pixels[screen_y * cpu->screen_width + screen_x] = target_color;
                        }
                    }
                }
            }

            current_x += 8;
            current_addr++;
        }
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
    }
    break;
    case INT_BLIT: // 0x17
    {
        int dx = cpu->registers[0];
        int dy = cpu->registers[1];
        int src_addr = cpu->registers[2];
        int sw = cpu->registers[3];
        int sh = cpu->registers[4];

        if (!cpu->pixels) break;
        if (sw <= 0 || sh <= 0) break;

        long long num_bytes_needed = (long long)sw * sh;
        if (num_bytes_needed > (MEMORY_SIZE - src_addr) || src_addr < 0 || !isValidMem(src_addr) || !isValidMem(src_addr + num_bytes_needed - 1)) {
            fprintf(stderr, "Error (INT 0x17): Invalid source memory range [0x%X - 0x%X] for %dx%d blit.\n",
                src_addr, src_addr + (int)num_bytes_needed - 1, sw, sh);
            break;
        }

        for (int src_y = 0; src_y < sh; ++src_y) {
            int screen_y = dy + src_y;

            if (screen_y < 0 || screen_y >= cpu->screen_height) {
                continue;
            }

            for (int src_x = 0; src_x < sw; ++src_x) {
                int screen_x = dx + src_x;

                if (screen_x < 0 || screen_x >= cpu->screen_width) {
                    continue;
                }

                int mem_idx = src_addr + src_y * sw + src_x;

                int color_idx = cpu->memory[mem_idx];

                if (color_idx >= 0 && color_idx < PALETTE_SIZE) {
                    SDL_Color palette_color = cpu->palette[color_idx];
                    Uint32 pixel_color = ((Uint32)palette_color.a << 24) |
                        ((Uint32)palette_color.r << 16) |
                        ((Uint32)palette_color.g << 8) |
                        ((Uint32)palette_color.b);
                    cpu->pixels[screen_y * cpu->screen_width + screen_x] = pixel_color;
                }
                else {
                }
            }
        }
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
    }
    break;
    case INT_GET_SCREEN_SIZE: // 0x18
    {
        int width_reg = 0;
        int height_reg = 1;

        if (!isValidReg(width_reg) || !isValidReg(height_reg)) {
            fprintf(stderr, "Error (INT 0x18): Hardcoded result registers R%d/R%d invalid!\n", width_reg, height_reg);
            if (isValidReg(0)) cpu->registers[0] = -1;
            if (isValidReg(1)) cpu->registers[1] = -1;
            break;
        }

        cpu->registers[width_reg] = cpu->screen_width;
        cpu->registers[height_reg] = cpu->screen_height;
    }
    break;
    case INT_UPDATE_GFX_SCREEN: // 0x19
        updateScreen(cpu);
        break;
    case INT_DRAW_LINE: // 0x70
        draw_line_gfx(cpu);
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        break;
    case INT_DRAW_RECT_FILLED: // 0x71
        draw_rect_filled_gfx(cpu);
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        break;
    case INT_DRAW_RECT_HOLLOW: // 0x72
        draw_rect_hollow_gfx(cpu);
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        break;
    case INT_DRAW_CIRCLE_FILLED: // 0x73
        draw_circle_filled_gfx(cpu);
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        break;
    case INT_DRAW_CIRCLE_HOLLOW: // 0x74
        draw_circle_hollow_gfx(cpu);
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        break;
    case INT_DRAW_TRIANGLE_FILLED: // 0x76
        draw_triangle_filled_gfx(cpu);
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        break;
    case INT_DRAW_TRIANGLE_HOLLOW: // 0x77
        draw_triangle_hollow_gfx(cpu);
        if (cpu->screen_on == 1) {
            updateScreen(cpu);
        }
        break;


    case INT_SPEAKER_ON: // 0x20
        SDL_LockAudioDevice(cpu->audioDevice);
        cpu->speaker_on = 1;
        cpu->sleep_duration = 0;
        SDL_UnlockAudioDevice(cpu->audioDevice);
        break;
    case INT_SPEAKER_OFF: // 0x21
        SDL_LockAudioDevice(cpu->audioDevice);
        cpu->speaker_on = 0;
        cpu->sleep_duration = 0;
        cpu->sleep_start_time = 0;
        SDL_UnlockAudioDevice(cpu->audioDevice);
        break;
    case INT_SET_FREQ: // 0x22
        SDL_LockAudioDevice(cpu->audioDevice);
        cpu->frequency = (double)cpu->registers[0];
        if (cpu->frequency <= 0) cpu->frequency = 1.0;
        SDL_UnlockAudioDevice(cpu->audioDevice);
        break;
    case INT_SET_VOLUME: // 0x23
        SDL_LockAudioDevice(cpu->audioDevice);
        cpu->volume = (double)cpu->registers[0] / 100.0;
        if (cpu->volume < 0.0) cpu->volume = 0.0;
        if (cpu->volume > 1.0) cpu->volume = 1.0;
        SDL_UnlockAudioDevice(cpu->audioDevice);
        break;
    case INT_SPEAKER_SLEEP: // 0x24
        SDL_LockAudioDevice(cpu->audioDevice);
        cpu->sleep_duration = cpu->registers[0];
        cpu->sleep_start_time = SDL_GetTicks();
        SDL_UnlockAudioDevice(cpu->audioDevice);
        break;

    case INT_SLEEP_MS: // 0x30
    {
        int sleep_ms = cpu->registers[0];
        if (sleep_ms > 0) {
            SDL_Delay((Uint32)sleep_ms);
        }
        else if (sleep_ms < 0) {
            fprintf(stderr, "Warning (INT 0x30): Negative sleep duration (%d ms). Ignoring.\n", sleep_ms);
        }
    }
    break;
    case INT_GET_TIME: // 0x31
    {
        time_t timer;
        struct tm* tm_info;
        time(&timer);
        tm_info = localtime(&timer);
        cpu->registers[0] = tm_info->tm_hour;
        cpu->registers[1] = tm_info->tm_min;
        cpu->registers[2] = tm_info->tm_sec;
    }
    break;
    case INT_GET_TICKS: // 0x32
        cpu->registers[0] = SDL_GetTicks();
        break;
    case INT_GET_DATETIME: // 0x33
    {
        time_t timer;
        struct tm* tm_info;
        time(&timer);
        tm_info = localtime(&timer);

        cpu->registers[0] = tm_info->tm_year + 1900;
        cpu->registers[1] = tm_info->tm_mon + 1;
        cpu->registers[2] = tm_info->tm_mday;
        cpu->registers[3] = tm_info->tm_hour;
        cpu->registers[4] = tm_info->tm_min;
        cpu->registers[5] = tm_info->tm_sec;
    }
    break;

    case INT_RESET_CPU: // 0x40
    {
        int old_w = cpu->screen_width;
        int old_h = cpu->screen_height;
        Uint32* old_pixels = cpu->pixels;
        SDL_Color old_palette[PALETTE_SIZE];
        memcpy(old_palette, cpu->palette, sizeof(old_palette));
        int old_screen_on = cpu->screen_on;
        int old_speaker_on = cpu->speaker_on;
        double old_freq = cpu->frequency;
        double old_vol = cpu->volume;

        init_cpu(cpu);

        cpu->screen_width = old_w;
        cpu->screen_height = old_h;
        cpu->pixels = old_pixels;
        memcpy(cpu->palette, old_palette, sizeof(cpu->palette));
        cpu->screen_on = old_screen_on;
        cpu->speaker_on = old_speaker_on;
        cpu->frequency = old_freq;
        cpu->volume = old_vol;
    }
    break;
    case INT_SYSTEM_SHUTDOWN: // 0x41
        cpu->shutdown_requested = true;
        exit(0);
        break;
    case INT_GET_MEMORY_SIZE: // 0x42
    {
        int dest_reg = 0;
        cpu->registers[dest_reg] = MEMORY_SIZE;
    }
    break;
    case INT_BREAKPOINT: // 0x43
#ifdef _MSC_VER
        __debugbreak();
#elif defined(__GNUC__) || defined(__clang__)
        __builtin_trap();
#else
        fprintf(stderr, "INT 0x44: Breakpoint requested, but no specific mechanism for this compiler.\n");
        raise(SIGTRAP);
#endif
        break;
    case INT_DUMP_REGISTERS: // 0x44
        printf("--- Register Dump ---\n");
        for (int i = 0; i < NUM_REGISTERS; i++) {
            printf(" R%d: %d (0x%X)\n", i, cpu->registers[i], cpu->registers[i]);
        }
        printf("--- FPU Register Dump ---\n");
        for (int i = 0; i < NUM_F_REGISTERS; i++) {
            printf(" F%d: %g\n", i, cpu->f_registers[i]);
        }
        printf("--- CPU State ---\n");
        printf(" IP: %d\n", cpu->ip);
        printf(" Flags: 0x%X (Z:%d G:%d L:%d O:%d)\n", cpu->flags,
            (cpu->flags & FLAG_ZERO) ? 1 : 0,
            (cpu->flags & FLAG_GREATER) ? 1 : 0,
            (cpu->flags & FLAG_LESS) ? 1 : 0,
            (cpu->flags & FLAG_OVERFLOW) ? 1 : 0);
        printf(" SP: %d (0x%X)\n", cpu->sp, cpu->sp);
        printf("---------------------\n");
        break;
    case INT_DUMP_MEMORY: // 0x45
    {
        int addr = cpu->registers[0];
        int length = cpu->registers[1];
        if (addr < 0) addr = 0;
        if (length <= 0) length = 16;
        if (addr >= MEMORY_SIZE) {
            fprintf(stderr, "Warning (INT 0x46): Start address 0x%X out of bounds.\n", addr);
            break;
        }
        if (addr + length > MEMORY_SIZE) {
            length = MEMORY_SIZE - addr;
            fprintf(stderr, "Warning (INT 0x46): Length clamped to %d due to memory bounds.\n", length);
        }


        printf("--- Memory Dump Addr: 0x%X, Len: %d ---\n", addr, length);
        for (int i = 0; i < length; i++) {
            if (i % 16 == 0) {
                if (i != 0) printf("\n");
                printf("%08X: ", addr + i);
            }
            if (isValidMem(addr + i)) {
                printf("%02X ", cpu->memory[addr + i] & 0xFF);
            }
            else {
                printf("?? ");
            }
        }
        printf("\n");
        printf("---------------------------------------\n");
    }
    break;

    case INT_GET_KEY_STATE: // 0x50
    {
        int scancode_reg = 0;
        int result_reg = 0;
        SDL_Scancode scancode = (SDL_Scancode)cpu->registers[scancode_reg];

        SDL_PumpEvents();

        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        if (keystate == NULL) {
            fprintf(stderr, "Error (INT 0x50): SDL_GetKeyboardState returned NULL.\n");
            cpu->registers[result_reg] = 0;
            break;
        }

        if (scancode >= 0 && scancode < SDL_NUM_SCANCODES) {
            cpu->registers[result_reg] = keystate[scancode] ? 1 : 0;
        }
        else {
            cpu->registers[result_reg] = 0;
        }
    }
    break;
    case INT_WAIT_FOR_KEY: // 0x51
    {
        int result_reg = 0;
        SDL_Event event;
        bool key_pressed = false;

        while (!key_pressed && !cpu->shutdown_requested) {
            if (SDL_WaitEventTimeout(&event, 10)) {
                if (event.type == SDL_QUIT) {
                    fprintf(stdout, "SDL_QUIT received during INT 0x51.\n");
                    cpu->registers[result_reg] = -1;
                    cpu->shutdown_requested = true;
                    key_pressed = true;
                }
                else if (event.type == SDL_KEYDOWN) {
                    cpu->registers[result_reg] = event.key.keysym.scancode;
                    key_pressed = true;
                }
            }
            SDL_PumpEvents();
        }
        if (cpu->shutdown_requested && !key_pressed) {
            cpu->registers[result_reg] = -1;
        }
    }
    break;
    case INT_GET_MOUSE_STATE: // 0x52
    {
        int x_reg = 0;
        int y_reg = 1;
        int button_reg = 2;
        int mouse_x, mouse_y;

        SDL_PumpEvents();

        Uint32 button_state = SDL_GetMouseState(&mouse_x, &mouse_y);

        cpu->registers[x_reg] = mouse_x;
        cpu->registers[y_reg] = mouse_y;

        int button_mask = 0;
        if (button_state & SDL_BUTTON(SDL_BUTTON_LEFT))   button_mask |= 1;
        if (button_state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) button_mask |= 2;
        if (button_state & SDL_BUTTON(SDL_BUTTON_RIGHT))  button_mask |= 4;

        cpu->registers[button_reg] = button_mask;
    }
    break;

    case INT_DISK_READ: // 0x60
    {
        int sector_num_reg = 0;
        int mem_addr_reg = 1;
        int num_sectors_reg = 2;
        int status_reg = 0;

        int sector_num = cpu->registers[sector_num_reg];
        int mem_addr = cpu->registers[mem_addr_reg];
        int num_sectors = cpu->registers[num_sectors_reg];
        int status = 0;

        if (!cpu->disk_image_fp) {
            fprintf(stderr, "Error (INT 0x60): Disk image not open.\n");
            status = 3;
            goto disk_read_end;
        }
        if (num_sectors <= 0) {
            status = 0;
            goto disk_read_end;
        }

        long long max_sector = (cpu->disk_image_size / cpu->disk_sector_size) - 1;
        if (sector_num < 0 || sector_num > max_sector || (sector_num + num_sectors - 1) > max_sector) {
            fprintf(stderr, "Error (INT 0x60): Invalid sector range %d to %d (max %lld).\n",
                sector_num, sector_num + num_sectors - 1, max_sector);
            status = 1;
            goto disk_read_end;
        }

        long long total_bytes = (long long)num_sectors * cpu->disk_sector_size;
        if (!isValidMem(mem_addr) || !isValidMem(mem_addr + total_bytes - 1)) {
            fprintf(stderr, "Error (INT 0x60): Invalid memory range 0x%X to 0x%X for %lld bytes.\n",
                mem_addr, mem_addr + (int)total_bytes - 1, total_bytes);
            status = 2;
            goto disk_read_end;
        }

        long long seek_pos = (long long)sector_num * cpu->disk_sector_size;
        if (fseek(cpu->disk_image_fp, seek_pos, SEEK_SET) != 0) {
            perror("Error (INT 0x60): fseek failed");
            status = 3;
            goto disk_read_end;
        }

        char* sector_buffer = malloc(total_bytes);
        if (!sector_buffer) {
            fprintf(stderr, "Error (INT 0x60): Failed to allocate sector buffer (%lld bytes).\n", total_bytes);
            status = 3;
            goto disk_read_end;
        }

        size_t bytes_read = fread(sector_buffer, 1, total_bytes, cpu->disk_image_fp);
        if (bytes_read != (size_t)total_bytes) {
            if (ferror(cpu->disk_image_fp)) {
                perror("Error (INT 0x60): fread failed");
            }
            else {
                fprintf(stderr, "Error (INT 0x60): fread returned short count (%zu != %lld).\n", bytes_read, total_bytes);
            }
            free(sector_buffer);
            status = 3;
            goto disk_read_end;
        }

        for (long long i = 0; i < total_bytes; ++i) {
            if (isValidMem(mem_addr + i)) {
                cpu->memory[mem_addr + i] = (int)(unsigned char)sector_buffer[i];
            }
            else {
                fprintf(stderr, "FATAL Error (INT 0x60): Memory bounds exceeded during copy after fread!.\n");
                status = 2;
                break;
            }
        }
        free(sector_buffer);

    disk_read_end:
        cpu->registers[status_reg] = status;
    }
    break;
    case INT_DISK_WRITE: // 0x61
    {
        int sector_num_reg = 0;
        int mem_addr_reg = 1;
        int num_sectors_reg = 2;
        int status_reg = 0;

        int sector_num = cpu->registers[sector_num_reg];
        int mem_addr = cpu->registers[mem_addr_reg];
        int num_sectors = cpu->registers[num_sectors_reg];
        int status = 0;

        if (!cpu->disk_image_fp) {
            fprintf(stderr, "Error (INT 0x61): Disk image not open.\n");
            status = 3;
            goto disk_write_end;
        }
        if (num_sectors <= 0) {
            status = 0;
            goto disk_write_end;
        }

        long long max_sector = (cpu->disk_image_size / cpu->disk_sector_size) - 1;
        if (sector_num < 0 || sector_num > max_sector || (sector_num + num_sectors - 1) > max_sector) {
            fprintf(stderr, "Error (INT 0x61): Invalid sector range %d to %d (max %lld).\n",
                sector_num, sector_num + num_sectors - 1, max_sector);
            status = 1;
            goto disk_write_end;
        }

        long long total_bytes = (long long)num_sectors * cpu->disk_sector_size;
        if (!isValidMem(mem_addr) || !isValidMem(mem_addr + total_bytes - 1)) {
            fprintf(stderr, "Error (INT 0x61): Invalid memory range 0x%X to 0x%X for %lld bytes.\n",
                mem_addr, mem_addr + (int)total_bytes - 1, total_bytes);
            status = 2;
            goto disk_write_end;
        }

        char* sector_buffer = malloc(total_bytes);
        if (!sector_buffer) {
            fprintf(stderr, "Error (INT 0x61): Failed to allocate sector buffer (%lld bytes).\n", total_bytes);
            status = 3;
            goto disk_write_end;
        }

        for (long long i = 0; i < total_bytes; ++i) {
            if (isValidMem(mem_addr + i)) {
                sector_buffer[i] = (char)(cpu->memory[mem_addr + i] & 0xFF);
            }
            else {
                fprintf(stderr, "FATAL Error (INT 0x61): Memory bounds exceeded during copy before fwrite!.\n");
                free(sector_buffer);
                status = 2;
                goto disk_write_end;
            }
        }

        long long seek_pos = (long long)sector_num * cpu->disk_sector_size;
        if (fseek(cpu->disk_image_fp, seek_pos, SEEK_SET) != 0) {
            perror("Error (INT 0x61): fseek failed");
            free(sector_buffer);
            status = 3;
            goto disk_write_end;
        }

        size_t bytes_written = fwrite(sector_buffer, 1, total_bytes, cpu->disk_image_fp);
        free(sector_buffer);

        if (bytes_written != (size_t)total_bytes) {
            if (ferror(cpu->disk_image_fp)) {
                perror("Error (INT 0x61): fwrite failed");
            }
            else {
                fprintf(stderr, "Error (INT 0x61): fwrite wrote short count (%zu != %lld).\n", bytes_written, total_bytes);
            }
            status = 3;
            fflush(cpu->disk_image_fp);
            goto disk_write_end;
        }

        if (fflush(cpu->disk_image_fp) != 0) {
            perror("Error (INT 0x61): fflush failed after write");
            status = 3;
            goto disk_write_end;
        }

    disk_write_end:
        cpu->registers[status_reg] = status;
    }
    break;
    case INT_DISK_INFO: // 0x62
    {
        int total_sectors_reg = 0;
        int sector_size_reg = 1;

        if (!cpu->disk_image_fp) {
            fprintf(stderr, "Error (INT 0x62): Disk image not open.\n");
            cpu->registers[total_sectors_reg] = -1;
            cpu->registers[sector_size_reg] = 0;
        }
        else {
            long long total_sectors = cpu->disk_image_size / cpu->disk_sector_size;
            if (total_sectors > INT_MAX) {
                fprintf(stderr, "Warning (INT 0x62): Total sector count (%lld) exceeds INT_MAX.\n", total_sectors);
                cpu->registers[total_sectors_reg] = INT_MAX;
            }
            else {
                cpu->registers[total_sectors_reg] = (int)total_sectors;
            }
            cpu->registers[sector_size_reg] = cpu->disk_sector_size;
        }
    }
    break;

    case INT_DISK_FORMAT: // 0x63
    {
        int status_reg = 0;
        int status = 0;

        if (!cpu->disk_image_fp) {
            status = 3;
            goto disk_format_end;
        }

        long long total_sectors = cpu->disk_image_size / cpu->disk_sector_size;
        if (total_sectors <= 0 || cpu->disk_sector_size <= 0) {
            status = 0;
            goto disk_format_end;
        }

        char* zero_buffer = (char*)calloc(cpu->disk_sector_size, 1);
        if (!zero_buffer) {
            status = 3;
            goto disk_format_end;
        }

        if (fseek(cpu->disk_image_fp, 0, SEEK_SET) != 0) {
            free(zero_buffer);
            status = 3;
            goto disk_format_end;
        }

        bool write_error = false;
        for (long long i = 0; i < total_sectors; ++i) {
            size_t bytes_written = fwrite(zero_buffer, 1, cpu->disk_sector_size, cpu->disk_image_fp);
            if (bytes_written != (size_t)cpu->disk_sector_size) {
                write_error = true;
                break;
            }
        }

        free(zero_buffer);

        if (fflush(cpu->disk_image_fp) != 0) {
            status = 3;
        }
        else {
            status = write_error ? 3 : 0;
        }

    disk_format_end:
        if (!isValidReg(status_reg)) {
            fprintf(stderr, "FATAL (INT 0x63): Status register R%d is invalid!\n", status_reg);
            cpu->shutdown_requested = true;
        }
        else {
            cpu->registers[status_reg] = status;
        }
    }
    break;

    default:
        fprintf(stderr, "Error: Unknown interrupt code 0x%X at line %d\n", interrupt_id, cpu->ip + 1);
        cpu->shutdown_requested = true;
        break;
    }
}

void execute(VirtualCPU* cpu, char* program[], int program_size, bool debug_mode) {
    bool running = true;
    while (running && cpu->ip < program_size && !cpu->shutdown_requested) {
        char* current_instruction_line = program[cpu->ip];
        char op_str[10];
        int operands[3] = { 0, 0, 0 };

        if (debug_mode) {
            printf("[DEBUG IP:%d] %s\n", cpu->ip + 1, current_instruction_line);
        }

        int parsed_items = sscanf(current_instruction_line, "%9s", op_str);
        if (parsed_items < 1) {
            fprintf(stderr, "Warning: Empty or invalid instruction format at line %d: '%s'. Skipping.\n", cpu->ip + 1, current_instruction_line);
            cpu->ip++;
            continue;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
        }
        if (!running) break;

        op_str[MAX_LINE_LENGTH];
        char* line_ptr = current_instruction_line;

        while (*line_ptr && isspace((unsigned char)*line_ptr)) {
            line_ptr++;
        }

        int i = 0;
        while (*line_ptr && !isspace((unsigned char)*line_ptr) && *line_ptr != ',' && i < sizeof(op_str) - 1) {
            op_str[i++] = *line_ptr++;
        }
        op_str[i] = '\0';

        InstructionType inst = parseInstruction(op_str);

        int current_ip = cpu->ip;

        switch (inst) {
        case MOV: {
            char operand1_str[64], operand2_str[MAX_LINE_LENGTH];
            int dest_reg = -1;

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error MOV: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                int src_reg = -1;
                int immediate_val = 0;
                bool parsed_operand2 = false;

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error MOV: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false;
                    }
                    else {
                        cpu->registers[dest_reg] = cpu->registers[src_reg];
                        parsed_operand2 = true;
                    }
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    mov(cpu, dest_reg, immediate_val);
                    parsed_operand2 = true;
                }

                if (!parsed_operand2 && running) {
                    fprintf(stderr, "Error: Invalid MOV second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid MOV format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case ADD: {
            int dest_reg = -1;
            int src_reg = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error ADD: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error ADD: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    add(cpu, dest_reg, src_reg);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    int val1 = cpu->registers[dest_reg];
                    int val2 = immediate_val;
                    int result = val1 + val2;
                    if (((val1 > 0 && val2 > 0) && (result < 0)) || ((val1 < 0 && val2 < 0) && (result > 0))) {
                        cpu->flags |= FLAG_OVERFLOW;
                    }
                    else {
                        cpu->flags &= ~FLAG_OVERFLOW;
                    }
                    cpu->registers[dest_reg] = result;
                }
                else {
                    fprintf(stderr, "Error ADD: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid ADD format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case SUB: {
            int dest_reg = -1;
            int src_reg = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error SUB: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error SUB: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    sub(cpu, dest_reg, src_reg);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    int val1 = cpu->registers[dest_reg];
                    int val2 = immediate_val;
                    int result = val1 - val2;
                    if (((val1 > 0 && val2 < 0) && (result < 0)) || ((val1 < 0 && val2 > 0) && (result > 0))) {
                        cpu->flags |= FLAG_OVERFLOW;
                    }
                    else {
                        cpu->flags &= ~FLAG_OVERFLOW;
                    }
                    cpu->registers[dest_reg] = result;
                }
                else {
                    fprintf(stderr, "Error SUB: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid SUB format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case MUL: {
            int dest_reg = -1;
            int src_reg = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error MUL: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error MUL: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    mul(cpu, dest_reg, src_reg);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    long long val1 = (long long)cpu->registers[dest_reg];
                    long long val2 = (long long)immediate_val;
                    long long result = val1 * val2;
                    if (result > INT_MAX || result < INT_MIN) {
                        cpu->flags |= FLAG_OVERFLOW;
                    }
                    else {
                        cpu->flags &= ~FLAG_OVERFLOW;
                    }
                    cpu->registers[dest_reg] = (int)result;
                }
                else {
                    fprintf(stderr, "Error MUL: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid MUL format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case DIV: {
            int dest_reg = -1;
            int src_reg = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error DIV: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error DIV: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    divi(cpu, dest_reg, src_reg);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    int val1 = cpu->registers[dest_reg];
                    int val2 = immediate_val;
                    if (val2 == 0) {
                        fprintf(stderr, "Error: Division by zero (DIV R%d, %d) at line %d.\n", dest_reg, val2, cpu->ip + 1);
                        cpu->flags &= ~FLAG_OVERFLOW;
                        running = false; break;
                    }
                    if (val1 == INT_MIN && val2 == -1) {
                        cpu->flags |= FLAG_OVERFLOW;
                        cpu->registers[dest_reg] = val1 / val2;
                    }
                    else {
                        cpu->flags &= ~FLAG_OVERFLOW;
                        cpu->registers[dest_reg] = val1 / val2;
                    }
                }
                else {
                    fprintf(stderr, "Error DIV: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid DIV format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case MOD: {
            int dest_reg = -1;
            int src_reg = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error MOD: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error MOD: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    mod_op(cpu, dest_reg, src_reg);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    if (immediate_val != 0) {
                        cpu->registers[dest_reg] %= immediate_val;
                    }
                    else {
                        fprintf(stderr, "Error: Modulo by zero (MOD R%d, %d) at line %d.\n", dest_reg, immediate_val, cpu->ip + 1);
                        running = false; break;
                    }
                }
                else {
                    fprintf(stderr, "Error MOD: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid MOD format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case POW: {
            int dest_reg = -1;
            int src_reg = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error POW: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error POW: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    pow_op(cpu, dest_reg, src_reg);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    long long base = cpu->registers[dest_reg];
                    int exponent = immediate_val;
                    long long result_ll = 1;

                    cpu->flags &= ~FLAG_OVERFLOW;

                    if (exponent < 0) {
                        fprintf(stderr, "Warning (POW): Negative immediate exponent %d at line %d. Result 0 (unless base +/-1).\n", exponent, cpu->ip + 1);
                        if (base == 1) result_ll = 1;
                        else if (base == -1) result_ll = ((exponent % 2) == 0) ? 1 : -1;
                        else result_ll = 0;
                    }
                    else if (exponent == 0) {
                        result_ll = 1;
                    }
                    else if (base == 0) {
                        result_ll = 0;
                    }
                    else {
                        for (int i = 0; i < exponent; ++i) {
                            long long abs_base = llabs(base);
                            long long abs_res = llabs(result_ll);
                            if (abs_base > 0 && abs_res > 0 && abs_res > LLONG_MAX / abs_base) {
                                cpu->flags |= FLAG_OVERFLOW;
                                fprintf(stderr, "Warning (POW): Overflow during immediate power %lld^%d at line %d.\n", base, exponent, cpu->ip + 1);
                                result_ll = (base > 0 || (exponent % 2 == 0)) ? INT_MAX : INT_MIN;
                                break;
                            }
                            result_ll *= base;
                        }
                    }

                    if (result_ll > INT_MAX) {
                        cpu->registers[dest_reg] = INT_MAX;
                        if (!(cpu->flags & FLAG_OVERFLOW)) {
                            fprintf(stderr, "Warning (POW): Result clamped to INT_MAX at line %d.\n", cpu->ip + 1);
                            cpu->flags |= FLAG_OVERFLOW;
                        }
                    }
                    else if (result_ll < INT_MIN) {
                        cpu->registers[dest_reg] = INT_MIN;
                        if (!(cpu->flags & FLAG_OVERFLOW)) {
                            fprintf(stderr, "Warning (POW): Result clamped to INT_MIN at line %d.\n", cpu->ip + 1);
                            cpu->flags |= FLAG_OVERFLOW;
                        }
                    }
                    else {
                        cpu->registers[dest_reg] = (int)result_ll;
                    }
                }
                else {
                    fprintf(stderr, "Error POW: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid POW format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case ABS:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                abs_op(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid ABS format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case LOOP:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                loop_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid LOOP format at line %d (Expected: LOOP Rx, <line_number>)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case INTR:
            if (sscanf(current_instruction_line, "%*s %x", &operands[0]) == 1) {
                interrupt(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid INT format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case NOP:
            nop(cpu);
            break;
        case HLT:
            hlt(cpu, &running);
            break;
        case NOT:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                not_op(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid NOT format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case AND: {
            int dest_reg = -1;
            int src_reg = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error AND: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error AND: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    and_op(cpu, dest_reg, src_reg);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    cpu->registers[dest_reg] &= immediate_val;
                }
                else {
                    fprintf(stderr, "Error AND: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid AND format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case OR: {
            int dest_reg = -1;
            int src_reg = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error OR: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error OR: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    or_op(cpu, dest_reg, src_reg);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    cpu->registers[dest_reg] |= immediate_val;
                }
                else {
                    fprintf(stderr, "Error OR: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid OR format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case XOR: {
            int dest_reg = -1;
            int src_reg = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error XOR: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &src_reg) == 1) {
                    if (!isValidReg(src_reg)) {
                        fprintf(stderr, "Error XOR: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    xor_op(cpu, dest_reg, src_reg);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    cpu->registers[dest_reg] ^= immediate_val;
                }
                else {
                    fprintf(stderr, "Error XOR: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid XOR format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case SHL:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                shl(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid SHL format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case SHR:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                shr(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid SHR format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case ROL:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                rol(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid ROL format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case ROR:
            if (sscanf(current_instruction_line, "%*s R%d, %d", &operands[0], &operands[1]) == 2) {
                ror(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid ROR format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case JMP:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                jmp(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid JMP format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CMP: {
            int reg1 = -1;
            int reg2 = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &reg1, operand2_str) == 2) {
                if (!isValidReg(reg1)) {
                    fprintf(stderr, "Error CMP: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &reg2) == 1) {
                    if (!isValidReg(reg2)) {
                        fprintf(stderr, "Error CMP: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    cmp(cpu, reg1, reg2);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    int val1 = cpu->registers[reg1];
                    int val2 = immediate_val;
                    int result = val1 - val2;
                    if (((val1 > 0 && val2 < 0) && (result < 0)) || ((val1 < 0 && val2 > 0) && (result > 0))) {
                        cpu->flags = (cpu->flags & ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS)) | FLAG_OVERFLOW;
                    }
                    else {
                        cpu->flags &= ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS | FLAG_OVERFLOW);
                    }
                    if (val1 == val2) { cpu->flags |= FLAG_ZERO; }
                    else if (val1 > val2) { cpu->flags |= FLAG_GREATER; }
                    else { cpu->flags |= FLAG_LESS; }
                }
                else {
                    fprintf(stderr, "Error CMP: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid CMP format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case JMPNE:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_ZERO) == 0) { jmp(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid JNE format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case JMPE:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_ZERO) != 0) { jmp(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid JMPE format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case JMPH:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_GREATER) != 0) { jmp(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid JMPH format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case JMPL:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_LESS) != 0) { jmp(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid JMPL format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case NEG:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                neg(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid NEG format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case INC:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                inc(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid INC format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case DEC:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                dec(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid DEC format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case XCHG:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                xchg(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid XCHG format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CLR:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                clr(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid CLR format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case PUSH:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                push(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid PUSH format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case POP:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                pop(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid POP format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CALL:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                call(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid CALL format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case RET:
            ret(cpu);
            break;
        case CALLH:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_GREATER) != 0) { call(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid CALLH format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CALLL:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_LESS) != 0) { call(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid CALLL format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CALLE:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_ZERO) != 0) { call(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid CALLE format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CALLNE:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_ZERO) == 0) { call(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid CALLNE format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CALLO:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_OVERFLOW) != 0) { call(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid CALLO format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CALLNO:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_OVERFLOW) == 0) { call(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid CALLNO format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CALLHE:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_LESS) == 0) { call(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid CALLHE format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case CALLLE:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_GREATER) == 0) { call(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid CALLLE format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case RND:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                rnd(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid RND format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case STRMOV: {
            char* args_start = strchr(current_instruction_line, ' ');
            if (!args_start) {
                fprintf(stderr, "Error: Invalid STRMOV format (no arguments) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false; break;
            }
            args_start++;
            char* end_ptr_addr;
            long parsed_addr = strtol(args_start, &end_ptr_addr, 10);
            if (end_ptr_addr == args_start) {
                fprintf(stderr, "Error: Invalid STRMOV format (cannot parse address) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false; break;
            }
            int addr = (int)parsed_addr;
            char* comma_ptr = strchr(end_ptr_addr, ',');
            if (!comma_ptr) {
                fprintf(stderr, "Error: Invalid STRMOV format (missing comma) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false; break;
            }
            char* quote_start = strchr(comma_ptr + 1, '"');
            if (!quote_start) {
                fprintf(stderr, "Error: Invalid STRMOV format (missing opening quote) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false; break;
            }
            quote_start++;
            char* quote_end = strchr(quote_start, '"');
            if (!quote_end) {
                fprintf(stderr, "Error: Invalid STRMOV format (missing closing quote) at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false; break;
            }
            size_t str_len = quote_end - quote_start;
            if (addr >= 0 && addr < MEMORY_SIZE) {
                if (addr + str_len >= MEMORY_SIZE) {
                    fprintf(stderr, "Error: STRMOV destination out of bounds (addr=%d, len=%zu) at line %d\n", addr, str_len, cpu->ip + 1);
                    running = false; break;
                }
                for (size_t i = 0; i < str_len; i++) {
                    if (quote_start + i >= current_instruction_line + strlen(current_instruction_line)) {
                        fprintf(stderr, "Error: Internal parsing error reading STRMOV string at line %d\n", cpu->ip + 1);
                        running = false; break;
                    }
                    cpu->memory[addr + i] = *(quote_start + i);
                }
                if (!running) break;
                cpu->memory[addr + str_len] = 0;
            }
            else {
                fprintf(stderr, "Error: Invalid STRMOV memory address %d at line %d\n", addr, cpu->ip + 1);
                running = false;
            }
        } break;
        case LOAD:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                load_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid LOAD format at line %d (Expected: LOAD Rdest, Raddr_src)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case STORE: {
            int val_src_reg = -1;
            int addr_dest_reg = -1;
            int immediate_val = 0;
            char operand1_str[MAX_LINE_LENGTH];
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s %[^,], %s", operand1_str, operand2_str) == 2) {
                if (!((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &addr_dest_reg) == 1 && isValidReg(addr_dest_reg))) {
                    fprintf(stderr, "Error STORE: Invalid destination address register '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand1_str[0] == 'R' || operand1_str[0] == 'r') && sscanf(operand1_str, "R%d", &val_src_reg) == 1) {
                    if (!isValidReg(val_src_reg)) {
                        fprintf(stderr, "Error STORE: Invalid source value register '%s' at line %d\n", operand1_str, cpu->ip + 1);
                        running = false; break;
                    }
                    store_op(cpu, val_src_reg, addr_dest_reg);
                }
                else if (sscanf(operand1_str, "%i", &immediate_val) == 1) {
                    int address = cpu->registers[addr_dest_reg];
                    if (!isValidMem(address)) {
                        fprintf(stderr, "Error STORE: Invalid memory address %d (0x%X) in R%d at line %d\n", address, address, addr_dest_reg, cpu->ip + 1);
                        running = false; break;
                    }
                    cpu->memory[address] = immediate_val;
                }
                else {
                    fprintf(stderr, "Error STORE: Invalid source operand '%s' at line %d\n", operand1_str, cpu->ip + 1);
                    running = false; break;
                }

            }
            else {
                fprintf(stderr, "Error: Invalid STORE format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case TEST: {
            int reg1 = -1;
            int reg2 = -1;
            int immediate_val = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &reg1, operand2_str) == 2) {
                if (!isValidReg(reg1)) {
                    fprintf(stderr, "Error TEST: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &reg2) == 1) {
                    if (!isValidReg(reg2)) {
                        fprintf(stderr, "Error TEST: Invalid source register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    test_op(cpu, reg1, reg2);
                }
                else if (sscanf(operand2_str, "%i", &immediate_val) == 1) {
                    int val1 = cpu->registers[reg1];
                    int val2 = immediate_val;
                    int result = val1 & val2;
                    cpu->flags &= ~(FLAG_ZERO | FLAG_OVERFLOW);
                    if (result == 0) {
                        cpu->flags |= FLAG_ZERO;
                    }
                }
                else {
                    fprintf(stderr, "Error TEST: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid TEST format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case LEA: {
            int dest_reg = -1, base_reg = -1, offset = 0;
            char base_reg_str[10];
            if (sscanf(current_instruction_line, "%*s R%d, %[^,], %d", &dest_reg, base_reg_str, &offset) == 3) {
                if (sscanf(base_reg_str, "R%d", &base_reg) == 1) {
                    lea_op(cpu, dest_reg, base_reg, offset);
                }
                else {
                    fprintf(stderr, "Error: Invalid LEA base register format '%s' at line %d\n", base_reg_str, cpu->ip + 1);
                    running = false;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid LEA format at line %d (Expected: LEA Rdest, Rbase, offset)\n", cpu->ip + 1);
                running = false;
            }
            break;
        }
        case PUSHF:
            pushf_op(cpu);
            break;
        case POPF:
            popf_op(cpu);
            break;
        case SETF:
            if (sscanf(current_instruction_line, "%*s %x", &operands[0]) == 1) {
                setf_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid SETF format at line %d (Expected: SETF <hex_mask>)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case CLRF:
            if (sscanf(current_instruction_line, "%*s %x", &operands[0]) == 1) {
                clrf_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid CLRF format at line %d (Expected: CLRF <hex_mask> or CLRF 0)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case BT:
        case BSET:
        case BCLR:
        case BTOG: {
            int dest_reg = -1;
            char operand2_str[MAX_LINE_LENGTH];
            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error %s: Invalid destination register R%d at line %d\n", op_str, dest_reg, cpu->ip + 1);
                    running = false; break;
                }
                int bit_source_reg = -1;
                int immediate_bit_index = -1;
                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &bit_source_reg) == 1) {
                    if (!isValidReg(bit_source_reg)) {
                        fprintf(stderr, "Error %s: Invalid bit source register %s at line %d\n", op_str, operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    immediate_bit_index = cpu->registers[bit_source_reg];
                }
                else if (sscanf(operand2_str, "%d", &immediate_bit_index) == 1) {
                    /* Immediate mode, index is already set */
                }
                else {
                    fprintf(stderr, "Error %s: Invalid second operand '%s' at line %d\n", op_str, operand2_str, cpu->ip + 1);
                    running = false; break;
                }
                if (immediate_bit_index < 0 || immediate_bit_index >= 32) {
                    fprintf(stderr, "Error %s: Invalid bit index %d (must be 0-31) at line %d\n", op_str, immediate_bit_index, cpu->ip + 1);
                    running = false; break;
                }
                switch (inst) {
                case BT:   bt_op(cpu, dest_reg, immediate_bit_index); break;
                case BSET: bset_op(cpu, dest_reg, immediate_bit_index); break;
                case BCLR: bclr_op(cpu, dest_reg, immediate_bit_index); break;
                case BTOG: btog_op(cpu, dest_reg, immediate_bit_index); break;
                default: fprintf(stderr, "Error: Reached bit op handler with invalid instruction %d\n", inst); running = false; break;
                }
            }
            else {
                fprintf(stderr, "Error: Invalid %s format structure at line %d: '%s'\n", op_str, cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case STRCMP:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                strcmp_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid STRCMP format at line %d (Expected: STRCMP Raddr1, Raddr2)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case STRLEN:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                strlen_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid STRLEN format at line %d (Expected: STRLEN Rdest, Raddr)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case STRCPY:
            if (sscanf(current_instruction_line, "%*s R%d, R%d", &operands[0], &operands[1]) == 2) {
                strcpy_op(cpu, operands[0], operands[1]);
            }
            else {
                fprintf(stderr, "Error: Invalid STRCPY format at line %d (Expected: STRCPY Rdest_addr, Rsrc_addr)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case MEMCPY:
            if (sscanf(current_instruction_line, "%*s R%d, R%d, R%d", &operands[0], &operands[1], &operands[2]) == 3) {
                memcpy_op(cpu, operands[0], operands[1], operands[2]);
            }
            else {
                fprintf(stderr, "Error: Invalid MEMCPY format at line %d (Expected: MEMCPY Rdest_addr, Rsrc_addr, Rlen)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case MEMSET:
            if (sscanf(current_instruction_line, "%*s R%d, R%d, R%d", &operands[0], &operands[1], &operands[2]) == 3) {
                memset_op(cpu, operands[0], operands[1], operands[2]);
            }
            else {
                fprintf(stderr, "Error: Invalid MEMSET format at line %d (Expected: MEMSET Rdest_addr, Rval, Rlen)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case CPUID:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                cpuid_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid CPUID format at line %d (Expected: CPUID Rdest)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case BSWAP:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                bswap_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid BSWAP format at line %d (Expected: BSWAP Rx)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case SAR: {
            int dest_reg = -1;
            int source_reg = -1;
            int count = 0;
            char operand2_str[MAX_LINE_LENGTH];

            if (sscanf(current_instruction_line, "%*s R%d, %s", &dest_reg, operand2_str) == 2) {
                if (!isValidReg(dest_reg)) {
                    fprintf(stderr, "Error SAR: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'R' || operand2_str[0] == 'r') && sscanf(operand2_str, "R%d", &source_reg) == 1) {
                    if (!isValidReg(source_reg)) {
                        fprintf(stderr, "Error SAR: Invalid count register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    count = cpu->registers[source_reg];
                }
                else if (sscanf(operand2_str, "%d", &count) == 1) {

                }
                else {
                    fprintf(stderr, "Error SAR: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false; break;
                }

                if (count < 0) {
                    fprintf(stderr, "Warning SAR: Negative shift count %d at line %d. Treating as 0.\n", count, cpu->ip + 1);
                    count = 0;
                }

                sar_op(cpu, dest_reg, count);

            }
            else {
                fprintf(stderr, "Error: Invalid SAR format structure at line %d: '%s'. Expected: SAR Rdest, Rcount or SAR Rdest, count\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case RVD:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                rvd_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid RVD format at line %d (Expected: RVD Rx)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case INC_MEM:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                inc_mem_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid INC_MEM format at line %d (Expected: INC_MEM Rx)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case DEC_MEM:
            if (sscanf(current_instruction_line, "%*s R%d", &operands[0]) == 1) {
                dec_mem_op(cpu, operands[0]);
            }
            else {
                fprintf(stderr, "Error: Invalid DEC_MEM format at line %d (Expected: DEC_MEM Rx)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case JMPO:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_OVERFLOW) != 0) { jmp(cpu, operands[0]); }
            }
            else {
                fprintf(stderr, "Error: Invalid JMPO format at line %d (Expected: JO <line_number/label>)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case JMPNO:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_OVERFLOW) == 0) { jmp(cpu, operands[0]); }
            }
            else {
                fprintf(stderr, "Error: Invalid JMPNO format at line %d (Expected: JNO <line_number/label>)\n", cpu->ip + 1);
                running = false;
            }
            break;
        case JMPHE:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_LESS) == 0) { jmp(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid JMPHE format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case JMPLE:
            if (sscanf(current_instruction_line, "%*s %d", &operands[0]) == 1) {
                if ((cpu->flags & FLAG_GREATER) == 0) { jmp(cpu, operands[0]); }
            }
            else { fprintf(stderr, "Error: Invalid JMPLE format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case FMOV: {
            int dest_freg = -1;
            int src_freg = -1;
            char operand2_str[MAX_LINE_LENGTH];
            double immediate_val = 0.0;
            char* endptr = NULL;

            if (sscanf(current_instruction_line, "%*s F%d, F%d", &dest_freg, &src_freg) == 2) {
                fmov_reg(cpu, dest_freg, src_freg);
            }
            else if (sscanf(current_instruction_line, "%*s F%d, %s", &dest_freg, operand2_str) == 2) {
                immediate_val = strtod(operand2_str, &endptr);
                if (operand2_str == endptr || *endptr != '\0') {
                    fprintf(stderr, "Error: Invalid FMOV immediate float value '%s' at line %d\n", operand2_str, cpu->ip + 1);
                    running = false;
                }
                else {
                    fmov_imm(cpu, dest_freg, immediate_val);
                }
            }
            else {
                fprintf(stderr, "Error: Invalid FMOV format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case FADD: {
            int dest_freg = -1;
            int src_freg = -1;
            double immediate_val = 0.0;
            char operand2_str[MAX_LINE_LENGTH];
            char* endptr = NULL;

            if (sscanf(current_instruction_line, "%*s F%d, %s", &dest_freg, operand2_str) == 2) {
                if (!isValidFReg(dest_freg)) {
                    fprintf(stderr, "Error FADD: Invalid destination F register F%d\n", dest_freg);
                    running = false; break;
                }

                if ((operand2_str[0] == 'F' || operand2_str[0] == 'f') && sscanf(operand2_str, "F%d", &src_freg) == 1) {
                    if (!isValidFReg(src_freg)) {
                        fprintf(stderr, "Error FADD: Invalid source F register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    fadd(cpu, dest_freg, src_freg);
                }
                else {
                    immediate_val = strtod(operand2_str, &endptr);
                    if (operand2_str != endptr && *endptr == '\0') {
                        double result = cpu->f_registers[dest_freg] + immediate_val;
                        if (isinf(result)) { cpu->flags |= FLAG_OVERFLOW; }
                        else { cpu->flags &= ~FLAG_OVERFLOW; }
                        cpu->f_registers[dest_freg] = result;
                    }
                    else {
                        fprintf(stderr, "Error FADD: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                }
            }
            else {
                fprintf(stderr, "Error: Invalid FADD format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case FSUB: {
            int dest_freg = -1;
            int src_freg = -1;
            double immediate_val = 0.0;
            char operand2_str[MAX_LINE_LENGTH];
            char* endptr = NULL;

            if (sscanf(current_instruction_line, "%*s F%d, %s", &dest_freg, operand2_str) == 2) {
                if (!isValidFReg(dest_freg)) { fprintf(stderr, "Error FSUB: Invalid dest F%d\n", dest_freg); running = false; break; }

                if ((operand2_str[0] == 'F' || operand2_str[0] == 'f') && sscanf(operand2_str, "F%d", &src_freg) == 1) {
                    if (!isValidFReg(src_freg)) { fprintf(stderr, "Error FSUB: Invalid src F%s\n", operand2_str); running = false; break; }
                    fsub(cpu, dest_freg, src_freg);
                }
                else {
                    immediate_val = strtod(operand2_str, &endptr);
                    if (operand2_str != endptr && *endptr == '\0') {
                        double result = cpu->f_registers[dest_freg] - immediate_val;
                        if (isinf(result)) { cpu->flags |= FLAG_OVERFLOW; }
                        else { cpu->flags &= ~FLAG_OVERFLOW; }
                        cpu->f_registers[dest_freg] = result;
                    }
                    else { fprintf(stderr, "Error FSUB: Invalid second op '%s'\n", operand2_str); running = false; break; }
                }
            }
            else { fprintf(stderr, "Error: Invalid FSUB format\n"); running = false; }
            break;
        }
        case FMUL: {
            int dest_freg = -1;
            int src_freg = -1;
            double immediate_val = 0.0;
            char operand2_str[MAX_LINE_LENGTH];
            char* endptr = NULL;

            if (sscanf(current_instruction_line, "%*s F%d, %s", &dest_freg, operand2_str) == 2) {
                if (!isValidFReg(dest_freg)) { fprintf(stderr, "Error FMUL: Invalid dest F%d\n", dest_freg); running = false; break; }

                if ((operand2_str[0] == 'F' || operand2_str[0] == 'f') && sscanf(operand2_str, "F%d", &src_freg) == 1) {
                    if (!isValidFReg(src_freg)) { fprintf(stderr, "Error FMUL: Invalid src F%s\n", operand2_str); running = false; break; }
                    fmul(cpu, dest_freg, src_freg);
                }
                else {
                    immediate_val = strtod(operand2_str, &endptr);
                    if (operand2_str != endptr && *endptr == '\0') {
                        double result = cpu->f_registers[dest_freg] * immediate_val;
                        if (isinf(result)) { cpu->flags |= FLAG_OVERFLOW; }
                        else { cpu->flags &= ~FLAG_OVERFLOW; }
                        cpu->f_registers[dest_freg] = result;
                    }
                    else { fprintf(stderr, "Error FMUL: Invalid second op '%s'\n", operand2_str); running = false; break; }
                }
            }
            else { fprintf(stderr, "Error: Invalid FMUL format\n"); running = false; }
            break;
        }
        case FDIV: {
            int dest_freg = -1;
            int src_freg = -1;
            double immediate_val = 0.0;
            char operand2_str[MAX_LINE_LENGTH];
            char* endptr = NULL;

            if (sscanf(current_instruction_line, "%*s F%d, %s", &dest_freg, operand2_str) == 2) {
                if (!isValidFReg(dest_freg)) { fprintf(stderr, "Error FDIV: Invalid dest F%d\n", dest_freg); running = false; break; }

                if ((operand2_str[0] == 'F' || operand2_str[0] == 'f') && sscanf(operand2_str, "F%d", &src_freg) == 1) {
                    if (!isValidFReg(src_freg)) { fprintf(stderr, "Error FDIV: Invalid src F%s\n", operand2_str); running = false; break; }
                    fdiv(cpu, dest_freg, src_freg);
                }
                else {
                    immediate_val = strtod(operand2_str, &endptr);
                    if (operand2_str != endptr && *endptr == '\0') {
                        double divisor = immediate_val;
                        if (divisor == 0.0) {
                            fprintf(stderr, "Error: Float division by zero (FDIV F%d, %f)\n", dest_freg, divisor);
                            cpu->f_registers[dest_freg] = INFINITY;
                            cpu->flags |= FLAG_OVERFLOW;
                            running = false; break;
                        }
                        double result = cpu->f_registers[dest_freg] / divisor;
                        if (isinf(result)) { cpu->flags |= FLAG_OVERFLOW; }
                        else { cpu->flags &= ~FLAG_OVERFLOW; }
                        cpu->f_registers[dest_freg] = result;
                    }
                    else { fprintf(stderr, "Error FDIV: Invalid second op '%s'\n", operand2_str); running = false; break; }
                }
            }
            else { fprintf(stderr, "Error: Invalid FDIV format\n"); running = false; }
            break;
        }
        case FCMP: {
            int freg1 = -1;
            int freg2 = -1;
            double immediate_val = 0.0;
            char operand2_str[MAX_LINE_LENGTH];
            char* endptr = NULL;

            if (sscanf(current_instruction_line, "%*s F%d, %s", &freg1, operand2_str) == 2) {
                if (!isValidFReg(freg1)) { fprintf(stderr, "Error FCMP: Invalid F%d\n", freg1); running = false; break; }

                if ((operand2_str[0] == 'F' || operand2_str[0] == 'f') && sscanf(operand2_str, "F%d", &freg2) == 1) {
                    if (!isValidFReg(freg2)) { fprintf(stderr, "Error FCMP: Invalid F%s\n", operand2_str); running = false; break; }
                    fcmp(cpu, freg1, freg2);
                }
                else {
                    immediate_val = strtod(operand2_str, &endptr);
                    if (operand2_str != endptr && *endptr == '\0') {
                        double val1 = cpu->f_registers[freg1];
                        double val2 = immediate_val;
                        cpu->flags &= ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS | FLAG_OVERFLOW);
                        if (fabs(val1 - val2) < FPU_EPSILON) { cpu->flags |= FLAG_ZERO; }
                        else if (val1 > val2) { cpu->flags |= FLAG_GREATER; }
                        else { cpu->flags |= FLAG_LESS; }
                    }
                    else { fprintf(stderr, "Error FCMP: Invalid second op '%s'\n", operand2_str); running = false; break; }
                }
            }
            else { fprintf(stderr, "Error: Invalid FCMP format\n"); running = false; }
            break;
        }
        case FABS:
            if (sscanf(current_instruction_line, "%*s F%d", &operands[0]) == 1) {
                fabs_op(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid FABS format\n"); running = false; }
            break;
        case FNEG:
            if (sscanf(current_instruction_line, "%*s F%d", &operands[0]) == 1) {
                fneg_op(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid FNEG format\n"); running = false; }
            break;
        case FSQRT:
            if (sscanf(current_instruction_line, "%*s F%d", &operands[0]) == 1) {
                fsqrt(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid FSQRT format\n"); running = false; }
            break;
        case CVTIF:
            if (sscanf(current_instruction_line, "%*s F%d, R%d", &operands[0], &operands[1]) == 2) {
                cvtif(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid CVTIF format\n"); running = false; }
            break;
        case CVTFI:
            if (sscanf(current_instruction_line, "%*s R%d, F%d", &operands[0], &operands[1]) == 2) {
                cvtfi(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid CVTFI format\n"); running = false; }
            break;
        case FLOAD:
            if (sscanf(current_instruction_line, "%*s F%d, R%d", &operands[0], &operands[1]) == 2) {
                fload(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid FLOAD format\n"); running = false; }
            break;
        case FSTORE:
            if (sscanf(current_instruction_line, "%*s R%d, F%d", &operands[0], &operands[1]) == 2) {
                fstore(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid FSTORE format\n"); running = false; }
            break;
        case FINC:
            if (sscanf(current_instruction_line, "%*s F%d", &operands[0]) == 1) {
                finc(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid FINC format\n"); running = false; }
            break;
        case FDEC:
            if (sscanf(current_instruction_line, "%*s F%d", &operands[0]) == 1) {
                fdec(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid FDEC format\n"); running = false; }
            break;
        case FPUSH:
            if (sscanf(current_instruction_line, "%*s F%d", &operands[0]) == 1) {
                fpush(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid FPUSH format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case FPOP:
            if (sscanf(current_instruction_line, "%*s F%d", &operands[0]) == 1) {
                fpop(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid FPOP format at line %d\n", cpu->ip + 1); running = false; }
            break;
        case FCLR:
            if (sscanf(current_instruction_line, "%*s F%d", &operands[0]) == 1) {
                fclr(cpu, operands[0]);
            }
            else { fprintf(stderr, "Error: Invalid FCLR format\n"); running = false; }
            break;
        case FXCHG:
            if (sscanf(current_instruction_line, "%*s F%d, F%d", &operands[0], &operands[1]) == 2) {
                fxchg(cpu, operands[0], operands[1]);
            }
            else { fprintf(stderr, "Error: Invalid FXCHG format\n"); running = false; }
            break;
        case FPOW: {
            int dest_freg = -1;
            int src_freg = -1;
            double immediate_val = 0.0;
            char operand2_str[MAX_LINE_LENGTH];
            char* endptr = NULL;

            if (sscanf(current_instruction_line, "%*s F%d, %s", &dest_freg, operand2_str) == 2) {
                if (!isValidFReg(dest_freg)) {
                    fprintf(stderr, "Error FPOW: Invalid destination F register F%d at line %d\n", dest_freg, cpu->ip + 1);
                    running = false; break;
                }

                if ((operand2_str[0] == 'F' || operand2_str[0] == 'f') && sscanf(operand2_str, "F%d", &src_freg) == 1) {
                    if (!isValidFReg(src_freg)) {
                        fprintf(stderr, "Error FPOW: Invalid source F register %s at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                    fpow_op(cpu, dest_freg, src_freg);
                }
                else {
                    immediate_val = strtod(operand2_str, &endptr);
                    if (operand2_str != endptr && *endptr == '\0') {
                        double base = cpu->f_registers[dest_freg];
                        double exponent = immediate_val;
                        double result = pow(base, exponent);

                        cpu->flags &= ~FLAG_OVERFLOW;
                        if (isinf(result) || isnan(result)) {
                            cpu->flags |= FLAG_OVERFLOW;
                            if (isnan(result)) {
                                fprintf(stderr, "Warning FPOW: Immediate result is NaN at line %d.\n", cpu->ip + 1);
                            }
                            else {
                                fprintf(stderr, "Warning FPOW: Immediate result is Infinity at line %d.\n", cpu->ip + 1);
                            }
                        }
                        cpu->f_registers[dest_freg] = result;
                    }
                    else {
                        fprintf(stderr, "Error FPOW: Invalid second operand '%s' at line %d\n", operand2_str, cpu->ip + 1);
                        running = false; break;
                    }
                }
            }
            else {
                fprintf(stderr, "Error: Invalid FPOW format structure at line %d: '%s'\n", cpu->ip + 1, current_instruction_line);
                running = false;
            }
            break;
        }
        case IF: {
            char reg1_str[64], operator_str[10], val_str[MAX_LINE_LENGTH];

            if (parse_condition_operands(line_ptr, reg1_str, operator_str, val_str, sizeof(val_str), cpu->ip + 1) == -1) {
                running = false;
                break;
            }

            int condition_is_true = evaluate_condition(cpu, reg1_str, operator_str, val_str, cpu->ip + 1);

            if (condition_is_true == -1) {
                running = false;
                break;
            }

            if (condition_is_true == 0) { // Condition is FALSE

                int nested_level = 0;
                int scan_ip = cpu->ip + 1;
                bool found_else = false;
                bool found_end = false;

                while (scan_ip < program_size) {
                    char scan_op_str_buf[MAX_LINE_LENGTH];
                    char* scan_line = program[scan_ip];
                    if (!scan_line) { scan_ip++; continue; }

                    char* scan_ptr = scan_line;
                    while (*scan_ptr && isspace((unsigned char)*scan_ptr)) scan_ptr++;
                    int scan_op_idx = 0;
                    while (*scan_ptr && !isspace((unsigned char)*scan_ptr) && *scan_ptr != ',' && scan_op_idx < sizeof(scan_op_str_buf) - 1) {
                        scan_op_str_buf[scan_op_idx++] = toupper((unsigned char)*scan_ptr++);
                    }
                    scan_op_str_buf[scan_op_idx] = '\0';

                    InstructionType scanned_inst = parseInstruction(scan_op_str_buf);

                    if (scanned_inst == IF || scanned_inst == WHILE) {
                        nested_level++;
                    }
                    else if (scanned_inst == ELSE && nested_level == 0) {
                        found_else = true;
                        cpu->ip = scan_ip;
                        break;
                    }
                    else if (scanned_inst == END) {
                        if (nested_level > 0) {
                            nested_level--;
                        }
                        else {
                            found_end = true;
                            cpu->ip = scan_ip;
                            break;
                        }
                    }
                    scan_ip++;
                }

                if (!found_else && !found_end) {
                    fprintf(stderr, "Error: Matching END not found for IF starting at line %d. Execution halted.\n", current_ip + 1);
                    running = false;
                }
                else if (found_else)
                {


                }
                else
                {

                }
            }
            break;
        }
        case ELSE: {
            int nested_level = 0;
            int scan_ip = cpu->ip + 1;
            bool found_end = false;

            while (scan_ip < program_size) {
                char scan_op_str_buf[MAX_LINE_LENGTH];
                char* scan_line = program[scan_ip];
                if (!scan_line) { scan_ip++; continue; }

                char* scan_ptr = scan_line;
                while (*scan_ptr && isspace((unsigned char)*scan_ptr)) scan_ptr++;
                int scan_op_idx = 0;
                while (*scan_ptr && !isspace((unsigned char)*scan_ptr) && *scan_ptr != ',' && scan_op_idx < sizeof(scan_op_str_buf) - 1) {
                    scan_op_str_buf[scan_op_idx++] = toupper((unsigned char)*scan_ptr++);
                }
                scan_op_str_buf[scan_op_idx] = '\0';

                InstructionType scanned_inst = parseInstruction(scan_op_str_buf);

                if (scanned_inst == IF || scanned_inst == WHILE) {
                    nested_level++;
                }
                else if (scanned_inst == END) {
                    if (nested_level > 0) {
                        nested_level--;
                    }
                    else {
                        cpu->ip = scan_ip;
                        found_end = true;
                        break;
                    }
                }
                scan_ip++;
            }

            if (!found_end) {
                fprintf(stderr, "Error: Matching END not found for ELSE at line %d. Execution halted.\n", cpu->ip + 1);
                running = false;
            }
            break;
        }

        case WHILE: {
            char reg1_str[64], operator_str[10], val_str[MAX_LINE_LENGTH];

            if (parse_condition_operands(line_ptr, reg1_str, operator_str, val_str, sizeof(val_str), cpu->ip + 1) == -1) {
                running = false;
                break;
            }

            int condition_is_true = evaluate_condition(cpu, reg1_str, operator_str, val_str, cpu->ip + 1);

            if (condition_is_true == -1) {
                running = false;
                break;
            }

            if (condition_is_true == 0) {
                int nested_level = 0;
                int scan_ip = cpu->ip + 1;
                bool found_end = false;

                while (scan_ip < program_size) {
                    char scan_op_str_buf[MAX_LINE_LENGTH];
                    char* scan_line = program[scan_ip];
                    if (!scan_line) { scan_ip++; continue; }

                    char* scan_ptr = scan_line;
                    while (*scan_ptr && isspace((unsigned char)*scan_ptr)) scan_ptr++;
                    int scan_op_idx = 0;
                    while (*scan_ptr && !isspace((unsigned char)*scan_ptr) && *scan_ptr != ',' && scan_op_idx < sizeof(scan_op_str_buf) - 1) {
                        scan_op_str_buf[scan_op_idx++] = toupper((unsigned char)*scan_ptr++);
                    }
                    scan_op_str_buf[scan_op_idx] = '\0';

                    InstructionType scanned_inst = parseInstruction(scan_op_str_buf);

                    if (scanned_inst == IF || scanned_inst == WHILE) {
                        nested_level++;
                    }
                    else if (scanned_inst == END) {
                        if (nested_level > 0) {
                            nested_level--;
                        }
                        else {
                            cpu->ip = scan_ip;
                            found_end = true;
                            break;
                        }
                    }
                    scan_ip++;
                }

                if (!found_end) {
                    fprintf(stderr, "Error: Matching END not found for WHILE starting at line %d. Execution halted.\n", current_ip + 1);
                    running = false;
                }
            }
            break;
        }

        case BREAK: {
            int while_ip = find_enclosing_while_ip(program, program_size, cpu->ip);
            if (while_ip != -1) {
                int while_end_ip = find_matching_end(program, program_size, while_ip);
                if (while_end_ip != -1) {
                    cpu->ip = while_end_ip + 1;
                }
                else {
                    fprintf(stderr, "Error (Internal): WHILE at line %d has no matching END.\n", while_ip + 1);
                    running = false;
                }
            }
            else {
                fprintf(stderr, "Error: BREAK at line %d is not inside a WHILE block. Execution halted.\n", cpu->ip + 1);
                running = false;
            }
            break;
        }

        case CONTINUE: {
            int while_ip = find_enclosing_while_ip(program, program_size, cpu->ip);
            if (while_ip != -1) {
                int while_end_ip = find_matching_end(program, program_size, while_ip);
                if (while_end_ip != -1) {
                    cpu->ip = while_end_ip;
                }
                else {
                    fprintf(stderr, "Error (Internal): WHILE at line %d has no matching END.\n", while_ip + 1);
                    running = false;
                }
            }
            else {
                fprintf(stderr, "Error: CONTINUE at line %d is not inside a WHILE block. Execution halted.\n", cpu->ip + 1);
                running = false;
            }
            break;
        }

        case END: {
            int nested_level = 0;
            int scan_ip = cpu->ip - 1;
            bool found_match = false;
            int matching_block_start_ip = -1;

            while (scan_ip >= 0) {
                char scan_op_str_buf[MAX_LINE_LENGTH];
                char* scan_line = program[scan_ip];
                if (!scan_line) { scan_ip--; continue; }

                char* scan_ptr = scan_line;
                while (*scan_ptr && isspace((unsigned char)*scan_ptr)) scan_ptr++;
                int scan_op_idx = 0;
                while (*scan_ptr && !isspace((unsigned char)*scan_ptr) && *scan_ptr != ',' && scan_op_idx < sizeof(scan_op_str_buf) - 1) {
                    scan_op_str_buf[scan_op_idx++] = toupper((unsigned char)*scan_ptr++);
                }
                scan_op_str_buf[scan_op_idx] = '\0';

                InstructionType scanned_inst = parseInstruction(scan_op_str_buf);

                if (scanned_inst == END) {
                    nested_level++;
                }
                else if (scanned_inst == IF || scanned_inst == WHILE) {
                    if (nested_level > 0) {
                        nested_level--;
                    }
                    else {
                        found_match = true;
                        matching_block_start_ip = scan_ip;

                        if (scanned_inst == WHILE) {
                            char while_reg1_str[64], while_op_str[10], while_val_str[MAX_LINE_LENGTH];
                            char* while_line_ptr = program[scan_ip];

                            while (*while_line_ptr && isspace((unsigned char)*while_line_ptr)) while_line_ptr++;
                            while (*while_line_ptr && !isspace((unsigned char)*while_line_ptr) && *while_line_ptr != ',') while_line_ptr++;

                            if (parse_condition_operands(while_line_ptr, while_reg1_str, while_op_str, while_val_str, sizeof(while_val_str), scan_ip + 1) == -1) {
                                running = false;
                                break;
                            }

                            int condition_is_true = evaluate_condition(cpu, while_reg1_str, while_op_str, while_val_str, scan_ip + 1);

                            if (condition_is_true == -1) {
                                running = false;
                                break;
                            }

                            if (condition_is_true == 1) {
                                cpu->ip = scan_ip;
                            }
                        }
                        break;
                    }
                }
                scan_ip--;
            }

            if (!found_match && running) {
                fprintf(stderr, "Error: Stray END found at line %d. No matching WHILE or IF block. Execution halted.\n", current_ip + 1);
                running = false;
            }
            break;
        }

        case INVALID_INST:
        default:
            fprintf(stderr, "Error: Unknown or invalid instruction '%s' at line %d\n", op_str, cpu->ip + 1);
            running = false;
            break;
        }

        if (running && cpu->ip == current_ip) {
            cpu->ip++;
        }
    }

    if (cpu->ip >= program_size && running && !cpu->shutdown_requested) {
        printf("Program finished by running past the last instruction.\n");
    }
    else if (!running && !cpu->shutdown_requested) {
        printf("Execution halted due to error or HLT instruction.\n");
    }
    else if (cpu->shutdown_requested) {
        printf("Execution halted due to shutdown request (e.g., INT 0x41 or SDL Quit).\n");
    }
    else {
        printf("Execution stopped unexpectedly.\n");
    }
}


void mov(VirtualCPU* cpu, int reg1, int value) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error MOV: Invalid register R%d\n", reg1); return; }
    cpu->registers[reg1] = value;
}

void add(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error ADD: Invalid register R%d or R%d\n", reg1, reg2); return; }
    int val1 = cpu->registers[reg1];
    int val2 = cpu->registers[reg2];
    int result = val1 + val2;

    if (((val1 > 0 && val2 > 0) && (result < 0)) || ((val1 < 0 && val2 < 0) && (result > 0))) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->registers[reg1] = result;
}

void sub(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error SUB: Invalid register R%d or R%d\n", reg1, reg2); return; }
    int val1 = cpu->registers[reg1];
    int val2 = cpu->registers[reg2];
    int result = val1 - val2;

    if (((val1 > 0 && val2 < 0) && (result < 0)) || ((val1 < 0 && val2 > 0) && (result > 0))) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->registers[reg1] = result;
}

void mul(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error MUL: Invalid register R%d or R%d\n", reg1, reg2); return; }
    long long val1 = (long long)cpu->registers[reg1];
    long long val2 = (long long)cpu->registers[reg2];
    long long result = val1 * val2;

    if (result > INT_MAX || result < INT_MIN) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->registers[reg1] = (int)result;
}

void divi(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error DIV: Invalid register R%d or R%d\n", reg1, reg2); return; }
    int val1 = cpu->registers[reg1];
    int val2 = cpu->registers[reg2];

    if (val2 == 0) {
        fprintf(stderr, "Error: Division by zero (DIV R%d, R%d) at line %d.\n", reg1, reg2, cpu->ip + 1);
        cpu->flags &= ~FLAG_OVERFLOW;
        return;
    }

    if (val1 == INT_MIN && val2 == -1) {
        cpu->flags |= FLAG_OVERFLOW;
        cpu->registers[reg1] = val1 / val2;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
        cpu->registers[reg1] = val1 / val2;
    }
}

void mod_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error MOD: Invalid register R%d or R%d\n", reg1, reg2); return; }
    if (cpu->registers[reg2] != 0) {
        cpu->registers[reg1] %= cpu->registers[reg2];
    }
    else {
        fprintf(stderr, "Error: Modulo by zero (MOD R%d, R%d) at line %d.\n", reg1, reg2, cpu->ip + 1);
    }
}

void abs_op(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error ABS: Invalid register R%d\n", reg1); return; }
    int current_value = cpu->registers[reg1];
    if (current_value == INT_MIN) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        if (current_value < 0) {
            cpu->registers[reg1] = -current_value;
        }
        cpu->flags &= ~FLAG_OVERFLOW;
    }
}

void loop_op(VirtualCPU* cpu, int counter_reg, int target_line) {
    if (!isValidReg(counter_reg)) {
        fprintf(stderr, "Error LOOP: Invalid counter register R%d at line %d\n", counter_reg, cpu->ip + 1);
        return;
    }

    cpu->registers[counter_reg]--;

    if (cpu->registers[counter_reg] != 0) {
        jmp(cpu, target_line);
    }
}

void nop(VirtualCPU* cpu) {
    (void)cpu;
}

void hlt(VirtualCPU* cpu, bool* running_flag) {
    while (1) {

    }
}

void not_op(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error NOT: Invalid register R%d\n", reg1); return; }
    cpu->registers[reg1] = ~cpu->registers[reg1];
}

void and_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error AND: Invalid register R%d or R%d\n", reg1, reg2); return; }
    cpu->registers[reg1] &= cpu->registers[reg2];
}

void or_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error OR: Invalid register R%d or R%d\n", reg1, reg2); return; }
    cpu->registers[reg1] |= cpu->registers[reg2];
}

void xor_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error XOR: Invalid register R%d or R%d\n", reg1, reg2); return; }
    cpu->registers[reg1] ^= cpu->registers[reg2];
}

void shl(VirtualCPU* cpu, int reg1, int count) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error SHL: Invalid register R%d\n", reg1); return; }
    if (count < 0) { fprintf(stderr, "Warning SHL: Negative shift count %d. Ignored.\n", count); return; }
    cpu->registers[reg1] <<= count;
}

void shr(VirtualCPU* cpu, int reg1, int count) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error SHR: Invalid register R%d\n", reg1); return; }
    if (count < 0) { fprintf(stderr, "Warning SHR: Negative shift count %d. Ignored.\n", count); return; }
    cpu->registers[reg1] >>= count;
}

void jmp(VirtualCPU* cpu, int line_number) {
    int target_ip = line_number - 1;
    if (target_ip >= 0) {
        cpu->ip = target_ip;
    }
    else {
        fprintf(stderr, "Error: Invalid jump target line %d (-> index %d) at line %d.\n", line_number, target_ip, cpu->ip + 1);
    }
}

void cmp(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error CMP: Invalid register R%d or R%d\n", reg1, reg2); return; }
    int val1 = cpu->registers[reg1];
    int val2 = cpu->registers[reg2];
    int result = val1 - val2;

    if (((val1 > 0 && val2 < 0) && (result < 0)) || ((val1 < 0 && val2 > 0) && (result > 0))) {
        cpu->flags = (cpu->flags & ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS)) | FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS | FLAG_OVERFLOW);
    }

    if (val1 == val2) {
        cpu->flags |= FLAG_ZERO;
    }
    else if (val1 > val2) {
        cpu->flags |= FLAG_GREATER;
    }
    else {
        cpu->flags |= FLAG_LESS;
    }
}

void neg(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error NEG: Invalid register R%d\n", reg1); return; }
    int current_value = cpu->registers[reg1];
    if (current_value == INT_MIN) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->registers[reg1] = -current_value;
        cpu->flags &= ~FLAG_OVERFLOW;
    }
}

void inc(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error INC: Invalid register R%d\n", reg1); return; }
    int current_value = cpu->registers[reg1];
    if (current_value == INT_MAX) {
        cpu->registers[reg1] = INT_MIN;
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->registers[reg1]++;
        cpu->flags &= ~FLAG_OVERFLOW;
    }
}

void dec(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error DEC: Invalid register R%d\n", reg1); return; }
    int current_value = cpu->registers[reg1];
    if (current_value == INT_MIN) {
        cpu->registers[reg1] = INT_MAX;
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->registers[reg1]--;
        cpu->flags &= ~FLAG_OVERFLOW;
    }
}

void xchg(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error XCHG: Invalid register R%d or R%d\n", reg1, reg2); return; }
    int temp = cpu->registers[reg1];
    cpu->registers[reg1] = cpu->registers[reg2];
    cpu->registers[reg2] = temp;
}

void clr(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error CLR: Invalid register R%d\n", reg1); return; }
    cpu->registers[reg1] = 0;
}

void push(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error PUSH: Invalid register R%d\n", reg1); return; }
    int sp = cpu->sp;
    sp--;
    if (sp < 0) {
        fprintf(stderr, "Error: Stack Overflow during PUSH at line %d\n", cpu->ip + 1);
        cpu->sp = 0;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during PUSH.\n", sp);
        return;
    }

    cpu->memory[sp] = cpu->registers[reg1];
    cpu->sp = sp;
}

void pop(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error POP: Invalid register R%d\n", reg1); return; }
    int sp = cpu->sp;
    if (sp >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Stack Underflow during POP at line %d (SP=%d)\n", cpu->ip + 1, sp);
        cpu->sp = MEMORY_SIZE;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during POP.\n", sp);
        return;
    }

    cpu->registers[reg1] = cpu->memory[sp];
    cpu->sp = sp + 1;
}

void call(VirtualCPU* cpu, int line_number) {
    int target_ip = line_number - 1;
    if (target_ip < 0) {
        fprintf(stderr, "Error: Invalid CALL target line %d (-> index %d) at line %d.\n", line_number, target_ip, cpu->ip + 1);
        return;
    }

    int sp = cpu->sp;
    sp--;
    if (sp < 0) {
        fprintf(stderr, "Error: Stack Overflow during CALL at line %d\n", cpu->ip + 1);
        cpu->sp = 0;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during CALL.\n", sp);
        return;
    }

    cpu->memory[sp] = cpu->ip + 1;
    cpu->sp = sp;

    cpu->ip = target_ip;
}

void ret(VirtualCPU* cpu) {
    int sp = cpu->sp;
    if (sp >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Stack Underflow during RET at line %d (SP=%d)\n", cpu->ip + 1, sp);
        cpu->sp = MEMORY_SIZE;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during RET.\n", sp);
        return;
    }

    int return_ip = cpu->memory[sp];
    cpu->sp = sp + 1;

    if (return_ip >= 0) {
        cpu->ip = return_ip;
    }
    else {
        fprintf(stderr, "Error: Invalid return address %d popped from stack during RET at line %d.\n", return_ip, cpu->ip + 1);
        cpu->ip = MEMORY_SIZE;
    }
}

void rol(VirtualCPU* cpu, int reg1, int count) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error ROL: Invalid register R%d\n", reg1); return; }
    if (count < 0) { fprintf(stderr, "Warning ROL: Negative rotate count %d. Ignored.\n", count); return; }

    unsigned int val = (unsigned int)cpu->registers[reg1];
    int bit_size = sizeof(int) * 8;
    count %= bit_size;

    if (count > 0) {
        cpu->registers[reg1] = (int)((val << count) | (val >> (bit_size - count)));
    }
}

void ror(VirtualCPU* cpu, int reg1, int count) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error ROR: Invalid register R%d\n", reg1); return; }
    if (count < 0) { fprintf(stderr, "Warning ROR: Negative rotate count %d. Ignored.\n", count); return; }

    unsigned int val = (unsigned int)cpu->registers[reg1];
    int bit_size = sizeof(int) * 8;
    count %= bit_size;

    if (count > 0) {
        cpu->registers[reg1] = (int)((val >> count) | (val << (bit_size - count)));
    }
}

void rnd(VirtualCPU* cpu, int reg1) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error RND: Invalid register R%d\n", reg1); return; }
    cpu->registers[reg1] = rand();
}

void load_op(VirtualCPU* cpu, int dest_reg, int addr_src_reg) {
    if (!isValidReg(dest_reg)) {
        fprintf(stderr, "Error LOAD: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
        return;
    }
    if (!isValidReg(addr_src_reg)) {
        fprintf(stderr, "Error LOAD: Invalid address source register R%d at line %d\n", addr_src_reg, cpu->ip + 1);
        return;
    }

    int address = cpu->registers[addr_src_reg];

    if (!isValidMem(address)) {
        fprintf(stderr, "Error LOAD: Invalid memory address %d (0x%X) in R%d at line %d\n", address, address, addr_src_reg, cpu->ip + 1);
        return;
    }

    cpu->registers[dest_reg] = cpu->memory[address];
}

void store_op(VirtualCPU* cpu, int val_src_reg, int addr_dest_reg) {
    if (!isValidReg(val_src_reg)) {
        fprintf(stderr, "Error STORE: Invalid value source register R%d at line %d\n", val_src_reg, cpu->ip + 1);
        return;
    }
    if (!isValidReg(addr_dest_reg)) {
        fprintf(stderr, "Error STORE: Invalid address destination register R%d at line %d\n", addr_dest_reg, cpu->ip + 1);
        return;
    }

    int address = cpu->registers[addr_dest_reg];

    if (!isValidMem(address)) {
        fprintf(stderr, "Error STORE: Invalid memory address %d (0x%X) in R%d at line %d\n", address, address, addr_dest_reg, cpu->ip + 1);
        return;
    }

    cpu->memory[address] = cpu->registers[val_src_reg];
}

void test_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1)) { fprintf(stderr, "Error TEST: Invalid register R%d at line %d\n", reg1, cpu->ip + 1); return; }
    if (!isValidReg(reg2)) { fprintf(stderr, "Error TEST: Invalid register R%d at line %d\n", reg2, cpu->ip + 1); return; }

    int val1 = cpu->registers[reg1];
    int val2 = cpu->registers[reg2];
    int result = val1 & val2;

    cpu->flags &= ~(FLAG_ZERO | FLAG_OVERFLOW);
    if (result == 0) {
        cpu->flags |= FLAG_ZERO;
    }
}

void lea_op(VirtualCPU* cpu, int dest_reg, int base_reg, int offset) {
    if (!isValidReg(dest_reg)) {
        fprintf(stderr, "Error LEA: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
        return;
    }
    if (!isValidReg(base_reg)) {
        fprintf(stderr, "Error LEA: Invalid base register R%d at line %d\n", base_reg, cpu->ip + 1);
        return;
    }

    long long calculated_address = (long long)cpu->registers[base_reg] + offset;

    if (calculated_address > INT_MAX || calculated_address < INT_MIN) {
        fprintf(stderr, "Warning LEA: Calculated address %lld overflows standard integer at line %d. Clamping.\n", calculated_address, cpu->ip + 1);
        if (calculated_address > INT_MAX) calculated_address = INT_MAX;
        if (calculated_address < INT_MIN) calculated_address = INT_MIN;
    }

    cpu->registers[dest_reg] = (int)calculated_address;
}

void pushf_op(VirtualCPU* cpu) {
    int sp = cpu->sp;
    sp--;

    if (sp < 0) {
        fprintf(stderr, "Error: Stack Overflow during PUSHF at line %d\n", cpu->ip + 1);
        cpu->sp = 0;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during PUSHF at line %d.\n", sp, cpu->ip + 1);
        return;
    }

    cpu->memory[sp] = cpu->flags;
    cpu->sp = sp;
}

void popf_op(VirtualCPU* cpu) {
    int sp = cpu->sp;

    if (sp >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Stack Underflow during POPF at line %d (SP=%d)\n", cpu->ip + 1, sp);
        cpu->sp = MEMORY_SIZE;
        return;
    }
    if (!isValidMem(sp)) {
        fprintf(stderr, "Error: Stack Pointer %d out of bounds during POPF at line %d.\n", sp, cpu->ip + 1);
        return;
    }

    cpu->flags = cpu->memory[sp];
    cpu->sp = sp + 1;
}

void setf_op(VirtualCPU* cpu, int flag_mask) {
    if (flag_mask == 0) {
        cpu->flags |= (FLAG_ZERO | FLAG_GREATER | FLAG_LESS | FLAG_OVERFLOW);
    }
    else {
        cpu->flags |= flag_mask;
    }
}

void clrf_op(VirtualCPU* cpu, int flag_mask) {
    if (flag_mask == 0) {
        cpu->flags &= ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS | FLAG_OVERFLOW);
    }
    else {
        cpu->flags &= ~flag_mask;
    }
}

void bt_op(VirtualCPU* cpu, int reg1, int bit_index) {
    if (!isValidReg(reg1)) {
        fprintf(stderr, "Error BT: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
        return;
    }
    if (bit_index < 0 || bit_index >= 32) {
        fprintf(stderr, "Error BT: Invalid bit index %d at line %d\n", bit_index, cpu->ip + 1);
        return;
    }

    unsigned int mask = 1U << bit_index;

    cpu->flags &= ~FLAG_ZERO;

    if ((cpu->registers[reg1] & mask) == 0) {
        cpu->flags |= FLAG_ZERO;
    }
}

void bset_op(VirtualCPU* cpu, int reg1, int bit_index) {
    if (!isValidReg(reg1)) {
        fprintf(stderr, "Error BSET: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
        return;
    }
    if (bit_index < 0 || bit_index >= 32) {
        fprintf(stderr, "Error BSET: Invalid bit index %d at line %d\n", bit_index, cpu->ip + 1);
        return;
    }

    unsigned int mask = 1U << bit_index;
    cpu->registers[reg1] |= mask;
}

void bclr_op(VirtualCPU* cpu, int reg1, int bit_index) {
    if (!isValidReg(reg1)) {
        fprintf(stderr, "Error BCLR: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
        return;
    }
    if (bit_index < 0 || bit_index >= 32) {
        fprintf(stderr, "Error BCLR: Invalid bit index %d at line %d\n", bit_index, cpu->ip + 1);
        return;
    }

    unsigned int mask = 1U << bit_index;
    cpu->registers[reg1] &= ~mask;
}

void btog_op(VirtualCPU* cpu, int reg1, int bit_index) {
    if (!isValidReg(reg1)) {
        fprintf(stderr, "Error BTOG: Invalid register R%d at line %d\n", reg1, cpu->ip + 1);
        return;
    }
    if (bit_index < 0 || bit_index >= 32) {
        fprintf(stderr, "Error BTOG: Invalid bit index %d at line %d\n", bit_index, cpu->ip + 1);
        return;
    }

    unsigned int mask = 1U << bit_index;
    cpu->registers[reg1] ^= mask;
}

void strcmp_op(VirtualCPU* cpu, int reg_addr1, int reg_addr2) {
    if (!isValidReg(reg_addr1) || !isValidReg(reg_addr2)) {
        fprintf(stderr, "Error STRCMP: Invalid register R%d or R%d at line %d\n", reg_addr1, reg_addr2, cpu->ip + 1);
        return;
    }

    int addr1 = cpu->registers[reg_addr1];
    int addr2 = cpu->registers[reg_addr2];

    if (!isValidMem(addr1) || !isValidMem(addr2)) {
        fprintf(stderr, "Error STRCMP: Invalid memory address 0x%X or 0x%X at line %d\n", addr1, addr2, cpu->ip + 1);
        return;
    }

    int char1, char2;
    int offset = 0;
    while (true) {
        int current_addr1 = addr1 + offset;
        int current_addr2 = addr2 + offset;

        if (!isValidMem(current_addr1) || !isValidMem(current_addr2)) {
            fprintf(stderr, "Error STRCMP: Memory access out of bounds during compare at line %d\n", cpu->ip + 1);
            cpu->flags &= ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS);
            cpu->flags |= FLAG_LESS;
            return;
        }

        char1 = cpu->memory[current_addr1];
        char2 = cpu->memory[current_addr2];

        if (char1 == 0 && char2 == 0) {
            cpu->flags &= ~(FLAG_GREATER | FLAG_LESS);
            cpu->flags |= FLAG_ZERO;
            return;
        }
        if (char1 != char2) {
            cpu->flags &= ~FLAG_ZERO;
            if (char1 > char2) {
                cpu->flags |= FLAG_GREATER;
                cpu->flags &= ~FLAG_LESS;
            }
            else {
                cpu->flags |= FLAG_LESS;
                cpu->flags &= ~FLAG_GREATER;
            }
            return;
        }
        offset++;
        if (offset >= MEMORY_SIZE) {
            fprintf(stderr, "Warning STRCMP: Comparison exceeded safety limit at line %d\n", cpu->ip + 1);
            cpu->flags &= ~FLAG_ZERO;
            cpu->flags |= FLAG_LESS;
            cpu->flags &= ~FLAG_GREATER;
            return;
        }
    }
}

void strlen_op(VirtualCPU* cpu, int reg_dest, int reg_addr) {
    if (!isValidReg(reg_dest) || !isValidReg(reg_addr)) {
        fprintf(stderr, "Error STRLEN: Invalid register R%d or R%d at line %d\n", reg_dest, reg_addr, cpu->ip + 1);
        return;
    }

    int addr = cpu->registers[reg_addr];
    if (!isValidMem(addr)) {
        fprintf(stderr, "Error STRLEN: Invalid memory address 0x%X in R%d at line %d\n", addr, reg_addr, cpu->ip + 1);
        cpu->registers[reg_dest] = -1;
        return;
    }

    int length = 0;
    int current_addr = addr;
    while (isValidMem(current_addr) && cpu->memory[current_addr] != 0) {
        length++;
        current_addr++;
        if (length >= MEMORY_SIZE) {
            fprintf(stderr, "Warning STRLEN: Length calculation exceeded safety limit at line %d\n", cpu->ip + 1);
            break;
        }
    }

    cpu->registers[reg_dest] = length;

    cpu->flags &= ~FLAG_ZERO;
    if (length == 0) {
        cpu->flags |= FLAG_ZERO;
    }
}

void strcpy_op(VirtualCPU* cpu, int reg_dest_addr, int reg_src_addr) {
    if (!isValidReg(reg_dest_addr) || !isValidReg(reg_src_addr)) {
        fprintf(stderr, "Error STRCPY: Invalid register R%d or R%d at line %d\n", reg_dest_addr, reg_src_addr, cpu->ip + 1);
        return;
    }

    int dest_addr = cpu->registers[reg_dest_addr];
    int src_addr = cpu->registers[reg_src_addr];

    if (!isValidMem(dest_addr) || !isValidMem(src_addr)) {
        fprintf(stderr, "Error STRCPY: Invalid memory address 0x%X or 0x%X at line %d\n", dest_addr, src_addr, cpu->ip + 1);
        return;
    }

    if (dest_addr == src_addr) {
        return;
    }

    int offset = 0;
    int character;
    while (true) {
        int current_src_addr = src_addr + offset;
        int current_dest_addr = dest_addr + offset;

        if (!isValidMem(current_src_addr) || !isValidMem(current_dest_addr)) {
            fprintf(stderr, "Error STRCPY: Memory access out of bounds during copy at line %d\n", cpu->ip + 1);
            if (isValidMem(dest_addr + offset - 1) && offset > 0) {
                cpu->memory[dest_addr + offset - 1] = 0;
            }
            return;
        }

        character = cpu->memory[current_src_addr];
        cpu->memory[current_dest_addr] = character;

        if (character == 0) {
            break;
        }

        offset++;
        if (offset >= MEMORY_SIZE) {
            fprintf(stderr, "Warning STRCPY: Copy exceeded safety limit at line %d. Destination might not be null-terminated.\n", cpu->ip + 1);
            if (isValidMem(current_dest_addr)) {
                cpu->memory[current_dest_addr] = 0;
            }
            break;
        }
    }
}

void memcpy_op(VirtualCPU* cpu, int dest_addr_reg, int src_addr_reg, int len_reg) {
    if (!isValidReg(dest_addr_reg) || !isValidReg(src_addr_reg) || !isValidReg(len_reg)) {
        fprintf(stderr, "Error MEMCPY: Invalid register operand at line %d\n", cpu->ip + 1);
        return;
    }

    int dest_addr = cpu->registers[dest_addr_reg];
    int src_addr = cpu->registers[src_addr_reg];
    int len = cpu->registers[len_reg];

    if (len <= 0) {
        return;
    }

    if (!isValidMem(src_addr) || !isValidMem(src_addr + len - 1)) {
        fprintf(stderr, "Error MEMCPY: Source memory range [0x%X - 0x%X] is invalid at line %d\n",
            src_addr, src_addr + len - 1, cpu->ip + 1);
        return;
    }
    if (!isValidMem(dest_addr) || !isValidMem(dest_addr + len - 1)) {
        fprintf(stderr, "Error MEMCPY: Destination memory range [0x%X - 0x%X] is invalid at line %d\n",
            dest_addr, dest_addr + len - 1, cpu->ip + 1);
        return;
    }

    if ((src_addr < dest_addr && src_addr + len > dest_addr) ||
        (dest_addr < src_addr && dest_addr + len > src_addr)) {
        fprintf(stderr, "Warning MEMCPY: Source and destination memory regions overlap at line %d. Behavior might be undefined.\n", cpu->ip + 1);
    }

    for (int i = 0; i < len; i++) {
        if (isValidMem(src_addr + i) && isValidMem(dest_addr + i)) {
            cpu->memory[dest_addr + i] = cpu->memory[src_addr + i];
        }
        else {
            fprintf(stderr, "FATAL Error MEMCPY: Bounds exceeded *during* copy at line %d! Should not happen.\n", cpu->ip + 1);
            cpu->ip = MEMORY_SIZE + 1;
            break;
        }
    }
}

void memset_op(VirtualCPU* cpu, int dest_addr_reg, int val_reg, int len_reg) {
    if (!isValidReg(dest_addr_reg) || !isValidReg(val_reg) || !isValidReg(len_reg)) {
        fprintf(stderr, "Error MEMSET: Invalid register operand at line %d\n", cpu->ip + 1);
        return;
    }

    int dest_addr = cpu->registers[dest_addr_reg];
    int value = cpu->registers[val_reg];
    int len = cpu->registers[len_reg];
    unsigned char byte_value = value & 0xFF;

    if (len <= 0) {
        return;
    }

    if (!isValidMem(dest_addr) || !isValidMem(dest_addr + len - 1)) {
        fprintf(stderr, "Error MEMSET: Destination memory range [0x%X - 0x%X] is invalid at line %d\n",
            dest_addr, dest_addr + len - 1, cpu->ip + 1);
        return;
    }

    for (int i = 0; i < len; i++) {
        if (isValidMem(dest_addr + i)) {
            cpu->memory[dest_addr + i] = byte_value;
        }
        else {
            fprintf(stderr, "FATAL Error MEMSET: Bounds exceeded *during* set at line %d! Should not happen.\n", cpu->ip + 1);
            cpu->ip = MEMORY_SIZE + 1;
            break;
        }
    }
}

void cpuid_op(VirtualCPU* cpu, int dest_reg) {
    if (!isValidReg(dest_reg)) {
        fprintf(stderr, "Error CPUID: Invalid destination register R%d at line %d\n", dest_reg, cpu->ip + 1);
        return;
    }
    cpu->registers[dest_reg] = CPU_VERSION;
}

void bswap_op(VirtualCPU* cpu, int reg) {
    if (!isValidReg(reg)) {
        fprintf(stderr, "Error BSWAP: Invalid register R%d at line %d\n", reg, cpu->ip + 1);
        return;
    }
    unsigned int val = (unsigned int)cpu->registers[reg];
    unsigned int byte1 = (val >> 24) & 0xff;
    unsigned int byte2 = (val >> 16) & 0xff;
    unsigned int byte3 = (val >> 8) & 0xff;
    unsigned int byte4 = val & 0xff;
    unsigned int swapped_val = (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | byte1;
    cpu->registers[reg] = (int)swapped_val;
}

void sar_op(VirtualCPU* cpu, int reg, int count) {
    if (!isValidReg(reg)) {
        fprintf(stderr, "Error SAR: Invalid register R%d at line %d\n", reg, cpu->ip + 1);
        return;
    }

    cpu->registers[reg] >>= count;
}

void rvd_op(VirtualCPU* cpu, int reg) {
    if (!isValidReg(reg)) {
        fprintf(stderr, "Error RVD: Invalid register R%d at line %d\n", reg, cpu->ip + 1);
        return;
    }

    int original_value = cpu->registers[reg];
    int reversed_value = 0;
    int sign = 1;
    long long temp_original;

    if (original_value == 0) {
        return;
    }

    if (original_value < 0) {
        sign = -1;
        if (original_value == INT_MIN) {
            fprintf(stderr, "Warning RVD: Cannot reverse INT_MIN due to overflow potential. Value unchanged.\n");
            return;
        }
        temp_original = -(long long)original_value;
    }
    else {
        temp_original = original_value;
    }

    long long temp_reversed = 0;

    while (temp_original > 0) {
        int digit = temp_original % 10;
        if (temp_reversed > (LLONG_MAX - digit) / 10) {
            fprintf(stderr, "Warning RVD: Overflow detected during decimal reversal of %d at line %d. Result may be incorrect.\n", original_value, cpu->ip + 1);
        }
        temp_reversed = temp_reversed * 10 + digit;
        temp_original /= 10;
    }

    if (sign * temp_reversed > INT_MAX || sign * temp_reversed < INT_MIN) {
        fprintf(stderr, "Warning RVD: Reversed value %lld * %d overflows standard integer for %d at line %d. Result may be incorrect.\n", temp_reversed, sign, original_value, cpu->ip + 1);
    }

    reversed_value = (int)(sign * temp_reversed);
    cpu->registers[reg] = reversed_value;
}

void inc_mem_op(VirtualCPU* cpu, int addr_reg) {
    if (!isValidReg(addr_reg)) {
        fprintf(stderr, "Error INC_MEM: Invalid address register R%d at line %d\n", addr_reg, cpu->ip + 1);
        return;
    }
    int address = cpu->registers[addr_reg];
    if (!isValidMem(address)) {
        fprintf(stderr, "Error INC_MEM: Invalid memory address 0x%X (from R%d) at line %d\n", address, addr_reg, cpu->ip + 1);
        return;
    }

    int current_value = cpu->memory[address];
    int new_value;

    if (current_value == INT_MAX) {
        new_value = INT_MIN;
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        new_value = current_value + 1;
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->memory[address] = new_value;
}

void dec_mem_op(VirtualCPU* cpu, int addr_reg) {
    if (!isValidReg(addr_reg)) {
        fprintf(stderr, "Error DEC_MEM: Invalid address register R%d at line %d\n", addr_reg, cpu->ip + 1);
        return;
    }
    int address = cpu->registers[addr_reg];
    if (!isValidMem(address)) {
        fprintf(stderr, "Error DEC_MEM: Invalid memory address 0x%X (from R%d) at line %d\n", address, addr_reg, cpu->ip + 1);
        return;
    }

    int current_value = cpu->memory[address];
    int new_value;

    if (current_value == INT_MIN) {
        new_value = INT_MAX;
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        new_value = current_value - 1;
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->memory[address] = new_value;
}

void loopo_op(VirtualCPU* cpu, int counter_reg, int target_line) {
    if (!isValidReg(counter_reg)) {
        fprintf(stderr, "Error LOOPO: Invalid counter register R%d at line %d\n", counter_reg, cpu->ip + 1);
        return;
    }

    cpu->registers[counter_reg]--;

    if (cpu->registers[counter_reg] != 0 && (cpu->flags & FLAG_OVERFLOW) != 0) {
        jmp(cpu, target_line);
    }
}

void loopno_op(VirtualCPU* cpu, int counter_reg, int target_line) {
    if (!isValidReg(counter_reg)) {
        fprintf(stderr, "Error LOOPNO: Invalid counter register R%d at line %d\n", counter_reg, cpu->ip + 1);
        return;
    }

    cpu->registers[counter_reg]--;

    if (cpu->registers[counter_reg] != 0 && (cpu->flags & FLAG_OVERFLOW) == 0) {
        jmp(cpu, target_line);
    }
}

void fmov_reg(VirtualCPU* cpu, int dest_freg, int src_freg) {
    if (!isValidFReg(dest_freg) || !isValidFReg(src_freg)) {
        fprintf(stderr, "Error FMOV: Invalid F register F%d or F%d\n", dest_freg, src_freg);
        return;
    }
    cpu->f_registers[dest_freg] = cpu->f_registers[src_freg];
}

void fmov_imm(VirtualCPU* cpu, int dest_freg, double value) {
    if (!isValidFReg(dest_freg)) {
        fprintf(stderr, "Error FMOV: Invalid destination F register F%d\n", dest_freg);
        return;
    }
    cpu->f_registers[dest_freg] = value;
}

void fadd(VirtualCPU* cpu, int dest_freg, int src_freg) {
    if (!isValidFReg(dest_freg) || !isValidFReg(src_freg)) {
        fprintf(stderr, "Error FADD: Invalid F register F%d or F%d\n", dest_freg, src_freg);
        return;
    }
    double result = cpu->f_registers[dest_freg] + cpu->f_registers[src_freg];
    if (isinf(result)) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->f_registers[dest_freg] = result;
}

void fsub(VirtualCPU* cpu, int dest_freg, int src_freg) {
    if (!isValidFReg(dest_freg) || !isValidFReg(src_freg)) {
        fprintf(stderr, "Error FSUB: Invalid F register F%d or F%d\n", dest_freg, src_freg);
        return;
    }
    double result = cpu->f_registers[dest_freg] - cpu->f_registers[src_freg];
    if (isinf(result)) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->f_registers[dest_freg] = result;
}

void fmul(VirtualCPU* cpu, int dest_freg, int src_freg) {
    if (!isValidFReg(dest_freg) || !isValidFReg(src_freg)) {
        fprintf(stderr, "Error FMUL: Invalid F register F%d or F%d\n", dest_freg, src_freg);
        return;
    }
    double result = cpu->f_registers[dest_freg] * cpu->f_registers[src_freg];
    if (isinf(result)) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->f_registers[dest_freg] = result;
}

void fdiv(VirtualCPU* cpu, int dest_freg, int src_freg) {
    if (!isValidFReg(dest_freg) || !isValidFReg(src_freg)) {
        fprintf(stderr, "Error FDIV: Invalid F register F%d or F%d\n", dest_freg, src_freg);
        return;
    }
    double divisor = cpu->f_registers[src_freg];
    if (divisor == 0.0) {
        fprintf(stderr, "Error: Floating point division by zero (FDIV F%d, F%d) at line %d.\n", dest_freg, src_freg, cpu->ip + 1);
        cpu->f_registers[dest_freg] = INFINITY;
        cpu->flags |= FLAG_OVERFLOW;
        return;
    }
    double result = cpu->f_registers[dest_freg] / divisor;
    if (isinf(result)) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->f_registers[dest_freg] = result;
}

#define FPU_EPSILON 1e-9

void fcmp(VirtualCPU* cpu, int freg1, int freg2) {
    if (!isValidFReg(freg1) || !isValidFReg(freg2)) {
        fprintf(stderr, "Error FCMP: Invalid F register F%d or F%d\n", freg1, freg2);
        return;
    }
    double val1 = cpu->f_registers[freg1];
    double val2 = cpu->f_registers[freg2];

    cpu->flags &= ~(FLAG_ZERO | FLAG_GREATER | FLAG_LESS | FLAG_OVERFLOW);

    if (fabs(val1 - val2) < FPU_EPSILON) {
        cpu->flags |= FLAG_ZERO;
    }
    else if (val1 > val2) {
        cpu->flags |= FLAG_GREATER;
    }
    else {
        cpu->flags |= FLAG_LESS;
    }
}

void fabs_op(VirtualCPU* cpu, int freg) {
    if (!isValidFReg(freg)) {
        fprintf(stderr, "Error FABS: Invalid F register F%d\n", freg);
        return;
    }
    cpu->f_registers[freg] = fabs(cpu->f_registers[freg]);
}

void fneg_op(VirtualCPU* cpu, int freg) {
    if (!isValidFReg(freg)) {
        fprintf(stderr, "Error FNEG: Invalid F register F%d\n", freg);
        return;
    }
    cpu->f_registers[freg] = -cpu->f_registers[freg];
}

void fsqrt(VirtualCPU* cpu, int freg) {
    if (!isValidFReg(freg)) {
        fprintf(stderr, "Error FSQRT: Invalid F register F%d\n", freg);
        return;
    }
    double value = cpu->f_registers[freg];
    double result;

    cpu->flags &= ~FLAG_OVERFLOW;

    if (value < 0.0) {
        fprintf(stderr, "Warning: FSQRT domain error (sqrt of negative number %f) at line %d.\n", value, cpu->ip + 1);
        result = nan("");
    }
    else {
        result = sqrt(value);
    }

    if (isinf(result)) {
        cpu->flags |= FLAG_OVERFLOW;
    }

    cpu->f_registers[freg] = result;
}

void cvtif(VirtualCPU* cpu, int dest_freg, int src_reg) {
    if (!isValidFReg(dest_freg)) {
        fprintf(stderr, "Error CVTIF: Invalid destination F register F%d\n", dest_freg);
        return;
    }
    if (!isValidReg(src_reg)) {
        fprintf(stderr, "Error CVTIF: Invalid source register R%d\n", src_reg);
        return;
    }
    cpu->f_registers[dest_freg] = (double)cpu->registers[src_reg];
    cpu->flags &= ~FLAG_OVERFLOW;
}

void cvtfi(VirtualCPU* cpu, int dest_reg, int src_freg) {
    if (!isValidReg(dest_reg)) {
        fprintf(stderr, "Error CVTFI: Invalid destination register R%d\n", dest_reg);
        return;
    }
    if (!isValidFReg(src_freg)) {
        fprintf(stderr, "Error CVTFI: Invalid source F register F%d\n", src_freg);
        return;
    }

    double float_value = cpu->f_registers[src_freg];
    double rounded_value = round(float_value);

    if (rounded_value > (double)INT_MAX || rounded_value < (double)INT_MIN || isnan(rounded_value)) {
        cpu->flags |= FLAG_OVERFLOW;
        if (isnan(rounded_value)) {
            cpu->registers[dest_reg] = 0;
        }
        else if (rounded_value > 0) {
            cpu->registers[dest_reg] = INT_MAX;
        }
        else {
            cpu->registers[dest_reg] = INT_MIN;
        }
        fprintf(stderr, "Warning: Overflow or NaN during CVTFI conversion of %f at line %d\n", float_value, cpu->ip + 1);
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
        cpu->registers[dest_reg] = (int)rounded_value;
    }
}

void fload(VirtualCPU* cpu, int dest_freg, int addr_src_reg) {
    if (!isValidFReg(dest_freg)) {
        fprintf(stderr, "Error FLOAD: Invalid destination F register F%d\n", dest_freg);
        return;
    }
    if (!isValidReg(addr_src_reg)) {
        fprintf(stderr, "Error FLOAD: Invalid source address register R%d\n", addr_src_reg);
        return;
    }

    int address = cpu->registers[addr_src_reg];

    size_t bytes_needed = sizeof(double);
    if (bytes_needed == 0 || bytes_needed > MEMORY_SIZE) {
        fprintf(stderr, "Error FLOAD: Invalid sizeof(double) calculation.\n");
        return;
    }
    if (!isValidMem(address) || !isValidMem(address + bytes_needed - 1)) {
        fprintf(stderr, "Error FLOAD: Invalid memory range [0x%X - 0x%X] from R%d at line %d\n",
            address, address + (int)bytes_needed - 1, addr_src_reg, cpu->ip + 1);
        return;
    }

    double loaded_value;

    memcpy(&loaded_value, &cpu->memory[address], sizeof(double));

    cpu->f_registers[dest_freg] = loaded_value;
    cpu->flags &= ~FLAG_OVERFLOW;
}

void fstore(VirtualCPU* cpu, int addr_dest_reg, int src_freg) {
    if (!isValidReg(addr_dest_reg)) {
        fprintf(stderr, "Error FSTORE: Invalid destination address register R%d\n", addr_dest_reg);
        return;
    }
    if (!isValidFReg(src_freg)) {
        fprintf(stderr, "Error FSTORE: Invalid source F register F%d\n", src_freg);
        return;
    }

    int address = cpu->registers[addr_dest_reg];
    double value_to_store = cpu->f_registers[src_freg];

    size_t bytes_needed = sizeof(double);
    if (bytes_needed == 0 || bytes_needed > MEMORY_SIZE) {
        fprintf(stderr, "Error FSTORE: Invalid sizeof(double) calculation.\n");
        return;
    }
    if (!isValidMem(address) || !isValidMem(address + bytes_needed - 1)) {
        fprintf(stderr, "Error FSTORE: Invalid memory range [0x%X - 0x%X] from R%d at line %d\n",
            address, address + (int)bytes_needed - 1, addr_dest_reg, cpu->ip + 1);
        return;
    }

    memcpy(&cpu->memory[address], &value_to_store, sizeof(double));

    cpu->flags &= ~FLAG_OVERFLOW;
}

void finc(VirtualCPU* cpu, int freg) {
    if (!isValidFReg(freg)) {
        fprintf(stderr, "Error FINC: Invalid F register F%d\n", freg);
        return;
    }
    double result = cpu->f_registers[freg] + 1.0;
    if (isinf(result)) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->f_registers[freg] = result;
}

void fdec(VirtualCPU* cpu, int freg) {
    if (!isValidFReg(freg)) {
        fprintf(stderr, "Error FDEC: Invalid F register F%d\n", freg);
        return;
    }
    double result = cpu->f_registers[freg] - 1.0;
    if (isinf(result)) {
        cpu->flags |= FLAG_OVERFLOW;
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }
    cpu->f_registers[freg] = result;
}

#define SLOTS_PER_DOUBLE (sizeof(double) / sizeof(int))

void fpush(VirtualCPU* cpu, int freg) {
    if (!isValidFReg(freg)) {
        fprintf(stderr, "Error FPUSH: Invalid F register F%d\n", freg);
        return;
    }
    if (SLOTS_PER_DOUBLE <= 0 || sizeof(double) % sizeof(int) != 0) {
        fprintf(stderr, "Error FPUSH: Incompatible double/int sizes for stack.\n");
        return;
    }

    int sp = cpu->sp;
    sp -= SLOTS_PER_DOUBLE;

    if (sp < 0) {
        fprintf(stderr, "Error: Stack Overflow during FPUSH at line %d\n", cpu->ip + 1);
        cpu->sp = 0;
        return;
    }

    if (!isValidMem(sp) || !isValidMem(sp + SLOTS_PER_DOUBLE - 1)) {
        fprintf(stderr, "Error: Stack Pointer range [%d - %d] out of bounds during FPUSH.\n", sp, sp + SLOTS_PER_DOUBLE - 1);
        return;
    }

    memcpy(&cpu->memory[sp], &cpu->f_registers[freg], sizeof(double));
    cpu->sp = sp;
}

void fpop(VirtualCPU* cpu, int freg) {
    if (!isValidFReg(freg)) {
        fprintf(stderr, "Error FPOP: Invalid F register F%d\n", freg);
        return;
    }
    if (SLOTS_PER_DOUBLE <= 0 || sizeof(double) % sizeof(int) != 0) {
        fprintf(stderr, "Error FPOP: Incompatible double/int sizes for stack.\n");
        return;
    }

    int sp = cpu->sp;
    if (sp + SLOTS_PER_DOUBLE > MEMORY_SIZE) {
        fprintf(stderr, "Error: Stack Underflow during FPOP at line %d (SP=%d)\n", cpu->ip + 1, sp);
        cpu->sp = MEMORY_SIZE;
        return;
    }

    if (!isValidMem(sp) || !isValidMem(sp + SLOTS_PER_DOUBLE - 1)) {
        fprintf(stderr, "Error: Stack Pointer range [%d - %d] out of bounds during FPOP.\n", sp, sp + SLOTS_PER_DOUBLE - 1);
        return;
    }

    memcpy(&cpu->f_registers[freg], &cpu->memory[sp], sizeof(double));
    cpu->sp = sp + SLOTS_PER_DOUBLE;
}

void fclr(VirtualCPU* cpu, int freg) {
    if (!isValidFReg(freg)) {
        fprintf(stderr, "Error FCLR: Invalid F register F%d\n", freg);
        return;
    }
    cpu->f_registers[freg] = 0.0;
}

void fxchg(VirtualCPU* cpu, int freg1, int freg2) {
    if (!isValidFReg(freg1) || !isValidFReg(freg2)) {
        fprintf(stderr, "Error FXCHG: Invalid F register F%d or F%d\n", freg1, freg2);
        return;
    }
    double temp = cpu->f_registers[freg1];
    cpu->f_registers[freg1] = cpu->f_registers[freg2];
    cpu->f_registers[freg2] = temp;
}

void pow_op(VirtualCPU* cpu, int reg1, int reg2) {
    if (!isValidReg(reg1) || !isValidReg(reg2)) { fprintf(stderr, "Error POW: Invalid register R%d or R%d\n", reg1, reg2); return; }
    long long base = cpu->registers[reg1];
    int exponent = cpu->registers[reg2];
    long long result = 1;

    if (exponent < 0) {
        fprintf(stderr, "Warning (POW): Negative exponent %d. Result set to 0.\n", exponent);
        result = 0;
    }
    else if (exponent == 0) {
        result = 1;
    }
    else {
        for (int i = 0; i < exponent; i++) {
            if (llabs(base) > 0 && llabs(result) > INT_MAX / llabs(base)) {
                fprintf(stderr, "Warning (POW): Potential integer overflow. Result truncated.\n");
            }
            result *= base;
        }
    }
    if (result > INT_MAX) cpu->registers[reg1] = INT_MAX;
    else if (result < INT_MIN) cpu->registers[reg1] = INT_MIN;
    else cpu->registers[reg1] = (int)result;
}

void fpow_op(VirtualCPU* cpu, int dest_freg, int src_freg) {
    if (!isValidFReg(dest_freg) || !isValidFReg(src_freg)) {
        fprintf(stderr, "Error FPOW: Invalid F register F%d or F%d at line %d\n", dest_freg, src_freg, cpu->ip + 1);
        cpu->ip = MEMORY_SIZE + 1;
        return;
    }

    double base = cpu->f_registers[dest_freg];
    double exponent = cpu->f_registers[src_freg];
    double result = pow(base, exponent);

    if (isinf(result) || isnan(result)) {
        cpu->flags |= FLAG_OVERFLOW;
        if (isnan(result)) {
            fprintf(stderr, "Warning FPOW: Result is NaN (e.g., pow(negative, non-integer)) at line %d. Storing NaN.\n", cpu->ip + 1);
        }
        else {
            fprintf(stderr, "Warning FPOW: Result is Infinity (overflow) at line %d. Storing Infinity.\n", cpu->ip + 1);
        }
    }
    else {
        cpu->flags &= ~FLAG_OVERFLOW;
    }

    cpu->f_registers[dest_freg] = result;
}

void audioCallback(void* userdata, Uint8* stream, int len) {
    VirtualCPU* cpu = (VirtualCPU*)userdata;
    Sint16* audio_stream = (Sint16*)stream;
    int samples_to_generate = len / sizeof(Sint16);

    static double phase = 0.0;
    double phase_increment = cpu->frequency / (double)cpu->audioSpec.freq;

    for (int i = 0; i < samples_to_generate; i++) {
        Sint16 sample_value = 0;

        if (cpu->speaker_on) {
            bool is_sleeping = false;
            if (cpu->sleep_duration > 0) {
                Uint32 current_time = SDL_GetTicks();
                if (current_time - cpu->sleep_start_time < (Uint32)cpu->sleep_duration) {
                    is_sleeping = true;
                }
                else {
                    cpu->sleep_duration = 0;
                    cpu->sleep_start_time = 0;
                    is_sleeping = false;
                }
            }
            if (!is_sleeping) {
                double wave_value = sin(2.0 * M_PI * phase);
                sample_value = (wave_value > 0) ? (Sint16)(32767 * cpu->volume) : (Sint16)(-32767 * cpu->volume);
            }
        }

        audio_stream[i] = sample_value;
        phase += phase_increment;
        if (phase >= 1.0) phase -= 1.0;
    }
}

int main(void) {
    char** program = NULL;
    char filename[256];
    int program_size = 0;
    int start_ip = -1;
    VirtualCPU* cpu_ptr = NULL;
    bool sdl_initialized = false;
    bool cpu_initialized = false;

    printf("Enter the filename of the .asm file to run: ");
    if (scanf("%255s", filename) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    program = malloc(MAX_PROGRAM_SIZE * sizeof(char*));
    if (program == NULL) {
        fprintf(stderr, "Fatal: Failed to allocate memory for program array.\n");
        return 1;
    }
    memset(program, 0, MAX_PROGRAM_SIZE * sizeof(char*));

    printf("Allocating Virtual CPU on heap...\n");
    cpu_ptr = (VirtualCPU*)malloc(sizeof(VirtualCPU));
    if (!cpu_ptr) {
        fprintf(stderr, "Fatal: Failed to allocate memory for VirtualCPU struct.\n");
        return 1;
    }
    printf("CPU Struct Allocated.\n");

    printf("Initializing Virtual CPU...\n");
    if (!init_cpu(cpu_ptr)) {
        fprintf(stderr, "CPU Initialization failed.\n");
        free(cpu_ptr);
        return 1;
    }
    cpu_initialized = true;
    printf("CPU Initialized.\n");

    if (!initialize_disk_image(cpu_ptr)) {
        fprintf(stderr, "Failed to initialize disk image. Continuing without disk support...\n");
    }

    printf("Initializing SDL...\n");
    if (!init_sdl(cpu_ptr)) {
        fprintf(stderr, "SDL Initialization failed.\n");
        if (cpu_initialized) cleanup_cpu(cpu_ptr);
        free(cpu_ptr);
        return 1;
    }
    sdl_initialized = true;
    printf("SDL Initialized.\n");

#ifdef _WIN32
    enable_virtual_terminal_processing_if_needed();
    puts("\n");
    printf("Virtual terminal processing checked/enabled (Windows).\n");
#endif

    printf("Loading program '%s'...\n", filename);
    program_size = loadProgram(filename, program, MAX_PROGRAM_SIZE, &start_ip);

    if (program_size < 0) {
        fprintf(stderr, "Failed to load program or find entry point.\n");
        if (sdl_initialized) cleanup_sdl(cpu_ptr);
        if (cpu_initialized) cleanup_cpu(cpu_ptr);
        free(cpu_ptr);
        if (program) free(program);
        return 1;
    }
    printf("Program loaded successfully (%d lines).\n", program_size);
    printf("Entry point '_main:' found at line %d (IP %d).\n", start_ip + 1, start_ip);

    bool debug_mode = false;
    char debug_choice[10];
    printf("Enable debug mode? (y/n): ");
    fflush(stdout);
    if (fgets(debug_choice, sizeof(debug_choice), stdin) != NULL) {
        if (debug_choice[0] == 'y' || debug_choice[0] == 'Y') {
            debug_mode = true;
            printf("Debug mode enabled.\n");
        }
        else {
            printf("Debug mode disabled.\n");
        }
    }
    else {
        fprintf(stderr, "Warning: Could not read debug mode choice, defaulting to disabled.\n");
    }

    printf("Starting execution...\n");
    cpu_ptr->ip = start_ip;
    execute(cpu_ptr, program, program_size, debug_mode);

    printf("Cleaning up...\n");
    for (int i = 0; i < program_size; i++) {
        if (program[i] != NULL) {
            free(program[i]);
            program[i] = NULL;
        }
    }
    printf("Program memory freed.\n");

    free(program);
    program = NULL;
    printf("Program array memory freed.\n");

    if (sdl_initialized) {
        cleanup_sdl(cpu_ptr);
        printf("SDL resources cleaned up.\n");
    }

    if (cpu_initialized) {
        cleanup_cpu(cpu_ptr);
        printf("CPU resources cleaned up.\n");
    }

    free(cpu_ptr);
    printf("CPU struct memory freed.\n");

    printf("Exiting.\n");
    return 0;
}
