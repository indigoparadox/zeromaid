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
#include "graphics.h"
#include "event.h"
#include "tilemap.h"
#include "mobile.h"
#include "cache.h"

/* = Definitions = */

/* = Type and Struct Definitions = */

typedef struct {
   MOBILE_MOBILE* mobile;
   int speed; /* The number of pixels to move per cycle. */
   TILEMAP_DIR direction; /* The direction being walked in. */
} SYSTYPE_ADVENTURE_WALK;

/* = Function Prototypes = */

int systype_adventure_loop( bstring );
BOOL systype_adventure_mobile_walk(
   MOBILE_MOBILE*, TILEMAP_DIR, MOBILE_MOBILE[], int, TILEMAP_TILEMAP*
);
MOBILE_MOBILE* systype_adventure_load_mobiles( int*, TILEMAP_TILEMAP* );

#endif /* SYSTYPE_ADVENTURE_H */
