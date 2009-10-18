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
#elif defined USEWII
/* Nintendo Wii */
#include <grrlib.h>
#endif /* USESDL, USEWII */

#include "defines.h"

/* = Definitions = */

#define GFX_FPS 30

/* TODO: Make the screen size variable at runtime. */
#define GFX_SCREENWIDTH 640
#define GFX_SCREENHEIGHT 480
#define GFX_SCREENDEPTH 16

#define GFX_BLACK 0
#define GFX_GREY 1
#define GFX_WHITE 2
#define GFX_RED 3

/* = Type and Struct Definitions = */

#ifdef USESDL
typedef SDL_Surface GFX_SURFACE;
typedef SDL_Rect GFX_RECTANGLE;
typedef SDL_Color GFX_COLOR;
#elif defined USEWII
typedef GRRLIB_texImg GFX_SURFACE;
typedef struct {
   int x, y, w, h;
} GFX_RECTANGLE;
typedef u32 GFX_COLOR;
#else
#error "No graphics types defined for this platform!"
#endif /* USESDL, USEWII */

typedef struct {
   GFX_SURFACE* image;
   unsigned int tile_size;
} GFX_TILESET;

/* = Global Variables = */

/* = Function Prototypes = */

GFX_SURFACE* graphics_create_screen( int, int, int, bstring );
GFX_SURFACE* graphics_create_image( bstring );
GFX_TILESET* graphics_create_tileset( bstring, int );
GFX_COLOR* graphics_create_color( unsigned char, unsigned char, unsigned char );
void graphics_draw_text( int, int, bstring, bstring, int, GFX_COLOR* );
void graphics_draw_blit_tile( GFX_SURFACE*, GFX_RECTANGLE*, GFX_RECTANGLE* );
void graphics_draw_blit_sprite( GFX_SURFACE*, int, int, int );
void graphics_draw_blank( GFX_COLOR* );
void graphics_draw_fadein( GFX_COLOR* );
void graphics_draw_fadeout( GFX_COLOR* );
void graphics_do_update( void );
void graphics_free_image( GFX_SURFACE* );
void graphics_free_tileset( GFX_TILESET* );

#endif /* GRAPHICS_H */
