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

#include "event.h"

DBG_ENABLE

/* = Global Variables = */

#ifdef USEDIRECTX
extern LPDIRECTINPUTDEVICE gs_keyboard;
extern unsigned char gac_keystate[256];
#elif defined( USESDL )
SDL_Joystick* gps_joystick_current;
#endif /* USEDIRECTX, USESDL */

/* = Functions = */

/* Purpose: Create a new timer.                                               */
/* Parameters: The timer to operate on.                                       */
EVENT_TIMER* event_timer_create( void ) {
   EVENT_TIMER* ps_timer_out = NULL;

   ps_timer_out = (EVENT_TIMER*)calloc( 1, sizeof( EVENT_TIMER ) );

   if( NULL == ps_timer_out ) {
      DBG_ERR( "Unable to allocate memory for timer!" );
   }

   return ps_timer_out;
}

/* Purpose: Free a given timer.                                               */
/* Parameters: The timer to operate on.                                       */
void event_timer_free( EVENT_TIMER* ps_timer_in ) {
   free( ps_timer_in );
}

/* Purpose: Start a given timer.                                              */
/* Parameters: The timer to operate on.                                       */
void event_timer_start( EVENT_TIMER* ps_timer_in ) {
   ps_timer_in->i_bol_started = TRUE;
   ps_timer_in->i_bol_paused = FALSE;

   ps_timer_in->i_ticks_start = EVENT_TIMER_TICKS;
}

/* Purpose: Stop a given timer.                                               */
/* Parameters: The timer to operate on.                                       */
void event_timer_stop( EVENT_TIMER* ps_timer_in ) {
   ps_timer_in->i_bol_started = FALSE;
   ps_timer_in->i_bol_paused = FALSE;
}

/* Purpose: Pause a given timer.                                              */
/* Parameters: The timer to operate on.                                       */
void event_timer_pause( EVENT_TIMER* ps_timer_in ) {
   /* Only allow a pause if the timer is running and isn't already paused.    */
   if( ps_timer_in->i_bol_started && !ps_timer_in->i_bol_paused ) {
      ps_timer_in->i_bol_paused = TRUE;
      ps_timer_in->i_ticks_paused =
         EVENT_TIMER_TICKS - ps_timer_in->i_ticks_start;
   }
}

/* Purpose: Unpause a given timer.                                             */
/* Parameters: The timer to operate on.                                          */
void event_timer_unpause( EVENT_TIMER* ps_timer_in ) {
   if( ps_timer_in->i_bol_paused ) {
      ps_timer_in->i_bol_paused = FALSE;
      ps_timer_in->i_ticks_start =
         EVENT_TIMER_TICKS - ps_timer_in->i_ticks_paused;
      ps_timer_in->i_ticks_paused = 0;
   }
}

