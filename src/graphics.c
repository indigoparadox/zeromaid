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

#include "graphics.h"

DBG_ENABLE

/* = Global Variables = */

#ifdef USEDIRECTX
LPDIRECTDRAW7 gps_dx_ddraw = NULL;
LPDIRECTDRAWSURFACE7 gps_surface_primary = NULL;
LPDIRECTDRAWSURFACE7 gps_surface_back = NULL;
DDSURFACEDESC2 gs_surface_desc;
#elif defined USEALLEGRO
GFX_SURFACE* gps_screen_buffer = NULL;
#endif /* USEDIRECTX, USEALLEGRO */

/* = Functions = */

/* Purpose: Prepare the surface that, when drawn to, draws to the screen. The *
 *          surface can then be accessed through a global mechanism later.    */
/* Parameters: Screen width, screen height, screen color depth, screen window *
 *             title.                                                         */
/* Return: A boolean indicating success or failure.                           */
BOOL graphics_create_screen(
   int i_width_in, int i_height_in, int i_depth_in, bstring ps_title_in
) {
   BOOL b_success = TRUE;

   #ifdef USESDL
   if( NULL == SDL_SetVideoMode(
      i_width_in, i_height_in, i_depth_in, SDL_HWSURFACE | SDL_DOUBLEBUF
   ) ) {
      DBG_ERR_STR( "Unable to setup screen", SDL_GetError() );
      b_success = FALSE;
      goto gcs_cleanup;
   } else {
      /* The screen was opened. */
      SDL_WM_SetCaption(
         (const char *)ps_title_in->data, (const char *)ps_title_in->data
      );
   }
   #elif defined USEDIRECTX
   if( FAILED(
      DirectDrawCreateEx( NULL, (void**)gps_dx_ddraw, IID_IDirectDraw7, NULL )
   ) ) {
      DBG_ERR( "Unable to setup screen." );
      b_success = FALSE;
      goto gcs_cleanup;
   }

   /* Attach the primary surface. */
   gs_surface_desc.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
   gs_surface_desc.dwBackBufferCount = 1;
   gs_surface_desc.ddsCaps.dwCaps =
      DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;

   if( gps_dx_ddraw->CreateSurface(
      &gs_surface_desc, &gps_surface_primary, NULL
   ) != DD_OK ) {
      DBG_ERR( "Failed to create DirectDraw primary surface." );
      b_success = FALSE;
      goto gcs_cleanup;
   }

   gs_surface_desc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

   if( gps_surface_primary->GetAttachedSurface(
      &gs_surface_desc.ddsCaps, &gps_surface_back
   ) != DD_OK ) {
      DBG_ERR( "Failed to create DirectDraw backbuffer surface." );
      b_success = FALSE;
      goto gcs_cleanup;
   }
   #elif defined USEALLEGRO
   if( 0 == set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0 ) ) {
      /* Everything went better than expected! */
      goto gcs_cleanup;
   } else if( 0 == set_gfx_mode(GFX_SAFE, 640, 480, 0, 0) ) {
	   /* Beggars can't be choosers... */
	   DBG_ERR_STR( "Unable to set auto graphic mode", allegro_error );
	   goto gcs_cleanup;
   } else {
      DBG_ERR_STR( "Unable to set any graphic mode", allegro_error );
      b_success = FALSE;
      goto gcs_cleanup;
   }

   /* Now that the screen is taken care of, setup the buffer page. */
   gps_screen_buffer = create_system_bitmap( SCREEN_W, SCREEN_H );
   if( NULL == gps_screen_buffer ) {
      DBG_ERR_STR( "Unable to setup screen buffer", allegro_error );
      b_success = FALSE;
      goto gcs_cleanup;
   }
   #else
   #error "No screen-getting mechanism defined for this platform!"
   #endif /* USESDL */

gcs_cleanup:

   return b_success;
}

