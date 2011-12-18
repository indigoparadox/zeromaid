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

SERVER_ROOM* gps_server_rooms;

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

   return NULL;
}

/* Purpose: Handle an incoming client connection indefinitely after it has    *
 *          been established in server_main().                                */
void* server_handle( SERVER_HANDLE_PARMS* ps_parms_in ) {
   bstring ps_client_msg = bformat( "" ),
      ps_client_msg_iter = bformat( "" ),
      ps_client_msg_data = bformat( "" );
   char pc_client_msg_temp[SERVER_NET_BUFFER_SIZE];
   int i_client_msg_result = 0,
      i_client_command = 0,
      i_next_newline = 0;
   ROUGHIRC_PARSE_DATA s_parse_data;
   SERVER_ROOM* ps_room_op,
      * ps_room_iter;
   SERVER_CLIENT_HANDLE* ps_clihan_iter,
      * ps_clihan_new;

   DBG_INFO(
      "Handling client connection: %d",
      ps_parms_in->socket_client
   );

   memset( &pc_client_msg_temp, '\0', SERVER_NET_BUFFER_SIZE );
   //while( 0 != i_client_msg_result ) {
   /* TODO: Die when there's nothing more to listen for. */
   while( 1 ) {
      /* Clear the parsing structures and read the next stanza. */
      memset( &s_parse_data, '\0', sizeof( ROUGHIRC_PARSE_DATA ) );
      memset(
         pc_client_msg_temp,
         '\0',
         SERVER_NET_BUFFER_SIZE * sizeof( char )
      );
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

      /* If nothing was read then the client probably closed the connection.  */
      if( 0 == i_client_msg_result ) {
         break;
      }

      /* Break the response into lines and process it line-by-line. */
      while( 0 != blength( ps_client_msg ) ) {
         /* Pull off the first line and process it. */
         i_next_newline = bstrchr( ps_client_msg, '\n' );
         bassignmidstr( ps_client_msg_iter, ps_client_msg, 0, i_next_newline + 1 );
         bdelete( ps_client_msg, 0, i_next_newline + 1 );

         #ifdef NET_DEBUG_PROTOCOL_PRINT
         DBG_INFO(
            "Client %d response: %s",
            ps_parms_in->socket_client,
            bdata( ps_client_msg_iter )
         );
         #endif /* NET_DEBUG_PROTOCOL_PRINT */

         /* Determine the client command and act on it. */
         i_client_command = roughirc_server_parse(
            ps_client_msg_iter,
            &s_parse_data
         );
         switch( i_client_command ) {
            case ROUGHIRC_COMMAND_NICK:
               ps_parms_in->client_nick =
                  roughirc_server_parse_nick( ps_client_msg_iter );

               #ifdef NET_DEBUG_PROTOCOL_PRINT
               DBG_INFO(
                  "Client %d NICK set: %s",
                  ps_parms_in->socket_client,
                  bdata( ps_parms_in->client_nick )
               );
               #endif /* NET_DEBUG_PROTOCOL_PRINT */
               break;

            case ROUGHIRC_COMMAND_JOIN:
               ps_client_msg_data =
                  roughirc_server_parse_join( ps_client_msg_iter );

               /* TODO: JOIN 0 should leave all channels. */

               /* Try to find the specified channel and join it. */
               UTIL_LLIST_FSTR(
                  gps_server_rooms, ps_room_op, map_name, ps_client_msg_data
               );
               if( NULL == ps_room_op ) {
                  /* Create the requested room if it does not exist. */

                  #ifdef NET_DEBUG_PROTOCOL_PRINT
                  DBG_INFO(
                     "Client %d tried to enter non-existant room \"%s\"; "
                        "creating...",
                     ps_parms_in->socket_client,
                     bdata( ps_client_msg_data )
                  );
                  #endif /* NET_DEBUG_PROTOCOL_PRINT */

                  ps_room_op = calloc( 1, sizeof( SERVER_ROOM ) );
                  ps_room_op->map_name = ps_client_msg_data;

                  /* TODO: Create a map for the room. */

                  UTIL_LLIST_ADD(
                     gps_server_rooms,
                     ps_room_iter,
                     server_handle_cleanup,
                     ps_room_op
                  );
               }

               #ifdef NET_DEBUG_PROTOCOL_PRINT
               DBG_INFO(
                  "Client %d entering room \"%s\"...",
                  ps_parms_in->socket_client,
                  bdata( ps_client_msg_data )
               );
               #endif /* NET_DEBUG_PROTOCOL_PRINT */

               /* TODO: Add the client to the requested room. */
               ps_clihan_new = calloc( 1, sizeof( SERVER_CLIENT_HANDLE ) );
               ps_clihan_new->client_id = ps_parms_in->socket_client;

               UTIL_LLIST_ADD(
                  ps_room_op->clients_present,
                  ps_clihan_iter,
                  server_handle_cleanup,
                  ps_clihan_new
               );

               break;

            case ROUGHIRC_COMMAND_PART:
               ps_client_msg_data =
                  roughirc_server_parse_part( ps_client_msg_iter );

               /* TODO: Accept multiple comma-separated channels. */

               UTIL_LLIST_FSTR(
                  gps_server_rooms, ps_room_op, map_name, ps_client_msg_data
               );
               if( NULL != ps_room_op ) {
                  UTIL_LLIST_FINT(
                     ps_room_op->clients_present,
                     ps_clihan_new,
                     client_id,
                     ps_parms_in->socket_client
                  );
                  UTIL_LLIST_REMOVE(
                     ps_room_op->clients_present,
                     ps_clihan_iter,
                     ps_clihan_new
                  );
               }
               break;

            default:
               #ifdef NET_DEBUG_PROTOCOL_PRINT
               DBG_INFO(
                  "Client %d unknown command: \"%s\"",
                  ps_parms_in->socket_client,
                  bdata( ps_client_msg_iter )
               );
               #endif /* NET_DEBUG_PROTOCOL_PRINT */
         }
      }

      //break;
   }

server_handle_cleanup:

   DBG_INFO(
      "Done with client connection: %d",
      ps_parms_in->socket_client
   );

   close( ps_parms_in->socket_client );
   free( ps_parms_in );
   bdestroy( ps_client_msg );

   return NULL;
}

