#ifndef LINE_RENDERER_H
#define LINE_RENDERER_H

#include "io.h"

typedef struct renderer renderer;

typedef struct line {
	float x0;
	float y0;
	float x1;
	float y1;
	
	color c;
} line;

typedef struct vbo_line {
	float x0;
	float y0;
	color c0;
	float x1;
	float y1;
	color c1;
} vbo_line;

typedef struct line_renderer {
	GLuint vao;
	GLuint vbo;
	GLuint shader;
	
	vbo_line* vbo_lines;
	int num_instances;
} line_renderer;

line_renderer* line_renderer_create(renderer* r, int max_instances);
void line_renderer_upload(line_renderer* lr);
void line_renderer_push(line_renderer* lr, const line* l);
void line_renderer_flush(line_renderer* lr);
void line_renderer_destroy(line_renderer* lr);

#endif