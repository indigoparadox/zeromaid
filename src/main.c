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
#include "systype_visnov.h"

#ifdef USEWII
#include "zeromaid_wii_data.h"
#endif /* USEWII */

DBG_MAIN
GFX_DRAW_LOOP_DECLARE

/* = Global Variables = */

extern CACHE_CACHE* gps_cache;

#ifdef USEDIRECTX
LPDIRECTINPUT gs_lpdi;
LPDIRECTINPUTDEVICE gs_keyboard;
unsigned char gac_keystate[256];
#endif /* USEDIRECTX */

/* = Functions = */

int main( int argc, char* argv[] ) {
   int i_last_return, /* Contains the last loop-returned value. */
      i_error_level = 0; /* The program error level returned to the shell. */
   bstring ps_system_path;
   bstring ps_title;
   #ifdef USEDIRECTX
   HWND s_window;
   #endif /* USEDIRECTX */

   /* Initialize what we need to use functions to initialize. */
   ps_title = bformat( "%s", SYSTEM_TITLE );
   ps_system_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_SYSTEM );

   /* Setup the random number generator. */
   srand( time( NULL ) );

   /* Setup the loop timer. */
   GFX_DRAW_LOOP_INIT

   /* If we're on the Wii, start the dolfs ramdisk and the gamepad input. */
   #ifdef USEWII
   dolfsInit( &zeromaid_wii_data );
   WPAD_Init();
   #endif /* USEWII */

   /* Initialize DirectX input stuff. */
   #ifdef USEDIRECTX
   s_window = GetActiveWindow();
   if( FAILED( DirectInput8Create(
      GetModuleHandle( NULL ),
      DIRECTINPUT_VERSION,
      IID_IDirectInput8,
      (void**)&gs_lpdi,
      NULL
   ) ) ) {
      // error code
   }
   if( FAILED( gs_lpdi->CreateDevice( GUID_SysKeyboard, &gs_keyboard, NULL ) ) ) {
      /* error code */
   }
   if( FAILED( gs_keyboard->SetDataFormat( &c_dfDIKeyboard ) ) ) {
      /* error code */
   }
   if( FAILED(
      gs_keyboard->SetCooperativeLevel(
         s_window,
         DISCL_BACKGROUND | DISCL_NONEXCLUSIVE
      )
   ) ) {
      /* error code */
   }
   if( FAILED( gs_keyboard->Acquire() ) ) {
      /* error code */
   }
   #endif /* USEDIRECTX */

   #ifdef OUTTOFILE
   gps_debug = fopen( DEBUG_OUT_PATH, "a" );
   #endif /* OUTTOFILE */

   /* Platform-dependent miscellaneous initialization. */
   #ifdef USESDL
   TTF_Init();
   SDL_EnableKeyRepeat( 0, 0 );
   #endif /* USESDL */

   /* Set up the display. */
   DBG_INFO( "Setting up the screen..." );
   graphics_create_screen(
      GFX_SCREENWIDTH,
      GFX_SCREENHEIGHT,
      GFX_SCREENDEPTH,
      ps_title
   );

   /* Verify the integrity of the system data file. */
   if( !file_exists( ps_system_path ) ) {
      DBG_ERR( "Unable to find system.xml." );
      i_error_level = ERROR_LEVEL_NOSYS;
      goto main_cleanup;
   }

   /* The "cache" is an area in memory which holds all relevant data to the   *
    * current player/team.                                                    */
   gps_cache = (CACHE_CACHE*)calloc( 1, sizeof( CACHE_CACHE ) );
   if( NULL == gps_cache ) {
      DBG_ERR( "There was a problem allocating the system cache." );
      i_error_level = ERROR_LEVEL_MALLOC;
      goto main_cleanup;
   }

   /* Start the loop that loads the other gameplay loops. */
   i_last_return = systype_title_loop();
   while( RETURN_ACTION_QUIT != i_last_return ) {

      switch( i_last_return ) {
         case RETURN_ACTION_LOADCACHE:
            /* Execute the next instruction based on the system cache. */
            if( SYSTEM_TYPE_ADVENTURE == gps_cache->game_type ) {
               i_last_return = systype_adventure_loop( gps_cache->map_name );
            } else if( SYSTEM_TYPE_VISNOV == gps_cache->game_type ) {
               i_last_return = systype_visnov_loop( gps_cache->map_name );
            }
            break;

         default:
            i_last_return = systype_title_loop();
            break;
      }
   }

main_cleanup:

   GFX_DRAW_LOOP_FREE

   bdestroy( ps_system_path );
   bdestroy( ps_title );

   #ifdef OUTTOFILE
   fclose( gps_debug );
   #endif /* OUTTOFILE */

   #ifdef USESDL
   TTF_Quit();
   #endif /* USESDL */

   #ifdef USEDIRECTX
   if( gs_keyboard ) {
      gs_keyboard->Release();
   }

   if( gs_lpdi ) {
      gs_lpdi->Release();
   }
   #endif /* USEDIRECTX */

   return i_error_level;
}

#ifdef USEALLEGRO
END_OF_MAIN();
#endif // USEALLEGRO