/* Purpose: Load all of the assignments for the given input type.             */
int* event_load_assignments( int i_input_type_in ) {
   bstring ps_events_path = NULL;
   ezxml_t ps_xml_events = NULL,
      ps_xml_engine = NULL,
      ps_xml_input_iter = NULL;
   int* ai_input_events_out = NULL,
      i_event_target = 0, /* The index at which to assign the scancode. */
      i_event_assignment = 0, /* The scancode to assign. */
      i; /* Loop counter. */

   /* Open the events.xml configuration file and read the key assignments for *
    * the current platform.                                                   */
   ps_events_path = bformat( "%s%s", PATH_SHARE, PATH_FILE_EVENTS );

   /* Verify the XML file exists and open or abort accordingly. */
   if( !zm_file_exists( ps_events_path ) ) {
      DBG_ERR_STR( "Unable to find events file.", ps_events_path->data );
      goto ela_cleanup;
   } else {
      ps_xml_events = ezxml_parse_file( (const char*)ps_events_path->data );
   }

   /* Try to open the section for the current input engine. */
   ps_xml_engine = ezxml_child( ps_xml_events, EVENT_INPUT_ENGINE );
   if( NULL == ps_xml_engine ) {
      DBG_ERR_STR(
         "Invalid events data format",
         "Missing <" EVENT_INPUT_ENGINE "> element."
      );
      goto ela_cleanup;
   }

   /* Cycle through and load all of the assignments for the selected input    *
    * type.                                                                   */
   switch( i_input_type_in ) {
      case EVENT_INPUT_TYPE_KEY:
         ps_xml_input_iter = ezxml_child( ps_xml_engine, "key" );
         break;

      case EVENT_INPUT_TYPE_JBUTTON:
         ps_xml_input_iter = ezxml_child( ps_xml_engine, "jbutton" );
         break;

      case EVENT_INPUT_TYPE_JHAT:
         ps_xml_input_iter = ezxml_child( ps_xml_engine, "jhat" );
         break;

      default:
         DBG_ERR( "Input assignments loader called with no type." );
         goto ela_cleanup;
   }
   ai_input_events_out = (int*)calloc( EVENT_ID_MAX, sizeof( int ) );
   for( i = 0 ; i < EVENT_ID_MAX ; i++ ) {
      ai_input_events_out[i] = -1;
   }
   if( NULL == ai_input_events_out ) {
      DBG_ERR( "Unable to allocate input event array." );
      goto ela_cleanup;
   }
   while( NULL != ps_xml_input_iter ) {
      i_event_target = atoi( ezxml_attr( ps_xml_input_iter, "id" ) );
      i_event_assignment =
         atoi( ezxml_attr( ps_xml_input_iter, "assignment" ) );

      if(
         0 < i_event_target && EVENT_ID_MAX > i_event_target
      ) {
         /* The assignment seems legit. */
         ai_input_events_out[i_event_target] = i_event_assignment;

         DBG_INFO_INT_INT(
            "Input event assigned",
            atoi( ezxml_attr( ps_xml_input_iter, "id" ) ),
            atoi( ezxml_attr( ps_xml_input_iter, "assignment" ) )
         )
      }

      /* Move on to the next one. */
      ps_xml_input_iter = ezxml_next( ps_xml_input_iter );
   }

ela_cleanup:

   ezxml_free( ps_xml_events );

   return ai_input_events_out;
}

/* Purpose: Given an event ID, get the assigned keyboard scancode for the     *
 *          given event.                                                      */
int event_get_assigned( int i_input_type_in, int i_event_id_in ) {
   static int *ti_key_events = NULL,
      *ti_jhat_events = NULL,
      *ti_jbutton_events = NULL;

   /* Be hygenic if the system is asking the input assignment cache to free   *
    * its memory.                                                             */
   if( EVENT_INPUT_TYPE_FREE == i_input_type_in ) {
      if( NULL != ti_key_events ) {
         free( ti_key_events );
      }
      if( NULL != ti_jhat_events ) {
         free( ti_jhat_events );
      }
      if( NULL != ti_jbutton_events ) {
         free( ti_jbutton_events );
      }
   }

   /* If we're missing cached data for the type of input we're trying to look *
    * up, then load it from the disk.                                         */
   if( NULL == ti_key_events && EVENT_INPUT_TYPE_KEY == i_input_type_in ) {
      ti_key_events = event_load_assignments( i_input_type_in );
   }
   if( NULL == ti_jhat_events && EVENT_INPUT_TYPE_JHAT == i_input_type_in ) {
      ti_jhat_events = event_load_assignments( i_input_type_in );
   }
   if(
      NULL == ti_jbutton_events &&
      EVENT_INPUT_TYPE_JBUTTON == i_input_type_in
   ) {
      ti_jbutton_events = event_load_assignments( i_input_type_in );
   }

   /* Look up and return the event ID requested. */
   switch( i_input_type_in ) {
      case EVENT_INPUT_TYPE_KEY:
         return ti_key_events[i_event_id_in];

      case EVENT_INPUT_TYPE_JHAT:
         return ti_jhat_events[i_event_id_in];

      case EVENT_INPUT_TYPE_JBUTTON:
         return ti_jbutton_events[i_event_id_in];

      default:
         return 0;
   }
}