/* Purpose: Create a surface with the specified image loaded onto it.         */
/* Parameters: File system path to the desired image.                         */
/* Return: A surface with the specified image loaded onto it.                 */
GFX_SURFACE* graphics_create_image( bstring ps_path_in ) {
   GFX_SURFACE* ps_image = NULL;

   #ifdef USESDL
   Uint32 i_color_key = 0;
   GFX_SURFACE* ps_temp = NULL; /* Image before transparency. */

   /* Load the temporary image. */
   ps_temp = SDL_LoadBMP( (const char*)ps_path_in->data );

   if( NULL != ps_temp ) {
      ps_image = SDL_DisplayFormat( ps_temp );

      /* Setup transparency. */
      i_color_key = SDL_MapRGB( ps_image->format, 0xFF, 0, 0xFF );
      SDL_SetColorKey( ps_image, SDL_RLEACCEL | SDL_SRCCOLORKEY, i_color_key );

      DBG_INFO_STR_PTR( "Loaded image", (const char*)ps_path_in->data, ps_image );
   } else {
      DBG_ERR_STR( "Failed to load image", ps_path_in->data );
      goto gci_cleanup;
   }
   #elif defined USEDIRECTX
   FILE* ps_file_bmp = NULL; /* Pointer to read extra bitmap info. */
   long i_offset,
      i_width,
      i_height;
	short i_type;

   /* Verify that the given path leads to a bitmap. */
   ps_file_bmp = fopen( (const char*)ps_path_in->data, "rb" );
	if( NULL == ps_file_bmp ) {
      DBG_ERR_STR( "Failed to load image", ps_path_in->data );
      goto gci_cleanup;
	}
	fread( &i_type, sizeof( short ), 1, ps_file_bmp );
	if( 0x4D42 != i_type ) {
      DBG_ERR_STR( "Invalid bitmap specified", ps_path_in->data );
      goto gci_cleanup;
	}

	/* Get the bitmap height/width. */
	fseek( ps_file_bmp, 10, SEEK_SET );
	fread( &i_offset, sizeof( long ), 1, ps_file_bmp );
	fseek( ps_file_bmp, 4, SEEK_CUR );
	fread( &i_width, sizeof( long ), 1, ps_file_bmp );
	fread( &i_height, sizeof( long ), 1, ps_file_bmp );
	fseek( ps_file_bmp, 2, SEEK_CUR );
	fclose( ps_file_bmp );

   /* Build and fill the surface struct. */
   ps_image = (GFX_SURFACE*)calloc( 1, sizeof( GFX_SURFACE ) );
   if( NULL == ps_image ) {
      DBG_ERR( "Unable to allocate surface." );
      goto gci_cleanup;
   }

   ps_image->surface = DDLoadBitmap( gps_dx_ddraw, (LPCSTR)ps_path_in->data, 0, 0 );
   ps_image->w = i_width;
   ps_image->h = i_height;
   #elif defined USEALLEGRO
   ps_image = load_bitmap( ps_path_in->data, NULL );
   #else
   #error "No image loading mechanism defined for this platform!"
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */

gci_cleanup:

   #ifdef USESDL
   SDL_FreeSurface( ps_temp );
   #elif defined USEDIRECTX
   fclose( ps_file_bmp );
   #endif /* USESDL */

   return ps_image;
}

/* Purpose: Create a spritesheet that can be used to populate a mobile or     *
 *          foreground object.                                                */
/* Parameters: File system path to the spritesheet data file.                 */
/* Return: A spritesheet struct with the image and data prescribed by the     *
 *         data file.                                                         */
GFX_SPRITESHEET* graphics_create_spritesheet( bstring ps_path_in ) {
   GFX_SPRITESHEET* ps_spritesheet_out =
      (GFX_SPRITESHEET*)calloc( sizeof( GFX_SPRITESHEET ), 1 );

   /* Create the spritesheet structure. */
   if( NULL == ps_spritesheet_out ) {
      DBG_ERR( "Unable to allocate spritesheet." );
      goto gcs_cleanup;
   }

   /* Load the spritesheet image. */
   ps_spritesheet_out->image = graphics_create_image( ps_path_in );
   if( NULL == ps_spritesheet_out ) {
      /* graphics_create_image() should handle any error output. */
      free( ps_spritesheet_out );
      ps_spritesheet_out = NULL;
      goto gcs_cleanup;
   }

   /* TODO: Figure out the pixel size automatically. */
   ps_spritesheet_out->pixel_size = 32;

gcs_cleanup:

   return ps_spritesheet_out;
}

