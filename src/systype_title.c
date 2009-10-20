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
   short int i_bol_running = 1;
   int i_menu_selected = 0;
   EVENT_TIMER* ps_fps = event_timer_create();
   GFX_COLOR* ps_color_fade = graphics_create_color( 0, 0, 0 );
   TITLE_TITLESCREEN* ps_title_screens = systype_title_load_titlescreens();
   TITLE_TITLESCREEN* ps_title_iter = ps_title_screens;
   /* GFX_RECTANGLE s_blit_dest;
   GFX_RECTANGLE s_blit_error; */

   /* Setup the loading title and image, then blit them onto one surface. */
   DBG_INFO( "Setting up the title image..." );

   /* p_surTitleText = new agfx::SurfaceCore( "Maid Quest", strTitleFont, 48, agfx::BLACK );
   p_surSubtText  = new agfx::SurfaceCore( "Quest of the maid", strTitleFont, 18, agfx::BLACK ); */
   // Setup the error message text, in case we need it.
   /* p_surErrorText = new agfx::SurfaceCore( "There was an error starting Maid Quest!", strTitleFont, 18, agfx::RED );
   rctBlitError.set_x( 50 );
   rctBlitError.set_y( 150 );
   rctBlitError.set_width( p_surErrorText->get_width() );
   rctBlitError.set_height( p_surErrorText->get_height() ); */

   /* Show the title screen until the user selects something. */
   DBG_INFO( "Running title screen loop..." );
   if( NULL != ps_title_iter ) {
      graphics_draw_blit_tile( ps_title_iter->bg_image, NULL, NULL );
      graphics_draw_transition( GFX_TRANS_FADE_IN, ps_color_fade );
   }
   while( i_bol_running ) {
      /* Run the title screen menu input wait loop. */
      event_timer_start( ps_fps );

      /* Load the next title screen or reduce the delay on this one. */
      if( NULL != ps_title_iter && 0 == ps_title_iter->delay && NULL != ps_title_iter->next ) {
         graphics_draw_transition( GFX_TRANS_FADE_OUT, ps_color_fade );
         ps_title_iter = ps_title_iter->next;
         graphics_draw_blit_tile( ps_title_iter->bg_image, NULL, NULL );
         graphics_draw_transition( GFX_TRANS_FADE_IN, ps_color_fade );
      } else if( NULL != ps_title_iter && 0 < ps_title_iter->delay && NULL != ps_title_iter->next ) {
         ps_title_iter->delay--;
      }

      /* Listen for events. */
      int i_event = event_do_poll();
      switch( i_event ) {
         case EVENT_ID_QUIT:
            /* Quitting is universal. */
            printf( "Quitting! %d", i_event );
            goto slt_cleanup;

         /* case agfx::EVT_KEYESCAPE:
            intTitleCommand = TCMD_QUIT;
            break;

         case agfx::EVT_KEYENTER:
            intTitleCommand = TCMD_STORY;
            break; */
      }

      /* Loop through and draw all on-screen items. */
      if( NULL != ps_title_iter ) {
         graphics_draw_blit_tile( ps_title_iter->bg_image, NULL, NULL );
      }
      if( ps_title_iter->show_menu ) {

      }

      /* if( !bolTest ) {
         //p_scrScreen->do_blit( p_surErrorText, NULL, &rctBlitError );
         p_scrScreen->do_text( 50, 150, agfx::SMALLFONT, "There was an error starting Maid Quest!", strTitleFont, agfx::RED );
      } */

      /* GFX_COLOR = GFX_RED;
      graphics_draw_text( "There was an error starting Maid Quest!", "Arial", 10,  ); */

      graphics_do_update();

      while( GFX_FPS > ps_fps->i_ticks_start );
   }

   /* switch( i_command ) {
       case TCMD_STORY:
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
         break;

      case COMMAND_QUIT:
         i_bol_running = FALSE;
         i_command = COMMAND_NONE;
         break;
   } */

