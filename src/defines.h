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
#include "bstring/bstrlib.h"
#include "ezxml/ezxml.h"

/* = Definitions = */

/* Platform-specific stuff. */
#define FILE_EXTENSION_IMAGE "bmp"
#define FILE_EXTENSION_FONT "ttf"

#define SYSTEM_TITLE "ZeroMaid"

/* System loop types. */
#define SYSTEM_TYPE_TITLE 0
#define SYSTEM_TYPE_ADVENTURE 1
#define SYSTEM_TYPE_TACTICS 2
#define SYSTEM_TYPE_FISHING 3
#define SYSTEM_TYPE_VISNOV 4

#define FALSE 0
#define TRUE 1

/* The directory from which to fetch data files. */
#ifdef USEWII
#define PATH_SHARE "ramdisk:/"
#else
#define PATH_SHARE "./"
#endif /* USEWII */

#define PATH_FILE_SYSTEM "system.xml"

#define RETURN_ACTION_QUIT 0 /* Quit cleanly. */
#define RETURN_ACTION_TITLE 1 /* Display the title screen. */
#define RETURN_ACTION_LOADCACHE 2 /* Load the next instruction/data from the  *
                                   * cache.                                   */
#define TEAM_MAX_SIZE 10

#define SYSTYPE_TITLE 1

#define ERROR_LEVEL_MALLOC 1 /* There was a problem allocating memory! */

#ifdef OUTTOFILE

#define DEBUG_OUT_PATH "./dbg_out.txt"
#define DEBUG_HANDLE_INFO gps_debug
#define DEBUG_HANDLE_ERR gps_debug

#define DBG_MAIN FILE* gps_debug;
#define DBG_ENABLE extern FILE* gps_debug;

#else

#define DEBUG_HANDLE_INFO stdout
#define DEBUG_HANDLE_ERR stderr

/* If output is to the console, we don't need to enable. */
#define DBG_MAIN
#define DBG_ENABLE

#define DBG_CLOSE

#endif /* OUTTOFILE */

/* = Macros = */

/* The path separator is different on Windows! */
#ifdef VISUALSTUDIO
#define FILE_SHORT __FILE__
#else
#ifdef WIN32
#define FILE_SHORT ((strrchr(__FILE__, '\\') ? : __FILE__- 1) + 1)
#else
#define FILE_SHORT ((strrchr(__FILE__, '/') ? : __FILE__- 1) + 1)
#endif /* WIN32 */
#endif /* VISUALSTUDIO */

/* The debug macros use normal C strings instead of bstrings since they'll    *
 * usually be printing literals and there are some tricky deallocation issues *
 * otherwise.                                                                 */

/* Print an info message. */
#define DBG_INFO( msg ) \
   fprintf( DEBUG_HANDLE_INFO, "INFO: %s,%d: %s\n", FILE_SHORT, __LINE__, msg ); \
   fflush( DEBUG_HANDLE_INFO );

/* Print an info message involving a string. */
#define DBG_INFO_STR( msg, string ) \
   fprintf( DEBUG_HANDLE_INFO, "INFO: %s,%d: %s: %s\n", FILE_SHORT, __LINE__, msg, string ); \
   fflush( DEBUG_HANDLE_INFO );

/* Print an info message involving a number. */
#define DBG_INFO_NUM( msg, number ) \
   fprintf( DEBUG_HANDLE_INFO, "INFO: %s,%d: %s: %d\n", FILE_SHORT, __LINE__, msg, number ); \
   fflush( DEBUG_HANDLE_INFO );

/* Print an error message. */
#define DBG_ERR( msg ) \
   fprintf( DEBUG_HANDLE_ERR, "ERROR: %s,%d: %s\n", FILE_SHORT, __LINE__, msg ); \
   fflush( DEBUG_HANDLE_ERR );

/* Print an error message involving a string. */
#define DBG_ERR_STR( msg, string ) \
   fprintf( DEBUG_HANDLE_ERR, "ERROR: %s,%d: %s: %s\n", FILE_SHORT, __LINE__, msg, string ); \
   fflush( DEBUG_HANDLE_ERR );

#ifdef USESDL
/* Convert a standard color to SDL color. */
#define CONV_COLOR_SDL( color_sdl, color_std ) \
   color_sdl.r = color_std->r; \
   color_sdl.g = color_std->g; \
   color_sdl.b = color_std->b;
#endif /* USESDL */

#endif /* DEFINES_H */
