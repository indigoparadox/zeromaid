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

#ifndef SYSLOOP_TITLE_CLIENT_H
#define SYSLOOP_TITLE_CLIENT_H

/* = Includes = */

#include "graphics.h"
#include "systype_title.h"

/* = Type and Struct Definitions = */

/* = Function Prototypes = */

int systype_title_client_loop( CACHE_CACHE* );
void systype_title_client_show_menu(
   int, bstring[], SYSTYPE_TITLE_TITLESCREEN*
);
void systype_title_client_free_titlescreen( SYSTYPE_TITLE_TITLESCREEN* );

#endif /* !SYSLOOP_TITLE_CLIENT_H */
