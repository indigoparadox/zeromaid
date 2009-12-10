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

#include "systype_adventure.h"

DBG_ENABLE

/* = Global Variables = */

extern CACHE_CACHE* gps_cache;

/* = Functions = */

/* Purpose: Adventure game loop.                                              */
/* Return: The code for the next action to take.                              */
int systype_adventure_loop( bstring ps_map_name_in ) {
   int i_act_return = RETURN_ACTION_TITLE,
      i = 0, /* Loop iterator. */
      i_mobile_count = 0;
   bstring ps_map_path =
      bformat( "%smap_%s_map.tmx", PATH_SHARE , ps_map_name_in->data );
   TILEMAP_TILEMAP* ps_map =
      tilemap_create_map( ps_map_name_in, ps_map_path );
   bdestroy( ps_map_path );
   GFX_COLOR* ps_color_fade = graphics_create_color( 0, 0, 0 );
   GFX_COLOR* ps_color_bg = graphics_create_color( 255, 0, 0 );
   GFX_RECTANGLE s_viewport;
   MOBILE_MOBILE* as_mobile_list;
   #ifndef USESDL
   EVENT_TIMER* ps_fps = event_timer_create();
   #endif /* USESDL */
   EVENT_EVENT s_event;

   /* Setup structures we need to run. */
   s_viewport.x = 0;
   s_viewport.y = 0;
   s_viewport.w = GFX_SCREENWIDTH;
   s_viewport.h = GFX_SCREENHEIGHT;

   /* Load the mobiles for this field. */
   as_mobile_list =
      systype_adventure_load_mobiles( &i_mobile_count, ps_map );

   /* TODO: Check the scratch file for the game we're supposed to load. */

   /* Draw the initial playing field and fade the screen in. */
   tilemap_draw( ps_map, &s_viewport, TRUE );
   //graphics_do_update();
   graphics_draw_transition( GFX_TRANS_FADE_IN, ps_color_fade );
   DBG_INFO( "Running adventure game loop..." );

   while( 1 ) {
      #ifdef USESDL
      /* SDL uses its own sleep function. */
      #else
      /* Run the title screen menu input wait loop. */
      event_timer_start( ps_fps );
      #endif /* USESDL */

      /* Listen for events. */
      event_do_poll( &s_event, TRUE );
      if( s_event.state[EVENT_ID_UP] ) {
         //s_viewport.y -= 32;
         if( !as_mobile_list[0].moving ) {
            as_mobile_list[0].current_animation = MOBILE_ANI_WALK_NORTH;
            systype_adventure_mobile_walk(
               &as_mobile_list[0],
               TILEMAP_DIR_NORTH,
               as_mobile_list,
               i_mobile_count,
               ps_map
            );
         }
      }
      if( s_event.state[EVENT_ID_DOWN] ) {
         //s_viewport.y += 32;
         if( !as_mobile_list[0].moving ) {
            as_mobile_list[0].current_animation = MOBILE_ANI_WALK_SOUTH;
            systype_adventure_mobile_walk(
               &as_mobile_list[0],
               TILEMAP_DIR_SOUTH,
               as_mobile_list,
               i_mobile_count,
               ps_map
            );
         }
      }
      if( s_event.state[EVENT_ID_RIGHT] ) {
         //s_viewport.x += 32;
         if( !as_mobile_list[0].moving ) {
            as_mobile_list[0].current_animation = MOBILE_ANI_WALK_EAST;
            systype_adventure_mobile_walk(
               &as_mobile_list[0],
               TILEMAP_DIR_EAST,
               as_mobile_list,
               i_mobile_count,
               ps_map
            );
         }
      }
      if( s_event.state[EVENT_ID_LEFT] ) {
         //s_viewport.x -= 32;
         if( !as_mobile_list[0].moving ) {
            as_mobile_list[0].current_animation = MOBILE_ANI_WALK_WEST;
            systype_adventure_mobile_walk(
               &as_mobile_list[0],
               TILEMAP_DIR_WEST,
               as_mobile_list,
               i_mobile_count,
               ps_map
            );
         }
      }
      if( s_event.state[EVENT_ID_ESC] || s_event.state[EVENT_ID_QUIT] ) {
         gps_cache->game_type = SYSTEM_TYPE_TITLE;
         goto sal_cleanup;
      }

      /* Move mobiles who are walking. */
      systype_adventure_mobile_walk(
         NULL, TILEMAP_DIR_NULL, as_mobile_list, i_mobile_count, ps_map
      );

      /* All tiles with mobiles on them are dirty. */
      for( i = 0 ; i < i_mobile_count ; i++ ) {
         tilemap_get_tile(
            as_mobile_list[i].pixel_x / ps_map->tileset->pixel_size,
            as_mobile_list[i].pixel_y / ps_map->tileset->pixel_size,
            ps_map
         )->dirty = TRUE;
      }

      /* Loop through and draw all on-screen items. */
      tilemap_draw( ps_map, &s_viewport, FALSE ); /* Map. */
      for( i = 0 ; i < i_mobile_count ; i++ ) {
         mobile_execute_ai( &as_mobile_list[i], MOBILE_AI_ADV_NORMAL );
         mobile_draw( &as_mobile_list[i], &s_viewport ); /* NPCs */
      }
      for( i = 0 ; i < gps_cache->player_team_count ; i++ ) {
         mobile_draw( &gps_cache->player_team[i], &s_viewport ); /* PCs */
      }

      graphics_do_update();

      /* If possible, try to delay without busy-spinning. */
      #ifdef USESDL
      SDL_Delay( 1000 / GFX_FPS );
      #else
      while( GFX_FPS > ps_fps->i_ticks_start );
      #endif /* USESDL */
   }

sal_cleanup:

   /* Fade out the playing field screen. */
   graphics_draw_transition( GFX_TRANS_FADE_OUT, ps_color_fade );

   /* TODO: Perform the between-level autosave. */

   /* Clean up! */
   #ifdef USESDL
   #else
   event_timer_free( ps_fps );
   #endif /* USESDL */
   free( ps_color_fade );
   free( ps_color_bg );
   tilemap_free( ps_map );
   for( i = 0 ; i < i_mobile_count ; i++ ) {
      mobile_free_arr( &as_mobile_list[i] );
   }
   free( as_mobile_list );
   systype_adventure_mobile_walk( NULL, GEN_OPCODE_CLEAN, NULL, -1, NULL );

   return i_act_return;
}

