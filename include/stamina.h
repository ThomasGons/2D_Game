#ifndef __STAMINA_H__
#define __STAMINA_H__


#include "common.h"
#include "structs.h"


/************************* DEFINES *************************/

/**
 * Stamina section limits for the bar.
*/
#define STAMINA_HIGH 66.66
#define STAMINA_MED 33.33
#define STAMINA_LOW 0


/************************* FUNCTIONS *************************/

/**
 * Update stamina bar value.
*/
void stamina_update_val();

/**
 * Render stamina bar.
*/
void stamina_bar_render();


#endif