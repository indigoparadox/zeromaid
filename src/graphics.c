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

/* = Global Variables = */

int gi_animation_frame = GFX_ANIMATION_MAX_HEARTBEAT; /* The current anime    *
                                                       * frame.               */

/* = Functions = */

/* Purpose: Create a surface that, when drawn to, draws to the screen.        */
/* Parameters: Screen width, screen height, screen color depth, screen window *
 *             title.                                                         */
/* Return: A pointer to the screen surface.                                   */
GFX_SURFACE* graphics_create_screen(
   int i_width_in, int i_height_in, int i_depth_in, bstring ps_title_in
) {
   GFX_SURFACE* ps_screen = NULL;

   #ifdef USESDL
   ps_screen = SDL_SetVideoMode(
      i_width_in, i_height_in, i_depth_in, SDL_HWSURFACE | SDL_DOUBLEBUF
   );

   if( NULL == ps_screen ) {
      DBG_ERR_STR( "Unable to setup screen", SDL_GetError() );
      return NULL;
   }

   #ifdef __arm__
   SDL_WM_ToggleFullScreen( ps_screen );
   #endif /* __arm__ */

   SDL_WM_SetCaption( ps_title_in->data, ps_title_in->data );

   #elif defined USEWII
   /* Nintendo Wii doesn't have a screen object. It does have some things to  *
    * take care of, though.                                                   */
   GRRLIB_Init();
   #else
   #error "No screen-getting mechanism defined for this platform!"
   #endif /* USESDL, USEWII */

   return ps_screen;
}

/* Purpose: Create a surface with the specified image loaded onto it.         */
/* Parameters: File system path to the desired image.                         */
/* Return: A surface with the specified image loaded onto it.                 */
GFX_SURFACE* graphics_create_image( bstring ps_path_in ) {
   GFX_SURFACE* ps_image = NULL;

   #ifdef USESDL
   Uint32 i_color_key = 0;
   GFX_SURFACE* ps_temp = SDL_LoadBMP( ps_path_in->data );

   if( NULL != ps_temp ) {
      ps_image = SDL_DisplayFormat( ps_temp );
      SDL_FreeSurface( ps_temp );

      /* Setup transparency. */
      i_color_key = SDL_MapRGB( ps_image->format, 0xFF, 0, 0xFF );
      SDL_SetColorKey( ps_image, SDL_RLEACCEL | SDL_SRCCOLORKEY, i_color_key );

      DBG_INFO_STR( "Successfully loaded image", ps_path_in->data );
   } else {
      DBG_ERR_STR( "Failed to load image", ps_path_in->data );
   }
   #elif defined USEWII
   ps_image = GRRLIB_LoadTexturePNG( (const u8*)ps_path_in->data );
   #else
   #error "No image loading mechanism defined for this platform!"
   #endif /* USESDL, USEWII */

    return ps_image;
}

/* Purpose: Create a spritesheet that can be used to populate a mobile or     *
 *          foreground object.                                                */
/* Parameters: File system path to the spritesheet data file.                 */
/* Return: A spritesheet struct with the image and data prescribed by the     *
 *         data file.                                                         */
GFX_SPRITESHEET* graphics_create_spritesheet( bstring ps_path_in ) {
   // XXX
   return NULL;
}

/* Purpose: Create a tileset that can be used to populate a map.              */
/* Parameters: File system path to the tileset data file.                     */
/* Return: A tileset struct with the image and data prescribed by the data    *
 *         file.                                                              */
