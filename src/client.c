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

#include "client.h"

DBG_ENABLE
TITLE_ERROR_ENABLE

/* = Global Variables = */

extern bstring gps_system_path;

#ifdef USEDIRECTX
LPDIRECTINPUT gs_lpdi;
LPDIRECTINPUTDEVICE gs_keyboard;
unsigned char gac_keystate[256];
#elif defined( USESDL )
SDL_Joystick* gps_joystick_current;
#endif /* USEDIRECTX, USESDL */

/* = Functions = */

void* client_main( void* arg ) {
   int i_last_return, /* Contains the last loop-returned value. */
      /* TODO: Can we return this to the shell, or something? */
      i_error_level = 0, /* The program error level returned to the shell. */
      i; /* Loop counter. */
   bstring ps_title;
   /* TODO: Only the server should have a cache. */
   CACHE_CACHE* ps_cache = NULL;
   ezxml_t ps_xml_system;
   const char* pc_title;
   #ifdef USEDIRECTX
   HWND s_window;
   #endif /* USEDIRECTX */

   /* Setup the loop timer. */
   GFX_DRAW_LOOP_INIT

   /* Try to setup the screen and input systems. */
   #ifdef USESDL
   SDL_Init( SDL_INIT_JOYSTICK );
   /* TODO: Figure out the joystick number from a configuration file. */
   for( i = 0 ; i < 10 ; i++ ) {
      gps_joystick_current = SDL_JoystickOpen( i );
      if( NULL != gps_joystick_current ) {
         DBG_INFO( "Opened SDL joystick: %d", i );
         break;
      } else {
         DBG_ERR( "Failed to open SDL joystick: %d", i );
      }
   }
   TTF_Init();
   SDL_EnableKeyRepeat( 0, 0 );
   #elif defined USEDIRECTX
   /* Initialize DirectX input stuff. */
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
   #elif defined USEALLEGRO
   if( allegro_init() != 0 ) {
      DBG_ERR( "Unable to initialize Allegro." );
      i_error_level = ERROR_LEVEL_INITFAIL;
      goto main_cleanup;
   }
   install_keyboard();
   set_keyboard_rate( 0, 0 );
   #endif /* USESDL, USEDIRECTX, USEALLEGRO */

   /* Load the game title. */
   ps_xml_system = ezxml_parse_file( (const char*)gps_system_path->data );
   pc_title = ezxml_attr( ps_xml_system, "name" );
   ps_title = bformat( "%s", pc_title );
   DBG_INFO( "System selected: %s", pc_title );
   ezxml_free( ps_xml_system );

   /* Set up the display. */
   DBG_INFO( "Setting up the screen..." );
   graphics_create_screen(
      GEO_SCREENWIDTH,
      GEO_SCREENHEIGHT,
      GFX_SCREENDEPTH,
      ps_title
   );

   /* Load the subsystems which require initialization. */
   if( !window_init() ) {
      DBG_ERR( "Unable to initialize window system." );
      i_error_level = ERROR_LEVEL_WINDOW;
      goto main_client_cleanup;
   }

   /* The "cache" is an area in memory which holds all relevant data to the   *
    * current player/team.                                                    */
   ps_cache = (CACHE_CACHE*)calloc( 1, sizeof( CACHE_CACHE ) );
   if( NULL == ps_cache ) {
      DBG_ERR( "There was a problem allocating the system cache." );
      i_error_level = ERROR_LEVEL_MALLOC;
      goto main_client_cleanup;
   }

   /* Start the loop that loads the other gameplay loops. */
   i_last_return = systype_title_client_loop( ps_cache );
   while( RETURN_ACTION_QUIT != i_last_return ) {

      switch( i_last_return ) {
         case RETURN_ACTION_LOADCACHE:
            /* Execute the next instruction based on the system cache. */
            if(
               NULL != ps_cache->game_type &&
               0 == strcmp( SYSTEM_TYPE_ADVENTURE,
                  (const char*)ps_cache->game_type->data )
            ) {
               i_last_return = systype_adventure_client_loop( ps_cache );
            } else {
               i_last_return = systype_title_loop( ps_cache );
               TITLE_ERROR_SET( "Invalid game type specified" );
               DBG_ERR(
                  "Invalid game type specified: %s",
                  ps_cache->game_type->data
               );
            }
            break;

         default:
            i_last_return = systype_title_client_loop( ps_cache );
            break;
      }
   }

   main_client_cleanup:

   GFX_DRAW_LOOP_FREE

   event_get_assigned( EVENT_INPUT_TYPE_FREE, 0 );

   bdestroy( ps_title );

   window_cleanup();

   #ifdef USESDL
   SDL_JoystickClose( gps_joystick_current );
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
}
