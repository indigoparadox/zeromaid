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
TITLE_ERROR_ENABLE
GFX_DRAW_LOOP_ENABLE

/* = Functions = */

/* Purpose: Visual novel loop.                                                */
/* Parameters: The name of the scene to play.                                 */
/* Return: The code for the next action to take.                              */
int systype_visnov_loop( CACHE_CACHE* ps_cache_in ) {
   CACHE_VARIABLE* as_locals = NULL;
   MOBILE_MOBILE** aps_actors_onscreen = NULL; /* Dupe ptrs to actors on-screen. */
   MOBILE_MOBILE* as_actors = NULL;
   SYSTYPE_VISNOV_COMMAND* as_commands = NULL;
   int i_actors_count = 0,
      i_commands_count = 0,
      i_act_return = RETURN_ACTION_TITLE,
      i_command_cursor = 0, /* The index of the command to execute next. */
      i, j, /* Loop iterators. */
      i_locals_count = 0, /* Local variable count. */
      i_actors_onscreen_count = 0;
   GFX_COLOR* ps_color_fade = NULL;
   GFX_RECTANGLE s_rect_actor;
   bstring ps_scene_path = NULL;
   ezxml_t ps_xml_scene = NULL;
   EVENT_EVENT s_event;
   BOOL b_teleport = FALSE, /* Was the last teleport command successful? */
      b_commands_loaded,
      b_actors_loaded;
   WINDOW_MENU* ps_menu = NULL;
   GFX_SURFACE* ps_bg = NULL; /* Current scene background. */

   TITLE_ERROR_CLEAR();

   /* Verify the XML file exists and open or abort accordingly. */
   ps_scene_path =
      bformat( "%s%s.xml", PATH_SHARE , ps_cache_in->map_name->data );
   if( !zm_file_exists( ps_scene_path ) ) {
      DBG_ERR_STR( "Unable to load scene data", ps_scene_path->data );
      i_act_return = RETURN_ACTION_TITLE;
      goto stvnl_cleanup;
   }
   ps_xml_scene = ezxml_parse_file( (const char*)ps_scene_path->data );

   /* Initialize what we need to use functions to initialize. */
   memset( &s_rect_actor, 0, sizeof( GFX_RECTANGLE ) );
   memset( &s_event, 0, sizeof( EVENT_EVENT ) );
   ps_color_fade = graphics_create_color( 0, 0, 0 );
   b_commands_loaded = systype_visnov_load_commands(
      &as_commands,
      &i_commands_count,
      ezxml_child( ps_xml_scene, "script" )
   );
   b_actors_loaded = mobile_load_mobiles(
      &as_actors,
      &i_actors_count,
      ezxml_child( ps_xml_scene, "mobiles" ),
      NULL
   );

   if( !b_commands_loaded || !b_actors_loaded ) {
      DBG_ERR( "Unable to completely load scene. Aborting." );
      TITLE_ERROR_SET( "Unable to load selected visual novel." );
      goto stvnl_cleanup;
   }

   while( 1 ) {
      GFX_DRAW_LOOP_START

      /* Listen for events. */
      event_do_poll( &s_event, FALSE );
      /* Execute the next command unless we're waiting. */
      switch( as_commands[i_command_cursor].command ) {
         case SYSTYPE_VISNOV_CMD_BACKGROUND:
            i_command_cursor = systype_visnov_exec_background(
               &as_commands[i_command_cursor], &ps_bg, i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_PAUSE:
            i_command_cursor = systype_visnov_exec_pause(
               &as_commands[i_command_cursor], i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_COND:
            i_command_cursor = systype_visnov_exec_cond(
               &as_commands[i_command_cursor], as_commands, i_commands_count,
               as_locals, i_locals_count, ps_cache_in, i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_TALK:
            i_command_cursor = systype_visnov_exec_talk(
               &as_commands[i_command_cursor], ps_cache_in, &s_event,
               as_actors, i_actors_count, i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_GOTO:
            i_command_cursor = systype_visnov_exec_goto(
               &as_commands[i_command_cursor], as_commands, i_commands_count,
               i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_PORTRAIT:
            i_command_cursor = systype_visnov_exec_portrait(
               &as_commands[i_command_cursor], &aps_actors_onscreen,
               &i_actors_onscreen_count, as_actors, i_actors_count,
               i_command_cursor, ps_cache_in
            );
            break;

         case SYSTYPE_VISNOV_CMD_TELEPORT:
            i_command_cursor = systype_visnov_exec_teleport(
               &as_commands[i_command_cursor], &b_teleport, ps_cache_in,
               i_command_cursor
            );

            /* If the teleport command was successful, exit the loop and let  *
             * the cache contents take care of the rest.                      */
            if( b_teleport ) {
               goto stvnl_cleanup;
            }
            break;

         case SYSTYPE_VISNOV_CMD_MENU:
            i_command_cursor = systype_visnov_exec_menu(
               &as_commands[i_command_cursor], &ps_menu, &as_locals,
               &i_locals_count, ps_cache_in, &s_event, i_command_cursor
            );
            break;

         case SYSTYPE_VISNOV_CMD_SET:
            i_command_cursor = systype_visnov_exec_set(
               &as_commands[i_command_cursor], &as_locals, &i_locals_count,
               ps_cache_in, i_command_cursor
            );

         default:
            /* Just skip commands we don't understand yet. */
            i_command_cursor++;
      }

      if( s_event.state[EVENT_ID_CANCEL] || s_event.state[EVENT_ID_QUIT] ) {
         ps_cache_in->game_type = SYSTEM_TYPE_TITLE;
         goto stvnl_cleanup;
      }

      /* Draw the on-screen items. */
      if( NULL != ps_bg ) {
         graphics_draw_blit_tile( ps_bg, NULL, NULL );
      }
      for( i = 0 ; i < i_actors_onscreen_count ; i++ ) {
         j = aps_actors_onscreen[i]->emotion_current;

         if( j >= aps_actors_onscreen[i]->emotions_count || j < 0 ) {
            /* A emotion index was used for an emotion that does not exist. */
            continue;
         }

         s_rect_actor.x = aps_actors_onscreen[i]->emotion_x;
         s_rect_actor.y = aps_actors_onscreen[i]->emotion_y;
         s_rect_actor.w = aps_actors_onscreen[i]->emotions[j].image->w;
         s_rect_actor.h = aps_actors_onscreen[i]->emotions[j].image->h;
         graphics_draw_blit_tile(
            aps_actors_onscreen[i]->emotions[j].image,
            NULL,
            &s_rect_actor
         );
      }
      window_draw_text( 0, ps_cache_in );
      if( NULL != ps_menu ) {
         window_draw_menu( ps_menu );
      }
      graphics_do_update();

      GFX_DRAW_LOOP_END
   }

stvnl_cleanup:

   /* Fade out the playing map screen. */
   graphics_draw_transition( GFX_TRANS_FADE_OUT, ps_color_fade );

   /* We won't free the bg pointer inside of the scene struct because it      *
    * points to an image which is also pointed to by one of the commands      *
    * below. If we free it as part of the commands then it's no problem.      */
   for( i = 0 ; i < i_locals_count ; i++ ) {
      bdestroy( as_locals[i].key );
      bdestroy( as_locals[i].value );
   }
   free( as_locals );

   /* Clean up! */
   free( ps_color_fade );
   ezxml_free( ps_xml_scene );
   bdestroy( ps_scene_path );

   for( i = 0 ; i < i_actors_count ; i++ ) {
      mobile_free_arr( &as_actors[i] );
   }
   free( as_actors );

   for( i = 0 ; i < i_commands_count ; i++ ) {
      systype_visnov_free_command_arr( &as_commands[i] );
   }
   free( as_commands );

   if( NULL != ps_menu ) {
      window_free_menu( ps_menu );
   }

   return i_act_return;
}

/* Purpose: Load the array of scene commands.                                 */
/* Parameters: The address of the command array size indicator.               */
/* Return: A pointer to the command array.                                    */
BOOL systype_visnov_load_commands(
   SYSTYPE_VISNOV_COMMAND** aps_commands_in,
   int* pi_count_out,
   ezxml_t ps_xml_script_in
) {
   ezxml_t ps_xml_command = NULL;
   const char* pc_command_action = NULL; /* Pointer to ezxml action result. */
   bstring ps_command_attr = bfromcstr( "" ), /* String holding what to add to cmd data. */
      ps_command_action = bfromcstr( "" ); /* Copy of the command's action for later. */
   SYSTYPE_VISNOV_COMMAND s_command_tmp;
   BOOL b_success = TRUE;

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
         /* TODO: Standardize the loading of an image command into a macro. */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_BACKGROUND, SYSTYPE_VISNOV_CMD_BACKGROUND_DC );
         bassignformat(
            ps_command_attr,
            "%s%s.%s", PATH_SHARE, ezxml_attr( ps_xml_command, "bg" ),
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
         STVN_PARSE_CMD_DAT_SCOPE( scope, 1 );
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
         STVN_PARSE_CMD_DAT_STR( emotion, 1 );
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
         STVN_PARSE_CMD_DAT_SCOPE( scope, 1 );
         STVN_PARSE_CMD_DAT_COL( color_fg, 2 );
         STVN_PARSE_CMD_DAT_COL( color_bg, 3 );
         STVN_PARSE_CMD_DAT_COL( color_sfg, 4 );
         STVN_PARSE_CMD_DAT_COL( color_sbg, 5 );

      } else if( 0 == strcmp( pc_command_action, "set" ) ) {
         /* COMMAND: SET */
         STVN_PARSE_CMD_ALLOC(
            SYSTYPE_VISNOV_CMD_SET, SYSTYPE_VISNOV_CMD_SET_DC );
         STVN_PARSE_CMD_DAT_INT( null, 0 );
         STVN_PARSE_CMD_DAT_SCOPE( scope, 1 );
         STVN_PARSE_CMD_DAT_STR( key, 2 );
         STVN_PARSE_CMD_DAT_STR( equals, 3 );

      }

      /* Allocate the new command item at the end of the list. */
      UTIL_ARRAY_ADD(
         SYSTYPE_VISNOV_COMMAND, *aps_commands_in, *pi_count_out, stvnlc_cleanup,
         &s_command_tmp
      );

      DBG_INFO_STR( "Command added", ps_command_action->data );

      /* Go on to the next one. */
      ps_xml_command = ezxml_next( ps_xml_command );
   }

stvnlc_cleanup:

   bdestroy( ps_command_action );
   bdestroy( ps_command_attr );

   return b_success;
}

/* COMMAND: BACKGROUND */
int systype_visnov_exec_background(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   GFX_SURFACE** pps_bg_in,
   int i_command_cursor_in
) {
   GFX_COLOR* ps_color_fade = NULL;

   ps_color_fade = graphics_create_color( 0, 0, 0 );
   *pps_bg_in = ps_command_in->data->bg;
   graphics_draw_blit_tile( *pps_bg_in, NULL, NULL );
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
   SYSTYPE_VISNOV_COMMAND* as_command_list_in,
   int i_command_list_count_in,
   CACHE_VARIABLE* as_locals_in,
   int i_locals_count_in,
   CACHE_CACHE* ps_cache_in,
   int i_command_cursor_in
) {
   int i; /* Loop iterator. */

   DBG_INFO_STR_STR(
      "Testing condition (key, equals)",
      ps_command_in->data[2].key->data,
      ps_command_in->data[3].equals->data
   );

   if( COND_SCOPE_GLOBAL == ps_command_in->data[1].scope ) {
      for( i = 0 ; i < ps_cache_in->globals_count ; i++ ) {

         CACHE_VARIABLE* svar = &ps_cache_in->globals[i];
         bstring skey = svar->key;
         DBG_INFO( skey->data );

         if(
            0 == bstrcmp(
               ps_command_in->data[2].key, ps_cache_in->globals[i].key ) &&
            0 == bstrcmp(
               ps_command_in->data[3].equals, ps_cache_in->globals[i].value )
         ) {
            /* Conditions are matched, so jump to target. */
            return systype_visnov_exec_goto(
               ps_command_in,
               as_command_list_in,
               i_command_list_count_in,
               i_command_cursor_in
            );
         } else if(
            0 == bstrcmp(
               ps_command_in->data[2].key, ps_cache_in->globals[i].key )
         ) {
            /* The key was found but the value was different. */
            DBG_INFO_STR_STR(
               "Key found with different value",
               ps_cache_in->globals[i].key->data,
               ps_cache_in->globals[i].value->data
            );
         } else {
            DBG_INFO_STR( "Key not found", ps_command_in->data[2].key->data );
         }
      }
   } else if( COND_SCOPE_LOCAL == ps_command_in->data[1].scope ) {
      for( i = 0 ; i < i_locals_count_in ; i++ ) {
         if(
            0 == bstrcmp(
               ps_command_in->data[2].key, as_locals_in[i].key ) &&
            0 == bstrcmp(
               ps_command_in->data[3].equals, as_locals_in[i].value )
         ) {
            /* Conditions are matched, so jump to target. */
            return systype_visnov_exec_goto(
               ps_command_in,
               as_command_list_in,
               i_command_list_count_in,
               i_command_cursor_in
            );
         } else if(
            0 == bstrcmp(
               ps_command_in->data[2].key, as_locals_in[i].key )
         ) {
            /* The key was found but the value was different. */
            DBG_INFO_STR_STR(
               "Key found with different value",
               as_locals_in[i].key->data,
               as_locals_in[i].value->data
            );
         } else {
            DBG_INFO_STR( "Key not found", ps_command_in->data[2].key->data );
         }
      }
   }

   /* The condition was not found or didn't match, so keep going. */
   return ++i_command_cursor_in;
}

/* COMMAND: TALK */
int systype_visnov_exec_talk(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   CACHE_CACHE* ps_cache_in,
   EVENT_EVENT* ps_event_in,
   MOBILE_MOBILE* as_actors_in,
   int i_actors_count_in,
   int i_command_cursor_in
) {
   bstring ps_talk_text = NULL;
   static int ti_cleared_count = 0;
   MOBILE_MOBILE* ps_mob_talk = NULL;

   /* If the window we're about to create is already on the stack, then just  *
    * reset the command cursor back one and quit.                             */
   if(
      0 < ps_cache_in->text_log_count &&
      ti_cleared_count < ps_cache_in->text_log_count
   ) {
      /* The last entry created is the entry currently displaying. */
      if( ps_event_in->state[EVENT_ID_FIRE] ) {
         /* The player wants to advance to the front of the text array. */
         ti_cleared_count = ps_cache_in->text_log_count;
         ps_cache_in->text_log_current = 0;
         return ++i_command_cursor_in;
      } else {
         return i_command_cursor_in;
      }
   }

   ps_mob_talk = systype_visnov_get_actor(
      ps_command_in->data[0].serial,
      as_actors_in,
      i_actors_count_in,
      ps_cache_in
   );
   if( NULL != ps_mob_talk ) {
      ps_talk_text = bformat(
         "%s: %s",
         ps_mob_talk->proper_name->data,
         ps_command_in->data[1].talktext->data
      );
   } else {
      /* Couldn't find the mobile to talk! */
      DBG_ERR_INT(
         "Invalid talk serial specified.", ps_command_in->data[0].serial
      );
      i_command_cursor_in++;
      goto stvnet_cleanup;
   }

   /* Actually display the text window. */
   ps_cache_in->text_log = window_create_text(
      ps_talk_text, ps_cache_in->text_log, &ps_cache_in->text_log_count
   );
   ps_cache_in->text_log_current = 0;

stvnet_cleanup:

   /* Clean up. */
   bdestroy( ps_talk_text );

   return i_command_cursor_in;
}

int systype_visnov_exec_goto(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   SYSTYPE_VISNOV_COMMAND* as_commands_in,
   int i_commands_count_in,
   int i_command_cursor_in
) {
   int i; /* Loop iterator. */

   for( i = 0 ; i < i_commands_count_in ; i++ ) {
      if(
         SYSTYPE_VISNOV_CMD_LABEL == as_commands_in[i].command &&
         0 == bstrcmp(
            ps_command_in->data[0].target, as_commands_in[i].data[0].name )
      ) {
         DBG_INFO_STR(
            "Jumping to label", as_commands_in[i].data[0].name->data
         );
         /* Line found, return its index. */
         return i;
      }
   }

   /* Label not found, move on. */
   DBG_ERR_STR(
      "Visual novel label not found", ps_command_in->data[0].target->data
   );
   return ++i_command_cursor_in;
}

/* COMMAND: PORTRAIT */
int systype_visnov_exec_portrait(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   MOBILE_MOBILE*** paps_actors_onscreen_in,
   int* pi_actors_onscreen_count_in,
   MOBILE_MOBILE* as_actors_in,
   int i_actors_count_in,
   int i_command_cursor_in,
   CACHE_CACHE* ps_cache_in
) {

   int i, z, /* Loop iterators. */
      emotion_selected;
   MOBILE_MOBILE* ps_actor_current;

   /* Make sure an actor with the given serial really exists first. */
   if( NULL == systype_visnov_get_actor(
      ps_command_in->data[0].serial, as_actors_in, i_actors_count_in,
      ps_cache_in
   ) ) {
      DBG_ERR_INT(
         "Invalid portrait serial specified", ps_command_in->data[0].serial
      );
      goto stvnepr_cleanup;
   }

   /* If the given actor is already in the scene, remove them. */
   for( i = 0 ; i < *pi_actors_onscreen_count_in ; i++ ) {
      if(
         (*paps_actors_onscreen_in)[i]->serial ==
         ps_command_in->data[0].serial
      ) {
         UTIL_ARRAY_DEL(
            MOBILE_MOBILE*, *paps_actors_onscreen_in,
            *pi_actors_onscreen_count_in, stvnepr_cleanup, i
         );
      }
   }

   /* Add the actor to the on-screen list. */
   UTIL_ARRAY_REALLOC(
      MOBILE_MOBILE*, *paps_actors_onscreen_in,
      *pi_actors_onscreen_count_in, stvnepr_cleanup
   );
   (*paps_actors_onscreen_in)[*pi_actors_onscreen_count_in - 1] =
      systype_visnov_get_actor(
         ps_command_in->data[0].serial,
         as_actors_in,
         i_actors_count_in,
         ps_cache_in
      );
   ps_actor_current =
      (*paps_actors_onscreen_in)[(*pi_actors_onscreen_count_in) - 1];

   /* Set the actor's current emotion. */
   emotion_selected = -1;
   for( i = 0 ; i < ps_actor_current->emotions_count ; i++ ) {
      if( 0 != bstricmp (
         ps_command_in->data[1].emotion,
         ps_actor_current->emotions[i].id
      ) ) {
         continue;
      } else {
         /* The emotions match so select this one. */
         emotion_selected = i;
         break;
      }
   }

   ps_actor_current->emotion_current = emotion_selected;
   ps_actor_current->emotion_x = ps_command_in->data[3].x;
   ps_actor_current->emotion_y = ps_command_in->data[4].y;

stvnepr_cleanup:

   return ++i_command_cursor_in;
}

/* COMMAND: TELEPORT */
int systype_visnov_exec_teleport(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   BOOL* pb_teleport_out,
   CACHE_CACHE* ps_cache_in,
   int i_command_cursor_in
) {
   /* Verify that the target map exists. */

   //DBG_INFO_STR( "Initiating teleportation", ps_command_in->data[0].destmap );

   return ++i_command_cursor_in;
}

/* COMMAND: MENU */
/* NOTE: This command modifies the list of menus, so a pointer to the pointer *
 *       to the list is expected in case realloc changes the list address.    */
int systype_visnov_exec_menu(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   WINDOW_MENU** pps_menu_in,
   CACHE_VARIABLE** pas_locals_in,
   int* pi_locals_count_in,
   CACHE_CACHE* ps_cache_in,
   EVENT_EVENT* ps_event_in,
   int i_command_cursor_in
) {
   WINDOW_MENU_COLORS s_colors_tmp;
   bstring ps_items = ps_command_in->data[0].items;
   COND_SCOPE i_scope = ps_command_in->data[1].scope;

   /* If the menu we're about to create is already active, then just reset    *
    * the command cursor back one and quit.                                   */
   if( NULL != *pps_menu_in ) {
      /* The last menu created is the menu currently displaying. */
      if( ps_event_in->state[EVENT_ID_FIRE] ) {
         /* The player wants to advance. */
         STVN_CACHE_SET(
            (*pps_menu_in)->options[(*pps_menu_in)->selected].key,
            (*pps_menu_in)->options[(*pps_menu_in)->selected].value,
            (*pps_menu_in)->scope,
            &ps_cache_in->globals,
            &ps_cache_in->globals_count,
            pas_locals_in,
            pi_locals_count_in
         );
         window_free_menu( *pps_menu_in );
         *pps_menu_in = NULL;

         i_command_cursor_in++;

         goto stvnem_cleanup;

      } else if( ps_event_in->state[EVENT_ID_DOWN] ) {
         /* Move the menu cursor down. */
         if( (*pps_menu_in)->selected < (*pps_menu_in)->options_count - 1 ) {
            (*pps_menu_in)->selected++;
         } else {
            (*pps_menu_in)->selected = 0;
         }
         return i_command_cursor_in;

      } else if( ps_event_in->state[EVENT_ID_UP] ) {
         /* Move the menu cursor up. */
         if( (*pps_menu_in)->selected > 0 ) {
            (*pps_menu_in)->selected--;
         } else {
            (*pps_menu_in)->selected = (*pps_menu_in)->options_count - 1;
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
   *pps_menu_in = window_create_menu(
      ps_items, i_scope, &s_colors_tmp
   );

stvnem_cleanup:

   return i_command_cursor_in;
}

/* COMMAND: SET */
int systype_visnov_exec_set(
   SYSTYPE_VISNOV_COMMAND* ps_command_in,
   CACHE_VARIABLE** pas_locals_in,
   int* pi_locals_count_in,
   CACHE_CACHE* ps_cache_in,
   int i_command_cursor_in
) {
   STVN_CACHE_SET(
      ps_command_in->data[2].key,
      ps_command_in->data[3].equals,
      ps_command_in->data[1].scope,
      &ps_cache_in->globals,
      &ps_cache_in->globals_count,
      pas_locals_in,
      pi_locals_count_in
   );

   return ++i_command_cursor_in;
}

/* Purpose: Return a pointer to the first actor with a given serial number.   */
MOBILE_MOBILE* systype_visnov_get_actor(
   int i_serial_in,
   MOBILE_MOBILE* as_actors_in,
   int i_actors_count_in,
   CACHE_CACHE* ps_cache_in
) {
   int i; /* Loop iterator. */

   for( i = 0 ; i < i_actors_count_in ; i++ ) {
      if( as_actors_in[i].serial == i_serial_in ) {
         return &as_actors_in[i];
      }
   }

   /* It wasn't in the normal mobile list so try the player team. */
   for( i = 0 ; i < ps_cache_in->player_team_count ; i++ ) {
      if( ps_cache_in->player_team[i].serial == i_serial_in ) {
         return &ps_cache_in->player_team[i];
      }
   }

   /* Actor wasn't found. */
   return NULL;
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
         bdestroy( ps_command_in->data[1].emotion );
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

      case SYSTYPE_VISNOV_CMD_SET:
         bdestroy( ps_command_in->data[2].key );
         bdestroy( ps_command_in->data[3].equals );
         break;
   }
}
