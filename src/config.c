#include "config.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include "dbg.h"
#include "prompt.h"

#define CFGSUFFIX "/.aoerc"

static const char *homedir = NULL;
static char *cfgname = NULL;
static int custom_root = 0;

struct config reg_cfg = {
	.screen_size = 1280,
	.rollover_text = 1,
	.mouse_style = 2,
	.custom_mouse = 0,
	.sfx_volume = 0,
	.game_speed = 15,
	.difficulty = 5,
	.scroll_speed = 84,
	.pathfind = 3,
	.mp_pathfind = 3,
	.root_path = NULL,
	.flags = 0,
	.display = 0,
	.mode_fixed = RES_HIGH,
};

static const char *cfg_default =
	"# Age of Empires configuration file\n"
	"screen_size = 1280\n"
	"rollover_text = 1\n"
	"mouse_style = 2\n"
	"custom_mouse = 0\n"
	"sfx_volume = 0\n"
	"game_speed = 15\n"
	"difficulty = 5\n"
	"scroll_speed = 84\n"
	"pathfind = 3\n"
	"mp_pathfind = 3\n"
	"display = 0\n"
	"fullscreen = no\n";

void config_free(void)
{
	if (cfgname) {
		config_save();
		free(cfgname);
		cfgname = NULL;
	}
}

/* try to access config file and create if it does not exist */
static int cfg_touch(void)
{
	struct stat st;
	int ret = 0, fd = -1;
	if (stat(cfgname, &st)) {
		fd = creat(cfgname, 0600);
		if (fd == -1) goto fail;
		dbgs("create/replace config file");
		size_t n = strlen(cfg_default);
		ssize_t out = write(fd, cfg_default, n);
		if (out < 0 || (size_t)out != n) {
			fputs("failed to create config file\n", stderr);
			goto fail;
		}
	}
	ret = 1;
fail:
	if (fd != -1) close(fd);
	return ret;
}

int config_init(void)
{
	homedir = getenv("HOME");
	if (!homedir) {
		struct passwd *pwd = getpwuid(getuid());
		if (pwd)
			homedir = pwd->pw_dir;
		else {
			fputs("failed to get pwd\n", stderr);
			return 0;
		}
	}
	if (!strncmp(homedir, "/root", 5) || !getuid()) {
		fputs("do not run this as root\n", stderr);
		return 0;
	}
	cfgname = malloc(strlen(homedir) + strlen(CFGSUFFIX) + 1);
	if (!cfgname) {
		fputs("out of memory\n", stderr);
		return 0;
	}
	sprintf(cfgname, "%s%s", homedir, CFGSUFFIX);
	return cfg_touch();
}

/* NOTE we have about 10 options so we don't have to optimize this */
static int cfg_update(unsigned i, const char *arg, const char *val, char **path)
{
	char err[80] = "internal error";
	if (!strcmp(arg, "screen_size")) {
		int size = atoi(val);
		if (size < 400) {
			snprintf(err, sizeof err, "bad screen size: %s (minimum=400)", val);
			goto fail;
		}
		if (size > 1920) {
			snprintf(err, sizeof err, "bad screen size: %s (maximum=1920)", val);
			goto fail;
		}
		reg_cfg.screen_size = size;
	} else if (!strcmp(arg, "rollover_text")) {
		int text = atoi(val);
		if (text < 0) {
			snprintf(err, sizeof err, "bad rollover text: %s (minimum=0)", val);
			goto fail;
		}
		if (text > 2) {
			snprintf(err, sizeof err, "bad rollover text: %s (maximum=2)", val);
			goto fail;
		}
		reg_cfg.rollover_text = text;
	} else if (!strcmp(arg, "mouse_style")) {
		int style = atoi(val);
		if (style < 1) {
			snprintf(err, sizeof err, "bad mouse style: %s (minimum=1)", val);
			goto fail;
		}
		if (style > 2) {
			snprintf(err, sizeof err, "bad mouse style: %s (maximum=2)", val);
			goto fail;
		}
		reg_cfg.mouse_style = style;
	} else if (!strcmp(arg, "custom_mouse")) {
		int custom = atoi(val);
		if (custom < 0) {
			snprintf(err, sizeof err, "bad custom mouse: %s (minimum=0)", val);
			goto fail;
		}
		if (custom > 2) {
			snprintf(err, sizeof err, "bad custom mouse: %s (maximum=2)", val);
			goto fail;
		}
		reg_cfg.custom_mouse = custom;
	} else if (!strcmp(arg, "sfx_volume")) {
		int volume = atoi(val);
		if (volume < 0) {
			snprintf(err, sizeof err, "bad sound effects volume: %s (minimum=0)", val);
			goto fail;
		}
		reg_cfg.sfx_volume = volume;
	} else if (!strcmp(arg, "game_speed")) {
		int speed = atoi(val);
		if (speed < 1) {
			snprintf(err, sizeof err, "bad game speed: %s (minimum=1)", val);
			goto fail;
		}
		reg_cfg.game_speed = speed;
	} else if (!strcmp(arg, "difficulty")) {
		int diff = atoi(val);
		if (diff < 0) {
			snprintf(err, sizeof err, "bad difficulty: %s (minimum=0)", val);
			goto fail;
		}
		if (diff > 5) {
			snprintf(err, sizeof err, "bad difficulty: %s (maximum=5)", val);
			goto fail;
		}
		reg_cfg.difficulty = diff;
	} else if (!strcmp(arg, "scroll_speed")) {
		int speed = atoi(val);
		if (speed < 1) {
			snprintf(err, sizeof err, "bad scroll speed: %s (minimum=1)", val);
			goto fail;
		}
		reg_cfg.scroll_speed = speed;
	} else if (!strcmp(arg, "pathfind")) {
		int find = atoi(val);
		if (find < 1) {
			snprintf(err, sizeof err, "bad pathfind: %s (minimum=1)", val);
			goto fail;
		}
		if (find > 127) {
			snprintf(err, sizeof err, "bad pathfind: %s (maximum=127)", val);
			goto fail;
		}
		reg_cfg.pathfind = find;
	} else if (!strcmp(arg, "mp_pathfind")) {
		int find = atoi(val);
		if (find < 1) {
			snprintf(err, sizeof err, "bad multiplayer pathfind: %s (minimum=1)", val);
			goto fail;
		}
		if (find > 127) {
			snprintf(err, sizeof err, "bad multiplayer pathfind: %s (maximum=127)", val);
			goto fail;
		}
		reg_cfg.mp_pathfind = find;
	} else if (!strcmp(arg, "root_path")) {
		if (!*path) {
			reg_cfg.root_path = *path = strdup(val);
			custom_root = 1;
			dbgf("root_path: %s\n", val);
		}
	} else if (!strcmp(arg, "fullscreen")) {
		int enable = atoi(val);
		if (enable)
			reg_cfg.flags |= CFG_FULL;
		else
			reg_cfg.flags &= ~CFG_FULL;
	} else {
		snprintf(err, sizeof err, "invalid option: %s", arg);
fail:
		fprintf(stderr, "syntax error at line %u: %s\n", i, err);
		return 0;
	}
	return 1;
}