/* Purpose: Poll user input devices.                                          */
void event_do_poll( EVENT_EVENT* ps_event_out, BOOL b_repeat_in ) {
   static BOOL tab_poll_last_key[EVENT_ID_MAX] = { FALSE },
      tab_poll_last_joybutton[EVENT_ID_MAX] = { FALSE },
      tab_poll_last_joyhat[EVENT_ID_MAX] = { FALSE };
   int i_key_test = 0, /* A translation placeholder for pressed keys. */
      i_joybutton_test = 0,
      /* i_joyaxis_test = 0, */
      i_joyhat_test = 0,
      i; /* Loop iterator. */

   /* Platform-specific method of setting up. */
   #ifdef USESDL
   Uint8* as_keys = NULL,
      as_joybuttons[SDL_JOYBUTTONS_MAX] = { FALSE },
      as_joyhat[SDL_JOYHATPOS_MAX] = { FALSE };
   static SDL_Event* tps_event_temp = NULL;

   /* Create the event object if it doesn't exist yet. */
   if( NULL == tps_event_temp ) {
      tps_event_temp = (SDL_Event*)calloc( 1, sizeof( SDL_Event ) );
   }
   if( NULL == tps_event_temp ) {
      DBG_ERR( "Unable to allocate event object." );
      return;
   }
   #elif defined USEDIRECTX
   BOOL as_keys[EVENT_ID_MAX];
   #elif defined USEALLEGRO
   /* int i_key_value;
   int as_keys[KEY_MAX] = { FALSE },
      as_joybuttons[MAX_JOYSTICK_BUTTONS] = { FALSE }; */
   #else
   #error "No event polling mechanism defined for this platform!"
   #endif /* USEWII, USESDL, USEDIRECTX */

   /* Perform the polling and event assignment. */
   #ifdef USESDL
   /* Perform the poll. */
   SDL_JoystickUpdate();
   SDL_PollEvent( tps_event_temp );
   as_keys = SDL_GetKeyState( NULL );
   memset( ps_event_out, 0, sizeof( EVENT_EVENT ) );

   if( SDL_KEYDOWN == tps_event_temp->type ) {

   } else if( SDL_JOYHATMOTION == tps_event_temp->type ) {
      event_do_poll_sdl_joystick_hats( tps_event_temp, as_joyhat );

   } else if( SDL_JOYAXISMOTION == tps_event_temp->type ) {
      /* event_do_poll_sdl_joystick_axis( tps_event_temp, as_joybuttons ); */

   } else if( SDL_JOYBUTTONDOWN == tps_event_temp->type ) {
      event_do_poll_sdl_joystick_buttons( tps_event_temp, as_joybuttons );

   } else if( SDL_QUIT == tps_event_temp->type ) {
      /* A quit event can be processed right away without refinement. */
      ps_event_out->state[EVENT_ID_QUIT] = TRUE;
      return;
   }
   #elif defined USEDIRECTX
   // TODO
   if( FAILED( gs_keyboard->GetDeviceState(
      sizeof( unsigned char[256] ), (LPVOID)gac_keystate
   ) ) ) {
      /* error code */
   }
   #elif defined USEALLEGRO
   /* i_key_value = readkey(); */
   poll_keyboard();
   #else
   #error "No event polling mechanism defined for this platform!"
   #endif /* USEWII, USESDL, USEDIRECTX */

   /* Translate each event to a neutral struct and test that struct. */
   for( i = 0 ; i < EVENT_ID_MAX ; i++ ) {
      /* Each time this loop goes around, one of the events selected by the   *
       * select statements below is tested for each type of input device.     */

      i_key_test = event_get_assigned( EVENT_INPUT_TYPE_KEY, i );
      i_joybutton_test = event_get_assigned( EVENT_INPUT_TYPE_JBUTTON, i );
      i_joyhat_test = event_get_assigned( EVENT_INPUT_TYPE_JHAT, i );
      /* i_joyaxis_test = event_get_assigned( EVENT_TYPE_JAXIS, i ); */

      /* Perform the actual input test. i_key_test, i_joybutton_test,         *
       * i_joyaxis_test, and i_joyhat_test all contain their native           *
       * constants.                                                           */
      /* TODO: Test joy axis. */

      /* Test the keyboard keys. */
      if(
         /* Avoid a negative index issue for unassigned events! Heh... */
         -1 != i_key_test &&

         /* Repeat is off and the key is down and it wasn't down before. */
         ((!b_repeat_in &&
         as_keys[i_key_test] &&
         !tab_poll_last_key[i]) ||

         /* Repeat is on and the key is down. */
         (b_repeat_in &&
         as_keys[i_key_test]))
      ) {
         /* A key was presset. */
         tab_poll_last_key[i] = TRUE;
         ps_event_out->state[i] = TRUE;

      } else if( !as_keys[i_key_test] ) {
         /* The key isn't down. */
         tab_poll_last_key[i] = FALSE;
      }

      #ifndef USEALLEGRO
      /* Test the joystick buttons. */
      if(
         -1 != i_joybutton_test &&

         /* Repeat is off and the joybutton is down and it wasn't down        *
          * before.                                                           */
         ((!b_repeat_in &&
         as_joybuttons[i_joybutton_test] &&
         !tab_poll_last_joybutton[i]) ||

         /* Repeat is on and the joybutton is down. */
         (b_repeat_in &&
         as_joybuttons[i_joybutton_test]))
      ) {
         /* A joybutton was presset. */
         tab_poll_last_joybutton[i] = TRUE;
         ps_event_out->state[i] = TRUE;

      } else if( !as_joybuttons[i_joybutton_test] ) {
         /* The joybutton isn't down. */
         tab_poll_last_joybutton[i] = FALSE;
      }

      /* Test the joystick hat. */
      if(
         -1 != i_joyhat_test &&

         /* Repeat is off and the joyhat is down and it wasn't down before. */
         ((!b_repeat_in &&
         as_joyhat[i_joyhat_test] &&
         !tab_poll_last_joyhat[i]) ||

         /* Repeat is on and the joyhat is down. */
         (b_repeat_in &&
         as_joyhat[i_joyhat_test]))
      ) {
         /* A joyhat was presset. */
         tab_poll_last_joyhat[i] = TRUE;
         ps_event_out->state[i] = TRUE;

      } else if( !as_joyhat[i_joyhat_test] ) {
         /* The joyhat isn't down. */
         tab_poll_last_joyhat[i] = FALSE;

      }
      #endif /* USEALLEGRO */
   }
}

