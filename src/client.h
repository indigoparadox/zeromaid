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

#ifndef CLIENT_H
#define CLIENT_H

/* = Includes = */

#ifdef USESDL
#ifdef __APPLE__
#include <SDL/SDL.h>
#elif defined __unix__
#include <SDL/SDL.h>
#elif defined USEWII
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif /* __APPLE__, __unix__, USEWII */
#endif /* USESDL */

#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/socket.h>
#  include <netdb.h>
#  include <arpa/inet.h>
#  include <sys/select.h>
#endif /* WIN32 */

#include "bstring/bstraux.h"
#include "bstring/bstrlib.h"
#include "defines.h"
#include "cache.h"
#include "graphics.h"
#include "systype_title_client.h"
#include "systype_adventure_client.h"

/* = Function Prototypes = */

void* client_main( void* );

#endif /* !CLIENT_H */
