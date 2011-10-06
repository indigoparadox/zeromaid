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

#ifndef DEFINES_H
#define DEFINES_H

/* = Includes = */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "bstring/bstraux.h"
#include "bstring/bstrlib.h"
#include "ezxml/ezxml.h"

#ifdef USEWII
#include <gctypes.h>
#endif /* USEWII */

#if defined( USEWII ) && defined( USEDEBUG ) && defined( USENET )
#include "net_print/net_print.h"
#endif /* USEWII && USEDEBUG && USENET */

/* = Definitions = */

/* TODO: Make network stuff user-definable at runtime. */
#define NET_PORT_LISTEN 2551

/* Platform-specific stuff. */
#define FILE_EXTENSION_IMAGE "bmp"
#define FILE_EXTENSION_AI "aib"

#ifdef USEALLEGRO
#define FILE_EXTENSION_FONT "bmp"
#else
#define FILE_EXTENSION_FONT "ttf"
#endif /* USEALLEGRO */

/* System loop types. */
#define SYSTEM_TYPE_TITLE 0
#define SYSTEM_TYPE_ADVENTURE "adventure"
#define SYSTEM_TYPE_TACTICS "tactics"
#define SYSTEM_TYPE_FISHING "fishing"
#define SYSTEM_TYPE_VISNOV "visnov"
#define SYSTEM_TYPE_PLATFORM "platform"

/* Some platforms don't need BOOL defined. */
#if ! defined USEWII && ! defined USEDIRECTX && ! defined BOOL
/* typedef unsigned int BOOL; */
/* We use a define here instead of a typedef because we're incompetent and    *
 * win32 is terrible.                                                         */
#define BOOL unsigned int
#endif /* USEWII, USEDIRECTX */

/* Some platforms don't have BOOL, but do have TRUE and FALSE. */
#if ! defined USEWII && ! defined USEDIRECTX && ! defined USEALLEGRO
#define FALSE 0
#define TRUE 1
#endif /* USEWII, USEDIRECTX, USEALLEGRO */

typedef unsigned char BYTE;

/* CONDs are used in mobiles and visual novel scenes right now. */
typedef int COND_SCOPE;
#define COND_SCOPE_LOCAL 0
#define COND_SCOPE_GLOBAL 0

/* The directory from which to fetch data files. */
#ifdef USEWII
#define PATH_SHARE "zeromaid_wii_data:/"
#else
#define PATH_SHARE "./"
#endif /* USEWII */

#define PATH_FILE_DEFAULT "default.txt"
#define PATH_FILE_SYSTEM "system.xml"
#define PATH_FILE_EVENTS "events.xml"
#define PATH_FILE_WINDOW_WIDE "system_window_wide." FILE_EXTENSION_IMAGE
#define PATH_FILE_WINDOW_RECT "system_window." FILE_EXTENSION_IMAGE

#define RETURN_ACTION_QUIT 0 /* Quit cleanly. */
#define RETURN_ACTION_TITLE 1 /* Display the title screen. */
#define RETURN_ACTION_LOADCACHE 2 /* Load the next instruction/data from the  *
                                   * cache.                                   */
#define TEAM_MAX_SIZE 10

#define ERROR_LEVEL_MALLOC 1 /* There was a problem allocating memory. */
#define ERROR_LEVEL_NOSYS 2 /* Unable to find system.xml. */
#define ERROR_LEVEL_WINDOW 3 /* Unable to setup default fonts. */
#define ERROR_LEVEL_INITFAIL 4 /* Unable to initialize abstraction. */

#define GEN_OPCODE_CLEAN -999999 /* For cleaning static tables. */

#define TITLE_ERROR_ENABLE extern bstring gps_title_error;

#define TRANS_FADE_IN 0
#define TRANS_FADE_OUT 1

/* Added indentation here to make it less confusing. */
#ifdef OUTTOFILE
   #ifdef USEWII
      #define DEBUG_OUT_PATH "sd:/zeromaid_out.txt"
   #else
      #define DEBUG_OUT_PATH "./dbg_out.txt"
   #endif /* USEWII */
   #define DEBUG_HANDLE_INFO gps_debug
   #define DEBUG_HANDLE_ERR gps_debug
   #define DBG_MAIN \
      FILE* gps_debug; \
      pthread_mutex_t gps_debug_mutex = PTHREAD_MUTEX_INITIALIZER;
   #define DBG_ENABLE \
      extern FILE* gps_debug; \
      extern pthread_mutex_t gps_debug_mutex; \
      pthread_t gps_debug_tid;
