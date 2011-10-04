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

#include "window.h"

DBG_ENABLE

/* = Global Variables = */

static GFX_FONT* tgps_default_text_font = NULL,
   * tgps_default_menu_font = NULL;
static WINDOW_MENU_COLORS* tgps_default_menu_colors = NULL;

/* = Functions = */

/* Purpose: Try to setup some things the window system requires to function   *
 *          properly, such as the default window fonts.                       */
BOOL window_init( void ) {
   bstring ps_default_font_path;
   BOOL b_success = TRUE;

   /* Attempt to setup the default menu colors. */
   tgps_default_menu_colors = calloc( 1, sizeof( WINDOW_MENU_COLORS ) );
   if( NULL != tgps_default_menu_colors ) {
      tgps_default_menu_colors->fg.r = WINDOW_MENU_DEFAULT_COLOR_FG_R;
      tgps_default_menu_colors->fg.g = WINDOW_MENU_DEFAULT_COLOR_FG_G;
      tgps_default_menu_colors->fg.b = WINDOW_MENU_DEFAULT_COLOR_FG_B;
      tgps_default_menu_colors->bg.r = WINDOW_MENU_DEFAULT_COLOR_BG_R;
      tgps_default_menu_colors->bg.g = WINDOW_MENU_DEFAULT_COLOR_BG_G;
      tgps_default_menu_colors->bg.b = WINDOW_MENU_DEFAULT_COLOR_BG_B;
      tgps_default_menu_colors->sfg.r = WINDOW_MENU_DEFAULT_COLOR_FG_S_R;
      tgps_default_menu_colors->sfg.g = WINDOW_MENU_DEFAULT_COLOR_FG_S_G;
      tgps_default_menu_colors->sfg.b = WINDOW_MENU_DEFAULT_COLOR_FG_S_B;
      tgps_default_menu_colors->sbg.r = WINDOW_MENU_DEFAULT_COLOR_BG_S_R;
      tgps_default_menu_colors->sbg.g = WINDOW_MENU_DEFAULT_COLOR_BG_S_G;
      tgps_default_menu_colors->sbg.b = WINDOW_MENU_DEFAULT_COLOR_BG_S_B;
   } else {
      DBG_ERR( "Unable to allocate window menu default colors." );
      b_success = FALSE;
   }

   /* Attempt to load the default text font. */
   ps_default_font_path = bfromcstr( WINDOW_TEXT_DEFAULT_FONT );
   window_set_text_font(
      ps_default_font_path,
      WINDOW_TEXT_DEFAULT_SIZE
   );
   if( NULL == tgps_default_text_font ) {
      DBG_ERR_STR(
         "Unable to load window default text font",
         ps_default_font_path->data
      );
      b_success = FALSE;
   }
   bdestroy( ps_default_font_path );

   /* Attempt to load the default menu font. */
   ps_default_font_path = bfromcstr( WINDOW_MENU_DEFAULT_FONT );
   window_set_menu_font(
      ps_default_font_path,
      WINDOW_MENU_DEFAULT_SIZE
   );
   if( NULL == tgps_default_menu_font ) {
      DBG_ERR_STR(
         "Unable to load window default menu font",
         ps_default_font_path->data
      );
      b_success = FALSE;
   }

wi_cleanup:

   bdestroy( ps_default_font_path );

   return b_success;
}

/* Purpose: Clean up the window system once the game won't need it anymore.   *
 *          It's probably a good idea to call this at the end of a game       *
 *          module and  then window_init() at the beginning of the next one,  *
 *          maybe.                                                            */
void window_cleanup( void ){
   if( NULL != tgps_default_text_font ) {
      graphics_free_font( tgps_default_text_font );
   }

   if( NULL != tgps_default_menu_font ) {
      graphics_free_font( tgps_default_menu_font );
   }

   if( NULL != tgps_default_menu_colors ) {
      free( tgps_default_menu_colors );
   }
}

/* Purpose: Create a text log entry with it at the end of the given list.     */
/* Parameters: The text of the entry to create, a pointer to the list to      *
 *             create it in, a pointer to the number of entries on the list.  */
