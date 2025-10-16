#pragma once
#include "Geometry2D.h"
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>
#include <fstream>
#include <iostream>

using namespace gl;

struct PointData2D {
	Point2D point;
	glm::vec3 color;
};



//class ShapeDrawer2D {
//public:
//	ShapeDrawer2D() {
//		static char errorBuffer[1 << 12], srcCodeBuffer[1 << 16];
//		auto compileShader = [](GLenum type, const char *src) -> GLuint {
//			GLuint shader = glCreateShader(type);
//			glShaderSource(shader, 1, &src, nullptr);
//			glCompileShader(shader);
//			GLint ok = 0;
//			glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
//			if(!ok) {
//				glGetShaderInfoLog(shader, 1 << 12, nullptr, errorBuffer);
//				std::cout << errorBuffer << std::endl;
//			}
//			return shader;
//		};
//		std::ifstream fVertexShader("VertexShader2D.glsl");
//		fVertexShader.read(srcCodeBuffer, LLONG_MAX);
//		srcCodeBuffer[fVertexShader.gcount()] = '\0';
//		GLuint vertexShader = compileShader(GL_VERTEX_SHADER, srcCodeBuffer);
//		fVertexShader.close();
//		std::ifstream fFragmentShader("FragmentShader2D.glsl");
//		fFragmentShader.read(srcCodeBuffer, LLONG_MAX);
//		srcCodeBuffer[fFragmentShader.gcount()] = '\0';
//		GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, srcCodeBuffer);
//		fFragmentShader.close();
//		program = glCreateProgram();
//		glAttachShader(program, vertexShader);
//		glAttachShader(program, fragmentShader);
//		glLinkProgram(program);
//		GLint ok = 0;
//		glGetProgramiv(program, GL_LINK_STATUS, &ok);
//		if(!ok) {
//			glGetProgramInfoLog(program, 1 << 12, nullptr, errorBuffer);
//			std::cout << errorBuffer << std::endl;
//		}
//		glDeleteShader(vertexShader);
//		glDeleteShader(fragmentShader);
//	}
//	~ShapeDrawer2D() {
//		glDeleteProgram(program);
//		glDeleteVertexArrays(1, &vao);
//		glDeleteBuffers(1, &vbo);
//	}
//	//void drawInit(const BatchData &data) {
//	//	if(vao != 0) glDeleteVertexArrays(1, &vao);
//	//	if(vbo != 0) glDeleteBuffers(1, &vbo);
//
//	//	glGenVertexArrays(1, &vao);
//	//	glGenBuffers(1, &vbo);
//	//	glBindVertexArray(vao);
//	//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//
//	//	glBufferData(GL_ARRAY_BUFFER, sizeof(PointData) * data.data.size(), data, GL_STATIC_DRAW);
//	//	glEnableVertexAttribArray(0);
//	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PointData), 0);
//	//	glEnableVertexAttribArray(1);
//	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PointData), (void *)(sizeof(Point)));
//	//	glBindVertexArray(0);
//	//}
//	//void drawLoop(const BatchData &data) {
//	//	glClear(GL_COLOR_BUFFER_BIT);
//	//	glUseProgram(program);
//	//	glBindVertexArray(vao);
//	//	if(data.pointSize != 0) {
//	//		glDrawArrays(GL_POINTS, 0, data.pointSize);
//	//	}
//	//	if(data.lineSize != 0) {
//	//		glDrawArrays(GL_LINES, data.pointSize, data.lineSize * 2);
//	//	}
//	//	if(data.triangleSize != 0) {
//	//		glDrawArrays(GL_TRIANGLES, data.pointSize + 2 * data.lineSize, data.triangleSize * 3);
//	//	}
//	//	glBindVertexArray(0);
//	//}
//protected:
//	GLuint vao = 0, vbo = 0;
//	GLuint program;
//};