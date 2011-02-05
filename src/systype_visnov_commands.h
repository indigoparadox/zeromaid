
typedef union {
   int null;   /* Can be used to space out DIs that can't be reconciled. */
   GFX_COLOR
      * color_fg,    /* DI 2 - MENU */
      * color_bg,    /* DI 3 - MENU */
      * color_sfg,   /* DI 4 - MENU */
      * color_sbg;   /* DI 5 - MENU */
   bstring name,     /* DI 0 - LABEL */
      emotion,       /* DI 1 - PORTRAIT */
      key,           /* DI 2 - COND, SET */
      equals,        /* DI 3 - COND, SET */
      destmap,       /* DI 0 - TELEPORT */
      talktext,      /* DI 1 - TALK */
      target,        /* DI 0 - GOTO, COND */
      items,         /* DI 0 - MENU */
      type;          /* DI 3 - TELEPORT */
   COND_SCOPE scope; /* DI 1 - COND, MENU, SET */
   GFX_SURFACE* bg;  /* DI 0 - BACKGROUND */
   int serial,       /* DI 0 - PORTRAIT, TALK */
      x,             /* DI 3 - PORTRAIT */
      y,             /* DI 4 - PORTRAIT */
      delay,         /* DI 0 - PAUSE*/
      destx,         /* DI 1 - TELEPORT */
      desty,         /* DI 2 - TELEPORT */
      speed;         /* DI 2 - TALK */
   float zoom;       /* DI 2 - PORTRAIT */
} SYSTYPE_VISNOV_DATA;

typedef int SYSTYPE_VISNOV_CMD;
#define SYSTYPE_VISNOV_CMD_NULL 0
#define SYSTYPE_VISNOV_CMD_BACKGROUND 1
#define SYSTYPE_VISNOV_CMD_BACKGROUND_DC 1
#define SYSTYPE_VISNOV_CMD_PAUSE 2
#define SYSTYPE_VISNOV_CMD_PAUSE_DC 1
#define SYSTYPE_VISNOV_CMD_LABEL 3
#define SYSTYPE_VISNOV_CMD_LABEL_DC 1
#define SYSTYPE_VISNOV_CMD_COND 4
#define SYSTYPE_VISNOV_CMD_COND_DC 3
#define SYSTYPE_VISNOV_CMD_TALK 5
#define SYSTYPE_VISNOV_CMD_TALK_DC 3
#define SYSTYPE_VISNOV_CMD_GOTO 6
#define SYSTYPE_VISNOV_CMD_GOTO_DC 1
#define SYSTYPE_VISNOV_CMD_PORTRAIT 7
#define SYSTYPE_VISNOV_CMD_PORTRAIT_DC 5
#define SYSTYPE_VISNOV_CMD_TELEPORT 8
#define SYSTYPE_VISNOV_CMD_TELEPORT_DC 4
#define SYSTYPE_VISNOV_CMD_MENU 9
#define SYSTYPE_VISNOV_CMD_MENU_DC 6
#define SYSTYPE_VISNOV_CMD_SET 10
#define SYSTYPE_VISNOV_CMD_SET_DC 4

/* TODO: MUSIC, MENU */

typedef struct {
   SYSTYPE_VISNOV_COMMAND command;
   bstring image_path;
} SYSTYPE_VISNOV_COMMAND_BACKGROUND;

#define STVN_COMMAND_LOAD_BACKGROUND( cmd_struct_in, xml_cmd_in ) \
   if( 0 == strcmp( "background", ezxml_attr( xml_cmd_in, "action" ) ) ) { \
      cmd_struct_in = \
         calloc( 1, sizeof( SYSTYPE_VISNOV_COMMAND_BACKGROUND ) ); \
      STVN_PARSE_CMD_DAT_STR( bg, ((SYSTYPE_VISNOV_COMMAND_BACKGROUND*)cmd_struct_in)->image_path ); \
   }
