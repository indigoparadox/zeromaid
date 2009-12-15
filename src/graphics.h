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

#ifndef GRAPHICS_H
#define GRAPHICS_H

/* = Includes = */

#ifdef USESDL
#ifdef __APPLE__
#include <SDL/SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#elif defined __unix__
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#endif /* __APPLE__, __unix__ */
#elif defined USEDIRECTX
#include <windows.h>
#include <ddraw.h>
#include "ddutil.h"
#endif /* USESDL, USEDIRECTX */

#include "defines.h"
#include "util.h"

/* = Definitions = */

#define GFX_FPS 30

/* TODO: Make the screen size variable at runtime. */
#define GFX_SCREENWIDTH 640
#define GFX_SCREENHEIGHT 480
#define GFX_SCREENDEPTH 16

#define GFX_TRANS_FADE_IN 0
#define GFX_TRANS_FADE_OUT 1

#define GFX_ALPHA_MAX 255
#define GFX_ALPHA_FADE_INC 25
#define GFX_ALPHA_FADE_STEP 50

#define GFX_TILE_DEFAULT_SIZE 32

/* = Type and Struct Definitions = */

#ifdef USESDL
typedef SDL_Surface GFX_SURFACE;
typedef SDL_Rect GFX_RECTANGLE;
typedef SDL_Color GFX_COLOR;
#elif defined USEDIRECTX
typedef struct {
   LPDIRECTDRAWSURFACE7 surface;
   int w,
      h;
} GFX_SURFACE;
typedef struct { int x, y, w, h; } GFX_RECTANGLE;
typedef struct { int r, g, b; } GFX_COLOR;
#else
#error "No graphics types defined for this platform!"
#endif /* USESDL */

/* We might be able to speed up operations using TILEDATA structs by making   *
 * them tree-able, but there probably won't be very many tiles for a map and  *
 * we feel the simplicity gained this way is worthwhile.                      */
typedef struct _GFX_TILEDATA {
   unsigned int gid;
   short int animated,
      hindrance;
} GFX_TILEDATA;

typedef struct {
   unsigned int pixel_size;
   GFX_SURFACE* image;
   GFX_TILEDATA* tile_list;
   int tile_list_count;
} GFX_TILESET;

typedef struct {
   unsigned int pixel_size;
   GFX_SURFACE* image;
} GFX_SPRITESHEET;

/* = Macros = */

/* If possible, try to delay without busy-spinning. */

#ifdef USESDL

#define GFX_DRAW_LOOP_DECLARE
#define GFX_DRAW_LOOP_INIT
#define GFX_DRAW_LOOP_ENABLE
#define GFX_DRAW_LOOP_START
#define GFX_DRAW_LOOP_END SDL_Delay( 1000 / GFX_FPS );
#define GFX_DRAW_LOOP_FREE

#else

#define GFX_DRAW_LOOP_DECLARE EVENT_TIMER* gps_fps;
#define GFX_DRAW_LOOP_INIT gps_fps = event_timer_create();
#define GFX_DRAW_LOOP_ENABLE extern EVENT_TIMER* gps_fps;
#define GFX_DRAW_LOOP_START event_timer_start( gps_fps );
#define GFX_DRAW_LOOP_END while( GFX_FPS > gps_fps->i_ticks_start );
#define GFX_DRAW_LOOP_FREE event_timer_free( gps_fps );

#endif /* USESDL */

#ifdef USEDIRECTX
#define PACK_16_BIT( r, g, b ) \
   (((r & 248) << 8) + ((g & 252) << 3) + (b >> 3))
#endif /* USEDIRECTX */

/* = Function Prototypes = */

BOOL graphics_create_screen( int, int, int, bstring );
GFX_SURFACE* graphics_create_image( bstring );
GFX_SPRITESHEET* graphics_create_spritesheet( bstring );
GFX_TILESET* graphics_create_tileset( bstring );
GFX_COLOR* graphics_create_color( unsigned char, unsigned char, unsigned char );
GFX_COLOR* graphics_create_color_html( bstring );
void graphics_draw_text( int, int, bstring, bstring, int, GFX_COLOR* );
void graphics_draw_blit_tile( GFX_SURFACE*, GFX_RECTANGLE*, GFX_RECTANGLE* );
void graphics_draw_blit_sprite( GFX_SURFACE*, GFX_RECTANGLE*, GFX_RECTANGLE* );
void graphics_draw_blank( GFX_COLOR* );
void graphics_draw_transition( int, GFX_COLOR* );
void graphics_do_update( void );
GFX_TILEDATA* graphics_get_tiledata( int, GFX_TILESET* );
void graphics_free_image( GFX_SURFACE* );
void graphics_free_spritesheet( GFX_SPRITESHEET* );
void graphics_free_tileset( GFX_TILESET* );

#endif /* GRAPHICS_H */
