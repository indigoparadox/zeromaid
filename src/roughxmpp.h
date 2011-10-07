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

#ifndef ROUGHXMPP_H
#define ROUGHXMPP_H

/* = Includes = */

#include "bstring/bstraux.h"
#include "bstring/bstrlib.h"
#include "ezxml/ezxml.h"
#include "defines.h"

/* = Definitions = */

#define ROUGHXMPP_COMMAND_STREAM_START 1

/* = Type and Struct Definitions = */

typedef struct {

} ROUGHXMPP_PARSE_DATA;

/* = Function Prototypes = */

int roughxmpp_parse_stanza( bstring, ROUGHXMPP_PARSE_DATA* );
bstring roughxmpp_create_stanza_hello( void );

#endif /* !ROUGHXMPP_H */
