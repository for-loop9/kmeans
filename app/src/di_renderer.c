#include "renderer.h"

#include <stdlib.h>
#include <string.h>

#include "io.h"

di_renderer* di_renderer_create(renderer* r, int max_instances) {
	di_renderer* result = malloc(sizeof(di_renderer));
	result->instances = malloc(max_instances * sizeof(di));
	result->num_instances = 0;

	float di_data[] = {
		0, 1,
		-1, 0,
		1, 0,
		0, -1
	};

	glGenBuffers(1, &result->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, result->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(di_data), di_data, GL_STATIC_DRAW);

	glGenBuffers(1, &result->vboi);
	glBindBuffer(GL_ARRAY_BUFFER, result->vboi);
	glBufferData(GL_ARRAY_BUFFER, max_instances * sizeof(di), NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &result->vao);
	glBindVertexArray(result->vao);
	glBindBuffer(GL_ARRAY_BUFFER, result->vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, result->vboi);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(di), (void*) 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(di), (void*) offsetof(di, c));
	glVertexAttribDivisor(2, 1);
	glBindVertexArray(0);

	result->shader = create_shader("app/res/shaders/di.vert", "app/res/shaders/di.frag");

	return result;
}

void di_renderer_upload(di_renderer* dr) {
	glBindBuffer(GL_ARRAY_BUFFER, dr->vboi);
	void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(data, dr->instances, dr->num_instances * sizeof(di));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void di_renderer_push(di_renderer* dr, const di* d) {
	dr->instances[dr->num_instances++] = *d;
}

void di_renderer_flush(di_renderer* dr) {
	glUseProgram(dr->shader);
	glBindVertexArray(dr->vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, dr->num_instances);

	dr->num_instances = 0;
}

void di_renderer_destroy(di_renderer* dr) {
	glDeleteProgram(dr->shader);
	glDeleteVertexArrays(1, &dr->vao);
	glDeleteBuffers(1, &dr->vboi);
	glDeleteBuffers(1, &dr->vbo);

	free(dr->instances);
	free(dr);
}