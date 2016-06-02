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
#include "main.h"

int main(int argc, char *argv[])
{
	//Set up the render system and register input callbacks
	setupGraphics();
	
	//Initialize the Chip8 system and load the game into memory
	myChip8.initialize();
	myChip8.loadGame(std::string(argv[1]));

	//Emulation loop
	for (;;)
	{
		//emulate one cycle
		myChip8.executeCycle();

		//If the draw flag is set, update the screen
		//opcodes to clear screen:
		//0x00E0 - Clears the screen
		//0xDXYN - Draws a sprite on the screen
		drawGraphics();

		//Store key press  state (Press and Release)
		//If the function returns true, that means the user requested to close the application
		if (setEvents())
			break;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	system("pause");
	return 0;
}

void setupGraphics()
{
	// Initialize SDL Video rendering and Audio
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(
		"Chip8 Interpreter - Jonathan Del Corpo",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		DEFAULT_WINDOW_WIDTH,
		DEFAULT_WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE);

	getWindowSize();
	getScale();

	renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED |
		SDL_RENDERER_TARGETTEXTURE);
}

void getWindowSize()
{
	SDL_GetWindowSize(window, &windowSize.w, &windowSize.h);
}

void getScale()
{
	scaleX = ((float)windowSize.w / 64.0f);
	scaleY = ((float)windowSize.h / 32.0f);
}


//returns true when X butto is pressed
bool setEvents()
{
	//Handle events on queue
	while (SDL_PollEvent(&event) != 0)
	{
		//Event types
		switch (event.type)
		{
		case SDL_QUIT:
			return true;
			break;

		case SDL_WINDOWEVENT:
			windowEvent(event);
			break;

		case SDL_KEYDOWN:
			keyDown(event);
			break;

		case SDL_KEYUP:
			keyUp(event);
			break;
		}
	}
	return false;
}

void windowEvent(SDL_Event& e)
{
	switch (e.window.event)
	{
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		getWindowSize();
		getScale();
		break;
	}
}

void keyDown(SDL_Event& e)
{
	handleKeys(e, 1);
}

void keyUp(SDL_Event& e)
{
	handleKeys(e, 0);
}

void handleKeys(SDL_Event& e, char value)
{
	switch (e.key.keysym.sym)
	{

		//1
	case SDLK_1:
		myChip8.key[1] = value;
		break;
	
		//2
	case SDLK_2:
		myChip8.key[2] = value;
		break;

		//3
	case SDLK_3:
		myChip8.key[3] = value;
		break;

		//C
	case SDLK_4:
		myChip8.key[0xC] = value;
		break;

		//4
	case SDLK_q:
		myChip8.key[4] = value;
		break;

		//5
	case SDLK_w:
		myChip8.key[5] = value;
		break;

		//6
	case SDLK_e:
		myChip8.key[6] = value;
		break;

		//D
	case SDLK_r:
		myChip8.key[0xD] = value;
		break;

		//7
	case SDLK_a:
		myChip8.key[7] = value;
		break;

		//8
	case SDLK_s:
		myChip8.key[8] = value;
		break;

		//9
	case SDLK_d:
		myChip8.key[9] = value;
		break;

		//E
	case SDLK_f:
		myChip8.key[0xE] = value;
		break;

		//A
	case SDLK_z:
		myChip8.key[0xA] = value;
		break;

		//0
	case SDLK_x:
		myChip8.key[0] = value;
		break;

		//B
	case SDLK_c:
		myChip8.key[0xB] = value;
		break;

		//F
	case SDLK_v:
		myChip8.key[0xF] = value;
		break;
	}
}



void drawGraphics()
{
	
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			if(myChip8.gfx[i][j] != 0)
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			else
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

			SDL_Rect rect;
			rect.x = (float)j * scaleX;
			rect.y = (float)i * scaleY;
			rect.w = scaleX;
			rect.h = scaleY;
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	SDL_RenderPresent(renderer);
}
