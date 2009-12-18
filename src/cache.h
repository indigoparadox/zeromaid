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
   bstring var_keys, /* Global variable keys. */
      var_values, /* Parallel to var_keys, global variable values. */
      * text_log; /* Log of previously spoken text. */
   int var_count, /* Global variable count. */
      text_log_count, /* Number of previously spoken text item. */
      player_team_count, /* Number of members in the player's party. */
      player_team_front; /* The team member currently playable. */
   SYSTEM_TYPE game_type;
   bstring map_name; /* Name of the map currently loaded. */
   MOBILE_MOBILE* player_team; /* Dynamic array of mobiles in player's party. */
} CACHE_CACHE;

/* = Macros = */

/* = Function Prototypes = */

short int cache_save( bstring );
short int cache_load( bstring );

#endif /* CACHE_H */
