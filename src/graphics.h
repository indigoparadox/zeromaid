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
#elif defined USEWII
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_ttf.h>
#endif /* __APPLE__, __unix__, USEWII */
#elif defined USEDIRECTX
#include <windows.h>
#include <ddraw.h>
#include "ddutil.h"
#elif defined USEALLEGRO
#include <allegro.h>
#endif /* USESDL, USEDIRECTX, USEALLEGRO */

#include "defines.h"
#include "util.h"
#include "geometry.h"
#include "tilemap.h"

/* = Definitions = */

#define GFX_FPS 30

#define GFX_SCREENDEPTH 16

#define GFX_TRANS_FX_FADE 0 /* Simple, modest, fade. */
#define GFX_TRANS_FX_BLIP 1 /* An old CRTube blip effect. */

#define GFX_ALPHA_MAX 255
#define GFX_ALPHA_FADE_INC 25
#define GFX_ALPHA_FADE_STEP 50

#define GFX_BLIP_HEIGHT_INC 10
#define GFX_BLIP_WAIT_TIME 50

/* = Type and Struct Definitions = */

typedef int GFX_TRANS_EFFECT;

#ifdef USESDL
typedef SDL_Surface GFX_SURFACE;
typedef TTF_Font GFX_FONT;
#elif defined USEDIRECTX
typedef struct {
   LPDIRECTDRAWSURFACE7 surface;
   int w,
      h;
} GFX_SURFACE;
#elif defined USEALLEGRO
typedef BITMAP GFX_SURFACE;
typedef FONT GFX_FONT;
#else
#error "No graphics types defined for this platform!"
#endif /* USESDL, USEDIRECTX, USEALLEGRO */

/* = Macros = */

/* If possible, try to delay without busy-spinning. */

#define GFX_SPRITESHEET_WIDTH 96
#define GFX_SPRITESHEET_HEIGHT 256

#ifdef USESDL
   #define GFX_GET_SCREEN_WIDTH SDL_GetVideoSurface()->w
   #define GFX_GET_SCREEN_HEIGHT SDL_GetVideoSurface()->h
#elif defined USEALLEGRO
   #define GFX_GET_SCREEN_WIDTH SCREEN_W
   #define GFX_GET_SCREEN_HEIGHT SCREEN_W
#endif /* USESDL, USEALLEGRO */

#ifdef USESDL
   #define GFX_DRAW_LOOP_DECLARE
   #define GFX_DRAW_LOOP_INIT
   #define GFX_DRAW_LOOP_ENABLE
   #define GFX_DRAW_LOOP_START
   #define GFX_DRAW_LOOP_END SDL_Delay( 1000 / GFX_FPS );
   #define GFX_DRAW_LOOP_FREE
#elif defined USEALLEGRO
   #define GFX_DRAW_LOOP_DECLARE
   #define GFX_DRAW_LOOP_INIT
   #define GFX_DRAW_LOOP_ENABLE
   #define GFX_DRAW_LOOP_START
   #define GFX_DRAW_LOOP_END rest( 1000 / GFX_FPS );
   #define GFX_DRAW_LOOP_FREE
#else
   #define GFX_DRAW_LOOP_DECLARE EVENT_TIMER* gps_fps;
   #define GFX_DRAW_LOOP_INIT gps_fps = event_timer_create();
   #define GFX_DRAW_LOOP_ENABLE extern EVENT_TIMER* gps_fps;
   #define GFX_DRAW_LOOP_START event_timer_start( gps_fps );
   #define GFX_DRAW_LOOP_END while( GFX_FPS > gps_fps->i_ticks_start );
   #define GFX_DRAW_LOOP_FREE event_timer_free( gps_fps );
#endif /* USESDL, USEALLEGRO */

#ifdef USEDIRECTX
   #define PACK_16_BIT( r, g, b ) \
      (((r & 248) << 8) + ((g & 252) << 3) + (b >> 3))
#elif defined USEALLEGRO
   /* #define GFX_PALETTE_DEFAULT default_palette */
#endif /* USEDIRECTX, USEALLEGRO */

/* = Function Prototypes = */

BOOL graphics_create_screen( int, int, int, bstring );
GFX_SURFACE* graphics_create_image( bstring );
GFX_FONT* graphics_create_font( bstring, int );
void graphics_draw_text( int, int, bstring, GFX_FONT*, GEO_COLOR* );
void graphics_draw_blit_tile( GFX_SURFACE*, GEO_RECTANGLE*, GEO_RECTANGLE* );
void graphics_draw_blit_sprite( GFX_SURFACE*, GEO_RECTANGLE*, GEO_RECTANGLE* );
void graphics_draw_blank( GEO_COLOR* );
void graphics_draw_transition( GFX_TRANS_EFFECT, int, GEO_COLOR* );
void graphics_do_update( void );
int graphics_get_font_height( GFX_FONT* );
void graphics_free_image( GFX_SURFACE* );
void graphics_free_font( GFX_FONT* );

#endif /* GRAPHICS_H */
