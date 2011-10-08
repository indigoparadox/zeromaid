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

DBG_ENABLE

/* = Functions = */

bstring roughxmpp_create_id( void ) {
   /* XXX: Actually generate an ID. */
   return bformat( "FFFFFF" );
}

int roughxmpp_parse_stanza(
   bstring ps_stanza_in,
   ROUGHXMPP_PARSE_DATA* ps_data_out
) {
   ezxml_t ps_xml_stanza = NULL;
   int i_command_out = 0;
   char* pc_stanza_name = NULL;

   /* Try to interpret the incoming stanza in an XML way. */
   ps_xml_stanza = ezxml_parse_str(
      bdata( ps_stanza_in ),
      blength( ps_stanza_in )
   );
   if( NULL == ps_xml_stanza ) {
      DBG_ERR( "Invalid stanza specified: %s", bdata( ps_stanza_in ) );
      i_command_out = 0;
      goto rxps_cleanup;
   }
   pc_stanza_name = ezxml_name( ps_xml_stanza );
   if( NULL == pc_stanza_name ) {
      /* This happens sometimes... */
      i_command_out = 0;
      goto rxps_cleanup;
   }

   /* TODO: Is strcmp a security problem? */
   if( 0 == strcmp( "stream:stream", pc_stanza_name ) ) {
      /* We're super-forgiving, so we don't really care about all of those    *
       * silly namespaces and what-not.                                       */
      i_command_out = ROUGHXMPP_COMMAND_STREAM_START;
   }

rxps_cleanup:

   ezxml_free( ps_xml_stanza );

   return i_command_out;
}

bstring roughxmpp_create_stanza_hello( int i_client_server_in ) {
   ezxml_t ps_xml_stanza_out;
   bstring ps_attr_temp,
      ps_stanza_out;
   char* pc_xml_temp = NULL;

   /* Build the XML stanza to export. */
   ps_xml_stanza_out = ezxml_new( "stream:stream" );
   ezxml_set_attr( ps_xml_stanza_out, "xmlns", "jabber:client" );
   ezxml_set_attr(
      ps_xml_stanza_out, "xmlns:stream", "http://etherx.jabber.org/streams"
   );
   if( ROUGHXMPP_SERVER == i_client_server_in ) {
      ps_attr_temp = roughxmpp_create_id();
      ezxml_set_attr(
         ps_xml_stanza_out,
         "id",
         "FFFF"
         //bdata( ps_attr_temp )
      );
      bdestroy( ps_attr_temp );
      // TODO: Allow servers hosted on non-localhost.
      ezxml_set_attr( ps_xml_stanza_out, "from", "localhost" );
   } else if( ROUGHXMPP_CLIENT == i_client_server_in ) {
      // TODO: Client "to" attribute.
   }
   ezxml_set_attr( ps_xml_stanza_out, "version", "1.0" );

   /* Convert the stanza to a string. */
   ps_stanza_out = bfromcstr( pc_xml_temp = ezxml_toxml( ps_xml_stanza_out ) );
   free( pc_xml_temp );

rxcs_hello_cleanup:

   ezxml_free( ps_xml_stanza_out );

   return ps_stanza_out;
}

bstring roughxmpp_create_stanza_authinform( void ) {
   ezxml_t ps_xml_stanza_out,
      ps_xml_stanza_mechanisms,
      ps_xml_stanza_mech_plain;
   bstring ps_attr_temp,
      ps_stanza_out;
   char* pc_xml_temp = NULL;

   /* Build the XML stanza to export. */
   ps_xml_stanza_out = ezxml_new( "stream:features" );
   ps_xml_stanza_mechanisms = ezxml_add_child(
      ps_xml_stanza_out,
      "mechanisms",
      0
   );
   ezxml_set_attr(
      ps_xml_stanza_mechanisms,
      "xmlns",
      "urn:ietf:params:xml:ns:xmpp-sasl"
   );
   ps_xml_stanza_mech_plain = ezxml_add_child(
      ps_xml_stanza_mechanisms,
      "mechanism",
      0
   );
   ezxml_set_txt( ps_xml_stanza_mech_plain, "PLAIN" );

   /* Convert the stanza to a string. */
   ps_stanza_out = bfromcstr( pc_xml_temp = ezxml_toxml( ps_xml_stanza_out ) );
   free( pc_xml_temp );

rxcs_hello_cleanup:

   ezxml_free( ps_xml_stanza_out );

   return ps_stanza_out;
}
