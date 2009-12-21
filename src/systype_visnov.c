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

/* = Functions = */

/* Purpose: Visual novel loop.                                                */
/* Parameters: The name of the scene to play.                                 */
/* Return: The code for the next action to take.                              */
int systype_visnov_loop( CACHE_CACHE* ps_cache_in ) {
   SYSTYPE_VISNOV_ACTOR* as_actors = NULL;
   SYSTYPE_VISNOV_COMMAND* as_commands = NULL;
   WINDOW_MENU* as_menus = NULL;
   int i_actors_count = 0,
      i_commands_count = 0,
      i_menus_count = 0,
      i_act_return = RETURN_ACTION_TITLE,
      i_command_cursor = 0, /* The index of the command to execute next. */
      i, j; /* Loop iterators. */
   GFX_COLOR* ps_color_fade;
   GFX_RECTANGLE s_rect_actor;
   bstring ps_scene_path = NULL;
   ezxml_t ps_xml_scene = NULL;
   EVENT_EVENT s_event;
   SYSTYPE_VISNOV_SCENE s_scene;

   /* Verify the XML file exists and open or abort accordingly. */
   ps_scene_path =
      bformat( "%sscene_%s.xml", PATH_SHARE , ps_cache_in->map_name->data );
   if( !file_exists( ps_scene_path ) ) {
      DBG_ERR_STR( "Unable to load scene data", ps_scene_path->data );
      i_act_return = RETURN_ACTION_TITLE;
      goto stvnl_cleanup;
   }
   ps_xml_scene = ezxml_parse_file( (const char*)ps_scene_path->data );

   /* Initialize what we need to use functions to initialize. */
   memset( &s_scene, 0, sizeof( SYSTYPE_VISNOV_SCENE ) );
   memset( &s_rect_actor, 0, sizeof( GFX_RECTANGLE ) );
   memset( &s_event, 0, sizeof( EVENT_EVENT ) );
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
      event_do_poll( &s_event, FALSE );
      /* Execute the next command unless we're waiting. */
      switch( as_commands[i_command_cursor].command ) {
         case SYSTYPE_VISNOV_CMD_BACKGROUND:
            i_command_cursor = systype_visnov_exec_background(
               &as_commands[i_command_cursor], &s_scene, i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_PAUSE:
            i_command_cursor = systype_visnov_exec_pause(
               &as_commands[i_command_cursor], i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_COND:
            i_command_cursor = systype_visnov_exec_cond(
               &as_commands[i_command_cursor], &s_scene, i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_TALK:
            i_command_cursor = systype_visnov_exec_talk(
               &as_commands[i_command_cursor], ps_cache_in, &s_event,
               as_actors, i_actors_count, i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_PORTRAIT:
            i_command_cursor = systype_visnov_exec_portrait(
               &as_commands[i_command_cursor], &s_scene, as_actors,
               i_actors_count, i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_MENU:
            i_command_cursor = systype_visnov_exec_menu(
               &as_commands[i_command_cursor], ps_cache_in, &s_event,
               &as_menus, &i_menus_count, i_command_cursor
            );
            break;

         default:
            /* Just skip commands we don't understand yet. */
            i_command_cursor++;
      }

      if( s_event.state[EVENT_ID_ESC] || s_event.state[EVENT_ID_QUIT] ) {
         ps_cache_in->game_type = SYSTEM_TYPE_TITLE;
         goto stvnl_cleanup;
      }

      /* Draw the on-screen items. */
      if( NULL != s_scene.bg ) {
         graphics_draw_blit_tile( s_scene.bg, NULL, NULL );
      }
      for( i = 0 ; i < s_scene.actors_onscreen_count ; i++ ) {
         j = s_scene.actors_onscreen[i]->emotion_current;
         s_rect_actor.x = s_scene.actors_onscreen[i]->x;
         s_rect_actor.y = s_scene.actors_onscreen[i]->y;
         s_rect_actor.w = s_scene.actors_onscreen[i]->emotions[j].image->w;
         s_rect_actor.h = s_scene.actors_onscreen[i]->emotions[j].image->h;
         graphics_draw_blit_tile(
            s_scene.actors_onscreen[i]->emotions[j].image,
            NULL,
            &s_rect_actor
         );
      }
      window_draw_text( 0, ps_cache_in );
      window_draw_menu( as_menus, i_menus_count );
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
   for( i = 0 ; i < i_actors_count ; i++ ) {
      systype_visnov_free_actor_arr( &as_actors[i] );
   }
   free( as_actors );
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
         UTIL_ARRAY_ADD(
            SYSTYPE_VISNOV_EMOTION, s_actor_tmp.emotions,
            s_actor_tmp.emotions_count, stvnla_cleanup, &s_emotion_tmp
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
      UTIL_ARRAY_ADD(
         SYSTYPE_VISNOV_ACTOR, ps_actors_out, *pi_count_out, stvnla_cleanup,
         &s_actor_tmp
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
   SYSTYPE_VISNOV_COMMAND s_command_tmp;

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
      memset( &s_command_tmp, 0, sizeof( SYSTYPE_VISNOV_COMMAND ) );

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

      /* Parse the command's data and opcode. See the systype_visnov.h header    *
       * for information on which Data Index (DI) belongs to which command and   *
       * data item for that command, although that much should be evident from   *
       * the list below. At any rate, be aware of the header and be sure to      *
       * update it accordingly when adding new items.                            */
      if( 0 == strcmp( pc_command_action, "background" ) ) {
         /* COMMAND: BACKGROUND */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_BACKGROUND, SYSTYPE_VISNOV_CMD_BACKGROUND_DC );
         bassignformat(
            ps_command_attr,
            "%svnbg_%s.%s", PATH_SHARE, ezxml_attr( ps_xml_command, "bg" ),
            FILE_EXTENSION_IMAGE
         );
         s_command_tmp.data[0].bg = graphics_create_image( ps_command_attr );

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
         STVN_PARSE_CMD_DAT_INT( scope, 1 ); /* TODO: Parse the scope. */
         STVN_PARSE_CMD_DAT_STR( key, 2 );
         STVN_PARSE_CMD_DAT_STR( equals, 3 );

      } else if( 0 == strcmp( pc_command_action, "talk" ) ) {
         /* COMMAND: TALK */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_TALK, SYSTYPE_VISNOV_CMD_TALK_DC );
         STVN_PARSE_CMD_DAT_INT( serial, 0 );
         STVN_PARSE_CMD_DAT_STR_BODY( talktext, 1 );
         STVN_PARSE_CMD_DAT_INT( speed, 2 );

      } else if( 0 == strcmp( pc_command_action, "goto" ) ) {
         /* COMMAND: GOTO */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_GOTO, SYSTYPE_VISNOV_CMD_GOTO_DC );
         STVN_PARSE_CMD_DAT_STR( target, 0 );

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

      } else if( 0 == strcmp( pc_command_action, "menu" ) ) {
         /* COMMAND: MENU */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_MENU, SYSTYPE_VISNOV_CMD_MENU_DC );
         STVN_PARSE_CMD_DAT_STR( items, 0 );
         STVN_PARSE_CMD_DAT_INT( scope, 1 ); /* TODO: Parse the scope. */
         STVN_PARSE_CMD_DAT_COL( color_fg, 2 );
         STVN_PARSE_CMD_DAT_COL( color_bg, 3 );
         STVN_PARSE_CMD_DAT_COL( color_sfg, 4 );
         STVN_PARSE_CMD_DAT_COL( color_sbg, 5 );

      }

      /* Allocate the new command item at the end of the list. */
      UTIL_ARRAY_ADD(
         SYSTYPE_VISNOV_COMMAND, ps_commands_out, *pi_count_out, stvnlc_cleanup,
         &s_command_tmp
      );

      DBG_INFO_STR( "Command added", ps_command_action->data );

      /* Go on to the next one. */
      ps_xml_command = ezxml_next( ps_xml_command );
   }

stvnlc_cleanup:

   bdestroy( ps_command_action );
   bdestroy( ps_command_attr );

   return ps_commands_out;
}

/* COMMAND: BACKGROUND */
int systype_visnov_exec_background(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   SYSTYPE_VISNOV_SCENE* ps_scene_in,
   int i_command_cursor_in
) {
   GFX_COLOR* ps_color_fade = NULL;

   ps_color_fade = graphics_create_color( 0, 0, 0 );
   ps_scene_in->bg = ps_command_in->data->bg;
   graphics_draw_blit_tile( ps_scene_in->bg, NULL, NULL );
   graphics_draw_transition( GFX_TRANS_FADE_IN, ps_color_fade );
   free( ps_color_fade );

   return ++i_command_cursor_in;
}

/* COMMAND: PAUSE */
int systype_visnov_exec_pause(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   int i_command_cursor_in
) {
   static int ti_countdown = -1;

   /* If we're still counting down then count down one more, reverse the      *
    * calling execution cursor, and quit.                                     */
   if( 0 <= ti_countdown ) {
      ti_countdown--;
      if( 0 <= ti_countdown ) {
         /* This isn't the last tick, so keep waiting. */
         return i_command_cursor_in;
      } else {
         /* Time can move again. */
         return ++i_command_cursor_in;
      }
   } else {
      ti_countdown = ps_command_in->data[0].delay;
      return i_command_cursor_in;
   }
}

/* COMMAND: COND */
int systype_visnov_exec_cond(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   SYSTYPE_VISNOV_SCENE* ps_scene_in,
   int i_command_cursor_in
) {
   int i; /* Loop iterator. */

   for( i = 0 ; i < ps_scene_in->var_count ; i++ ) {
      if(
         (COND_SCOPE_LOCAL == ps_command_in->data[1].scope &&
         0 == bstrcmp( ps_command_in->data[2].key, &ps_scene_in->var_keys[i] ) &&
         0 == bstrcmp( ps_command_in->data[3].equals, &ps_scene_in->var_values[i] ))
      ) {
         /* TODO */
      }
   }

   return ++i_command_cursor_in;
}

/* COMMAND: TALK */
int systype_visnov_exec_talk(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   CACHE_CACHE* ps_cache_in,
   EVENT_EVENT* ps_event_in,
   SYSTYPE_VISNOV_ACTOR* as_actors_in,
   int i_actors_count_in,
   int i_command_cursor_in
) {
   bstring ps_talk_text = NULL, ps_formatted_talk = NULL;
   static CACHE_LOG_ENTRY* tps_last_text = NULL;

   /* If the window we're about to create is already on the stack, then just  *
    * reset the command cursor back one and quit.                             */
   if(
      0 < ps_cache_in->text_log_count &&
      tps_last_text == &ps_cache_in->text_log[ps_cache_in->text_log_count - 1]
   ) {
      /* The last entry created is the entry currently displaying. */
      if( ps_event_in->state[EVENT_ID_FIRE] ) {
         /* The player wants to advance. */
         tps_last_text = NULL;
         return ++i_command_cursor_in;
      } else {
         return i_command_cursor_in;
      }
   }

   /* Format and prettify the text for the window. */
   ps_formatted_talk = bstrcpy( ps_command_in->data[1].talktext );
   btrimws( ps_formatted_talk );

   ps_talk_text = bformat(
      "%s: %s",
      systype_visnov_get_actor(
         ps_command_in->data[0].serial,
         as_actors_in,
         i_actors_count_in
      )->name->data,
      ps_formatted_talk->data
   );

   /* Actually display the text window. */
   ps_cache_in->text_log = window_create_text(
      ps_talk_text, ps_cache_in->text_log, &ps_cache_in->text_log_count
   );
   tps_last_text = &ps_cache_in->text_log[ps_cache_in->text_log_count - 1];

   /* Clean up. */
   bdestroy( ps_formatted_talk );
   bdestroy( ps_talk_text );

   return i_command_cursor_in;
}

/* COMMAND: PORTRAIT */
int systype_visnov_exec_portrait(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   SYSTYPE_VISNOV_SCENE* ps_scene_in,
   SYSTYPE_VISNOV_ACTOR* as_actors_in,
   int i_actors_count_in,
   int i_command_cursor_in
) {

   int i, z; /* Loop iterators. */

   /* If the given actor is already in the scene, remove them. */
   for( i = 0 ; i < ps_scene_in->actors_onscreen_count ; i++ ) {
      if(
         ps_scene_in->actors_onscreen[i]->serial ==
         ps_command_in->data[0].serial
      ) {
         UTIL_ARRAY_DEL(
            SYSTYPE_VISNOV_ACTOR*, ps_scene_in->actors_onscreen,
            ps_scene_in->actors_onscreen_count, stvnepr_cleanup, i
         );
      }
   }

   /* Add the given emotion portrait to the scene. */
   UTIL_ARRAY_ADD(
      SYSTYPE_VISNOV_ACTOR*, ps_scene_in->actors_onscreen,
      ps_scene_in->actors_onscreen_count, stvnepr_cleanup, NULL
   );
   ps_scene_in->actors_onscreen[ps_scene_in->actors_onscreen_count - 1] =
      systype_visnov_get_actor(
         ps_command_in->data[0].serial,
         as_actors_in,
         i_actors_count_in
      );
   ps_scene_in->actors_onscreen[ps_scene_in->actors_onscreen_count - 1]->
      emotion_current = ps_command_in->data[1].emotion;
   ps_scene_in->actors_onscreen[ps_scene_in->actors_onscreen_count - 1]->
      x = ps_command_in->data[3].x;
   ps_scene_in->actors_onscreen[ps_scene_in->actors_onscreen_count - 1]->
            y = ps_command_in->data[4].y;

stvnepr_cleanup:

   return ++i_command_cursor_in;
}

/* COMMAND: MENU */
/* NOTE: This command modifies the list of menus, so a pointer to the pointer *
 *       to the list is expected in case realloc changes the list address.    */
int systype_visnov_exec_menu(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   CACHE_CACHE* ps_cache_in,
   EVENT_EVENT* ps_event_in,
   WINDOW_MENU** pas_menu_list_in,
   int* pi_menu_list_count_in,
   int i_command_cursor_in
) {
   static WINDOW_MENU* tps_last_menu = NULL;
   WINDOW_MENU_COLORS s_colors_tmp;
   int z;

   /* If the menu we're about to create is already on the stack, then just    *
    * reset the command cursor back one and quit.                             */
   if(
      0 < *pi_menu_list_count_in &&
      tps_last_menu == &(*pas_menu_list_in)[*pi_menu_list_count_in - 1]
   ) {
      /* The last menu created is the menu currently displaying. */
      if( ps_event_in->state[EVENT_ID_FIRE] ) {
         /* The player wants to advance. */
         cache_set_var(
            tps_last_menu->options[tps_last_menu->selected].key,
            tps_last_menu->options[tps_last_menu->selected].value,
            tps_last_menu->scope,
            ps_cache_in
         );
         UTIL_ARRAY_DEL(
            WINDOW_MENU, *pas_menu_list_in, *pi_menu_list_count_in,
            stvnem_cleanup, (*pi_menu_list_count_in - 1)
         );
         tps_last_menu = NULL;

         i_command_cursor_in++;

         goto stvnem_cleanup;

      } else if( ps_event_in->state[EVENT_ID_DOWN] ) {
         /* Move the menu cursor down. */
         if( tps_last_menu->selected < tps_last_menu->options_count - 1 ) {
            tps_last_menu->selected++;
         } else {
            tps_last_menu->selected = 0;
         }
         return i_command_cursor_in;

      } else if( ps_event_in->state[EVENT_ID_UP] ) {
         /* Move the menu cursor up. */
         if( tps_last_menu->selected > 0 ) {
            tps_last_menu->selected--;
         } else {
            tps_last_menu->selected = tps_last_menu->options_count - 1;
         }
         return i_command_cursor_in;

      } else {
         /* No command, so hold our position. */
         return i_command_cursor_in;

      }
   }

   /* Setup the new menu colors. */
   memcpy( &s_colors_tmp.fg, ps_command_in->data[2].color_fg, sizeof( GFX_COLOR ) );
   memcpy( &s_colors_tmp.bg, ps_command_in->data[3].color_bg, sizeof( GFX_COLOR ) );
   memcpy( &s_colors_tmp.sfg, ps_command_in->data[4].color_sfg, sizeof( GFX_COLOR ) );
   memcpy( &s_colors_tmp.sbg, ps_command_in->data[5].color_sbg, sizeof( GFX_COLOR ) );

   /* Append the menu struct and clean up. It's all right to just free  *
    * it since the dynamic stuff pointed to it will be pointed to by    *
    * the copy on the menu stack.                                       */
   (*pas_menu_list_in) = window_create_menu(
      ps_command_in->data[0].items, ps_command_in->data[1].scope, &s_colors_tmp,
      *pas_menu_list_in, pi_menu_list_count_in
   );
   tps_last_menu = &(*pas_menu_list_in)[(*pi_menu_list_count_in) - 1];

stvnem_cleanup:

   return i_command_cursor_in;
}

/* Purpose: Return a pointer to the first actor with a given serial number.   */
SYSTYPE_VISNOV_ACTOR* systype_visnov_get_actor(
   int i_serial_in,
   SYSTYPE_VISNOV_ACTOR* as_actors_in,
   int i_actors_count_in
) {
   int i; /* Loop iterator. */

   for( i = 0 ; i < i_actors_count_in ; i++ ) {
      if( as_actors_in[i].serial == i_serial_in ) {
         return &as_actors_in[i];
      }
   }

   /* Actor wasn't found. */
   return NULL;
}

/* Purpose: Free the memory held by the given actor's attributes.             *
 *          (But DON'T free the pointer to the struct itself!)                */
/* Parameters: A pointer to the actor struct to free.                         */
void systype_visnov_free_actor_arr( SYSTYPE_VISNOV_ACTOR* ps_actor_in ) {
   int i; /* Loop iterator. */

   if( NULL == ps_actor_in ) {
      return;
   }

   for( i = 0 ; i < ps_actor_in->emotions_count ; i++ ) {
      graphics_free_image( ps_actor_in->emotions[i].image );
   }
   free( ps_actor_in->emotions );

   DBG_INFO_INT( "Actor freed", ps_actor_in->serial );
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
         bdestroy( ps_command_in->data[0].target );
         bdestroy( ps_command_in->data[2].key );
         bdestroy( ps_command_in->data[3].equals );
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

      case SYSTYPE_VISNOV_CMD_MENU:
         bdestroy( ps_command_in->data[0].items );
         free( ps_command_in->data[2].color_fg );
         free( ps_command_in->data[3].color_bg );
         free( ps_command_in->data[4].color_sfg );
         free( ps_command_in->data[5].color_sbg );
         break;
   }
}
