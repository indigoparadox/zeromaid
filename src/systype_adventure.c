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

   /* Setup the loading title and image, then blit them onto one surface. */
   /* DBG_OUT( "Setting up the title image..." );
   bstring ps_path_title = cstr2bstr( PATH_SHARE "/" FILE_TITLE );
   ps_title_image = graphics_create_image( ps_path_title );
   bdestroy( ps_path_title ); */

   // Show the title screen until the user wants to quit.
   DBG_OUT( "Running adventure game loop..." );


   while( i_bol_running ) {
      /* Once the field is initially drawn, fade the screen in. */
      // XXX: Coolor struct generation macro.
      //graphics_draw_fadein(  );

      /* Run the title screen menu input wait loop. */
      event_timer_start( ps_fps );

      /* Listen for events. */
      event_do_poll( &s_event );
      if( EVENT_ID_QUIT == s_event.type ) {
         break;
      }

      /* Loop through and draw all on-screen items. */
      GFX_COLOR* ps_color_bg = graphics_create_color( 80, 0, 0 );
      graphics_draw_blank( ps_color_bg );
      free( ps_color_bg );

      graphics_do_update();

      while( GFX_FPS > ps_fps->i_ticks_start );
   }

   /* Fade out the playing field screen. */
   // XXX

   // Clean up!
   event_timer_free( ps_fps );
}
