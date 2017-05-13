/* Asteroids model */
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

/* Hardware Platform Library */
#include "model.h"
#include "utils.h"
#include "asteroids.h"
#include "view.h"

//linked list for missiles
static const int missile_heapsize = 5;
static missile_t heap[missile_heapsize];
static missile_t *freenodes;

//linked list for asteroids
static const int asteroid_heapsize = 9;
static asteroid_t asteroid_heap[asteroid_heapsize];
static asteroid_t *asteroidfreenodes;

float current_life_timeAlive;
int total_timeAlive;
int lives;
bool shieldActive;
int shieldHealth = 3;

//ship data
float ship_start_x;
float ship_start_y;
int ship_heading;
float ship_headingRadians;
float point_x;
float point_y;
float ship_left_x;
float ship_left_y;
float ship_right_x;
float ship_right_y;

double ship_speed;
double acceleration;

static const float Dt = 0.01;
struct missile *missile_active = NULL;
struct asteroid *asteroid_active = NULL;

/*main physics for game. How ship moves, asteroids and bullets are added to their lists */
void physics(void) {
	checkShipHitAsteroid(asteroid_active);
	checkMissileHitAsteroid(missile_active,asteroid_active);
	
	updateMissile(missile_active);
	updateAsteroid(asteroid_active);
	
	screenWrap();
	spawnAsteroids();
	
	ship_headingRadians=radians(ship_heading); // Calculates radians of angle which will be used to calculate the coordinates of each point of the ship
	point_x=18*(cos(ship_headingRadians)); //Uses the length of the triangle's sides (18 pixels) and the angle radians to calculate the x coordinate of the top of the triangle
	point_y=18*(sin(ship_headingRadians)); //Uses the length of the triangle's sides (18 pixels) and the angle radians to calculate the y coordinate of the top of the triangle
	
	ship_left_x = (-6*(cos(ship_headingRadians)))-(6*(sin(ship_headingRadians))); //Uses the angle radians to calculate the left point of the triangle's coordinates
	ship_left_y = (-6*(sin(ship_headingRadians)))+(6*(cos(ship_headingRadians)));	//Uses the angle radians to calculate the left point of the triangle's coordinates
	
	ship_right_x = (-6*(cos(ship_headingRadians)))-(-6*(sin(ship_headingRadians)));	//Uses the angle radians to calculate the right point of the triangle's coordinates
	ship_right_y = (-6*(sin(ship_headingRadians)))+(-6*(cos(ship_headingRadians)));	//Uses the angle radians to calculate the right point of the triangle's coordinates
	
	ship_start_x = ship_start_x+ship_speed*(cos(ship_headingRadians)); //uses the heading radians and ship speed to calculate the ships new x coordinate
	ship_start_y = ship_start_y+ship_speed*(sin(ship_headingRadians)); //uses the heading radians and ship speed to calculate the ships new y coordinate
	
	//increase time alive by 1 second
	current_life_timeAlive = current_life_timeAlive + Dt; //changed to use Dt as well (kept freezing.....)

  //increase the ship speed - check for the max speed (4) otherwise increase via the acceleration
	if(ship_speed < 3){
		ship_speed = ship_speed+acceleration;
	}
	if(acceleration > 0){
		acceleration = acceleration-0.1;
	}
	if(ship_speed > 0){
		ship_speed = ship_speed-0.02;
	}
}

/* Initialises values before game starts
  - Where the ship spawns
  - Initial values for game physics
  - Shield status*/
void initialiseGame(void) {
	ship_start_x = 240; //centre ship hor
	ship_start_y = 250; //towards the bottom of screen
	ship_heading = 270; //pointing upwards
	
	current_life_timeAlive = 0;
	total_timeAlive = 0;
	shieldHealth = 3;
	acceleration = 0.0;
	shieldActive = true;
}

/* Wraps ship around the screen if it goes out the boundaries (Top, bottom, left and right) */
void screenWrap(void){
  if(ship_start_x > 480){
		ship_start_x = 5;		
	} 
	else if(ship_start_x < 0){
		ship_start_x = 475;
		
	} else if(ship_start_y > 280){
		ship_start_y = 35;
	} 
	else if(ship_start_y < 34){
		ship_start_y = 260;
	}
}
/*Constucts a new missile*/
void spawnObject(struct missile *m){
    m->missile_x = ship_start_x;
    m->missile_y = ship_start_y;
    m->missile_vx = 200*(cos(ship_headingRadians));
    m->missile_vy = 200*(sin(ship_headingRadians));
    m->missile_ttl = 200;
}

/*Constructs a new asteroid*/
void spawnObject(struct asteroid *a){
    a->asteroid_x = randrange(10,470);
    a->asteroid_y = randrange(10,250);
    a->asteroidV_x = randrange(-40,40);
    a->asteroidV_y = randrange(-40,40);
    a->asteroid_ttl = randrange(900,1100);
}