/* Return: The new pointer for the window list.                               */
CACHE_LOG_ENTRY* window_create_text(
   bstring ps_text_in,
   CACHE_LOG_ENTRY* as_text_windows_in,
   int* pi_text_windows_count_in
) {
   CACHE_LOG_ENTRY s_entry_tmp;
   int z; /* Loop iterator. */

   memset( &s_entry_tmp, 0, sizeof( CACHE_LOG_ENTRY ) );

   s_entry_tmp.text = bstrcpy( ps_text_in );

   UTIL_STACK_ADD(
      CACHE_LOG_ENTRY, as_text_windows_in, *pi_text_windows_count_in,
      wct_cleanup, &s_entry_tmp
   );

   DBG_INFO_STR( "Text window created", ps_text_in->data );

wct_cleanup:

   return as_text_windows_in;
}

/* Purpose: Create the given menu in the given list of menus.                 */
/* Parameters: A pointer to the menu to add and the cache to add it to.       */
/* Return: The new address of the given menu list.                            */
WINDOW_MENU* window_create_menu(
   bstring ps_items_in,
   COND_SCOPE i_scope_in,
   void (*f_callback_in)( void ),
   WINDOW_MENU_COLORS* ps_colors_in
) {
   struct bstrList* ps_items_list = NULL,
      * ps_item_iter = NULL,
      * ps_keyval_iter = NULL;
   WINDOW_MENU* ps_menu_out = NULL; /* Temporary menu struct to copy to the stack. */
   int i; /* Loop iterator. */
   WINDOW_MENU_COLORS* ps_colors = NULL;
   BOOL b_default_colors; /* We're using a copy of the default colors. */

   /* If no colors were specified, use the default colors. */
   if( NULL == ps_colors_in ) {
      ps_colors = malloc( sizeof( WINDOW_MENU_COLORS ) );
      if( NULL == ps_colors ) {
         DBG_ERR(
            "Unable to allocate temporary buffer for default menu colors."
         );
         goto wcm_cleanup;
      }
      memcpy(
         ps_colors, tgps_default_menu_colors, sizeof( WINDOW_MENU_COLORS )
      );
      b_default_colors = TRUE;
   }

   ps_menu_out = calloc( 1, sizeof( WINDOW_MENU ) );

   /* Copy the colors struct. */
   if( NULL != ps_colors_in ) {
      memcpy( &ps_menu_out->colors, ps_colors_in, sizeof( WINDOW_MENU_COLORS ) );
   }

   /* Set the scope. */
   ps_menu_out->scope = i_scope_in;

   /* Split apart the items list into Label:Target pairs. */
   ps_items_list = bsplit( ps_items_in, ';' );
   for( i = 0 ; i < ps_items_list->qty ; i++ ) {
      ps_item_iter = bsplit( ps_items_list->entry[i], ':' );

      /* There's no point in proceeding if there's not a description    *
       * and target for this item.                                      */
      if( ps_item_iter->qty < 2 ) {
         continue;
      }

      ps_keyval_iter = bsplit( ps_item_iter->entry[1], ',' );

      /* If there's no value then the default should be "true".         */
      if( ps_keyval_iter->qty < 2 ) {
         ps_keyval_iter->qty++;
         ps_keyval_iter->entry[1] = bformat( "true" );
      }

      /* Add the menu item to the new menu. */
      UTIL_ARRAY_REALLOC(
         WINDOW_MENU_ITEM, ps_menu_out->options,
         ps_menu_out->options_count, wcm_cleanup
      );
      ps_menu_out->options[ps_menu_out->options_count - 1].desc =
         bstrcpy( ps_item_iter->entry[0] );
      ps_menu_out->options[ps_menu_out->options_count - 1].key =
         bstrcpy( ps_keyval_iter->entry[0] );
      ps_menu_out->options[ps_menu_out->options_count - 1].value =
         bstrcpy( ps_keyval_iter->entry[1] );

      /* Clean up. */
      bstrListDestroy( ps_item_iter );
   }
   bstrListDestroy( ps_items_list );

   DBG_INFO( "Menu created" );

wcm_cleanup:

   if( b_default_colors && NULL != ps_colors ) {
      free( ps_colors );
   }

   return ps_menu_out;
}

/* Purpose: Draw the selected text window on-screen, or the top-most window   *
 *          if no index is given or the index is invalid.                     */
