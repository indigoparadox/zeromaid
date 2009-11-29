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

/* = Global Variables = */

extern FILE* gps_debug;
extern CACHE_CACHE* gps_cache;

/* = Functions = */

/* Purpose: Title screen loop.                                                */
/* Return: The code for the next action to take.                              */
int systype_title_loop( void ) {
   int i_menu_selected = 0, i = 0, i_act_return = RETURN_ACTION_QUIT;
   GFX_COLOR* ps_color_fade = graphics_create_color( 0, 0, 0 );
   SYSTYPE_TITLE_TITLESCREEN* ps_title_screens = systype_title_load_titlescreens();
   SYSTYPE_TITLE_TITLESCREEN* ps_title_iter = ps_title_screens;
   SYSTYPE_TITLE_TEXT* ps_text_iter = NULL;
   bstring ps_font_name = NULL,
      as_menu_text[] = {
         bformat( SYSTYPE_TITLE_MENU_LABEL_SPSTART ),
         bformat( SYSTYPE_TITLE_MENU_LABEL_LOAD ),
         bformat( SYSTYPE_TITLE_MENU_LABEL_QUIT )
      };
   #ifdef USESDL
   #else
   EVENT_TIMER* ps_fps = event_timer_create();
   #endif /* USESDL */

   /* Show the title screen until the user selects something. */
   DBG_INFO( "Running title screen loop..." );
   if( NULL != ps_title_iter ) {
      graphics_draw_blit_tile( ps_title_iter->bg_image, NULL, NULL );
      graphics_draw_transition( ps_title_iter->i_trans, ps_color_fade );
   }
   while( 1 ) {
      #ifdef USESDL
      /* SDL uses its own sleep function. */
      #else
      /* Run the title screen menu input wait loop. */
      event_timer_start( ps_fps );
      #endif /* USESDL */

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
      int i_event = event_do_poll_once();
      switch( i_event ) {
         case EVENT_ID_ESC:
         case EVENT_ID_QUIT:
            /* Quitting is universal. */
            DBG_INFO( "Quitting..." );
            goto slt_cleanup;

         case EVENT_ID_DOWN:
            /* Next menu item. */
            if( (SYSTYPE_TITLE_MENU_LEN - 1) > i_menu_selected ) {
               i_menu_selected++;
            } else {
               i_menu_selected = 0;
            }
            break;

         case EVENT_ID_UP:
            /* Previous menu item. */
            if( 0 < i_menu_selected ) {
               i_menu_selected--;
            } else {
               i_menu_selected = SYSTYPE_TITLE_MENU_LEN - 1;
            }
            break;

         case EVENT_ID_FIRE:
            /* Generally move things along. */
            if( !ps_title_iter->show_menu ) {
               /* The menu is hidden, so move to the next screen. */
               ps_title_iter->delay = 1;
            } else {
               /* Select a menu item. */
               if( SYSTYPE_TITLE_MENU_INDEX_SPSTART == i_menu_selected ) {
                  /* Menu: SP Start */

                  /* TODO: Set the cache to a new single-player game          *
                   * according to story data files.                           */
                  gps_cache->game_type = SYSTEM_TYPE_ADVENTURE;
                  i_act_return = RETURN_ACTION_LOADCACHE;

                  goto slt_cleanup;

               } else if( SYSTYPE_TITLE_MENU_INDEX_LOAD == i_menu_selected ) {
                  /* Menu: Load */

               } else if( SYSTYPE_TITLE_MENU_INDEX_QUIT == i_menu_selected ) {
                  /* Menu: Quit */
                  goto slt_cleanup;
               }
            }

            break;
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
         systype_title_show_menu(
            i_menu_selected,
            as_menu_text,
            ps_title_iter->menu_font,
            ps_title_iter->fg_color,
            ps_title_iter->fg_highlight
         );
      }

      graphics_do_update();

      /* If possible, try to delay without busy-spinning. */
      #ifdef USESDL
      SDL_Delay( 1000 / GFX_FPS );
      #else
      while( GFX_FPS > ps_fps->i_ticks_start );
      #endif /* USESDL */
   }

slt_cleanup:

   graphics_draw_transition( ps_title_iter->o_trans, ps_color_fade );

   /* Clean up! */
   ps_title_iter = ps_title_screens;
   while( NULL != ps_title_screens ) {
      ps_title_iter = ps_title_screens;
      ps_title_screens = ps_title_screens->next;
      graphics_free_image( ps_title_iter->bg_image );
      bdestroy( ps_title_iter->menu_font );
      free( ps_title_iter->bg_color );
      free( ps_title_iter->fg_color );
      free( ps_title_iter->fg_highlight );
      free( ps_title_iter );
   }
   #ifdef USESDL
   #else
   event_timer_free( ps_fps );
   #endif /* USESDL */
   free( ps_color_fade );
   bdestroy( ps_font_name );
   for( i = 0 ; SYSTYPE_TITLE_MENU_LEN > i ; i++ ) {
      bdestroy( as_menu_text[i] );
   }

   return i_act_return;
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
      DBG_ERR_STR( "Unable to parse system configuration", PATH_SHARE "/" PATH_FILE_SYSTEM );
      goto stlt_cleanup;
   }
   ps_xml_title = ezxml_child( ps_xml_system, "title" );
   if( NULL == ps_xml_title ) {
      DBG_ERR_STR( "Unable to parse title data in system configuration", PATH_SHARE "/" PATH_FILE_SYSTEM );
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
         DBG_INFO_STR(
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
         DBG_INFO_STR(
            "Title screen: Set menu font",
            ps_titlescreen_iter->menu_font->data
         );
      }

      /* ATTRIB: FG COLOR */
      ps_string_attrib = bformat( "%s", ezxml_attr( ps_xml_titlescreen_iter, "fgcolor" ) );
      if( NULL != ps_string_attrib ) {
         ps_titlescreen_iter->fg_color = graphics_create_color_html( ps_string_attrib );
      }
      bdestroy( ps_string_attrib );

      /* ATTRIB: FG HIGHLIGHT COLOR */
      ps_string_attrib = bformat( "%s", ezxml_attr( ps_xml_titlescreen_iter, "fghighlight" ) );
      if( NULL != ps_string_attrib ) {
         ps_titlescreen_iter->fg_highlight = graphics_create_color_html( ps_string_attrib );
      }
      bdestroy( ps_string_attrib );

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
void systype_title_show_menu(
   int i_index_in,
   bstring as_menu_list_in[],
   bstring ps_font_name_in,
   GFX_COLOR* ps_color_in,
   GFX_COLOR* ps_highlight_in
) {
   static int i = 0, i_x = 100, i_y = 300, i_size = 18;

   for( i = 0 ; SYSTYPE_TITLE_MENU_LEN > i ; i++ ) {
      if( i == i_index_in ) {
         /* The item is selected. */
         graphics_draw_text(
            i_x,
            i_y + (30 * i),
            as_menu_list_in[i],
            ps_font_name_in,
            i_size,
            ps_highlight_in
         );
      } else {
         /* The item is not selected. */
         graphics_draw_text(
            i_x,
            i_y + (30 * i),
            as_menu_list_in[i],
            ps_font_name_in,
            i_size,
            ps_color_in
         );
      }
   }
}

