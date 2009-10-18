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

void sysloop_adventure( void ) {
   int i_bol_running = 1;
   EVENT_EVENT s_event;
   EVENT_TIMER* ps_fps = event_timer_create();
   bstring ps_tilemap_path = cstr2bstr( PATH_SHARE "/mapdata/field_map.tmx" );
   TILEMAP_TILEMAP* ps_tilemap = tilemap_create( ps_tilemap_path );
   free( ps_tilemap_path );
   GFX_COLOR* ps_color_fade = graphics_create_color( 0, 0, 0 );
   GFX_COLOR* ps_color_bg = graphics_create_color( 255, 0, 0 );
   GFX_RECTANGLE s_viewport;

   /* Setup structures we need to run. */
   s_viewport.x = 0;
   s_viewport.y = 0;
   s_viewport.w = GFX_SCREENWIDTH;
   s_viewport.h = GFX_SCREENHEIGHT;

   /* Draw the initial playing field and fade the screen in. */
   graphics_draw_blank( ps_color_bg );
   graphics_draw_fade( GFX_FADE_IN, ps_color_fade );
   DBG_INFO( "Running adventure game loop..." );

   while( i_bol_running ) {
      /* Run the title screen menu input wait loop. */
      event_timer_start( ps_fps );

      /* Listen for events. */
      event_do_poll( &s_event );
      if( EVENT_ID_QUIT == s_event.type ) {
         break;
      }

      /* Loop through and draw all on-screen items. */
      graphics_draw_blank( ps_color_bg );

      graphics_do_update();

      while( GFX_FPS > ps_fps->i_ticks_start );
   }

   /* Fade out the playing field screen. */
   graphics_draw_fade( GFX_FADE_OUT, ps_color_fade );

   /* TODO: Perform the between-level autosave. */

   /* Clean up! */
   event_timer_free( ps_fps );
   free( ps_color_fade );
   free( ps_color_bg );
   free( ps_tilemap );
}
