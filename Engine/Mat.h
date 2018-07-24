#pragma once

#include "Vec.h"
#include "Math.h"

// TODO: speed up with simd
// TODO: look up strick aliasing rules
typedef union Mat4x4f {
	float mat1d[16];

	struct {
		float mat2d[4][4];
	};

	struct {
		float m00, m01, m02, m03;
		float m10, m11, m12, m13;
		float m20, m21, m22, m23;
		float m30, m31, m32, m33;
	};


} Mat4x4f;

Mat4x4f inline transpose(Mat4x4f* mat) {
	Mat4x4f result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.mat2d[j][i] = mat->mat2d[i][j];
		}
	}
	return result;
}


Mat4x4f inline mat4x4f_identity() {
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

void inline mat4x4f_identity2(Mat4x4f* mat) {
	

	for (int i = 0; i < 16; i++) {
		mat->mat1d[i] = 0;
	}
	mat->mat2d[0][0] = 1.0f;
	mat->mat2d[1][1] = 1.0f;
	mat->mat2d[2][2] = 1.0f;
	mat->mat2d[3][3] = 1.0f;
	
}

Mat4x4f inline mat4x4f_zero() {
	Mat4x4f mat;

	for (int i = 0; i < 16; i++) {
		mat.mat1d[i] = 0;
	}
	return mat;

}

// Taken and modified from the Mesa Glu lib,
bool inline mat4x4f_invert(Mat4x4f* m, Mat4x4f* invOut) {
	
	
	double inv[16], det;
	int i;

	inv[0] = m->mat1d[5] * m->mat1d[10] * m->mat1d[15] -
		m->mat1d[5] * m->mat1d[11] * m->mat1d[14] -
		m->mat1d[9] * m->mat1d[6] * m->mat1d[15] +
		m->mat1d[9] * m->mat1d[7] * m->mat1d[14] +
		m->mat1d[13] * m->mat1d[6] * m->mat1d[11] -
		m->mat1d[13] * m->mat1d[7] * m->mat1d[10];

	inv[4] = -m->mat1d[4] * m->mat1d[10] * m->mat1d[15] +
		m->mat1d[4] * m->mat1d[11] * m->mat1d[14] +
		m->mat1d[8] * m->mat1d[6] * m->mat1d[15] -
		m->mat1d[8] * m->mat1d[7] * m->mat1d[14] -
		m->mat1d[12] * m->mat1d[6] * m->mat1d[11] +
		m->mat1d[12] * m->mat1d[7] * m->mat1d[10];

	inv[8] = m->mat1d[4] * m->mat1d[9] * m->mat1d[15] -
		m->mat1d[4] * m->mat1d[11] * m->mat1d[13] -
		m->mat1d[8] * m->mat1d[5] * m->mat1d[15] +
		m->mat1d[8] * m->mat1d[7] * m->mat1d[13] +
		m->mat1d[12] * m->mat1d[5] * m->mat1d[11] -
		m->mat1d[12] * m->mat1d[7] * m->mat1d[9];

	inv[12] = -m->mat1d[4] * m->mat1d[9] * m->mat1d[14] +
		m->mat1d[4] * m->mat1d[10] * m->mat1d[13] +
		m->mat1d[8] * m->mat1d[5] * m->mat1d[14] -
		m->mat1d[8] * m->mat1d[6] * m->mat1d[13] -
		m->mat1d[12] * m->mat1d[5] * m->mat1d[10] +
		m->mat1d[12] * m->mat1d[6] * m->mat1d[9];

	inv[1] = -m->mat1d[1] * m->mat1d[10] * m->mat1d[15] +
		m->mat1d[1] * m->mat1d[11] * m->mat1d[14] +
		m->mat1d[9] * m->mat1d[2] * m->mat1d[15] -
		m->mat1d[9] * m->mat1d[3] * m->mat1d[14] -
		m->mat1d[13] * m->mat1d[2] * m->mat1d[11] +
		m->mat1d[13] * m->mat1d[3] * m->mat1d[10];

	inv[5] = m->mat1d[0] * m->mat1d[10] * m->mat1d[15] -
		m->mat1d[0] * m->mat1d[11] * m->mat1d[14] -
		m->mat1d[8] * m->mat1d[2] * m->mat1d[15] +
		m->mat1d[8] * m->mat1d[3] * m->mat1d[14] +
		m->mat1d[12] * m->mat1d[2] * m->mat1d[11] -
		m->mat1d[12] * m->mat1d[3] * m->mat1d[10];

	inv[9] = -m->mat1d[0] * m->mat1d[9] * m->mat1d[15] +
		m->mat1d[0] * m->mat1d[11] * m->mat1d[13] +
		m->mat1d[8] * m->mat1d[1] * m->mat1d[15] -
		m->mat1d[8] * m->mat1d[3] * m->mat1d[13] -
		m->mat1d[12] * m->mat1d[1] * m->mat1d[11] +
		m->mat1d[12] * m->mat1d[3] * m->mat1d[9];

	inv[13] = m->mat1d[0] * m->mat1d[9] * m->mat1d[14] -
		m->mat1d[0] * m->mat1d[10] * m->mat1d[13] -
		m->mat1d[8] * m->mat1d[1] * m->mat1d[14] +
		m->mat1d[8] * m->mat1d[2] * m->mat1d[13] +
		m->mat1d[12] * m->mat1d[1] * m->mat1d[10] -
		m->mat1d[12] * m->mat1d[2] * m->mat1d[9];

	inv[2] = m->mat1d[1] * m->mat1d[6] * m->mat1d[15] -
		m->mat1d[1] * m->mat1d[7] * m->mat1d[14] -
		m->mat1d[5] * m->mat1d[2] * m->mat1d[15] +
		m->mat1d[5] * m->mat1d[3] * m->mat1d[14] +
		m->mat1d[13] * m->mat1d[2] * m->mat1d[7] -
		m->mat1d[13] * m->mat1d[3] * m->mat1d[6];

	inv[6] = -m->mat1d[0] * m->mat1d[6] * m->mat1d[15] +
		m->mat1d[0] * m->mat1d[7] * m->mat1d[14] +
		m->mat1d[4] * m->mat1d[2] * m->mat1d[15] -
		m->mat1d[4] * m->mat1d[3] * m->mat1d[14] -
		m->mat1d[12] * m->mat1d[2] * m->mat1d[7] +
		m->mat1d[12] * m->mat1d[3] * m->mat1d[6];

	inv[10] = m->mat1d[0] * m->mat1d[5] * m->mat1d[15] -
		m->mat1d[0] * m->mat1d[7] * m->mat1d[13] -
		m->mat1d[4] * m->mat1d[1] * m->mat1d[15] +
		m->mat1d[4] * m->mat1d[3] * m->mat1d[13] +
		m->mat1d[12] * m->mat1d[1] * m->mat1d[7] -
		m->mat1d[12] * m->mat1d[3] * m->mat1d[5];

	inv[14] = -m->mat1d[0] * m->mat1d[5] * m->mat1d[14] +
		m->mat1d[0] * m->mat1d[6] * m->mat1d[13] +
		m->mat1d[4] * m->mat1d[1] * m->mat1d[14] -
		m->mat1d[4] * m->mat1d[2] * m->mat1d[13] -
		m->mat1d[12] * m->mat1d[1] * m->mat1d[6] +
		m->mat1d[12] * m->mat1d[2] * m->mat1d[5];

	inv[3] = -m->mat1d[1] * m->mat1d[6] * m->mat1d[11] +
		m->mat1d[1] * m->mat1d[7] * m->mat1d[10] +
		m->mat1d[5] * m->mat1d[2] * m->mat1d[11] -
		m->mat1d[5] * m->mat1d[3] * m->mat1d[10] -
		m->mat1d[9] * m->mat1d[2] * m->mat1d[7] +
		m->mat1d[9] * m->mat1d[3] * m->mat1d[6];

	inv[7] = m->mat1d[0] * m->mat1d[6] * m->mat1d[11] -
		m->mat1d[0] * m->mat1d[7] * m->mat1d[10] -
		m->mat1d[4] * m->mat1d[2] * m->mat1d[11] +
		m->mat1d[4] * m->mat1d[3] * m->mat1d[10] +
		m->mat1d[8] * m->mat1d[2] * m->mat1d[7] -
		m->mat1d[8] * m->mat1d[3] * m->mat1d[6];

	inv[11] = -m->mat1d[0] * m->mat1d[5] * m->mat1d[11] +
		m->mat1d[0] * m->mat1d[7] * m->mat1d[9] +
		m->mat1d[4] * m->mat1d[1] * m->mat1d[11] -
		m->mat1d[4] * m->mat1d[3] * m->mat1d[9] -
		m->mat1d[8] * m->mat1d[1] * m->mat1d[7] +
		m->mat1d[8] * m->mat1d[3] * m->mat1d[5];

	inv[15] = m->mat1d[0] * m->mat1d[5] * m->mat1d[10] -
		m->mat1d[0] * m->mat1d[6] * m->mat1d[9] -
		m->mat1d[4] * m->mat1d[1] * m->mat1d[10] +
		m->mat1d[4] * m->mat1d[2] * m->mat1d[9] +
		m->mat1d[8] * m->mat1d[1] * m->mat1d[6] -
		m->mat1d[8] * m->mat1d[2] * m->mat1d[5];

	det = m->mat1d[0] * inv[0] + m->mat1d[1] * inv[4] + m->mat1d[2] * inv[8] + m->mat1d[3] * inv[12];

	if (det == 0) {
		return false;
	}
		

	det = 1.0 / det;

	for (i = 0; i < 16; i++) {
		invOut->mat1d[i] = inv[i] * det;
	}
		

	return true;
}


Mat4x4f inline perspective(float near, float far, float fov, float aspect_ratio) {

	Mat4x4f mat = mat4x4f_identity();

	//float depth = near - far;
	//float inverse_depth = 1 / depth;


	float depth = near - far;
	float inverse_depth = 1 / depth;

	float f = 1 / tanf(fov * 0.5f * PI / 180.0f);

	mat.mat2d[0][0] = f / aspect_ratio;
	mat.mat2d[1][1] = f;
	mat.mat2d[2][2] = -(far + near) * inverse_depth;
	mat.mat2d[3][2] = 2 * (far * near) * inverse_depth;
	mat.mat2d[2][3] = -1;
	mat.mat2d[3][3] = 0;

	//mat = transpose(&mat);

	return mat;
}


Mat4x4f inline mat4x4_mul(Mat4x4f* m1, Mat4x4f* m2) {
	int i, j, k;
	Mat4x4f result;
	/*for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			result.mat2d[i][j] = 0;
			for (k = 0; k < 4; k++)
				result.mat2d[i][j] += m1->mat2d[i][k] * m2->mat2d[k][j];
		}
	}*/

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			result.mat1d[j * 4 + i] = 0;

			for (k = 0; k < 4; k++) {
				result.mat1d[j * 4 + i] += m1->mat1d[k * 4 + i] * m2->mat1d[j * 4 + k];
			}
		}
	}

	//result = transpose(&result);
	return result;
}
Vec4f inline mat4x4_vec_mul(Mat4x4f* m, Vec4f v) {
	// NOTE: this is column major multiplication
	Vec4f result;
	result.x = (m->m00 * v.x) + (m->m01 * v.y) + (m->m02 * v.z) + (m->m03 * v.w);
	result.y = (m->m10 * v.x) + (m->m11 * v.y) + (m->m12 * v.z) + (m->m13 * v.w);
	result.z = (m->m20 * v.x) + (m->m21 * v.y) + (m->m22 * v.z) + (m->m23 * v.w);
	result.w = (m->m30 * v.x) + (m->m31 * v.y) + (m->m32 * v.z) + (m->m33 * v.w);
	float w = result.w;
	if (w != 1) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}
	result.w = 1;


	return result;
}


