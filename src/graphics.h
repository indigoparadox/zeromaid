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

#include <stdio.h>

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
#elif defined _ARCH_PPCGR
/* Nintendo Wii */
#include <MLlib.h>
#endif /* USESDL, _ARCH_PPCGR */

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
typedef struct {
   ML_Image* ps_spritedata;
   ML_Sprite* ps_sprite;
} GFX_SURFACE;
typedef struct {
   int x, y, w, h;
} GFX_RECTANGLE;
// XXX
typedef void GFX_COLOR;
#else
#error "No graphics types defined for this platform!"
#endif /* USESDL, USEWII */

/* = Global Variables = */

GFX_RECTANGLE gs_viewport;

/* = Function Prototypes = */

GFX_SURFACE* graphics_create_screen( int, int, int, char* );
GFX_SURFACE* graphics_create_image( char* );
/* GFX_SURFACE* graphics_create_blank( int, int ); */
GFX_SURFACE* graphics_draw_text( char*, char*, int, GFX_COLOR* );
void graphics_draw_blit( GFX_SURFACE*, GFX_SURFACE*, GFX_RECTANGLE*, GFX_RECTANGLE* );
void graphics_draw_blank( GFX_SURFACE*, int, int );
/* void graphics_draw_fadein( GFX_SURFACE *, GFX_SURFACE*, int );
void graphics_draw_fadeout( GFX_SURFACE *, GFX_SURFACE*, int ); */
void graphics_do_update( void );
void graphics_do_free( GFX_SURFACE* );

#endif /* GRAPHICS_H */
