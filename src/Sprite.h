
#include <stdint.h>

typedef enum {dead, alive} status_t;

class Sprite {
private:
	int32_t x, y; // position coordinates
	int32_t vx, vy; // velocity in x and y direction (pixels / frame)
	const unsigned short *image; // image pointer
	const unsigned short *black; // black image of same height and width
	uint32_t w, h; // width and height of images
  status_t life; // life status (alive or dead)
	uint8_t needDraw; // indicates whether Sprite needs to be redrawn
	uint8_t count; // for alien randomized movement
								 // once this reaches a certain value their direction will change and count will reset
	uint8_t enemy; // indicates whether this is an enemy (missile) or a player (missile)
	uint8_t missile; // indicates whether this is a missile or not
	uint8_t lives; // # of lives the Sprite has remaining
	uint8_t fireRate;
public:
	Sprite();
	void init(int32_t ix, int32_t iy, const uint16_t *p, const uint16_t *bp, 
		uint32_t wi, uint32_t he, uint8_t e, uint8_t m); // Initialization of Sprite
	int8_t Move(void); // Calculates the velocities and moves the Sprite using discrete integration
										 // Return value is used by the game engine (see cpp file)
	void Draw(void); // Draws if the needDraw flag is set
	void Redraw(void); // Draws regardless of the needDraw flag
	// Accessor Methods
	int32_t getX();
	int32_t getY();
	int32_t getW();
	int32_t getH();
	uint8_t getLives();
	uint8_t isAlive();
	void setImage(const uint16_t *p); // Allows you the sprite to change images (granted the image is of same width and height)
	void hit(); // Indicates that the sprite has been hit and lives and life status are updated accordingly
	void setFireRate(uint8_t rate); //how often does the enemy shoot?
	uint8_t collisionCheck(Sprite &other); // Checks if this sprite intersects with Sprite other
};
