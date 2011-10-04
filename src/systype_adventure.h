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

#ifndef SYSTYPE_ADVENTURE_H
#define SYSTYPE_ADVENTURE_H

/* = Includes = */

#include "defines.h"
// #include "graphics.h"
#include "event.h"
#include "tilemap.h"
#include "mobile.h"
#include "cache.h"
#include "window.h"

/* = Definitions = */

/* The viewport width/height both have to be divisible by this or scrolling   *
 * won't work and weird bugs will happen.                                     */
#define SYSTYPE_ADVENTURE_SCROLL_SPEED 160

/* = Type and Struct Definitions = */

typedef struct {
   MOBILE_MOBILE* mobile;
   int speed; /* The number of pixels to move per cycle. */
   TILEMAP_DIR direction; /* The direction being walked in. */
} SYSTYPE_ADVENTURE_WALK;

/* = Function Prototypes = */

int systype_adventure_loop( CACHE_CACHE* );
BOOL systype_adventure_mobile_walk(
   MOBILE_MOBILE*, TILEMAP_DIR, MOBILE_MOBILE[], int, TILEMAP_TILEMAP*
);
void systype_adventure_menu_show( WINDOW_MENU**, CACHE_CACHE* );
void systype_adventure_menu_test( void );
void systype_adventure_viewport_scroll(
   GEO_RECTANGLE*, CACHE_CACHE*, MOBILE_MOBILE[], int, TILEMAP_TILEMAP*,
   WINDOW_MENU*, TILEMAP_DIR
);
void systype_adventure_viewport_draw(
   GEO_RECTANGLE*, CACHE_CACHE*, MOBILE_MOBILE[], int, TILEMAP_TILEMAP*,
   WINDOW_MENU*, BOOL
);

#endif /* SYSTYPE_ADVENTURE_H */
