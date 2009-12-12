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

/* = Global Variables = */

extern CACHE_CACHE* gps_cache;

/* = Functions = */

/* Purpose: Create a MOBILE struct from the mobile data at the given path.    */
/* Parameters: The struct in which to place the new mobile, the path to the   *
 *             mobile's data file.                                            */
void mobile_load_mobile( MOBILE_MOBILE* ps_mob_out, bstring ps_path_in ) {
   ezxml_t ps_xml_mob = NULL, ps_xml_props = NULL, ps_xml_prop_iter = NULL;
   bstring ps_path_temp = NULL;

   /* Verify the XML file exists and open or abort accordingly. */
   if( !file_exists( ps_path_in ) ) {
      DBG_ERR_STR( "Unable to load mobile", ps_path_in->data );
      return;
   }
   ps_xml_mob = ezxml_parse_file( ps_path_in->data );

   /* Load the properties tree. */
   ps_xml_props = ezxml_child( ps_xml_mob, "properties" );
   ps_xml_prop_iter = ezxml_child( ps_xml_props, "property" );
   while( NULL != ps_xml_prop_iter ) {
      /* Load the current property into the struct. */
      if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "propername" ) ) {
         ps_mob_out->proper_name =
            bformat( "%s", ezxml_attr( ps_xml_prop_iter, "value" ) );
         DBG_INFO_STR( "Mobile proper name", ps_mob_out->proper_name->data );

      } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "hp" ) ) {
         ps_mob_out->hp = atoi( ezxml_attr( ps_xml_prop_iter, "value" ) );
         DBG_INFO_NUM( "Mobile HP", ps_mob_out->hp );

      } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "spritesheet" ) ) {
         ps_path_temp = bformat(
            "%smob_sprites_%s.%s",
            PATH_SHARE,
            ezxml_attr( ps_xml_prop_iter, "value" ),
            FILE_EXTENSION_IMAGE
         );
         ps_mob_out->spritesheet = graphics_create_spritesheet( ps_path_temp );
         bdestroy( ps_path_temp );

      } else if( 0 == strcmp( ezxml_attr( ps_xml_prop_iter, "name" ), "ai_adv_normal" ) ) {
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

   /* Mobiles should have a image drawing information. */
   if( 0 >= ps_mob_out->pixel_size ) {
      ps_mob_out->pixel_size = GFX_TILE_DEFAULT_SIZE;
   }
   if( 0 >= ps_mob_out->pixel_multiplier ) {
      ps_mob_out->pixel_multiplier = 1.0f;
   }

   /* Mobiles should have a sprite sheet. */
   if( NULL == ps_mob_out->spritesheet ) {
      DBG_ERR( "Mobile validation failed: No sprite sheet!" );
      mobile_free_arr( ps_mob_out );
      goto mlm_cleanup;
   }

   /* Mobiles must always have a proper name. */
   if( NULL == ps_mob_out->proper_name ) {
      ps_mob_out->proper_name = bformat( "???" );
   }

   DBG_INFO_STR( "Mobile loaded", ps_mob_out->mobile_type->data );

   /* Clean up. */

mlm_cleanup:

   ezxml_free( ps_xml_mob );
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

   for( i = 0 ; i < ps_mob_in->portraits_count ; i++ ) {
      graphics_free_image( ps_mob_in->portraits[i].image );
   }
   free( ps_mob_in->portraits );
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
