/* Controller */

/* C libraries */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* hardware platform libraries */
#include <mbed.h>

/* asteroids */
#include "model.h"
#include "asteroids.h"
#include "view.h"

/* Joystick buttons*/
enum position { JLT,JRT,JUP,JDN,JCR};
DigitalIn buttons[] = {P5_0, P5_4, P5_2, P5_1, P5_3};

/* Returns the button that has just be pressed */
bool btnPressed(enum position b){
    return !buttons[b];
}

bool jsPrsdAndRlsd(enum position b) { 			/*function to initialise buttons*/
	bool result = false;
	uint32_t state;
	static uint32_t savedState[5] = {1,1,1,1,1};
        //initially all 1s: nothing pressed
	state = buttons[b].read();
  if ((savedState[b] == 0) && (state == 1)) {
		result = true;
	}
	savedState[b] = state;
	return result;
}

/* Main game controls for the ship controls. Ship-heading controls the ship angle 
   - Left or right changes top ship point.
	 - Up adds acceleration.
*/
void gameControls(void) {
	  if(btnPressed(JLT)){
			ship_heading=ship_heading+15; 
    } 
		else if(btnPressed(JRT)){
			ship_heading=ship_heading-15;
		} 
		else if(btnPressed(JUP)){
			if(acceleration<2){
				acceleration=acceleration+0.2;
			}
		}
		// Fires a missile
		if (jsPrsdAndRlsd(JCR)) {
			fireMissle();
		}
}

/* Used to start the game from the start screen */
bool start(void){
	if(playing == false && btnPressed(JCR)){
		playing = true;
		return true;
	}
	return false;
}

/* Restarts the game when the game is over */
bool startagain(void){
	if(lives == -1 && btnPressed(JCR)){
		lives = 3;
		playing = true;
		return true;
	}
	return false;
}
