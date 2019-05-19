#pragma once

#include "Math/Vec.h"

#include <stdbool.h>

typedef union Mat4x4f {

	float mat1d[16];
	float mat2d[4][4];
		
	

	Mat4x4f() {
		// Create identity matrix
		for (int i = 0; i < 16; ++i) {
			mat1d[i] = 0;
		}
		mat2d[0][0] = 1.0f;
		mat2d[1][1] = 1.0f;
		mat2d[2][2] = 1.0f;
		mat2d[3][3] = 1.0f;
	};
	Mat4x4f(float fill_matrix_with_number) {
		// Create identity matrix
		for (int i = 0; i < 16; ++i) {
			mat1d[i] = fill_matrix_with_number;
		}
	};

	// Returns a pointer to the inner array
	// this is very unsafe, but it allows for better [][] indexing
	// it is unsafe cause it returns a pointer and the caller can try to index higher indices then the matrix
	// examle mat[2][5] is undefined
	// it is assumed that the caller will provide indices between [0][0] and [3][3]
	// while another solution would be to use a inner proxy class/struct. tbd
	float* Mat4x4f::operator[](const int col_index) {
		return this->mat2d[col_index];
	}
	

} Mat4x4f;


Mat4x4f transpose(const Mat4x4f& mat);
Mat4x4f mat4x4f_identity();// You can use the construtor, but this can be used for more clarity
Mat4x4f mat4x4f_zero();// You can use the construtor, but this can be used for more clarity
bool mat4x4f_invert(const Mat4x4f& m, Mat4x4f* invOut);
Mat4x4f operator*(const Mat4x4f& m1, const Mat4x4f& m2);

Vec4f operator*(const Mat4x4f& m, const Vec4f& v);
Mat4x4f perspective(float near_clip, float far_clip, float fov_deg, float aspect_ratio);
Mat4x4f ortho(float near_clip, float far_clip, float top, float bottom, float right, float left);
Mat4x4f look_at(const Vec3f& eye, const Vec3f& to, const Vec3f& up);
Mat4x4f viewport(int x, int y, int w, int h, float normalized_near, float normalized_far);







