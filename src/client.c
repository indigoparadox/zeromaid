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
   struct sockaddr_in s_server_address;
   unsigned short  i_server_port;

   DBG_INFO( "Client thread started." );

   /* TODO: Open server connection. */

   /* TODO: Try to connect to default room; that can redirect to system-load  *
    *       room, maybe?                                                      */

}
