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

/* Purpose: Adventure game loop.                                              */
/* Return: The code for the next action to take.                              */
int systype_adventure_loop( void ) {
   int i_act_return = RETURN_ACTION_QUIT;
   EVENT_TIMER* ps_fps = event_timer_create();
   bstring ps_map_path = bfromcstr( PATH_SHARE "/mapdata/field_map.tmx" );
   TILEMAP_TILEMAP* ps_map = tilemap_create_map( ps_map_path );
   free( ps_map_path );
   GFX_COLOR* ps_color_fade = graphics_create_color( 0, 0, 0 );
   GFX_COLOR* ps_color_bg = graphics_create_color( 255, 0, 0 );
   GFX_RECTANGLE s_viewport;

   /* Setup structures we need to run. */
   s_viewport.x = 0;
   s_viewport.y = 0;
   s_viewport.w = GFX_SCREENWIDTH;
   s_viewport.h = GFX_SCREENHEIGHT;

   /* TODO: Check the scratch file for the game we're supposed to load. */

   /* Draw the initial playing field and fade the screen in. */
   graphics_draw_blank( ps_color_bg );
   graphics_draw_transition( GFX_TRANS_FADE_IN, ps_color_fade );
   DBG_INFO( "Running adventure game loop..." );

   while( 1 ) {
      /* Run the title screen menu input wait loop. */
      event_timer_start( ps_fps );

      /* Listen for events. */
      switch( event_do_poll() ) {
         case EVENT_ID_QUIT:
            goto sla_cleanup;
      }

      /* Loop through and draw all on-screen items. */
      graphics_draw_blank( ps_color_bg );
      tilemap_draw( ps_map );

      graphics_do_update();

      while( GFX_FPS > ps_fps->i_ticks_start );
   }

sla_cleanup:

   /* Fade out the playing field screen. */
   graphics_draw_transition( GFX_TRANS_FADE_OUT, ps_color_fade );

   /* TODO: Perform the between-level autosave. */

   /* Clean up! */
   event_timer_free( ps_fps );
   free( ps_color_fade );
   free( ps_color_bg );
   free( ps_map );

   return i_act_return;
}
