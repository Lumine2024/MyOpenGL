#pragma once
#include "Geometry2D.h"
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>
#include <fstream>
#include <iostream>
#include <filesystem>

using namespace gl;
namespace fs = std::filesystem;

struct PointData2D {
	Point2D point;
	glm::vec3 color;
};

struct BatchData2D {
	std::vector<PointData2D> data;
	std::vector<GLenum> types;
	BatchData2D() = default;
	BatchData2D &addPoint(const Point2D &point, const glm::vec3 &color) {
		data.push_back({point, color});
		types.push_back(Point2D::type);
		return *this;
	}
	BatchData2D &addLine(const Line2D &line, const glm::vec3 &color) {
		data.push_back({line.a, color});
		data.push_back({line.b, color});
		types.push_back(Line2D::type);
		return *this;
	}
	BatchData2D &addTriangle(const Triangle2D &triangle, const glm::vec3 &color) {
		for(int i = 0; i < 3; ++i) {
			data.push_back({triangle.vertices[i], color});
		}
		types.push_back(Triangle2D::type);
		return *this;
	}
	template<class T> BatchData2D &addGraph(const T &graph, const glm::vec3 &color) {
		if constexpr(T::type == GL_POINTS) {
			return addPoint(graph, color);
		} else if constexpr(T::type == GL_LINES) {
			return addLine(graph, color);
		} else if constexpr(T::type == GL_TRIANGLES) {
			return addTriangle(graph, color);
		} else {
			static_assert(false, "Unknown graph type");
		}
	}
	template<class T> BatchData2D &operator<<(const std::pair<T, glm::vec3> &p) {
		return addGraph(p.first, p.second);
	}
};

constexpr GLsizei vertexSize(GLenum type) {
	if(type == GL_POINTS) return 1;
	if(type == GL_LINES) return 2;
	if(type == GL_TRIANGLES) return 3;
	throw std::invalid_argument("Unknown type");
}

class ShapeDrawer2D {
public:
	ShapeDrawer2D() {
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
		std::ifstream fVertexShader(pa / "VertexShader2D.glsl");
		fVertexShader.read(srcCodeBuffer, sizeof(srcCodeBuffer) - 1);
		srcCodeBuffer[fVertexShader.gcount()] = '\0';
		GLuint vertexShader = compileShader(GL_VERTEX_SHADER, srcCodeBuffer);
		fVertexShader.close();
		std::ifstream fFragmentShader(pa / "FragmentShader2D.glsl");
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

	~ShapeDrawer2D() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteProgram(program);
	}

	void drawInit(const BatchData2D &data) {
		if(vao != 0) {
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
		}
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.data.size() * sizeof(PointData2D), data.data.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(PointData2D), reinterpret_cast<void *>(offsetof(PointData2D, point)));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PointData2D), reinterpret_cast<void *>(offsetof(PointData2D, color)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void drawLoop(const BatchData2D &data) {
		glUseProgram(program);
		glBindVertexArray(vao);
		GLsizei offset = 0;
		for(const auto &type : data.types) {
			GLsizei count = vertexSize(type);
			glDrawArrays(type, offset, count);
			offset += count;
		}
		glBindVertexArray(0);
		glUseProgram(0);
	}
private:
	GLuint program = 0;
	GLuint vao = 0, vbo = 0;
};