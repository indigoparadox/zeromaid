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

#include "roughxmpp.h"

/* = Functions = */

int roughxmpp_parse_stanza( bstring ps_stanza_in ) {
   ezxml_t ps_xml_stanza;

   ps_xml_stanza = ezxml_parse_str(
      bdata( ps_stanza_in ),
      blength( ps_stanza_in )
   );

   if( 0 == strncmp( "stream:stream", ezxml_name( ps_xml_stanza ), 13 ) ) {
      /* We're super-forgiving, so we don't really care about all of those    *
       * silly namespaces and what-not.                                       */
   }
}
