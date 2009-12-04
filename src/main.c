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

#ifdef USESDL
#ifdef __APPLE__
#include <SDL/SDL.h>
#elif defined __unix__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif /* __APPLE__, __unix__ */
#endif /* USESDL */

#include "defines.h"
#include "cache.h"
#include "graphics.h"
#include "systype_title.h"
#include "systype_adventure.h"

#ifdef USEWII
/* Include the Wii data files. */
#include "zeromaid_wii_data.h"
#endif /* USEWII */

DBG_MAIN

/* = Global Variables = */

extern CACHE_CACHE* gps_cache;

/* = Functions = */

int main( int argc, char* argv[] ) {
   // short unsigned int i_bol_running = TRUE;
   int i_last_return, /* Contains the last loop-returned value. */
      i_error_level = 0; /* The program error level returned to the shell. */

   /* If we're on the Wii, start the dolfs ramdisk. */
   #ifdef USEWII
   dolfsInit( &zeromaid_wii_data );
   #endif /* USEWII */

   #ifdef OUTTOFILE
   gps_debug = fopen( DEBUG_OUT_PATH, "a" );
   #endif /* OUTTOFILE */

   /* Platform-dependent miscellaneous initialization. */
   #ifdef USESDL
   TTF_Init();
   #endif /* USESDL */

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
   gps_cache = malloc( sizeof( CACHE_CACHE ) );
   if( NULL == gps_cache ) {
      DBG_ERR( "There was a problem allocating the system cache." );
      i_error_level = ERROR_LEVEL_MALLOC;
      goto main_cleanup;
   }
   memset( gps_cache, 0, sizeof( CACHE_CACHE ) );

   /* Start the loop that loads the other gameplay loops. */
   i_last_return = systype_title_loop();
   while( RETURN_ACTION_QUIT != i_last_return ) {

      switch( i_last_return ) {
         case RETURN_ACTION_LOADCACHE:
            /* Execute the next instruction based on the system cache. */
            if( SYSTEM_TYPE_ADVENTURE == gps_cache->game_type ) {
               i_last_return = systype_adventure_loop();
            } else if( SYSTEM_TYPE_VISNOV == gps_cache->game_type ) {
               /* TODO: Visual Novel */
            }
            break;

         default:
            i_last_return = systype_title_loop();
            break;
      }
   }

main_cleanup:

   #ifdef OUTTOFILE
   fclose( gps_debug );
   #endif /* OUTTOFILE */

   #ifdef USESDL
   TTF_Quit();
   #endif /* USESDL */

   return 0;
}

#ifdef USEALLEGRO
END_OF_MAIN();
#endif // USEALLEGRO
