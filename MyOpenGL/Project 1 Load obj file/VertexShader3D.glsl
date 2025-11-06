#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 norm;
layout(location = 2) in vec4 color;
uniform mat4 mvp;
uniform vec4 light;
out vec4 colorIn;
void main() {
	gl_Position = mvp * position;
	float c = dot(light, normalize(mvp * norm));
	float x = 1.0f - 0.5f * (1.0f - c);
	colorIn = vec4(color.rgb * x, color.a);
}