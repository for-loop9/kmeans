#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void read_file(const char* filename, char* out) {
	FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("file error!\n");
        return;
    }

    char c;
    while ((c = fgetc(file)) != EOF) {
        *out++ = c;
    }

	fclose(file);
}

GLuint create_shader(const char* vfilename, const char* ffilename) {
	GLuint vid;
	GLuint fid;
	GLuint pid;

	char vshader_data[4096] = {};
	char fshader_data[4096] = {};

	read_file(vfilename, vshader_data);
	read_file(ffilename, fshader_data);

	const char* vcc = vshader_data;
	const char* fcc = fshader_data;

	vid = glCreateShader(GL_VERTEX_SHADER);
	fid = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vid, 1, &vcc, NULL);
	glShaderSource(fid, 1, &fcc, NULL);

	glCompileShader(vid);
	glCompileShader(fid);

	pid = glCreateProgram();
	glAttachShader(pid, vid);
	glAttachShader(pid, fid);

	glLinkProgram(pid);

	glDetachShader(pid, fid);
	glDetachShader(pid, vid);
	glDeleteShader(fid);
	glDeleteShader(vid);

	glUseProgram(pid);
	unsigned int global_index = glGetUniformBlockIndex(pid, "Global");
	glUniformBlockBinding(pid, global_index, 0);

	return pid;
}

void shuffle_colors(color* colors, size_t size) {
	if (size <= 1)
		return;

	srand((unsigned int) time(NULL));

	for (size_t i = size - 1; i > 0; i--) {
		size_t j = rand() % (i + 1);
		color temp = colors[i];
		colors[i] = colors[j];
		colors[j] = temp;
	}
}

file_data read_file_data(const char* filename) {
	file_data result;
	FILE* file = fopen(filename, "r");

	if (!file) {
		printf("file error!\n");
		exit(-1);
	}

	color pastel_colors[] = {
		{ 0.95, 0.63, 0.81 },
		{ 0.68, 0.85, 0.90 },
		{ 0.77, 0.93, 0.78 },
		{ 0.99, 0.85, 0.61 },
		{ 0.91, 0.80, 0.88 },
		{ 0.62, 0.82, 0.95 },
		{ 0.85, 0.70, 0.90 },
		{ 0.85, 0.87, 0.63 },
		{ 0.97, 0.76, 0.75 },
		{ 0.94, 0.80, 0.65 } 
	};
	// shuffle_colors(pastel_colors, 10);

	fscanf(file, "%d", &result.n);
	result.data_points = malloc(result.n * sizeof(point));
	for (int i = 0; i < result.n; i++) {
		fscanf(file, "%f %f", &result.data_points[i].x, &result.data_points[i].y);
		#if DARK_THEME
		result.data_points[i].c = (color) { .r = 0.4f, .g = 0.4f, .b = 0.4f };
		#else
		result.data_points[i].c = (color) { .r = 0.6f, .g = 0.7f, .b = 0.7f };
		#endif
		result.data_points[i].cluster = -1;
	}

	fscanf(file, "%d", &result.k);
	result.centroids = malloc(result.k * sizeof(point));
	for (int i = 0; i < result.k; i++) {
		fscanf(file, "%f %f", &result.centroids[i].x, &result.centroids[i].y);
		result.centroids[i].c = pastel_colors[i % 10];
		result.centroids[i].cluster = -1;
	}

	fclose(file);
	return result;
}

file_data copy_file_data(const file_data* fd) {
	file_data result;
	result.n = fd->n;
	result.k = fd->k;

	result.data_points = malloc(result.n * sizeof(point));
	result.centroids = malloc(result.k * sizeof(point));

	memcpy(result.data_points, fd->data_points, result.n * sizeof(point));
	memcpy(result.centroids, fd->centroids, result.k * sizeof(point));

	return result;	
}