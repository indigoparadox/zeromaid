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
   #ifdef USEWII
   /* Input on the Wii is handled through WPAD since SDL is a bit buggy. */
   PAD_ScanPads();

   #elif defined USESDL
   static SDL_Event tps_event_temp;
   static EVENT_EVENT* tps_event_last = NULL;
   BOOL b_key_state;
   int i; /* Loop iterator. */

   /* Create the event object if it doesn't exist yet. */
   /* if( NULL == tps_event_temp ) {
      tps_event_temp = calloc( 1, sizeof( SDL_Event ) );
   }
   if( NULL == tps_event_temp ) {
      DBG_ERR( "Unable to allocate event object." );
      return;
   } */

   memset( &tps_event_temp, 0, sizeof( SDL_Event ) );

   /* Create an image of what the event state looked like last cycle if we're *
    * supposed to avoid repeats.                                              */
   if( !b_repeat_in ) {
      if( NULL == tps_event_last ) {
         /* It might not've been created yet... */
         tps_event_last = calloc( 1, sizeof( EVENT_EVENT ) );
      }
      if( NULL == tps_event_last ) {
         DBG_ERR( "Unable to allocate repeat guard." );
         return;
      }

      /* Perform the copy. */
      memcpy( tps_event_last, ps_event_out, sizeof( EVENT_EVENT ) );
   }

   /* Perform the polling and event assignment. */
   SDL_PollEvent( &tps_event_temp );
   if( SDL_QUIT == tps_event_temp.type ) {
      ps_event_out->state[EVENT_ID_ESC] = TRUE;
   } else if(
      SDL_KEYDOWN == tps_event_temp.type ||
      SDL_KEYUP == tps_event_temp.type
   ) {
      /* Was the key pressed or released? */
      if( SDL_KEYDOWN == tps_event_temp.type ) {
         b_key_state = TRUE;
      } else if( SDL_KEYDOWN == tps_event_temp.type ) {
         b_key_state = FALSE;
      }

      /* Update the key state in the event table. */
      switch( tps_event_temp.key.keysym.sym ) {
         case SDLK_UP:
            ps_event_out->state[EVENT_ID_UP] = b_key_state;
            break;

         case SDLK_DOWN:
            ps_event_out->state[EVENT_ID_DOWN] = b_key_state;
            break;

         case SDLK_RIGHT:
            ps_event_out->state[EVENT_ID_RIGHT] = b_key_state;
            break;

         case SDLK_LEFT:
            ps_event_out->state[EVENT_ID_LEFT] = b_key_state;
            break;

         case SDLK_z:
            ps_event_out->state[EVENT_ID_FIRE] = b_key_state;
            break;

         case SDLK_x:
            ps_event_out->state[EVENT_ID_JUMP] = b_key_state;
            break;

         case SDLK_ESCAPE:
            ps_event_out->state[EVENT_ID_ESC] = b_key_state;
            break;

         default:
            /* It was a key we don't know about... */
            break;
      }
   }

   /* If repeat is off then clear previous states. */
   if( !b_repeat_in ) {
      for( i = 0 ; i < EVENT_ID_MAX ; i++ ) {
         if( ps_event_out->state[i] ) {
            ps_event_out->state[i] ^= tps_event_last->state[i];
         }
      }
   }
   #else
   #error "No event polling mechanism defined for this platform!"
   #endif /* USESDL */
}
