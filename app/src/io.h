#ifndef IO_H
#define IO_H

#include <epoxy/gl.h>

#define DARK_THEME 0

typedef struct color {
	float r;
	float g;
	float b;
} color;

typedef struct point {
	float x;
	float y;
	color c;
	int cluster;
} point;

typedef struct file_data {
	int n;
	int k;

	point* data_points;
	point* centroids;
} file_data;

void read_file(const char* filename, char* out);
GLuint create_shader(const char* vfilename, const char* ffilename);
file_data read_file_data(const char* filename);
file_data copy_file_data(const file_data* fd);

#endif
