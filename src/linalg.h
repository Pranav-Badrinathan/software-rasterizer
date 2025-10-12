#ifndef SREND_LINALG_H_
#define SREND_LINALG_H_

// 4D Homogeneous Vectors
struct Vec4 {
	float x;
	float y;
	float z;
	float w;
};

// 4x4 Matricies to work with 4D Homogeneous Vectors.
struct Mat4x4 {
	float v[4][4];
};

/*
* VECTOR FUNCTIONS
*/

// Scale a vector value by a scalar.
// returns: the scaled vector.
struct Vec4 vector_mul(struct Vec4 *v1, float scalar);

// REDUNTANT. vector_mul can also do this. REMOVE LATER.
// Divide a vector value by a scalar.
struct Vec4 vector_div(struct Vec4 *v1, float divisor);

// Add two vectors together.
// returns: the addition of the two provided vectors.
struct Vec4 vector_add(struct Vec4 *v1, struct Vec4 *v2);

// Subtract two vectors.
// returns: the difference of two given vectors.
struct Vec4 vector_sub(struct Vec4 *v1, struct Vec4 *v2);

// Normalize a vector (set it's magnitude to 1 while maintaining direction)
// returns: the normalized vector.
struct Vec4 vector_norm(struct Vec4 *v);

// Get the magnitude of a given vector.
// returns: the magnitude of the vector.
float vector_mag(struct Vec4 *v);

// Get the dot product of two given vectors.
// returns: the dot product.
float vector_dot(struct Vec4 *v1, struct Vec4 *v2);

// Get the cross product of two given vectors.
// returns: the cross product of the vectors.
struct Vec4 vector_cross(struct Vec4 *u, struct Vec4 *v);

/*
* MATRIX FUNCTIONS
*/

// Apply a matrix transformation to a vector.
// returns: the transformed vector.
struct Vec4 matrix_vec_mul(struct Mat4x4 *mat, struct Vec4 *v);

// Multiply two 4x4 matrices together.
// returns: the resulting multiplied matrix.
struct Mat4x4 matrix_matrix_mul(struct Mat4x4 mat1, struct Mat4x4 mat2);

// Transpose a given matrix from Row to Column or vice versa.
// returns: the transposed matrix.
struct Mat4x4 matrix_transpose(struct Mat4x4 *mat);

// Return a Projection Matrix based on given values. 
// v_fov: vertical field of view, in radians.
struct Mat4x4 get_proj_matrix(const float near, 
							  const float far, 
							  const float v_fov, 
							  const float aspect_ratio);

// Returns a Rotation matrix with the given rotation vector.
struct Mat4x4 get_rot_matrix(const struct Vec4 *rot);

// Return a model matrix based on given values.
struct Mat4x4 get_model_matrix(const struct Vec4 *rot, 
							   const struct Vec4 *trans, 
							   const struct Vec4 *scale);

// Take a camera's model matrix, invert it, and return it. This is 
// called the view matrix.
// returns: The view matrix related to a given model matrix.
struct Mat4x4 get_view_matrix(const struct Mat4x4 cam_model);

/*
* PRINT FUNCTIONS
*/

// Print out the given vector in the format:
// {Label} : [v[0], v[1], v[2], v[3]]
void print_vec4(struct Vec4 v, const char* label);

// Print out the given matrix in the format:
// {Label} : [0, 0, 0, 0]
//           [0, 0, 0, 0]
//           [0, 0, 0, 0]
//           [0, 0, 0, 0]
void print_mat4x4(struct Mat4x4 m, const char* label);

#endif
