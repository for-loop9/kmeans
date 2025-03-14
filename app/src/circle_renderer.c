#include "renderer.h"

#include <stdlib.h>
#include <string.h>

#include "io.h"

circle_renderer* circle_renderer_create(renderer* r, int max_instances) {
	circle_renderer* result = malloc(sizeof(circle_renderer));
	result->instances = malloc(max_instances * sizeof(circle));
	result->num_instances = 0;

	glGenBuffers(1, &result->vboi);
	glBindBuffer(GL_ARRAY_BUFFER, result->vboi);
	glBufferData(GL_ARRAY_BUFFER, max_instances * sizeof(circle), NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &result->vao);
	glBindVertexArray(result->vao);
	glBindBuffer(GL_ARRAY_BUFFER, r->quad_vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, result->vboi);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(circle), (void*) 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(circle), (void*) offsetof(circle, c));
	glVertexAttribDivisor(2, 1);
	glBindVertexArray(0);

	result->shader = create_shader("app/res/shaders/circle.vert", "app/res/shaders/circle.frag");

	return result;
}

void circle_renderer_upload(circle_renderer* cr) {
	glBindBuffer(GL_ARRAY_BUFFER, cr->vboi);
	void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(data, cr->instances, cr->num_instances * sizeof(circle));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void circle_renderer_push(circle_renderer* cr, const circle* c) {
	cr->instances[cr->num_instances++] = *c;
}

void circle_renderer_flush(circle_renderer* cr) {
	glUseProgram(cr->shader);
	glBindVertexArray(cr->vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, cr->num_instances);

	cr->num_instances = 0;
}

void circle_renderer_destroy(circle_renderer* cr) {
	glDeleteProgram(cr->shader);
	glDeleteVertexArrays(1, &cr->vao);
	glDeleteBuffers(1, &cr->vboi);

	free(cr->instances);
	free(cr);
}
