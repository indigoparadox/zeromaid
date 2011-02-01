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

DBG_ENABLE
GFX_DRAW_LOOP_ENABLE
TITLE_ERROR_ENABLE

/* = Functions = */

/* Purpose: Title screen loop.                                                */
/* Return: The code for the next action to take.                              */
int systype_title_loop( CACHE_CACHE* ps_cache_in ) {
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
   EVENT_EVENT s_event;

   memset( &s_event, 0, sizeof( EVENT_EVENT ) );

   /* Show the title screen until the user selects something. */
   DBG_INFO( "Running title screen loop..." );
   if( NULL != ps_title_iter ) {
      graphics_draw_blit_tile( ps_title_iter->bg_image, NULL, NULL );
      graphics_draw_transition( ps_title_iter->i_trans, ps_color_fade );
   }
   while( 1 ) {
      GFX_DRAW_LOOP_START

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
      event_do_poll( &s_event, FALSE );
      if( s_event.state[EVENT_ID_CANCEL] || s_event.state[EVENT_ID_QUIT] ) {
         DBG_INFO( "Quitting..." );
         goto slt_cleanup;
      }
      if( s_event.state[EVENT_ID_DOWN] ) {
         if( (SYSTYPE_TITLE_MENU_LEN - 1) > i_menu_selected ) {
            i_menu_selected++;
         } else {
            i_menu_selected = 0;
         }
      }
      if( s_event.state[EVENT_ID_UP] ) {
         /* Previous menu item. */
         if( 0 < i_menu_selected ) {
            i_menu_selected--;
         } else {
            i_menu_selected = SYSTYPE_TITLE_MENU_LEN - 1;
         }
      }
      if( s_event.state[EVENT_ID_FIRE] ) {
         /* Generally move things along. */
         if( !ps_title_iter->show_menu ) {
            /* The menu is hidden, so move to the next screen. */
            ps_title_iter->delay = 1;
         } else {
            /* Select a menu item. */
            if( SYSTYPE_TITLE_MENU_INDEX_SPSTART == i_menu_selected ) {
               /* Menu: SP Start */
               /* Set the cache to a new single-player game according to data *
                * files and set the engine to load the new game.              */
               if( !systype_title_load_start( ps_cache_in ) ) {
                  i_act_return = RETURN_ACTION_TITLE;
                  TITLE_ERROR_SET( "Unable to setup game." );
                  goto slt_cleanup;
               }

               if( !systype_title_load_team( ps_cache_in ) ) {
                  i_act_return = RETURN_ACTION_TITLE;
                  TITLE_ERROR_SET( "Unable to create player team." );
                  goto slt_cleanup;
               }

               i_act_return = RETURN_ACTION_LOADCACHE;
               goto slt_cleanup;

            } else if( SYSTYPE_TITLE_MENU_INDEX_LOAD == i_menu_selected ) {
               /* Menu: Load */

            } else if( SYSTYPE_TITLE_MENU_INDEX_QUIT == i_menu_selected ) {
               /* Menu: Quit */
               goto slt_cleanup;
            }
         }
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
               ps_text_iter->font,
               ps_text_iter->fg_color
            );

            /* Go to the next one! */
            ps_text_iter = ps_text_iter->next;
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
      }

      graphics_do_update();

      GFX_DRAW_LOOP_END
   }

