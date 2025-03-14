#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "io.h"
#include "renderer.h"

line_renderer* line_renderer_create(renderer* r, int max_instances) {
	line_renderer* result = malloc(sizeof(line_renderer));
	result->vbo_lines = malloc(max_instances * sizeof(vbo_line));
	result->num_instances = 0;

	glGenBuffers(1, &result->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, result->vbo);
	glBufferData(GL_ARRAY_BUFFER, max_instances * sizeof(vbo_line), NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &result->vao);
	glBindVertexArray(result->vao);
	glBindBuffer(GL_ARRAY_BUFFER, result->vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 20, (void*) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 20, (void*) 8);

	result->shader = create_shader("app/res/shaders/line.vert", "app/res/shaders/line.frag");

	return result;
}

void line_renderer_upload(line_renderer* lr) {
	glBindBuffer(GL_ARRAY_BUFFER, lr->vbo);
	void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(data, lr->vbo_lines, lr->num_instances * sizeof(vbo_line));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void line_renderer_push(line_renderer* lr, const line* l) {
	lr->vbo_lines[lr->num_instances].x0 = l->x0;
	lr->vbo_lines[lr->num_instances].y0 = l->y0;
	lr->vbo_lines[lr->num_instances].c0.r = l->c.r;
	lr->vbo_lines[lr->num_instances].c0.g = l->c.g;
	lr->vbo_lines[lr->num_instances].c0.b = l->c.b;
	lr->vbo_lines[lr->num_instances].x1 = l->x1;
	lr->vbo_lines[lr->num_instances].y1 = l->y1;
	lr->vbo_lines[lr->num_instances].c1.r = l->c.r;
	lr->vbo_lines[lr->num_instances].c1.g = l->c.g;
	lr->vbo_lines[lr->num_instances].c1.b = l->c.b;

	lr->num_instances++;
}

void line_renderer_flush(line_renderer* lr) {
	glUseProgram(lr->shader);
	glBindVertexArray(lr->vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_LINES, 0, lr->num_instances * 2);

	lr->num_instances = 0;
}

void line_renderer_destroy(line_renderer* lr) {
	glDeleteProgram(lr->shader);
	glDeleteVertexArrays(1, &lr->vao);
	glDeleteBuffers(1, &lr->vbo);

	free(lr->vbo_lines);
	free(lr);
}