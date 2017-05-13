/* Asteroids Model */
typedef struct missile {
    float missile_x, missile_y;
    float missile_vx, missile_vy;//never used so delete in fute
    float missile_ttl;
    struct missile *next;
} missile_t;


extern double   score;
extern int   lives;
extern float current_life_timeAlive;
extern int total_timeAlive;
extern int shieldHealth;
extern bool shieldActive;


extern float ship_start_x;
extern float ship_start_y;
extern int   ship_heading;
extern float ship_headingRadians;
extern float point_x;
extern float point_y;

extern float ship_left_x;
extern float ship_left_y;
extern float ship_right_x;
extern float ship_right_y;

extern double ship_speed;
extern double acceleration;

extern struct missile *missile_active;
extern struct asteroid *asteroid_active;

extern const int missile_heapsize;
extern const int asteroid_heapsize;

extern int total_timeAlive; //never used

void screenWrap(void);

struct point {
    float x,y;
};
typedef struct point coordinate_t;
typedef struct point vector_t;

typedef struct asteroid {
    float asteroid_x,asteroid_y;
    float asteroidV_x, asteroidV_y;
    float asteroid_ttl;
    struct asteroid *next;
} asteroid_t;


void physics(void);
void initialiseGame(void);
void fireMissle(void);
void checkShipHitAsteroid(struct asteroid *c);

void time(void);

void spawnObject(struct missile *r);
void spawnObject(struct asteroid *r);

void initialise_heap(void);
missile_t *allocnode(void);

void freenode(missile_t *n);
void fireMissle(void);
void updateMissile(struct missile *l);

void initialise_asteroid_heap(void);
asteroid_t *asteroidallocnode(void);

void asteroidfreenode(asteroid_t *n);
void spawnAsteroids(void);
void updateAsteroid(struct asteroid *l);

void checkShipHitAsteroid(struct asteroid *c);
void checkMissileHitAsteroid(struct missile *a, struct asteroid *b);

