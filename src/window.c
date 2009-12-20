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

/* = Functions = */

void window_create_text(
   bstring ps_text_in,
   CACHE_CACHE* ps_cache_in
) {
   UTIL_ARRAY_ADD(
      bstring, ps_cache_in->text_log, ps_cache_in->text_log_count, wct_cleanup,
      NULL
   );
   ps_cache_in->text_log[ps_cache_in->text_log_count - 1] =
      bstrcpy( ps_text_in );

   DBG_INFO_STR( "Text window created", ps_text_in->data );

wct_cleanup:

   return;
}

/* Purpose: Draw the selected text window on-screen, or the top-most window   *
 *          if no index is given or the index is invalid.                     */
void window_draw_text( int i_index_in, CACHE_CACHE* ps_cache_in ) {
   static GFX_SURFACE* tps_text_window_bg = NULL;
   static GFX_COLOR ts_color_text;
   static GFX_RECTANGLE ts_rect_window;
   static BOOL tb_success = TRUE;
   static bstring tps_font_name = NULL;
   int i_window_index = (ps_cache_in->text_log_count - 1) - i_index_in,
      i_buffer_result = BSTR_OK;
   size_t i_buffer_start = 0, /* Offset to print from current input buffer. */
      i_buffer_end = WINDOW_BUFFER_LENGTH,
      i_buffer_len; /* The length of the input buffer. */
   bstring ps_bg_path = NULL,
      ps_line_buffer = bfromcstr( "" ); /* The buffer for multi-line strings. */

   /* Verify that the requested window index is valid. */
   if( 0 > i_window_index || !tb_success ) {
      return;
   }

   /* If we passed the success check above and the window background is       *
    * empty, assume everything still needs to be setup.                       */
   if( NULL == tps_text_window_bg ) {
      /* Setup the font name. */
      tps_font_name =
         bformat( "%s%s.%s", PATH_SHARE, FONT_WINDOW_TEXT, FILE_EXTENSION_FONT );

      /* Setup the background. */
      ps_bg_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_WINDOW_WIDE );
      tps_text_window_bg = graphics_create_image( ps_bg_path );
      bdestroy( ps_bg_path );

      /* Setup the color. */
      memset( &ts_color_text, 0, sizeof( GFX_COLOR ) );

      /* Setup the window rectangle. */
      ts_rect_window.w = tps_text_window_bg->w;
      ts_rect_window.h = tps_text_window_bg->h;
      ts_rect_window.x = (GFX_GET_SCREEN_WIDTH - ts_rect_window.w) / 2;
      ts_rect_window.y = (GFX_GET_SCREEN_HEIGHT - ts_rect_window.h) - 10;

      /* If something's still empty then there's no helping it. */
      if( NULL == tps_text_window_bg || NULL == tps_font_name ) {
         DBG_ERR( "Unable to setup window drawing facilities." );
         tb_success = FALSE;
         goto wdt_cleanup;
      }
   }

   /* Draw the actual window and text. */
   graphics_draw_blit_tile( tps_text_window_bg, NULL, &ts_rect_window );
   i_buffer_len = strlen( ps_cache_in->text_log[i_window_index]->data );
   while( i_buffer_start < i_buffer_len && BSTR_ERR != i_buffer_result ) {
      if( i_buffer_start + WINDOW_BUFFER_LENGTH > i_buffer_len ) {
         i_buffer_end = i_buffer_len - i_buffer_start;
      } else {
         i_buffer_end = WINDOW_BUFFER_LENGTH;
      }

      i_buffer_result = bassignmidstr (
         ps_line_buffer,
         ps_cache_in->text_log[i_window_index],
         i_buffer_start,
         i_buffer_end
      );

      if( i_buffer_start < i_buffer_len && BSTR_ERR != i_buffer_result ) {
         graphics_draw_text(
            ts_rect_window.x + 20,
            ts_rect_window.y +
               ((i_buffer_start / WINDOW_BUFFER_LENGTH) * 20) +  20,
            ps_line_buffer,
            tps_font_name,
            12,
            &ts_color_text
         );
         i_buffer_start += WINDOW_BUFFER_LENGTH;
      }
   }

wdt_cleanup:

   bdestroy( ps_line_buffer );

   return;
}

void window_draw_menu( CACHE_CACHE* ps_cache_in ) {

}