#ifdef USESDL

void event_do_poll_sdl_joystick_buttons(
   SDL_Event* ps_event_in, Uint8* as_joybuttons_in
) {
   int i; /* Loop counter. */

   for( i = 0 ; i < SDL_JOYBUTTONS_MAX ; i++ ) {
      if( i == ps_event_in->jbutton.button ) {
         as_joybuttons_in[i] = TRUE;
         DBG_INFO_INT( "Button", ps_event_in->jbutton.button );
      } else {
         as_joybuttons_in[i] = FALSE;
      }
      /* DBG_INFO_INT( "Button", ps_event_in->jbutton.button ); */
   }
}

void event_do_poll_sdl_joystick_hats(
   SDL_Event* ps_event_in, Uint8* as_joyhat_in
) {
   int i_joyhat_state = 0;

   /* Get the joyhat state from SDL. */
   i_joyhat_state = SDL_JoystickGetHat( gps_joystick_current, 0 );

   DBG_INFO_INT( "Joyhat", i_joyhat_state );

   /* Set all positions to zero and light up the one that's being pressed. */
   memset( as_joyhat_in, FALSE, sizeof( Uint8 ) * SDL_JOYHATPOS_MAX );
   as_joyhat_in[i_joyhat_state] = TRUE;
}

#endif /* USESDL */
