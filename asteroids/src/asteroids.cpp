/* Asteroids

Henry Williams and Andy White 
Last Modified: 11/05/2017*/

/* C libraries */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

/* hardware platform libraries */
#include <display.h>
#include <mbed.h>

/* Main game elements */
#include "model.h"
#include "view.h"
#include "controller.h"
#include "utils.h"

float Dt = 0.01f;
Ticker model, view, controller, asteroids;
bool paused = true;
bool playing;

int main() {
	init_DBuffer();
	initialise_heap();
	initialise_asteroid_heap();
	initialiseGame();
	lives = 3;
	view.attach(drawGameStartScreen,1.0/50.0);
	
    /* Pause to start */
    while(1){ 	
			if (lives <= -1) {
				view.detach();
				model.detach();
			
			}
			if (startagain()) {
				lives = 3;
				initialiseGame();
				total_timeAlive = 0;
				model.attach(physics,Dt);
				view.attach(draw,0.050); //draw game screen
				controller.attach( gameControls, 0.1);//gameControls for main game to control ship
			}

			if (start()) {
				view.detach();//clear start screen
				view.attach(draw, 0.050); //draw game screen
				model.attach(physics,Dt);
				controller.attach( gameControls, 0.1);//gameControls for main game to control ship
			}	
    }
}
