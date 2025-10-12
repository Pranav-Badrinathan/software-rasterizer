#include "linalg.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* VECTOR OPERATIONS
*/

struct Vec4 vector_add(struct Vec4 *v1, struct Vec4 *v2) {
	return (struct Vec4) {
		v1->x + v2->x,
		v1->y + v2->y,
		v1->z + v2->z,
	};
}

struct Vec4 vector_sub(struct Vec4 *v1, struct Vec4 *v2) {
	return (struct Vec4) {
		v1->x - v2->x,
		v1->y - v2->y,
		v1->z - v2->z,
	};
}

struct Vec4 vector_mul(struct Vec4 *v1, float scalar) {
	return (struct Vec4) {
		v1->x * scalar,
		v1->y * scalar,
		v1->z * scalar,
	};
}

struct Vec4 vector_div(struct Vec4 *v1, float divisor) {
	return (struct Vec4) {
		v1->x / divisor,
		v1->y / divisor,
		v1->z / divisor,
	};
}

float vector_dot(struct Vec4 *v1, struct Vec4 *v2) {
	return (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z) + (v1->w * v2->w);
}

struct Vec4 vector_cross(struct Vec4 *u, struct Vec4 *v) {
	return (struct Vec4) {
		u->y * v->z - u->z * v->y,
		u->z * v->x - u->x * v->z,
		u->x * v->y - u->y * v->x,
	};
}

float vector_mag(struct Vec4 *v) {
	return sqrtf(vector_dot(v, v));
}

struct Vec4 vector_norm(struct Vec4 *v) {
	return vector_div(v, vector_mag(v));
}

/*
* MATRIX OPERATIONS 
*/

struct Vec4 matrix_vec_mul(struct Mat4x4 *mat, struct Vec4 *v) {
	struct Vec4 point = {0};

	point.x = v->x * mat->v[0][0] + v->y * mat->v[1][0] + v->z * mat->v[2][0] + v->w * mat->v[3][0];
	point.y = v->x * mat->v[0][1] + v->y * mat->v[1][1] + v->z * mat->v[2][1] + v->w * mat->v[3][1];
	point.z = v->x * mat->v[0][2] + v->y * mat->v[1][2] + v->z * mat->v[2][2] + v->w * mat->v[3][2];
	point.w = v->x * mat->v[0][3] + v->y * mat->v[1][3] + v->z * mat->v[2][3] + v->w * mat->v[3][3];

	return point;
}

struct Mat4x4 matrix_transpose(struct Mat4x4 *mat) {
	struct Mat4x4 ret = {0};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			ret.v[j][i] = mat->v[i][j];
		}
	}

	return ret;
}

struct Mat4x4 matrix_matrix_mul(struct Mat4x4 mat1, struct Mat4x4 mat2) {
	struct Mat4x4 ret = {0};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				ret.v[i][j] += mat1.v[i][k] * mat2.v[k][j];
			}
		}
	}

	return ret;
}

// PREDEFINED MATRICIES

struct Mat4x4 get_identity_matrix(void) {
	struct Mat4x4 identity = {0};
	identity.v[0][0] = 1.0f;
	identity.v[1][1] = 1.0f;
	identity.v[2][2] = 1.0f;
	identity.v[3][3] = 1.0f;

	return identity;
}

struct Mat4x4 get_proj_matrix(const float near, 
							  const float far, 
							  const float v_fov, 
							  const float aspect_ratio) {
	// Initialize matrix to 0;
	struct Mat4x4 mat = {0};

	const float atan = 1.0f / tanf(v_fov/2);

	mat.v[0][0] = aspect_ratio * atan;
	mat.v[1][1] = atan;
	mat.v[2][2] = far / (far - near);
	mat.v[3][2] = (-far * near) / (far - near);
	mat.v[2][3] = -1.0f;
	mat.v[3][3] = 0.0f;

	return mat;
}

struct Mat4x4 get_rot_matrix(const struct Vec4 *rot) {
	float cosX = cosf(rot->x);
	float sinX = sinf(rot->x);
	float cosY = cosf(rot->y);
	float sinY = sinf(rot->y);
	float cosZ = cosf(rot->z);
	float sinZ = sinf(rot->z);