GFX_TILESET* graphics_create_tileset( bstring ps_path_in ) {
   GFX_TILESET* ps_tileset_out = NULL;
   GFX_SURFACE* ps_surface = NULL;
   GFX_TILEDATA* ps_tile_iter = NULL;
   ezxml_t ps_xml_tileset, ps_xml_image, ps_xml_tile, ps_xml_props,
      ps_xml_prop_iter;
   bstring ps_image_filename = NULL, ps_image_path = NULL;

   DBG_INFO_STR( "Loading tile data", ps_path_in->data );

   /* Verify the XML file exists and open or abort accordingly. */
   if( !file_exists( ps_path_in ) ) {
      DBG_ERR_STR( "Unable to load tile data", ps_path_in->data );
      return NULL;
   }
   ps_xml_tileset = ezxml_parse_file( ps_path_in->data );

   /* Load the image file. */
   ps_xml_image = ezxml_child( ps_xml_tileset, "image" );
   ps_image_path = bfromcstr( PATH_SHARE PATH_MAPDATA "/" );
   ps_image_filename = bfromcstr( ezxml_attr( ps_xml_image, "source" ) );
   bconcat( ps_image_path, ps_image_filename );
   ps_surface = graphics_create_image( ps_image_path );

   /* If the surface loaded all right, then construct the tileset. */
   if( NULL != ps_surface ) {
      /* Create the tileset struct. */
      ps_tileset_out = malloc( sizeof( GFX_TILESET ) );
      if( NULL == ps_tileset_out ) {
         DBG_ERR( "There was a problem allocating tileset memory." );
         graphics_free_image( ps_surface );
         goto gct_cleanup;
      }

      /* Load the properties of each tile into a linked list. */
      ps_xml_tile = ezxml_child( ps_xml_tileset, "tile" );
      while( NULL != ps_xml_tile ) {
         /* Allocate a fresh new tile! */
         if( NULL == ps_tile_iter ) {
            ps_tile_iter = malloc( sizeof( GFX_TILEDATA ) );

            /* Link the first tile to the tileset struct. */
            ps_tileset_out->tile_list = ps_tile_iter;
         } else {
            ps_tile_iter->next = malloc( sizeof( GFX_TILEDATA ) );

            /* Move the cursor to the new tile. */
            ps_tile_iter = ps_tile_iter->next;
         }

         /* A second null check to verify memory allocaton. */
         if( NULL == ps_tile_iter ) {
            /* There was a memory allocation problem! */
            DBG_ERR( "There was a problem allocating tile memory." );
            break;
         }
         memset( ps_tile_iter, 0, sizeof( GFX_TILEDATA ) );

         /* Cycle through all present properties and add each one to the      *
          * final struct.                                                     */
         ps_xml_props = ezxml_child( ps_xml_tile, "properties" );
         ps_xml_prop_iter = ezxml_child( ps_xml_props, "property" );
         while( NULL != ps_xml_prop_iter ) {
            /* Load the current property into the struct. */
            if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "hindrance" ) ) {
               ps_tile_iter->hindrance = atoi( ezxml_attr( ps_xml_prop_iter, "value" ) );
            } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "animated" ) ) {
               ps_tile_iter->animated = atoi( ezxml_attr( ps_xml_prop_iter, "value" ) );
            }

            /* Clean up and go to the next one! */
            ps_xml_prop_iter = ps_xml_prop_iter->next;
         }

         /* Go to the next one! */
         ps_xml_tile = ps_xml_tile->next;
      }

      /* Place the image into the tileset struct. */
      ps_tileset_out->image = ps_surface;

      /* Figure out the file size. */
      ps_tileset_out->pixel_size = atoi( ezxml_attr( ps_xml_tileset, "tileheight" ) );
      DBG_INFO_NUM( "Found tile size", ps_tileset_out->pixel_size );

      /* Do any platform-specific stuff. */
      #ifdef USEWII
      GRRLIB_InitTileSet(
         ps_tileset_out->image,
         ps_tileset_out->tile_size,
         ps_tileset_out->tile_size,
         0
      );
      #endif

      DBG_INFO_STR( "Successfully loaded tile data", ps_path_in->data );
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

   #ifdef USESDL
   ps_color_out = malloc( sizeof( GFX_COLOR ) );
   ps_color_out->r = i_red_in;
   ps_color_out->g = i_green_in;
   ps_color_out->b = i_blue_in;
   #elif defined USEWII
   /* Wii colors are an unsigned 32-bit integer in the format RGBA. */
   /* TODO: Use bit shifting for efficiency. */
   ps_color_out = malloc( sizeof( GFX_COLOR ) );
   *ps_color_out =
      (i_red_in * 16777216) +
      (i_green_in * 65536) +
      (i_blue_in * 256);
   #else
   #error "No color creation mechanism defined for this platform!"
   #endif /* USESDL, USEWII */

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

