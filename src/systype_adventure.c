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
GFX_DRAW_LOOP_ENABLE
TITLE_ERROR_ENABLE

/* = Functions = */

/* Purpose: Adventure game loop.                                              */
/* Return: The code for the next action to take.                              */
int systype_adventure_loop( CACHE_CACHE* ps_cache_in ) {
   int i_act_return = RETURN_ACTION_TITLE,
      i = 0, /* Loop iterator. */
      i_mobile_count = 0; /* Counter for the NPC array below. */
   bstring ps_path_map = NULL,
      ps_path_mobiles = NULL;
   ezxml_t ps_xml_mobiles = NULL;
   TILEMAP_TILEMAP* ps_map = NULL;
   GFX_COLOR* ps_color_fade = NULL;
   GFX_RECTANGLE s_viewport;
   MOBILE_MOBILE* as_mobile_list; /* An array of NPCs and other mobiles. */
   EVENT_EVENT s_event;

   /* Initialize what we need to use functions to initialize. */
   memset( &s_event, 0, sizeof( EVENT_EVENT ) );
   ps_path_map =
      bformat( "%s%s.tmx", PATH_SHARE , ps_cache_in->map_name->data );
   ps_map = tilemap_create_map( ps_cache_in->map_name, ps_path_map );
   bdestroy( ps_path_map );
   ps_color_fade = graphics_create_color( 0, 0, 0 );

   /* Verify the sanity of loaded data. */
   if( NULL == ps_map ) {
      TITLE_ERROR_SET( "Unable to load adventure map." );
      goto sal_cleanup;
   }

   /* Setup structures we need to run. */
   s_viewport.x = 0;
   s_viewport.y = 0;
   s_viewport.w = GFX_SCREENWIDTH;
   s_viewport.h = GFX_SCREENHEIGHT;

   /* Load the mobiles for this map. */
   ps_path_mobiles = bformat(
      "%smap_%s_mobiles.xml",
      PATH_SHARE,
      ps_cache_in->map_name->data
   );
   ps_xml_mobiles = ezxml_parse_file( (const char*)ps_path_mobiles );
   /* TODO: Make sure mobiles loaded correctly. */
   mobile_load_mobiles(
      &as_mobile_list, &i_mobile_count, ps_xml_mobiles, ps_map
   );
   bdestroy( ps_path_mobiles );
   ezxml_free( ps_xml_mobiles );

   /* Draw the initial playing map and fade the screen in. */
   tilemap_draw( ps_map, &s_viewport, TRUE );
   graphics_draw_transition( GFX_TRANS_FADE_IN, ps_color_fade );
   DBG_INFO( "Running adventure game loop..." );

   /* DEBUG */
   ps_cache_in->player_team_front = 0;
   ps_cache_in->player_team_count = 0;
   ps_cache_in->player_team = calloc( 1, sizeof( MOBILE_MOBILE ) );
   memcpy( &ps_cache_in->player_team[0], &as_mobile_list[0], sizeof( MOBILE_MOBILE ) );
   ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_x +=
      ps_map->tileset->pixel_size;

   while( 1 ) {
      GFX_DRAW_LOOP_START

      /* Listen for events. */
      event_do_poll( &s_event, TRUE );
      if( s_event.state[EVENT_ID_UP] ) {
         //s_viewport.y -= 32;
         if( !ps_cache_in->player_team[ps_cache_in->player_team_front].moving ) {
            ps_cache_in->player_team[ps_cache_in->player_team_front].
               current_animation = MOBILE_ANI_WALK_NORTH;
            systype_adventure_mobile_walk(
               &ps_cache_in->player_team[ps_cache_in->player_team_front],
               TILEMAP_DIR_NORTH,
               as_mobile_list,
               i_mobile_count,
               ps_map
            );
         }
      }
      if( s_event.state[EVENT_ID_DOWN] ) {
         //s_viewport.y += 32;
         if( !ps_cache_in->player_team[ps_cache_in->player_team_front].moving ) {
            ps_cache_in->player_team[ps_cache_in->player_team_front].
               current_animation = MOBILE_ANI_WALK_SOUTH;
            systype_adventure_mobile_walk(
               &ps_cache_in->player_team[ps_cache_in->player_team_front],
               TILEMAP_DIR_SOUTH,
               as_mobile_list,
               i_mobile_count,
               ps_map
            );
         }
      }
      if( s_event.state[EVENT_ID_RIGHT] ) {
         //s_viewport.x += 32;
         if( !ps_cache_in->player_team[ps_cache_in->player_team_front].moving ) {
            ps_cache_in->player_team[ps_cache_in->player_team_front].
               current_animation = MOBILE_ANI_WALK_EAST;
            systype_adventure_mobile_walk(
               &ps_cache_in->player_team[ps_cache_in->player_team_front],
               TILEMAP_DIR_EAST,
               as_mobile_list,
               i_mobile_count,
               ps_map
            );
         }
      }
      if( s_event.state[EVENT_ID_LEFT] ) {
         //s_viewport.x -= 32;
         if( !ps_cache_in->player_team[ps_cache_in->player_team_front].moving ) {
            ps_cache_in->player_team[ps_cache_in->player_team_front].
               current_animation = MOBILE_ANI_WALK_WEST;
            systype_adventure_mobile_walk(
               &ps_cache_in->player_team[ps_cache_in->player_team_front],
               TILEMAP_DIR_WEST,
               as_mobile_list,
               i_mobile_count,
               ps_map
            );
         }
      }
      if( s_event.state[EVENT_ID_ESC] || s_event.state[EVENT_ID_QUIT] ) {
         ps_cache_in->game_type = SYSTEM_TYPE_TITLE;
         goto sal_cleanup;
      }

      /* Move mobiles who are walking. */
      systype_adventure_mobile_walk(
         NULL, TILEMAP_DIR_NULL, as_mobile_list, i_mobile_count, ps_map
      );

      /* If the player is out of the viewport then scroll the viewport to     *
       * accomodate.                                                          */
      if(
         ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_y <
         s_viewport.y
      ) { /* North */
         systype_adventure_viewport_scroll(
            &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map,
            TILEMAP_DIR_NORTH
         );
      } else if(
         ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_y >=
         s_viewport.y + s_viewport.h
      ) { /* South */
         systype_adventure_viewport_scroll(
            &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map,
            TILEMAP_DIR_SOUTH
         );
      } else if(
         ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_x >=
         s_viewport.x + s_viewport.w
      ) { /* East */
         systype_adventure_viewport_scroll(
            &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map,
            TILEMAP_DIR_EAST
         );
      } else if(
         ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_x <
         s_viewport.x
      ) { /* West */
         systype_adventure_viewport_scroll(
            &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map,
            TILEMAP_DIR_WEST
         );
      }

      /* All tiles with mobiles on them are dirty. */
      for( i = 0 ; i < i_mobile_count ; i++ ) {
         tilemap_get_tile(
            as_mobile_list[i].pixel_x / ps_map->tileset->pixel_size,
            as_mobile_list[i].pixel_y / ps_map->tileset->pixel_size,
            ps_map
         )->dirty = TRUE;
      }
      tilemap_get_tile(
         ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_x /
            ps_map->tileset->pixel_size,
         ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_y /
            ps_map->tileset->pixel_size,
         ps_map
      )->dirty = TRUE;

      /* Draw the viewport. */
      systype_adventure_viewport_draw(
         &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map, FALSE
      );

      GFX_DRAW_LOOP_END
   }

sal_cleanup:

   /* Fade out the playing map screen. */
   graphics_draw_transition( GFX_TRANS_FADE_OUT, ps_color_fade );

   /* Clean up! */
   free( ps_color_fade );
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
      i_speed = 4, /* The speed at which a new walk will start. */
      i_tile_px_sz = 0; /* Pixel size of the given map's tiles. */
   TILEMAP_TILE* ps_tile_dest = NULL; /* The tile to walk onto. */
   GFX_TILEDATA* ps_tiledata_dest = NULL;

   /* Set the tile pixel size if we can. */
   if( NULL != ps_map_in ) {
      i_tile_px_sz = ps_map_in->tileset->pixel_size;
   }

   /* If the special "cleaning" direction was given, then clean up the static *
    * move lists.                                                             */
   if( GEN_OPCODE_CLEAN == i_dir_in ) {
      free( tas_walking_ops );
      return FALSE;
   }

   /* We're not allowed to go off the edge of the map. */
   if(
      (TILEMAP_DIR_NORTH == i_dir_in && ps_mob_in->pixel_y <= 0) ||

      (TILEMAP_DIR_SOUTH == i_dir_in &&
      ps_mob_in->pixel_y >= (ps_map_in->tile_h * i_tile_px_sz) - i_tile_px_sz) ||

      (TILEMAP_DIR_EAST == i_dir_in &&
      ps_mob_in->pixel_x >= (ps_map_in->tile_w * i_tile_px_sz) - i_tile_px_sz) ||

      (TILEMAP_DIR_WEST == i_dir_in && ps_mob_in->pixel_x <= 0)
   ) {
      return FALSE;
   }

   if( NULL == ps_mob_in && TILEMAP_DIR_NULL == i_dir_in ) {
      /* If this function was called with a NULL direction then go through    *
       * the table of already walking mobiles and advance them.               */
      for( i = 0 ; i < ti_walking_ops_count ; i++ ) {
         /* Move the mobile by the iterated movement's speed. */

         tas_walking_ops[i].mobile->pixel_x +=
            (tilemap_dir_get_add_x( tas_walking_ops[i].direction ) *
            tas_walking_ops[i].speed);
         tas_walking_ops[i].mobile->pixel_y +=
            (tilemap_dir_get_add_y( tas_walking_ops[i].direction ) *
            tas_walking_ops[i].speed);

         /* Mark the tile being moved onto as dirty. */
         tilemap_get_tile(
            (tas_walking_ops[i].mobile->pixel_x /
               ps_map_in->tileset->pixel_size) +
                  tilemap_dir_get_add_x( tas_walking_ops[i].direction ),
            (tas_walking_ops[i].mobile->pixel_y /
               ps_map_in->tileset->pixel_size) +
                  tilemap_dir_get_add_y( tas_walking_ops[i].direction ),
            ps_map_in
         )->dirty = TRUE;

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
            tas_walking_ops = (SYSTYPE_ADVENTURE_WALK*)realloc(
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
   ps_tile_dest = tilemap_get_tile(
      (ps_mob_in->pixel_x / ps_mob_in->pixel_size) +
         tilemap_dir_get_add_x( i_dir_in ),
      (ps_mob_in->pixel_y / ps_mob_in->pixel_size) +
         tilemap_dir_get_add_y( i_dir_in ),
      ps_map_in
   );

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
   tas_walking_ops = (SYSTYPE_ADVENTURE_WALK*)realloc(
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

void systype_adventure_viewport_scroll(
   GFX_RECTANGLE* ps_viewport_in,
   CACHE_CACHE* ps_cache_in,
   MOBILE_MOBILE as_mobiles_in[],
   int i_mobiles_count_in,
   TILEMAP_TILEMAP* ps_map_in,
   TILEMAP_DIR i_dir_in
) {
   /* Figure out the desired X and Y of the viewport. */
   int i_x_dest = ps_viewport_in->x +
         (ps_viewport_in->w * tilemap_dir_get_add_x( i_dir_in )),
      i_y_dest = ps_viewport_in->y +
         (ps_viewport_in->h * tilemap_dir_get_add_y( i_dir_in ));

   /* Move the viewport. */
   while( ps_viewport_in->x != i_x_dest || ps_viewport_in->y != i_y_dest ) {
      GFX_DRAW_LOOP_START

      ps_viewport_in->x +=
         (tilemap_dir_get_add_x( i_dir_in ) * SYSTYPE_ADVENTURE_SCROLL_SPEED);
      ps_viewport_in->y +=
         (tilemap_dir_get_add_y( i_dir_in ) * SYSTYPE_ADVENTURE_SCROLL_SPEED);

      systype_adventure_viewport_draw(
         ps_viewport_in, ps_cache_in, as_mobiles_in, i_mobiles_count_in, ps_map_in, TRUE
      );

      GFX_DRAW_LOOP_END
   }

}

/* Purpose: Draw the current viewport.                                        */
void systype_adventure_viewport_draw(
   GFX_RECTANGLE* ps_viewport_in,
   CACHE_CACHE* ps_cache_in,
   MOBILE_MOBILE as_mobiles_in[],
   int i_mobiles_count_in,
   TILEMAP_TILEMAP* ps_map_in,
   BOOL b_force_redraw_in
) {
   int i; /* Loop iterator. */

   /* Loop through and draw all on-screen items. */
   tilemap_draw( ps_map_in, ps_viewport_in, b_force_redraw_in ); /* Map. */
   for( i = 0 ; i < i_mobiles_count_in ; i++ ) {
      mobile_execute_ai( &as_mobiles_in[i], MOBILE_AI_ADV_NORMAL );
      mobile_draw( &as_mobiles_in[i], ps_viewport_in ); /* NPCs */
   }
   mobile_draw(
      &ps_cache_in->player_team[ps_cache_in->player_team_front], ps_viewport_in
   ); /* PCs */

   graphics_do_update();
}
