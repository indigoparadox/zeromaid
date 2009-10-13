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

#include <stdlib.h>

/* #ifdef __APPLE__
   #include <SDL/SDL.h>
   #include <SDL_mixer/SDL_mixer.h>
   #ifdef USEMUSIC
      #include <SDL_mixer/SDL_mixer.h>
   #endif // USEMUSIC
#elif defined __unix__
   #include <SDL/SDL.h>
   #ifdef USEMUSIC
      #include <SDL/SDL_mixer.h>
   #endif // USEMUSIC
#else
   #include <SDL.h>
   #ifdef USEMUSIC
      #include <SDL_mixer.h>
   #endif // USEMUSIC
#endif */

#include "defines.h"
#include "graphics.h"
#include "systype_title.h"

extern GFX_SURFACE* gps_screen;

/* = Functions = */

int main( int argc, char* argv[] ) {

   DBG_OUT( "Setting up the screen..." );
   gps_screen = graphics_create_screen(
      GFX_SCREENWIDTH,
      GFX_SCREENHEIGHT,
      GFX_SCREENDEPTH,
      "Maid Quest"
   );
   sysloop_title();


   return 0;
}

#ifdef USEALLEGRO
END_OF_MAIN();
#endif // USEALLEGRO