void server_send( int i_client_in, bstring ps_send_in ) {
   int i_client_msg_result;

   #ifdef NET_DEBUG_PROTOCOL_PRINT
   DBG_INFO(
      "Server %d response: %s",
      i_client_in,
      bdata( ps_send_in )
   );
   #endif /* NET_DEBUG_PROTOCOL_PRINT */
   i_client_msg_result = send(
      i_client_in,
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

/* Purpose: Verify that the system file is valid and load its XML tree.       */
ezxml_t server_load_system( void ) {
   bstring ps_system_path;

   ps_system_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_SYSTEM );

   /* Verify the XML file exists and open or abort accordingly. */
   if( !zm_file_exists( ps_system_path ) ) {
      DBG_ERR( "Unable to find system file: %s", ps_system_path->data );
      return NULL;
   } else {
      return ezxml_parse_file( (const char*)ps_system_path->data );
   }
}

/* Purpose: Get the starting team and load it into the given cache object.    */
/* Return: A boolean indicating success (TRUE) or failure (FALSE).            */
BOOL server_load_team( CACHE_CACHE* ps_cache_in ) {
   BOOL b_success = TRUE;
   bstring ps_mobile_src = NULL;
   ezxml_t ps_xml_system = NULL,
      ps_xml_team = NULL,
      ps_xml_story = NULL,
      ps_xml_member_iter = NULL;
   MOBILE_MOBILE* ps_member_temp;

   /* Load the system file. */
   ps_xml_system = server_load_system();
   if( NULL == ps_xml_system ) {
      b_success = FALSE;
      goto stlt_cleanup;
   }

   /* Load the team XML node. */
   ps_xml_story = ezxml_child( ps_xml_system, "story" );
   if( NULL == ps_xml_story ) {
      DBG_ERR( "Invalid system data format: Missing <story> element." );
      b_success = FALSE;
      goto stlt_cleanup;
   }
   ps_xml_team = ezxml_child( ps_xml_story, "team" );
   if( NULL == ps_xml_team ) {
      DBG_ERR(
         "Invalid system data format: Missing <team> element."
      );
      b_success = FALSE;
      goto stlt_cleanup;
   }

   /* Cycle through team member nodes and create their structures. */
   DBG_INFO( "Loading team members..." );
   ps_xml_member_iter = ezxml_child( ps_xml_team, "member" );
   while( NULL != ps_xml_member_iter ) {
      /* Create a new mobile. */
      ps_mobile_src = bformat( "%s", ezxml_attr( ps_xml_member_iter, "src" ) );
      ps_member_temp = mobile_load_mobile( ps_mobile_src );
      if( NULL != ps_member_temp ) {
         /* Set the special properties making this a player mobile. */
         ps_member_temp->serial =
            atoi( ezxml_attr( ps_xml_member_iter, "serial" ) );
         ps_member_temp->pixel_x =
            ps_member_temp->pixel_size *
            atoi( ezxml_attr( ps_xml_team, "startx" ) );
         ps_member_temp->pixel_y =
            ps_member_temp->pixel_size *
            atoi( ezxml_attr( ps_xml_team, "starty" ) );

         /* Move this mobile to the team list. */
         UTIL_ARRAY_ADD(
            MOBILE_MOBILE, ps_cache_in->player_team,
            ps_cache_in->player_team_count, stlt_cleanup, ps_member_temp
         );
         ps_member_temp = NULL;
      } else {
      }

      /* Go to the next one! */
      ps_xml_member_iter = ezxml_next( ps_xml_member_iter );
   }

stlt_cleanup:

   ezxml_free( ps_xml_system );

   /* TODO: Write this function. */
   return b_success;
}

/* Purpose: Get the starting game and load it into the given cache object.    */
/* Parameters: A cache object to fill with the starting game data.            */
/* Return: A boolean indicating success (TRUE) or failure (FALSE).            */
BOOL server_load_start( CACHE_CACHE* ps_cache_out ) {
   BOOL b_success = TRUE;
   ezxml_t ps_xml_system = NULL,
      ps_xml_story = NULL,
      ps_xml_smap = NULL;
   bstring ps_system_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_SYSTEM );
   CACHE_CACHE s_cache_temp;

   /* Reminder to my future self: If the cache remained a global variable,    *
    * might never have found this. Stupid. ~_~                                */
   memset( &s_cache_temp, 0, sizeof( CACHE_CACHE ) );

   /* Verify the XML file exists and open or abort accordingly. */
   ps_xml_system = server_load_system();
   if( NULL == ps_xml_system ) {
      b_success = FALSE;
      goto stls_cleanup;
   }

   /* Load the single-player story data. */
   ps_xml_story = ezxml_child( ps_xml_system, "story" );
   if( NULL == ps_xml_story ) {
      DBG_ERR( "Invalid system data format: Missing <story> element." );
      b_success = FALSE;
      goto stls_cleanup;
   }
   ps_xml_smap = ezxml_child( ps_xml_story, "startmap" );
   if( NULL == ps_xml_smap ) {
      DBG_ERR(
         "Invalid system data format: Missing <startmap> element."
      );
      b_success = FALSE;
      goto stls_cleanup;
   }

   /* Load the starting map. */
   if( NULL != ezxml_attr( ps_xml_smap, "type" ) ) {
      CACHE_SYSTEM_TYPE_SET( &s_cache_temp, ezxml_attr( ps_xml_smap, "type" ) );
      DBG_INFO( "Game type selected: %s", ezxml_attr( ps_xml_smap, "type" ) );
   } else {
      DBG_ERR( "No game type specefied." );
      b_success = FALSE;
      goto stls_cleanup;
   }

   /* Load the starting field name. */
   CACHE_MAP_NAME_SET( &s_cache_temp, ezxml_attr( ps_xml_smap, "name" ) );

   /* If everything checks out then copy the temp cache onto the given cache. */
   /* TODO: Validation per game type. */
   memcpy( ps_cache_out, &s_cache_temp, sizeof( CACHE_CACHE ) );

stls_cleanup:

   bdestroy( ps_system_path );
   ezxml_free( ps_xml_system );

   return b_success;
}
