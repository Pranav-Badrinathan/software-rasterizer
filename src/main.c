#include "defs.h"
#include "gui.h"
#include "linalg.h"
#include "draw.h"
#include "input.h"
#include <stdio.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <sys/time.h>

SDL_Window *window;
struct Vec4 inp_rots = {0};
struct Vec4 cam_trans = {0};
struct Vec4 delta_trans = {0};

void err_if_null(void *ptr, char *str) {
	if (!ptr) {
		fprintf(stderr, "%s", str);
		exit(EXIT_FAILURE);
	}
}

void cam_transform(void) {
	delta_trans = (struct Vec4) {0,0,0,0};
	if (key_held(SDL_SCANCODE_W)) delta_trans.z -= 0.0007;
	if (key_held(SDL_SCANCODE_S)) delta_trans.z += 0.0007;
	if (key_held(SDL_SCANCODE_A)) delta_trans.x -= 0.0007;
	if (key_held(SDL_SCANCODE_D)) delta_trans.x += 0.0007;
	if (key_held(SDL_SCANCODE_SPACE)) delta_trans.y -= 0.0007;
	if (key_held(SDL_SCANCODE_LSHIFT)) delta_trans.y += 0.0007;

	if (key_held(SDL_SCANCODE_LEFT)) inp_rots.y += 0.0005;
	if (key_held(SDL_SCANCODE_RIGHT)) inp_rots.y -= 0.0005;
	if (key_held(SDL_SCANCODE_UP)) inp_rots.x -= 0.0005;
	if (key_held(SDL_SCANCODE_DOWN)) inp_rots.x += 0.0005;
}

void draw_cube(SDL_Surface *surface, long delta) {
	struct Triangle cube[12] = {
		(struct Triangle) {(struct Vec4){0, 0, 0, 1}, (struct Vec4){0, 1, 0, 1}, (struct Vec4){1, 1, 0, 1}},
		(struct Triangle) {(struct Vec4){0, 0, 0, 1}, (struct Vec4){1, 1, 0, 1}, (struct Vec4){1, 0, 0, 1}},

		(struct Triangle) {(struct Vec4){1, 0, 0, 1}, (struct Vec4){1, 1, 0, 1}, (struct Vec4){1, 1, 1, 1}},
		(struct Triangle) {(struct Vec4){1, 0, 0, 1}, (struct Vec4){1, 1, 1, 1}, (struct Vec4){1, 0, 1, 1}},

		(struct Triangle) {(struct Vec4){1, 0, 1, 1}, (struct Vec4){1, 1, 1, 1}, (struct Vec4){0, 1, 1, 1}},
		(struct Triangle) {(struct Vec4){1, 0, 1, 1}, (struct Vec4){0, 1, 1, 1}, (struct Vec4){0, 0, 1, 1}},

		(struct Triangle) {(struct Vec4){0, 0, 1, 1}, (struct Vec4){0, 1, 1, 1}, (struct Vec4){0, 1, 0, 1}},
		(struct Triangle) {(struct Vec4){0, 0, 1, 1}, (struct Vec4){0, 1, 0, 1}, (struct Vec4){0, 0, 0, 1}},

		(struct Triangle) {(struct Vec4){0, 1, 0, 1}, (struct Vec4){0, 1, 1, 1}, (struct Vec4){1, 1, 1, 1}},
		(struct Triangle) {(struct Vec4){0, 1, 0, 1}, (struct Vec4){1, 1, 1, 1}, (struct Vec4){1, 1, 0, 1}},

		(struct Triangle) {(struct Vec4){1, 0, 1, 1}, (struct Vec4){0, 0, 1, 1}, (struct Vec4){0, 0, 0, 1}},
		(struct Triangle) {(struct Vec4){1, 0, 1, 1}, (struct Vec4){0, 0, 0, 1}, (struct Vec4){1, 0, 0, 1}},
	};

	cam_transform();

	struct Mat4x4 proj = get_proj_matrix(0.1f, 1000.0f, M_PI/4, 
									  (float)SCREEN_HEIGHT/(float)SCREEN_WIDTH);

	struct Mat4x4 model = get_model_matrix(&(struct Vec4){0.0f, 0.0f,  0.0f},
										   &(struct Vec4){0.0f, 0.0f, -3.0f},
										   &(struct Vec4){1.0f, 1.0f,  1.0f});


	// Transform the input absolute position to camera relative position.
	struct Mat4x4 cam_rot = get_rot_matrix(&inp_rots);
	struct Vec4 rel_trans = matrix_vec_mul(&cam_rot, &delta_trans);

	cam_trans = vector_add(&cam_trans, &rel_trans);

	struct Mat4x4 cam_m = get_model_matrix(&inp_rots,
										   &cam_trans,
										   &(struct Vec4){1.0f, 1.0f, 1.0f});

	struct Mat4x4 view = get_view_matrix(cam_m);

	struct Mat4x4 model_view = matrix_matrix_mul(model, view);

/*	for (int i = 0; i < 4; i++) {
		printf("| ");
		for (int j = 0; j < 4; j++) {
			printf("%.2f ", model.v[j][i]);
		}
		printf("|\n");
	}
	printf("----------\n"); */

	for (int i = 0; i < 12; i++) {
		struct Triangle t;
		for (int j = 0; j < 3; j++) {
			t.verts[j] = cube[i].verts[j];

			// This can be done with a model matrix.
			t.verts[j] = matrix_vec_mul(&model_view, &t.verts[j]);

			// Projection matrix step.
			t.verts[j] = matrix_vec_mul(&proj, &t.verts[j]);
			// Projection division step.
			t.verts[j] = vector_div(&t.verts[j], t.verts[j].w);

			t.verts[j] = vector_add(&t.verts[j], &((struct Vec4){1, 1, 0}));
			
			t.verts[j].x *= 0.5f * (float)SCREEN_WIDTH;
			t.verts[j].y *= 0.5f * (float)SCREEN_HEIGHT;

			/* printf("x: %f, y: %f, w: %f\n", t.verts[j].x, t.verts[j].y, t.verts[j].w); */
		}

		struct Vec4 a = vector_sub(&t.verts[0], &t.verts[1]);
		struct Vec4 b = vector_sub(&t.verts[0], &t.verts[2]);

		// Positive if CCW, negative if CW. Only render CCW faces.
		// 2D cross prod (determinant formula).
		float winding_order = (a.x * b.y) - (b.x * a.y);

		// Only draw if winding in CCW. CW means backface.
		if (winding_order > 0) draw_triangle(surface, t);
	}
}

