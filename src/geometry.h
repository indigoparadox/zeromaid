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

#ifndef GEOMETRY_H
#define GEOMETRY_H

/* = Includes = */

/* = Definitions = */

/* TODO: Make the screen size variable at runtime. */
#define GEO_SCREENWIDTH 640
#define GEO_SCREENHEIGHT 480

#define GEO_TILE_DEFAULT_SIZE 32

/* = Type and Struct Definitions = */

#ifdef USESDL
typedef SDL_Color GEO_COLOR;
typedef SDL_Rect GEO_RECTANGLE;
#else
typedef struct { int r, g, b; } GEO_COLOR;
typedef struct { int x, y, w, h; } GEO_RECTANGLE;
#endif /* USESDL */

/* We might be able to speed up operations using TILEDATA structs by making   *
 * them tree-able, but there probably won't be very many tiles for a map and  *
 * we feel the simplicity gained this way is worthwhile.                      */
typedef struct {
   unsigned int gid;
   short int animated,
      hindrance;
} GEO_TILEDATA;

typedef struct {
   unsigned int pixel_size;
   // ZZZ: GFX_SURFACE* image;
   bstring image_filename;
   GEO_TILEDATA* tile_list;
   int tile_list_count;
} GEO_TILESET;

typedef struct {
   unsigned int pixel_size;
   // ZZZ: GFX_SURFACE* image;
   bstring image_filename;
} GEO_SPRITESHEET;

#endif /* GEOMETRY_H */
