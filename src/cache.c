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
   CACHE_VARIABLE** as_varlist_in,
   int* pi_varlist_count_in
) {
   BOOL b_found = FALSE; /* Did the variable already exist? */
   CACHE_VARIABLE s_var_tmp;
   int i; /* Loop iterator. */

   /* Find and set the variable. */
   for( i = 0 ; i < *pi_varlist_count_in ; i++ ) {
      if( 0 == bstrcmp( (*as_varlist_in)[i].key, ps_key_in ) ) {
         /* Found it! Now set the value. */
         bassign( (*as_varlist_in)[i].value, ps_value_in );
         b_found = TRUE;
         DBG_INFO_STR_STR( "Key reset", ps_key_in->data, ps_value_in->data );
         break;
      }
   }

   /* If the variable didn't exist, create it. */
   if( !b_found ) {
      memset( &s_var_tmp, 0, sizeof( CACHE_VARIABLE ) );
      s_var_tmp.key = bstrcpy( ps_key_in );
      s_var_tmp.value = bformat( "false" ); //bstrcpy( ps_value_in );

      UTIL_ARRAY_ADD(
         CACHE_VARIABLE, *as_varlist_in, *pi_varlist_count_in, csv_cleanup,
         &s_var_tmp
      );

      DBG_INFO_STR_STR( "Key created", ps_key_in->data, ps_value_in->data );
   }

csv_cleanup:

   return;
}
