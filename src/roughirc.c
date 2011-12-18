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

#include "roughirc.h"

int roughirc_server_parse(
   bstring ps_client_string_in,
   ROUGHIRC_PARSE_DATA* ps_connect_data_out
) {
   if( 0 == binstrcaseless( ps_client_string_in, 0, bfromcstr( "NICK" ) ) ) {
      return ROUGHIRC_COMMAND_NICK;

   } else if(
      0 == binstrcaseless( ps_client_string_in, 0, bfromcstr( "JOIN" ) )
   ) {
      return ROUGHIRC_COMMAND_JOIN;

   } else if(
      0 == binstrcaseless( ps_client_string_in, 0, bfromcstr( "PART" ) )
   ) {
      return ROUGHIRC_COMMAND_PART;

   } else {
      return ROUGHIRC_COMMAND_UNKNOWN;
   }
}

/* Purpose: Extract and return the nickname from the given NICK stanza.       */
bstring roughirc_server_parse_nick( bstring ps_client_string_in ) {
   bstring ps_nick_out;

   /* TODO: Some error-checking, maybe. */

   /* Trim off the "NICK" command and any whitespace. */
   ps_nick_out = bmidstr( ps_client_string_in, 5, ROUGHIRC_OPT_NICK_MAXLEN );
   btrimws( ps_nick_out );

   return ps_nick_out;
}

/* Purpose: Extract and return the channel from the given JOIN stanza.        */
bstring roughirc_server_parse_join( bstring ps_client_string_in ) {
   bstring ps_room_out;

   /* TODO: Some error-checking, maybe. */

   /* Trim off the "JOIN" command and any whitespace. */
   ps_room_out = bmidstr( ps_client_string_in, 5, ROUGHIRC_OPT_ROOM_MAXLEN );
   btrimws( ps_room_out );

   return ps_room_out;
}

/* Purpose: Extract and return the channel from the given PART stanza.        */
bstring roughirc_server_parse_part( bstring ps_client_string_in ) {
   bstring ps_room_out;

   /* TODO: Some error-checking, maybe. */

   /* Trim off the "PART" command and any whitespace. */
   ps_room_out = bmidstr( ps_client_string_in, 5, ROUGHIRC_OPT_ROOM_MAXLEN );
   btrimws( ps_room_out );

   return ps_room_out;
}
