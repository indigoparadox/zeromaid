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

#include "server.h"

DBG_ENABLE

void* server_main( void* arg ) {
   int i_socket_listener, /* Listener socket handle. */
      i_socket_client, /* A new client connection. */
      i_thread_error,
      i_bind_result; /* Error code for listening bind. */
   struct sockaddr_in s_server_address,
      s_client_address;
   socklen_t i_client_address_len;
   SERVER_HANDLE_PARMS* ps_parms;
   pthread_t ps_client_handler;
   #ifdef WIN32
   WORD i_ws_version_requested;
   WSADATA s_wsa_data;
   int i_ws_result;
   #endif /* WIN32 */

   DBG_INFO( "Server thread started." );

   #ifdef WIN32
   i_ws_version_requested = MAKEWORD(2, 2);
   i_ws_result = WSAStartup( i_ws_version_requested, &s_wsa_data );
   if ( i_ws_result ) {
      DBG_ERR( "WSAStartup failed with error: %d", i_ws_result );
      goto main_server_cleanup;
   }
   #endif /* WIN32 */

   /* Start the socket listener and start listening on the server port. */
   i_socket_listener = socket( AF_INET, SOCK_STREAM, 0 );
   if( 0 > i_socket_listener ) {
      DBG_ERR( "Unable to open server listener socket: %d", i_socket_listener );
      goto main_server_cleanup;
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
      goto main_server_cleanup;
   }
   listen( i_socket_listener, NET_MAX_BACKLOG );
   DBG_INFO( "Server listening on port: %d", NET_PORT_LISTEN );

   /* Listen for new connections and branch off when one happens. */
   while( 1 ) {
      i_socket_client = accept(
         i_socket_listener,
         (struct sockaddr*)&s_client_address,
         &i_client_address_len
      );

      if( 0 > i_socket_client ) {
         DBG_ERR( "Error accepting new client connection." );
         continue;
      }

      /* Build the disposable parameter list to ship off with the handler. */
      ps_parms = calloc( 1, sizeof( SERVER_HANDLE_PARMS ) );
      ps_parms->socket_client = i_socket_client;
      ps_parms->client_address = bformat(
         "%s",
         inet_ntoa( s_client_address.sin_addr )
      );

      DBG_INFO(
         "Client connected: %s, %d",
         ps_parms->client_address->data,
         i_socket_client
      );

      i_thread_error = pthread_create(
         &ps_client_handler,
         NULL,
         &server_handle,
         (void*)ps_parms
      );
      if( i_thread_error ) {
         DBG_ERR(
            "Unable to create handler thread, error code: %d",
            i_thread_error
         );
         goto main_server_cleanup;
      }
   }

main_server_cleanup:

   close( i_socket_listener );

   // pthread_exit( NULL );
}

void* server_handle( SERVER_HANDLE_PARMS* ps_parms_in ) {
   bstring ps_client_msg = bformat( "" );
   char pc_client_msg_temp[SERVER_NET_BUFFER_SIZE];
   int i_client_msg_result = 0,
      i_client_command = 0;

   DBG_INFO(
      "Handling client connection: %d",
      ps_parms_in->socket_client
   );

   memset( &pc_client_msg_temp, '\0', SERVER_NET_BUFFER_SIZE );
   //while( 0 != i_client_msg_result ) {
   /* TODO: Die when there's nothing more to listen for. */
   while( 1 ) {
      i_client_msg_result = recv(
         ps_parms_in->socket_client,
         pc_client_msg_temp,
         SERVER_NET_BUFFER_SIZE - 1,
         0
      );
      bassigncstr(
         ps_client_msg,
         pc_client_msg_temp
      );

      i_client_command = roughxmpp_parse_stanza( ps_client_msg );

      //bdestroy( ps_client_msg );
      //break;
   }

   DBG_INFO( "%s", ps_client_msg->data );

server_handle_cleanup:

   close( ps_parms_in->socket_client );
   free( ps_parms_in );

   return NULL;
}
