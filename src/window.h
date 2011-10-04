/* This file is part of ZeroMaid.                                             *
 *                                                                            *
 * ZeroMaid is free software; you can redistribute it and/or modify it under  *
 * the terms of the GNU General Public License as published by the Free       *
 * Software Foundation; either version 2 of the License, or (at your option)  *
 * any later version.                                                         *
 *                                                                            *
 * ZeroMaid is distributed in the hope that it will be useful, but WITHOUT    *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   *
 * more details.                                                              *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with ZeroMaid; if not, write to the Free Software Foundation, Inc.,        *
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA                     */

#ifndef WINDOW_H
#define WINDOW_H

/* = Notes = */

/* The window list in the system cache starts with the oldest window at index *
 * 0. Newer windows have higher indexes. This is an implementation detail,    *
 * however, and the functions below start with 0 being the index that refers  *
 * to the newest window. This is because the newest window will usually be a  *
 * reasonable default and it's simpler when writing the calling functions to  *
 * think of it that way.                                                      */

/* = Includes = */

#include <time.h>

#include "defines.h"
#include "cache.h"

/* = Definitions = */

#define WINDOW_BUFFER_LENGTH 38

#define WINDOW_TEXT_DEFAULT_FONT "font_manaspc"
#define WINDOW_TEXT_DEFAULT_SIZE 22
#define WINDOW_TEXT_DEFAULT_HEIGHT 25

#define WINDOW_MENU_DEFAULT_FONT "font_manaspc"
#define WINDOW_MENU_DEFAULT_SIZE 22
#define WINDOW_MENU_DEFAULT_HEIGHT 30

#define WINDOW_MENU_DEFAULT_COLOR_FG_R 255
#define WINDOW_MENU_DEFAULT_COLOR_FG_G 255
#define WINDOW_MENU_DEFAULT_COLOR_FG_B 255
#define WINDOW_MENU_DEFAULT_COLOR_BG_R 0
#define WINDOW_MENU_DEFAULT_COLOR_BG_G 0
#define WINDOW_MENU_DEFAULT_COLOR_BG_B 0
#define WINDOW_MENU_DEFAULT_COLOR_FG_S_R 0
#define WINDOW_MENU_DEFAULT_COLOR_FG_S_G 0
#define WINDOW_MENU_DEFAULT_COLOR_FG_S_B 0
#define WINDOW_MENU_DEFAULT_COLOR_BG_S_R 255
#define WINDOW_MENU_DEFAULT_COLOR_BG_S_G 255
#define WINDOW_MENU_DEFAULT_COLOR_BG_S_B 255


/* = Type and Struct Definitions = */

typedef struct {
   GEO_COLOR fg, bg, sfg, sbg;
} WINDOW_MENU_COLORS;

typedef struct {
   bstring desc, /* The description to display on-screen for this item. */
      key, /* The key to set if this item is selected. */
      value, /* The value to set it to if this item is selected. */
      font_filename;
   // ZZZ: GFX_FONT* font;
} WINDOW_MENU_ITEM;

typedef struct {
   WINDOW_MENU_ITEM* options;
   int options_count, /* The total number of options. */
      selected, /* The currently selected option. */
      x, y; /* Where the menu on the screen is located. */
   WINDOW_MENU_COLORS colors;
   COND_SCOPE scope;
   void (*callback)( void );
} WINDOW_MENU;

/* = Function Prototypes = */

BOOL window_init( void );
void window_cleanup( void );
CACHE_LOG_ENTRY* window_create_text( bstring, CACHE_LOG_ENTRY*, int* );
WINDOW_MENU* window_create_menu(
   bstring, COND_SCOPE, void (*)( void ), WINDOW_MENU_COLORS*
);
void window_draw_text( int, CACHE_CACHE* );
void window_draw_menu( WINDOW_MENU* );
void window_set_text_font( bstring, int );
void window_set_menu_font( bstring, int );

void window_free_menu( WINDOW_MENU* );

#endif /* WINDOW_H */