slt_cleanup:

   graphics_draw_transition( ps_title_iter->o_trans, ps_color_fade );

   /* Clean up! */
   ps_title_iter = ps_title_screens;
   while( NULL != ps_title_screens ) {
      ps_title_iter = ps_title_screens;
      ps_title_screens = ps_title_screens->next;
      systype_title_free_titlescreen( ps_title_iter );
   }
   GFX_DRAW_LOOP_FREE
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
   ezxml_t ps_xml_system,
      ps_xml_title = NULL,
      ps_xml_titlescreen_iter = NULL;
   SYSTYPE_TITLE_TITLESCREEN* ps_titlescreen_head = NULL,
      * ps_titlescreen_iter = NULL,
      * ps_titlescreen_iter_prev = NULL;
   bstring ps_string_attrib = NULL,
      ps_font_path_iter = NULL;
   const char* pc_ezxml_attr;

   /* Load and verify down to the level of the title data. */
   ps_xml_system = ezxml_parse_file( PATH_SHARE "/" PATH_FILE_SYSTEM );
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
      ps_titlescreen_iter_prev = ps_titlescreen_iter;
      if( NULL == ps_titlescreen_iter ) {
         ps_titlescreen_iter = (SYSTYPE_TITLE_TITLESCREEN*)malloc(
            sizeof( SYSTYPE_TITLE_TITLESCREEN ) );
         ps_titlescreen_head = ps_titlescreen_iter;
      } else {
         ps_titlescreen_iter->next = (SYSTYPE_TITLE_TITLESCREEN*)malloc(
            sizeof( SYSTYPE_TITLE_TITLESCREEN ) );
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
         PATH_SHARE "%s." FILE_EXTENSION_IMAGE,
         ezxml_attr( ps_xml_titlescreen_iter, "bgimage" )
      );
      ps_titlescreen_iter->bg_image = graphics_create_image( ps_string_attrib );
      bdestroy( ps_string_attrib );

      /* ATTRIB: DELAY */
      pc_ezxml_attr = ezxml_attr( ps_xml_titlescreen_iter, "delay" );
      if( NULL != pc_ezxml_attr ) {
         ps_titlescreen_iter->delay = atoi( pc_ezxml_attr );
         if( 0 != ps_titlescreen_iter->delay ) {
            DBG_INFO_STR(
               "Title screen: Set delay",
               ezxml_attr( ps_xml_titlescreen_iter, "delay" )
            );
         }
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
         ps_font_path_iter = bformat(
            PATH_SHARE "%s." FILE_EXTENSION_FONT,
            ezxml_attr( ps_xml_titlescreen_iter, "menufont" )
         );
         ps_titlescreen_iter->menu_font = graphics_create_font(
            // TODO: XML should control menu size.
            ps_font_path_iter, SYSTYPE_TITLE_MENU_SIZE_DEFAULT
         );
         bdestroy( ps_font_path_iter );
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
      systype_title_load_titlescreen_text(
         &ps_titlescreen_iter->text_node, ps_xml_titlescreen_iter
      );

      /* Verify title screen integrity against missing attributes. */
      if( NULL == ps_titlescreen_iter->bg_image ) {
         /* TODO */
         /* If we delete the screen here, it's not very helpful. */
         /* systype_title_free_titlescreen( ps_titlescreen_iter );
         ps_titlescreen_iter = ps_titlescreen_iter_prev;
         ps_titlescreen_iter->next = NULL; */
      }

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

BOOL systype_title_load_titlescreen_text(
   SYSTYPE_TITLE_TEXT** pps_text_head_in,
   ezxml_t ps_xml_titlescreem_in
) {
   BOOL b_success = TRUE;
   SYSTYPE_TITLE_TEXT* ps_text_iter = NULL;
   ezxml_t ps_xml_text_iter;
   const char* pc_ezxml_attr;
   int i_text_size_iter = 0;
   bstring ps_string_attrib = NULL,
      ps_font_path_iter = NULL,
      ps_token_find = NULL,
      ps_token_replace = NULL;

   ps_xml_text_iter = ezxml_child( ps_xml_titlescreem_in, "text" );
   while( NULL != ps_xml_text_iter ) {
      if( NULL == *pps_text_head_in ) {
         /* Create the first text node. */
         *pps_text_head_in =
            (SYSTYPE_TITLE_TEXT*)malloc( sizeof( SYSTYPE_TITLE_TEXT ) );
         ps_text_iter = *pps_text_head_in;
      } else {
         /* Append a new text node. */
         ps_text_iter->next =
            (SYSTYPE_TITLE_TEXT*)malloc( sizeof( SYSTYPE_TITLE_TEXT ) );
         ps_text_iter = ps_text_iter->next;
      }
      if( NULL == ps_text_iter ) {
         DBG_ERR( "Unable to allocate title screen text node!" );
         break;
      }
      memset( ps_text_iter, 0, sizeof( SYSTYPE_TITLE_TEXT ) );

      /* Load the text node attributes. */
      /* ATTRIB: LOCATION/SIZE */
      pc_ezxml_attr = ezxml_attr( ps_xml_text_iter, "x" );
      if( NULL != pc_ezxml_attr ) {
         ps_text_iter->x = atoi( pc_ezxml_attr );
      }
      pc_ezxml_attr = ezxml_attr( ps_xml_text_iter, "y" );
      if( NULL != pc_ezxml_attr ) {
         ps_text_iter->y = atoi( pc_ezxml_attr );
      }
      pc_ezxml_attr = ezxml_attr( ps_xml_text_iter, "size" );
      if( NULL != pc_ezxml_attr ) {
         i_text_size_iter = atoi( pc_ezxml_attr );
      } else {
         i_text_size_iter = SYSTYPE_TITLE_TEXT_SIZE_DEFAULT;
      }

      /* ATTRIB: FG COLOR */
      ps_string_attrib = bformat( "%s", ezxml_attr( ps_xml_text_iter, "fgcolor" ) );
      if( NULL != ps_string_attrib ) {
         ps_text_iter->fg_color = graphics_create_color_html( ps_string_attrib );
      }
      bdestroy( ps_string_attrib );

      /* ATTRIB: FONT */
      ps_font_path_iter = bformat(
         PATH_SHARE "%s." FILE_EXTENSION_FONT,
         ezxml_attr( ps_xml_text_iter, "font" )
      );
      ps_text_iter->font =
         graphics_create_font( ps_font_path_iter, i_text_size_iter );
      bdestroy( ps_font_path_iter );

      /* ATTRIB: TEXT */
      ps_text_iter->text = bformat( "%s", ps_xml_text_iter->txt );
      if( NULL == ps_text_iter->text ) {
         DBG_ERR( "Created empty text node!" );
         /* We're not really gonna do anything about this, but it's not    *
          * valid.                                                         */
      }

      /* Replace all system tokens in the output text. */
      if( NULL == gps_title_error ) {
         SYSTYPE_TITLE_REPLACE_TOKENS( "error", "" );
      } else {
         SYSTYPE_TITLE_REPLACE_TOKENS( "error", gps_title_error->data );
      }

      /* Report. */
      DBG_INFO_INT_INT(
         "Loaded text node (x, y)",
         ps_text_iter->x,
         ps_text_iter->y
      );

      /* Go to the next one! */
      ps_xml_text_iter = ps_xml_text_iter->next;
   }

   return b_success;
}

/* Purpose: Verify that the system file is valid and load its XML tree.       */
ezxml_t systype_title_load_system( void ) {
   bstring ps_system_path;

   ps_system_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_SYSTEM );

   /* Verify the XML file exists and open or abort accordingly. */
   if( !zm_file_exists( ps_system_path ) ) {
      DBG_ERR_STR( "Unable to find system file.", ps_system_path->data );
      return NULL;
   } else {
      return ezxml_parse_file( (const char*)ps_system_path->data );
   }
}

