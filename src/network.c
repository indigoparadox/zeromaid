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

#include "network.h"

DBG_ENABLE

/* = Functions = */

NETWORK_SOCKET network_listen( void ) {
   NETWORK_SOCKET i_socket_listener;
   int i_bind_result; /* Error code for listening bind. */
   NETWORK_ADDRESS_IN s_server_address;
   #ifdef WIN32
   WORD i_ws_version_requested;
   WSADATA s_wsa_data;
   int i_ws_result;
   #endif /* WIN32 */

   #ifdef WIN32
   i_ws_version_requested = MAKEWORD(2, 2);
   i_ws_result = WSAStartup( i_ws_version_requested, &s_wsa_data );
   if ( i_ws_result ) {
      DBG_ERR( "WSAStartup failed with error: %d", i_ws_result );
      goto network_listen_cleanup;
   }
   #endif /* WIN32 */

   /* Start the socket listener and start listening on the server port. */
   i_socket_listener = socket( AF_INET, SOCK_STREAM, 0 );
   if( 0 > i_socket_listener ) {
      DBG_ERR( "Unable to open server listener socket: %d", i_socket_listener );
      goto network_listen_cleanup;
   }
   memset( &s_server_address, '\0', sizeof( struct sockaddr_in ) );
   s_server_address.sin_family = AF_INET;
   s_server_address.sin_addr.s_addr = INADDR_ANY;
   s_server_address.sin_port = htons( NET_PORT_LISTEN );
   i_bind_result = bind(
      i_socket_listener,
      (struct sockaddr*)&s_server_address,
      sizeof( s_server_address )
   );
   if( 0 > i_bind_result ) {
      DBG_ERR( "Unable to listen on port: %d", NET_PORT_LISTEN );
      goto network_listen_cleanup;
   }
   listen( i_socket_listener, NET_MAX_BACKLOG );
   DBG_INFO( "Server listening on port: %d", NET_PORT_LISTEN );

network_listen_cleanup:

   return i_socket_listener;
}

NETWORK_SOCKET network_accept(
   NETWORK_SOCKET i_socket_listener_in,
   NETWORK_ADDRESS* ps_client_address_in,
   NETWORK_SOCKLEN* ps_client_address_len_in
) {
   return accept(
      i_socket_listener_in,
      (struct sockaddr*)ps_client_address_in,
      ps_client_address_len_in
   );
}

void network_send( NETWORK_SOCKET i_remote_in, bstring ps_send_in ) {
   int i_client_msg_result;

   #ifdef NET_DEBUG_PROTOCOL_PRINT
   DBG_INFO(
      "Server %d response: %s",
      i_remote_in,
      bdata( ps_send_in )
   );
   #endif /* NET_DEBUG_PROTOCOL_PRINT */
   i_client_msg_result = send(
      i_remote_in,
      bdata( ps_send_in ),
      blength( ps_send_in ),
      0
   );
   if( blength( ps_send_in ) != i_client_msg_result ) {
      DBG_ERR(
         "Unable to send entire stanza: %s",
         bdata( ps_send_in )
      );
   }
}
