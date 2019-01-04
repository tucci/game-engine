#pragma once


#include "engine_platform.h"

#define ENGINE_MODE_EDITOR 1

typedef struct EditorData {
	HWND editor_hwnd;

	bool was_last_frame_using_right_click;
} EditorData;