/* This file is part of Maid Quest.                                        *
 * Maid Quest is free software; you can redistribute it and/or modify it   *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or (at   *
 * your option) any later version.                                         *
 *                                                                         *
 * Maid Quest is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANT-    *
 * ABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public *
 * License for more details.                                               *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with Maid Quest; if not, write to the Free Software Foundation, Inc.,   *
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA                  */

#ifndef MQ_DEFS_H
#define MQ_DEFS_H

#include <iostream>
#include <string>
#include <vector>

#include "tinyxml/tinyxml.h"

/* #ifdef USESDL
#ifdef __APPLE__
   #include <SDL/SDL.h>
#elif defined __unix__
   #include <SDL/SDL.h>
#else
   #include <SDL.h>
#endif
#endif */ // USESDL

const std::string CONSTR_BLANK = "";

const int MAX_FPS = 15;
const int MAX_FRAME_SHOWTIME = 5;
const int MAX_SCREEN_WIDTH = 640;
const int MAX_SCREEN_HEIGHT = 480;
const int MAX_FIELD_WIDTH = 512;
const int MAX_FIELD_HEIGHT = 416;
const int MAX_EVTREPEAT = 1; // It's small but it's enough!

// Some of these have to be different for optimal performance on different
// platforms.
#ifdef __APPLE__

#ifdef USEALLEGRO
const int MAX_SCREEN_DEPTH = 16;
//const bool MAX_SCREEN_FULL = false;
#elif defined USESDL
const int MAX_SCREEN_DEPTH = 16;
//const bool MAX_SCREEN_FULL = false;
#endif

#else

#ifdef USEALLEGRO
const int MAX_SCREEN_DEPTH = 8;
//const bool MAX_SCREEN_FULL = false;
#elif defined USESDL
const int MAX_SCREEN_DEPTH = 16;
//const bool MAX_SCREEN_FULL = false;
#endif

#endif // __APPLE__

/* const int FNTSIZE_MENU_SEL = 24;
const int FNTSIZE_WINDOW = 24; */

const int STEP_SHIFT_HORIZONTAL = 128;
const int STEP_SHIFT_VERTICAL = 104;

#define SIZE_WALK_STEP 8 /* Should be evenly divisible by 32. */
#define SIZE_TILE 32

const std::string VERSION = "0.3";

const std::string DFILE_MUSIC_TITLE = "mus-title";
const std::string DFILE_SOUND_MENUSEL = "sfx-cowbell.wav";
const std::string DFILE_SCREEN_TITLE = "screen_title.bmp";
const std::string DFILE_SCREEN_LOADING = "screen_title.bmp";
const std::string DFILE_SCREEN_MENU = "screen_menu.bmp";
const std::string DFILE_SCREEN_WINDOW = "screen_window.bmp";
const std::string DFILE_FONT_SYSTEM = "font_artillum";
const std::string DFILE_PREFS = "gamedata/optiondata.xml";

// #ifdef __WIN32__
const std::string DPATH_ROOT = "";
/* #elif defined __unix__
const std::string CONF_ROOT = "/usr/local/";
const std::string DPATH_ROOT = CONSTR_BLANK + CONF_ROOT + "share/maidquest/";
#elif defined __APPLE__
const std::string DPATH_ROOT = "";
#else
#error *** Could not detect current platform; could not set root directory. ***
#endif */

const std::string DPATH_MOBS = DPATH_ROOT + "mobdata/";
const std::string DPATH_MAPS = DPATH_ROOT + "mapdata/";
const std::string DPATH_MUSIC = DPATH_ROOT + "audata/";
const std::string DPATH_SOUNDS = DPATH_ROOT + "audata/";
const std::string DPATH_SCREENS = DPATH_ROOT + "scrdata/";
const std::string DPATH_ANIDATA = DPATH_ROOT + "anidata/";
const std::string DPATH_FONTS = DPATH_ROOT + "scrdata/";

const std::string SUFFIX_GRX = ".bmp";
const std::string SUFFIX_MAP = ".tmx";
const std::string SUFFIX_MOB = ".xml";
const std::string SUFFIX_MUS = ""; // Varies based on platform.

