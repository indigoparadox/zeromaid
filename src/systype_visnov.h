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

#ifndef SYSTYPE_VISNOV_H
#define SYSTYPE_VISNOV_H

/* = Notes = */

/* Conditionals and labels are a simple-to-implement system of flow control   *
 * for the time being. If a label is defined more than once, the first one    *
 * defined will be used.                                                      */

/* In order to add a command to the visual novel engine, one must:            *
 *                                                                            *
 * 1. Add an opcode constant and data count (parameter count) constant to the *
 *    definitions section below.                                              *
 * 2. Work the parameters into the data union below without messing up the    *
 *    data types for other commands (this is a pain).                         *
 * 3. Add a section to the load_commands function to load the new command.    *
 * 4. Add an exec function to execute the new command.                        */

/* To figure out a DI for a field:                                            *
 * 1. Figure out which commands will need that field. For example, both COND  *
 *    and SET use a "key" field.                                              *
 * 2. Find an index which does not collide with any other fields already      *
 *    being used by those commands you've chosen.                             */

/* = Includes = */

#include <stdarg.h>

#include "defines.h"
#include "cache.h"
// #include "graphics.h"
#include "event.h"
#include "util.h"
#include "window.h"
#include "mobile.h"

/* = Definitons = */

/* Each command is denoted by a numerical opcode, and then a data count (or   *
 * "DC") which specifies the size of the data array. The items in the data    *
 * array should then be accessed via their numerical indexes ("DI") as out-   *
 * lined in the SYSTYPE_VISNOV_DATA union definition below.                   */
typedef int SYSTYPE_VISNOV_CMD;
#define SYSTYPE_VISNOV_CMD_NULL 0
#define SYSTYPE_VISNOV_CMD_BACKGROUND 1
#define SYSTYPE_VISNOV_CMD_BACKGROUND_DC 1
#define SYSTYPE_VISNOV_CMD_PAUSE 2
#define SYSTYPE_VISNOV_CMD_PAUSE_DC 1
#define SYSTYPE_VISNOV_CMD_LABEL 3
#define SYSTYPE_VISNOV_CMD_LABEL_DC 1
#define SYSTYPE_VISNOV_CMD_COND 4
#define SYSTYPE_VISNOV_CMD_COND_DC 3
#define SYSTYPE_VISNOV_CMD_TALK 5
#define SYSTYPE_VISNOV_CMD_TALK_DC 3
#define SYSTYPE_VISNOV_CMD_GOTO 6
#define SYSTYPE_VISNOV_CMD_GOTO_DC 1
#define SYSTYPE_VISNOV_CMD_PORTRAIT 7
#define SYSTYPE_VISNOV_CMD_PORTRAIT_DC 5
#define SYSTYPE_VISNOV_CMD_TELEPORT 8
#define SYSTYPE_VISNOV_CMD_TELEPORT_DC 4
#define SYSTYPE_VISNOV_CMD_MENU 9
#define SYSTYPE_VISNOV_CMD_MENU_DC 6
#define SYSTYPE_VISNOV_CMD_SET 10
#define SYSTYPE_VISNOV_CMD_SET_DC 4

/* TODO: MUSIC, MENU */

/* = Type and Struct Definitions = */

/* == Command Stuff == */

typedef union {
   int null;   /* Can be used to space out DIs that can't be reconciled. */
   GEO_COLOR
      * color_fg,    /* DI 2 - MENU */
      * color_bg,    /* DI 3 - MENU */
      * color_sfg,   /* DI 4 - MENU */
      * color_sbg;   /* DI 5 - MENU */
   bstring name,     /* DI 0 - LABEL */
      emotion,       /* DI 1 - PORTRAIT */
      key,           /* DI 2 - COND, SET */
      equals,        /* DI 3 - COND, SET */
      destmap,       /* DI 0 - TELEPORT */
      talktext,      /* DI 1 - TALK */
      target,        /* DI 0 - GOTO, COND */
      items,         /* DI 0 - MENU */
      type,          /* DI 3 - TELEPORT */
      bg_filename;   /* DI 0 - BACKGROUND */
   COND_SCOPE scope; /* DI 1 - COND, MENU, SET */
   // ZZZ: GFX_SURFACE* bg;  /* DI 0 - BACKGROUND */
   int serial,       /* DI 0 - PORTRAIT, TALK */
      x,             /* DI 3 - PORTRAIT */
      y,             /* DI 4 - PORTRAIT */
      delay,         /* DI 0 - PAUSE*/
      destx,         /* DI 1 - TELEPORT */
      desty,         /* DI 2 - TELEPORT */
      speed;         /* DI 2 - TALK */
   float zoom;       /* DI 2 - PORTRAIT */
} SYSTYPE_VISNOV_DATA;

/* "data" is a dynamic array, but a count is not necessary as the required    *
 * attributes are guaranteed to be available by DI as specified in the        *
 * SYSTYPE_VISNOV_DATA struct definition above.                               */
typedef struct {
   SYSTYPE_VISNOV_CMD command;
   SYSTYPE_VISNOV_DATA* data;
} SYSTYPE_VISNOV_COMMAND;

/* = Macros = */

/* XXX: Don't mind this. We're just thinking to ourselves... */
/* #define STVN_COMMAND( cmd_name, ... ) \
   void stvn_##cmd_name##_implement( __VA_ARGS__ ) { \
   } \
   \
   void stvn_##cmd_name##_load( __VA_ARGS__ ) { \
   } */

/* Purpose: Select the appropriate cache for the given scope and set the      *
 *          given key to the given value within it.                           */
