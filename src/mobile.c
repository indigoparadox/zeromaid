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

#include "mobile.h"

/* = Global Variables= */

extern int gi_animation_frame;

/* = Functions = */

/* Purpose: Create a MOBILE struct from the mobile data at the given path.    */
/* Parameters: The path to the mobile's data file.                            */
/* Return: A new MOBILE struct.                                               */
MOBILE_MOBILE* mobile_create_mobile( bstring ps_path_in ) {

}

/* Purpose: Draw the given mobile to the screen if it's within the current    *
 *          viewport.                                                         */
/* Parameters: The mobile to draw, the current viewport.                      */
void mobile_draw( MOBILE_MOBILE* ps_mob_in, GFX_RECTANGLE* ps_viewport_in ) {
   int i_tile_start_x = 0, i_tile_start_y = 0, /* Tile coordinates within     */
      i_tile_width = 0, i_tile_height = 0;     /* which to draw.              */
   GFX_TILEDATA* ps_tile_data = NULL; /* The data for the iterated tile. */
   GFX_RECTANGLE s_tile_rect, s_screen_rect; /* Blit the tile from/to. */

   /* Setup the blit source and destination. */
   s_screen_rect.w = ps_mob_in->pixel_size;
   s_screen_rect.h = ps_mob_in->pixel_size;
   s_tile_rect.w = ps_mob_in->pixel_size;
   s_tile_rect.h = ps_mob_in->pixel_size;

   /* Only draw the mobile if it's visible. */
   if(
      ps_mob_in->tile_x < (i_tile_start_x + i_tile_width) &&
      ps_mob_in->tile_y < (i_tile_start_y + i_tile_height) &&
      ps_mob_in->tile_x >= i_tile_start_x &&
      ps_mob_in->tile_y >= i_tile_start_y
   ) {
      /* Figure out the offset of the tile onscreen. */
      /* TODO: Center the mobile. */
      s_screen_rect.x = (ps_mob_in->tile_x - i_tile_start_x) *
         ps_mob_in->pixel_size;
      s_screen_rect.y = (ps_mob_in->tile_y - i_tile_start_y) *
         ps_mob_in->pixel_size;

      /* Figure out where on the spritesheet the sprite is. */
      //s_tile_rect.x = gi_animation_frame * ps_mob_in->pixel_size;
      //s_tile_rect.y = (ps_tile_iter->gid - 1) * ps_mob_in->pixel_size;

      /* Draw the tile! */
      /* ps_tile_data = graphics_get_tiledata(
         ps_tile_iter->gid, ps_map_in->tileset
      ); */
      graphics_draw_blit_tile(
         ps_mob_in->spritesheet->image,
         &s_tile_rect,
         &s_screen_rect
      );
   }
}

/* Purpose: Draw the in-viewport mobiles from the given list of mobiles to    *
 *          the screen starting with the first linked mobile.                 */
/* Parameters: The list of mobiles to draw, the number of mobiles in the      *
 *             list, the current viewport.                                    */
void mobile_draw_list(
   MOBILE_MOBILE as_mob_in[],
   int i_mob_count_in,
   GFX_RECTANGLE* ps_viewport_in
) {
   MOBILE_MOBILE* ps_mobile_iter = NULL; /* The mobile drawing iterator. */
   int i; /* Loop iterator. */

   for( i = 0 ; i < i_mob_count_in ; i++ ) {
      mobile_draw( &as_mob_in[i], ps_viewport_in );
   }
}
