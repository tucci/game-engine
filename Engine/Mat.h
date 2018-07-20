#pragma once

#include "Vec.h"
#include "Math.h"

// TODO: speed up with simd

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

Mat4x4f inline mat4x4f_zero() {
	Mat4x4f mat;

	for (int i = 0; i < 16; i++) {
		mat.mat1d[i] = 0;
	}
	return mat;

}


Mat4x4f inline perspective(float near, float far, float fov, float aspect_ratio) {

	Mat4x4f mat = mat4x4f_identity();

	//float depth = near - far;
	//float inverse_depth = 1 / depth;


	float depth = far - near;
	float inverse_depth = 1 / depth;

	float f = 1 / tan(fov * 0.5f * M_PI / 180);

	mat.mat2d[0][0] = f / aspect_ratio;
	mat.mat2d[1][1] = f;
	mat.mat2d[2][2] = -(far + near) * inverse_depth;
	mat.mat2d[3][2] = 2 * (far * near) * inverse_depth;
	mat.mat2d[2][3] = -1;
	mat.mat2d[3][3] = 0;

	/*float depth = far - near;
	float inverse_depth = 1 / depth;

	mat.mat2d[0][0] = f;
	mat.mat2d[1][1] = f;
	mat.mat2d[2][2] = -far * inverse_depth;
	mat.mat2d[3][2] = (-far * near) * inverse_depth;
	mat.mat2d[2][3] = -1;
	mat.mat2d[3][3] = 0;*/


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
	Vec3f z = vec_normalize(vec_sub(eye, to));
	Vec3f x = vec_normalize(vec_cross(up, z));
	Vec3f y = vec_normalize(vec_cross(z, x));
	Mat4x4f mat = mat4x4f_identity();
	Mat4x4f mat2 = mat4x4f_identity();
	

	for (int i = 0; i<3; i++) {
		mat.mat2d[0][i] = x.data[i];
		mat.mat2d[1][i] = y.data[i];
		mat.mat2d[2][i] = z.data[i];
		mat2.mat2d[i][3] = -to.data[i];
	}

	//mat = transpose(&mat);
	return mat4x4_mul(&mat, &mat2);

	/*mat.mat2d[0][0] = right.x;
	mat.mat2d[0][1] = right.y;
	mat.mat2d[0][2] = right.z;
	mat.mat2d[1][0] = up.x;
	mat.mat2d[1][1] = up.y;
	mat.mat2d[1][2] = up.z;
	mat.mat2d[2][0] = forward.x;
	mat.mat2d[2][1] = forward.y;
	mat.mat2d[2][2] = forward.z;

	mat.mat2d[3][0] = eye.x;
	mat.mat2d[3][1] = eye.y;
	mat.mat2d[3][2] = eye.z;*/

	

	
}

Mat4x4f inline viewport(int x, int y, int w, int h, float depth) {
	Mat4x4f m = mat4x4f_identity();
	m.mat2d[0][3] = x + w / 2.f;
	m.mat2d[1][3] = y + h / 2.f;
	m.mat2d[2][3] = depth / 2.f;

	m.mat2d[0][0] = w / 2.f;
	m.mat2d[1][1] = h / 2.f;
	m.mat2d[2][2] = 1;

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

	angle = deg_to_rad(angle);
	/*Mat4x4f x = mat4x4f_identity();
	Mat4x4f y = mat4x4f_identity();
	Mat4x4f z = mat4x4f_identity();
	x.mat2d[1][1] = cos(angle);
	x.mat2d[2][2] = x.mat2d[1][1];
	x.mat2d[1][2] = sin(angle);
	x.mat2d[2][1] = -x.mat2d[1][2];


	y.mat2d[0][0] = cos(angle);
	y.mat2d[2][2] = y.mat2d[0][0];
	y.mat2d[2][0] = sin(angle);
	y.mat2d[0][2] = -y.mat2d[2][0];


	z.mat2d[0][0] = cos(angle);
	z.mat2d[1][1] = z.mat2d[0][0];
	z.mat2d[0][1] = sin(angle);
	z.mat2d[1][0] = -z.mat2d[0][1];
	
	Mat4x4f yx = mat4x4_mul(&y, &x);
	return mat4x4_mul(&z, &yx);*/


	Mat4x4f rot = mat4x4f_identity();
	float cos_angle = cos(angle);
	float sin_angle = sin(angle);

	float one_minus_cos = 1 - cos_angle;

	float xy = axis.x * axis.y;
	float yz = axis.y * axis.z;
	float xz = axis.x * axis.z;

	rot.mat2d[0][0] = axis.x * axis.x * one_minus_cos + cos_angle;
	rot.mat2d[0][1] = xy * one_minus_cos - axis.z * sin_angle;
	rot.mat2d[0][2] = xz * one_minus_cos - axis.y * sin_angle;

	rot.mat2d[1][0] = xy * one_minus_cos + axis.z * sin_angle;
	rot.mat2d[1][1] = axis.y * axis.y * one_minus_cos + cos_angle;
	rot.mat2d[1][2] = yz * one_minus_cos + axis.x * sin_angle;

	rot.mat2d[2][0] = xz * one_minus_cos - axis.y * sin_angle;
	rot.mat2d[2][1] = yz * one_minus_cos + axis.x * sin_angle;
	rot.mat2d[2][2] = axis.z * axis.z * one_minus_cos + cos_angle;

	rot = transpose(&rot);
	return rot;
}


