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

#include "systype_visnov.h"

DBG_ENABLE

/* = Global Variables = */

extern CACHE_CACHE* gps_cache;

/* = Functions = */

/* Purpose: Visual novel loop.                                                */
/* Parameters: The name of the scene to play.                                 */
/* Return: The code for the next action to take.                              */
int systype_visnov_loop( bstring ps_scene_name_in ) {
   SYSTYPE_VISNOV_COMMAND* as_commands = NULL;
   int i_commands_count = 0,
      i_act_return = RETURN_ACTION_TITLE,
      i_command_cursor, /* The index of the command to execute next. */
      i; /* Loop iterator. */
   GFX_COLOR* ps_color_fade;
   bstring ps_scene_path = NULL;
   ezxml_t ps_xml_scene = NULL;
   BOOL b_wait_for_talk = FALSE; /* Are we waiting for the player to read? */
   EVENT_EVENT s_event;
   SYSTYPE_VISNOV_SCENE s_scene;

   /* Verify the XML file exists and open or abort accordingly. */
   ps_scene_path =
      bformat( "%sscene_%s.xml", PATH_SHARE , ps_scene_name_in->data );
   if( !file_exists( ps_scene_path ) ) {
      DBG_ERR_STR( "Unable to load scene data", ps_scene_path->data );
      i_act_return = RETURN_ACTION_TITLE;
      goto stvnl_cleanup;
   }
   ps_xml_scene = ezxml_parse_file( (const char*)ps_scene_path->data );

   /* Initialize what we need to use functions to initialize. */
   // TODO: Load scene commands, etc.
   ps_color_fade = graphics_create_color( 0, 0, 0 );
   as_commands = systype_visnov_load_commands(
      &i_commands_count,
      ezxml_child( ps_xml_scene, "script" )
   );

   while( 1 ) {
      GFX_DRAW_LOOP_START

      /* Listen for events. */
      event_do_poll( &s_event, TRUE );
      if( s_event.state[EVENT_ID_FIRE] && b_wait_for_talk ) {
         b_wait_for_talk = FALSE;
      }

      /* Execute the next command unless we're waiting. */
      if( !b_wait_for_talk ) {
         systype_visnov_exec_command(
            &as_commands[i_command_cursor], &i_command_cursor, &s_scene
         );
      }
      if( s_event.state[EVENT_ID_ESC] || s_event.state[EVENT_ID_QUIT] ) {
         gps_cache->game_type = SYSTEM_TYPE_TITLE;
         goto stvnl_cleanup;
      }

      GFX_DRAW_LOOP_END
   }

stvnl_cleanup:

   bdestroy( ps_scene_path );
   for( i = 0 ; i < i_commands_count ; i++ ) {
      systype_visnov_free_command_arr( &as_commands[i] );
   }
   free( as_commands );

   return i_act_return;
}