void init_sdl(void) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Setting x and y pos to SDL_WINDOW_POS_UNDEFINED leaves window positioning
	// upto the OS
	window = SDL_CreateWindow(
		"Renderer", 
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 
		SCREEN_WIDTH, SCREEN_HEIGHT, 
		SDL_WINDOW_RESIZABLE
	);
	err_if_null(window, "Couldn't initialize SDL Window. Returned NULL!");
}

void draw_loop(long delta) {
	err_if_null(window, "Couldn't initialize SDL Window. Returned NULL!");

	SDL_Surface *surface = SDL_GetWindowSurface(window);
	err_if_null(surface, "Couldn't initialize SDL Surface. Returned NULL!");

	SDL_LockSurface(surface);
	SDL_FillRect(surface, NULL, 0x000000);

	draw_cube(surface, delta);
	
	SDL_UnlockSurface(surface);
	SDL_UpdateWindowSurface(window);
}

void cleanup(void) {
	// Handle application closing
	SDL_DestroyWindow(window);
	SDL_Quit();
}

double delta_time = 0;

int main(int argc, char *argv[]) {
	struct timeval start;
	struct timeval end;

	// As name suggests, initialize SDL.
	init_sdl();
	init_gui(window);

	atexit(cleanup);

	// Main "game" loop.
	while (1) {
		gettimeofday(&start, NULL);
		update_input();

		draw_loop(delta_time);
		draw_gui();
		gettimeofday(&end, NULL);
		delta_time = ((((end.tv_sec - start.tv_sec) * 1000000) + end.tv_usec) - start.tv_usec) / 1000000.0;

		/* printf("FPS: %f\n", 1/delta_time); */
	}

	return EXIT_SUCCESS;
}