/* Purpose: Render a given text string to the screen.                         */
/* Parameters: Coordinates to render to, string to render, font name, point   *
 * size, render color.                                                        */
void graphics_draw_text(
   int i_x_in,
   int i_y_in,
   bstring ps_string_in,
   bstring ps_font_name_in,
   int i_size_in,
   GFX_COLOR* ps_color_in
) {
   if( NULL == ps_string_in ) {
      DBG_ERR( "Attempted to blit NULL string!" );
      return;
   }
   if( NULL == ps_font_name_in ) {
      DBG_ERR( "Attempted to blit string with NULL font!" );
      return;
   }
   if( NULL == ps_color_in ) {
      DBG_ERR( "Attempted to blit string with NULL color!" );
      return;
   }

   #ifdef USESDL
   GFX_SURFACE* ps_type_render_out = NULL;
   TTF_Font* ps_font = NULL;
   SDL_Color ps_color_sdl;
   SDL_Rect ps_destreg;
   static bstring ps_last_font = NULL;

   /* Handle format conversions. */
   CONV_COLOR_SDL( ps_color_sdl, ps_color_in );

   /* Open the font and render the text. */
   ps_font = TTF_OpenFont( ps_font_name_in->data, i_size_in );
   if( NULL == ps_font ) {
      /* Only log the error once. */
      if( 0 != bstrcmp( ps_last_font, ps_font_name_in ) ) {
         DBG_ERR_STR( "Unable to load font", ps_font_name_in->data );
         DBG_ERR_STR( "SDL TTF Error was", TTF_GetError() );
         bdestroy( ps_last_font );
         ps_last_font = bstrcpy( ps_font_name_in );
      }
      return;
   }
   ps_type_render_out = TTF_RenderText_Solid(
      ps_font, ps_string_in->data, ps_color_sdl
   );

   /* Place the text on the screen. */
   ps_destreg.x = i_x_in;
   ps_destreg.y = i_y_in;
   ps_destreg.w = ps_type_render_out->w;
   ps_destreg.h = ps_type_render_out->h;
   graphics_draw_blit_tile( ps_type_render_out, NULL, &ps_destreg );

   /* Clean up. */
   TTF_CloseFont( ps_font );
   SDL_FreeSurface( ps_type_render_out );
   #elif defined USEWII

   #else
   #error "No text rendering mechanism defined for this platform!"
   #endif /* USESDL, USEWII */
}

/* Purpose: Blit a tile from a surface to the screen. We should never be      *
 * blitting from surface to surface anyway.                                   */
/* Parameters: Source surface, source and destination regions.                */
void graphics_draw_blit_tile(
   GFX_SURFACE* ps_src_in,
   GFX_RECTANGLE* ps_srcreg_in,
   GFX_RECTANGLE* ps_destreg_in
) {
   #ifdef USESDL
   if( NULL == ps_src_in ) {
      DBG_ERR( "Attempted to blit from NULL source!" );
      return;
   }

   /* No problems, blit! */
   SDL_BlitSurface( ps_src_in, ps_srcreg_in, SDL_GetVideoSurface(), ps_destreg_in );
   #else
   #error "No tile blitting mechanism defined for this platform!"
   #endif /* USESDL */
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
   #else
   #error "No sprite blitting mechanism defined for this platform!"
   #endif /* USESDL, USEWII */
}

