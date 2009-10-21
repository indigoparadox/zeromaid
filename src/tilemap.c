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
TILEMAP_TILEMAP* tilemap_create_map( bstring ps_path_in ) {
   TILEMAP_TILEMAP* ps_map_out = malloc( sizeof( TILEMAP_TILEMAP ) );
   ezxml_t ps_xml_map = NULL, ps_xml_layer = NULL, ps_xml_props = NULL,
      ps_xml_prop_iter = NULL, ps_xml_tiledata = NULL;
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
   ps_tiledata_path = bfromcstr( PATH_SHARE PATH_MAPDATA "/" );
   ps_tiledata_filename = bfromcstr( ezxml_attr( ps_xml_tiledata, "source" ) );
   bconcat( ps_tiledata_path, ps_tiledata_filename );
   if( file_exists( ps_tiledata_path ) ) {
      /* Load the map property data. */
      ps_xml_props = ezxml_child( ps_xml_map, "properties" );
      ps_xml_prop_iter = ezxml_child( ps_xml_props, "property" );
      while( NULL != ps_xml_prop_iter ) {
         /* Load the current property into the struct. */
         if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "mapname" ) ) {
            ps_map_out->map_name = bfromcstr( ezxml_attr( ps_xml_prop_iter, "value" ) );

         } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "musicpath" ) ) {
            ps_map_out->music_path = bfromcstr( ezxml_attr( ps_xml_prop_iter, "value" ) );

         } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "timegoes" ) ) {
            if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "value" ), "true" ) ) {
               ps_map_out->time_moves = TRUE;
            } else {
               ps_map_out->time_moves = FALSE;
            }

         }

         /* Go to the next one! */
         ps_xml_prop_iter = ps_xml_prop_iter->next;
      }

      /* Load the map's tile data. */
      ps_map_out->tileset = graphics_create_tileset( ps_tiledata_path );

      /* Load the actual map tiles. */
      ps_xml_layer = ezxml_child( ps_xml_map, "layer" );
      while( NULL != ps_xml_layer ) {
         if( 0 == strcmp( ezxml_attr( ps_xml_layer, "name" ), "background" ) ) {
            ps_map_out->tile_w = atoi( ezxml_attr( ps_xml_layer, "width" ) );
            ps_map_out->tile_h = atoi( ezxml_attr( ps_xml_layer, "height" ) );
            ps_map_out->tile_list = tilemap_create_layer( ps_map_out, ps_xml_layer );
         } else {
            DBG_INFO_FILE(
               "Found unrecognized map layer",
               ezxml_attr( ps_xml_layer, "name" )
            );
         }

         /* Go to the next one! */
         ps_xml_layer = ps_xml_layer->next;
      }

   } else {
      DBG_ERR_FILE( "Unable to load tile data", ps_tiledata_path->data );
   }

   /* Clean up. */
   bdestroy( ps_tiledata_path );
   bdestroy( ps_tiledata_filename );
   ezxml_free( ps_xml_map );

   return ps_map_out;
}

TILEMAP_TILE* tilemap_create_layer( TILEMAP_TILEMAP* ps_map_in, ezxml_t ps_xml_layer_in ) {
   ezxml_t ps_xml_tile = NULL, ps_xml_data = NULL;
   TILEMAP_TILE* ps_tile_head = NULL;
   TILEMAP_TILE* ps_tile_iter = NULL;
   unsigned int i_tile_count;

   DBG_INFO_FILE( "Loading background layer for map", ps_map_in->map_name->data );

   ps_xml_data = ezxml_child( ps_xml_layer_in, "data" );
   ps_xml_tile = ezxml_child( ps_xml_data, "tile" );
   while( NULL != ps_xml_tile ) {
      if( NULL == ps_tile_iter ) {
         /* This is the first tile in the list, so make it the head. */
         ps_tile_head = ps_tile_iter = malloc( sizeof( TILEMAP_TILE ) );
      } else {
         ps_tile_iter->next = malloc( sizeof( TILEMAP_TILE ) );
         ps_tile_iter = ps_tile_iter->next;
      }

      /* Verify the last created map tile's allocation. */
      if( NULL == ps_tile_iter ) {
         DBG_ERR( "Unable to allocate map tile." );
         break;
      }

      /* Load the tile's properties. */
      ps_tile_iter->gid = atoi( ezxml_attr( ps_xml_tile, "gid" ) );
      ps_tile_iter->x = i_tile_count % ps_map_in->tile_w;
      ps_tile_iter->y = i_tile_count / ps_map_in->tile_w;

      /* Go to the next one! */
      ps_xml_tile = ps_xml_tile->next;
      i_tile_count++;
   }

   /* Clean up. We don't need to worry about the XML since it was passed from *
    * the caller.                                                             */

   return ps_tile_head;
}

/* Purpose: Draw the part of the given tile map indicated by its viewport to  *
 *          the screen.                                                       */
/* Parameters: The tile map to draw.                                          */
void tilemap_draw( TILEMAP_TILEMAP* ps_map_in ) {

}
