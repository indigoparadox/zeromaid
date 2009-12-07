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

#ifndef TILEMAP_H
#define TILEMAP_H

/* = Includes = */

#include "defines.h"
#include "util.h"
#include "graphics.h"
#include "event.h"

/* = Definitions = */

#define TILEMAP_ANIM_FRAMES 1 /* Max index of animation frames. */
#define TILEMAP_ANIM_DRAWS 10 /* Max index of drawing cycles per frame. */

/* = Type and Struct Definitions = */

typedef struct {
   int gid, tile_x, tile_y;
} TILEMAP_TILE;

typedef struct {
   GFX_TILESET* tileset;
   GFX_RECTANGLE* viewport;
   TILEMAP_TILE* tiles; /* Pointer to a flexible array of tiles. */
   bstring map_name, music_path;
   short int time_moves;
   int tile_w, tile_h,
      tiles_count; /* Number of tiles in the tile list. */
} TILEMAP_TILEMAP;

/* = Function Prototypes = */

TILEMAP_TILEMAP* tilemap_create_map( bstring );
void tilemap_load_layer( TILEMAP_TILEMAP*, ezxml_t );
void tilemap_draw( TILEMAP_TILEMAP*, GFX_RECTANGLE* );
void tilemap_free( TILEMAP_TILEMAP* );

#endif /* TILEMAP_H */

