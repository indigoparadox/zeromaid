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

WINDOW_MENU* gps_menu = NULL;

/* = Functions = */

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
   WINDOW_MENU_COLORS* ps_colors_in
) {
   struct bstrList* ps_items_list = NULL,
      * ps_item_iter = NULL,
      * ps_keyval_iter = NULL;
   WINDOW_MENU* ps_menu_out = NULL; /* Temporary menu struct to copy to the stack. */
   int i; /* Loop iterator. */

   ps_menu_out = calloc( 1, sizeof( WINDOW_MENU ) );

   /* Copy the colors struct. */
   memcpy( &ps_menu_out->colors, ps_colors_in, sizeof( WINDOW_MENU_COLORS ) );

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
      UTIL_ARRAY_ADD(
         WINDOW_MENU_ITEM, ps_menu_out->options,
         ps_menu_out->options_count, wcm_cleanup, NULL
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

   return ps_menu_out;
}

/* Purpose: Draw the selected text window on-screen, or the top-most window   *
 *          if no index is given or the index is invalid.                     */
void window_draw_text( int i_index_in, CACHE_CACHE* ps_cache_in ) {
   static GFX_SURFACE* tps_text_window_bg = NULL;
   static GFX_COLOR ts_color_text;
   static GFX_RECTANGLE ts_rect_window;
   static BOOL tb_success = TRUE; /* Were we able to setup window drawing? */
   static bstring tps_font_name = NULL;
   int i_lines_printed = 0,
      i_buffer_result = BSTR_OK;
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
      /* Setup the font name. */
      tps_font_name = bformat(
         "%s%s.%s", PATH_SHARE, WINDOW_TEXT_FONT, FILE_EXTENSION_FONT
      );

      /* Setup the background. */
      ps_bg_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_WINDOW_WIDE );
      tps_text_window_bg = graphics_create_image( ps_bg_path );
      bdestroy( ps_bg_path );

      /* Setup the color. */
      memset( &ts_color_text, 0, sizeof( GFX_COLOR ) );

      /* Setup the window rectangle. */
      memset( &ts_rect_window, 0, sizeof( GFX_RECTANGLE ) );
      ts_rect_window.w = tps_text_window_bg->w;
      ts_rect_window.h = tps_text_window_bg->h;
      ts_rect_window.x = (GFX_GET_SCREEN_WIDTH - ts_rect_window.w) / 2;
      ts_rect_window.y = (GFX_GET_SCREEN_HEIGHT - ts_rect_window.h) - 10;

      /* If something's still empty then there's no helping it. */
      if( NULL == tps_text_window_bg || NULL == tps_font_name ) {
         DBG_ERR( "Unable to setup text window drawing facilities." );
         tb_success = FALSE;
         goto wdt_cleanup;
      }
   }

   DBG_INFO_INT( "index", i_index_in );
   DBG_INFO_INT( "count", ps_cache_in->text_log_count );
   DBG_INFO_PTR( "ptr", ps_cache_in->text_log );
   DBG_INFO_STR( "str0", ps_cache_in->text_log[0].text->data );

   /* Draw the actual window and text. */
   graphics_draw_blit_tile( tps_text_window_bg, NULL, &ts_rect_window );
   i_buffer_len = strlen( ps_cache_in->text_log[i_index_in].text->data );
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
               (i_lines_printed * WINDOW_TEXT_HEIGHT) +  20,
            ps_line_buffer,
            tps_font_name,
            WINDOW_TEXT_SIZE,
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
   static GFX_RECTANGLE ts_rect_window;
   static BOOL tb_success = TRUE; /* Were we able to setup window drawing? */
   static bstring tps_font_path = NULL;
   GFX_COLOR* ps_color = NULL;
   bstring ps_bg_path = NULL;
   int i; /* Loop iterators. */

   /* If we passed the success check above and the window background is       *
    * empty, assume everything still needs to be setup.                       */
   if( NULL == tps_menu_window_bg ) {
      /* Setup the font name. */
      tps_font_path = bformat(
         "%s%s.%s", PATH_SHARE, WINDOW_MENU_TEXT_FONT, FILE_EXTENSION_FONT
      );

      /* Setup the background. */
      ps_bg_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_WINDOW_RECT );
      tps_menu_window_bg = graphics_create_image( ps_bg_path );
      bdestroy( ps_bg_path );

      /* Setup the window rectangle. */
      memset( &ts_rect_window, 0, sizeof( GFX_RECTANGLE ) );
      ts_rect_window.w = tps_menu_window_bg->w;
      ts_rect_window.h = tps_menu_window_bg->h;
      ts_rect_window.x = (GFX_GET_SCREEN_WIDTH - ts_rect_window.w) / 2;
      ts_rect_window.y = (GFX_GET_SCREEN_HEIGHT - ts_rect_window.h) / 2;

      /* If something's still empty then there's no helping it. */
      if( NULL == tps_menu_window_bg || NULL == tps_font_path ) {
         DBG_ERR( "Unable to setup menu window drawing facilities." );
         tb_success = FALSE;
         goto wdm_cleanup;
      }
   }

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
         ts_rect_window.y + 30 + (WINDOW_MENU_TEXT_HEIGHT * i),
         ps_menu_in->options[i].desc,
         tps_font_path,
         WINDOW_MENU_TEXT_SIZE,
         ps_color
      );
   }

wdm_cleanup:

   return;
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

wfm_cleanup:

   return;
}
