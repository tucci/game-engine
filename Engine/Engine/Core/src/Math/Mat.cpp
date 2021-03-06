#pragma once

#include "Math/Mat.h"
#include "Math/Math.h"




// TODO: speed up with simd
Mat4x4f transpose(const Mat4x4f& mat) {
	Mat4x4f result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.mat2d[j][i] = mat.mat2d[i][j];
		}
	}
	return result;
}

Mat4x4f mat4x4f_identity() {
	Mat4x4f mat;

	for (int i = 0; i < 16; i++) {
		mat.mat1d[i] = 0;
	}
	mat.mat2d[0][0] = 1.0f;
	mat.mat2d[1][1] = 1.0f;
	mat.mat2d[2][2] = 1.0f;
	mat.mat2d[3][3] = 1.0f;
	return mat;
}

Mat4x4f mat4x4f_zero() {
	Mat4x4f mat;

	for (int i = 0; i < 16; i++) {
		mat.mat1d[i] = 0;
	}
	return mat;

}


bool mat4x4f_invert(const Mat4x4f& m, Mat4x4f* invOut) {

	// Taken and modified from the Mesa Glu lib,
	double inv[16], det;
	int i;

	inv[0] = m.mat1d[5] * m.mat1d[10] * m.mat1d[15] -
		m.mat1d[5] * m.mat1d[11] * m.mat1d[14] -
		m.mat1d[9] * m.mat1d[6] * m.mat1d[15] +
		m.mat1d[9] * m.mat1d[7] * m.mat1d[14] +
		m.mat1d[13] * m.mat1d[6] * m.mat1d[11] -
		m.mat1d[13] * m.mat1d[7] * m.mat1d[10];

	inv[4] = -m.mat1d[4] * m.mat1d[10] * m.mat1d[15] +
		m.mat1d[4] * m.mat1d[11] * m.mat1d[14] +
		m.mat1d[8] * m.mat1d[6] * m.mat1d[15] -
		m.mat1d[8] * m.mat1d[7] * m.mat1d[14] -
		m.mat1d[12] * m.mat1d[6] * m.mat1d[11] +
		m.mat1d[12] * m.mat1d[7] * m.mat1d[10];

	inv[8] = m.mat1d[4] * m.mat1d[9] * m.mat1d[15] -
		m.mat1d[4] * m.mat1d[11] * m.mat1d[13] -
		m.mat1d[8] * m.mat1d[5] * m.mat1d[15] +
		m.mat1d[8] * m.mat1d[7] * m.mat1d[13] +
		m.mat1d[12] * m.mat1d[5] * m.mat1d[11] -
		m.mat1d[12] * m.mat1d[7] * m.mat1d[9];

	inv[12] = -m.mat1d[4] * m.mat1d[9] * m.mat1d[14] +
		m.mat1d[4] * m.mat1d[10] * m.mat1d[13] +
		m.mat1d[8] * m.mat1d[5] * m.mat1d[14] -
		m.mat1d[8] * m.mat1d[6] * m.mat1d[13] -
		m.mat1d[12] * m.mat1d[5] * m.mat1d[10] +
		m.mat1d[12] * m.mat1d[6] * m.mat1d[9];

	inv[1] = -m.mat1d[1] * m.mat1d[10] * m.mat1d[15] +
		m.mat1d[1] * m.mat1d[11] * m.mat1d[14] +
		m.mat1d[9] * m.mat1d[2] * m.mat1d[15] -
		m.mat1d[9] * m.mat1d[3] * m.mat1d[14] -
		m.mat1d[13] * m.mat1d[2] * m.mat1d[11] +
		m.mat1d[13] * m.mat1d[3] * m.mat1d[10];

	inv[5] = m.mat1d[0] * m.mat1d[10] * m.mat1d[15] -
		m.mat1d[0] * m.mat1d[11] * m.mat1d[14] -
		m.mat1d[8] * m.mat1d[2] * m.mat1d[15] +
		m.mat1d[8] * m.mat1d[3] * m.mat1d[14] +
		m.mat1d[12] * m.mat1d[2] * m.mat1d[11] -
		m.mat1d[12] * m.mat1d[3] * m.mat1d[10];

	inv[9] = -m.mat1d[0] * m.mat1d[9] * m.mat1d[15] +
		m.mat1d[0] * m.mat1d[11] * m.mat1d[13] +
		m.mat1d[8] * m.mat1d[1] * m.mat1d[15] -
		m.mat1d[8] * m.mat1d[3] * m.mat1d[13] -
		m.mat1d[12] * m.mat1d[1] * m.mat1d[11] +
		m.mat1d[12] * m.mat1d[3] * m.mat1d[9];

	inv[13] = m.mat1d[0] * m.mat1d[9] * m.mat1d[14] -
		m.mat1d[0] * m.mat1d[10] * m.mat1d[13] -
		m.mat1d[8] * m.mat1d[1] * m.mat1d[14] +
		m.mat1d[8] * m.mat1d[2] * m.mat1d[13] +
		m.mat1d[12] * m.mat1d[1] * m.mat1d[10] -
		m.mat1d[12] * m.mat1d[2] * m.mat1d[9];

	inv[2] = m.mat1d[1] * m.mat1d[6] * m.mat1d[15] -
		m.mat1d[1] * m.mat1d[7] * m.mat1d[14] -
		m.mat1d[5] * m.mat1d[2] * m.mat1d[15] +
		m.mat1d[5] * m.mat1d[3] * m.mat1d[14] +
		m.mat1d[13] * m.mat1d[2] * m.mat1d[7] -
		m.mat1d[13] * m.mat1d[3] * m.mat1d[6];

	inv[6] = -m.mat1d[0] * m.mat1d[6] * m.mat1d[15] +
		m.mat1d[0] * m.mat1d[7] * m.mat1d[14] +
		m.mat1d[4] * m.mat1d[2] * m.mat1d[15] -
		m.mat1d[4] * m.mat1d[3] * m.mat1d[14] -
		m.mat1d[12] * m.mat1d[2] * m.mat1d[7] +
		m.mat1d[12] * m.mat1d[3] * m.mat1d[6];

	inv[10] = m.mat1d[0] * m.mat1d[5] * m.mat1d[15] -
		m.mat1d[0] * m.mat1d[7] * m.mat1d[13] -
		m.mat1d[4] * m.mat1d[1] * m.mat1d[15] +
		m.mat1d[4] * m.mat1d[3] * m.mat1d[13] +
		m.mat1d[12] * m.mat1d[1] * m.mat1d[7] -
		m.mat1d[12] * m.mat1d[3] * m.mat1d[5];

	inv[14] = -m.mat1d[0] * m.mat1d[5] * m.mat1d[14] +
		m.mat1d[0] * m.mat1d[6] * m.mat1d[13] +
		m.mat1d[4] * m.mat1d[1] * m.mat1d[14] -
		m.mat1d[4] * m.mat1d[2] * m.mat1d[13] -
		m.mat1d[12] * m.mat1d[1] * m.mat1d[6] +
		m.mat1d[12] * m.mat1d[2] * m.mat1d[5];

	inv[3] = -m.mat1d[1] * m.mat1d[6] * m.mat1d[11] +
		m.mat1d[1] * m.mat1d[7] * m.mat1d[10] +
		m.mat1d[5] * m.mat1d[2] * m.mat1d[11] -
		m.mat1d[5] * m.mat1d[3] * m.mat1d[10] -
		m.mat1d[9] * m.mat1d[2] * m.mat1d[7] +
		m.mat1d[9] * m.mat1d[3] * m.mat1d[6];

	inv[7] = m.mat1d[0] * m.mat1d[6] * m.mat1d[11] -
		m.mat1d[0] * m.mat1d[7] * m.mat1d[10] -
		m.mat1d[4] * m.mat1d[2] * m.mat1d[11] +
		m.mat1d[4] * m.mat1d[3] * m.mat1d[10] +
		m.mat1d[8] * m.mat1d[2] * m.mat1d[7] -
		m.mat1d[8] * m.mat1d[3] * m.mat1d[6];

	inv[11] = -m.mat1d[0] * m.mat1d[5] * m.mat1d[11] +
		m.mat1d[0] * m.mat1d[7] * m.mat1d[9] +
		m.mat1d[4] * m.mat1d[1] * m.mat1d[11] -
		m.mat1d[4] * m.mat1d[3] * m.mat1d[9] -
		m.mat1d[8] * m.mat1d[1] * m.mat1d[7] +
		m.mat1d[8] * m.mat1d[3] * m.mat1d[5];

	inv[15] = m.mat1d[0] * m.mat1d[5] * m.mat1d[10] -
		m.mat1d[0] * m.mat1d[6] * m.mat1d[9] -
		m.mat1d[4] * m.mat1d[1] * m.mat1d[10] +
		m.mat1d[4] * m.mat1d[2] * m.mat1d[9] +
		m.mat1d[8] * m.mat1d[1] * m.mat1d[6] -
		m.mat1d[8] * m.mat1d[2] * m.mat1d[5];

	det = m.mat1d[0] * inv[0] + m.mat1d[1] * inv[4] + m.mat1d[2] * inv[8] + m.mat1d[3] * inv[12];

	if (det == 0) {
		return false;
	}


	det = 1.0 / det;

	for (i = 0; i < 16; i++) {
		invOut->mat1d[i] = (float)(inv[i] * det);
	}


	return true;
}




