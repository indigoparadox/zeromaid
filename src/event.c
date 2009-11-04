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

/* Purpose: Poll user input devices, but reject repeat results.               */
int event_do_poll_once( void ) {
   static int i_last_result = EVENT_ID_NULL;
   int i_result = EVENT_ID_NULL;

   /* Poll and compare it to the last result. */
   i_result = event_do_poll();
   if( i_result != i_last_result ) {
      i_last_result = i_result;
      return i_result;
   }

   /* No new result detected. */
   return EVENT_ID_NULL;
}

/* Purpose: Poll user input devices.                                          */
int event_do_poll( void ) {
   #ifdef USESDL
   static SDL_Event* ps_event_temp = NULL;

   /* Create the event object if it doesn't exist yet. */
   if( NULL == ps_event_temp ) {
      ps_event_temp = malloc( sizeof( SDL_Event ) );
   }
   if( NULL == ps_event_temp ) {
      DBG_ERR( "Unable to allocate event object." );
      return SDL_QUIT; /* Safe option. */
   }
   memset( ps_event_temp, 0, sizeof( SDL_Event ) );

   /* Perform the polling and event assignment. */
   SDL_PollEvent( ps_event_temp );
   if( SDL_QUIT == ps_event_temp->type ) {
      return EVENT_ID_QUIT;
   } else if( SDL_KEYDOWN == ps_event_temp->type ) {
      /* A key was pressed, so be more specific. */
      switch( ps_event_temp->key.keysym.sym ) {
         case SDLK_UP:
            return EVENT_ID_UP;

         case SDLK_DOWN:
            return EVENT_ID_DOWN;

         case SDLK_RIGHT:
            return EVENT_ID_RIGHT;

         case SDLK_LEFT:
            return EVENT_ID_LEFT;

         case SDLK_z:
            return EVENT_ID_FIRE;

         case SDLK_x:
            return EVENT_ID_JUMP;

         case SDLK_ESCAPE:
            return EVENT_ID_ESC;

         default:
            /* It was a key we don't know about... */
            return EVENT_ID_NULL;
      }
   } else {
      /* Nothing happened... */
      return EVENT_ID_NULL;
   }
   #elif defined USEWII
   WPAD_ScanPads();
   if( WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME ) {
      return EVENT_ID_QUIT;
   } else if( WPAD_ButtonsDown(0) & WPAD_BUTTON_UP ) {
      return EVENT_ID_UP;
   } else {
      return EVENT_ID_NULL;
   }
   #else
   #error "No event polling mechanism defined for this platform!"
   #endif /* USESDL, USEWII */
}