/* Purpose: Blank the screen.                                                 */
/* Parameters: The color to blank the screen.                                 */
void graphics_draw_blank( GFX_COLOR* ps_color_in ) {
   #ifdef USESDL
   GFX_SURFACE* ps_screen = SDL_GetVideoSurface();

   /* Define a rectangle encompassing the screen. */
   SDL_Rect s_screenrect;
   s_screenrect.x = 0;
   s_screenrect.y = 0;
   s_screenrect.w = ps_screen->w;
   s_screenrect.h = ps_screen->h;

   /* Define a color in a format te fill function will understand. */
   Uint32 i_color_temp = SDL_MapRGB(
      ps_screen->format,
      ps_color_in->r,
      ps_color_in->g,
      ps_color_in->b
   );

   SDL_FillRect( ps_screen, &s_screenrect, i_color_temp );
   #elif defined USEWII
   GRRLIB_FillScreen( ps_color_in );
   #else
   #error "No blanking mechanism defined for this platform!"
   #endif /* USESDL, USEWII */
}

/* Purpose: Fade the screen in or out.                                        */
/* Parameters: Whether to fade in or out, the color from/to which to fade.    */
void graphics_draw_transition( int i_fade_io, GFX_COLOR* ps_color_in ) {
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
   #else
   #error "No in-fading mechanism defined for this platform!"
   #endif /* USESDL, USEWII */
}

/* Purpose: Flip the screen surface buffer.                                   */
void graphics_do_update( void ) {
   #ifdef USESDL
   GFX_SURFACE* ps_screen = SDL_GetVideoSurface();
   SDL_Flip( ps_screen );
   #elif defined USEWII
   GRRLIB_Render();
   #else
   #error "No surface flipping mechanism defined for this platform!"
   #endif /* USESDL, USEWII */
}

/* Purpose: Increment the animation frame counter.                            */
void graphics_do_heartbeat( void ) {
   /* Draws left till frame changes. */
   static short int i_animation_draws_left = GFX_ANIMATION_MAX_DRAWS;

   /* Increment the animation frame. */
   if( i_animation_draws_left <= 0 ) {
      i_animation_draws_left = GFX_ANIMATION_MAX_DRAWS;
      if( GFX_ANIMATION_MAX_HEARTBEAT <= gi_animation_frame ) {
         gi_animation_frame = 0;
      } else {
         gi_animation_frame++;
      }
   } else {
      i_animation_draws_left--;
   }
}

/* Purpose: Get the data for the tile with the given GID.                     */
/* Parameters: The index to lookup and the tileset in which to look it up.    */
/* Return: The address of the requested tile.                                 */
GFX_TILEDATA* graphics_get_tiledata( int i_index_in, GFX_TILESET* ps_tileset_in ) {
   GFX_TILEDATA* ps_tiledata_out = NULL;

   ps_tiledata_out = ps_tileset_in->tile_list;
   while( NULL != ps_tiledata_out ) {
      if( i_index_in == ps_tiledata_out->gid ) {
         /* Found it! */
         return ps_tiledata_out;
      }

      /* Go to the next one! */
      ps_tiledata_out = ps_tiledata_out->next;
   }

   /* Not found! */
   return NULL;
}

/* Purpose: Free the given surface buffer.                                    */
/* Parameters: The surface to free.                                           */
void graphics_free_image( GFX_SURFACE* ps_surface_in ) {
   #ifdef USESDL
   SDL_FreeSurface( ps_surface_in );
   #elif defined USEWII
   GRRLIB_FlushTex( ps_surface_in );
   #else
   #error "No surface freeing mechanism defined for this platform!"
   #endif /* USESDL, USEWII */
}

/* Purpose: Free the given tileset buffer.                                    */
/* Parameters: The tileset to free.                                           */
void graphics_free_tileset( GFX_TILESET* ps_tileset_in ) {
   #ifdef USESDL
   SDL_FreeSurface( ps_tileset_in->image );
   #elif defined USEWII
   GRRLIB_FlushTex( ps_tileset_in->image );
   #else
   #error "No surface freeing mechanism defined for this platform!"
   #endif /* USESDL, USEWII */

   free( ps_tileset_in );
}
