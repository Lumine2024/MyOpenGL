#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
uniform mat4 mvp;
out vec4 colorIn;
void main() {
	gl_Position = mvp * position;
	colorIn = color;
}