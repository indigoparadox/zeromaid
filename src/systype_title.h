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

#ifndef SYSLOOP_TITLE_H
#define SYSLOOP_TITLE_H

/* = Includes = */

#include <stdio.h>

#include "defines.h"
#include "graphics.h"
#include "event.h"
#include "cache.h"

/* = Definitions = */

#define SYSTYPE_TITLE_MENU_LABEL_SPSTART "Start"
#define SYSTYPE_TITLE_MENU_LABEL_LOAD "Load"
#define SYSTYPE_TITLE_MENU_LABEL_QUIT "Quit"

#define SYSTYPE_TITLE_MENU_INDEX_SPSTART 0
#define SYSTYPE_TITLE_MENU_INDEX_LOAD 1
#define SYSTYPE_TITLE_MENU_INDEX_QUIT 2

/* The following must be the length of the list above. */
#define SYSTYPE_TITLE_MENU_LEN 3

/* = Type and Struct Definitions = */

typedef struct _SYSTYPE_TITLE_TEXT {
   bstring text, font_name;
   GFX_COLOR* fg_color;
   int x, y, size;
   struct _SYSTYPE_TITLE_TEXT* next;
} SYSTYPE_TITLE_TEXT;

typedef struct _SYSTYPE_TITLE_TITLESCREEN {
   bstring menu_font;
   GFX_SURFACE* bg_image;
   GFX_COLOR* bg_color, * fg_color, * fg_highlight;
   int i_trans, o_trans, delay;
   short int show_menu;
   SYSTYPE_TITLE_TEXT* text_node;
   struct _SYSTYPE_TITLE_TITLESCREEN* next;
} SYSTYPE_TITLE_TITLESCREEN;

/* = Function Prototypes = */

int systype_title_loop( CACHE_CACHE* );
SYSTYPE_TITLE_TITLESCREEN* systype_title_load_titlescreens( void );
BOOL systype_title_load_start( CACHE_CACHE* );
void systype_title_show_menu( int, bstring[], bstring, GFX_COLOR*, GFX_COLOR* );

#endif /* SYSLOOP_TITLE_H */
