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

#include "tilemap.h"

/* Purpose: Load the map with the given filename into a TILEMAP struct.       */
/* Parameters: The filename to load.                                          */
/* Return: The loaded tile map.                                               */
TILEMAP_TILEMAP* tilemap_create( bstring ps_path_in ) {
   TILEMAP_TILEMAP* ps_map_out = malloc( sizeof( TILEMAP_TILEMAP ) );
   ezxml_t ps_xml_map = NULL, ps_xml_layer0 = NULL, ps_xml_tile = NULL,
      ps_xml_props = NULL, ps_xml_prop_iter = NULL, ps_xml_tiledata = NULL;
   bstring ps_tiledata_path = NULL, ps_tiledata_filename = NULL;

   /* Verify memory allocation. */
   if( NULL == ps_map_out ) {
      DBG_ERR( "Unable to allocate map." );
   } else {
      memset( ps_map_out, 0, sizeof( TILEMAP_TILEMAP ) );
   }

   /* Verify the XML file exists and open or abort accordingly. */
   if( !file_exists( ps_path_in ) ) {
      DBG_ERR_FILE( "Unable to load map", ps_path_in->data );
      return NULL;
   }
   ps_xml_map = ezxml_parse_file( ps_path_in->data );

   /* Load the tileset data if it exists, or just let cleanup run if it       *
    * doesn't.                                                                */
   ps_xml_tiledata = ezxml_child( ps_xml_map, "tileset" );
   ps_tiledata_path = cstr2bstr( PATH_SHARE PATH_MAPDATA "/" );
   ps_tiledata_filename = cstr2bstr( ezxml_attr( ps_xml_tiledata, "source" ) );
   bconcat( ps_tiledata_path, ps_tiledata_filename );
   if( file_exists( ps_tiledata_path ) ) {
      /* Load the tile data. */
      ps_map_out->tileset = graphics_create_tileset( ps_tiledata_path );

      /* Load the tile positions. */

   } else {
      DBG_ERR_FILE( "Unable to load tile data", ps_tiledata_path->data );
   }

   /* Clean up. */
   bdestroy( ps_tiledata_path );
   bdestroy( ps_tiledata_filename );
   ezxml_free( ps_xml_map );

   return ps_map_out;
}

/* Purpose: Draw the part of the given tile map indicated by its viewport to  *
 *          the screen.                                                       */
/* Parameters: The tile map to draw.                                          */
void tilemap_draw( TILEMAP_TILEMAP* ps_map_in ) {

}
