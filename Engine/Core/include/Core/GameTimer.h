#pragma once

struct GameTimer {

	bool cap_framerate;
	int frame_count;
	int target_fps;
	int fps;

	// Target delta time used to cap frame rate
	float target_delta_time;
	// Time since last frame
	float delta_time;
	// Physics time step. This is fixed
	float time_step;
	// The current time since game start up
	float current_time;

	float max_delta;
	// Accumulator used for the fixed time step
	float accumulator;
	// The current time for physics calculations since game start up
	float physics_time;
};


inline float delta_time(GameTimer* gt) {
	return gt->delta_time;
}

inline float get_time(GameTimer* gt) {
	return gt->current_time;
}

inline int get_fps(GameTimer* gt) {
	return gt->fps;
}