/* Purpose: Load the array of scene commands.                                 */
/* Parameters: The address of the command array size indicator.               */
/* Return: A pointer to the command array.                                    */
SYSTYPE_VISNOV_COMMAND* systype_visnov_load_commands(
   int* i_count_out,
   ezxml_t ps_xml_script_in
) {
   SYSTYPE_VISNOV_COMMAND* ps_commands_out = NULL;
   ezxml_t ps_xml_command = NULL;
   const char* pc_command_action = NULL; /* Pointer to ezxml action result. */
   bstring ps_command_attr = bfromcstr( "" ), /* String holding what to add to cmd data. */
      ps_command_action = bfromcstr( "" ); /* Copy of the command's action for later. */

   /* Verify that the script exists. */
   if( NULL == ps_xml_script_in ) {
      DBG_ERR( "Invalid visual novel script detected." );
      goto stvnlc_cleanup;
   }

   /* Initialize what we need to. */
   *i_count_out = 0;

   /* Load each command in order. */
   ps_xml_command = ezxml_child( ps_xml_script_in, "command" );
   while( NULL != ps_xml_command ) {
      *i_count_out += 1;

      pc_command_action = ezxml_attr( ps_xml_command, "action" );
      bassignformat(
         ps_command_action,
         "%s",
         pc_command_action
      );
      if( NULL == pc_command_action ) {
         /* If the action isn't defined then don't even bother. */
         DBG_ERR( "Invalid script command found: No action specified." );
         continue;
      }

      /* Allocate the new command item at the end of the list. */
      ps_commands_out = (SYSTYPE_VISNOV_COMMAND*)realloc(
         ps_commands_out,
         *i_count_out * sizeof( SYSTYPE_VISNOV_COMMAND )
      );
      if( NULL == ps_commands_out ) {
         DBG_ERR( "Unable to allocate commands list." );
         goto stvnlc_cleanup;
      }
      memset(
         &ps_commands_out[*i_count_out - 1],
         0,
         sizeof( SYSTYPE_VISNOV_COMMAND )
      );

      /* Parse the command's data and opcode. */
      if( 0 == strcmp( pc_command_action, "background" ) ) {
         /* COMMAND: BACKGROUND */
         ps_commands_out[*i_count_out - 1].command =
            SYSTYPE_VISNOV_CMD_BACKGROUND;
         ps_commands_out[*i_count_out - 1].data = calloc(
            SYSTYPE_VISNOV_CMD_BACKGROUND_DC,
            sizeof( SYSTYPE_VISNOV_DATA )
         );
         bassignformat(
            ps_command_attr,
            "%svnbg_%s.%s",
            PATH_SHARE,
            ezxml_attr( ps_xml_command, "bg" ),
            FILE_EXTENSION_IMAGE
         );
         ps_commands_out[*i_count_out - 1].data[0].bg =
            graphics_create_image( ps_command_attr );

      } else if( 0 == strcmp( pc_command_action, "pause" ) ) {
         /* COMMAND: PAUSE */
         ps_commands_out[*i_count_out - 1].command =
            SYSTYPE_VISNOV_CMD_PAUSE;
         ps_commands_out[*i_count_out - 1].data = calloc(
            SYSTYPE_VISNOV_CMD_PAUSE_DC,
            sizeof( SYSTYPE_VISNOV_DATA )
         );
         bassignformat(
            ps_command_attr,
            "%s",
            ezxml_attr( ps_xml_command, "delay" )
         );
         ps_commands_out[*i_count_out - 1].data[0].delay =
            atoi( ps_command_attr->data );

      } else if( 0 == strcmp( pc_command_action, "label" ) ) {
         /* COMMAND: LABEL */
         ps_commands_out[*i_count_out - 1].command =
            SYSTYPE_VISNOV_CMD_LABEL;
         ps_commands_out[*i_count_out - 1].data = calloc(
            SYSTYPE_VISNOV_CMD_LABEL_DC,
            sizeof( SYSTYPE_VISNOV_DATA )
         );
         bassignformat(
            ps_command_attr,
            "%s",
            ezxml_attr( ps_xml_command, "name" )
         );
         ps_commands_out[*i_count_out - 1].data[0].name =
            bstrcpy( ps_command_attr );

      }

      DBG_INFO_STR( "Command added", ps_command_action->data );

      /* Go on to the next one. */
      ps_xml_command = ezxml_next( ps_xml_command );
   }

stvnlc_cleanup:

   bdestroy( ps_command_action );
   bdestroy( ps_command_attr );

   return ps_commands_out;
}

/* Purpose: Execute the given command.                                        */
/* Parameters: A pointer to the command struct to execute and a pointer to    *
 *             the calling loop's command index cursor.                       */
void systype_visnov_exec_command(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   int* pi_command_cursor_in,
   SYSTYPE_VISNOV_SCENE* ps_scene_in
) {

}

/* Purpose: Free the memory held by the given command struct's data elements  *
 *          (But DON'T free the pointer to the struct itself!)                */
/* Parameters: A pointer to the command struct to free.                       */
void systype_visnov_free_command_arr( SYSTYPE_VISNOV_COMMAND* ps_command_in ) {
   if( NULL == ps_command_in ) {
      return; /* Don't bother if it's already free. */
   }

   /* We know which data items are present by the command present. */
   switch( ps_command_in->command ) {
      case SYSTYPE_VISNOV_CMD_BACKGROUND:
         graphics_free_image( ps_command_in->data[0].bg );
         break;

      case SYSTYPE_VISNOV_CMD_PAUSE:
         break;

      case SYSTYPE_VISNOV_CMD_LABEL:
         bdestroy( ps_command_in->data[0].name );
         break;

      case SYSTYPE_VISNOV_CMD_COND:
         bdestroy( ps_command_in->data[0].key );
         bdestroy( ps_command_in->data[1].equals );
         break;

      case SYSTYPE_VISNOV_CMD_TALK:
         bdestroy( ps_command_in->data[1].talktext );
         break;

      case SYSTYPE_VISNOV_CMD_GOTO:
         bdestroy( ps_command_in->data[0].target );
         break;

      case SYSTYPE_VISNOV_CMD_PORTRAIT:
         break;

      case SYSTYPE_VISNOV_CMD_TELEPORT:
         bdestroy( ps_command_in->data[0].destmap );
         break;
   }
}
