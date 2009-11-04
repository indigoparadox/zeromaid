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
   #include <SDL/SDL_ttf.h>
   #ifdef USEMUSIC
      #include <SDL_mixer/SDL_mixer.h>
   #endif // USEMUSIC
#elif defined __unix__
   #include <SDL/SDL.h>
   #include <SDL/SDL_ttf.h>
   #ifdef USEMUSIC
      #include <SDL/SDL_mixer.h>
   #endif // USEMUSIC
#else
   #include <SDL.h>
   #include <SDL_ttf.h>
   #ifdef USEMUSIC
      #include <SDL_mixer.h>
   #endif // USEMUSIC
#endif */

#include "defines.h"
#include "cache.h"
#include "graphics.h"
#include "systype_title.h"
#include "systype_adventure.h"

/* = Global Variables = */

extern SYS_CACHE* gps_cache;

/* = Functions = */

int main( int argc, char* argv[] ) {
   short unsigned int i_bol_running = TRUE;
   int i_last_return; /* Contains the last loop-returned value. */

   #ifdef OUTTOFILE
   gps_debug = fopen( DEBUG_OUT_PATH, "a" );
   #endif /* OUTTOFILE */

   /* Platform-dependent miscellaneous initialization. */
   #ifdef USESDL
   TTF_Init();
   #elif defined USEWII
   WPAD_Init();
   #endif /* USESDL, USEWII */

   /* Set up the display. */
   DBG_INFO( "Setting up the screen..." );
   bstring ps_title = bfromcstr( SYSTEM_TITLE );
   graphics_create_screen(
      GFX_SCREENWIDTH,
      GFX_SCREENHEIGHT,
      GFX_SCREENDEPTH,
      ps_title
   );
   bdestroy( ps_title );

   /* The "cache" is an area in memory which holds all relevant data to the   *
    * current player/team.                                                    */
   gps_cache = malloc( sizeof( SYS_CACHE ) );

   while( i_bol_running ) {
      i_last_return = systype_title_loop();
      switch( i_last_return ) {
         case RETURN_ACTION_LOADCACHE:
            /* TODO: Execute the next instruction based on the cache file. */
            systype_adventure_loop();
            break;

         case RETURN_ACTION_QUIT:
            /* Quit! */
            i_bol_running = 0;
            break;
      }
   }

   #ifdef OUTTOFILE
   fclose( gps_debug );
   #endif /* OUTTOFILE */

   #ifdef USESDL
   TTF_Quit();
   #elif defined USEWII
   GRRLIB_Exit();
   #endif /* USESDL, USEWII */

   return 0;
}

#ifdef USEALLEGRO
END_OF_MAIN();
#endif // USEALLEGRO
