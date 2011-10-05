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

DBG_ENABLE

/* = Functions = */

/* Purpose: Load the map with the given filename into a TILEMAP struct.       */
/* Parameters: The filename to load.                                          */
/* Return: The loaded tile map.                                               */
TILEMAP_TILEMAP* tilemap_create_map( bstring ps_name_in, bstring ps_path_in ) {
   TILEMAP_TILEMAP* ps_map_out = NULL;
   ezxml_t ps_xml_map = NULL, ps_xml_layer = NULL, ps_xml_props = NULL,
      ps_xml_prop_iter = NULL, ps_xml_tiledata = NULL;
   bstring ps_tiledata_path = NULL, ps_tiledata_filename = NULL;

   /* Verify memory allocation. */
   ps_map_out = (TILEMAP_TILEMAP*)calloc( 1, sizeof( TILEMAP_TILEMAP ) );
   if( NULL == ps_map_out ) {
      DBG_ERR( "Unable to allocate map." );
   }

   /* Set the system map name. */
   ps_map_out->sys_name = bformat( "%s", ps_name_in->data );

   /* Verify the XML file exists and open or abort accordingly. */
   if( !zm_file_exists( ps_path_in ) ) {
      DBG_ERR_STR( "Unable to load map", ps_path_in->data );
      return NULL;
   }
   ps_xml_map = ezxml_parse_file( (const char*)ps_path_in->data );

   /* Load the tileset data if it exists, or just let cleanup run if it       *
    * doesn't.                                                                */
   ps_xml_tiledata = ezxml_child( ps_xml_map, "tileset" );
   ps_tiledata_path = bfromcstr( PATH_SHARE );
   ps_tiledata_filename = bfromcstr( ezxml_attr( ps_xml_tiledata, "source" ) );
   bconcat( ps_tiledata_path, ps_tiledata_filename );
   if( zm_file_exists( ps_tiledata_path ) ) {
      /* Load the map property data. */
      ps_xml_props = ezxml_child( ps_xml_map, "properties" );
      ps_xml_prop_iter = ezxml_child( ps_xml_props, "property" );
      while( NULL != ps_xml_prop_iter ) {
         /* Load the current property into the struct. */
         if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "map_name" ) ) {
            ps_map_out->proper_name = bfromcstr( ezxml_attr( ps_xml_prop_iter, "value" ) );

         } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "light_str" ) ) {
            ps_map_out->light_str = atoi( ezxml_attr( ps_xml_prop_iter, "value" ) );

         } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "music_path" ) ) {
            ps_map_out->music_path = bfromcstr( ezxml_attr( ps_xml_prop_iter, "value" ) );

         } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "time_moves" ) ) {
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
      ps_map_out->tileset = tilemap_create_tileset( ps_tiledata_path );

      /* Load the actual map tiles. */
      ps_xml_layer = ezxml_child( ps_xml_map, "layer" );
      while( NULL != ps_xml_layer ) {
         if( 0 == strcmp( ezxml_attr( ps_xml_layer, "name" ), "background" ) ) {
            ps_map_out->tile_w = atoi( ezxml_attr( ps_xml_layer, "width" ) );
            ps_map_out->tile_h = atoi( ezxml_attr( ps_xml_layer, "height" ) );
            tilemap_load_layer( ps_map_out, ps_xml_layer );
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

/* Purpose: Create a tileset that can be used to populate a map.              */
/* Parameters: File system path to the tileset data file.                     */
/* Return: A tileset struct with the image and data prescribed by the data    *
 *         file.                                                              */
TILEMAP_TILESET* tilemap_create_tileset( bstring ps_path_in ) {
   TILEMAP_TILESET* ps_tileset_out = NULL;
   TILEMAP_TILEDATA* ps_tile_new_malloc = NULL;
   ezxml_t ps_xml_tileset, ps_xml_image, ps_xml_tile, ps_xml_props,
      ps_xml_prop_iter;
   bstring ps_image_filename = NULL, ps_image_path = NULL,
      ps_gid_string = NULL, ps_prop_string = NULL;
   int i_gid; /* Tile GID iterator. */
   #ifndef USESERVER
   GFX_SURFACE* ps_surface = NULL;
   #endif /* !USESERVER */

   DBG_INFO_STR( "Loading tile data", ps_path_in->data );

   /* Verify the XML file exists and open or abort accordingly. */
   if( !zm_file_exists( ps_path_in ) ) {
      DBG_ERR_STR( "Unable to load tile data", ps_path_in->data );
      return NULL;
   }
   ps_xml_tileset = ezxml_parse_file( (const char*)ps_path_in->data );

   /* Load the image file. */
   #ifndef USESERVER
   ps_xml_image = ezxml_child( ps_xml_tileset, "image" );
   ps_image_path = bfromcstr( PATH_SHARE );
   ps_image_filename = bfromcstr( ezxml_attr( ps_xml_image, "source" ) );
   bconcat( ps_image_path, ps_image_filename );
   ps_surface = graphics_create_image( ps_image_path );

   if( NULL == ps_surface ) {
      /* There was a problem somewhere. */
      DBG_ERR_STR( "Unable to load tile image", ps_image_path->data );
      goto gct_cleanup;
   }
   #endif /* !USESERVER */

   /* Create the tileset struct. */
   ps_tileset_out = (TILEMAP_TILESET*)calloc( 1, sizeof( TILEMAP_TILESET ) );
   if( NULL == ps_tileset_out ) {
      DBG_ERR( "There was a problem allocating tileset memory." );
      #ifndef USESERVER
      graphics_free_image( ps_surface );
      #endif /* !USESERVER */
      goto gct_cleanup;
   }

   /* Load the properties of each tile into a linked list. */
   ps_xml_tile = ezxml_child( ps_xml_tileset, "tile" );
   while( NULL != ps_xml_tile ) {
      /* Get the GID of the next tile. */
      ps_gid_string = bfromcstr( ezxml_attr( ps_xml_tile, "id" ) );
      if( NULL != ps_gid_string ) {
         i_gid = atoi( (const char*)ps_gid_string->data );
         bdestroy( ps_gid_string );
      } else {
         /* A tile with no GID is useless. */
         bdestroy( ps_gid_string );
         continue;
      }

      /* Ensure the tile list is big enough to reach this GID. */
      if( ps_tileset_out->tile_list_count <= i_gid ) {
         ps_tileset_out->tile_list_count = i_gid + 1;
         ps_tile_new_malloc = (TILEMAP_TILEDATA*)realloc(
            ps_tileset_out->tile_list,
            ps_tileset_out->tile_list_count * sizeof( TILEMAP_TILEDATA )
         );

         /* Verify memory allocaton. */
         if( NULL == ps_tile_new_malloc ) {
            /* There was a memory allocation problem! */
            DBG_ERR( "There was a problem allocating tile memory." );
            break;
         } else {
            ps_tileset_out->tile_list = ps_tile_new_malloc;
         }
      }

      /* Ensure that the GID's cell is empty. */
      memset(
         &ps_tileset_out->tile_list[i_gid],
         0,
         sizeof( TILEMAP_TILEDATA )
      );

      /* Cycle through all present properties and add each one to the      *
       * final struct.                                                     */
      ps_xml_props = ezxml_child( ps_xml_tile, "properties" );
      ps_xml_prop_iter = ezxml_child( ps_xml_props, "property" );
      while( NULL != ps_xml_prop_iter ) {
         /* Load the current property into the struct. */
         ps_prop_string = bfromcstr( ezxml_attr( ps_xml_prop_iter, "value" ) );
         if(
            NULL != ps_prop_string &&
            0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "hindrance" )
         ) {
            ps_tileset_out->tile_list[i_gid].hindrance =
               atoi( (const char*)ps_prop_string->data );

         } else if(
            NULL != ps_prop_string &&
            0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "animated" )
         ) {
            if( zm_string_is_true( ps_prop_string ) ) {
               ps_tileset_out->tile_list[i_gid].animated = TRUE;
            }
         }

         /* Clean up and go to the next one! */
         bdestroy( ps_prop_string );
         ps_xml_prop_iter = ezxml_next( ps_xml_prop_iter );
      }

      /* Go to the next one! */
      ps_xml_tile = ezxml_next( ps_xml_tile );
   }

   #ifndef USESERVER
   /* Place the image into the tileset struct. */
   ps_tileset_out->image = ps_surface;
   #endif /* !USESERVER */
   ps_tileset_out->image_filename = bformat(
      "%s", ezxml_attr( ps_xml_image, "source" )
   );

   /* Figure out the file size. */
   ps_tileset_out->pixel_size = atoi( ezxml_attr( ps_xml_tileset, "tileheight" ) );
   DBG_INFO_INT( "Found tile size", ps_tileset_out->pixel_size );

   DBG_INFO_STR_PTR( "Loaded tile data", ps_path_in->data, ps_tileset_out );

gct_cleanup:

   /* Clean up. */
   bdestroy( ps_image_path );
   bdestroy( ps_image_filename );
   ezxml_free( ps_xml_tileset );

   return ps_tileset_out;
}

/* Purpose: Get the X position in tiles for the given tile on the given map.  */
int tilemap_get_tile_x( int i_tile_index_in, TILEMAP_TILEMAP* ps_map_in ) {
   return i_tile_index_in % ps_map_in->tile_w;
}

/* Purpose: Get the Y position in tiles for the given tile on the given map.  */
int tilemap_get_tile_y( int i_tile_index_in, TILEMAP_TILEMAP* ps_map_in ) {
   return i_tile_index_in / ps_map_in->tile_w;
}

/* Purpose: Get a pointer to the tile at the given X, Y coordinates on the    *
 *          given map.                                                        */
/* Parameters: The X and Y coordinates of the desired tile and a pointer to   *
 *             the desired map.                                               */
TILEMAP_TILE* tilemap_get_tile(
   int i_x_in,
   int i_y_in,
   TILEMAP_TILEMAP* ps_map_in
) {
   return &ps_map_in->tiles[(i_y_in * ps_map_in->tile_w) + i_x_in];
}

/* Purpose: Get the data for the tile with the given GID.                     */
/* Parameters: The index to lookup and the tileset in which to look it up.    */
/* Return: The address of the requested tile.                                 */
TILEMAP_TILEDATA* tilemap_get_tiledata(
   int i_gid_in, TILEMAP_TILESET* ps_tileset_in
) {
   return &ps_tileset_in->tile_list[i_gid_in - 1];
}

/* Purpose: Get the amount to add to the X coordinate to go in the specified  *
 *          direction.                                                        */
/* Parameters: The direction to go in.                                        */
/* Return: The amount to add to an X coordinate.                              */
int tilemap_dir_get_add_x( TILEMAP_DIR i_dir_in ) {
   switch( i_dir_in ) {
      case TILEMAP_DIR_EAST:
         return 1;

      case TILEMAP_DIR_WEST:
         return -1;

      default:
         return 0;
   }
}

/* Purpose: Get the amount to add to the Y coordinate to go in the specified  *
 *          direction.                                                        */
/* Parameters: The direction to go in.                                        */
/* Return: The amount to add to an Y coordinate.                              */
int tilemap_dir_get_add_y( TILEMAP_DIR i_dir_in ) {
   switch( i_dir_in ) {
      case TILEMAP_DIR_NORTH:
         return -1;

      case TILEMAP_DIR_SOUTH:
         return 1;

      default:
         return 0;
   }
}

/* Purpose: Load the specified layer into the given map struct.               */
/* Parameters: The map struct into which to load, the XML struct describing   *
 *             the layer.                                                     */
void tilemap_load_layer( TILEMAP_TILEMAP* ps_map_in, ezxml_t ps_xml_layer_in ) {
   ezxml_t ps_xml_tile = NULL, ps_xml_data = NULL;
   //unsigned int i_row_count = 0; /* The current tile row. *//

   DBG_INFO_STR(
      "Loading background layer for map",
      ps_map_in->proper_name->data
   );

   ps_xml_data = ezxml_child( ps_xml_layer_in, "data" );

   /* Now load the data for each tile. */
   ps_xml_tile = ezxml_child( ps_xml_data, "tile" );
   while( NULL != ps_xml_tile ) {
      /* A tile was found in the XML, so prepare the array for it. */
      ps_map_in->tiles_count++;
      ps_map_in->tiles = (TILEMAP_TILE*)realloc(
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
      /* ps_map_in->tiles[ps_map_in->tiles_count - 1].tile_x =
         (ps_map_in->tiles_count - 1) % ps_map_in->tile_w;
      ps_map_in->tiles[ps_map_in->tiles_count - 1].tile_y =
         (ps_map_in->tiles_count - 1) / ps_map_in->tile_w; */

      /* Go to the next one! */
      ps_xml_tile = ps_xml_tile->next;
   }

   /* Clean up. We don't need to worry about the XML since it was passed from *
    * the caller.                                                             */
}

#ifndef USESERVER
/* Purpose: Draw the part of the given tile map indicated by its viewport to  *
 *          the screen.                                                       */
/* Parameters: The tile map to draw, the current viewport, and a boolean      *
 *             indicating whether or not to "force" drawing (that is, ignore  *
 *             animation or dirtiness and just redraw everything.             */
void tilemap_draw(
   TILEMAP_TILEMAP* ps_map_in,
   GEO_RECTANGLE* ps_viewport_in,
   BOOL b_force_in
) {
   int i_tile_start_x = 0, i_tile_start_y = 0, /* Tile coordinates within     */
      i_tile_width = 0, i_tile_height = 0,     /* which to draw.              */
      x_tile, y_tile, /* Loop iterators. */
      i_tiledata_index;
   GEO_RECTANGLE s_tile_rect, s_screen_rect; /* Blit the tile from/to. */
   static int ti_anim_frame = 0;
   static int ti_frame_draws = 0;
   TILEMAP_TILEDATA* ps_tiledata = NULL; /* Data on the tile being drawn. */

   /* What animation frame are we on? */
   ti_frame_draws++;
   if( TILEMAP_ANIM_DRAWS <= ti_frame_draws ) {
      if( TILEMAP_ANIM_FRAMES <= ti_anim_frame ) {
         ti_anim_frame = 0;
      } else {
         ti_anim_frame++;
      }
      ti_frame_draws = 0;
   }

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
   for(
      y_tile = i_tile_start_y;
      y_tile < i_tile_start_y + i_tile_height;
      y_tile++
   ) {
      for(
         x_tile = i_tile_start_x;
         x_tile < i_tile_start_x + i_tile_width;
         x_tile++
      ) {
         i_tiledata_index = (y_tile * ps_map_in->tile_w) + x_tile;
         if( i_tiledata_index >= ps_map_in->tiles_count ) {
            /* Invalid tile attempted. */
            continue;
         }

         ps_tiledata = graphics_get_tiledata(
            ps_map_in->tiles[i_tiledata_index].gid,
            ps_map_in->tileset
         );
         if(
            !b_force_in &&
            (!ps_tiledata->animated &&
            !ps_map_in->tiles[i_tiledata_index].dirty)
         ) {
            /* This tile hasn't changed, so don't bother redrawing it. */
            continue;
         }

         /* Figure out the offset of the tile onscreen. */
         s_screen_rect.x =
            (x_tile * ps_map_in->tileset->pixel_size) - ps_viewport_in->x;
         s_screen_rect.y =
            (y_tile * ps_map_in->tileset->pixel_size) - ps_viewport_in->y;

         /* Figure out where on the tilesheet the tile is. Bear in mind that  *
          * GIDs are 1-indexed.                                               */
         /* TODO: Add offset for night time. */
         s_tile_rect.x = ti_anim_frame * ps_map_in->tileset->pixel_size;
         s_tile_rect.y =
            (ps_map_in->tiles[(y_tile * ps_map_in->tile_w) + x_tile].gid - 1) *
            ps_map_in->tileset->pixel_size;

         /* Draw the tile. */
         graphics_draw_blit_tile(
            ps_map_in->tileset->image,
            &s_tile_rect,
            &s_screen_rect
         );
      }
   }
}
#endif /* !USESERVER */

/* Purpose: Free the given tile map pointer.                                  */
/* Parameters: The tile map pointer to free.                                  */
void tilemap_free( TILEMAP_TILEMAP* ps_map_in ) {
   if( NULL == ps_map_in ) {
      return;
   }

   free( ps_map_in->tiles );
   bdestroy( ps_map_in->proper_name );
   bdestroy( ps_map_in->sys_name );
   bdestroy( ps_map_in->music_path );
   tilemap_free_tileset( ps_map_in->tileset );
   free( ps_map_in->viewport );
   free( ps_map_in );
}

/* Purpose: Free the given tileset buffer.                                    */
/* Parameters: The tileset to free.                                           */
void tilemap_free_tileset( TILEMAP_TILESET* ps_tileset_in ) {
   if( NULL == ps_tileset_in ) {
      return;
   }

   #ifndef USESERVER
   graphics_free_image( ps_tileset_in->image );
   #endif /* !USESERVER */

   free( ps_tileset_in );
}
