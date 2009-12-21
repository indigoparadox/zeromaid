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

#ifndef CACHE_H
#define CACHE_H

/* = Includes = */

#include "defines.h"
#include "mobile.h"

/* = Type and Struct Definitions = */

typedef struct {
   bstring key,
      value;
} CACHE_VARIABLE;

typedef struct {
   bstring text;
} CACHE_LOG_ENTRY;

typedef struct {
   CACHE_VARIABLE* globals, /* Global variables. */
      * locals; /* Local variables. */
   CACHE_LOG_ENTRY* text_log; /* Log of previously displayed text. */
   int globals_count,
      locals_count,
      text_log_count, /* Number of previously spoken text item. */
      player_team_count, /* Number of members in the player's party. */
      player_team_front; /* The team member currently playable. */
   SYSTEM_TYPE game_type;
   bstring map_name; /* Name of the map currently loaded. */
   MOBILE_MOBILE* player_team; /* Dynamic array of mobiles in player's party. */
} CACHE_CACHE;

/* = Macros = */

/* = Function Prototypes = */

BOOL cache_save( bstring );
BOOL cache_load( bstring );

#endif /* CACHE_H */
