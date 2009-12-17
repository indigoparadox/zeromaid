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

#include "systype_platform.h"

DBG_ENABLE
GFX_DRAW_LOOP_ENABLE

/* = Global Variables = */

extern CACHE_CACHE* gps_cache;

/* = Functions = */

int systype_platform_loop( bstring ps_map_name_in ) {
   int i_jump_const = 0; /* The upward Y accelleration to apply. */
   GFX_DRAW_LOOP_INIT

   while( 1 ) {
      GFX_DRAW_LOOP_START

      /* Apply the jump velocity and then reduce it by jump decel. */
      i_jump_const -= SYSTYPE_PLATFORM_JUMP_ACCEL;

      GFX_DRAW_LOOP_END
   }


stpl_cleanup:

   return 0;
}

void systype_platform_jump() {

}
