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

/* Purpose: Create a new timer.                                               */
/* Parameters: The timer to operate on.                                       */
EVENT_TIMER* event_timer_create( void ) {
   EVENT_TIMER* ps_timer_out = NULL;

   ps_timer_out = malloc( sizeof( EVENT_TIMER ) );

   if( NULL == ps_timer_out ) {
      DBG_ERR( "Unable to allocate memory for timer!" );
   } else {
      memset( ps_timer_out, 0, sizeof( EVENT_TIMER ) );
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
   static BOOL tab_poll_last[EVENT_ID_MAX] = { FALSE };
   int i_key_test, /* A translation placeholder for pressed keys. */
      i; /* Loop iterator. */

   /* Platform-specific method of setting up. */
   #ifdef USEWII
   u16 i_buttons_down;
   BOOL as_keys[EVENT_ID_MAX];
   #elif defined USESDL
   Uint8* as_keys = NULL;
   static SDL_Event* tps_event_temp = NULL;

   /* Create the event object if it doesn't exist yet. */
   if( NULL == tps_event_temp ) {
      tps_event_temp = calloc( 1, sizeof( SDL_Event ) );
   }
   if( NULL == tps_event_temp ) {
      DBG_ERR( "Unable to allocate event object." );
      return;
   }
   #else
   #error "No event polling mechanism defined for this platform!"
   #endif /* USEWII, USESDL */



   /* Perform the polling and event assignment. */
   #ifdef USEWII
   WPAD_ScanPads();
   i_buttons_down = WPAD_ButtonsHeld( 0 );
   #elif defined USESDL
   SDL_PollEvent( tps_event_temp );

   if( SDL_QUIT == tps_event_temp->type ) {
      /* A quit event takes precedence over all others. */
      memset( ps_event_out, 0, sizeof( EVENT_EVENT ) );
      ps_event_out->state[EVENT_ID_ESC] = TRUE;
      return;
   }

   /* It wasn't a quit event, so poll the keys. */
   as_keys = SDL_GetKeyState( NULL );
   #else
   #error "No event polling mechanism defined for this platform!"
   #endif /* USEWII, USESDL */

   for( i = 0 ; i < EVENT_ID_MAX ; i++ ) {
      #ifdef USEWII
      switch( i ) {
         case EVENT_ID_UP:
            if( i_buttons_down & WPAD_BUTTON_RIGHT ) {
               i_key_test = EVENT_ID_UP;
            }
            break;

         case EVENT_ID_ESC:
            if( i_buttons_down & WPAD_BUTTON_HOME ) {
               i_key_test = EVENT_ID_ESC;
            }
            break;

         default:
            i_key_test = 0;
            break;
      }
      #elif defined USESDL
      switch( i ) {
         case EVENT_ID_UP:
            i_key_test = SDLK_UP;
            break;
         case EVENT_ID_DOWN:
            i_key_test = SDLK_DOWN;
            break;
         case EVENT_ID_RIGHT:
            i_key_test = SDLK_RIGHT;
            break;
         case EVENT_ID_LEFT:
            i_key_test = SDLK_LEFT;
            break;
         case EVENT_ID_FIRE:
            i_key_test = SDLK_z;
            break;
         case EVENT_ID_JUMP:
            i_key_test = SDLK_x;
            break;
         case EVENT_ID_ESC:
            i_key_test = SDLK_ESCAPE;
            break;
         default:
            continue;
      }
      #endif /* USEWII, USESDL */

      if(
         /* Repeat is off and the key is down and it wasn't down before. */
         (!b_repeat_in &&
         as_keys[i_key_test] &&
         !tab_poll_last[i]) ||

         /* Repeat is on and the key is down. */
         (b_repeat_in &&
         as_keys[i_key_test])
      ) {
         tab_poll_last[i] = TRUE;
         ps_event_out->state[i] = TRUE;

      } else if( !as_keys[i_key_test] ) {
         /* The key isn't down. */
         tab_poll_last[i] = FALSE;
         ps_event_out->state[i] = FALSE;

      } else {
         ps_event_out->state[i] = FALSE;
      }
   }
}
