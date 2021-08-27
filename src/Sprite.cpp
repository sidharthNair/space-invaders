#include "Sprite.h"
#include "random.h"
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "SlidePot.h"



Sprite::Sprite() {
	life = dead;
}
void Sprite::init(int32_t ix, int32_t iy, const uint16_t *p, const uint16_t *bp, 
	uint32_t wi, uint32_t he, uint8_t e, uint8_t m) {
	life = alive;
	count = 0;
	x = ix; y = iy; image = p; black = bp; w = wi; h = he;
	enemy = e;
  missile = m;
	needDraw = 1;
	lives = 3;
	vx = 0;
	vy = 0;
	if (missile && !enemy) { // if Sprite is a player's missile
		vy = -2;
	}
	else if (missile) { // if Sprite is an enemy's missile
		vy = 2;
	}
	if (enemy) {
		lives = 1;
	}
}
void Sprite::setFireRate(uint8_t rate){
	fireRate = rate;
}
int8_t Sprite::Move(void) {
	needDraw = 0;
	if (enemy && !missile) { // Case 1: Enemy Movement
		if (life == alive) {
			needDraw = 1;
			if (y > 160) {
				life = dead;
				return -1; 
				// Return Value Case 1:
				// A negative value is used in the main.cpp file to indicate that the game is over
				// because the enemy sprite has reached the bottom of the screen
			}
			else {
				if (count == 15) { // only updates the direction of the alien every 15 game ticks
					vx = Random()%2 - 1; // Calculating random x velocity -- Range: [-1, 1]
					vy = Random()%100 > 65; // Calculating random y velocity -- Range: [0, 1]
																	// 35% chance of going down, 65% chance of staying still with respect to y
					if ((vx > 0 && (x > (128 - 2*w))) || (vx < 0 && (x < 2*w))) {
						vx *= -1; // Used to make sure that the aliens don't approach the left and right walls of the screen
					}
					count = 0;
				}
				x += vx;;
				y += vy;
				count++;
			}
			return ((Random()%150) > (148 - fireRate/3)); 
			// Return Value Case 2:
			// This random value is used by the game engine to determine whether 
			// the alien is going to shoot this game tick. There is a 1/150 chance
			// that each alien will shoot during each game tick.
			// aliens shoot faster every third wave
		}
	}
	else if (missile) { // Case 2: Missile Movement
		needDraw = 0;
		if (life == alive) {
			needDraw = 1;
			if (y > 160 || y < 20 || x > 128 || x < 0) {
					life = dead;
			}
			else {
				y += vy;
			}
		}
	}
	else { // Case 3: Player Movement
		needDraw = 0;
		vx = getMovementDirection();
		if ((vx > 0 && x < 128-w) || (vx < 0 && x > 0)) {
			needDraw = 1;
			x += vx;
		}
	}
	return 0; 
	// Return Value Case 3:
	// A return value of 0 tells the game engine that no special 
	// case needs to be accounted for
}
void Sprite::Draw(void) {
	if (needDraw) {
		if (life == alive) {
			ST7735_DrawBitmap(x, y, image, w, h);
		}
		else {
			ST7735_DrawBitmap(x, y, black, w, h);
		}
		needDraw = 0;
	}
}
void Sprite::Redraw(void) {
	ST7735_DrawBitmap(x, y, image, w, h);
}
int32_t Sprite::getX() {
	return x;
}
int32_t Sprite::getY() {
	return y;
}
int32_t Sprite::getW() {
	return w;
}
int32_t Sprite::getH() {
	return h;
}
uint8_t Sprite::getLives() {
	return lives;
}
uint8_t Sprite::isAlive() {
	return life;
}
void Sprite::hit() {
	lives--;
	if (lives == 0) {
		needDraw = 1;
		life = dead;
	}
}
void Sprite::setImage(const uint16_t *p) {
	needDraw = 1;
	image = p;
}
uint8_t Sprite::collisionCheck(Sprite& other) {
	if (((x+w/2) > other.getX() && (x+w/2) < other.getX()+other.getW()) && ((y+h/2) > other.getY() && (y+h/2) < other.getY()+other.getH()) && life && other.life) {
		other.hit();
		needDraw = 1;
		life = dead;
		return 1;
	}
	return 0;
}