void fireMissle(void){
	struct missile *spark = allocnode();
	if(spark) {
			spark->next = missile_active;
			missile_active = spark;
			spawnObject(spark);
	}
}

/*moves the missile once fired using Dt value*/
 void updateMissile(struct missile *m){
    for( ; m ; m = m->next ) {
        m->missile_x += m->missile_vx * Dt;
        m->missile_y += m->missile_vy * Dt;
			
				//check to see if missle goes past screen boundaries
        if( m->missile_x<32 || 480<m->missile_x ) {
					m->missile_ttl=0;
				}
        if( m->missile_y<32 || 260<m->missile_y ) {
					m->missile_ttl=0;
				}
        m->missile_ttl -= Dt;
        if( m->next->missile_ttl<=0 ) {
            struct missile *expired = m->next;
            m->next = m->next->next;
            freenode(expired);
        }
    }
}
 
/*moves the asteroid depending on asteroidV_x and y values*/
void updateAsteroid(struct asteroid *a){
    for( ; a ; a = a->next ) {
			a->asteroid_x += a->asteroidV_x * Dt;
			a->asteroid_y += a->asteroidV_y * Dt;
			
			//check asteroid is on screen
			if( a->asteroid_x<-30|| 485<a->asteroid_x ) {
				a->asteroid_ttl=0;
			}
			if( a->asteroid_y<40 || 280<a->asteroid_y ) {
				a->asteroid_ttl=0;
				a->asteroid_ttl -= Dt;
			}
			if( a->next->asteroid_ttl<=0 ) {
					struct asteroid *expired = a->next;
					a->next = a->next->next;
					asteroidfreenode(expired);
			}
    }
}
 
void spawnAsteroids(void){
        struct asteroid *spawnAsteroid = asteroidallocnode();
        if(spawnAsteroid) {
            spawnAsteroid->next = asteroid_active;
            asteroid_active = spawnAsteroid;
            spawnObject(spawnAsteroid);
        }
}
 
/*Check to see if asteroid collides with the ship. Lowever lives by 1 if lives available*/
void checkShipHitAsteroid(struct asteroid *c){
    for( ; c ; c = c->next ) {
        if(c->asteroid_x<(ship_start_x+16) && c->asteroid_x>(ship_start_x-16) &&
					(c->asteroid_y<(ship_start_y+16) && c->asteroid_y>(ship_start_y-16)) ){
						if (shieldHealth >= 0) {
							shieldHealth--;
						}

						if(lives > -1 && shieldHealth < 0){
							lives--;
							if (lives <= -1){
								drawGameOverScreen();
							}	
							total_timeAlive=total_timeAlive+current_life_timeAlive;
							current_life_timeAlive = 0;
						}

						//destroy asteroid that was hit (make it move outside of boundary so it disapears)
						//c->asteroid_ttl = 0; (kept making the game freeze - look over)
						c->asteroid_x = 500;
						c->asteroid_y = 500;
        }
    }//end for
}

void checkMissileHitAsteroid(struct missile *m, struct asteroid *a) {
  for(; m ; m = m->next){
    for(; a ; a = a->next){
      if(m->missile_x > (a->asteroid_x-12) && m->missile_x < (a->asteroid_x+12) && 
				(m->missile_y > (a->asteroid_y-12) && m->missile_y < (a->asteroid_y+12)) ){
          
					m->missile_x = 500;//outside of play (destroys)
					m->missile_y = 500;
          a->asteroid_ttl=0;
        }
		}//end for2
	}//end for1
 }

 /*heap for missiles*/
void initialise_heap(void){
    int n;
    for( n=0 ; n<(missile_heapsize-1) ; n++) {
        heap[n].next = &heap[n+1];
    }
    heap[n].next = NULL;
    freenodes = &heap[0];
}

missile_t *allocnode(void){
    missile_t *node = NULL;
    if( freenodes ) {
        node = freenodes;
        freenodes = freenodes->next;
    }
    return node;
}

void freenode(missile_t *n){
    n->next = freenodes;
    freenodes = n;
}

/*heap for asteroids*/
void initialise_asteroid_heap(void){
    int a;
    for( a=0 ; a<(asteroid_heapsize-1) ; a++) {
        asteroid_heap[a].next = &asteroid_heap[a+1];
    }
    asteroid_heap[a].next = NULL;
    asteroidfreenodes = &asteroid_heap[0];
}

asteroid_t *asteroidallocnode(void){
    asteroid_t *node = NULL;
    if( asteroidfreenodes ) {
        node = asteroidfreenodes;
        asteroidfreenodes = asteroidfreenodes->next;
    }
    return node;
}

void asteroidfreenode(asteroid_t *a){
    a->next = asteroidfreenodes;
    asteroidfreenodes = a;
}
