#include "renderer.h"

#include <stdlib.h>
#include <string.h>

#include "io.h"

tri_renderer* tri_renderer_create(renderer* r, int max_instances) {
	tri_renderer* result = malloc(sizeof(tri_renderer));
	result->instances = malloc(max_instances * sizeof(tri));
	result->num_instances = 0;

	float tri_data[] = {
		0, 0.577f * 2,
		-0.5f * 2, -0.289f * 2,
		0.5f * 2, -0.289f * 2,
	};

	glGenBuffers(1, &result->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, result->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri_data), tri_data, GL_STATIC_DRAW);

	glGenBuffers(1, &result->vboi);
	glBindBuffer(GL_ARRAY_BUFFER, result->vboi);
	glBufferData(GL_ARRAY_BUFFER, max_instances * sizeof(tri), NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &result->vao);
	glBindVertexArray(result->vao);
	glBindBuffer(GL_ARRAY_BUFFER, result->vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, result->vboi);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(tri), (void*) 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(tri), (void*) offsetof(tri, c));
	glVertexAttribDivisor(2, 1);
	glBindVertexArray(0);

	result->shader = create_shader("app/res/shaders/tri.vert", "app/res/shaders/tri.frag");

	return result;
}

void tri_renderer_upload(tri_renderer* tr) {
	glBindBuffer(GL_ARRAY_BUFFER, tr->vboi);
	void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(data, tr->instances, tr->num_instances * sizeof(tri));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void tri_renderer_push(tri_renderer* tr, const tri* t) {
	tr->instances[tr->num_instances++] = *t;
}

void tri_renderer_flush(tri_renderer* tr) {
	glUseProgram(tr->shader);
	glBindVertexArray(tr->vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, tr->num_instances);

	tr->num_instances = 0;
}

void tri_renderer_destroy(tri_renderer* tr) {
	glDeleteProgram(tr->shader);
	glDeleteVertexArrays(1, &tr->vao);
	glDeleteBuffers(1, &tr->vboi);
	glDeleteBuffers(1, &tr->vbo);

	free(tr->instances);
	free(tr);
}