#pragma once

#include "SDLApp.h"


int main(int argc, char* argv[]) {

	App app;

	if (init_app(&app)) {
		game_loop(&app);
		destroy_app(&app);
	}

    return 0;
}


