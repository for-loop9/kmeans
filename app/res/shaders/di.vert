#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 transform;
layout (location = 2) in vec3 color;

layout (std140) uniform Global {
	float width;
	float height;
	float cx;
	float cy;
	float cs;
};

out vec3 out_color;

void main() {
	vec2 ortho = vec2(0, 0);
	ortho.x = (((position.x * transform.z + transform.x) - cx) * cs) / (width / 2);
	ortho.y = (((position.y * transform.z + transform.y) - cy) * cs) / (height / 2);
	gl_Position = vec4(ortho, 0, 1);

	out_color = color;
}