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

#include "mobile.h"

DBG_ENABLE

/* = Functions = */

/* Purpose: Load the mobiles for this map into the given dynamic array.       */
/* Parameters: The address of number of mobiles in the output array, the XML  *
 *             branch from which to load the mobiles, and the tilemap to      *
 *             place them on. If the tilemap is NULL, the mobiles will not be *
 *             given a starting position.                                     */
/* Return: The address of the output array.                                   */
MOBILE_MOBILE* mobile_load_mobiles(
   int* i_count_out,
   ezxml_t ps_xml_mobiles_in,
   TILEMAP_TILEMAP* ps_map_in
) {
   ezxml_t ps_xml_mobiles = NULL, ps_xml_mob_iter = NULL;
   MOBILE_MOBILE* ps_mobs_out = NULL,
      * ps_mob_iter;
   bstring ps_mob_iter_type = NULL;
   char* pc_attr = NULL;
   ps_mob_iter_type = bfromcstr( "" );

   ps_xml_mob_iter = ezxml_child( ps_xml_mobiles_in, "mobile" );
   while( NULL != ps_xml_mob_iter ) {
      /* Try to load the mobile first to see if it's valid. */
      ps_mob_iter_type =
         bassignformat( "%s", ezxml_attr( ps_xml_mob_iter, "type" ) );
      ps_mob_iter = mobile_load_mobile( ps_mob_iter_type );

      /* Verify the loaded mobile. */
      if( NULL == ps_mob_iter ) {
         DBG_ERR_STR( "Unable to load mobile", ps_mob_iter_type->data );
      }

      /* Load the mobile's position. */
      if( NULL != ps_map_in ) {
         pc_attr = ezxml_attr( ps_xml_mob_iter, "startx" );
         if( NULL != pc_attr ) {
            ps_mob_iter->pixel_x = atoi( pc_attr ) *
               ps_map_in->tileset->pixel_size;
         }
         pc_attr = ezxml_attr( ps_xml_mob_iter, "starty" );
         if( NULL != pc_attr ) {
            ps_mob_iter->pixel_y = atoi( pc_attr ) *
               ps_map_in->tileset->pixel_size;
         }
      }

      /* A mobile was successfully loaded, so prepare the array for it. */
      UTIL_ARRAY_ADD(
         MOBILE_MOBILE, ps_mobs_out, *i_count_out, salm_cleanup, ps_mob_iter
      );

      /* Free the temporary mobile iterator now that it's been copied into    *
       * the array.                                                           */
      free( ps_mob_iter );

      /* Go to the next one! */
      ps_xml_mob_iter = ezxml_next( ps_xml_mob_iter );
   }

salm_cleanup:

   /* Clean up. */
   ezxml_free( ps_xml_mobiles );
   bdestroy( ps_mob_iter_type );

   return ps_mobs_out;
}

/* Purpose: Create a MOBILE struct from the mobile data at the given path.    */
/* Parameters: The struct in which to place the new mobile, the path to the   *
 *             mobile's data file.                                            */
