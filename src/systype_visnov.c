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
   SYSTYPE_VISNOV_ACTOR* as_actors = NULL;
   SYSTYPE_VISNOV_COMMAND* as_commands = NULL;
   int i_actors_count = 0,
      i_commands_count = 0,
      i_act_return = RETURN_ACTION_TITLE,
      i_command_cursor = 0, /* The index of the command to execute next. */
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
   memset( &s_scene, 0, sizeof( SYSTYPE_VISNOV_SCENE ) );
   ps_color_fade = graphics_create_color( 0, 0, 0 );
   as_commands = systype_visnov_load_commands(
      &i_commands_count,
      ezxml_child( ps_xml_scene, "script" )
   );
   as_actors = systype_visnov_load_actors(
      &i_actors_count,
      ezxml_child( ps_xml_scene, "actors" )
   );

   while( 1 ) {
      GFX_DRAW_LOOP_START

      /* Listen for events. */
      event_do_poll( &s_event, TRUE );
      if( s_event.state[EVENT_ID_FIRE] && b_wait_for_talk ) {
         b_wait_for_talk = FALSE;
      }

      /* Execute the next command unless we're waiting. */
      if( !b_wait_for_talk && i_command_cursor < i_commands_count ) {
         systype_visnov_exec_command(
            &as_commands[i_command_cursor], &i_command_cursor, &s_scene,
            as_actors, i_actors_count
         );
      }
      if( s_event.state[EVENT_ID_ESC] || s_event.state[EVENT_ID_QUIT] ) {
         gps_cache->game_type = SYSTEM_TYPE_TITLE;
         goto stvnl_cleanup;
      }

      /* Draw the on-screen items. */
      if( NULL != s_scene.bg ) {
         graphics_draw_blit_tile( s_scene.bg, NULL, NULL );
      }
      graphics_do_update();

      GFX_DRAW_LOOP_END
   }

stvnl_cleanup:

   /* Fade out the playing map screen. */
   graphics_draw_transition( GFX_TRANS_FADE_OUT, ps_color_fade );

   /* Clean up! */
   free( ps_color_fade );

   ezxml_free( ps_xml_scene );
   bdestroy( ps_scene_path );
   for( i = 0 ; i < i_commands_count ; i++ ) {
      systype_visnov_free_command_arr( &as_commands[i] );
   }
   free( as_commands );

   return i_act_return;
}

