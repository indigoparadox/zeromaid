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

#ifndef EVENT_H
#define EVENT_H

/* = Includes = */

#include <time.h>

#ifdef USESDL
#ifdef __APPLE__
   #include <SDL/SDL.h>
#elif defined __unix__
   #include <SDL/SDL.h>
#else
   #include <SDL.h>
#endif /* __APPLE__, __unix__ */
#endif /* USESDL */

#ifdef USEWII
typedef struct _lwpnode {
	struct _lwpnode *next;
	struct _lwpnode *prev;
} lwp_node;

#include <wiiuse/wpad.h>
#endif /* USEWII */

#ifdef USEDIRECTX
#include <dinput.h>
#endif /* USEDIRECTX */

#include "defines.h"

/* = Definitions = */

#define EVENT_ID_NULL 0

#define EVENT_ID_QUIT 1
#define EVENT_ID_UP 2
#define EVENT_ID_DOWN 3
#define EVENT_ID_RIGHT 4
#define EVENT_ID_LEFT 5
#define EVENT_ID_FIRE 6
#define EVENT_ID_JUMP 7
#define EVENT_ID_ESC 8

#define EVENT_ID_MAX 9

/* It might not be a good idea to use time() as our tick timer, so keep an    *
 * out for problems this might cause.                                         */
#define EVENT_TIMER_TICKS time( NULL )

/* = Type and Struct Definitions = */

typedef struct {
   time_t i_ticks_start;
   time_t i_ticks_paused;
   time_t i_bol_started;
   time_t i_bol_paused;
} EVENT_TIMER;

typedef struct {
   BOOL state[EVENT_ID_MAX];
} EVENT_EVENT;

/* = Function Prototypes = */

EVENT_TIMER* event_timer_create( void );
void event_timer_free( EVENT_TIMER* );
void event_timer_start( EVENT_TIMER* );
void event_timer_start( EVENT_TIMER* );
void event_timer_pause( EVENT_TIMER* );
void event_timer_unpause( EVENT_TIMER* );
void event_do_poll( EVENT_EVENT*, BOOL );

#endif /* EVENT_H */
