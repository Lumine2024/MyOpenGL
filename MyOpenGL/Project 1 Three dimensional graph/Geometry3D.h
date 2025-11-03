#pragma once

#include "glm/glm.hpp"
#include <glbinding/gl/gl.h>
#include <vector>
#include <array>

using namespace gl;

struct Point3D : public glm::vec4 {
	constexpr Point3D() : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) {}
	constexpr Point3D(float _x, float _y, float _z) : glm::vec4(_x, _y, _z, 1.0f) {}
	constexpr explicit Point3D(const value_type (&point)[3]) : glm::vec4(point[0], point[1], point[2], 1.0f) {}
	constexpr explicit Point3D(const value_type (&point)[4]) : glm::vec4(point[0], point[1], point[2], point[3]) {}
	constexpr explicit Point3D(const std::array<value_type, 3> &point) : glm::vec4(point[0], point[1], point[2], 1.0f) {}
	constexpr explicit Point3D(const std::array<value_type, 4> &point) : glm::vec4(point[0], point[1], point[2], point[3]) {}
	using glm::vec4::vec4;
	static constexpr auto type = GL_POINTS;
};
using Vector3D = Point3D;

struct Line3D {
	Point3D a, b;
	Line3D() = default;
	constexpr Line3D(const Point3D &_a, const Point3D &_b) : a(_a), b(_b) {}
	constexpr explicit Line3D(const Point3D (&points)[2]) : a(points[0]), b(points[1]) {}
	constexpr explicit Line3D(const std::array<Point3D, 2> &points) : a(points[0]), b(points[1]) {}
	explicit Line3D(const std::vector<Point3D> &points) : a(points.at(0)), b(points.at(1)) {}
	static constexpr auto type = GL_LINES;
};

struct Triangle3D {
	Point3D vertices[3];
	Triangle3D() {}
	constexpr Triangle3D(const Point3D &a, const Point3D &b, const Point3D &c) : vertices{ a, b, c } {}
	constexpr explicit Triangle3D(const Point3D (&points)[3]) : vertices{ points[0], points[1], points[2] } {}
	constexpr explicit Triangle3D(const std::array<Point3D, 3> &points) : vertices{ points[0], points[1], points[2] } {}
	explicit Triangle3D(const std::vector<Point3D> &points) : vertices{ points.at(0), points.at(1), points.at(2) } {}
	static constexpr auto type = GL_TRIANGLES;
};
