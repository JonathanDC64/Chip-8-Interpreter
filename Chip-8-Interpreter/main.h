///////////////////////////////////////////////////////////////////////////////
// Name: Chip 8 Interpreter
//
// Author: Jonathan Del Corpo
// Contact: jonathan_delcorpo@hotmail.com
//
// License: GNU General Public License (GPL) v2 
// ( http://www.gnu.org/licenses/old-licenses/gpl-2.0.html )
//
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <math.h>
#include <SDL.h>
#include "Chip8.h"

Chip8 myChip8;
SDL_Rect windowSize;
SDL_Window * window = NULL;
SDL_Renderer * renderer = NULL;
SDL_Event event;

const int DEFAULT_WINDOW_WIDTH = 512;
const int DEFAULT_WINDOW_HEIGHT = 256;

const float FRAME_RATE = 1000.0f / 60.0f;

float scaleX;
float scaleY;

void drawGraphics();
void setupGraphics();
void getWindowSize();
void getScale();
bool setEvents();
void windowEvent(SDL_Event& e);
void keyDown(SDL_Event& e);
void keyUp(SDL_Event& e);
void handleKeys(SDL_Event& e, char value);

