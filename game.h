#ifndef AOE_GAME_H
#define AOE_GAME_H

#include <stdint.h>
#include "engine.h"
#include "ship.h"
#include "gfx.h"
#include "sfx.h"
#include "log.h"

#define GAME_LOGCTL_FILE 1
#define GAME_LOGCTL_STDOUT 2

#define GE_LIB 1
#define GE_OPT 2
#define GE_TIME 3
#define GE_FOCUS 4
#define GE_ICON 5
#define GE_FULLSCREEN 6
#define GE_GFX 7
#define GE_MOUSE 8
#define GE_WINCTL2 9
#define GE_SFX 10
#define GE_CTL 11
#define GE_SFX2 12
#define GE_WINCTL 16
#define GE_PALETTE 17
#define GE_LOWOS 20
#define GE_LOWMEM 22

struct window_ctl2 {
	unsigned num0, num4, num8, numC;
};

struct game;

struct game_vtbl {
	int (*dtor)(struct game*, unsigned);
	unsigned (*main)(struct game*);
	unsigned (*get_state)(struct game*);
	char *(*get_res_str)(unsigned, char*, unsigned);
	char *(*strerr)(struct game*, int, signed, int, char*, unsigned);
	int (*parse_opt)(struct game*);
	signed (*init_icon)(struct game*);
	int (*go_fullscreen)(struct game*);
	int (*gfx_init)(struct game*);
	int (*set_palette)(struct game*);
	int (*shp)(struct game*);
	// XXX consider inlining
	int (*translate_event)(struct game*, unsigned*);
	void (*handle_event)(struct game*, unsigned);
	int (*init_mouse)(struct game*);
};

extern struct game_vtbl g_vtbl, g_vtbl2;

#define OPTBUFSZ 256

// FIXME change char arrays to const char* if not modified anywhere
struct game_config {
	char title[48];
	unsigned reg_state;
	char version[21];
	char win_name[121];
	char tr_world_txt[137];
	unsigned window, gl;
	char langpath[32];
	char dir_empires[261];
	char strAOE[263];
	unsigned d0, d1p0, d3;
	unsigned hInst, hPrevInst;
	char reg_path[256];
	char optbuf[OPTBUFSZ];
	unsigned nshowcmd;
	char icon[41];
	char menu_name[41];
	char palette[261];
	char cursors[251];
	unsigned d8p0;
	unsigned chk_time;
	short time[3];
	unsigned d1p1;
	unsigned window_request_focus;
	unsigned no_start;
	unsigned window_show_focus_update;
	unsigned mouse_opts;
	unsigned gfx8bitchk;
	unsigned sys_memmap;
	unsigned midi_enable;
	unsigned sfx_enable;
	unsigned midi_opts[7];
	int scroll0;
	float f4_0p0;
	int scroll1;
	float f4_0p1, f0_5;
	short mouse_style;
	unsigned width, height;
	struct window_ctl2 winctl, winctl2;
	char dir_data2[244];
	//HKEY root;
	float gamespeed;
	unsigned difficulty;
	char pathfind;
	char dir_empty[261], dir_save[261];
	char dir_scene[53], dir_camp[52], dir_sound[261];
	char dir_data_2[261], dir_data_3[261];
	char dir_movies[261];
	char prompt_title[256], prompt_message[256];
};

#define CWDBUFSZ 261

struct game {
	struct game_vtbl *vtbl;
	struct game_focus *focus;
	void *num8;
	struct game_config *cfg;
	unsigned window;
	unsigned running;
	// REMAP typeof(HPALETTE palette) == void*
	struct pal_entry *palette;
	// REMAP typeof(HANDLE mutex) == void*
	unsigned mutex_state;
	unsigned vtbl28[4];
	unsigned screensaver_state;
	// REMAP typeof(PVOID powersaving) == void*
	unsigned powersaving;
	unsigned state;
	unsigned timer;
	struct video_mode *mode;
	unsigned no_normal_mouse;
	unsigned short shp_count;
	struct ship_20 shptbl[3];
	short sfx_count;
	struct clip **sfx_tbl;
	unsigned cursor_old_state;
	// REMAP typeof(HCURSOR cursor_old) == void*
	unsigned cursor_old;
	struct game18C *ptr18C;
	struct logger *log;
	unsigned logctl;
	struct regpair rpair_root;
	union reg_value rpair_rootval;
	struct cursor_cfg *curscfg;
	unsigned midi_sync;
	unsigned cursor_state;
	unsigned apply_new_cursor;
	// REMAP typeof(HCURSOR cursor) == void*
	unsigned cursor;
	unsigned window2;
	char cwdbuf[261];
	char libname[15];
	char num97D_97E_is_zero, num97E_97D_is_zero;
	char hsv[3];
	float start_gamespeed;
	char mp_pathfind;
	unsigned vtbl9A8;
	unsigned num9AC;
	unsigned tbl9B0[9];
	unsigned rollover_text;
	float gamespeed;
	unsigned difficulty;
	char pathfind;
	char tblA15[11];
	struct map *map_area;
	unsigned brightness;
};

struct game *game_ctor(struct game *this, struct game_config *cfg, int should_start_game);

#endif