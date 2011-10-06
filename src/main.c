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

#include "roughxmpp.h"
#include "defines.h"
#include "cache.h"
#include "graphics.h"
#include "server.h"
#include "client.h"
#include "systype_title_client.h"
#include "systype_adventure_client.h"

#ifdef USEWII
#include <network.h>
#include "zeromaid_wii_data.h"
#ifdef USEDEBUG
#include <debug.h>
#endif
#endif /* USEWII */

DBG_MAIN
#ifndef USESERVER
GFX_DRAW_LOOP_DECLARE
#endif /* !USESERVER */

/* = Global Variables = */

bstring gps_title_error,
   gps_system_path;

pthread_mutex_t gps_system_path_mutex = PTHREAD_MUTEX_INITIALIZER;

/* = Type and Struct Definitions = */

/* typedef struct {
   bstring system_path;
} MAIN_PARAMS; */

/* = Function Prototypes = */

#ifndef USESERVER
void* main_client( void* );
#endif /* USESERVER */

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

   /* == End System Init == */

   main_system_ok:

   // XXX
   /* MAIN_PARAMS* ps_params_server = calloc(
      1, sizeof( MAIN_PARAMS )
   );
   ps_params_server->debug_file = gps_debug; */
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

   #ifndef USESERVER
   // Create the client thread.
   i_thread_error = pthread_create(
      &ps_thread_client,
      NULL,
      &client_main,
      NULL
   );
   if( i_thread_error ) {
      DBG_ERR(
         "Unable to create server thread, error code: %d",
         i_thread_error
      );
      goto main_cleanup;
   }
   #endif /* !USESERVER */
   // TODO: Change this to server once communication protocol is in place and
   //       server controls the game flow.
   pthread_join(
      ps_thread_client,
      NULL
   );

   #if 0
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



main_cleanup:

   //pthread_exit( NULL );

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

#ifndef USESERVER
#endif /* !USESERVER */