MOBILE_MOBILE* mobile_load_mobile( bstring ps_type_in ) {
   ezxml_t ps_xml_mob = NULL, ps_xml_props = NULL, ps_xml_prop_iter = NULL;
   bstring ps_path_temp = NULL;
   char* pc_attr = NULL;
   MOBILE_MOBILE* ps_mob_out = NULL;

   ps_path_temp = bformat( "%smob_%s.xml", PATH_SHARE, ps_type_in );

   /* Verify the XML file exists and open or abort accordingly. */
   if( !file_exists( ps_path_temp ) ) {
      DBG_ERR_STR( "Unable to load mobile data file", ps_path_temp->data );
      return NULL;
   }
   ps_xml_mob = ezxml_parse_file( (const char*)ps_path_temp->data );

   /* Allocate and verify the mobile to return. */
   ps_mob_out = calloc( 1, sizeof( MOBILE_MOBILE ) );
   if( NULL == ps_mob_out ) {
      DBG_ERR_STR( "Unable to allocate mobile", ps_type_in->data );
      return NULL;
   }

   /* Slip the mobile's type in before loading the properties. */
   ps_mob_out->mobile_type = bstrcpy( ps_type_in );

   /* Load the properties tree. */
   ps_xml_props = ezxml_child( ps_xml_mob, "properties" );
   ps_xml_prop_iter = ezxml_child( ps_xml_props, "property" );
   while( NULL != ps_xml_prop_iter ) {
      /* Load the current property into the struct. */
      if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "propername" ) ) {
         /* ATTRIB: PROPER NAME */
         ps_mob_out->proper_name =
            bformat( "%s", ezxml_attr( ps_xml_prop_iter, "value" ) );
         DBG_INFO_STR( "Mobile proper name", ps_mob_out->proper_name->data );

      } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "SERIAL" ) ) {
         /* ATTRIB: SERIAL */
         pc_attr = ezxml_attr( ps_xml_prop_iter, "value" );
         if( NULL != pc_attr ) {
            /* The mobile can still be loaded if there's no serial, but it    *
             * just can't be addressed by scripting.                          */
            ps_mob_out->serial = atoi( pc_attr );
         }

      } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "hp" ) ) {
         /* ATTRIB: HP */
         ps_mob_out->hp = atoi( ezxml_attr( ps_xml_prop_iter, "value" ) );
         DBG_INFO_INT( "Mobile HP", ps_mob_out->hp );

      } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "spritesheet" ) ) {
         /* ATTRIB: SPRITESHEET */
         ps_path_temp = bformat(
            "%smob_sprites_%s.%s",
            PATH_SHARE,
            ezxml_attr( ps_xml_prop_iter, "value" ),
            FILE_EXTENSION_IMAGE
         );
         ps_mob_out->spritesheet = graphics_create_spritesheet( ps_path_temp );
         bdestroy( ps_path_temp );

      } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "ai_adv_normal" ) ) {
         /* AI: ADVENTURE NORMAL */
         ps_path_temp = bformat(
            "%sai_%s.%s",
            PATH_SHARE,
            ezxml_attr( ps_xml_prop_iter, "value" ),
            FILE_EXTENSION_AI
         );
         mobile_load_ai( ps_mob_out, MOBILE_AI_ADV_NORMAL, ps_path_temp );
         bdestroy( ps_path_temp );

      }

      /* Go on to the next one. */
      ps_xml_prop_iter = ezxml_next( ps_xml_prop_iter );
   }

   /* Load the mobile's emotions. */
   mobile_load_emotion(
      ps_mob_out,
      ezxml_child( ps_xml_mob, "emotion" )
   );

   /* Mobiles should have a image drawing information. */
   if( 0 >= ps_mob_out->pixel_size ) {
      ps_mob_out->pixel_size = GFX_TILE_DEFAULT_SIZE;
   }
   if( 0 >= ps_mob_out->pixel_multiplier ) {
      ps_mob_out->pixel_multiplier = 1.0f;
   }

   /* Mobiles must always have a proper name. */
   if( NULL == ps_mob_out->proper_name ) {
      ps_mob_out->proper_name = bformat( "???" );
   }

   DBG_INFO_STR( "Mobile loaded", ps_mob_out->mobile_type->data );

   /* Clean up. */

mlm_cleanup:

   return ps_mob_out;
}

/* Purpose: Load the emotions for a given mobile.                             */
/* Parameters: The mobile to load the emotions into and the XML tree from     *
 *             which to load the emotions.                                    */
/* Return: A boolean value indicating success.                                */
BOOL mobile_load_emotion( MOBILE_MOBILE* ps_mob_in, ezxml_t ps_xml_emotes_in ) {
   ezxml_t ps_xml_emote_iter = NULL;
   MOBILE_EMOTION s_emotion_tmp;
   BOOL b_success = TRUE;
   char* pc_attr = NULL;
   bstring ps_attr_temp = NULL;

   ps_attr_temp = bfromcstr( "" );

   /* Build the actor's list of emotions and their portraits. */
   ps_xml_emote_iter = ezxml_child( ps_xml_emotes_in, "emotion" );
   while( NULL != ps_xml_emote_iter ) {
      memset( &s_emotion_tmp, 0, sizeof( MOBILE_EMOTION ) );

      /* ATTRIB: ID */
      pc_attr = ezxml_attr( ps_xml_emote_iter, "id" );
      if( NULL == pc_attr ) {
         /* There's no ID, so this emotion is useless. */
         DBG_ERR( "ID not found; invalid emotion." );
         ps_xml_emote_iter = ezxml_next( ps_xml_emote_iter );
         continue;
      }
      s_emotion_tmp.id = atoi( pc_attr );

      /* ATTRIB: IMAGE */
      pc_attr = ezxml_txt( ps_xml_emote_iter );
      bassignformat(
         ps_attr_temp,
         "%svnprt_%s_%s.%s", PATH_SHARE, ps_mob_in->mobile_type->data,
            pc_attr, FILE_EXTENSION_IMAGE
      );
      btolower( ps_attr_temp );
      s_emotion_tmp.image = graphics_create_image( ps_attr_temp );
      if( NULL == pc_attr || NULL == s_emotion_tmp.image ) {
         /* There's no image, so this emotion is useless. */
         DBG_ERR( "Portrait not found; invalid emotion." );
         ps_xml_emote_iter = ezxml_next( ps_xml_emote_iter );
         continue;
      }

      /* If we've made it this far, the emotion is probably valid, so add  *
       * it to the current actor's list.                                   */
      UTIL_ARRAY_ADD(
         MOBILE_EMOTION, ps_mob_in->emotions,
         ps_mob_in->emotions_count, mle_cleanup, &s_emotion_tmp
      );

      DBG_INFO_INT( "Loaded emotion", s_emotion_tmp.id );

      /* Go on to the next one. */
      ps_xml_emote_iter = ezxml_next( ps_xml_emote_iter );
   }

mle_cleanup:

   return b_success;
}

