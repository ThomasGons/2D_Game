#ifndef __PLAYER_H__
#define __PLAYER_H__


#include "common.h"
#include "structs.h"
#include "util.h"
#include "map.h"

/************************* DEFINES *************************/

/**
 * Key settings.
*/
#define KEY_ESC 27

/**
 * Stamina settings.
*/
#define STAMINA_EASY 100
#define STAMINA_MEDIUM 70
#define STAMINA_HARD 50

#define STAMINA_LOSS 1
#define STAMINA_LOSS_OBS 10
#define STAMINA_GAIN 10

/**
 * Movements limit before losing stamina.
*/
#define MV_LIMIT_COUNT 5


/************************* FUNCTIONS *************************/

/**
 * Initialize the player.
 * \param level difficulty of game
*/
void player_init(Level level);

/**
 * Handle inputs using ncurses library.
 * \param quit game quit indicator
*/
void player_inputs(bool *quit);

/**
 * Update player position based on inputs.
*/
void player_update();

/**
 * Check if player is colliding with map borders or obstacles.
 * \param line line of cell to be checked
 * \param column column of cell to be checked
*/
void player_check_collisions(uint8_t line, uint8_t col);

/**
 * Render player in terminal with ncurses.
*/
void player_render();

/**
 * Free allocated memory for player.
*/
void player_free();


#endif