Mat4x4f operator*(const Mat4x4f& m1, const Mat4x4f& m2) {
	int i, j, k;
	Mat4x4f result;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			result.mat1d[j * 4 + i] = 0;
			for (k = 0; k < 4; k++) {
				result.mat1d[j * 4 + i] += m1.mat1d[k * 4 + i] * m2.mat1d[j * 4 + k];
			}
		}
	}

	//result = transpose(&result);
	return result;
}


Vec4f operator*(const Mat4x4f& m, const Vec4f& v) {
	
	
	Vec4f result;
	result.x = (m.mat2d[0][0] * v.x) + (m.mat2d[0][1] * v.y) + (m.mat2d[0][2] * v.z) + (m.mat2d[0][3] * v.w);
	result.y = (m.mat2d[1][0] * v.x) + (m.mat2d[1][1] * v.y) + (m.mat2d[1][2] * v.z) + (m.mat2d[1][3] * v.w);
	result.w = (m.mat2d[3][0] * v.x) + (m.mat2d[3][1] * v.y) + (m.mat2d[3][2] * v.z) + (m.mat2d[3][3] * v.w);
	result.z = (m.mat2d[2][0] * v.x) + (m.mat2d[2][1] * v.y) + (m.mat2d[2][2] * v.z) + (m.mat2d[2][3] * v.w);



	if (result.w != 1 && result.w != 0) {
		result.x /= result.w;
		result.y /= result.w;
		result.z /= result.w;
		result.w /= result.w;
	}


	
	return result;
}



