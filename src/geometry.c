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

/* = Includes = */

#include "geometry.h"

/* = Functions = */

/* Purpose: Generate a color for the given parameters.                        */
/* Parameters: Red, green, blue, with 255 being the strongest.                */
/* Return: A pointer to the specified color struct.                           */
GEO_COLOR* geometry_create_color(
   unsigned char i_red_in,
   unsigned char i_green_in,
   unsigned char i_blue_in
) {
   GEO_COLOR* ps_color_out = NULL;

   /* This should be safe for any available graphics platform, but just note  *
    * that maybe it might have to be tweaked if others are added.             */
   ps_color_out = (GEO_COLOR*)calloc( 1, sizeof( GEO_COLOR ) );
   if( NULL == ps_color_out ) {
      DBG_ERR( "Unable to allocate color." );
   }
   ps_color_out->r = i_red_in;
   ps_color_out->g = i_green_in;
   ps_color_out->b = i_blue_in;

   return ps_color_out;
}

/* Purpose: Parse a color from HTML #XXXXXX notation.                         */
/* Parameters: A string containing the color to parse.                        */
/* Return: The requested color.                                               */
GEO_COLOR* geometry_create_color_html( bstring ps_color_in ) {
   int i_red = 0, i_green = 0, i_blue = 0;
   char ac_color_iter[3] = { '\0' };
   char* pc_color_in = bdata( ps_color_in );

   /* Red */
   ac_color_iter[0] = pc_color_in[1];
   ac_color_iter[1] = pc_color_in[2];
   i_red = strtol( ac_color_iter, NULL, 16 );

   /* Green */
   ac_color_iter[0] = pc_color_in[3];
   ac_color_iter[1] = pc_color_in[4];
   i_green = strtol( ac_color_iter, NULL, 16 );

   /* Blue */
   ac_color_iter[0] = pc_color_in[5];
   ac_color_iter[1] = pc_color_in[6];
   i_blue = strtol( ac_color_iter, NULL, 16 );

   return geometry_create_color( i_red, i_green, i_blue );
}
