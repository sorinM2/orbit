#pragma once 

namespace orbit::application 
{
	bool initialize();
	void restart_graphics();
	void update();
	void render();

	bool should_close();
	void shutdown();
}