/* Purpose: Get the starting team and load it into the given cache object.    */
/* Return: A boolean indicating success (TRUE) or failure (FALSE).            */
BOOL systype_title_load_team( CACHE_CACHE* ps_cache_in ) {
   BOOL b_success = TRUE;
   bstring ps_mobile_src = NULL;
   ezxml_t ps_xml_system = NULL,
      ps_xml_team = NULL,
      ps_xml_story = NULL,
      ps_xml_member_iter = NULL;
   MOBILE_MOBILE* ps_member_temp;

   /* Load the system file. */
   ps_xml_system = systype_title_load_system();
   if( NULL == ps_xml_system ) {
      b_success = FALSE;
      goto stlt_cleanup;
   }

   /* Load the team XML node. */
   ps_xml_story = ezxml_child( ps_xml_system, "story" );
   if( NULL == ps_xml_story ) {
      DBG_ERR_STR( "Invalid system data format", "Missing <story> element." );
      b_success = FALSE;
      goto stlt_cleanup;
   }
   ps_xml_team = ezxml_child( ps_xml_story, "team" );
   if( NULL == ps_xml_team ) {
      DBG_ERR_STR(
         "Invalid system data format",
         "Missing <team> element."
      );
      b_success = FALSE;
      goto stlt_cleanup;
   }

   /* Cycle through team member nodes and create their structures. */
   DBG_INFO( "Loading team members..." );
   ps_xml_member_iter = ezxml_child( ps_xml_team, "member" );
   while( NULL != ps_xml_member_iter ) {
      /* Create a new mobile. */
      ps_mobile_src = bformat( "%s", ezxml_attr( ps_xml_member_iter, "src" ) );
      ps_member_temp = mobile_load_mobile( ps_mobile_src );
      if( NULL != ps_member_temp ) {
         /* Set the special properties making this a player mobile. */
         ps_member_temp->serial =
            atoi( ezxml_attr( ps_xml_member_iter, "serial" ) );
         ps_member_temp->pixel_x =
            ps_member_temp->pixel_size *
            atoi( ezxml_attr( ps_xml_team, "startx" ) );
         ps_member_temp->pixel_y =
            ps_member_temp->pixel_size *
            atoi( ezxml_attr( ps_xml_team, "starty" ) );

         /* Move this mobile to the team list. */
         UTIL_ARRAY_ADD(
            MOBILE_MOBILE, ps_cache_in->player_team,
            ps_cache_in->player_team_count, stlt_cleanup, ps_member_temp
         );
         ps_member_temp = NULL;
      } else {
      }

      /* Go to the next one! */
      ps_xml_member_iter = ezxml_next( ps_xml_member_iter );
   }

stlt_cleanup:

   ezxml_free( ps_xml_system );

   /* TODO: Write this function. */
   return b_success;
}

