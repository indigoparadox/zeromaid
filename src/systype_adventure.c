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

#include "systype_adventure.h"

DBG_ENABLE

/* = Global Variables = */

extern CACHE_CACHE* gps_cache;

/* = Functions = */

/* Purpose: Adventure game loop.                                              */
/* Return: The code for the next action to take.                              */
int systype_adventure_loop( bstring ps_map_name_in ) {
   int i_act_return = RETURN_ACTION_TITLE,
      i = 0, /* Loop iterator. */
      i_mobile_count = 0;
   bstring ps_map_path =
      bformat( "%smap_%s_map.tmx", PATH_SHARE , ps_map_name_in->data );
   TILEMAP_TILEMAP* ps_map = tilemap_create_map( ps_map_path );
   free( ps_map_path );
   GFX_COLOR* ps_color_fade = graphics_create_color( 0, 0, 0 );
   GFX_COLOR* ps_color_bg = graphics_create_color( 255, 0, 0 );
   GFX_RECTANGLE s_viewport;
   MOBILE_MOBILE* as_mobile_list;
   #ifndef USESDL
   EVENT_TIMER* ps_fps = event_timer_create();
   #endif /* USESDL */

   /* Setup structures we need to run. */
   s_viewport.x = 0;
   s_viewport.y = 0;
   s_viewport.w = GFX_SCREENWIDTH;
   s_viewport.h = GFX_SCREENHEIGHT;

   /* Load the mobiles for this field. */
   as_mobile_list =
      systype_adventure_load_mobiles( ps_map_name_in, &i_mobile_count );

   /* TODO: Check the scratch file for the game we're supposed to load. */

   /* Draw the initial playing field and fade the screen in. */
   tilemap_draw( ps_map, &s_viewport );
   //graphics_do_update();
   graphics_draw_transition( GFX_TRANS_FADE_IN, ps_color_fade );
   DBG_INFO( "Running adventure game loop..." );

   while( 1 ) {
      #ifdef USESDL
      /* SDL uses its own sleep function. */
      #else
      /* Run the title screen menu input wait loop. */
      event_timer_start( ps_fps );
      #endif /* USESDL */

      /* Listen for events. */
      switch( event_do_poll() ) {
         case EVENT_ID_UP:
            s_viewport.y -= 32;
            break;

         case EVENT_ID_DOWN:
            s_viewport.y += 32;
            break;

         case EVENT_ID_RIGHT:
            s_viewport.x += 32;
            break;

         case EVENT_ID_LEFT:
            s_viewport.x -= 32;
            break;

         case EVENT_ID_ESC:
         case EVENT_ID_QUIT:
            gps_cache->game_type = SYSTEM_TYPE_TITLE;
            goto sal_cleanup;
      }

      /* Loop through and draw all on-screen items. */
      tilemap_draw( ps_map, &s_viewport ); /* Map. */
      for( i = 0 ; i < i_mobile_count ; i++ ) {
         mobile_draw( &as_mobile_list[i], &s_viewport ); /* NPCs */
      }
      for( i = 0 ; i < gps_cache->player_team_count ; i++ ) {
         mobile_draw( &gps_cache->player_team[i], &s_viewport ); /* PCs */
      }

      graphics_do_update();

      /* If possible, try to delay without busy-spinning. */
      #ifdef USESDL
      SDL_Delay( 1000 / GFX_FPS );
      #else
      while( GFX_FPS > ps_fps->i_ticks_start );
      #endif /* USESDL */
   }

sal_cleanup:

   /* Fade out the playing field screen. */
   graphics_draw_transition( GFX_TRANS_FADE_OUT, ps_color_fade );

   /* TODO: Perform the between-level autosave. */

   /* Clean up! */
   #ifdef USESDL
   #else
   event_timer_free( ps_fps );
   #endif /* USESDL */
   free( ps_color_fade );
   free( ps_color_bg );
   tilemap_free( ps_map );
   for( i = 0 ; i < i_mobile_count ; i++ ) {
      mobile_free_arr( &as_mobile_list[i] );
   }
   free( as_mobile_list );

   return i_act_return;
}

/* Purpose: Load the mobiles for this map into the given dynamic array.       */
/* Parameters: The address of number of mobiles in the output array.          */
/* Return: The address of the output array.                                   */
MOBILE_MOBILE* systype_adventure_load_mobiles(
   bstring ps_map_name_in,
   int* i_count_out
) {
   ezxml_t ps_xml_mobiles = NULL, ps_xml_mob_iter = NULL;
   MOBILE_MOBILE* ps_mobs_out = NULL;
   bstring ps_mob_xml_path =
      bformat( "%smap_%s_mobiles.xml", PATH_SHARE , ps_map_name_in->data );
   bstring ps_mob_iter_path = NULL; /* The path to load iter mob data. */

   *i_count_out = 0;

   /* Verify the XML file exists and open or abort accordingly. */
   if( !file_exists( ps_mob_xml_path ) ) {
      DBG_ERR_STR( "Unable to load mobile list", ps_mob_xml_path->data );
      return NULL;
   }
   ps_xml_mobiles = ezxml_parse_file( ps_mob_xml_path->data );

   ps_xml_mob_iter = ezxml_child( ps_xml_mobiles, "mobile" );
   while( NULL != ps_xml_mob_iter ) {
      /* A tile was found in the XML, so prepare the array for it. */
      *i_count_out += 1;
      ps_mobs_out = realloc(
         ps_mobs_out,
         *i_count_out * sizeof( MOBILE_MOBILE )
      );

      /* Verify the last created map tile's allocation. */
      if( NULL == ps_mobs_out ) {
         DBG_ERR( "Unable to allocate mobile list." );
         goto salm_cleanup;
      }
      memset( &ps_mobs_out[*i_count_out - 1], 0, sizeof( MOBILE_MOBILE ) );

      /* Load the tile's properties. */
      ps_mobs_out[*i_count_out - 1].tile_x =
         atoi( ezxml_attr( ps_xml_mob_iter, "startx" ) );
      ps_mobs_out[*i_count_out - 1].tile_y =
         atoi( ezxml_attr( ps_xml_mob_iter, "starty" ) );
      ps_mobs_out[*i_count_out - 1].mobile_type =
         bformat( "%s", ezxml_attr( ps_xml_mob_iter, "type" ) );

      ps_mob_iter_path = bformat(
         "%smob_%s.xml",
         PATH_SHARE,
         ps_mobs_out[*i_count_out - 1].mobile_type->data
      );
      mobile_load_mobile(
         &ps_mobs_out[*i_count_out - 1],
         ps_mob_iter_path
      );
      bdestroy( ps_mob_iter_path );

      /* Go to the next one! */
      ps_xml_mob_iter = ezxml_next( ps_xml_mob_iter );
   }

salm_cleanup:
   /* Clean up. */
   ezxml_free( ps_xml_mobiles );

   return ps_mobs_out;
}
