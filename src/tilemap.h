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
#include "event.h"
#include "geometry.h"

#ifdef USECLIENT
#include "graphics.h"
#endif /* USECLIENT */

/* = Definitions = */

#define TILEMAP_ANIM_FRAMES 1 /* Max index of animation frames. */
#define TILEMAP_ANIM_DRAWS 10 /* Max index of drawing cycles per frame. */

typedef int TILEMAP_DIR;
#define TILEMAP_DIR_NULL 0
#define TILEMAP_DIR_NORTH 1
#define TILEMAP_DIR_SOUTH 2
#define TILEMAP_DIR_EAST 3
#define TILEMAP_DIR_WEST 4

/* = Type and Struct Definitions = */

/* We might be able to speed up operations using TILEDATA structs by making   *
 * them tree-able, but there probably won't be very many tiles for a map and  *
 * we feel the simplicity gained this way is worthwhile.                      */
typedef struct _GFX_TILEDATA {
   unsigned int gid;
   short int animated,
      hindrance;
} TILEMAP_TILEDATA;

typedef struct {
   unsigned int pixel_size;

   #ifdef USECLIENT
   GFX_SURFACE* image;
   #endif /* USECLIENT */
   bstring image_filename;

   TILEMAP_TILEDATA* tile_list;
   int tile_list_count,
      serial;
} TILEMAP_TILESET;

typedef struct {
   int gid,
      dirty; /* Should this tile be redrawn next cycle? */
} TILEMAP_TILE;

typedef struct {
   GEO_RECTANGLE* viewport;
   TILEMAP_TILE* tiles; /* Pointer to a flexible array of tiles. */
   bstring proper_name,
      music_path,
      sys_name;
   short int time_moves;
   BYTE light_str;
   TILEMAP_TILESET* tileset;
   int tile_w, tile_h,
      tiles_count; /* Number of tiles in the tile list. */
} TILEMAP_TILEMAP;

/* = Function Prototypes = */

TILEMAP_TILEMAP* tilemap_create_map( bstring, bstring );
TILEMAP_TILESET* tilemap_create_tileset( bstring );
int tilemap_get_tile_x( int, TILEMAP_TILEMAP* );
int tilemap_get_tile_y( int, TILEMAP_TILEMAP* );
int tilemap_dir_get_add_x( TILEMAP_DIR );
int tilemap_dir_get_add_y( TILEMAP_DIR );
TILEMAP_TILE* tilemap_get_tile( int, int, TILEMAP_TILEMAP* );
TILEMAP_TILEDATA* tilemap_get_tiledata( int, TILEMAP_TILESET* );
void tilemap_load_layer( TILEMAP_TILEMAP*, ezxml_t );
void tilemap_draw( TILEMAP_TILEMAP*, GEO_RECTANGLE*, BOOL );
void tilemap_free( TILEMAP_TILEMAP* );
void tilemap_free_tileset( TILEMAP_TILESET* );

#endif /* TILEMAP_H */