Mat4x4f inline look_at(Vec3f eye, Vec3f to, Vec3f up) {
	Vec3f z = vec_negate(vec_normalize(vec_sub(eye, to)));
	Vec3f x = vec_normalize(vec_cross(z, up));
	Vec3f y = vec_normalize(vec_cross(z, x));
	Mat4x4f mat = mat4x4f_identity();
	for (int i = 0; i<3; i++) {
		mat.mat2d[i][0] = x.data[i];
		mat.mat2d[i][1] = y.data[i];
		mat.mat2d[i][2] = z.data[i];
		mat.mat2d[i][3] = to.data[i];
	}

	mat4x4f_invert(&mat, &mat);
	return mat;
	
	
}

Mat4x4f inline viewport(int x, int y, int w, int h, float normalized_near, float normalized_far) {
	Mat4x4f m = mat4x4f_identity();
	m.mat2d[0][3] = x + (w / 2.f);
	m.mat2d[1][3] = y + (h / 2.f);
	m.mat2d[2][3] = normalized_near + normalized_far / 2.f;

	m.mat2d[0][0] = w / 2.f;
	m.mat2d[1][1] = h / 2.f;
	m.mat2d[2][2] = normalized_far - normalized_near/ 2.0f;
	m.mat2d[3][3] = 1;


	//m = transpose(&m);
	return m;
}




