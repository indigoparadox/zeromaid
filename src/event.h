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
#elif defined USEWII
#include <grrlib.h>
#endif /* USESDL, USEWII */

#include "defines.h"

/* = Definitions = */

#ifdef USESDL
#define EVENT_ID_QUIT SDL_QUIT
#elif defined _ARCH_PPCGR
#define EVENT_ID_QUIT 0
#else
#error "No event types defined for this platform!"
#endif /* USESDL, _ARCH_PPCGR */

/* #ifdef USESDL
#define EVENT_TIMER_TICKS SDL_GetTicks()
#elif defined _ARCH_PPCGR
// XXX
#define EVENT_TIMER_TICKS 0
#else
#error "No timer mechanism defined for this platform!"
#endif */ /* USESDL, _ARCH_PPCGR */

#define EVENT_TIMER_TICKS time( NULL )

/* = Type and Struct Definitions = */

typedef struct {
   int i_ticks_start;
   int i_ticks_paused;
   int i_bol_started;
   int i_bol_paused;
} EVENT_TIMER;

#ifdef USESDL
typedef SDL_Event EVENT_EVENT;
#elif defined _ARCH_PPCGR
/* Nintendo Wii */
typedef struct {
   int type;
} EVENT_EVENT;
#else
#error "No event entity defined for this platform!"
#endif /* USESDL, _ARCH_PPCGR */

/* = Function Prototypes = */

EVENT_TIMER* event_timer_create( void );
void event_timer_free( EVENT_TIMER* );
void event_timer_start( EVENT_TIMER* );
void event_timer_start( EVENT_TIMER* );
void event_timer_pause( EVENT_TIMER* );
void event_timer_unpause( EVENT_TIMER* );
void event_do_poll( EVENT_EVENT* );

#endif /* EVENT_H */
