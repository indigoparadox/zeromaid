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

#include "client.h"

DBG_ENABLE

/* = Functions = */

int client_main( MAIN_PARAMS* ps_params_in ) {
   CACHE_CACHE* ps_cache = NULL;

   DBG_INFO( "Client thread started." );

}

void client_send( int i_client_in, bstring ps_send_in ) {
   int i_server_msg_result;

   #ifdef NET_DEBUG_PROTOCOL_PRINT
   DBG_INFO(
      "Server %d response: %s",
      i_client_in,
      bdata( ps_send_in )
   );
   #endif /* NET_DEBUG_PROTOCOL_PRINT */
   i_server_msg_result = send(
      i_client_in,
      bdata( ps_send_in ),
      blength( ps_send_in ),
      0
   );
   if( blength( ps_send_in ) != i_server_msg_result ) {
      DBG_ERR(
         "Unable to send entire stanza: %s",
         bdata( ps_send_in )
      );
   }
}