#else
   #if defined( USEWII ) && defined( USEDEBUG ) && defined( USENET )
      #define DEBUG_HANDLE_INFO stdout
      #define DEBUG_HANDLE_ERR stderr
      #define DBG_MAIN bstring gps_output_buffer;
      #define DBG_ENABLE extern bstring gps_output_buffer;
      #define DBG_CLOSE
      #define DBG_NET_PORT 5194
      #define DBG_NET_HOST "192.168.250.10"
   #else
      #define DEBUG_HANDLE_INFO stdout
      #define DEBUG_HANDLE_ERR stderr
      /* If output is to the console, we don't need to enable. */
      #define DBG_MAIN
      #define DBG_ENABLE
      #define DBG_CLOSE
   #endif /* USEWII && USEDEBUG && USENET */
#endif /* OUTTOFILE */

/* Some platforms have trouble when we try to clean up all of our strings for *
 * some reason. These currently seem to include the Wii and some Linux x86    *
 * installations.                                                             */
#ifdef USEIFFYBDESTROY
   #define iffy_bdestroy( target )
#else
   #define iffy_bdestroy( target ) bdestroy( target )
#endif

/* = Macros = */

/* The path separator can be a slash or a backslash, so deal with it. */
#define FILE_SHORT_B \
   ((strrchr(__FILE__, '\\') ? : __FILE__- 1) + 1)
#define FILE_SHORT \
   ((strrchr(FILE_SHORT_B, '/') ? : FILE_SHORT_B- 1) + 1)

/* The debug macros use normal C strings instead of bstrings since they'll    *
 * usually be printing literals and there are some tricky deallocation issues *
 * otherwise.                                                                 */
#if defined( USEWII ) && defined( USEDEBUG ) && defined( USENET )

/* Print an info message. */
#define DBG_INFO( msg ) \
   gps_output_buffer = bformat( "%s\n", msg ); \
   net_print_string( FILE_SHORT, __LINE__, (const char*)gps_output_buffer->data );

#else

#define DBG_INFO( ... ) \
   pthread_mutex_lock( &gps_debug_mutex ); \
   fprintf( DEBUG_HANDLE_INFO, "%s: %d: ", FILE_SHORT, __LINE__ ); \
   fprintf( DEBUG_HANDLE_INFO, "THREAD %p ", pthread_self().p ); \
   fprintf( DEBUG_HANDLE_INFO, "INFO: " __VA_ARGS__ ); \
   fprintf( DEBUG_HANDLE_INFO, "\n" ); \
   fflush( DEBUG_HANDLE_INFO ); \
   pthread_mutex_unlock( &gps_debug_mutex );

#define DBG_ERR( ... ) \
   pthread_mutex_lock( &gps_debug_mutex ); \
   fprintf( DEBUG_HANDLE_ERR, "%s: %d: ", FILE_SHORT, __LINE__ ); \
   fprintf( DEBUG_HANDLE_ERR, "THREAD %p ", pthread_self().p ); \
   fprintf( DEBUG_HANDLE_ERR, "ERROR: " __VA_ARGS__ ); \
   fprintf( DEBUG_HANDLE_ERR, "\n" ); \
   fflush( DEBUG_HANDLE_ERR ); \
   pthread_mutex_unlock( &gps_debug_mutex );

#endif /* USEWII && USEDEBUG && USENET */

/* Set a new error to display on the title screen. */
#define TITLE_ERROR_SET( message ) \
   if( NULL != gps_title_error ) { \
      bdestroy( gps_title_error ); \
      gps_title_error = NULL; \
   } \
   gps_title_error = bformat( "%s", message );

/* Clear any existing title screen errors. */
#define TITLE_ERROR_CLEAR() \
   if( NULL != gps_title_error ) { \
      bdestroy( gps_title_error ); \
      gps_title_error = NULL; \
   }

#ifdef USESDL
/* Convert a standard color to SDL color. */
#define CONV_COLOR_SDL( color_sdl, color_std ) \
   color_sdl.r = color_std->r; \
   color_sdl.g = color_std->g; \
   color_sdl.b = color_std->b;
#endif /* USESDL */

#endif /* DEFINES_H */
