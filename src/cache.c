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

#include "cache.h"

/* = Functions = */

/* Purpose: Save the system cache to the specified file.                      */
/* Parameters: Path of the file to save to.                                   */
/* Return: A boolean integer indicating save success.                         */
BOOL cache_save( bstring ps_out_path_in ) {
   return FALSE;
}

/* Purpose: Load the system cache from the specified save file.               */
/* Parameters: Path of the save file to load.                                 */
/* Return: A boolean integer indicating load success.                         */
BOOL cache_load( bstring ps_in_path_in ) {
   return FALSE;
}

/* Purpose: Set the given variable to the given value within the given scope. */
/* Parameters: The key to set, the value to set it to, the scope in which to  *
 *             set it, and the cache to set it in.                            */
void cache_set_var(
   bstring ps_key_in,
   bstring ps_value_in,
   COND_SCOPE i_scope_in,
   CACHE_CACHE* ps_cache_in
) {
   BOOL b_found = FALSE; /* Did the variable already exist? */
   CACHE_VARIABLE s_var_tmp,
      * as_cache_head = NULL; /* Varies depending on scope. */
   int i, /* Loop iterator. */
      * pi_cache_head_count = NULL;

   /* Figure out the scope we're operating in. */
   if( COND_SCOPE_GLOBAL == i_scope_in ) {
      as_cache_head = ps_cache_in->globals;
      pi_cache_head_count = &ps_cache_in->globals_count;
   } else if( COND_SCOPE_LOCAL == i_scope_in ) {
      as_cache_head = ps_cache_in->locals;
      pi_cache_head_count = &ps_cache_in->locals_count;
   } else {
      /* A weird scope? */
      DBG_ERR_STR( "Unknown scope specified for variable", ps_key_in->data );
   }

   /* Find and set the variable. */
   for( i = 0 ; i < *pi_cache_head_count ; i++ ) {
      if( 0 == bstrcmp( as_cache_head[i].key, ps_key_in ) ) {
         /* Found it! Now set the value. */
         bassign( as_cache_head[i].value, ps_value_in );
         b_found = TRUE;
         break;
      }
   }

   /* If the variable didn't exist, create it. */
   if( !b_found ) {
      memset( &s_var_tmp, 0, sizeof( CACHE_VARIABLE ) );
      bassign( s_var_tmp.key, ps_key_in );
      bassign( s_var_tmp.value, ps_value_in );
      UTIL_ARRAY_ADD(
         CACHE_VARIABLE, as_cache_head, *pi_cache_head_count, csv_cleanup,
         &s_var_tmp
      );
   }

csv_cleanup:

   return;
}