	struct Mat4x4 mat_x = get_identity_matrix();
	mat_x.v[1][1] = cosX;
	mat_x.v[2][2] = cosX;
	mat_x.v[2][1] = -sinX;
	mat_x.v[1][2] = sinX;

	struct Mat4x4 mat_y = get_identity_matrix();
	mat_y.v[0][0] = cosY;
	mat_y.v[2][2] = cosY;
	mat_y.v[0][2] = -sinY;
	mat_y.v[2][0] = sinY;

	struct Mat4x4 mat_z = get_identity_matrix();
	mat_z.v[0][0] = cosZ;
	mat_z.v[1][1] = cosZ;
	mat_z.v[1][0] = -sinZ;
	mat_z.v[0][1] = sinZ;

	return matrix_matrix_mul(matrix_matrix_mul(mat_x, mat_y), mat_z);
}

struct Mat4x4 get_scale_matrix(const struct Vec4 *scale) {
	struct Mat4x4 mat = get_identity_matrix();

	mat.v[0][0] = scale->x;
	mat.v[1][1] = scale->y;
	mat.v[2][2] = scale->z;
	return mat;
}

struct Mat4x4 get_trans_matrix(const struct Vec4 *trans) {

	struct Mat4x4 mat = get_identity_matrix();

	mat.v[3][0] = trans->x;
	mat.v[3][1] = trans->y;
	mat.v[3][2] = trans->z;
	return mat;
}

struct Mat4x4 get_model_matrix(const struct Vec4 *rot, 
							   const struct Vec4 *trans, 
							   const struct Vec4 *scale) {
	struct Mat4x4 m_scale = get_scale_matrix(scale);
	struct Mat4x4 m_rot = get_rot_matrix(rot);
	struct Mat4x4 m_trans = get_trans_matrix(trans);
	
	return matrix_matrix_mul(matrix_matrix_mul(m_scale, m_rot), m_trans);
}

/*
* For a model matrix of form:, The view matrix is:
*
*  Model Matrix:		View Matrix:
*  [ux vx wx tx]		[ux uy uz -dot(u,t)]
*  [uy vy wy ty]		[vx vy vz -dot(v,t)]
*  [uz vz wz tz]		[wx wy wz -dot(w,t)]
*  [ 0  0  0  1]		[ 0  0  0     1    ]
*  
*  Notice the top 3x3 matrix in the view matrix is the transposed top 3x3
*  of the model matrix. So, define the translation part (with dot prods)
*  in the bottom row, and then transpose the whole matrix.
*
*  source: https://graphics.stanford.edu/courses/cs248-98-fall/Final/q4.html
*/
struct Mat4x4 get_view_matrix(const struct Mat4x4 cam_model) {
	// Copy the Model matrix.
	struct Mat4x4 view = cam_model;

	// Get the individual vectors.
	struct Vec4 u = { view.v[0][0], view.v[0][1], view.v[0][2] };
	struct Vec4 v = { view.v[1][0], view.v[1][1], view.v[1][2] };
	struct Vec4 w = { view.v[2][0], view.v[2][1], view.v[2][2] };
	struct Vec4 t = { view.v[3][0], view.v[3][1], view.v[3][2] };

	// Setup the bottom row as dot prods. Transposing will put it in place.
	view.v[0][3] = -vector_dot(&u, &t);
	view.v[1][3] = -vector_dot(&v, &t);
	view.v[2][3] = -vector_dot(&w, &t);

	// Zero out the last column.
	view.v[3][0] = 0;
    view.v[3][1] = 0;
    view.v[3][2] = 0;

	return matrix_transpose(&view);
}

/*
* PRINT SECTION
*/

void print_vec4(struct Vec4 v, const char* label) {
	printf("%s : [ %f, %f, %f, %f ]\n", label, v.x, v.y, v.z, v.w);
}


void print_mat4x4(struct Mat4x4 m, const char* label) {
	int len = strlen(label) + 3;
	
	char* spaces = (char*) malloc(sizeof(char) * (len + 1));
	memset(spaces, ' ', len);
	
	for (int i = 0; i < 4; i++) {
		if (i == 0) {
			printf("%s : ", label);
		} else {
			printf("%s", spaces);
		}
		printf("[ %f, %f, %f, %f ]\n", m.v[0][0], m.v[1][0], m.v[2][0], m.v[3][0]);
	}
	printf("\n");

	free(spaces);
}