/* Purpose: Load the array of scene actors.                                   */
/* Parameters: The address of the actor array size indicator.                 */
/* Return: A pointer to the actor array.                                      */
SYSTYPE_VISNOV_ACTOR* systype_visnov_load_actors(
   int* pi_count_out,
   ezxml_t ps_xml_actors_in
) {
   SYSTYPE_VISNOV_ACTOR* ps_actors_out = NULL;
   SYSTYPE_VISNOV_EMOTION s_emotion_tmp; /* A temp emotion iterator. */
   SYSTYPE_VISNOV_ACTOR s_actor_tmp;
   ezxml_t ps_xml_actor = NULL, ps_xml_emotion = NULL;
   const char* pc_attr = NULL;
   bstring ps_attr_temp = bfromcstr( "" );

   /* Verify that the script exists. */
   if( NULL == ps_xml_actors_in ) {
      DBG_ERR( "Invalid visual novel actor list detected." );
      goto stvnla_cleanup;
   }

   /* Initialize what we need to. */
   *pi_count_out = 0;

   /* Load each command in order. */
   ps_xml_actor = ezxml_child( ps_xml_actors_in, "actor" );
   while( NULL != ps_xml_actor ) {
      /* Allocate the new actor item at the end of the list. */
      UTIL_ARRAY_ALLOC(
         SYSTYPE_VISNOV_ACTOR, ps_actors_out, *pi_count_out, stvnla_cleanup
      );

      memset( &s_actor_tmp, 0, sizeof( SYSTYPE_VISNOV_ACTOR ) );

      /* ATTRIB: SERIAL */
      pc_attr = ezxml_attr( ps_xml_actor, "serial" );
      if( NULL == pc_attr ) {
         /* There's no serial, so this actor is useless. */
         DBG_ERR( "Invalid actor serial detected." );
         ps_xml_actor = ezxml_next( ps_xml_actor );
         continue;
      }
      s_actor_tmp.serial = atoi( pc_attr );

      /* ATTRIB: NAME */
      pc_attr = ezxml_attr( ps_xml_actor, "name" );
      if( NULL == pc_attr ) {
         /* There's no name, so this actor is useless. */
         DBG_ERR_INT(
            "Invalid actor name detected for actor", s_actor_tmp.serial
         );
         ps_xml_actor = ezxml_next( ps_xml_actor );
         continue;
      }
      s_actor_tmp.name = bformat( "%s", pc_attr );

      /* Build the actor's list of emotions and their portraits. */
      ps_xml_emotion = ezxml_child( ps_xml_actor, "emotion" );
      while( NULL != ps_xml_emotion ) {
         memset( &s_emotion_tmp, 0, sizeof( SYSTYPE_VISNOV_EMOTION ) );

         /* ATTRIB: ID */
         pc_attr = ezxml_attr( ps_xml_emotion, "id" );
         if( NULL == pc_attr ) {
            /* There's no ID, so this emotion is useless. */
            DBG_ERR_INT(
               "ID not found; invalid emotion for actor",
               s_actor_tmp.serial
            );
            ps_xml_emotion = ezxml_next( ps_xml_emotion );
            continue;
         }
         s_emotion_tmp.id = atoi( pc_attr );

         /* ATTRIB: IMAGE */
         pc_attr = ezxml_txt( ps_xml_emotion );
         bassignformat(
            ps_attr_temp,
            "%svnprt_%s_%s.%s", PATH_SHARE, s_actor_tmp.name->data,
               pc_attr, FILE_EXTENSION_IMAGE
         );
         btolower( ps_attr_temp );
         s_emotion_tmp.image = graphics_create_image( ps_attr_temp );
         if( NULL == pc_attr || NULL == s_emotion_tmp.image ) {
            /* There's no image, so this emotion is useless. */
            DBG_ERR_INT(
               "Portrait not found; invalid emotion for actor",
               s_actor_tmp.serial
            );
            ps_xml_emotion = ezxml_next( ps_xml_emotion );
            continue;
         }

         /* If we've made it this far, the emotion is probably valid, so add  *
          * it to the current actor's list.                                   */
         UTIL_ARRAY_ALLOC(
            SYSTYPE_VISNOV_EMOTION, s_actor_tmp.emotions,
            s_actor_tmp.emotions_count, stvnla_cleanup
         );
         memcpy(
            &s_actor_tmp.emotions[s_actor_tmp.emotions_count - 1],
            &s_emotion_tmp,
            sizeof( SYSTYPE_VISNOV_EMOTION )
         );

         DBG_INFO_INT_INT(
            "Loaded emotion (serial, emotion)",
            s_actor_tmp.serial,
            s_emotion_tmp.id
         );

         /* Go on to the next one. */
         ps_xml_emotion = ezxml_next( ps_xml_emotion );
      }

      /* If we've made it this far, the emotion is probably valid, so add  *
       * it to the current actor's list.                                   */
      UTIL_ARRAY_ALLOC(
         SYSTYPE_VISNOV_ACTOR, ps_actors_out, *pi_count_out, stvnla_cleanup
      );
      memcpy(
         &ps_actors_out[*pi_count_out - 1],
         &s_actor_tmp,
         sizeof( SYSTYPE_VISNOV_ACTOR )
      );

      DBG_INFO_STR_INT(
         "Loaded actor (name, serial)",
         s_actor_tmp.name->data,
         s_actor_tmp.serial
      );

      /* Go on to the next one. */
      ps_xml_actor = ezxml_next( ps_xml_actor );
   }

stvnla_cleanup:

   bdestroy( ps_attr_temp );

   return ps_actors_out;
}

