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

#include "systype_title_client.h"

DBG_ENABLE
GFX_DRAW_LOOP_ENABLE
TITLE_ERROR_ENABLE

/* = Functions = */

/* Purpose: Title screen loop.                                                */
/* Return: The code for the next action to take.                              */
int systype_title_client_loop( CACHE_CACHE* ps_cache_in ) {
   int i_menu_selected = 0, i = 0, i_act_return = RETURN_ACTION_QUIT;
   GEO_COLOR* ps_color_fade = geometry_create_color( 0, 0, 0 );
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
      #ifdef USEALLEGRO
      graphics_do_update();
      #endif /* USEALLEGRO */
      graphics_draw_transition(
         GFX_TRANS_FX_BLIP, ps_title_iter->i_trans, ps_color_fade
      );
   }
   while( 1 ) {
      GFX_DRAW_LOOP_START

      /* Load the next title screen or reduce the delay on this one. */
      if( NULL != ps_title_iter && 1 == ps_title_iter->delay && NULL != ps_title_iter->next ) {
         graphics_draw_transition( GFX_TRANS_FX_FADE, ps_title_iter->o_trans, ps_color_fade );
         ps_title_iter = ps_title_iter->next;
         graphics_draw_blit_tile( ps_title_iter->bg_image, NULL, NULL );
         graphics_draw_transition( GFX_TRANS_FX_FADE, ps_title_iter->i_trans, ps_color_fade );

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
               ps_title_iter
            );
         }
      }

      graphics_do_update();

      GFX_DRAW_LOOP_END
   }

slt_cleanup:

   graphics_draw_transition(
      GFX_TRANS_FX_BLIP, ps_title_iter->o_trans, ps_color_fade
   );

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

/* Purpose: Display the root menu.                                            */
/* Parameters: The currently selected index.                                  */
void systype_title_client_show_menu(
   int i_index_in,
   bstring as_menu_list_in[],
   SYSTYPE_TITLE_TITLESCREEN* ps_titlescreen_in
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
            ps_titlescreen_in->menu_font,
            ps_titlescreen_in->fg_highlight
         );
      } else {
         /* The item is not selected. */
         graphics_draw_text(
            i_x,
            i_y + (SYSTYPE_TITLE_MENU_Y_INC * i),
            as_menu_list_in[i],
            ps_titlescreen_in->menu_font,
            ps_titlescreen_in->fg_color
         );
      }
   }
}

/* Purpose: Free the memory held by the given title screen. */
void systype_title_client_free_titlescreen(
   SYSTYPE_TITLE_TITLESCREEN* ps_tilescreen_in
) {
   graphics_free_image(  ps_tilescreen_in->bg_image );
   graphics_free_font( ps_tilescreen_in->menu_font );
   free( ps_tilescreen_in->bg_color );
   free( ps_tilescreen_in->fg_color );
   free( ps_tilescreen_in->fg_highlight );
   free( ps_tilescreen_in );
}
