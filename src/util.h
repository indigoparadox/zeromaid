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

#ifndef UTIL_H
#define UTIL_H

/* = Includes = */

#include <stdio.h>

#include "bstring\bstrlib.h"
#include "defines.h"

#ifdef USEDIRECTX
#include <windows.h>
#endif /* USEDIRECTX */

/* = Macros = */

#define UTIL_ARRAY_ALLOC( type, array_ptr, array_count, cleanup ) \
   (array_count)++; \
   array_ptr = (type*)realloc( array_ptr, array_count * sizeof( type ) ); \
   if( NULL == array_ptr ) { \
      DBG_ERR( "Unable to allocate array: " #array_ptr ); \
      goto cleanup; \
   } \
   memset( &array_ptr[array_count - 1], 0, sizeof( type ) );

/* = Function Prototypes = */

int file_exists( bstring );
BOOL string_is_true( bstring );
int string_in_array( bstring, int, bstring*, int );

#endif /* UTIL_H */
