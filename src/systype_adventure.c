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
int systype_adventure_loop( void ) {
   int i_act_return = RETURN_ACTION_TITLE;
   bstring ps_map_path = bfromcstr( PATH_SHARE "/mapdata/field_map.tmx" );
   TILEMAP_TILEMAP* ps_map = tilemap_create_map( ps_map_path );
   free( ps_map_path );
   GFX_COLOR* ps_color_fade = graphics_create_color( 0, 0, 0 );
   GFX_COLOR* ps_color_bg = graphics_create_color( 255, 0, 0 );
   GFX_RECTANGLE s_viewport;
   #ifdef USESDL
   #else
   EVENT_TIMER* ps_fps = event_timer_create();
   #endif /* USESDL */
   MOBILE_MOBILE* as_mobile_list;
   int i_mobile_count = 0;

   /* Setup structures we need to run. */
   s_viewport.x = 0;
   s_viewport.y = 0;
   s_viewport.w = GFX_SCREENWIDTH;
   s_viewport.h = GFX_SCREENHEIGHT;

   /* TODO: Check the scratch file for the game we're supposed to load. */

   /* Draw the initial playing field and fade the screen in. */
   tilemap_draw( ps_map, &s_viewport );
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
      switch( event_do_poll() ) {
         case EVENT_ID_UP:
            s_viewport.y -= 32;
            break;

         case EVENT_ID_DOWN:
            s_viewport.y += 32;
            break;

         case EVENT_ID_RIGHT:
            s_viewport.x += 32;
            break;

         case EVENT_ID_LEFT:
            s_viewport.x -= 32;
            break;

         case EVENT_ID_ESC:
         case EVENT_ID_QUIT:
            gps_cache->game_type = SYSTEM_TYPE_TITLE;
            goto sla_cleanup;
      }

      /* Loop through and draw all on-screen items. */
      tilemap_draw( ps_map, &s_viewport );
      mobile_draw_list( as_mobile_list, i_mobile_count, &s_viewport );
      mobile_draw_list(
         gps_cache->player_team,
         gps_cache->player_team_count,
         &s_viewport
      );

      graphics_do_heartbeat();
      graphics_do_update();

      /* If possible, try to delay without busy-spinning. */
      #ifdef USESDL
      SDL_Delay( 1000 / GFX_FPS );
      #else
      while( GFX_FPS > ps_fps->i_ticks_start );
      #endif /* USESDL */
   }

sla_cleanup:

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

   return i_act_return;
}
