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
#include "graphics.h"
#include "event.h"

/* = Definitions = */

/* = Type and Struct Definitions = */

typedef struct {
   GFX_TILESET* ps_tileset;
   GFX_RECTANGLE* ps_viewport;
} TILEMAP_TILEMAP;

/* = Function Prototypes = */

TILEMAP_TILEMAP* tilemap_create( bstring );
void tilemap_draw( TILEMAP_TILEMAP* );

#endif /* TILEMAP_H */

