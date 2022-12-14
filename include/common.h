#ifndef __COMMON_H__
#define __COMMON_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <errno.h>
#include <dirent.h>
#include <ncurses.h>
#include <pthread.h>

/************************* DEFINES *************************/

/**
 * Key settings.
*/
#define KEY_ENTR    10
#define KEY_ESC     27
#define KEY_SPACE   32

/**
 * Ncurses windows sizes settings.
*/
#define MAP_LINES       40
#define MAP_COLS        80
#define BAR_SIZE        18
#define STM_BAR_SIZE    (MAP_LINES - (DIST_SIZE + STM_BAR_PAD_T + STM_BAR_PAD_B) + 1)
#define STM_BAR_WIDTH   7
#define STM_BAR_PAD_T   2
#define STM_BAR_PAD_B   6
#define STM_BAR_PAD_L   ((BAR_SIZE - STM_BAR_WIDTH) / 2)
#define DIST_SIZE       13

/**
 * Colors for ncurses character output.
*/
#define FORMAT_COLOR_EMPTY      1
#define FORMAT_COLOR_WHITE      2
#define FORMAT_COLOR_GREEN      3
#define FORMAT_COLOR_CYAN       4
#define FORMAT_COLOR_YELLOW     5
#define FORMAT_COLOR_RED        6
#define FORMAT_COLOR_MAGENTA    7

#define FORMAT_BGCOLOR_GREEN    8
#define FORMAT_BGCOLOR_YELLOW   9
#define FORMAT_BGCOLOR_RED      10

#define FORMAT_COLOR_WHITE_BG_RED   11

/**
 * Special UTF-8 characters for ncurses output
*/
#define PATH_VISITED_CHAR       ('.')
#define PATH_REPLAY_CHAR        ('+')
#define BONUS_CHAR              ("\u2726")          // star 2726
#define PLAYER_CHAR             ("\U0001F397")      // mdedal 1F396; ribbon 1F397; chessking 265A;
#define ENEMY_HOUSE_CHAR        ("\u26EB")          // trigram for wind 2634
#define ENEMY_BROKEN_HOUSE_CHAR ("\U0001D13C")      // half rest 1D13C
#define ENEMY_CHAR              ("\u2620")          // skull 2620;
#define OBSTACLE_CHAR           ("X")               // chains 26D3; triforce 26FC;
#define FLAG_CHAR               ("\u2691")          // flag 2691;

/**
 * Path of directories and files extensions.
*/
#define SAVES_DIR_PATH          "./data/saves/"
#define HIST_EXT                ".hist"
#define SAVE_EXT                ".save"


#endif