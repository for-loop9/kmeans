#version 330 core

out vec4 color;

in vec3 out_color;
in vec2 out_uv;

void main() {
	float d = length(out_uv);
   	float p = length(vec2(dFdx(d), dFdy(d)));
	float a = smoothstep(0.5, 0.5 - p * 1.5, d);
	color = vec4(out_color, a);
}