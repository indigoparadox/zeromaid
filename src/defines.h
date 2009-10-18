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
   fprintf( stdout, "INFO: %s,%d: %s\n", FILE_SHORT, __LINE__, msg );

/* Print an info message involving a data file filename. */
#define DBG_INFO_FILE( msg, filename ) \
   fprintf( stdout, "INFO: %s,%d: %s: %s\n", FILE_SHORT, __LINE__, msg, filename );

/* Print an error message. */
#define DBG_ERR( msg ) \
   fprintf( stderr, "ERROR: %s,%d: %s\n", FILE_SHORT, __LINE__, msg );

/* Print an error message involving a data file filename. */
#define DBG_ERR_FILE( msg, filename ) \
   fprintf( stderr, "ERROR: %s,%d: %s: %s\n", FILE_SHORT, __LINE__, msg, filename );

#ifdef USESDL
/* Convert a standard color to SDL color. */
#define CONV_COLOR_SDL( color_sdl, color_std ) \
   color_sdl.r = color_std->r; \
   color_sdl.g = color_std->g; \
   color_sdl.b = color_std->b;
#endif /* USESDL */

/* = Definitions = */

#define SYSTEM_TITLE "ZeroMaid"

#define FALSE 0
#define TRUE 1

#define PATH_SHARE "."
#define PATH_MAPDATA "/mapdata"

#define SYSTYPE_TITLE 1

/* = Global Variables = */



#endif /* DEFINES_H */
