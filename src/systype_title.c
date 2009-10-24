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

void systype_title_loop( void ) {
   short int i_bol_running = 1;
   int i_menu_selected = 0;
   EVENT_TIMER* ps_fps = event_timer_create();
   GFX_COLOR* ps_color_fade = graphics_create_color( 0, 0, 0 );
   SYSTYPE_TITLE_TITLESCREEN* ps_title_screens = systype_title_load_titlescreens();
   SYSTYPE_TITLE_TITLESCREEN* ps_title_iter = ps_title_screens;
   SYSTYPE_TITLE_TEXT* ps_text_iter = NULL;
   bstring ps_font_name = NULL;

   /* Show the title screen until the user selects something. */
   DBG_INFO( "Running title screen loop..." );
   if( NULL != ps_title_iter ) {
      graphics_draw_blit_tile( ps_title_iter->bg_image, NULL, NULL );
      graphics_draw_transition( ps_title_iter->i_trans, ps_color_fade );
   }
   while( i_bol_running ) {
      /* Run the title screen menu input wait loop. */
      event_timer_start( ps_fps );

      /* Load the next title screen or reduce the delay on this one. */
      if( NULL != ps_title_iter && 1 == ps_title_iter->delay && NULL != ps_title_iter->next ) {
         graphics_draw_transition( ps_title_iter->o_trans, ps_color_fade );
         ps_title_iter = ps_title_iter->next;
         graphics_draw_blit_tile( ps_title_iter->bg_image, NULL, NULL );
         graphics_draw_transition( ps_title_iter->i_trans, ps_color_fade );

         /* Every new title resets the menu regardless of anything. */
         i_menu_selected = 0;
      } else if( NULL != ps_title_iter && 0 < ps_title_iter->delay && NULL != ps_title_iter->next ) {
         ps_title_iter->delay--;
      }

      /* Listen for events. */
      int i_event = event_do_poll();
      switch( i_event ) {
         case EVENT_ID_QUIT:
            /* Quitting is universal. */
            DBG_INFO( "Quitting..." );
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

         /* Draw the text items. */
         ps_text_iter = ps_title_iter->text_node;
         while( NULL != ps_text_iter ) {
            graphics_draw_text(
               ps_text_iter->x,
               ps_text_iter->y,
               ps_text_iter->text,
               ps_text_iter->font_name,
               ps_text_iter->size,
               ps_text_iter->fg_color
            );

            /* Go to the next one! */
            ps_text_iter = ps_text_iter->next;
         }
      }
      if( ps_title_iter->show_menu ) {
         systype_title_show_menu( i_menu_selected );
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

   graphics_draw_transition( ps_title_iter->o_trans, ps_color_fade );

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
   bdestroy( ps_font_name );

   return;
}

/* Purpose: Load the title screen chain from the system.xml data file.        */
/* Return: The first screen in the chain.                                     */
SYSTYPE_TITLE_TITLESCREEN* systype_title_load_titlescreens( void ) {
   ezxml_t ps_xml_system = ezxml_parse_file( PATH_SHARE "/" PATH_FILE_SYSTEM ),
      ps_xml_title = NULL, ps_xml_titlescreen_iter = NULL, ps_xml_text_iter = NULL;
   SYSTYPE_TITLE_TITLESCREEN* ps_titlescreen_head = NULL;
   SYSTYPE_TITLE_TITLESCREEN* ps_titlescreen_iter = NULL;
   SYSTYPE_TITLE_TEXT* ps_text_iter = NULL;
   bstring ps_string_attrib = NULL;

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
   DBG_INFO( "Loading title screens..." );
   ps_xml_titlescreen_iter = ezxml_child( ps_xml_title, "titlescreen" );
   while( NULL != ps_xml_titlescreen_iter ) {
      /* Create a new title screen structure to fill. */
      if( NULL == ps_titlescreen_iter ) {
         ps_titlescreen_iter = malloc( sizeof( SYSTYPE_TITLE_TITLESCREEN ) );
         ps_titlescreen_head = ps_titlescreen_iter;
      } else {
         ps_titlescreen_iter->next = malloc( sizeof( SYSTYPE_TITLE_TITLESCREEN ) );
         ps_titlescreen_iter = ps_titlescreen_iter->next;
      }
      if( NULL == ps_titlescreen_iter ) {
         DBG_ERR( "Unable to allocate title screen!" );
         goto stlt_cleanup;
      }
      memset( ps_titlescreen_iter, 0, sizeof( SYSTYPE_TITLE_TITLESCREEN ) );

      DBG_INFO( "Started loading title screen..." );

      /* Load attribute data into the new title screen. */
      /* ATTRIB: BGIMAGE */
      ps_string_attrib = bformat(
         PATH_SHARE PATH_SCRDATA "/%s." FILE_EXTENSION_IMAGE,
         ezxml_attr( ps_xml_titlescreen_iter, "bgimage" )
      );
      ps_titlescreen_iter->bg_image = graphics_create_image( ps_string_attrib );
      bdestroy( ps_string_attrib );

      /* ATTRIB: DELAY */
      ps_titlescreen_iter->delay = atoi( ezxml_attr( ps_xml_titlescreen_iter, "delay" ) );
      if( 0 != ps_titlescreen_iter->delay ) {
         DBG_INFO_FILE(
            "Title screen: Set delay",
            ezxml_attr( ps_xml_titlescreen_iter, "delay" )
         );
      }

      /* ATTRIB: TRANSITION */
      /* TODO: When there are other transitions available, the default should *
       * be to fade.                                                          */
      /* if( 0 == strcmp( ezxml_attr( ps_xml_titlescreen_iter, "transition" ), "fade" ) { */
      ps_titlescreen_iter->i_trans = GFX_TRANS_FADE_IN;
      ps_titlescreen_iter->o_trans = GFX_TRANS_FADE_OUT;
      DBG_INFO( "Title screen: Set transitions." );

      /* ATTRIB: SHOWMENU */
      if( 0 == strcmp( ezxml_attr( ps_xml_titlescreen_iter, "showmenu" ), "true" ) ) {
         ps_titlescreen_iter->show_menu = TRUE;
         DBG_INFO( "Title screen: Showing menu." );
      }

      /* ATTRIB: MENUFONT */
      if( NULL != ezxml_attr( ps_xml_titlescreen_iter, "menufont" ) ) {
         ps_titlescreen_iter->menu_font = bformat(
            PATH_SHARE PATH_SCRDATA "/%s." FILE_EXTENSION_FONT,
            ezxml_attr( ps_xml_titlescreen_iter, "menufont" )
         );
         DBG_INFO_FILE(
            "Title screen: Set menu font",
            ps_titlescreen_iter->menu_font->data
         );
      }

      /* Load the text elements for each title screen. */
      ps_xml_text_iter = ezxml_child( ps_xml_titlescreen_iter, "text" );
      while( NULL != ps_xml_text_iter ) {
         if( NULL == ps_titlescreen_iter->text_node ) {
            /* Create the first text node. */
            ps_titlescreen_iter->text_node = malloc( sizeof( SYSTYPE_TITLE_TEXT ) );
            ps_text_iter = ps_titlescreen_iter->text_node;
         } else {
            /* Append a new text node. */
            ps_text_iter->next = malloc( sizeof( SYSTYPE_TITLE_TEXT ) );
            ps_text_iter = ps_text_iter->next;
         }
         if( NULL == ps_text_iter ) {
            DBG_ERR( "Unable to allocate title screen text node!" );
            break;
         }
         memset( ps_text_iter, 0, sizeof( SYSTYPE_TITLE_TEXT ) );

         /* Load the text node attributes. */
         /* ATTRIB: LOCATION /SIZE */
         ps_text_iter->x = atoi( ezxml_attr( ps_xml_text_iter, "x" ) );
         ps_text_iter->y = atoi( ezxml_attr( ps_xml_text_iter, "y" ) );
         ps_text_iter->size = atoi( ezxml_attr( ps_xml_text_iter, "size" ) );

         /* ATTRIB: FG COLOR */
         ps_string_attrib = bformat( "%s", ezxml_attr( ps_xml_text_iter, "fgcolor" ) );
         if( NULL != ps_string_attrib ) {
            ps_text_iter->fg_color = graphics_create_color_html( ps_string_attrib );
         }
         bdestroy( ps_string_attrib );

         /* ATTRIB: FONT NAME */
         ps_text_iter->font_name = bformat(
            PATH_SHARE PATH_SCRDATA "/%s." FILE_EXTENSION_FONT,
            ezxml_attr( ps_xml_text_iter, "font" )
         );

         /* ATTRIB: TEXT */
         ps_text_iter->text = bformat( "%s", ps_xml_text_iter->txt );
         if( NULL == ps_text_iter->text ) {
            DBG_ERR( "Created empty text node!" );
            /* We're not really gonna do anything about this, but it's not    *
             * valid.                                                         */
         }

         /* Report. */
         ps_string_attrib = bformat(
            "Title screen: Successfully loaded text node at %d, %d.",
            ps_text_iter->x,
            ps_text_iter->y
         );
         DBG_INFO( ps_string_attrib->data );
         bdestroy( ps_string_attrib );

         /* Go to the next one! */
         ps_xml_text_iter = ps_xml_text_iter->next;
      }

      /* Verify title screen integrity against missing attributes. */


      DBG_INFO( "Finished loading title screen." );

      /* Go to the next one! */
      ps_xml_titlescreen_iter = ps_xml_titlescreen_iter->next;
   }

stlt_cleanup:

   /* Clean up. */
   ezxml_free( ps_xml_system );
   /* Freeing this string here causes weird things to happen. It's freed      *
    * every time it's used above already, so whatever.                        */
   /* bdestroy( ps_string_attrib ); */

   return ps_titlescreen_head;
}

/* Purpose: Display the root menu.                                         */
/* Parameters: The currently selected index.                               */
void systype_title_show_menu( int i_index_in ) {

}
