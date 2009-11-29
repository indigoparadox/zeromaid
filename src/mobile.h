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

#ifndef MOBILE_H
#define MOBILE_H

/* = Includes = */

#include "graphics.h"

/* = Type and Struct Definitions = */

typedef struct {
   unsigned int hp;
   int tile_x, tile_y,
      pixel_size; /* The size of the mobile's sprite. */
   float pixel_multiplier; /* The size to expand the mobile to when blitted. */
   GFX_SPRITESHEET* spritesheet;
} MOBILE_MOBILE;

/* = Function Prototypes = */

MOBILE_MOBILE* mobile_create_mobile( bstring );
void mobile_draw( MOBILE_MOBILE*, GFX_RECTANGLE* );
void mobile_draw_list( MOBILE_MOBILE[], int, GFX_RECTANGLE* );

#endif /* MOBILE_H */