/* Purpose: Start the mobile walking in the given direction. Note that this   *
 *          function does NOT check to see if the mobile is already moving,   *
 *          and problems can arise if it is and it hasn't been accounted for. */
/* Parameters: The mobile to walk, the direction to walk in                   */
/* Return: A boolean indicating whether or not the mobile was able to -start- *
 *         walking.                                                           */
BOOL systype_adventure_mobile_walk(
   MOBILE_MOBILE* ps_mob_in,
   TILEMAP_DIR i_dir_in,
   MOBILE_MOBILE as_mob_npc_in[],
   int i_mob_npc_count_in,
   TILEMAP_TILEMAP* ps_map_in
) {
   static SYSTYPE_ADVENTURE_WALK* tas_walking_ops = NULL; /* Walking mobiles. */
   static int ti_walking_ops_count = 0;
   int i, j, /* Loop iterators. */
      i_speed = 4; /* The speed at which a new walk will start. */
   TILEMAP_TILE* ps_tile_dest = NULL; /* The tile to walk onto. */
   GFX_TILEDATA* ps_tiledata_dest = NULL;

   /* If the special "cleaning" direction was given, then clean up the static *
    * move lists.                                                             */
   if( GEN_OPCODE_CLEAN == i_dir_in ) {
      free( tas_walking_ops );
      return FALSE;
   }

   if( NULL == ps_mob_in && TILEMAP_DIR_NULL == i_dir_in ) {
      /* If this function was called with a NULL direction then go through    *
       * the table of already walking mobiles and advance them.               */
      for( i = 0 ; i < ti_walking_ops_count ; i++ ) {
         /* TODO: Mark the tile being moved off of and the tile being moved onto as dirty. */
         switch( tas_walking_ops[i].direction ) {
            case TILEMAP_DIR_NORTH:
               tas_walking_ops[i].mobile->pixel_y -= tas_walking_ops[i].speed;
               tilemap_get_tile(
                  (tas_walking_ops[i].mobile->pixel_x /
                     ps_map_in->tileset->pixel_size),
                  (tas_walking_ops[i].mobile->pixel_y /
                     ps_map_in->tileset->pixel_size) - 1,
                  ps_map_in
               )->dirty = TRUE;
               break;

            case TILEMAP_DIR_SOUTH:
               tas_walking_ops[i].mobile->pixel_y += tas_walking_ops[i].speed;
               tilemap_get_tile(
                  (tas_walking_ops[i].mobile->pixel_x /
                     ps_map_in->tileset->pixel_size),
                  (tas_walking_ops[i].mobile->pixel_y /
                     ps_map_in->tileset->pixel_size) + 1,
                  ps_map_in
               )->dirty = TRUE;
               break;

            case TILEMAP_DIR_EAST:
               tas_walking_ops[i].mobile->pixel_x += tas_walking_ops[i].speed;
               tilemap_get_tile(
                  (tas_walking_ops[i].mobile->pixel_x /
                     ps_map_in->tileset->pixel_size) + 1,
                  (tas_walking_ops[i].mobile->pixel_y /
                     ps_map_in->tileset->pixel_size),
                  ps_map_in
               )->dirty = TRUE;
               break;

            case TILEMAP_DIR_WEST:
               tas_walking_ops[i].mobile->pixel_x -= tas_walking_ops[i].speed;
               tilemap_get_tile(
                  (tas_walking_ops[i].mobile->pixel_x /
                     ps_map_in->tileset->pixel_size) - 1,
                  (tas_walking_ops[i].mobile->pixel_y /
                     ps_map_in->tileset->pixel_size),
                  ps_map_in
               )->dirty = TRUE;
               break;
         }

         /* If the mobile has reached a tile boundary then stop walking. */
         if(
            0 == tas_walking_ops[i].mobile->pixel_x %
               tas_walking_ops[i].mobile->pixel_size &&
            0 == tas_walking_ops[i].mobile->pixel_y %
               tas_walking_ops[i].mobile->pixel_size
         ) {
            tas_walking_ops[i].mobile->moving = FALSE;

            /* Shift the whole list up by one and then delete the empty last  *
             * entry.                                                         */
            for( j = i + 1 ; j < ti_walking_ops_count ; j++ ) {
               memcpy(
                  &tas_walking_ops[j - 1],
                  &tas_walking_ops[j],
                  sizeof( SYSTYPE_ADVENTURE_WALK )
               );
            }
            ti_walking_ops_count--;
            tas_walking_ops = realloc(
               tas_walking_ops,
               ti_walking_ops_count * sizeof( SYSTYPE_ADVENTURE_WALK )
            );
         }
      }
      return FALSE;
   }

   /* Check for collisions around the given mobile. */
   for( i = 0 ; i < i_mob_npc_count_in ; i++ ) {
      if(
         (TILEMAP_DIR_NORTH == i_dir_in && /* Moving north and mobile is in   */
         as_mob_npc_in[i].pixel_y >        /* closest north tile.             */
            (ps_mob_in->pixel_y - (2 * ps_mob_in->pixel_size)) &&
         as_mob_npc_in[i].pixel_y < ps_mob_in->pixel_y &&
         as_mob_npc_in[i].pixel_x == ps_mob_in->pixel_x) ||

         (TILEMAP_DIR_SOUTH == i_dir_in && /* Moving south and mobile is in   */
         as_mob_npc_in[i].pixel_y <        /* closest south tile.             */
            (ps_mob_in->pixel_y + (2 * ps_mob_in->pixel_size)) &&
         as_mob_npc_in[i].pixel_y >= ps_mob_in->pixel_y + ps_mob_in->pixel_size &&
         as_mob_npc_in[i].pixel_x == ps_mob_in->pixel_x) ||

         (TILEMAP_DIR_EAST == i_dir_in && /* Moving east and mobile is in     */
         as_mob_npc_in[i].pixel_x <       /* closest east tile.               */
            (ps_mob_in->pixel_x + (2 * ps_mob_in->pixel_size)) &&
         as_mob_npc_in[i].pixel_x >= ps_mob_in->pixel_x + ps_mob_in->pixel_size &&
         as_mob_npc_in[i].pixel_y == ps_mob_in->pixel_y) ||

         (TILEMAP_DIR_WEST == i_dir_in && /* Moving west and mobile is in     */
         as_mob_npc_in[i].pixel_x >       /* closest west tile.               */
            (ps_mob_in->pixel_x - (2 * ps_mob_in->pixel_size)) &&
         as_mob_npc_in[i].pixel_x < ps_mob_in->pixel_x &&
         as_mob_npc_in[i].pixel_y == ps_mob_in->pixel_y)
      ) {
         /* A mobile collision occurred, so fail walking. */
         return FALSE;
      }
   }

   /* Figure out the next tile on the map we'll end up on. */
   switch( i_dir_in ) {
      case TILEMAP_DIR_NORTH:
         ps_tile_dest = tilemap_get_tile(
            (ps_mob_in->pixel_x / ps_mob_in->pixel_size),
            (ps_mob_in->pixel_y / ps_mob_in->pixel_size) - 1,
            ps_map_in
         );
         break;

      case TILEMAP_DIR_SOUTH:
         ps_tile_dest = tilemap_get_tile(
            (ps_mob_in->pixel_x / ps_mob_in->pixel_size),
            (ps_mob_in->pixel_y / ps_mob_in->pixel_size) + 1,
            ps_map_in
         );
         break;

      case TILEMAP_DIR_EAST:
         ps_tile_dest = tilemap_get_tile(
            (ps_mob_in->pixel_x / ps_mob_in->pixel_size) + 1,
            (ps_mob_in->pixel_y / ps_mob_in->pixel_size),
            ps_map_in
         );
         break;

      case TILEMAP_DIR_WEST:
         ps_tile_dest = tilemap_get_tile(
            (ps_mob_in->pixel_x / ps_mob_in->pixel_size) - 1,
            (ps_mob_in->pixel_y / ps_mob_in->pixel_size),
            ps_map_in
         );
         break;
   }

   /* Check the map for collisions and terrain slowing. */
   ps_tiledata_dest =
      graphics_get_tiledata( ps_tile_dest->gid, ps_map_in->tileset );
   if( ps_tiledata_dest->hindrance >= ps_map_in->tileset->pixel_size ) {
      /* The hindrance is too high to move. */
      return FALSE;
   }

   /* The specified mobile is neither walking already nor otherwise moving,   *
    * so start walking and add the mobile to the list of walking mobiles.     */
   ti_walking_ops_count++;
   tas_walking_ops = realloc(
      tas_walking_ops,
      ti_walking_ops_count * sizeof( SYSTYPE_ADVENTURE_WALK )
   );
   tas_walking_ops[ti_walking_ops_count - 1].direction = i_dir_in;
   tas_walking_ops[ti_walking_ops_count - 1].mobile = ps_mob_in;
   tas_walking_ops[ti_walking_ops_count - 1].speed =
      i_speed / ps_map_in->tileset->tile_list[ps_tile_dest->gid - 1].hindrance;
   ps_mob_in->moving = TRUE;

   return TRUE;
}

