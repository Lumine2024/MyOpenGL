#pragma once

#include "glm/glm.hpp"
#include <glbinding/gl/gl.h>
#include <vector>
#include <array>

using namespace gl;

struct Point3D : public glm::vec4 {
	constexpr Point3D() : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) {}
	constexpr Point3D(float _x, float _y, float _z) : glm::vec4(_x, _y, _z, 1.0f) {}
	constexpr Point3D(const float (&point)[3]) : glm::vec4(point[0], point[1], point[2], 1.0f) {}
	constexpr Point3D(const float (&point)[4]) : glm::vec4(point[0], point[1], point[2], point[3]) {}
	constexpr Point3D(const std::array<value_type, 3> &point) : glm::vec4(point[0], point[1], point[2], 1.0f) {}
	constexpr Point3D(const std::array<value_type, 4> &point) : glm::vec4(point[0], point[1], point[2], point[3]) {}
	using glm::vec4::vec4;
	static constexpr auto type = GL_POINTS;
};
using Vector3D = Point3D;

struct Line3D {
	Point3D a, b;
	Line3D() = default;
	constexpr Line3D(const Point3D &_a, const Point3D &_b) : a(_a), b(_b) {}
	static constexpr auto type = GL_LINES;
};

struct Triangle3D {
	Point3D vertices[3];
	Triangle3D() {}
	constexpr Triangle3D(const Point3D &a, const Point3D &b, const Point3D &c) : vertices{ a, b, c } {}
	static constexpr auto type = GL_TRIANGLES;
};