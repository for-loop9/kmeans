#ifndef DI_RENDERER_H
#define DI_RENDERER_H

#include "io.h"

typedef struct renderer renderer;

typedef struct di {
	float x;
	float y;
	float r;
	
	color c;
} di;

typedef struct di_renderer {
	GLuint vao;
	GLuint vbo;
	GLuint vboi;
	GLuint shader;
	
	di* instances;
	int num_instances;
} di_renderer;

di_renderer* di_renderer_create(renderer* r, int max_instances);
void di_renderer_upload(di_renderer* dr);
void di_renderer_push(di_renderer* dr, const di* d);
void di_renderer_flush(di_renderer* dr);
void di_renderer_destroy(di_renderer* dr);

#endif