slt_cleanup:

   graphics_draw_transition( GFX_TRANS_FADE_OUT, ps_color_fade );

   /* Clean up! */
   ps_title_iter = ps_title_screens;
   while( NULL != ps_title_screens ) {
      ps_title_iter = ps_title_screens;
      ps_title_screens = ps_title_screens->next;
      graphics_free_image( ps_title_iter->bg_image );
      free( ps_title_iter->bg_color );
      free( ps_title_iter );
   }
   event_timer_free( ps_fps );
   free( ps_color_fade );

   return;
}

/* Purpose: Load the title screen chain from the system.xml data file.        */
/* Return: The first screen in the chain.                                     */
TITLE_TITLESCREEN* systype_title_load_titlescreens( void ) {
   ezxml_t ps_xml_system = ezxml_parse_file( PATH_SHARE "/" PATH_FILE_SYSTEM ),
      ps_xml_title = NULL, ps_xml_titlescreen_iter = NULL;
   TITLE_TITLESCREEN* ps_titlescreen_head = NULL;
   TITLE_TITLESCREEN* ps_titlescreen_iter = NULL;
   bstring ps_string_attrib = NULL, ps_string_temp1 = NULL, ps_string_temp2 = NULL;

   /* Load and verify down to the level of the title data. */
   if( NULL == ps_xml_system ) {
      DBG_ERR_FILE( "Unable to parse system configuration", PATH_SHARE "/" PATH_FILE_SYSTEM );
      goto stlt_cleanup;
   }
   ps_xml_title = ezxml_child( ps_xml_system, "title" );
   if( NULL == ps_xml_title ) {
      DBG_ERR_FILE( "Unable to parse title data in system configuration", PATH_SHARE "/" PATH_FILE_SYSTEM );
      goto stlt_cleanup;
   }

   /* Loop through and load each title screen's data. */
   ps_xml_titlescreen_iter = ezxml_child( ps_xml_title, "titlescreen" );
   while( NULL != ps_xml_titlescreen_iter ) {
      /* Create a new title screen structure to fill. */
      if( NULL == ps_titlescreen_iter ) {
         ps_titlescreen_iter = malloc( sizeof( TITLE_TITLESCREEN ) );
         ps_titlescreen_head = ps_titlescreen_iter;
      } else {
         ps_titlescreen_iter->next = malloc( sizeof( TITLE_TITLESCREEN ) );
         ps_titlescreen_iter = ps_titlescreen_iter->next;
      }
      if( NULL == ps_titlescreen_iter ) {
         DBG_ERR( "Unable to allocate title screen!" );
         goto stlt_cleanup;
      }
      memset( ps_titlescreen_iter, 0, sizeof( TITLE_TITLESCREEN ) );

      /* Load attribute data into the new title screen. */
      ps_string_temp1 = cstr2bstr( PATH_SHARE PATH_SCRDATA "/" );
      ps_string_attrib = cstr2bstr( ezxml_attr( ps_xml_titlescreen_iter, "bgimage" ) );
      ps_string_temp2 = cstr2bstr( "." FILE_EXTENSION_IMAGE );
      bconcat( ps_string_temp1, ps_string_attrib );
      bconcat( ps_string_temp1, ps_string_temp2 );
      ps_titlescreen_iter->bg_image = graphics_create_image( ps_string_temp1 );
      bdestroy( ps_string_attrib );
      bdestroy( ps_string_temp1 );
      bdestroy( ps_string_temp2 );

      ps_titlescreen_iter->delay = atoi( ezxml_attr( ps_xml_titlescreen_iter, "delay" ) );

      /* Verify title screen integrity against missing attributes. */

      /* Go to the next one! */
      ps_xml_titlescreen_iter = ps_xml_titlescreen_iter->next;
   }

stlt_cleanup:

   /* Clean up. */
   ezxml_free( ps_xml_system );
   bdestroy( ps_string_attrib );
   bdestroy( ps_string_temp1 );
   bdestroy( ps_string_temp2 );

   return ps_titlescreen_head;
}