void window_draw_text( int i_index_in, CACHE_CACHE* ps_cache_in ) {
   static GFX_SURFACE* tps_text_window_bg = NULL;
   static GEO_COLOR ts_color_text;
   static GEO_RECTANGLE ts_rect_window;
   static BOOL tb_success = TRUE; /* Were we able to setup window drawing? */
   int i_lines_printed = 0,
      i_buffer_result = BSTR_OK,
      i_font_height;
   size_t i_buffer_start = 0, /* Offset to print from current input buffer. */
      i_buffer_end = WINDOW_BUFFER_LENGTH,
      i_buffer_len = 0; /* The length of the input buffer. */
   bstring ps_bg_path = NULL,
      ps_line_buffer = bformat( "" ); /* The buffer for multi-line strings. */

   /* Verify that the requested window index is valid. */
   if( i_index_in >= ps_cache_in->text_log_count || !tb_success ) {
      return;
   }

   /* If we passed the success check above and the window background is       *
    * empty, assume everything still needs to be setup.                       */
   if( NULL == tps_text_window_bg ) {
      /* Setup the background. */
      ps_bg_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_WINDOW_WIDE );
      tps_text_window_bg = graphics_create_image( ps_bg_path );
      bdestroy( ps_bg_path );

      /* Make sure the background was able to load. */
      if( NULL == tps_text_window_bg ) {
         DBG_ERR( "Unable to setup text window background." );
         tb_success = FALSE;
         goto wdt_cleanup;
      }

      /* Setup the color. */
      memset( &ts_color_text, 0, sizeof( GEO_COLOR ) );

      /* Setup the window rectangle. */
      memset( &ts_rect_window, 0, sizeof( GEO_RECTANGLE ) );
      ts_rect_window.w = tps_text_window_bg->w;
      ts_rect_window.h = tps_text_window_bg->h;
      ts_rect_window.x = (GFX_GET_SCREEN_WIDTH - ts_rect_window.w) / 2;
      ts_rect_window.y = (GFX_GET_SCREEN_HEIGHT - ts_rect_window.h) - 10;
   }

   /* Figure out the font height. */
   i_font_height = graphics_get_font_height( tgps_default_text_font );

   /*DBG_INFO_INT( "index", i_index_in );
   DBG_INFO_INT( "count", ps_cache_in->text_log_count );
   DBG_INFO_PTR( "ptr", ps_cache_in->text_log );
   DBG_INFO_STR( "str0", ps_cache_in->text_log[0].text->data );*/

   /* Draw the actual window and text. */
   graphics_draw_blit_tile( tps_text_window_bg, NULL, &ts_rect_window );
   i_buffer_len = strlen( (const char*)ps_cache_in->text_log[i_index_in].text->data );
   while( i_buffer_start < i_buffer_len && BSTR_ERR != i_buffer_result ) {
      /* If the nominal buffer length would be beyond the end of the string,  *
       * then print to the end of the string. Otherwise, print to the nominal *
       * buffer length.                                                       */
      if( i_buffer_start + WINDOW_BUFFER_LENGTH > i_buffer_len ) {
         i_buffer_end = i_buffer_len - i_buffer_start;
      } else {
         i_buffer_end = WINDOW_BUFFER_LENGTH;

         /* Decrease the end until we find a whitespace character. */
         while(
            i_buffer_end > 1 &&
            ps_cache_in->text_log[i_index_in].text->
               data[i_buffer_start + i_buffer_end] != ' '
         ) {
            i_buffer_end--;
         }
      }

      i_buffer_result = bassignmidstr (
         ps_line_buffer,
         ps_cache_in->text_log[i_index_in].text,
         i_buffer_start,
         i_buffer_end
      );

      if( i_buffer_start < i_buffer_len && BSTR_ERR != i_buffer_result ) {
         graphics_draw_text(
            ts_rect_window.x + 20,
            ts_rect_window.y +
               (i_lines_printed * (i_font_height + 5 )) +  20,
            ps_line_buffer,
            tgps_default_text_font,
            // WINDOW_TEXT_SIZE,
            &ts_color_text
         );
         i_buffer_start += i_buffer_end + 1; /* Add 1 for trailing space. */
         i_lines_printed++;
      }
   }

