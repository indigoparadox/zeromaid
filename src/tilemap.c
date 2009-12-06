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

/* = Global Variables= */

extern FILE* gps_debug;
extern int gi_animation_frame;

/* = Functions = */

/* Purpose: Load the map with the given filename into a TILEMAP struct.       */
/* Parameters: The filename to load.                                          */
/* Return: The loaded tile map.                                               */
TILEMAP_TILEMAP* tilemap_create_map( bstring ps_path_in ) {
   TILEMAP_TILEMAP* ps_map_out = calloc( 1, sizeof( TILEMAP_TILEMAP ) );
   ezxml_t ps_xml_map = NULL, ps_xml_layer = NULL, ps_xml_props = NULL,
      ps_xml_prop_iter = NULL, ps_xml_tiledata = NULL;
   bstring ps_tiledata_path = NULL, ps_tiledata_filename = NULL;

   /* Verify memory allocation. */
   if( NULL == ps_map_out ) {
      DBG_ERR( "Unable to allocate map." );
   }

   /* Verify the XML file exists and open or abort accordingly. */
   if( !file_exists( ps_path_in ) ) {
      DBG_ERR_STR( "Unable to load map", ps_path_in->data );
      return NULL;
   }
   ps_xml_map = ezxml_parse_file( ps_path_in->data );

   /* Load the tileset data if it exists, or just let cleanup run if it       *
    * doesn't.                                                                */
   ps_xml_tiledata = ezxml_child( ps_xml_map, "tileset" );
   ps_tiledata_path = bfromcstr( PATH_SHARE );
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
            tilemap_create_layer( ps_map_out, ps_xml_layer );
         } else {
            DBG_INFO_STR(
               "Found unrecognized map layer",
               ezxml_attr( ps_xml_layer, "name" )
            );
         }

         /* Go to the next one! */
         ps_xml_layer = ps_xml_layer->next;
      }

   } else {
      DBG_ERR_STR( "Unable to load tile data", ps_tiledata_path->data );
   }

   /* Clean up. */
   bdestroy( ps_tiledata_path );
   bdestroy( ps_tiledata_filename );
   ezxml_free( ps_xml_map );

   return ps_map_out;
}

void tilemap_create_layer( TILEMAP_TILEMAP* ps_map_in, ezxml_t ps_xml_layer_in ) {
   ezxml_t ps_xml_tile = NULL, ps_xml_data = NULL;
   //unsigned int i_row_count = 0; /* The current tile row. *//

   DBG_INFO_STR( "Loading background layer for map", ps_map_in->map_name->data );

   ps_xml_data = ezxml_child( ps_xml_layer_in, "data" );

   /* Now load the data for each tile. */
   ps_xml_tile = ezxml_child( ps_xml_data, "tile" );
   while( NULL != ps_xml_tile ) {
      /* A tile was found in the XML, so prepare the array for it. */
      ps_map_in->tiles_count++;
      ps_map_in->tiles = realloc(
         ps_map_in->tiles,
         ps_map_in->tiles_count * sizeof( TILEMAP_TILE )
      );

      /* Verify the last created map tile's allocation. */
      if( NULL == ps_map_in->tiles ) {
         DBG_ERR( "Unable to allocate map tile." );
         break;
      }
      memset(
         &ps_map_in->tiles[ps_map_in->tiles_count - 1],
         0,
         sizeof( TILEMAP_TILE )
      );

      /* Load the tile's properties. */
      ps_map_in->tiles[ps_map_in->tiles_count - 1].gid =
         atoi( ezxml_attr( ps_xml_tile, "gid" ) );
      ps_map_in->tiles[ps_map_in->tiles_count - 1].tile_x =
         (ps_map_in->tiles_count - 1) % ps_map_in->tile_w;
      ps_map_in->tiles[ps_map_in->tiles_count - 1].tile_y =
         (ps_map_in->tiles_count - 1) / ps_map_in->tile_w;

      /* Go to the next one! */
      ps_xml_tile = ps_xml_tile->next;
   }

   /* Clean up. We don't need to worry about the XML since it was passed from *
    * the caller.                                                             */
}

/* Purpose: Draw the part of the given tile map indicated by its viewport to  *
 *          the screen.                                                       */
/* Parameters: The tile map to draw, the current viewport.                    */
void tilemap_draw( TILEMAP_TILEMAP* ps_map_in, GFX_RECTANGLE* ps_viewport_in ) {
   int i_tile_start_x = 0, i_tile_start_y = 0, /* Tile coordinates within     */
      i_tile_width = 0, i_tile_height = 0,     /* which to draw.              */
      i = 0; /* Loop iterator. */
   GFX_TILEDATA* ps_tile_data = NULL; /* The data for the iterated tile. */
   GFX_RECTANGLE s_tile_rect, s_screen_rect; /* Blit the tile from/to. */

   /* Setup the blit source and destination. */
   s_screen_rect.w = ps_map_in->tileset->pixel_size;
   s_screen_rect.h = ps_map_in->tileset->pixel_size;
   s_tile_rect.w = ps_map_in->tileset->pixel_size;
   s_tile_rect.h = ps_map_in->tileset->pixel_size;

   /* Figure out which tiles should be drawn from the viewport given. */
   i_tile_start_x = ps_viewport_in->x / ps_map_in->tileset->pixel_size;
   i_tile_start_y = ps_viewport_in->y / ps_map_in->tileset->pixel_size;
   i_tile_width = ps_viewport_in->w / ps_map_in->tileset->pixel_size;
   i_tile_height = ps_viewport_in->h / ps_map_in->tileset->pixel_size;

   /* Loop through all of the tiles in the map and draw the ones within the   *
    * current viewable range.                                                 */
   for( i = 0 ; i < ps_map_in->tiles_count ; i++ ) {
      if(
         ps_map_in->tiles[i].tile_x < (i_tile_start_x + i_tile_width) &&
         ps_map_in->tiles[i].tile_y < (i_tile_start_y + i_tile_height) &&
         ps_map_in->tiles[i].tile_x >= i_tile_start_x &&
         ps_map_in->tiles[i].tile_y >= i_tile_start_y
      ) {
         /* Figure out the offset of the tile onscreen. */
         s_screen_rect.x = (ps_map_in->tiles[i].tile_x - i_tile_start_x) *
            ps_map_in->tileset->pixel_size;
         s_screen_rect.y = (ps_map_in->tiles[i].tile_y - i_tile_start_y) *
            ps_map_in->tileset->pixel_size;

         /* Figure out where on the tilesheet the tile is. Bear in mind that  *
          * GIDs are 1-indexed.                                               */
         /* TODO: Add offset for night time. */
         s_tile_rect.x = gi_animation_frame * ps_map_in->tileset->pixel_size;
         s_tile_rect.y =
            (ps_map_in->tiles[i].gid - 1) * ps_map_in->tileset->pixel_size;

         /* Draw the tile! */
         ps_tile_data = graphics_get_tiledata(
            ps_map_in->tiles[i].gid, ps_map_in->tileset
         );
         graphics_draw_blit_tile(
            ps_map_in->tileset->image,
            &s_tile_rect,
            &s_screen_rect
         );
      }
   }
}

/* Purpose: Free the given tile map pointer.                                  */
/* Parameters: The tile map pointer to free.                                  */
void tilemap_free( TILEMAP_TILEMAP* ps_map_in ) {
   free( ps_map_in->tiles );
   bdestroy( ps_map_in->map_name );
   bdestroy( ps_map_in->music_path );
   graphics_free_tileset( ps_map_in->tileset );
   free( ps_map_in->viewport );
   free( ps_map_in );
}
