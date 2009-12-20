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

/* = Notes = */

/* The window list in the system cache starts with the oldest window at index *
 * 0. Newer windows have higher indexes. This is an implementation detail,    *
 * however, and the functions below start with 0 being the index that refers  *
 * to the newest window. This is because the newest window will usually be a  *
 * reasonable default and it's simpler when writing the calling functions to  *
 * think of it that way.                                                      */

/* = Includes = */

#include "defines.h"
#include "cache.h"

/* = Definitions = */

#define WINDOW_BUFFER_LENGTH 48

/* = Type and Struct Definitions = */

/* = Function Prototypes = */

void window_create_text( bstring, CACHE_CACHE* );
void window_draw_text( int, CACHE_CACHE* );
void window_draw_menu( CACHE_CACHE* );
