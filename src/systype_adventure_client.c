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

#undef SYSTYPE_ADVENTURE_CLIENT_H
#include "systype_adventure_client.h"

DBG_ENABLE
GFX_DRAW_LOOP_ENABLE
TITLE_ERROR_ENABLE

/* Purpose: Adventure game loop.                                              */
/* Return: The code for the next action to take.                              */
int systype_adventure_client_loop( CACHE_CACHE* ps_cache_in ) {
   int i_act_return = RETURN_ACTION_TITLE,
      i = 0, /* Loop iterator. */
      i_mobile_count = 0; /* Counter for the NPC array below. */
   bstring ps_path_map = NULL,
      ps_path_mobiles = NULL;
   ezxml_t ps_xml_mobiles = NULL;
   TILEMAP_TILEMAP* ps_map = NULL;
   GEO_COLOR* ps_color_fade = NULL;
   GEO_RECTANGLE s_viewport;
   MOBILE_MOBILE* as_mobile_list; /* An array of NPCs and other mobiles. */
   EVENT_EVENT s_event;
   WINDOW_MENU* ps_menu = NULL;

   /* Initialize what we need to use functions to initialize. */
   memset( &s_event, 0, sizeof( EVENT_EVENT ) );
   ps_path_map =
      bformat( "%s%s.tmx", PATH_SHARE , ps_cache_in->map_name->data );
   ps_map = tilemap_create_map( ps_cache_in->map_name, ps_path_map );
   bdestroy( ps_path_map );
   ps_color_fade = geometry_create_color( 0, 0, 0 );

   /* Verify the sanity of loaded data. */
   if( NULL == ps_map ) {
      TITLE_ERROR_SET( "Unable to load adventure map." );
      goto sal_cleanup;
   }

   /* Setup structures we need to run. */
   s_viewport.x = 0;
   s_viewport.y = 0;
   s_viewport.w = GEO_SCREENWIDTH;
   s_viewport.h = GEO_SCREENHEIGHT;

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
   graphics_draw_transition( GFX_TRANS_FX_FADE, TRANS_FADE_IN, ps_color_fade );
   DBG_INFO( "Running adventure game loop..." );

   /* DEBUG */
   /* ps_cache_in->player_team_front = 0;
   ps_cache_in->player_team_count = 0;
   ps_cache_in->player_team = calloc( 1, sizeof( MOBILE_MOBILE ) );
   memcpy( &ps_cache_in->player_team[0], &as_mobile_list[0], sizeof( MOBILE_MOBILE ) );
   ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_x +=
      ps_map->tileset->pixel_size; */

   while( 1 ) {
      GFX_DRAW_LOOP_START

      /* Listen for events. */
      event_do_poll( &s_event, TRUE );
      if( s_event.state[EVENT_ID_MENU] ) {
         if( NULL == ps_menu ) {
            systype_adventure_menu_show( &ps_menu, ps_cache_in );
         }
      }
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
      if( s_event.state[EVENT_ID_CANCEL] || s_event.state[EVENT_ID_QUIT] ) {
         CACHE_SYSTEM_TYPE_SET( ps_cache_in, SYSTEM_TYPE_TITLE );
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
         systype_adventure_client_viewport_scroll(
            &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map,
            ps_menu, TILEMAP_DIR_NORTH
         );
      } else if(
         ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_y >=
         s_viewport.y + s_viewport.h
      ) { /* South */
         systype_adventure_client_viewport_scroll(
            &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map,
            ps_menu, TILEMAP_DIR_SOUTH
         );
      } else if(
         ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_x >=
         s_viewport.x + s_viewport.w
      ) { /* East */
         systype_adventure_client_viewport_scroll(
            &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map,
            ps_menu, TILEMAP_DIR_EAST
         );
      } else if(
         ps_cache_in->player_team[ps_cache_in->player_team_front].pixel_x <
         s_viewport.x
      ) { /* West */
         systype_adventure_client_viewport_scroll(
            &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map,
            ps_menu, TILEMAP_DIR_WEST
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
      systype_adventure_client_viewport_draw(
         &s_viewport, ps_cache_in, as_mobile_list, i_mobile_count, ps_map,
         ps_menu, FALSE
      );

      GFX_DRAW_LOOP_END
   }

sal_cleanup:

   /* Fade out the playing map screen. */
   graphics_draw_transition( GFX_TRANS_FX_FADE, TRANS_FADE_OUT, ps_color_fade );

   /* Clean up! */
   #ifndef USEWII
   /* XXX: The Wii doesn't like the way we free things... This could be a     *
    *      problem.                                                           */
   free( ps_color_fade );
   tilemap_free( ps_map );
   for( i = 0 ; i < i_mobile_count ; i++ ) {
      mobile_free_arr( &as_mobile_list[i] );
   }
   free( as_mobile_list );
   systype_adventure_mobile_walk( NULL, GEN_OPCODE_CLEAN, NULL, -1, NULL );
   #endif /* USEWII */

   return i_act_return;
}

/* Purpose: Scroll the screen in the given direction, pausing the game but    *
 *          continuing to draw and refresh, a la the old Zelda games for the  *
 *          NES.                                                              */
void systype_adventure_client_viewport_scroll(
   GEO_RECTANGLE* ps_viewport_in,
   CACHE_CACHE* ps_cache_in,
   MOBILE_MOBILE as_mobiles_in[],
   int i_mobiles_count_in,
   TILEMAP_TILEMAP* ps_map_in,
   WINDOW_MENU* ps_menu_in,
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

      systype_adventure_client_viewport_draw(
         ps_viewport_in, ps_cache_in, as_mobiles_in, i_mobiles_count_in, ps_map_in,
         ps_menu_in, TRUE
      );

      GFX_DRAW_LOOP_END
   }

}

/* Purpose: Draw the current viewport.                                        */
void systype_adventure_client_viewport_draw(
   GEO_RECTANGLE* ps_viewport_in,
   CACHE_CACHE* ps_cache_in,
   MOBILE_MOBILE as_mobiles_in[],
   int i_mobiles_count_in,
   TILEMAP_TILEMAP* ps_map_in,
   WINDOW_MENU* ps_menu_in,
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

   /* Draw windows and menus. */
   /* TODO: Display a status window at the bottom of the screen most of the   *
    *       time until there's something new to read in the text log. Also,   *
    *       implement scrollback.                                             */
   /* window_draw_text( 0, ps_cache_in ); */
   if( NULL != ps_menu_in ) {
      window_draw_menu( ps_menu_in );
   }

   graphics_do_update();
}
