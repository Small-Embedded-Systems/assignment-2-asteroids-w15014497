/* Asteroids view*/

/* C libraries */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

/* hardware platform libraries */
#include <display.h>
#include <mbed.h>

#include "asteroids.h"
#include "model.h"
#include "utils.h"
#include "view.h"

Display *graphics = Display::theDisplay();
const colour_t background = rgb(0,51,102); /* Midnight Blue */
const colour_t asteroid_colour = rgb(163, 163, 194); /* Midnight Blue */

//assembly function for flipping image bit pattern
extern "C" uint8_t flipBits(uint8_t);
static void flipBits();
static unsigned char flippedImage[400];

/* double buffering functions */
void init_DBuffer(void)
{   /* initialise the LCD driver to use second frame in buffer */
    uint16_t *bufferbase = graphics->getFb();
    uint16_t *nextbuffer = bufferbase+ (480*272);
    LPC_LCD->UPBASE = (uint32_t)nextbuffer;
}

void swap_DBuffer(void)
{   /* swaps frames used by the LCD driver and the graphics object */
    unsigned int b;
    b = LPC_LCD->UPBASE;
    LPC_LCD->UPBASE = (uint32_t)graphics->getFb();
    graphics->setFb((uint16_t *)b);
}

/* Game start screen*/
void drawGameStartScreen(void){
	graphics->fillScreen(background);
	graphics->setTextColor(WHITE);
	graphics->setTextSize(4);
	graphics->setCursor(140,100);
	graphics->printf("Asteroids");
	
	graphics->setTextSize(3);
	graphics->setCursor(50,180);
	graphics->printf("Press Centre to Play!");
	swap_DBuffer();
}

/* Game over screen - shows last score*/
void drawGameOverScreen(void) {
	graphics->fillScreen(background);
	graphics->setTextColor(WHITE);
	graphics->setTextSize(5);
	graphics->setCursor(100,100);
	graphics->printf("Game Over!");
	graphics->setCursor(120,150);
	graphics->printf("Score %d", total_timeAlive);
	graphics->setTextSize(2);
	swap_DBuffer();
}

/* Missiles drawn as a circle */
void drawMissiles(struct missile *lst){
    while(lst) {
        graphics->fillCircle(lst->missile_x,lst->missile_y, 1, RED);
        lst = lst->next;
    }
}

/*Ships to disapear as a life is lost*/
void drawLives(void) {
	graphics->setCursor(10,5);
	graphics->printf("Lives Left:");
	
	if (lives > 0){
		graphics->drawLine(140,7,140,17,WHITE);
		graphics->drawLine(140,7,160,12,WHITE);
		graphics->drawLine(140,17,160,12,WHITE);
	}
	if (lives > 1){
		graphics->drawLine(170,7,170,17,WHITE);
		graphics->drawLine(170,7,190,12,WHITE);
		graphics->drawLine(170,17,190,12,WHITE);
	}
	if (lives > 2){
		graphics->drawLine(200,7,200,17,WHITE);
		graphics->drawLine(200,7,220,12,WHITE);
		graphics->drawLine(200,17,220,12,WHITE);
	}	
}

/*Draws the ship a different colour (or white) depending on shield health*/
void drawShip(){	
	if(shieldHealth==3){
		/*3 hits remaining*/
	  graphics->drawLine(ship_start_x+ship_left_x,ship_start_y+ship_left_y,ship_start_x+point_x,ship_start_y+point_y,GREEN);
		graphics->drawLine(ship_start_x+ship_left_x,ship_start_y+ship_left_y,ship_start_x+ship_right_x,ship_start_y+ship_right_y,GREEN);
		graphics->drawLine(ship_start_x+ship_right_x,ship_start_y+ship_right_y,ship_start_x+point_x,ship_start_y+point_y,GREEN);
		graphics->fillRect(425, 5, 45, 15, GREEN);
	}
	/*2 hits remaining*/
	else if (shieldHealth == 2) {
		graphics->drawLine(ship_start_x+ship_left_x,ship_start_y+ship_left_y,ship_start_x+point_x,ship_start_y+point_y,YELLOW);
		graphics->drawLine(ship_start_x+ship_left_x,ship_start_y+ship_left_y,ship_start_x+ship_right_x,ship_start_y+ship_right_y,YELLOW);
		graphics->drawLine(ship_start_x+ship_right_x,ship_start_y+ship_right_y,ship_start_x+point_x,ship_start_y+point_y,YELLOW);
		graphics->fillRect(425, 5, 30, 15, YELLOW);
	}
	
	/*1 hit remaining*/
	else if (shieldHealth == 1) {
		graphics->drawLine(ship_start_x+ship_left_x,ship_start_y+ship_left_y,ship_start_x+point_x,ship_start_y+point_y,RED);
		graphics->drawLine(ship_start_x+ship_left_x,ship_start_y+ship_left_y,ship_start_x+ship_right_x,ship_start_y+ship_right_y,RED);
		graphics->drawLine(ship_start_x+ship_right_x,ship_start_y+ship_right_y,ship_start_x+point_x,ship_start_y+point_y,RED);
		graphics->fillRect(425, 5, 15, 15, RED);
		
	}
	/* no shield remaining*/
	else if (shieldHealth <= 0) {
		graphics->drawLine(ship_start_x+ship_left_x,ship_start_y+ship_left_y,ship_start_x+point_x,ship_start_y+point_y,WHITE);
		graphics->drawLine(ship_start_x+ship_left_x,ship_start_y+ship_left_y,ship_start_x+ship_right_x,ship_start_y+ship_right_y,WHITE);
		graphics->drawLine(ship_start_x+ship_right_x,ship_start_y+ship_right_y,ship_start_x+point_x,ship_start_y+point_y,WHITE);
		shieldActive = false;
	}
}

/* Draws the game, first clears the screen. */
void draw(void) {
	graphics->fillScreen(background);
	drawLives();
	drawMissiles(missile_active);
	drawAsteroids(asteroid_active);
	drawShip();
	 	
	graphics->setTextSize(2);
	graphics->setCursor(230,5);
	graphics->printf("Life:%1.0F", current_life_timeAlive);
	graphics->setCursor(340,5);
  graphics->printf("Score:%d", total_timeAlive);
	
	graphics->drawLine(0,23,500,23,WHITE);
	swap_DBuffer();
}

/* Call to function to flip the bits to display graphic correctly */
void flipBits(){
	int currBit;
	for (currBit = 0; currBit < 400; currBit++){
		flippedImage[currBit] = flipBits(asteroid_bits[currBit]);
	}
}

/* Asteroids draw as a circle - hopefully a bitmap*/
void drawAsteroids(struct asteroid *lst){
  flipBits();  
	while(lst) {	
		//graphics->fillCircle(lst->asteroid_x,lst->asteroid_y, 10, WHITE);
		graphics->drawBitmap(lst->asteroid_x-20,lst->asteroid_y-20,flippedImage, asteroid_height,asteroid_width, asteroid_colour);
			lst = lst->next;
    }
}