/* Purpose: Load the array of scene commands.                                 */
/* Parameters: The address of the command array size indicator.               */
/* Return: A pointer to the command array.                                    */
SYSTYPE_VISNOV_COMMAND* systype_visnov_load_commands(
   int* pi_count_out,
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
   *pi_count_out = 0;

   /* Load each command in order. */
   ps_xml_command = ezxml_child( ps_xml_script_in, "command" );
   while( NULL != ps_xml_command ) {
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
      UTIL_ARRAY_ALLOC(
         SYSTYPE_VISNOV_COMMAND, ps_commands_out, *pi_count_out, stvnlc_cleanup
      );

      /* Parse the command's data and opcode. */
      if( 0 == strcmp( pc_command_action, "background" ) ) {
         /* COMMAND: BACKGROUND */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_BACKGROUND, SYSTYPE_VISNOV_CMD_BACKGROUND_DC );
         bassignformat(
            ps_command_attr,
            "%svnbg_%s.%s", PATH_SHARE, ezxml_attr( ps_xml_command, "bg" ),
            FILE_EXTENSION_IMAGE
         );
         ps_commands_out[*pi_count_out - 1].data[0].bg =
            graphics_create_image( ps_command_attr );

      } else if( 0 == strcmp( pc_command_action, "pause" ) ) {
         /* COMMAND: PAUSE */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_PAUSE, SYSTYPE_VISNOV_CMD_PAUSE_DC );
         STVN_PARSE_CMD_DAT_INT( delay, 0 );

      } else if( 0 == strcmp( pc_command_action, "label" ) ) {
         /* COMMAND: LABEL */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_LABEL, SYSTYPE_VISNOV_CMD_LABEL_DC );
         STVN_PARSE_CMD_DAT_STR( name, 0 );

      } else if( 0 == strcmp( pc_command_action, "cond" ) ) {
         /* COMMAND: COND */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_COND, SYSTYPE_VISNOV_CMD_COND_DC );
         STVN_PARSE_CMD_DAT_STR( target, 0 );
         STVN_PARSE_CMD_DAT_STR( key, 1 );
         STVN_PARSE_CMD_DAT_STR( equals, 2 );
         STVN_PARSE_CMD_DAT_INT( scope, 3 ); /* TODO: Parse the scope. */

      } else if( 0 == strcmp( pc_command_action, "label" ) ) {
         /* COMMAND: TALK */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_TALK, SYSTYPE_VISNOV_CMD_TALK_DC );
         STVN_PARSE_CMD_DAT_INT( serial, 0 );
         STVN_PARSE_CMD_DAT_STR( talktext, 1 );
         STVN_PARSE_CMD_DAT_INT( speed, 2 );

      } else if( 0 == strcmp( pc_command_action, "label" ) ) {
         /* COMMAND: GOTO */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_GOTO, SYSTYPE_VISNOV_CMD_GOTO_DC );
         STVN_PARSE_CMD_DAT_INT( serial, 0 );
         STVN_PARSE_CMD_DAT_STR( talktext, 1 );
         STVN_PARSE_CMD_DAT_INT( speed, 2 );

      } else if( 0 == strcmp( pc_command_action, "portrait" ) ) {
         /* COMMAND: PORTRAIT */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_PORTRAIT, SYSTYPE_VISNOV_CMD_PORTRAIT_DC );
         STVN_PARSE_CMD_DAT_INT( serial, 0 );
         STVN_PARSE_CMD_DAT_INT( emotion, 1 );
         STVN_PARSE_CMD_DAT_FLT( zoom, 2 );
         STVN_PARSE_CMD_DAT_INT( x, 3 );
         STVN_PARSE_CMD_DAT_INT( y, 4 );

      } else if( 0 == strcmp( pc_command_action, "teleport" ) ) {
         /* COMMAND: TELEPORT */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_TELEPORT, SYSTYPE_VISNOV_CMD_TELEPORT_DC );
         STVN_PARSE_CMD_DAT_STR( destmap, 0 );
         STVN_PARSE_CMD_DAT_INT( destx, 1 );
         STVN_PARSE_CMD_DAT_INT( desty, 2 );
         STVN_PARSE_CMD_DAT_INT( type, 3 ); /* TODO: Parse the type. */

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
   SYSTYPE_VISNOV_SCENE* ps_scene_in,
   SYSTYPE_VISNOV_ACTOR* as_actors_in,
   int i_actors_count_in
) {
   GFX_COLOR* ps_color_fade = NULL;
   static int ti_countdown = -1;
   int i; /* Loop iterator. */

   switch( ps_command_in->command ) {
      case SYSTYPE_VISNOV_CMD_BACKGROUND:
         ps_color_fade = graphics_create_color( 0, 0, 0 );
         ps_scene_in->bg = ps_command_in->data->bg;
         graphics_draw_blit_tile( ps_scene_in->bg, NULL, NULL );
         graphics_draw_transition( GFX_TRANS_FADE_IN, ps_color_fade );
         free( ps_color_fade );
         break;

      case SYSTYPE_VISNOV_CMD_PAUSE:
         /* If we're still counting down then count down one more and quit. */
         if( 0 <= ti_countdown ) {
            ti_countdown--;
            if( 0 <= ti_countdown ) {
               /* This isn't the last tick, so skip the instruction incr. */
               goto stvnec_cleanup;
            }
         } else {
            ti_countdown = ps_command_in->data[0].delay;
         }
         break;

      case SYSTYPE_VISNOV_CMD_COND:
         for( i = 0 ; i < ps_scene_in->var_count ; i++ ) {
            if(
               (COND_SCOPE_LOCAL == ps_command_in->data[3].scope &&
               0 == bstrcmp( ps_command_in->data[1].key, &ps_scene_in->var_keys[i] ) &&
               0 == bstrcmp( ps_command_in->data[2].equals, &ps_scene_in->var_values[i] ))
            ) {

            }
         }
         break;
   }

   /* This command was executed successfully, so go on to the next one. */
   (*pi_command_cursor_in)++;

stvnec_cleanup:

   return;
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
