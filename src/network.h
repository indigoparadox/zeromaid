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

#ifndef NETWORK_H
#define NETWORK_H

/* = Includes = */

#ifdef WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <sys/socket.h>
#  include <netdb.h>
#  include <arpa/inet.h>
#  include <sys/select.h>
#endif /* WIN32 */

#include "defines.h"

/* = Definitions = */

typedef int NETWORK_SOCKET;
typedef struct sockaddr NETWORK_ADDRESS;
typedef struct sockaddr_in NETWORK_ADDRESS_IN;
typedef socklen_t NETWORK_SOCKLEN;

/* = Function Prototypes = */

NETWORK_SOCKET network_listen( void );
NETWORK_SOCKET network_accept(
   NETWORK_SOCKET, NETWORK_ADDRESS*, NETWORK_SOCKLEN*
);
void network_send( NETWORK_SOCKET, bstring );

#endif /* !CLIENT_H */
