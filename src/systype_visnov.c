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

/* = Commands = */

/* XXX: Don't mind this. We're just thinking to ourselves... */
/* STVN_COMMAND( background, int foo );
STVN_COMMAND( pause );
STVN_COMMAND( label );
STVN_COMMAND( cond );
STVN_COMMAND( talk );
STVN_COMMAND( goto );
STVN_COMMAND( portrait );
STVN_COMMAND( teleport );
STVN_COMMAND( menu );
STVN_COMMAND( set ); */

/* = Functions = */

/* Purpose: Visual novel loop.                                                */
/* Parameters: The name of the scene to play.                                 */
/* Return: The code for the next action to take.                              */
int systype_visnov_loop( CACHE_CACHE* ps_cache_in ) {
   CACHE_VARIABLE* as_locals = NULL;
   MOBILE_MOBILE** aps_actors_onscreen = NULL; /* Dupe ptrs to actors on-screen. */
   MOBILE_MOBILE* as_actors = NULL;
   SYSTYPE_VISNOV_COMMAND* ps_commands = NULL;
   int i_actors_count = 0,
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
   ps_commands = systype_visnov_load_commands(
      ezxml_child( ps_xml_scene, "script" )
   );
   b_actors_loaded = mobile_load_mobiles(
      &as_actors,
      &i_actors_count,
      ezxml_child( ps_xml_scene, "mobiles" ),
      NULL
   );

   if( NULL == ps_commands || !b_actors_loaded ) {
      DBG_ERR( "Unable to completely load scene. Aborting." );
      TITLE_ERROR_SET( "Unable to load selected visual novel." );
      goto stvnl_cleanup;
   }

   while( 1 ) {
      GFX_DRAW_LOOP_START

      /* Listen for events. */
      event_do_poll( &s_event, FALSE );
      /* Execute the next command unless we're waiting. */
      /*switch( as_commands[i_command_cursor].command ) {
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

            if( b_teleport ) {
               i_act_return = RETURN_ACTION_LOADCACHE;
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
            break;

         default:
            i_command_cursor++;
      } */

      if( s_event.state[EVENT_ID_CANCEL] || s_event.state[EVENT_ID_QUIT] ) {
         CACHE_SYSTEM_TYPE_SET( ps_cache_in, SYSTEM_TYPE_TITLE );
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

   /* for( i = 0 ; i < i_commands_count ; i++ ) {
      systype_visnov_free_command_arr( &as_commands[i] );
   }
   free( as_commands ); */

   if( NULL != ps_menu ) {
      window_free_menu( ps_menu );
   }

   return i_act_return;
}

SYSTYPE_VISNOV_COMMAND* systype_visnov_load_commands(
   ezxml_t ps_xml_script_in
) {
    ezxml_t ps_xml_command = NULL;
   bstring ps_command_attr = NULL, /* String holding what to add to cmd data. */
      ps_command_action = NULL; /* Copy of the command's action for later. */
   SYSTYPE_VISNOV_COMMAND* ps_command_iter;
   BOOL b_success = TRUE;

   /* Verify that the script exists. */
   if( NULL == ps_xml_script_in ) {
      DBG_ERR( "Invalid visual novel script detected." );
      //goto stvnlc_cleanup;
   }

   /* Load each command in order. */
   ps_xml_command = ezxml_child( ps_xml_script_in, "command" );
   while( NULL != ps_xml_command ) {
      STVN_COMMAND_LOAD_BACKGROUND( ps_command_iter, ps_xml_command )
   }
}
