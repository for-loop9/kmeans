#ifndef RENDERER_H
#define RENDERER_H

#include <epoxy/gl.h>
#include <stdalign.h>
#include "circle_renderer.h"
#include "line_renderer.h"
#include "tri_renderer.h"
#include "di_renderer.h"

typedef struct renderer {
	GLuint ubo;
	GLuint quad_vbo;
	
	struct {
		alignas(4) float width;
		alignas(4) float height;
		alignas(4) float cx;
		alignas(4) float cy;
		alignas(4) float cs;
	} global;

	line_renderer* lr;
	circle_renderer* cr;
	tri_renderer* tr;
	di_renderer* dr;
} renderer;

renderer* renderer_create(int max_lines, int max_circles, int max_tris, int max_dis);
void renderer_push_line(renderer* r, const line* l);
void renderer_push_circle(renderer* r, const circle* c);
void renderer_push_tri(renderer* r, const tri* t);
void renderer_push_di(renderer* r, const di* d);
void renderer_flush(renderer* r);
void renderer_destroy(renderer* r);

#endif