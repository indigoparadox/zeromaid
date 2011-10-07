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

#ifndef SERVER_H
#define SERVER_H

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
#include "roughxmpp.h"

/* = Definitions = */

#define SERVER_NET_BUFFER_SIZE 4096

/* = Type and Struct Definitions = */

typedef struct {
   int socket_client;
   bstring client_address;
} SERVER_HANDLE_PARMS;

/* = Function Prototypes = */

void* server_main( void* );
void* server_handle( SERVER_HANDLE_PARMS* );

#endif /* !SERVER_H */
