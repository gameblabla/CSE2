#pragma once

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240

#define TILES_TO_PIXELS(x) ((x) * 0x10)
#define PIXELS_TO_TILES(x) ((x) / 0x10)
#define PIXELS_TO_UNITS(x) ((x) * 0x200)
#define UNITS_TO_PIXELS(x) ((x) / 0x200)
#define TILES_TO_UNITS(x) (PIXELS_TO_UNITS(TILES_TO_PIXELS(x)))	// * 0x2000
#define UNITS_TO_TILES(x) (PIXELS_TO_TILES(UNITS_TO_PIXELS(x)))	// / 0x2000

#define SECONDS_TO_FRAMES(x) ((x) * 50)
#define FRAMES_TO_SECONDS(x) ((x) / 50)

enum Collisions
{
	COLL_LEFT_WALL = 1,     // Touching a left wall
	COLL_CEILING = 2,       // Touching a ceiling
	COLL_RIGHT_WALL = 4,    // Touching a right wall
	COLL_GROUND = 8,        // Touching the ground
	// To be continued
};

enum Direction
{
	DIR_LEFT = 0,
	DIR_UP = 1,
	DIR_RIGHT = 2,
	DIR_DOWN = 3,
	DIR_AUTO = 4
};
