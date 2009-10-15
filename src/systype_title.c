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

#include "systype_title.h"

void sysloop_title( void ) {
   int i_bol_running = 1, i_command = COMMAND_NONE;
   EVENT_EVENT s_event;
   EVENT_TIMER* ps_fps = event_timer_create();

   /* These are the surfaces for drawing the title screen. */
   GFX_SURFACE* ps_title_image = NULL;
   GFX_RECTANGLE s_blit_dest;
   GFX_RECTANGLE s_blit_error;

   /* Setup the loading title and image, then blit them onto one surface. */
   DBG_OUT( "Setting up the title image..." );
   ps_title_image = graphics_create_image( PATH_SHARE "/" FILE_TITLE );
   //p_surTitleText = new agfx::SurfaceCore( "Maid Quest", strTitleFont, 48, agfx::BLACK );
   //p_surSubtText  = new agfx::SurfaceCore( "Quest of the maid", strTitleFont, 18, agfx::BLACK );

   // Setup the error message text, in case we need it.
   /* p_surErrorText = new agfx::SurfaceCore( "There was an error starting Maid Quest!", strTitleFont, 18, agfx::RED );
   rctBlitError.set_x( 50 );
   rctBlitError.set_y( 150 );
   rctBlitError.set_width( p_surErrorText->get_width() );
   rctBlitError.set_height( p_surErrorText->get_height() ); */

   // Show the title screen until the user wants to quit.
   DBG_OUT( "Running title screen loop..." );

   while( i_bol_running ) {
      // Fade in the title screen.
      //p_scrScreen->do_fadein( p_surTitleScreen, agfx::BLACK );

      /* Run the title screen menu input wait loop. */
      while( COMMAND_NONE == i_command ) {
         event_timer_start( ps_fps );

         /* Listen for events. */
         event_do_poll( &s_event );
         switch( s_event.type ) {
            case EVENT_ID_QUIT:
               i_command = COMMAND_QUIT;
               break;

               /* case agfx::EVT_KEYESCAPE:
                  intTitleCommand = TCMD_QUIT;
                  break;

               case agfx::EVT_KEYENTER:
                  intTitleCommand = TCMD_STORY;
                  break; */
         }

         /* Loop through and draw all on-screen items. */
         graphics_draw_blit( ps_title_image, NULL, NULL, NULL );

         /* if( !bolTest ) {
            //p_scrScreen->do_blit( p_surErrorText, NULL, &rctBlitError );
            p_scrScreen->do_text( 50, 150, agfx::SMALLFONT, "There was an error starting Maid Quest!", strTitleFont, agfx::RED );
         } */

         /* GFX_COLOR = GFX_RED;
         graphics_draw_text( "There was an error starting Maid Quest!", "Arial", 10,  ); */

         graphics_do_update();

         while( GFX_FPS > ps_fps->i_ticks_start );
      }

      /* Fade out the title screen.                                        */
      //p_scrScreen->do_fadeout( p_surTitleScreen, agfx::BLACK );

      switch( i_command ) {
         /* case TCMD_STORY:
            // TODO: Level loading loop!
            strField = "field";
            #ifdef USEMUSIC
            if( p_musMusic->get_playing() ) {
               p_musMusic->do_stopmusic();
            }
            #endif // USEMUSIC
            //bolTest = main_loop_game( &scrScreen, strField );
            while( "" != strField ) {
               p_mqMain = new MQCore( p_scrScreen, strField );
               strField = p_mqMain->do_game(); // Run the game loop until it quits.
               delete p_mqMain;
            }

            intTitleCommand = TCMD_NONE;
            break; */

         case COMMAND_QUIT:
            i_bol_running = FALSE;
            i_command = COMMAND_NONE;
            break;
      }

   }

   // Clean up!
   event_timer_free( ps_fps );
   graphics_do_free( ps_title_image );

   return;
}
