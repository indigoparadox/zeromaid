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

WINDOW_TEXT* window_create_text(
   bstring ps_text_in,
   CACHE_CACHE* ps_cache_in
) {
   bstring ps_text_new = NULL;

   /* The string will be copied to the new array, but the string itself is    *
    * actually just the bstring header, which should be cleared after calling *
    * this function anyway. So we have to make a proper copy of the bstring   *
    * before adding it to the array.                                          */
   ps_text_new = bstrcpy( ps_text_in );

   UTIL_ARRAY_ADD(
      bstring, ps_cache_in->text_log, ps_cache_in->text_log_count, wct_cleanup,
      ps_text_new
   );

wct_cleanup:

   return;
}

void window_draw_text( WINDOW_TEXT* ps_menu_in ) {

}

void window_draw_menu( WINDOW_MENU* ps_menu_in ) {

}
