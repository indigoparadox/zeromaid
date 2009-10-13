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

/* = Macros = */

/* The path separator is different on Windows! */
#ifdef WIN32
#define FILE_SHORT ((strrchr(__FILE__, '\\') ? : __FILE__- 1) + 1)
#else
#define FILE_SHORT ((strrchr(__FILE__, '/') ? : __FILE__- 1) + 1)
#endif /* WIN32 */

#define DBG_OUT( msg ) \
   printf( "%s,%d: %s\n", FILE_SHORT, __LINE__, msg );

#define DBG_ERR( msg ) \
   fprintf( stderr, "ERROR: %s,%d: %s\n", FILE_SHORT, __LINE__, msg );

/* = Definitions = */

#define FALSE 0
#define TRUE 1

#define PATH_SHARE "../share"

#define SYSTYPE_TITLE 1

/* = Global Variables = */



#endif /* DEFINES_H */
