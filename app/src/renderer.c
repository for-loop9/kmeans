#include "renderer.h"
#include <stdlib.h>
#include <string.h>

renderer* renderer_create(int max_lines, int max_circles, int max_tris, int max_dis) {
	renderer* result = malloc(sizeof(renderer));
	result->global.width = 0;
	result->global.height = 0;
	result->global.cx = 0;
	result->global.cy = 0;
	result->global.cs = 13;

	float quad_data[] = {
		-1, -1,
		-1, 1,
		1, -1,
		1, 1
	};

	glGenBuffers(1, &result->ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, result->ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(result->global), &result->global, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, result->ubo);

	glGenBuffers(1, &result->quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, result->quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);

	result->lr = line_renderer_create(result, max_lines);
	result->cr = circle_renderer_create(result, max_circles);
	result->tr = tri_renderer_create(result, max_tris);
	result->dr = di_renderer_create(result, max_dis);

	return result;
}

void renderer_push_line(renderer* r, const line* l) {
	line_renderer_push(r->lr, l);
}

void renderer_push_circle(renderer* r, const circle* c) {
	circle_renderer_push(r->cr, c);
}

void renderer_push_tri(renderer* r, const tri* t) {
	tri_renderer_push(r->tr, t);
}

void renderer_push_di(renderer* r, const di* d) {
	di_renderer_push(r->dr, d);
}

void renderer_flush(renderer* r) {
	line_renderer_upload(r->lr);
	circle_renderer_upload(r->cr);
	tri_renderer_upload(r->tr);
	di_renderer_upload(r->dr);

	glBindBuffer(GL_UNIFORM_BUFFER, r->ubo);
	void* data = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(data, &r->global, sizeof(r->global));
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	line_renderer_flush(r->lr);
	di_renderer_flush(r->dr);
	tri_renderer_flush(r->tr);
	circle_renderer_flush(r->cr);
}

void renderer_destroy(renderer* r) {
	di_renderer_destroy(r->dr);
	tri_renderer_destroy(r->tr);
	circle_renderer_destroy(r->cr);
	line_renderer_destroy(r->lr);

	glDeleteBuffers(1, &r->quad_vbo);
	glDeleteBuffers(1, &r->ubo);

	free(r);
}