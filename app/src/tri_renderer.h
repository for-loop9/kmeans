#ifndef TRI_RENDERER_H
#define TRI_RENDERER_H

#include "io.h"

typedef struct renderer renderer;

typedef struct tri {
	float x;
	float y;
	float r;
	
	color c;
} tri;

typedef struct tri_renderer {
	GLuint vao;
	GLuint vbo;
	GLuint vboi;
	GLuint shader;
	
	tri* instances;
	int num_instances;
} tri_renderer;

tri_renderer* tri_renderer_create(renderer* r, int max_instances);
void tri_renderer_upload(tri_renderer* tr);
void tri_renderer_push(tri_renderer* tr, const tri* t);
void tri_renderer_flush(tri_renderer* tr);
void tri_renderer_destroy(tri_renderer* tr);

#endif