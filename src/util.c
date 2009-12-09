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

/* Purpose: Determine if the specified file exists. This could be done with   *
 *          access(), but that function is unavailable on some target         *
 *          platforms.                                                        */
/* Parameters: The path of the file to test.                                  */
/* Return: TRUE if the file exists and can be read, otherwise FALSE.          */
int file_exists( bstring ps_path_in ) {
   FILE* ps_file_test = fopen( ps_path_in->data, "r" );
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
BOOL string_is_true( bstring ps_test_in ) {
   if(
      0 == strcmp( ps_test_in->data, "true" ) ||
      0 == strcmp( ps_test_in->data, "TRUE" ) ||
      0 == strcmp( ps_test_in->data, "1" )
   ) {
      return TRUE;
   } else {
      return FALSE;
   }
}
