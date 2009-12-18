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

/* = Notes = */

/* Conditionals and labels are a simple-to-implement system of flow control   *
 * for the time being. If a label is defined more than once, the first one    *
 * defined will be used.                                                      */

/* = Includes = */

#include "defines.h"
#include "cache.h"
#include "graphics.h"
#include "event.h"
#include "util.h"

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

/* TODO: MUSIC, MENU */

/* = Type and Struct Definitions = */

/* == Command Stuff == */

typedef union {
   bstring name,     /* DI 0 - LABEL */
      key,           /* DI 1 - COND */
      equals,        /* DI 2 - COND */
      destmap,       /* DI 0 - TELEPORT */
      talktext,      /* DI 1 - TALK */
      target;        /* DI 0 - GOTO, COND */
   COND_SCOPE scope; /* DI 3 - COND */
   GFX_SURFACE* bg;  /* DI 0 - BACKGROUND */
   int serial,       /* DI 0 - PORTRAIT, TALK */
      emotion,       /* DI 1 - PORTRAIT */
      x,             /* DI 3 - PORTRAIT */
      y,             /* DI 4 - PORTRAIT */
      delay,         /* DI 0 - PAUSE*/
      destx,         /* DI 1 - TELEPORT */
      desty,         /* DI 2 - TELEPORT */
      speed;         /* DI 2 - TALK */
   float zoom;       /* DI 2 - PORTRAIT */
   SYSTEM_TYPE type; /* DI 3 - TELEPORT */
} SYSTYPE_VISNOV_DATA;

/* "data" is a dynamic array, but a count is not necessary as the required    *
 * attributes are guaranteed to be available by DI as specified in the        *
 * SYSTYPE_VISNOV_DATA struct definition above.                               */
typedef struct {
   SYSTYPE_VISNOV_CMD command;
   SYSTYPE_VISNOV_DATA* data;
} SYSTYPE_VISNOV_COMMAND;

/* == Actor Stuff == */

typedef struct {
   int id;
   GFX_SURFACE* image;
} SYSTYPE_VISNOV_EMOTION;

typedef struct {
   bstring name;
   int serial,
      x, y, /* Current on-screen location. */
      emotion_current, /* Currently selected emotion. */
      emotions_count; /* Number of available emotions. */
   SYSTYPE_VISNOV_EMOTION* emotions; /* Dynamic array of available emotions. */
} SYSTYPE_VISNOV_ACTOR;

/* == Scene Stuff == */

typedef struct {
   BOOL dirty; /* Does the scene need to be redrawn? */
   GFX_SURFACE* bg; /* Current scene background. */
   bstring var_keys, /* Local variable keys. */
      var_values; /* Parallel to var_keys, local variable values. */
   int var_count, /* Local variable count. */
      actors_onscreen_count;
   SYSTYPE_VISNOV_ACTOR** actors_onscreen; /* Dupe ptrs to actors on-screen. */
} SYSTYPE_VISNOV_SCENE;

/* = Macros = */

#define STVN_PARSE_CMD_ALLOC( type, dc ) \
   s_command_tmp.command = type; \
   s_command_tmp.data = calloc( \
      dc, sizeof( SYSTYPE_VISNOV_DATA ) \
   );

#define STVN_PARSE_CMD_DAT_INT( dtype, di ) \
   bassignformat( \
      ps_command_attr, "%s", ezxml_attr( ps_xml_command, #dtype ) \
   ); \
   s_command_tmp.data[di].dtype = \
      atoi( ps_command_attr->data );

#define STVN_PARSE_CMD_DAT_FLT( dtype, di ) \
   bassignformat( \
      ps_command_attr, "%s", ezxml_attr( ps_xml_command, #dtype ) \
   ); \
   s_command_tmp.data[di].dtype = \
      atof( ps_command_attr->data );

#define STVN_PARSE_CMD_DAT_STR( dtype, di ) \
   bassignformat( \
      ps_command_attr, "%s", ezxml_attr( ps_xml_command, #dtype ) \
   ); \
   s_command_tmp.data[di].dtype = \
      bstrcpy( ps_command_attr );

/* = Function Prototypes = */

int systype_visnov_loop( bstring );
SYSTYPE_VISNOV_ACTOR* systype_visnov_load_actors( int*, ezxml_t );
SYSTYPE_VISNOV_COMMAND* systype_visnov_load_commands( int*, ezxml_t );
void systype_visnov_exec_command(
   SYSTYPE_VISNOV_COMMAND*, BOOL*, int*, SYSTYPE_VISNOV_SCENE*,
   SYSTYPE_VISNOV_ACTOR*, int
);
SYSTYPE_VISNOV_ACTOR* systype_visnov_get_actor(
   int, SYSTYPE_VISNOV_ACTOR*, int
);
void systype_visnov_free_actor_arr( SYSTYPE_VISNOV_ACTOR* );
void systype_visnov_free_command_arr( SYSTYPE_VISNOV_COMMAND* );