/* Purpose: Load the mobiles for this map into the given dynamic array.       */
/* Parameters: The address of number of mobiles in the output array.          */
/* Return: The address of the output array.                                   */
MOBILE_MOBILE* systype_adventure_load_mobiles(
   int* i_count_out,
   TILEMAP_TILEMAP* ps_map_in
) {
   ezxml_t ps_xml_mobiles = NULL, ps_xml_mob_iter = NULL;
   MOBILE_MOBILE* ps_mobs_out = NULL;
   bstring ps_mob_xml_path =
      bformat( "%smap_%s_mobiles.xml", PATH_SHARE , ps_map_in->sys_name->data );
   bstring ps_mob_iter_path = NULL; /* The path to load iter mob data. */

   *i_count_out = 0;

   /* Verify the XML file exists and open or abort accordingly. */
   if( !file_exists( ps_mob_xml_path ) ) {
      DBG_ERR_STR( "Unable to load mobile list", ps_mob_xml_path->data );
      return NULL;
   }
   ps_xml_mobiles = ezxml_parse_file( ps_mob_xml_path->data );

   ps_xml_mob_iter = ezxml_child( ps_xml_mobiles, "mobile" );
   while( NULL != ps_xml_mob_iter ) {
      /* A tile was found in the XML, so prepare the array for it. */
      *i_count_out += 1;
      ps_mobs_out = realloc(
         ps_mobs_out,
         *i_count_out * sizeof( MOBILE_MOBILE )
      );

      /* Verify the last created map tile's allocation. */
      if( NULL == ps_mobs_out ) {
         DBG_ERR( "Unable to allocate mobile list." );
         goto salm_cleanup;
      }
      memset( &ps_mobs_out[*i_count_out - 1], 0, sizeof( MOBILE_MOBILE ) );

      /* Load the mobile's type. */
      ps_mobs_out[*i_count_out - 1].mobile_type =
         bformat( "%s", ezxml_attr( ps_xml_mob_iter, "type" ) );

      /* Load the mobile's properties. */
      ps_mob_iter_path = bformat(
         "%smob_%s.xml",
         PATH_SHARE,
         ps_mobs_out[*i_count_out - 1].mobile_type->data
      );
      mobile_load_mobile(
         &ps_mobs_out[*i_count_out - 1],
         ps_mob_iter_path
      );
      bdestroy( ps_mob_iter_path );

      /* Load the mobile's position. */
      ps_mobs_out[*i_count_out - 1].pixel_x =
         atoi( ezxml_attr( ps_xml_mob_iter, "startx" ) ) *
         ps_map_in->tileset->pixel_size;
      ps_mobs_out[*i_count_out - 1].pixel_y =
         atoi( ezxml_attr( ps_xml_mob_iter, "starty" ) ) *
         ps_map_in->tileset->pixel_size;

      /* Go to the next one! */
      ps_xml_mob_iter = ezxml_next( ps_xml_mob_iter );
   }

salm_cleanup:
   /* Clean up. */
   ezxml_free( ps_xml_mobiles );

   return ps_mobs_out;
}
