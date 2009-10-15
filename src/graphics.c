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
      bstring ps_error_screen = cstr2bstr( "Unable to setup screen: " );
      bstring ps_error_sdl = cstr2bstr( SDL_GetError() );
      bconcat( ps_error_screen, ps_error_sdl );
      DBG_ERR( ps_error_screen->data );
      bdestroy( ps_error_screen );
      bdestroy( ps_error_sdl );
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

   /* Setup platform-agnostic screen accessories. */
   gs_viewport.w = i_width_in;
   gs_viewport.h = i_height_in;

   return ps_screen;
}

/* Purpose: Create a surface with the specified image loaded onto it.         */
/* Parameters: File system path to the desired image.                         */
/* Return: A surface with the specified image loaded onto it.                 */
GFX_SURFACE* graphics_create_image( bstring ps_path_in ) {
   GFX_SURFACE* ps_image = NULL;
   bstring ps_dbg_msg = NULL;
   int i_bol_dbg_ok = 1;

   #ifdef USESDL
   Uint32 i_color_key = 0;
   GFX_SURFACE* ps_temp = SDL_LoadBMP( ps_path_in->data );

   if( NULL != ps_temp ) {
      ps_image = SDL_DisplayFormat( ps_temp );
      SDL_FreeSurface( ps_temp );

      /* Setup transparency. */
      i_color_key = SDL_MapRGB( ps_image->format, 0xFF, 0, 0xFF );
      SDL_SetColorKey( ps_image, SDL_RLEACCEL | SDL_SRCCOLORKEY, i_color_key );

      ps_dbg_msg = cstr2bstr( "Successfully loaded image: " );
   } else {
      ps_dbg_msg = cstr2bstr( "Failed to load image: " );
      i_bol_dbg_ok = FALSE;
   }
   #elif defined USEWII
   ps_image = GRRLIB_LoadTexturePNG( (const u8*)ps_path_in->data );
   #else
   #error "No image loading mechanism defined for this platform!"
   #endif /* USESDL, USEWII */

   bconcat( ps_dbg_msg, ps_path_in );
   if( i_bol_dbg_ok ) {
      DBG_OUT( ps_dbg_msg->data );
   } else {
      DBG_ERR( ps_dbg_msg->data );
   }
   bdestroy( ps_dbg_msg );

   return ps_image;
}

/* Purpose: Create a tileset that can be used to populate a map.              */
/* Parameters: File system path to the desired image, individual tile width   *
 *             and height.                                                    */
/* Return: A tileset struct with the specified image and tile size.           */
GFX_TILESET* graphics_create_tileset( bstring ps_path_in, int i_tile_size_in ) {
   GFX_TILESET* ps_tileset_out = NULL;
   GFX_SURFACE* ps_surface = graphics_create_image( ps_path_in );
   if( NULL != ps_surface ) {
      /* The surface loaded all right, so construct the tileset. */
      ps_tileset_out = malloc( sizeof( GFX_TILESET ) );
      if( NULL == ps_tileset_out ) {
         DBG_ERR( "Error allocating memory for tileset!" );
         return NULL;
      }
      ps_tileset_out->image = ps_surface;
      ps_tileset_out->tile_size = i_tile_size_in;

      /* Do any platform-specific stuff. */
      #ifdef USESDL
      /* Nothing to do! */
      #elif defined USEWII
      GRRLIB_InitTileSet(
         ps_tileset_out->image,
         i_tile_size_in,
         i_tile_size_in,
         0
      );
      #else
      #error "No tile loading mechanism defined for this platform!"
      #endif
   }

   return ps_tileset_out;
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
   #ifdef USESDL
   GFX_SURFACE* ps_type_render_out = NULL;
   TTF_Font* ps_font = NULL;
   SDL_Color ps_color_sdl;
   SDL_Rect ps_destreg;

   /* Handle format conversions. */
   CONV_COLOR_SDL( ps_color_sdl, ps_color_in );

   /* Open the font and render the text. */
   ps_font = TTF_OpenFont( ps_font_name_in->data, i_size_in );
   if( NULL == ps_font ) {
      bstring ps_error = cstr2bstr( "Unable to load font: " );
      bconcat( ps_error, ps_font_name_in );
      DBG_ERR( ps_error->data );
      bdestroy( ps_error );
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
   #elif defined USEWII
   /* XXX: Color, scale, src region. */
   GRRLIB_DrawImg(
      ps_destreg_in->x,
      ps_destreg_in->y,
      ps_src_in,
      0,
      100,
      100,
      0xffffffff
   );

   #else
   #error "No blitting mechanism defined for this platform!"
   #endif /* USESDL, USEWII */
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
   #elif defined USEWII
   /* XXX: Color, scale, src region. */
   GRRLIB_DrawImg(
      ps_destreg_in->x,
      ps_destreg_in->y,
      ps_src_in,
      0,
      100,
      100,
      0xffffffff
   );
   #else
   #error "No blitting mechanism defined for this platform!"
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