wdt_cleanup:

   bdestroy( ps_line_buffer );

   return;
}

/* Purpose: Draw all on-screen menus with the newest layered foremost.        */
/* Parameters: The list of menus to draw and the number of menus in the list. */
void window_draw_menu( WINDOW_MENU* ps_menu_in ) {
   static GFX_SURFACE* tps_menu_window_bg = NULL;
   static GEO_RECTANGLE ts_rect_window;
   static BOOL tb_success = TRUE; /* Were we able to setup window drawing? */
   GEO_COLOR* ps_color = NULL;
   bstring ps_bg_path = NULL;
   int i, /* Loop iterator. */
      i_menuitem_height;

   /* If we passed the success check above and the window background is       *
    * empty, assume everything still needs to be setup.                       */
   if( NULL == tps_menu_window_bg ) {
      /* Setup the background. */
      ps_bg_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_WINDOW_RECT );
      tps_menu_window_bg = graphics_create_image( ps_bg_path );
      bdestroy( ps_bg_path );

      /* Make sure the window background was properly loaded. */
      if( NULL == tps_menu_window_bg ) {
         DBG_ERR( "Unable to setup menu window background." );
         tb_success = FALSE;
         goto wdm_cleanup;
      }

      /* Setup the window rectangle. */
      memset( &ts_rect_window, 0, sizeof( GEO_RECTANGLE ) );
      ts_rect_window.w = tps_menu_window_bg->w;
      ts_rect_window.h = tps_menu_window_bg->h;
      ts_rect_window.x = (GFX_GET_SCREEN_WIDTH - ts_rect_window.w) / 2;
      ts_rect_window.y = (GFX_GET_SCREEN_HEIGHT - ts_rect_window.h) / 2;
   }

   /* TODO: Process menu item's font if it has one. */
   i_menuitem_height = graphics_get_font_height( tgps_default_menu_font );

   graphics_draw_blit_tile( tps_menu_window_bg, NULL, &ts_rect_window );

   /* Draw the menu options. */
   for( i = 0 ; i < ps_menu_in->options_count ; i++ ) {
      if( i == ps_menu_in->selected ) {
         ps_color = &ps_menu_in->colors.sfg;
      } else {
         ps_color = &ps_menu_in->colors.fg;
      }

      graphics_draw_text(
         ts_rect_window.x + 30,
         ts_rect_window.y + 30 + ((i_menuitem_height + 5) * i),
         ps_menu_in->options[i].desc,
         tgps_default_menu_font,
         ps_color
      );
   }

wdm_cleanup:

   return;
}

/* Purpose: Set the font to use for drawing window text instead of the        *
 * default font.                                                              */
void window_set_text_font(
   bstring ps_font_name_in,
   int i_size_in
) {
   bstring ps_font_path = NULL;

   ps_font_path = bformat(
      PATH_SHARE "%s." FILE_EXTENSION_FONT,
      ps_font_name_in->data
   );
   tgps_default_text_font = graphics_create_font(
      // TODO: XML should control menu size.
      ps_font_path, i_size_in
   );
   bdestroy( ps_font_path );
}

/* Purpose: Set the font to use for drawing menu itemsinstead of the default  *
 * font.                                                                      */
void window_set_menu_font(
   bstring ps_font_name_in,
   int i_size_in
) {
   bstring ps_font_path = NULL;

   ps_font_path = bformat(
      PATH_SHARE "%s." FILE_EXTENSION_FONT,
      ps_font_name_in->data
   );
   tgps_default_menu_font = graphics_create_font(
      // TODO: XML should control menu size.
      ps_font_path, i_size_in
   );
   bdestroy( ps_font_path );
}

/* Purpose: Free the top-most menu from the given menu stack.                 */
void window_free_menu( WINDOW_MENU* ps_menu_in ) {
   int i; /* Loop iterator. */

   DBG_INFO( "Freeing menu" );

   for( i = 0 ; i < ps_menu_in->options_count ; i++ ) {
      bdestroy( ps_menu_in->options[i].desc );
      bdestroy( ps_menu_in->options[i].key );
      bdestroy( ps_menu_in->options[i].value );
   }

   /* The index has had 1 subtracted from it by the macro above. */
   DBG_INFO( "Menu freed" );

   return;
}
