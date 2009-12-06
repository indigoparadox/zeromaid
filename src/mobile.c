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

DBG_ENABLE

/* = Global Variables= */

extern int gi_animation_frame;

/* = Functions = */

/* Purpose: Create a MOBILE struct from the mobile data at the given path.    */
/* Parameters: The path to the mobile's data file.                            */
/* Return: A new MOBILE struct.                                               */
MOBILE_MOBILE* mobile_create_mobile( bstring ps_path_in ) {
   MOBILE_MOBILE* ps_mob_out = calloc( 1, sizeof( MOBILE_MOBILE ) );
   ezxml_t ps_xml_mob = NULL, ps_xml_props = NULL, ps_xml_prop_iter = NULL;

   /* Verify memory allocation. */
   if( NULL == ps_mob_out ) {
      DBG_ERR( "Unable to allocate map." );
   }

   /* Verify the XML file exists and open or abort accordingly. */
   if( !file_exists( ps_path_in ) ) {
      DBG_ERR_STR( "Unable to load mobile", ps_path_in->data );
      return NULL;
   }
   ps_xml_mob = ezxml_parse_file( ps_path_in->data );

   /* Load the properties tree. */
   ps_xml_props = ezxml_child( ps_xml_mob, "properties" );
   ps_xml_prop_iter = ezxml_child( ps_xml_props, "property" );
   while( NULL != ps_xml_prop_iter ) {
      /* Load the current property into the struct. */
      if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "propername" ) ) {
         ps_mob_out->proper_name = bfromcstr( ezxml_attr( ps_xml_prop_iter, "value" ) );

      } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "hp" ) ) {
         ps_mob_out->hp = atoi( ezxml_attr( ps_xml_prop_iter, "value" ) );

      }

      /* Go on to the next one. */
      ps_xml_prop_iter = ezxml_next( ps_xml_prop_iter );
   }

   return ps_mob_out;
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
      s_tile_rect.x = gi_animation_frame * ps_mob_in->pixel_size;
      s_tile_rect.y = ps_mob_in->current_animation * ps_mob_in->pixel_size;

      /* Draw the tile! */
      graphics_draw_blit_sprite(
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