Mat4x4f inline scale(Vec3f vec) {
	Mat4x4f mat = mat4x4f_identity();
	mat.m00 = vec.x;
	mat.m11 = vec.y;
	mat.m22 = vec.z;
	return mat;
}


Mat4x4f inline translate(Vec3f vec) {
	Mat4x4f mat = mat4x4f_identity();
	mat.m03 = vec.x;
	mat.m13 = vec.y;
	mat.m23 = vec.z;
	return mat;
}

Mat4x4f inline rotate(float angle, Vec3f axis) {

	// TODO: we should have a standard saying the angle should be passed in radians 
	angle = deg_to_rad(angle);

	Mat4x4f rot = mat4x4f_identity();
	float cos_angle = cosf(angle);
	float sin_angle = sinf(angle);

	float one_minus_cos = 1 - cos_angle;

	float xy = axis.x * axis.y;
	float yz = axis.y * axis.z;
	float xz = axis.x * axis.z;

	rot.mat2d[0][0] = axis.x * axis.x * one_minus_cos + cos_angle;
	rot.mat2d[0][1] = xy * one_minus_cos - axis.z * sin_angle;
	rot.mat2d[0][2] = xz * one_minus_cos + axis.y * sin_angle;

	rot.mat2d[1][0] = xy * one_minus_cos + axis.z * sin_angle;
	rot.mat2d[1][1] = axis.y * axis.y * one_minus_cos + cos_angle;
	rot.mat2d[1][2] = yz * one_minus_cos - axis.x * sin_angle;

	rot.mat2d[2][0] = xz * one_minus_cos - axis.y * sin_angle;
	rot.mat2d[2][1] = yz * one_minus_cos + axis.x * sin_angle;
	rot.mat2d[2][2] = axis.z * axis.z * one_minus_cos + cos_angle;

	//rot = transpose(&rot);
	return rot;
}


