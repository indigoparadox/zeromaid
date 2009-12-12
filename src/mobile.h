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

#ifndef MOBILE_H
#define MOBILE_H

/* = Includes = */

#include "defines.h"
#include "graphics.h"

/* = Defines = */

typedef int MOBILE_AI;
#define MOBILE_AI_ADV_NORMAL 0
#define MOBILE_AI_ADV_ATTACKED 1

#define MOBILE_ANI_WALK_SOUTH 0
#define MOBILE_ANI_WALK_NORTH 1
#define MOBILE_ANI_WALK_EAST 2
#define MOBILE_ANI_WALK_WEST 3
#define MOBILE_ANI_ATTACKING_SOUTH 4
#define MOBILE_ANI_ATTACKING_NORTH 5
#define MOBILE_ANI_ATTACKING_EAST 6
#define MOBILE_ANI_ATTACKING_WEST 7
#define MOBILE_ANI_ATTACKED_SOUTH 8
#define MOBILE_ANI_ATTACKED_NORTH 9
#define MOBILE_ANI_ATTACKED_EAST 10
#define MOBILE_ANI_ATTACKED_WEST 11

#define MOBILE_ANIM_FRAMES 3 /* Max index of animation frames. */
#define MOBILE_ANIM_DRAWS 20 /* Max index of drawing cycles per frame. */

/* = Type and Struct Definitions = */

typedef struct {
   GFX_SURFACE* image;
} MOBILE_PORTRAIT;

typedef struct {
   int opcode;
} MOBILE_AI_LIST;
typedef int MOBILE_AI_LIST_COUNTER;

typedef struct {
   MOBILE_AI_LIST ai_normal;
   MOBILE_AI_LIST_COUNTER ai_normal_counter;

   BOOL moving; /* Is the mobile moving or acting right now? */
   unsigned int hp;
   MOBILE_PORTRAIT* portraits;
   int pixel_x, pixel_y,
      pixel_size, /* The size of the mobile's sprite. */
      portraits_count,
      current_animation; /* Vertical offset of current frame on spritesheet. */
   float pixel_multiplier; /* The size to expand the mobile to when blitted. */
   bstring proper_name, /* The name to display for this mobile. */
      mobile_type; /* The system mobile type of this mobile. */
   GFX_SPRITESHEET* spritesheet;
} MOBILE_MOBILE;

/* = Function Prototypes = */

void mobile_load_mobile( MOBILE_MOBILE*, bstring );
void mobile_load_ai( MOBILE_MOBILE*, MOBILE_AI, bstring );
void mobile_draw( MOBILE_MOBILE*, GFX_RECTANGLE* );
void mobile_execute_ai( MOBILE_MOBILE*, MOBILE_AI );
void mobile_free_arr( MOBILE_MOBILE* );
void mobile_free( MOBILE_MOBILE* );

/* = Additional Includes = */
#include "cache.h"

#endif /* MOBILE_H */
