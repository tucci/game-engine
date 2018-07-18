#pragma once

typedef union Vec2i {
	struct {
		int x;
		int y;
	};
	float data[2];

} Vec2i;

typedef union Vec2f {
	struct {
		
		float x;
		float y;
	};
	struct {
		float u;
		float v;
	};
	float data[2];
	
} Vec2f;

typedef union Vec3f {

	struct {

		float x;
		float y;
		float z;
	};

	struct {
		float r;
		float g;
		float b;
	};

	struct {
		float u;
		float v;
		float w;
	};

	float data[3];
	
} Vec3f;


typedef union Vec3i {

	struct {

		int x;
		int y;
		int z;
	};

	struct {
		int r;
		int g;
		int b;
	};

	struct {
		int u;
		int v;
		int w;
	};

	int data[3];

} Vec3i;

typedef union Vec4f {

	struct {

		float x;
		float y;
		float z;
		float w;
	};

	struct {
		float r;
		float g;
		float b;
		float a;
	};

	/*struct {
		float u;
		float v;
		float w;
	};*/

	float data[4];

} Vec4f;