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
//#include "graphics.h"
#include "tilemap.h"

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

#define MOBILE_CROP_SIZE 110 /* The width and height of an image crop rect. */

/* These moveset IDs should be used anywhere in the code where behavior for   *
 * the player mobile will differ based on the member currently active.        */
/* #define MOBILE_MOVESET_HILDA 1
#define MOBILE_MOVESET_ROSETTE 2
#define MOBILE_MOVESET_ALICE 3
#define MOBILE_MOVESET_SHIRLEY 4 */

/* = Type and Struct Definitions = */

typedef int MOBILE_MOVE;

typedef struct {
   bstring id;

   #ifdef USECLIENT
   GFX_SURFACE* image;
   #endif /* USECLIENT */
   bstring image_filename;

   GEO_RECTANGLE image_crop; /* The area to show in an abbreviated dialog. */
} MOBILE_EMOTION;

typedef struct {
   unsigned int pixel_size;

   #ifdef USECLIENT
   GFX_SURFACE* image;
   #endif /* USECLIENT */
   bstring image_filename;

} MOBILE_SPRITESHEET;

typedef struct {
   int opcode;
} MOBILE_AI_LIST;
typedef int MOBILE_AI_LIST_COUNTER;

typedef struct {
   MOBILE_AI_LIST ai_normal;
   MOBILE_AI_LIST_COUNTER ai_normal_counter;

   BOOL moving; /* Is the mobile moving or acting right now? */
   unsigned int hp,
      serial;
   MOBILE_EMOTION* emotions;
   MOBILE_MOVE* moves; /* Specialized actions performable on the field. */
   int pixel_x, pixel_y,
      pixel_size, /* The size of the mobile's sprite. */
      emotions_count,
      emotion_current,
      emotion_x, emotion_y, /* Location on-screen of emotion portrait in VN. */
      current_animation, /* Vertical offset of current frame on spritesheet. */
      moves_count;
   int pixel_mult_percent; /* The size to expand the mobile to when blitted. */
   bstring proper_name, /* The name to display for this mobile. */
      mobile_type; /* The system mobile type of this mobile. */

   #ifdef USECLIENT
   MOBILE_SPRITESHEET* spritesheet;
   #endif /* USECLIENT */
   bstring spritesheet_filename;

} MOBILE_MOBILE;

/* = Function Prototypes = */

MOBILE_SPRITESHEET* mobile_create_spritesheet( bstring );
BOOL mobile_load_mobiles( MOBILE_MOBILE**, int*, ezxml_t, TILEMAP_TILEMAP* );
MOBILE_MOBILE* mobile_load_mobile( bstring );
BOOL mobile_load_emotion( MOBILE_MOBILE*, ezxml_t );
BOOL mobile_load_ai( MOBILE_MOBILE*, MOBILE_AI, bstring );
void mobile_draw( MOBILE_MOBILE*, GEO_RECTANGLE* );
void mobile_execute_ai( MOBILE_MOBILE*, MOBILE_AI );
void mobile_free_arr( MOBILE_MOBILE* );
void mobile_free( MOBILE_MOBILE* );
void mobile_free_spritesheet( MOBILE_SPRITESHEET* );

/* = Macros = */

#define MOBILE_MOBILES_LOAD_FAIL( fail_msg ) \
   DBG_ERR( fail_msg ); \
   mobile_free( ps_mob_iter ); \
   b_success = FALSE; \
   ps_xml_mob_iter = ezxml_next( ps_xml_mob_iter ); \
   continue;

/* = Additional Includes = */

#include "cache.h"

#endif /* MOBILE_H */
