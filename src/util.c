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

#include "util.h"

/* Purpose: Trims the whitespace from the given character array and returns a *
 *          modified copy. Does not modify the original character array.      */
char *zm_achar_trimws( char *ac_chara_in ) {
   int i_strlen,
      i_strpos, /* Character iterator. */
      i_ws, /* Whitespace iterator. */
      i_strend, /* Last untrimmed character. */
      i_strstart; /* First untrimmed character. */
   char* pc_out, /* The character array to return. */
      ac_ws[3] = { ' ', '\n', '\t' };
   BOOL b_strike = FALSE;

   i_strlen = strlen( ac_chara_in );

   /* Find the last acceptable character. */
   i_strpos = 0;
   while( i_strlen && i_strpos ) {
      /* This might be the end... */
      b_strike = TRUE;
      i_strend = i_strpos;

      /* Test our hypothesis. */
      for( i_ws = 0 ; i_ws < 3 ; i_ws++ ) {
         if( ac_ws[i_ws] == ac_chara_in[i_strpos] ) {
            /* This character is unacceptable. */
            b_strike = FALSE;
            break;
         }
      }

      /* It was the end after all! */
      if( b_strike ) {
         break;
      } else {
         i_strpos--;
      }
   }

   /* Find the first acceptable character. */
   i_strpos = 0;
   while( i_strlen && i_strpos < i_strend ) {
      /* This might be the start... */
      b_strike = TRUE;
      i_strend = i_strpos;

      /* Test our hypothesis. */
      for( i_ws = 0 ; i_ws < 3 ; i_ws++ ) {
         if( ac_ws[i_ws] == ac_chara_in[i_strpos] ) {
            /* This character is unacceptable. */
            b_strike = FALSE;
            break;
         }
      }

      /* It was the start after all! */
      if( b_strike ) {
         break;
      } else {
         i_strpos++;
      }
   }

   /* Allocate the new string. */
   pc_out = malloc( (i_strend - i_strstart) * sizeof( char ) );
   if( NULL != pc_out ) {
      strncpy( pc_out, &ac_chara_in[i_strstart], (i_strend - i_strstart) );
   }

   return pc_out;
}

/* Purpose: Determine if the specified file exists. This could be done with   *
 *          access(), but that function is unavailable on some target         *
 *          platforms.                                                        */
/* Parameters: The path of the file to test.                                  */
/* Return: TRUE if the file exists and can be read, otherwise FALSE.          */
int zm_file_exists( bstring ps_path_in ) {
   #ifdef USEWII
   /* TODO: For some reason, the test below can't detect files on the Wii. */
   return TRUE;
   #endif /* USEWII */

   FILE* ps_file_test = fopen( (const char*)ps_path_in->data, "r" );
   if( NULL != ps_file_test ) {
      fclose( ps_file_test );
      return TRUE;
   }

   return FALSE;
}

/* Purpose: Determine if the property set with the given string should        *
 *          evaluate to a boolean TRUE or not.                                */
/* Parameters: The string to test.                                            */
/* Return: A boolean indicator of the string's truth.                         */
BOOL zm_string_is_true( bstring ps_test_in ) {
   if(
      0 == strcmp( (const char*)ps_test_in->data, "true" ) ||
      0 == strcmp( (const char*)ps_test_in->data, "TRUE" ) ||
      0 == strcmp( (const char*)ps_test_in->data, "1" )
   ) {
      return TRUE;
   } else {
      return FALSE;
   }
}

int zm_string_in_array(
   bstring ps_needle_in,
   int i_start_in,
   bstring* ps_haystack_in,
   int i_count_in
) {
   int i; /* Loop iterator. */

   for( i = i_start_in ; i < i_count_in ; i++ ) {
      if( 0 == strcmp(
         (const char*)ps_needle_in->data, (const char*)ps_haystack_in[i]->data )
      ) {
         return i;
      }
   }

   /* Item wasn't found. */
   return -1;
}
