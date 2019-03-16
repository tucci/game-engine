#pragma once



struct GameTimer {

	bool cap_framerate;

	int frame_count;
	int target_fps;
	int fps;

	// Target delta time used to cap frame rate
	float target_delta_time;
	// Time since last frame
	//float delta_time;
	// Physics time step. This is fixed
	float time_step;
	// The current time since game start up
	//float current_time;

	float max_delta;
	// Accumulator used for the fixed time step
	float accumulator;
	// The current time for physics calculations since game start up
	float physics_time;


	u64 ticks;
	u64 delta_ticks;

	double seconds;
	float delta_seconds;

	u64 milliseconds;
	int delta_milliseconds;
	float delta_time; // delta_milliseconds / 1000.0f

	u64 ticks_per_sec;
	u64 sdl_start_ticks;
};


inline float delta_time(GameTimer* gt) {
	//return gt->delta_time;
	return gt->delta_time;
}

inline float get_time(GameTimer* gt) {
	//return gt->current_time;
	return gt->milliseconds;
}

inline int get_fps(GameTimer* gt) {
	return gt->fps;
}