#define STVN_CACHE_SET( key, value, scope, pg_addr, gc_addr, pl_addr, lc_addr ) \
   if( COND_SCOPE_GLOBAL == scope ) { \
      cache_set_var( key, value, pg_addr, gc_addr ); \
   } else if( COND_SCOPE_LOCAL == scope ) { \
      cache_set_var( key, value, pl_addr, lc_addr ); \
   } else { \
      DBG_ERR_INT( "Unrecognized variable scope", scope ); \
   }

/* Purpose: Allocate a new command of the given type with the given DI count. */
#define STVN_PARSE_CMD_ALLOC( type, dc ) \
   s_command_tmp.command = type; \
   s_command_tmp.data = calloc( \
      dc, sizeof( SYSTYPE_VISNOV_DATA ) \
   );

/* Purpose: Parse an int attribute into the property specified by dtype.      */
#define STVN_PARSE_CMD_DAT_INT( dtype, di ) \
   if( NULL != ezxml_attr( ps_xml_command, #dtype ) ) { \
      s_command_tmp.data[di].dtype = \
         atoi( ezxml_attr( ps_xml_command, #dtype ) ); \
   }

/* Purpose: Parse a float attribute into the property specified by dtype.     */
#define STVN_PARSE_CMD_DAT_FLT( dtype, di ) \
   if( NULL != ezxml_attr( ps_xml_command, #dtype ) ) { \
      s_command_tmp.data[di].dtype = \
         atof( ezxml_attr( ps_xml_command, #dtype ) ); \
   }

/* Purpose: Parse a string attribute into the property specified by dtype.    */
#define STVN_PARSE_CMD_DAT_STR( dtype, di ) \
   if( NULL != ezxml_attr( ps_xml_command, #dtype ) ) { \
      s_command_tmp.data[di].dtype = \
         bfromcstr( ezxml_attr( ps_xml_command, #dtype ) ); \
   }

/* Purpose: Parse a scope attribute into the property specified by dtype.     */
#define STVN_PARSE_CMD_DAT_SCOPE( dtype, di ) \
   if( 0 == strcmp( ezxml_attr( ps_xml_command, #dtype ), "local" ) ) { \
      s_command_tmp.data[di].dtype = COND_SCOPE_LOCAL; \
      DBG_INFO( "Setting scope: local" ); \
   } else if( 0 == strcmp( ezxml_attr( ps_xml_command, #dtype ), "global" ) ) { \
      s_command_tmp.data[di].dtype = COND_SCOPE_GLOBAL; \
      DBG_INFO( "Setting scope: global" ); \
   } else { \
      s_command_tmp.data[di].dtype = -1; \
      DBG_INFO( "Setting scope: unknown" ); \
   }

/* Purpose: Parse a color attribute into the property specified by dtype.     */
#define STVN_PARSE_CMD_DAT_COL( dtype, di ) \
   ps_command_attr = bfromcstr( ezxml_attr( ps_xml_command, #dtype ) ); \
   s_command_tmp.data[di].dtype = \
      graphics_create_color_html( ps_command_attr ); \
   bdestroy( ps_command_attr ); \
   ps_command_attr = NULL;

/* Purpose: Parse an string body into the property specified by dtype.        */
#define STVN_PARSE_CMD_DAT_STR_BODY( dtype, di ) \
   if( NULL != ezxml_txt( ps_xml_command ) ) { \
      s_command_tmp.data[di].dtype = \
         bfromcstr( ezxml_txt( ps_xml_command ) ); \
      btrimws( s_command_tmp.data[di].dtype ); \
   }

/* = Function Prototypes = */

int systype_visnov_loop( CACHE_CACHE* );
BOOL systype_visnov_load_commands( SYSTYPE_VISNOV_COMMAND**, int*, ezxml_t );
int systype_visnov_exec_background(
   SYSTYPE_VISNOV_COMMAND*, bstring**, int
);
int systype_visnov_exec_pause( SYSTYPE_VISNOV_COMMAND*, int );
int systype_visnov_exec_cond(
   SYSTYPE_VISNOV_COMMAND*, SYSTYPE_VISNOV_COMMAND*, int, CACHE_VARIABLE*,
   int, CACHE_CACHE*, int
);
int systype_visnov_exec_portrait(
   SYSTYPE_VISNOV_COMMAND*, MOBILE_MOBILE***, int*, MOBILE_MOBILE*,
   int, int, CACHE_CACHE*
);
int systype_visnov_exec_teleport(
   SYSTYPE_VISNOV_COMMAND*, BOOL*, CACHE_CACHE*, int
);
int systype_visnov_exec_talk(
   SYSTYPE_VISNOV_COMMAND*, CACHE_CACHE*, EVENT_EVENT*, MOBILE_MOBILE*,
   int, int
);
int systype_visnov_exec_goto(
   SYSTYPE_VISNOV_COMMAND*, SYSTYPE_VISNOV_COMMAND*, int, int
);
int systype_visnov_exec_menu(
   SYSTYPE_VISNOV_COMMAND*, WINDOW_MENU**, CACHE_VARIABLE**, int*,
   CACHE_CACHE*, EVENT_EVENT*, int
);
int systype_visnov_exec_set(
   SYSTYPE_VISNOV_COMMAND*, CACHE_VARIABLE**, int*, CACHE_CACHE*, int
);
MOBILE_MOBILE* systype_visnov_get_actor(
   int, MOBILE_MOBILE*, int, CACHE_CACHE*
);
void systype_visnov_free_command_arr( SYSTYPE_VISNOV_COMMAND* );

#endif /* SYSTYPE_VISNOV_H */
