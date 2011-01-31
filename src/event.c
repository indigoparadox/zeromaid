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

/* Purpose: Poll user input devices.                                          */
void event_do_poll( EVENT_EVENT* ps_event_out, BOOL b_repeat_in ) {
   static BOOL tab_poll_last_key[EVENT_ID_MAX] = { FALSE },
      tab_poll_last_joybutton[EVENT_ID_MAX] = { FALSE },
      tab_poll_last_joyhat[EVENT_ID_MAX] = { FALSE };
   int i_key_test = 0, /* A translation placeholder for pressed keys. */
      i_joybutton_test = 0,
      i_joyaxis_test = 0,
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
   int i_key_value;
   int as_keys[KEY_MAX] = { FALSE },
      as_joybuttons[MAX_JOYSTICK_BUTTONS] = { FALSE };
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
      DBG_INFO( "" );

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
   i_key_value = readkey();
   #else
   #error "No event polling mechanism defined for this platform!"
   #endif /* USEWII, USESDL, USEDIRECTX */

   /* Translate each event to a neutral struct and test that struct. */
   for( i = 0 ; i < EVENT_ID_MAX ; i++ ) {
      /* Each time this loop goes around, one of the events selected by the   *
       * select statements below is tested for each type of input device.     */
      #ifdef USEWII
      switch( i ) {
         case EVENT_ID_UP:
            i_key_test = -1;
            i_joybutton_test = -1;
            i_joyaxis_test = -1;
            i_joyhat_test = SDL_HAT_UP;
            break;
         case EVENT_ID_DOWN:
            i_key_test = -1;
            i_joybutton_test = -1;
            i_joyaxis_test = -1;
            i_joyhat_test = SDL_HAT_DOWN;
            break;
         case EVENT_ID_RIGHT:
            i_key_test = -1;
            i_joybutton_test = -1;
            i_joyaxis_test = -1;
            i_joyhat_test = SDL_HAT_RIGHT;
            break;
         case EVENT_ID_LEFT:
            i_key_test = -1;
            i_joybutton_test = -1;
            i_joyaxis_test = -1;
            i_joyhat_test = SDL_HAT_LEFT;
            break;
         case EVENT_ID_FIRE:
            i_key_test = -1;
            i_joybutton_test = 2;
            i_joyaxis_test = -1;
            i_joyhat_test = -1;
            break;
         case EVENT_ID_JUMP:
            i_key_test = -1;
            i_joybutton_test = 3;
            i_joyaxis_test = -1;
            i_joyhat_test = -1;
            break;
         case EVENT_ID_ESC:
            i_key_test = -1;
            i_joybutton_test = 6;
            i_joyaxis_test = -1;
            i_joyhat_test = -1;
            break;
         default:
            continue;
      }
      #elif defined USESDL
      switch( i ) {
         /* TODO: Assign joystick controls and make them configurable. */
         case EVENT_ID_UP:
            i_key_test = SDLK_UP;
            i_joybutton_test = -1;
            i_joyaxis_test = -1;
            i_joyhat_test = SDL_HAT_UP;
            break;
         case EVENT_ID_DOWN:
            i_key_test = SDLK_DOWN;
            i_joybutton_test = -1;
            i_joyaxis_test = -1;
            i_joyhat_test = SDL_HAT_DOWN;
            break;
         case EVENT_ID_RIGHT:
            i_key_test = SDLK_RIGHT;
            i_joybutton_test = -1;
            i_joyaxis_test = -1;
            i_joyhat_test = SDL_HAT_RIGHT;
            break;
         case EVENT_ID_LEFT:
            i_key_test = SDLK_LEFT;
            i_joybutton_test = -1;
            i_joyaxis_test = -1;
            i_joyhat_test = SDL_HAT_LEFT;
            break;
         case EVENT_ID_FIRE:
            i_key_test = SDLK_z;
            i_joybutton_test = 0;
            i_joyaxis_test = -1;
            i_joyhat_test = -1;
            break;
         case EVENT_ID_JUMP:
            i_key_test = SDLK_x;
            i_joybutton_test = 1;
            i_joyaxis_test = -1;
            i_joyhat_test = -1;
            break;
         case EVENT_ID_ESC:
            i_key_test = SDLK_ESCAPE;
            i_joybutton_test = 2;
            i_joyaxis_test = -1;
            i_joyhat_test = -1;
            break;
         default:
            continue;
      }
      #elif defined USEDIRECTX
      switch( i ) {
         case EVENT_ID_UP:
            i_key_test =
               ( gac_keystate[DIK_UP] & 0x80 ? EVENT_ID_UP : EVENT_ID_NULL );
            break;

         case EVENT_ID_DOWN:
            i_key_test =
               ( gac_keystate[DIK_DOWN] & 0x80 ? EVENT_ID_DOWN : EVENT_ID_NULL );
            break;

         case EVENT_ID_RIGHT:
            i_key_test =
               ( gac_keystate[DIK_RIGHT] & 0x80 ? EVENT_ID_RIGHT : EVENT_ID_NULL );
            break;

         case EVENT_ID_LEFT:
            i_key_test =
               ( gac_keystate[DIK_LEFT] & 0x80 ? EVENT_ID_LEFT : EVENT_ID_NULL );
            break;

         case EVENT_ID_ESC:
            i_key_test =
               ( gac_keystate[DIK_ESCAPE] & 0x80 ? EVENT_ID_ESC : EVENT_ID_NULL );
            break;

         case EVENT_ID_FIRE:
            i_key_test =
               ( gac_keystate[DIK_Z] & 0x80 ? EVENT_ID_FIRE : EVENT_ID_NULL );
            break;

         case EVENT_ID_JUMP:
            i_key_test =
               ( gac_keystate[DIK_X] & 0x80 ? EVENT_ID_JUMP : EVENT_ID_NULL );
            break;

         default:
            i_key_test = 0;
      }
      #elif defined USEALLEGRO
      switch( i ) {
         case EVENT_ID_UP:
            i_key_test = KEY_UP;
            break;

         case EVENT_ID_DOWN:
            i_key_test = KEY_DOWN;
            break;

         case EVENT_ID_RIGHT:
            i_key_test = KEY_RIGHT;
            break;

         case EVENT_ID_LEFT:
            i_key_test = KEY_LEFT;
            break;

         case EVENT_ID_ESC:
            i_key_test = KEY_ESC;
            break;

         case EVENT_ID_FIRE:
            i_key_test = KEY_Z;
            break;

         case EVENT_ID_JUMP:
            i_key_test = KEY_X;
            break;

         default: continue;
      }
      #endif /* USEWII, USESDL, USEDIRECTX */

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

      #ifndef USEALLEGRO
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