/* Purpose: Load the given script into the given mobile's selected AI.        */
/* Parameters: The mobile to load into, the ID of the list to load into, and  *
 *             the path to the file from which to load the ai.                */
void mobile_load_ai(
   MOBILE_MOBILE* ps_mob_in,
   MOBILE_AI i_list_in,
   bstring ps_path_in
) {

}

/* Purpose: Draw the given mobile to the screen if it's within the current    *
 *          viewport.                                                         */
/* Parameters: The mobile to draw, the current viewport.                      */
void mobile_draw( MOBILE_MOBILE* ps_mob_in, GFX_RECTANGLE* ps_viewport_in ) {
   GFX_RECTANGLE s_tile_rect, s_screen_rect; /* Blit the tile from/to. */
   static int ti_anim_frame = 0;
   static int ti_frame_draws = 0;

   /* What animation frame are we on? */
   ti_frame_draws++;
   if( MOBILE_ANIM_DRAWS <= ti_frame_draws ) {
      if( MOBILE_ANIM_FRAMES <= ti_anim_frame ) {
         ti_anim_frame = 0;
      } else {
         ti_anim_frame++;
      }
      ti_frame_draws = 0;
   }

   /* Setup the blit source and destination. */
   s_screen_rect.w = ps_mob_in->pixel_size;
   s_screen_rect.h = ps_mob_in->pixel_size;
   s_tile_rect.w = ps_mob_in->pixel_size;
   s_tile_rect.h = ps_mob_in->pixel_size;

   /* Only draw the mobile if it's visible. */
   if(
      ps_mob_in->pixel_x < (ps_viewport_in->x + ps_viewport_in->w) &&
      ps_mob_in->pixel_y < (ps_viewport_in->y + ps_viewport_in->h) &&
      ps_mob_in->pixel_x >= ps_viewport_in->x &&
      ps_mob_in->pixel_y >= ps_viewport_in->y
   ) {
      /* Figure out the offset of the tile onscreen. */
      /* TODO: Center the mobile. */
      s_screen_rect.x = ps_mob_in->pixel_x - ps_viewport_in->x;
      s_screen_rect.y = ps_mob_in->pixel_y - ps_viewport_in->y;

      /* Figure out where on the spritesheet the sprite is. */
      if( 2 == ti_anim_frame ) {
         /* On frame 3, use the first frame again. */
         s_tile_rect.x = 0 * ps_mob_in->pixel_size;
      } else if( 3 == ti_anim_frame ) {
         /* On frame 4, use the third frame. */
         s_tile_rect.x = 2 * ps_mob_in->pixel_size;
      } else {
         s_tile_rect.x = ti_anim_frame * ps_mob_in->pixel_size;
      }
      s_tile_rect.y = ps_mob_in->current_animation * ps_mob_in->pixel_size;

      /* Draw the tile! */
      graphics_draw_blit_sprite(
         ps_mob_in->spritesheet->image,
         &s_tile_rect,
         &s_screen_rect
      );
   }
}

/* Purpose: Execute the next instruction in the mobile's AI.                  */
/* Parameters: The mobile to act and the ID of the list to execute from.      */
void mobile_execute_ai( MOBILE_MOBILE* ps_mob_in, MOBILE_AI i_list_in ) {
   if( ps_mob_in->moving ) {
      /* Wait until the mobile's done acting to execute the next instruction. */
      return;
   }


}

/* Purpose: Free the given mobile's sub-pointers. Don't free the given ptr.   */
/* Parameters: The mobile pointer to clean out.                               */
void mobile_free_arr( MOBILE_MOBILE* ps_mob_in ) {
   int i; /* Loop iterator. */

   for( i = 0 ; i < ps_mob_in->emotions_count ; i++ ) {
      graphics_free_image( ps_mob_in->emotions[i].image );
   }
   free( ps_mob_in->emotions );
   bdestroy( ps_mob_in->proper_name );
   bdestroy( ps_mob_in->mobile_type );
   graphics_free_spritesheet( ps_mob_in->spritesheet );
}


/* Purpose: Free the given mobile's sub-pointers.                             */
/* Parameters: The mobile pointer to free.                                    */
void mobile_free( MOBILE_MOBILE* ps_mob_in ) {
   mobile_free_arr( ps_mob_in );
   free( ps_mob_in );
}
