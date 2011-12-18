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

#ifndef UTIL_H
#define UTIL_H

/* = Includes = */

#include <stdio.h>

#include "bstring/bstraux.h"
#include "bstring/bstrlib.h"
#include "defines.h"

#ifdef USEDIRECTX
#include <windows.h>
#elif defined USEALLEGRO
#include <allegro.h>
#endif /* USEDIRECTX, USEALLEGRO */

/* = Macros = */

/* Purpose: Allocate space for the additional item and then append it to the  *
 *          array.                                                            */
/* Parameters: The type of the array items, a pointer to the array, the       *
 *             number of items in the array, the label to jump to on failure, *
 *             a pointer to the item to add (NULL if none).                   */
#define UTIL_ARRAY_REALLOC( type, array_ptr, array_count, cleanup ) \
   (array_count)++; \
   array_ptr = (type*)realloc( array_ptr, array_count * sizeof( type ) ); \
   if( NULL == array_ptr ) { \
      DBG_ERR( "Unable to allocate array: " #array_ptr ); \
      goto cleanup; \
   }

/* Purpose: Allocate space for the additional item and then append it to the  *
 *          array.                                                            */
/* Parameters: The type of the array items, a pointer to the array, the       *
 *             number of items in the array, the label to jump to on failure, *
 *             a pointer to the item to add (NULL if none).                   */
#define UTIL_ARRAY_ADD( type, array_ptr, array_count, cleanup, item_addr ) \
   UTIL_ARRAY_REALLOC( type, array_ptr, array_count, cleanup ); \
   if( NULL != item_addr ) { \
      memcpy( &(array_ptr)[array_count - 1], item_addr, sizeof( type ) ); \
   }

/* Purpose: Remove the item with the given index and shrink the array         *
 *          accordingly.                                                      */
/* Parameters: The type of the array items, a pointer to the array, the       *
 *             number of items in the array, the label to jump to on failure, *
 *             the index of the item to remove.                               */
#define UTIL_ARRAY_DEL( type, array_ptr, array_count, cleanup, index ) \
   for( z = (index + 1) ; z < array_count ; z++ ) { \
      memcpy( &array_ptr[z - 1], &array_ptr[z], sizeof( type ) ); \
   } \
   array_ptr = (type*)realloc( array_ptr, array_count * sizeof( type ) ); \
   if( NULL == array_ptr ) { \
      DBG_ERR( "Unable to allocate array: " #array_ptr ); \
      goto cleanup; \
   } \
   (array_count)--;

/* Purpose: Allocate space for the additional item and then push it onto the  *
 *          given stack.                                                      */
/* Parameters: The type of the stack items, a pointer to the stack, the       *
 *             number of items in the stack, the label to jump to on failure, *
 *             a pointer to the item to add (NULL if none).                   */
#define UTIL_STACK_ADD( type, stack_ptr, stack_count, cleanup, item_addr ) \
   (stack_count)++; \
   stack_ptr = (type*)realloc( stack_ptr, stack_count * sizeof( type ) ); \
   if( NULL == stack_ptr ) { \
      DBG_ERR( "Unable to allocate stack: " #stack_ptr ); \
      goto cleanup; \
   } \
   for( z = stack_count - 1 ; z > 0 ; z-- ) { \
      memcpy( &stack_ptr[z], &stack_ptr[z - 1], sizeof( type ) ); \
   } \
   if( NULL != item_addr ) { \
      memcpy( &stack_ptr[0], item_addr, sizeof( type ) ); \
   }

#define UTIL_LLIST_ADD( list_ptr, list_iter, cleanup, item_addr ) \
   if( NULL == list_ptr ) { \
      list_ptr = item_addr; \
   } else { \
      list_iter = list_ptr; \
      while( NULL != list_iter->next ) { \
         list_iter = list_iter->next; \
      } \
      list_iter->next = item_addr; \
   }

/* XXX: Does this pointer-comparison work? */
#define UTIL_LLIST_REMOVE( list_ptr, list_iter, item_addr ) \
   list_iter = list_ptr; \
   while( NULL != list_iter ) { \
      if( item_addr == list_iter->next ) { \
         list_iter->next = item_addr->next; \
         item_addr->next = NULL; \
         break; \
      } \
      list_iter = list_iter->next; \
   }

#define UTIL_LLIST_FSTR( list_ptr, list_iter, find_key, find_val ) \
   list_iter = list_ptr; \
   while( NULL != list_iter ) { \
      if( 0 == bstrcmp( find_val, list_iter->find_key ) ) { \
         break; \
      } \
      list_iter = list_iter->next; \
   }

#define UTIL_LLIST_FINT( list_ptr, list_iter, find_key, find_val ) \
   list_iter = list_ptr; \
   while( NULL != list_iter ) { \
      if( find_val == list_iter->find_key ) { \
         break; \
      } \
      list_iter = list_iter->next; \
   }

/* = Function Prototypes = */

int zm_file_exists( bstring );
BOOL zm_string_is_true( bstring );
int zm_string_in_array( bstring, int, bstring*, int );

#endif /* UTIL_H */
