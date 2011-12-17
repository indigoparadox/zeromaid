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
#include <unistd.h>
//#include <pthread.h>
//#include <semaphore.h>

#include "roughirc.h"
#include "defines.h"
#include "cache.h"
#include "server.h"
#include "systype_adventure.h"

#ifdef USECLIENT
#include "client.h"
#include "systype_title.h"
#include "window.h"
#endif /* USECLIENT */

#ifdef USEWII
#include <network.h>
#include "zeromaid_wii_data.h"
#ifdef USEDEBUG
#include <debug.h>
#endif
#endif /* USEWII */

DBG_MAIN
#ifdef USECLIENT
GFX_DRAW_LOOP_DECLARE
#endif /* USECLIENT */

/* = Definitions = */

#define MAIN_TITLE_LOOP( __MSG_ERR_IN__ ) \
   TITLE_ERROR_SET( __MSG_ERR_IN__ ); \
   DBG_ERR( __MSG_ERR_IN__ ); \
   goto main_title_loop;

/* = Global Variables = */

bstring gps_title_error,
   gps_system_path;

pthread_mutex_t gps_system_path_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef USECLIENT
extern bstring gps_system_path;

#ifdef USEDIRECTX
LPDIRECTINPUT gs_lpdi;
LPDIRECTINPUTDEVICE gs_keyboard;
unsigned char gac_keystate[256];
#elif defined( USESDL )
SDL_Joystick* gps_joystick_current;
#endif /* USEDIRECTX, USESDL */
#endif /* USECLIENT */

/* = Functions = */

int main( int argc, char* argv[] ) {
   int i_thread_error,
      i_error_level = 0; /* The program error level returned to the shell. */
   #ifdef USEWII
   char ac_ipaddr_text[16],
      ac_gateway_text[16],
      ac_netmask_text[16];
   #else
   bstring ps_new_share_path; /* The directory containing shared files. */
   FILE* ps_file_default;
   #endif /* USEWII */
   pthread_t ps_thread_server,
      ps_thread_client;
   ezxml_t ps_xml_system;
   const char* pc_title;
   bstring ps_title;
   MAIN_PARAMS* ps_params_server = NULL;

   #ifdef USECLIENT
   /* Client-specific definitions. */
   int i_last_return, /* Contains the last loop-returned value. */
      /* TODO: Can we return this to the shell, or something? */
      i; /* Loop counter. */
   MAIN_PARAMS* ps_params_client = NULL;
   /* TODO: Only the server should have a cache. */
   #ifdef USEDIRECTX
   HWND s_window;
   #endif /* USEDIRECTX */
   #endif /* USECLIENT */

   /* == Pre-System Init == */
   /* This is stuff that should be taken care of before we start accessing    *
    * files and stuff.                                                        */

   #ifdef USEWII
   /* Push output down to where it can be seen on a TV screen. */
   printf( "\n\n\n\n" );
   #endif /* USEWII */

   /* Setup the random number generator. */
   srand( time( NULL ) );

   /* If we're on the Wii, start the dolfs ramdisk and the gamepad input. */
   #ifdef USEWII
   dolfsInit( &zeromaid_wii_data );
   #endif /* USEWII */

   #ifdef OUTTOFILE
   gps_debug = fopen( DEBUG_OUT_PATH, "a" );
   #endif /* OUTTOFILE */

   #ifdef USECLIENT
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

   /* Setup the loop timer. */
   GFX_DRAW_LOOP_INIT
   #endif /* USECLIENT */

   /* == End Pre-System Init == */

   /* == System Init == */
   /* This is stuff that pokes and prods at the game data files to start the  *
    * game.                                                                   */

   /* Do an initial check for a system file. Note that there's an inverse of  *
    * this test after the multimods init section. That's not in an else up    *
    * so that we can give the multimods system a chance to check for a        *
    * designated mod to load.                                                 */
   gps_system_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_SYSTEM );
   if( zm_file_exists( gps_system_path ) ) {
      /* We sound a system file right here, so skip looking for one. */
      goto main_system_ok;
   }

   #ifdef USEMULTIMODS
   /* The first system check didn't work, so open the default.txt file and    *
    * figure out the name of the game to load.                                */
   ps_file_default = fopen( PATH_FILE_DEFAULT, "r" );
   if( NULL == ps_file_default ) {
      DBG_ERR( "Unable to open default system indicator file." );
      goto main_cleanup;
   }

   /* Read the mod name and change into its directory. */
   /* TODO: Use the current directory if no default.txt is present. */
   ps_new_share_path = bformat( "./" );
   breada( ps_new_share_path, (bNread)fread, ps_file_default );
   btrimws( ps_new_share_path );
   fclose( ps_file_default );
   if( chdir( ps_new_share_path->data ) ) {
      DBG_ERR( "Unable to change to directory: %s", ps_new_share_path->data );
      goto main_cleanup;
   }
   DBG_INFO( "Directory changed: %s", ps_new_share_path->data );
   #endif /* USEMULTIMODS */

   /* If the system file doesn't exist in this directory, we're hosed. */
   if( !zm_file_exists( gps_system_path ) ) {
      DBG_ERR( "Unable to find system file: %s", gps_system_path->data );
      i_error_level = ERROR_LEVEL_NOSYS;
      goto main_cleanup;
   }

   /* Load the game title. */
   ps_xml_system = ezxml_parse_file( (const char*)gps_system_path->data );
   pc_title = ezxml_attr( ps_xml_system, "name" );
   ps_title = bformat( "%s", pc_title );
   DBG_INFO( "System selected: %s", pc_title );
   ezxml_free( ps_xml_system );

   #if 0
   /* ZZZ */
   /* XXX: Make this coexist with the new networking code below. */
   #if defined( USEWII ) && defined( USEDEBUG ) && defined( USENET )
   /* Setup the Wii network debugging infrastructure. */
	if( if_config( ac_ipaddr_text, ac_netmask_text, ac_gateway_text, TRUE ) ) {
	   /* Use printf here because debug output won't work without the network. */
	   printf( "Unable to setup network." );
	   sleep( 5 );
		goto main_cleanup;
	} else {
	   net_print_init( DBG_NET_HOST, DBG_NET_PORT );
      DBG_INFO( "Remote debugger connection established." );
      /* printf( "network configured, ip: %s, gw: %s, mask %s\n", ac_ipaddr_text, ac_gateway_text, ac_netmask_text );
      sleep( 5 ); */
   }

   //DEBUG_Init( 100, 5656 );
   #endif /* USEWII && USEDEBUG && USENET */
   #endif /* 0 */

   /* == End System Init == */

