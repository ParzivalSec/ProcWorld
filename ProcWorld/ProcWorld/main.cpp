#include <iostream>
#include "SDLWrapper.h"


int main(int argc, char** argv) {
	
	SDL_Init(SDL_INIT_VIDEO);

	sdl2::window_ptr_t window = sdl2::make_window("ProcWorld",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		1280, 720, SDL_WINDOW_OPENGL);

	SDL_Delay(3000);

	SDL_Quit();
	
	return 0;
}
