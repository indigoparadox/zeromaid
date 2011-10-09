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

#ifndef ROUGHIRC_H
#define ROUGHIRC_H

/* = Includes = */

#include "bstring/bstraux.h"
#include "bstring/bstrlib.h"

/* = Definitions = */

#define ROUGHIRC_COMMAND_USER 1
#define ROUGHIRC_COMMAND_NICK 2

#define ROUGHXMPP_SERVER 1
#define ROUGHXMPP_CLIENT 2

/* = Type and Struct Definitions = */

typedef struct {

} ROUGHIRC_PARSE_DATA;

/* = Function Prototypes = */

int roughirc_server_parse( bstring, ROUGHIRC_PARSE_DATA* );
bstring roughirc_connect_respond( int );

#endif /* !ROUGHIRC_H */