/*
read and parse configuration file
format is very simple:
* use # for comments
* whitespace is ignored
* specify options using option=value

errors are printed to stderr and the file will
be recreated if syntax errors have occurred.
*/
int config_load(char **path)
{
	int ret = 0;
	char *line = NULL;
	FILE *f = NULL;
	size_t len;
	ssize_t read;
	if (*path)
		custom_root = 1;
	if (!cfg_touch())
		goto fail;
	f = fopen(cfgname, "r");
	if (!f) goto fail;
	unsigned i = 0;
	while ((read = getline(&line, &len, f)) >= 0) {
		++i;
		unsigned val = 0, arg = 0, arg_end, n = (size_t)read;
		if (n) line[n - 1] = '\0';
		while (isspace(line[arg]))
			++arg;
		if (arg >= n || !line[arg] || line[arg] == '#') continue;
		char *sep = strchr(line + arg, '=');
		if (!sep) {
			fprintf(stderr, "syntax error at line %u: missing '='\n", i);
			goto nuke;
		}
		arg_end = arg;
		while (line[arg_end] != '=' && !isspace(line[arg_end]))
			++arg_end;
		val = (unsigned)(sep - line + 1);
		while (val < n && isspace(line[val]))
			++val;
		line[arg_end] = '\0';
		if (!cfg_update(i, line + arg, line + val, path))
			goto nuke;
	}
end:
	ret = 1;
fail:
	if (!ret) {
		char buf[256];
		snprintf(buf, sizeof buf, "%s: %s\n", cfgname, strerror(errno));
		show_error("Configuration error", buf);
	}
	if (line) free(line);
	if (f) fclose(f);
	return ret;
nuke:
	if (f) {
		fclose(f);
		f = NULL;
	}
	if (unlink(cfgname) == -1 || !cfg_touch())
		goto fail;
	goto end;
}

int config_save(void)
{
	int ret = 0;
	FILE *f = NULL;
	if (!cfg_touch()) goto fail;
	f = fopen(cfgname, "w");
	if (!f) {
		perror(cfgname);
		goto fail;
	}
	fprintf(f,
		"# Age of Empires configuration file\n"
		"screen_size = %u\n"
		"rollover_text = %u\n"
		"mouse_style = %u\n"
		"custom_mouse = %u\n"
		"sfx_volume = %u\n"
		"game_speed = %u\n"
		"difficulty = %u\n"
		"scroll_speed = %u\n"
		"pathfind = %hhu\n"
		"mp_pathfind = %hhu\n",
		reg_cfg.screen_size,
		reg_cfg.rollover_text,
		reg_cfg.mouse_style,
		reg_cfg.custom_mouse,
		reg_cfg.sfx_volume,
		reg_cfg.game_speed,
		reg_cfg.difficulty,
		reg_cfg.scroll_speed,
		reg_cfg.pathfind,
		reg_cfg.mp_pathfind
	);
	if (custom_root)
		fprintf(f, "root_path = %s\n", reg_cfg.root_path);
	fprintf(f,
		"fullscreen = %u\n",
		reg_cfg.flags & CFG_FULL ? 1 : 0
	);
	ret = 1;
fail:
	if (f) fclose(f);
	return ret;
}
