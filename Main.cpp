#include "GameEngine.h"

#include <sstream>


using namespace GE;

int main(int argc, char* argv[]) // for some reason needed or else compile fails??
{
	GameEngine ge;

	if (!ge.init())
	{
		ge.display_info_message("Failed to initialize the game engine!");
		return -1;
	}

	Uint32 last_time = SDL_GetTicks(), current_time = 0;

	int frame_count = 0;

	while (ge.keep_running())
	{
		ge.update();

		ge.processInput();

		ge.draw();

		frame_count++;

		current_time = SDL_GetTicks();

		if (current_time - last_time > 1000)
		{
			std::ostringstream msg;
			msg << "X-Treme Attempt: FPS: " << frame_count;

			std::ostringstream msg2;
			msg2 << "FPS: " << frame_count;

			ge.setwindowtitle(msg.str().c_str());
			ge.DebugFPS(msg2.str().c_str());
			frame_count = 0;
			last_time = current_time;
		}
	}

	ge.shutdown();

	return 0;

}