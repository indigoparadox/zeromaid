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

/* = Definitions = */

/* TODO: Make the screen size variable at runtime. */
#define GEO_SCREENWIDTH 640
#define GEO_SCREENHEIGHT 480

#define GEO_TILE_DEFAULT_SIZE 32

/* = Type and Struct Definitions = */

#ifdef USESDL
typedef SDL_Rect GEO_RECTANGLE;
typedef SDL_Color GEO_COLOR;
#else
typedef struct { int x, y, w, h; } GEO_RECTANGLE;
typedef struct { int r, g, b; } GEO_COLOR;
#endif /* USESDL */

/* = Function Prototypes = */

GEO_COLOR* geometry_create_color( unsigned char, unsigned char, unsigned char );
GEO_COLOR* geometry_create_color_html( bstring );

#endif /* GEOMETRY_H */
