#ifndef CIRCLE_RENDERER_H
#define CIRCLE_RENDERER_H

#include "io.h"

typedef struct renderer renderer;

typedef struct circle {
	float x;
	float y;
	float r;
	
	color c;
} circle;

typedef struct circle_renderer {
	GLuint vao;
	GLuint vboi;
	GLuint shader;
	
	circle* instances;
	int num_instances;
} circle_renderer;

circle_renderer* circle_renderer_create(renderer* r, int max_instances);
void circle_renderer_upload(circle_renderer* cr);
void circle_renderer_push(circle_renderer* cr, const circle* c);
void circle_renderer_flush(circle_renderer* cr);
void circle_renderer_destroy(circle_renderer* cr);

#endif