#ifdef USENET
//const std::string NET_DEFAULT_REMHOST = "quartznet.homedns.org";
const std::string NET_DEFAULT_REMHOST = "amethyst";
const int NET_DEFAULT_REMPORT = 60255;
#endif /* USENET */

#define AI_ACTION_WAIT 15

// #define AI_DEFAULT_FRIENDLY false
// #define AI_DEFAULT_HP 100
// #define AI_DEFAULT_AITYPE AI_NONE
// #define AI_DEFAULT_TYPE "unknown"

/* -Macros-                                                                */

#define DBGERR( strCore, strMsg ) std::cerr << "ERROR (" << strCore << "): " << strMsg << std::endl;
#ifdef USEDEBUG
   #define DBGOUT( strCore, strMsg ) std::cout << "INFO (" << strCore << "): " << strMsg << std::endl;
   #define DBGONCEOUT( strCore, strMsg ) if( intDbgCount < intDbgMax || intDbgMax == 0 ) { std::cout << "INFO (" << strCore << "): " << strMsg << std::endl; intDbgCount++; }
   #define DBGONCEOPEN() int intDbgMax = 0; int intDbgCount = 0;
   #define DBGONCECLOSE() if( 0 == intDbgMax ) { intDbgMax = intDbgCount; }
#else
   #define DBGOUT( strCore, strMsg )
   #define DBGONCEOUT( strCore, strMsg )
   #define DBGONCEOPEN()
   #define DBGONCECLOSE()
#endif /* USEDEBUG */

/* -Enums-                                                                 */

enum MapDirection {
   NORTH,
   SOUTH,
   EAST,
   WEST
};

// These should line up with the MapDirection enum.
const int MapDirectionXAdd[4] = {
   0,
   0,
   1,
   -1
};

// These should line up with the MapDirection enum.
const int MapDirectionYAdd[4] = {
   -1,
   1,
   0,
   0
};

enum MobileAction {
   MACT_WALKING,
};

// These describe a column on a mobile's sprite sheet.
enum MobileSpriteCol {
   MS_COL_FALL = 0,
   MS_COL_HIT1 = 32,
   MS_COL_HIT2 = 64,
};

// These describe a row on a mobile's sprite sheet.
enum MobileSpriteRow {
   MS_ROW_FACE_SOUTH = 0,
   MS_ROW_FACE_NORTH = 32,
   MS_ROW_FACE_EAST = 64,
   MS_ROW_FACE_WEST = 96,
   MS_ROW_FALL = 128,
};

enum CommandsGameLoop {
   GCMD_NONE,
   GCMD_UP,
   GCMD_DOWN,
   GCMD_RIGHT,
   GCMD_LEFT,
   GCMD_MENU,
   GCMD_QUIT,
   GCMD_ATTACK,
   GCMD_TALK,
   GCMD_LETTER,
};

enum CommandsGameMenu {
   MCMD_NONE,
   MCMD_QUIT,
};

enum CommandsTitleLoop {
   TCMD_NONE,
   TCMD_STORY,
   TCMD_QUIT,
};

/* Purpose: These are the types of dialog windows available to create.     */
enum MQWindowType {
   MQWT_INFORM, // Informational Window
   MQWT_GETTEXT, // Text Input Dialog
};

/* -Structure Definitions-                                                 */

typedef struct {
   int intHindrance; /* The amount by which to divide a passing mobile's speed. */
   int intSheetTile; /* The index of the tile from the tile sheet to use. */
   int intTileX; /* Tile's world position in tiles. */
   int intTileY;
   // vector<ItemCore> vctItems; /* All of the items currently sitting on that tile. */
} MapTileInfo;

typedef struct {
   int intVolMusic;
   int intVolSound;
   bool bolAnimateField;
   bool bolFullScreen;
} PrefsCore;

void do_loadprefs( PrefsCore* );
void do_saveprefs( PrefsCore* );

#endif /* MQ_DEFS_H */
