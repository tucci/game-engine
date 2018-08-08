#pragma once

#include "SDLApp.h"





int main(int argc, char* argv[]) {



	App app;

	app.renderer.type = BackenedRenderer_OpenGL;
	//app.renderer.type = BackenedRenderer_Software;

	if (init_app(&app)) {
		game_loop(&app);
		destroy_app(&app);
	}

    return 0;
}