/* Purpose: Create a tileset that can be used to populate a map.              */
/* Parameters: File system path to the tileset data file.                     */
/* Return: A tileset struct with the image and data prescribed by the data    *
 *         file.                                                              */
GFX_TILESET* graphics_create_tileset( bstring ps_path_in ) {
   GFX_TILESET* ps_tileset_out = NULL;
   GFX_SURFACE* ps_surface = NULL;
   GFX_TILEDATA* ps_tile_new_malloc = NULL;
   ezxml_t ps_xml_tileset, ps_xml_image, ps_xml_tile, ps_xml_props,
      ps_xml_prop_iter;
   bstring ps_image_filename = NULL, ps_image_path = NULL,
      ps_gid_string = NULL, ps_prop_string = NULL;
   int i_gid; /* Tile GID iterator. */

   DBG_INFO_STR( "Loading tile data", ps_path_in->data );

   /* Verify the XML file exists and open or abort accordingly. */
   if( !zm_file_exists( ps_path_in ) ) {
      DBG_ERR_STR( "Unable to load tile data", ps_path_in->data );
      return NULL;
   }
   ps_xml_tileset = ezxml_parse_file( (const char*)ps_path_in->data );

   /* Load the image file. */
   ps_xml_image = ezxml_child( ps_xml_tileset, "image" );
   ps_image_path = bfromcstr( PATH_SHARE );
   ps_image_filename = bfromcstr( ezxml_attr( ps_xml_image, "source" ) );
   bconcat( ps_image_path, ps_image_filename );
   ps_surface = graphics_create_image( ps_image_path );

   /* If the surface loaded all right, then construct the tileset. */
   if( NULL != ps_surface ) {
      /* Create the tileset struct. */
      ps_tileset_out = (GFX_TILESET*)calloc( 1, sizeof( GFX_TILESET ) );
      if( NULL == ps_tileset_out ) {
         DBG_ERR( "There was a problem allocating tileset memory." );
         graphics_free_image( ps_surface );
         goto gct_cleanup;
      }

      /* Load the properties of each tile into a linked list. */
      ps_xml_tile = ezxml_child( ps_xml_tileset, "tile" );
      while( NULL != ps_xml_tile ) {
         /* Get the GID of the next tile. */
         ps_gid_string = bformat( "%s", ezxml_attr( ps_xml_tile, "id" ) );
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
            ps_tile_new_malloc = (GFX_TILEDATA*)realloc(
               ps_tileset_out->tile_list,
               ps_tileset_out->tile_list_count * sizeof( GFX_TILEDATA )
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
            sizeof( GFX_TILEDATA )
         );

         /* Cycle through all present properties and add each one to the      *
          * final struct.                                                     */
         ps_xml_props = ezxml_child( ps_xml_tile, "properties" );
         ps_xml_prop_iter = ezxml_child( ps_xml_props, "property" );
         while( NULL != ps_xml_prop_iter ) {
            /* Load the current property into the struct. */
            ps_prop_string = bformat( "%s", ezxml_attr( ps_xml_prop_iter, "value" ) );
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

      /* Place the image into the tileset struct. */
      ps_tileset_out->image = ps_surface;

      /* Figure out the file size. */
      ps_tileset_out->pixel_size = atoi( ezxml_attr( ps_xml_tileset, "tileheight" ) );
      DBG_INFO_INT( "Found tile size", ps_tileset_out->pixel_size );

      DBG_INFO_STR_PTR( "Loaded tile data", ps_path_in->data, ps_tileset_out );
   } else {
      /* There was a problem somewhere. */
      DBG_ERR_STR( "Unable to load tile image", ps_image_path->data );
   }

gct_cleanup:

   /* Clean up. */
   bdestroy( ps_image_path );
   bdestroy( ps_image_filename );
   ezxml_free( ps_xml_tileset );

   return ps_tileset_out;
}

/* Purpose: Generate a color for the given parameters.                        */
/* Parameters: Red, green, blue, with 255 being the strongest.                */
/* Return: A pointer to the specified color struct.                           */
GFX_COLOR* graphics_create_color(
   unsigned char i_red_in,
   unsigned char i_green_in,
   unsigned char i_blue_in
) {
   GFX_COLOR* ps_color_out = NULL;

   #if defined USESDL || defined USEDIRECTX || defined USEALLEGRO
   ps_color_out = (GFX_COLOR*)calloc( 1, sizeof( GFX_COLOR ) );
   if( NULL == ps_color_out ) {
      DBG_ERR( "Unable to allocate color." );
   }
   ps_color_out->r = i_red_in;
   ps_color_out->g = i_green_in;
   ps_color_out->b = i_blue_in;
   #else
   #error "No color creation mechanism defined for this platform!"
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */

   return ps_color_out;
}

/* Purpose: Parse a color from HTML #XXXXXX notation.                         */
/* Parameters: A string containing the color to parse.                        */
/* Return: The requested color.                                               */
GFX_COLOR* graphics_create_color_html( bstring ps_color_in ) {
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

   return graphics_create_color( i_red, i_green, i_blue );
}

GFX_FONT* graphics_create_font( bstring ps_font_path_in, int i_size_in ) {
   GFX_FONT* ps_font_out;
   bstring ps_font_path_proc;

   /* Some font loaders are different, so we might need to alter the inputted *
    * path.                                                                   */
   ps_font_path_proc = bformat( "%s", ps_font_path_in->data );

   #ifdef USESDL
   ps_font_out = TTF_OpenFont(
      (const char *)ps_font_path_in->data, i_size_in
   );
   #elif defined USEALLEGRO
   bstring ps_font_size_txt;
   int i_file_extension_start;

   /* Allegro handles fonts a little differently and requires its own bitmap  *
    * format anyway, so we'll incorporate the size into the filename.         */
   ps_font_size_txt = bformat( "_%d", i_size_in );
   i_file_extension_start = bstrrchr( ps_font_path_proc, '.' );
   binsert( ps_font_path_proc, i_file_extension_start, ps_font_size_txt, NULL );

   ps_font_out = load_font( ps_font_path_proc->data, NULL, NULL );

   /* Clean up. */
   bdestroy( ps_font_size_txt );
   #else
   #error "No font loading mechanism defined for this platform!"
   #endif /* USESDL, USEALLEGRO */

   if( NULL == ps_font_out ) {
      DBG_ERR_STR( "Unable to load font", ps_font_path_proc->data );

      #ifdef USESDL
      DBG_ERR_STR( "SDL TTF Error was", TTF_GetError() );
      #endif /* USESDL */
   } else {
      DBG_INFO_STR( "Font loaded", ps_font_path_proc->data );
   }

gcf_cleanup:

   bdestroy( ps_font_path_proc );

   return ps_font_out;
}

/* Purpose: Render a given text string to the screen.                         */
/* Parameters: Coordinates to render to, string to render, font object, point *
 * size, render color.                                                        */
void graphics_draw_text(
   int i_x_in,
   int i_y_in,
   bstring ps_string_in,
   GFX_FONT* ps_font_in,
   GFX_COLOR* ps_color_in
) {
   #ifdef USESDL
   GFX_SURFACE* ps_type_render_out = NULL;
   SDL_Color ps_color_sdl;
   SDL_Rect ps_destreg;
   #endif /* USESDL */

   /* Sanity check before we begin. */
   if(
      NULL == ps_string_in ||
      NULL == ps_font_in ||
      NULL == ps_color_in
   ) {
      return;
   }

   #ifdef USESDL
   /* Handle format conversions. */
   CONV_COLOR_SDL( ps_color_sdl, ps_color_in );

   ps_type_render_out = TTF_RenderText_Solid(
      ps_font_in, (const char*)ps_string_in->data, ps_color_sdl
   );

   if( NULL == ps_type_render_out ) {
      goto gdt_cleanup;
   }

   /* Place the text on the screen. */
   ps_destreg.x = i_x_in;
   ps_destreg.y = i_y_in;
   ps_destreg.w = ps_type_render_out->w;
   ps_destreg.h = ps_type_render_out->h;
   graphics_draw_blit_tile( ps_type_render_out, NULL, &ps_destreg );
   #elif defined USEALLEGRO
   textout_ex(
      gps_screen_buffer,
      ps_font_in,
      ps_string_in->data,
      i_x_in,
      i_y_in,
      makecol( ps_color_in->r, ps_color_in->g, ps_color_in->b ),
      -1
   );
   #else
   #error "No text rendering mechanism defined for this platform!"
   #endif /* USESDL, USEALLEGRO */

gdt_cleanup:

   #ifdef USESDL
   /* Clean up. */
   SDL_FreeSurface( ps_type_render_out );
   #endif /* USESDL */

   return;
}

/* Purpose: Blit a tile from a surface to the screen. We should never be      *
 * blitting from surface to surface anyway.                                   */
/* Parameters: Source surface, source and destination regions.                */
void graphics_draw_blit_tile(
   GFX_SURFACE* ps_src_in,
   GFX_RECTANGLE* ps_srcreg_in,
   GFX_RECTANGLE* ps_destreg_in
) {
   if( NULL == ps_src_in ) {
      return;
   }

   #ifdef USESDL
   /* No problems, blit! */
   SDL_BlitSurface(
      ps_src_in, ps_srcreg_in, SDL_GetVideoSurface(), ps_destreg_in
   );
   #elif defined USEDIRECTX
   RECT s_dest = {
      ps_destreg_in->x,
      ps_destreg_in->y,
      ps_destreg_in->x + ps_src_in->w,
      ps_destreg_in->y + ps_src_in->h
   };
   gps_surface_back->Blt( &s_dest, ps_src_in->surface, NULL, DDBLT_WAIT, NULL );
   #elif defined USEALLEGRO
   masked_blit(
      ps_src_in,
      gps_screen_buffer,
      ps_srcreg_in->x,
      ps_srcreg_in->y,
      ps_destreg_in->x,
      ps_destreg_in->y,
      ps_srcreg_in->w,
      ps_srcreg_in->h
   );
   #else
   #error "No tile blitting mechanism defined for this platform!"
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */
}

/* Purpose: Blit a sprite from a surface to the screen. We should never be    *
 * blitting from surface to surface anyway.                                   */
/* Parameters: Source surface, source and destination regions.                */
void graphics_draw_blit_sprite(
   GFX_SURFACE* ps_src_in,
   GFX_RECTANGLE* ps_srcreg_in,
   GFX_RECTANGLE* ps_destreg_in
) {
   #ifdef USESDL
   graphics_draw_blit_tile( ps_src_in, ps_srcreg_in, ps_destreg_in );
   #elif defined USEDIRECTX
   graphics_draw_blit_tile( ps_src_in, ps_srcreg_in, ps_destreg_in );
   #elif defined USEALLEGRO
   graphics_draw_blit_tile( ps_src_in, ps_srcreg_in, ps_destreg_in );
   #else
   #error "No sprite blitting mechanism defined for this platform!"
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */
}

/* Purpose: Blank the screen.                                                 */
/* Parameters: The color to blank the screen.                                 */
void graphics_draw_blank( GFX_COLOR* ps_color_in ) {
   #ifdef USESDL
   GFX_SURFACE* ps_screen = SDL_GetVideoSurface();
   Uint32 i_color_temp;

   /* Define a rectangle encompassing the screen. */
   SDL_Rect s_screenrect;
   s_screenrect.x = 0;
   s_screenrect.y = 0;
   s_screenrect.w = ps_screen->w;
   s_screenrect.h = ps_screen->h;

   /* Define a color in a format te fill function will understand. */
   i_color_temp = SDL_MapRGB(
      ps_screen->format,
      ps_color_in->r,
      ps_color_in->g,
      ps_color_in->b
   );

   SDL_FillRect( ps_screen, &s_screenrect, i_color_temp );
   #elif defined USEDIRECTX
   DDBLTFX s_fx;
   memset( &s_fx, 0, sizeof( DDBLTFX ) );
   s_fx.dwSize = sizeof( DDBLTFX );
   s_fx.dwFillColor = PACK_16_BIT(
      ps_color_in->r,
      ps_color_in->g,
      ps_color_in->b
   );

   gps_surface_back->
      Blt( NULL, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT, &s_fx );
   #elif defined USEALLEGRO
   clear_to_color(
      gps_screen_buffer,
      makecol(
         ps_color_in->r,
         ps_color_in->g,
         ps_color_in->b
      )
   );
   #else
   #error "No blanking mechanism defined for this platform!"
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */
}

/* Purpose: Fade the screen in or out.                                        */
/* Parameters: Whether to fade in or out, the color from/to which to fade.    */
void graphics_draw_transition( int i_fade_io, GFX_COLOR* ps_color_in ) {
   if( NULL == ps_color_in ) {
      DBG_ERR( "Invalid color passed." );
      return;
   }

   #ifdef USESDL
   GFX_SURFACE* ps_screen = SDL_GetVideoSurface();
   GFX_SURFACE* ps_surface_screen_copy = NULL;
   GFX_SURFACE* ps_surface_fader = NULL;
   int i_alpha = 255;
   Uint32 i_color_temp = 0;

   /* Create a temporary copy of the screen to fade with. */
   ps_surface_screen_copy = SDL_CreateRGBSurface(
      ps_screen->flags | SDL_SRCALPHA,
      ps_screen->w,
      ps_screen->h,
      ps_screen->format->BitsPerPixel,
      ps_screen->format->Rmask,
      ps_screen->format->Gmask,
      ps_screen->format->Bmask,
      ps_screen->format->Amask
   );
   if( NULL == ps_surface_screen_copy ) {
      DBG_ERR( "Could not allocate screen copy." );
      return;
   }
   SDL_BlitSurface( ps_screen, NULL, ps_surface_screen_copy, NULL ) ;

   /* Create a surface of the requested color to fade with. */
   ps_surface_fader = SDL_CreateRGBSurface(
      ps_screen->flags|SDL_SRCALPHA,
      ps_screen->w,
      ps_screen->h,
      ps_screen->format->BitsPerPixel,
      ps_screen->format->Rmask,
      ps_screen->format->Gmask,
      ps_screen->format->Bmask,
      ps_screen->format->Amask
   );
   if( NULL == ps_surface_fader ) {
      DBG_ERR( "Could not allocate screen fader surface." );
      SDL_FreeSurface( ps_surface_screen_copy );
      return;
   }
   i_color_temp = SDL_MapRGB(
      ps_screen->format,
      ps_color_in->r,
      ps_color_in->g,
      ps_color_in->b
   );
   SDL_FillRect( ps_surface_fader, NULL, i_color_temp );

   /* Perform the actual fade. */
   while( 0 < i_alpha ) {
      if( GFX_TRANS_FADE_OUT == i_fade_io ) {
         SDL_SetAlpha( ps_surface_fader, SDL_SRCALPHA, (Uint8)(GFX_ALPHA_MAX - i_alpha) );
      } else {
         SDL_SetAlpha( ps_surface_fader, SDL_SRCALPHA, (Uint8)(i_alpha) );
      }

      SDL_BlitSurface( ps_surface_screen_copy, NULL, ps_screen, NULL );
      SDL_BlitSurface( ps_surface_fader, NULL, ps_screen, NULL );

      SDL_Flip( ps_screen );
      SDL_Delay( GFX_ALPHA_FADE_STEP );

      i_alpha -= GFX_ALPHA_FADE_INC;
   }

   /* Clean up. */
   SDL_FreeSurface( ps_surface_screen_copy );
   SDL_FreeSurface( ps_surface_fader );
   #elif defined USEALLEGRO
   if( GFX_TRANS_FADE_OUT == i_fade_io ) {
      fade_out( 32 );
   } else {
      fade_in( default_palette, 32 );
   }
   #else
   #error "No in-fading mechanism defined for this platform!"
   #endif /* USESDL, USEALLEGRO */
}

/* Purpose: Flip the screen surface buffer.                                   */
void graphics_do_update( void ) {
   #ifdef USESDL
   GFX_SURFACE* ps_screen = SDL_GetVideoSurface();
   SDL_Flip( ps_screen );
   #elif defined USEDIRECTX
   gps_surface_primary->Flip( gps_surface_back, DDFLIP_WAIT );
   #elif defined USEALLEGRO
   vsync();
   acquire_screen();
   blit( gps_screen_buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H );
   release_screen();
   #else
   #error "No surface flipping mechanism defined for this platform!"
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */
}

/* Purpose: Get the data for the tile with the given GID.                     */
/* Parameters: The index to lookup and the tileset in which to look it up.    */
/* Return: The address of the requested tile.                                 */
GFX_TILEDATA* graphics_get_tiledata( int i_gid_in, GFX_TILESET* ps_tileset_in ) {
   return &ps_tileset_in->tile_list[i_gid_in - 1];
}

/* Return: The height in pixels of the given font.                            */
int graphics_get_font_height( GFX_FONT* ps_font_in ) {
   int i_height_out;

   #ifdef USESDL
   i_height_out = TTF_FontHeight( ps_font_in );
   #elif defined USEALLEGRO
   /* XXX: Figure out a way to detect this automatically. */
   i_height_out = 20;
   #else
   #error "No font measuring mechanism defined for this platform!"
   #endif /* USESDL, USEALLEGRO */

   return i_height_out;
}

/* Purpose: Free the given surface buffer.                                    */
/* Parameters: The surface to free.                                           */
void graphics_free_image( GFX_SURFACE* ps_surface_in ) {
   if( NULL == ps_surface_in ) {
      return;
   }

   #ifdef USESDL
   SDL_FreeSurface( ps_surface_in );
   DBG_INFO_PTR( "Freed image", ps_surface_in );
   #elif defined USEDIRECTX
   free( ps_surface_in->surface );
   free( ps_surface_in );
   #elif defined USEALLEGRO
   destroy_bitmap( ps_surface_in );
   #else
   #error "No surface freeing mechanism defined for this platform!"
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */
}

/* Purpose: Free the given spritesheet buffer.                                */
/* Parameters: The spritesheet to free.                                       */
void graphics_free_spritesheet( GFX_SPRITESHEET* ps_spritesheet_in ) {
   #ifdef USESDL
   if( NULL != ps_spritesheet_in ) {
      SDL_FreeSurface( ps_spritesheet_in->image );
   }
   #elif defined USEDIRECTX
   graphics_free_image( ps_spritesheet_in->image );
   #elif defined USEALLEGRO
   graphics_free_image( ps_spritesheet_in->image );
   #else
   #error "No surface freeing mechanism defined for this platform!"
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */

   free( ps_spritesheet_in );
}

/* Purpose: Free the given tileset buffer.                                    */
/* Parameters: The tileset to free.                                           */
void graphics_free_tileset( GFX_TILESET* ps_tileset_in ) {
   #ifdef USESDL
   SDL_FreeSurface( ps_tileset_in->image );
   #elif defined USEDIRECTX
   graphics_free_image( ps_tileset_in->image );
   #elif defined USEALLEGRO
   graphics_free_image( ps_tileset_in->image );
   #else
   #error "No surface freeing mechanism defined for this platform!"
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */

   free( ps_tileset_in );
}

/* Purpose: Free the given font object.                                       */
/* Parameters: The font to free.                                              */
void graphics_free_font( GFX_FONT* ps_font_in ) {
   if( NULL == ps_font_in ) {
      return;
   }

   #ifdef USESDL
   TTF_CloseFont( ps_font_in );
   #elif defined USEALLEGRO
   destroy_font( ps_font_in );
   #else
   #error "No font freeing mechanism defined for this platform!"
   #endif /* USESDL */
}
