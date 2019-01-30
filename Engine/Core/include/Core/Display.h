#pragma once

#include "Math/Vec.h"

struct Display {
	float dpi;
	int refresh_rate;
	int w;
	int h;
};


inline float get_display_dpi(Display* display) {
	return display->dpi;
}

inline int get_display_refresh_rate(Display* display) {
	return display->refresh_rate;
}

inline Vec2i get_display_size(Display* display) {
	Vec2i ds = { display->w, display->h };
	return ds;
}



