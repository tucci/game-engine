#pragma once




#include "Math/Vec.h"
#include "Math/Mat.h"
#include "Math/Quaternion.h"
#include "Common/Map.h"

#include "Core/ECS/Entity.h"
#include "Common/Arena.h"
#include "Common/stretchy_buffer.h"


struct Entity;




struct Transform {
	

	Vec3f get_position;
	Vec3f scale;
	Quat get_rotation;
	// These may or may not be linked to the rotation. depends on the user
	Vec3f forward;
	Vec3f up;
	Vec3f right;

	

	Transform() {
		this->get_position = Vec3f_Zero;
		this->scale = Vec3f_One;
		this->forward = Vec3f_Forward;
		this->up = Vec3f_Up;
		this->right = Vec3f_Right;
		this->get_rotation = Quat();
	}
};

void inline update_direction_vectors_for_transform(Transform& transform) {
	transform.forward = transform.get_rotation * Vec3f_Forward;
	transform.up = transform.get_rotation * Vec3f_Up;
	transform.right = transform.get_rotation * Vec3f_Right;
}


Mat4x4f inline scale(const Vec3f& vec) {
	Mat4x4f mat;
	mat[0][0] = vec.x;
	mat[1][1] = vec.y;
	mat[2][2] = vec.z;
	return mat;
}


Mat4x4f inline translate(const Vec3f& vec) {
	Mat4x4f mat;
	mat[3][0] = vec.x;
	mat[3][1] = vec.y;
	mat[3][2] = vec.z;
	return mat;
}

Mat4x4f inline rotate(float rads, const Vec3f& axis) {
	Mat4x4f result;

	float cos_angle = cosf_(rads);
	float sin_angle = sinf_(rads);

	float one_minus_cos = 1 - cos_angle;

	float xy = axis.x * axis.y;
	float yz = axis.y * axis.z;
	float xz = axis.x * axis.z;

	result[0][0] = axis.x * axis.x * one_minus_cos + cos_angle;
	result[0][1] = xy * one_minus_cos - axis.z * sin_angle;
	result[0][2] = xz * one_minus_cos + axis.y * sin_angle;

	result[1][0] = xy * one_minus_cos + axis.z * sin_angle;
	result[1][1] = axis.y * axis.y * one_minus_cos + cos_angle;
	result[1][2] = yz * one_minus_cos - axis.x * sin_angle;

	result[2][0] = xz * one_minus_cos - axis.y * sin_angle;
	result[2][1] = yz * one_minus_cos + axis.x * sin_angle;
	result[2][2] = axis.z * axis.z * one_minus_cos + cos_angle;

	//result = transpose(result);
	return result;
}





struct TransformManager {
	
	CompactMap<u64> id_map;
	
	u64 enabled_count; // Amount of enabled transform components
	u64 total_count; // Total of transform components that are enabled and disabled

	Entity* entitys;

	// Global
	Vec3f* positions;
	Vec3f* scales;
	Quat* rotations;

	Vec3f* ups;
	Vec3f* forwards;
	Vec3f* rights;	

	Mat4x4f* local;
	Mat4x4f* world;


	// The entity tree uses a first_child/next sibling
	// Note that entity 0, is reserved for a None entity. 0 Is not the Root entity
	// Root entity is created by the entity manager
	// To check if something is the root entity, look using entity_manager->root

	// Example
	//              root
	//              /
	//             6
	//            / 
	//           3 -> 2 -> 5
	//         /          / 
	//         1->4      7->8

	// Then the first_child/next/prev/count table would look like

	//	    	|root|1|2|3|4|5|6|7|8|
	//_______________________________
	// parent   |   0|3|6|6|3|6|0|5|5|
	// first	|   6|0|0|1|0|7|3|0|0|
	// next		|   0|4|5|2|0|0|0|8|0|
	// prev		|   0|0|3|0|1|2|0|0|7|
	// count	|   1|0|0|2|0|2|3|0|0| 


	


	Entity* parent;
	Entity* first_child;
	Entity* next_sibling;
	Entity* prev_sibling;
	u64* child_count;

	

	

	
};




void init_transform_manager(TransformManager* manager);
void destroy_transform_manager(TransformManager* manager);
bool entity_add_transform_component(TransformManager* manager, Entity entity);
bool entity_remove_transform_component(TransformManager* manager, Entity entity);