/* Purpose: Get the starting game and load it into the given cache object.    */
/* Return: A boolean indicating success (TRUE) or failure (FALSE).            */
BOOL systype_title_load_start( CACHE_CACHE* ps_cache_in ) {
   BOOL b_success = TRUE;
   ezxml_t ps_xml_system = NULL,
      ps_xml_story = NULL,
      ps_xml_smap = NULL;
   bstring ps_system_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_SYSTEM );
   CACHE_CACHE s_cache_temp;

   /* ps_cache_in->game_type = SYSTEM_TYPE_ADVENTURE;
   bdestroy( ps_cache_in->map_name );
   ps_cache_in->map_name = bformat( "field" ); */

   /* Reminder to my future self: If the cache remained a global variable,    *
    * might never have found this. Stupid. ~_~                                */
   memset( &s_cache_temp, 0, sizeof( CACHE_CACHE ) );

   /* Verify the XML file exists and open or abort accordingly. */
   ps_xml_system = systype_title_load_system();
   if( NULL == ps_xml_system ) {
      b_success = FALSE;
      goto stls_cleanup;
   }

   /* Load the single-player story data. */
   ps_xml_story = ezxml_child( ps_xml_system, "story" );
   if( NULL == ps_xml_story ) {
      DBG_ERR_STR( "Invalid system data format", "Missing <story> element." );
      b_success = FALSE;
      goto stls_cleanup;
   }
   ps_xml_smap = ezxml_child( ps_xml_story, "startmap" );
   if( NULL == ps_xml_smap ) {
      DBG_ERR_STR(
         "Invalid system data format",
         "Missing <startmap> element."
      );
      b_success = FALSE;
      goto stls_cleanup;
   }

   /* Load the starting map. */
   if( 0 == strcmp( ezxml_attr( ps_xml_smap, "type" ), "visnov" ) ) {
      s_cache_temp.game_type = SYSTEM_TYPE_VISNOV;
      DBG_INFO_STR( "Game type selected", ezxml_attr( ps_xml_smap, "type" ) );
   } else if( 0 == strcmp( ezxml_attr( ps_xml_smap, "type" ), "adventure" ) ) {
      s_cache_temp.game_type = SYSTEM_TYPE_ADVENTURE;
      DBG_INFO_STR( "Game type selected", ezxml_attr( ps_xml_smap, "type" ) );
   } else if( 0 == strcmp( ezxml_attr( ps_xml_smap, "type" ), "platform" ) ) {
      s_cache_temp.game_type = SYSTEM_TYPE_PLATFORM;
      DBG_INFO_STR( "Game type selected", ezxml_attr( ps_xml_smap, "type" ) );
   } else {
      DBG_ERR( "Invalid game type." );
      b_success = FALSE;
      goto stls_cleanup;
   }

   /* Load the starting field name. */
   s_cache_temp.map_name = bformat( "%s", ezxml_attr( ps_xml_smap, "name" ) );

   /* If everything checks out then copy the temp cache onto the given cache. */
   // TODO: Validation per game type.
   memcpy( ps_cache_in, &s_cache_temp, sizeof( CACHE_CACHE ) );

stls_cleanup:

   bdestroy( ps_system_path );
   ezxml_free( ps_xml_system );

   return b_success;
}

/* Purpose: Display the root menu.                                            */
/* Parameters: The currently selected index.                                  */
void systype_title_show_menu(
   int i_index_in,
   bstring as_menu_list_in[],
   GFX_FONT* ps_font_in,
   GFX_COLOR* ps_color_in,
   GFX_COLOR* ps_highlight_in
) {
   static int i = 0,
      i_x = SYSTYPE_TITLE_MENU_X_START,
      i_y = SYSTYPE_TITLE_MENU_Y_START;

   for( i = 0 ; SYSTYPE_TITLE_MENU_LEN > i ; i++ ) {
      if( i == i_index_in ) {
         /* The item is selected. */
         graphics_draw_text(
            i_x,
            i_y + (SYSTYPE_TITLE_MENU_Y_INC * i),
            as_menu_list_in[i],
            ps_font_in,
            ps_highlight_in
         );
      } else {
         /* The item is not selected. */
         graphics_draw_text(
            i_x,
            i_y + (SYSTYPE_TITLE_MENU_Y_INC * i),
            as_menu_list_in[i],
            ps_font_in,
            ps_color_in
         );
      }
   }
}

/* Purpose: Free the memory held by the given title screen. */
void systype_title_free_titlescreen(
   SYSTYPE_TITLE_TITLESCREEN* ps_tilescreen_in
) {
   graphics_free_image(  ps_tilescreen_in->bg_image );
   graphics_free_font( ps_tilescreen_in->menu_font );
   free( ps_tilescreen_in->bg_color );
   free( ps_tilescreen_in->fg_color );
   free( ps_tilescreen_in->fg_highlight );
   free( ps_tilescreen_in );
}

