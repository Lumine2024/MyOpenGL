#pragma once
#include "Geometry3D.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glbinding/gl/gl.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <utility>
#include <filesystem>

using namespace gl;
namespace fs = std::filesystem;

struct PointData3D {
	Point3D point, norm;
	glm::vec4 color;
};

struct BatchData3D {
	std::vector<PointData3D> data;
	std::vector<GLenum> types;
	BatchData3D() = default;
	BatchData3D &addPoint(const Point3D &point, const Point3D &norm, const glm::vec4 &color) {
		data.push_back({point, norm, color});
		types.push_back(GL_POINTS);
		return *this;
	}
	BatchData3D &addLine(const Line3D &line, const Point3D &norm, const glm::vec4 &color) {
		data.push_back({line.a, norm, color});
		data.push_back({line.b, norm, color});
		types.push_back(GL_LINES);
		return *this;
	}
	BatchData3D &addTriangle(const Triangle3D &triangle, const Point3D &norm, const glm::vec4 &color) {
		for(int i = 0; i < 3; ++i) {
			data.push_back({triangle.vertices[i], norm, color});
		}
		types.push_back(GL_TRIANGLES);
		return *this;
	}
	template<class T> BatchData3D &addGraph(const T &graph, const Point3D &norm, const glm::vec4 &color) {
		if constexpr(T::type == GL_POINTS) {
			return addPoint(graph, norm, color);
		} else if constexpr(T::type == GL_LINES) {
			return addLine(graph, norm, color);
		} else if constexpr(T::type == GL_TRIANGLES) {
			return addTriangle(graph, norm, color);
		} else {
			static_assert(false, "Unknown graph type");
		}
	}
};

constexpr GLsizei vertexSize(GLenum type) {
	if(type == GL_POINTS) return 1;
	if(type == GL_LINES) return 2;
	if(type == GL_TRIANGLES) return 3;
	throw std::invalid_argument("Unknown type");
}

class ShapeDrawer3D {
public:
	ShapeDrawer3D() {
		static char errorBuffer[1 << 12], srcCodeBuffer[1 << 16];
		auto compileShader = [](GLenum type, const char *src) -> GLuint {
			GLuint shader = glCreateShader(type);
			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);

			GLint ok = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
			if(!ok) {
				glGetShaderInfoLog(shader, 1 << 12, nullptr, errorBuffer);
				std::cout << errorBuffer << std::endl;
			}

			return shader;
		};

		fs::path pa = fs::path(__FILE__).parent_path();
		std::ifstream fVertexShader(pa / "VertexShader3D.glsl");
		fVertexShader.read(srcCodeBuffer, sizeof(srcCodeBuffer) - 1);
		srcCodeBuffer[fVertexShader.gcount()] = '\0';
		GLuint vertexShader = compileShader(GL_VERTEX_SHADER, srcCodeBuffer);
		fVertexShader.close();
		std::ifstream fFragmentShader(pa / "FragmentShader3D.glsl");
		fFragmentShader.read(srcCodeBuffer, sizeof(srcCodeBuffer) - 1);
		srcCodeBuffer[fFragmentShader.gcount()] = '\0';
		GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, srcCodeBuffer);
		fFragmentShader.close();

		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		GLint ok = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &ok);
		if(!ok) {
			glGetProgramInfoLog(program, 1 << 12, nullptr, errorBuffer);
			std::cout << errorBuffer << std::endl;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	~ShapeDrawer3D() {
		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	void drawInit(const BatchData3D &data) {
		if(vao != 0) glDeleteVertexArrays(1, &vao);
		if(vbo != 0) glDeleteBuffers(1, &vbo);

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.data.size() * sizeof(PointData3D), data.data.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PointData3D), (void *)offsetof(PointData3D, point));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PointData3D), (void *)offsetof(PointData3D, norm));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(PointData3D), (GLvoid *)offsetof(PointData3D, color));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(program);

		glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LEQUAL);
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);
	}

	void setMVP(const glm::mat4 &mvp) {
		GLint loc = glGetUniformLocation(program, "mvp");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));
	}
	void setLight(const glm::vec4 &light) {
		GLint loc = glGetUniformLocation(program, "light");
		glUniform4fv(loc, 1, glm::value_ptr(light));
	}

	void drawLoop(const BatchData3D &data) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(vao);

		GLsizei offset = 0;
		for(auto type : data.types) {
			GLsizei count = vertexSize(type);
			glDrawArrays(type, offset, count);
			offset += count;
		}

		glBindVertexArray(0);
	}
private:
	GLuint program = 0;
	GLuint vao = 0, vbo = 0;
};

