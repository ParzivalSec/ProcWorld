#include <iostream>
#include "Application.h"

int main(int argc, char** argv) {

	Application proceduralWorlds("ProcWorld by Lukas Vogl", 1280, 720);

	if (!proceduralWorlds.HasErrors()) {
		proceduralWorlds.Run();
	} else {
		std::cout << "ERROR : Application could not be intiialized!" << std::endl;
	}
	
	return 0;
}
