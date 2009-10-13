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
   int i_width_in, int i_height_in, int i_depth_in, char* pc_title_in
) {
   GFX_SURFACE* ps_screen = NULL;

   #ifdef USESDL
   ps_screen = SDL_SetVideoMode(
      i_width_in, i_height_in, i_depth_in, SDL_HWSURFACE | SDL_DOUBLEBUF
   );

   if( NULL == ps_screen ) {
      char* pc_error = SDL_GetError();
      fprintf( stderr, "ERROR: Unable to setup screen: %s\n", pc_error );
      return NULL;
   }

   #ifdef __arm__
   SDL_WM_ToggleFullScreen( ps_screen );
   #endif /* __arm__ */

   SDL_WM_SetCaption( pc_title_in, pc_title_in );

   gs_viewport.w = i_width_in;
   gs_viewport.h = i_height_in;
   #else
   #error "No screen-getting mechanism defined for this platform!"
   #endif /* USESDL */

   return ps_screen;
}

/* Purpose: Create a surface with the specified image loaded onto it.         */
/* Parameters: File system path to the desired image.                         */
/* Return: A surface with the specified image loaded onto it.                 */
GFX_SURFACE* graphics_create_image( char* pc_path_in ) {
   GFX_SURFACE* ps_image = NULL;

   #ifdef USESDL
   Uint32 i_color_key = 0;
   GFX_SURFACE* ps_temp = SDL_LoadBMP( pc_path_in );

   if( NULL != ps_temp ) {
      ps_image = SDL_DisplayFormat( ps_temp );
      SDL_FreeSurface( ps_temp );

      /* Setup transparency. */
      i_color_key = SDL_MapRGB( ps_image->format, 0xFF, 0, 0xFF );
      SDL_SetColorKey( ps_image, SDL_RLEACCEL | SDL_SRCCOLORKEY, i_color_key );

      /* TODO: Work the filename of the loaded image into the debug message. */
      /* char* ac_dbg = "Successfully loaded image: " pc_path_in;
      DBG_OUT( ac_dbg ); */
   } else {
      /* char* ac_dbg = "Failed to load image: " pc_path_in;
      DBGERR( ac_dbg ); */
   }
   #else
   #error "No screen-getting mechanism defined for this platform!"
   #endif /* USESDL */

   return ps_image;
}

/* Purpose: Blit from surface to another.                                     */
/* Parameters: Source and destination surfaces, source and destination        *
 *             regions.                                                       */
void graphics_draw_blit(
   GFX_SURFACE* ps_src_in,
   GFX_SURFACE* ps_dest_in,
   GFX_RECTANGLE* ps_srcreg_in,
   GFX_RECTANGLE* ps_destreg_in
) {
   #ifdef USESDL
   if( NULL == ps_src_in ) {
      DBG_OUT( "Attempted to blit from NULL source!" );
      return;
   }

   if( NULL == ps_dest_in ) {
      DBG_OUT( "Attempted to blit to NULL destination!" );
      return;
   }

   /* No problems, blit! */
   SDL_BlitSurface( ps_src_in, ps_srcreg_in, ps_dest_in, ps_destreg_in );
   #else
   #error "No blitting mechanism defined for this platform!"
   #endif /* USESDL */
}

/* Purpose: Flip the given surface buffer.                                    */
/* Parameters: The surface to update.                                         */
void graphics_do_update( GFX_SURFACE* ps_surface_in ) {
   #ifdef USESDL
   SDL_Flip( ps_surface_in );
   #else
   #error "No surface flipping mechanism defined for this platform!"
   #endif /* USESDL */
}

/* Purpose: Free the given surface buffer.                                    */
/* Parameters: The surface to free.                                           */
void graphics_do_free( GFX_SURFACE* ps_surface_in ) {
   #ifdef USESDL
   SDL_FreeSurface( ps_surface_in );
   #else
   #error "No surface flipping mechanism defined for this platform!"
   #endif /* USESDL */
}