main_system_ok:

   /* Always startup the server. The client can direct it to load systems or  *
    * whatever.                                                               */
   i_thread_error = pthread_create(
      &ps_thread_server,
      NULL,
      &server_main,
      NULL
   );
   if( i_thread_error ) {
      DBG_ERR(
         "Unable to create server thread, error code: %d",
         i_thread_error
      );
      goto main_cleanup;
   }

   #ifdef USECLIENT
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
      goto main_cleanup;
   }

   /* Start the loop that loads the other gameplay loops. */
main_title_loop:
   i_last_return = systype_title_loop();
   while( RETURN_ACTION_QUIT != i_last_return ) {

      switch( i_last_return ) {
         case RETURN_ACTION_LOADCACHE:
            /* TODO: Start a server thread with a loaded memory image. */
            #if 0
            /* Execute the next instruction based on the system cache. */
            if(
               NULL != ps_cache->game_type &&
               0 == strcmp( SYSTEM_TYPE_ADVENTURE,
                  (const char*)ps_cache->game_type->data )
            ) {
               i_last_return = systype_adventure_loop( ps_cache );
            } else {
               i_last_return = systype_title_loop( ps_cache );
               TITLE_ERROR_SET( "Invalid game type specified" );
               DBG_ERR(
                  "Invalid game type specified: %s",
                  bdata( ps_cache->game_type )
               );
            }
            #endif /* 0 */
            break;

         case RETURN_ACTION_CLIENT_LOCAL:
            /* Direct the local server to start a new game. */
            ps_params_client = calloc(
               1, sizeof( MAIN_PARAMS )
            );
            ps_params_client->load = SERVER_LOAD_NEW;

            /* TODO: Signal the server to start a new story game, then create *
             *       a new client thread to listen for the type of client to  *
             *       create. Then create the client and reconnect.            */

            #if 0
            /* Connect the client to the local server. */
            i_thread_error = pthread_create(
               &ps_thread_client,
               NULL,
               &ps_params_client,
               NULL
            );
            if( i_thread_error ) {
               MAIN_TITLE_LOOP( "Unable to create local client." );
               goto main_title_loop;
            }

            /* Defer to the new client for now. */
            pthread_join(
               ps_thread_client,
               NULL
            );
            #endif

            break;

         default:
            i_last_return = systype_title_loop();
            break;
      }
   }
   #endif /* USECLIENT */

   /* TODO: Change this to server once communication protocol is in place and *
    *       server controls the game flow.                                    */
   #ifndef USECLIENT
   pthread_join(
      ps_thread_server,
      NULL
   );
   #endif /* !USECLIENT */

main_cleanup:

   #ifdef USECLIENT
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
   #endif /* USECLIENT */

   bdestroy( gps_system_path );

   #ifndef USEWII
   bdestroy( ps_new_share_path );
   #endif /* USEWII */

   #ifdef OUTTOFILE
   fclose( gps_debug );
   #endif /* OUTTOFILE */

   return i_error_level;
}

#ifdef USEALLEGRO
END_OF_MAIN();
#endif // USEALLEGRO
