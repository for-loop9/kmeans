#version 330 core

out vec4 color;

in vec3 out_color;

void main() {
	color = vec4(out_color, 1);
}