Mat4x4f perspective(float near_clip, float far_clip, float fov_deg, float aspect_ratio) {

	Mat4x4f mat;


	float depth = near_clip - far_clip;
	float inverse_depth = 1 / depth;

	float f = 1 / tanf_(fov_deg * 0.5f * PI / 180.0f);

	mat.mat2d[0][0] = f / aspect_ratio;
	mat.mat2d[1][1] = f;
	mat.mat2d[2][2] = (far_clip + near_clip) * inverse_depth;
	mat.mat2d[3][2] = (2 * (far_clip * near_clip)) * inverse_depth;
	mat.mat2d[2][3] = -1;
	mat.mat2d[3][3] = 0;

	return mat;
}



Mat4x4f ortho(float near_clip, float far_clip, float top, float bottom, float right, float left) {

	Mat4x4f mat;

	float rl = right - left;
	float tb = top - bottom;
	float fn = far_clip - near_clip;

	float inverse_rl = 1 / rl;
	float inverse_tb = 1 / tb;
	float inverse_fn = 1 / fn;


	mat.mat2d[0][0] = 2 * inverse_rl;
	mat.mat2d[1][1] = 2 * inverse_tb;
	mat.mat2d[2][2] = -2 * inverse_fn;
	mat.mat2d[3][3] = 1;


	mat.mat2d[3][0] = -(right + left) * inverse_rl;
	mat.mat2d[3][1] = -(top + bottom) * inverse_tb;
	mat.mat2d[3][2] = -(far_clip + near_clip) * inverse_fn;





	return mat;
}


Mat4x4f look_at(const Vec3f& eye, const Vec3f& to, const Vec3f& up) {
	Vec3f forward = normalize(eye - to);
	Vec3f right = normalize(cross(up, forward));
	Vec3f up2 = normalize(cross(forward, right));

	Mat4x4f mat;
	
	for (int i = 0; i<3; i++) {
		mat.mat2d[i][0] = right.data[i];
		mat.mat2d[i][1] = up2.data[i];
		mat.mat2d[i][2] = forward.data[i];
	}

	mat.mat2d[3][0] = -dot(right, eye);
	mat.mat2d[3][1] = -dot(up2, eye);
	mat.mat2d[3][2] = -dot(forward, eye);

	return mat;
}

Mat4x4f viewport(int x, int y, int w, int h, float normalized_near, float normalized_far) {
	Mat4x4f m;
	m.mat2d[0][3] = x + (w / 2.f);
	m.mat2d[1][3] = y + (h / 2.f);
	m.mat2d[2][3] = normalized_near + (normalized_far / 2.f);

	m.mat2d[0][0] = w / 2.f;
	m.mat2d[1][1] = h / 2.f;
	m.mat2d[2][2] = (normalized_far - normalized_near) / 2.0f;
	m.mat2d[3][3] = 1;

	return m;
}
