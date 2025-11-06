#version 330 core
in vec3 colorIn;
out vec4 colorOut;
void main() {
	colorOut = vec4(colorIn, 1.0);
}