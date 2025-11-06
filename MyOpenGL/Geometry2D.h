#pragma once
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
using namespace gl;

struct Point2D : public glm::vec3 {
	constexpr Point2D() : glm::vec3(0.0f, 0.0f, 1.0f) {}
	constexpr Point2D(float _x, float _y) : glm::vec3(_x, _y, 1.0f) {}
	constexpr explicit Point2D(const value_type (&point)[2]) : glm::vec3(point[0], point[1], 1.0f) {}
	constexpr explicit Point2D(const value_type (&point)[3]) : glm::vec3(point[0], point[1], point[2]) {}
	constexpr explicit Point2D(const std::array<value_type, 2> &point) : glm::vec3(point[0], point[1], 1.0f) {}
	constexpr explicit Point2D(const std::array<value_type, 3> &point) : glm::vec3(point[0], point[1], point[2]) {}
	explicit Point2D(const std::vector<value_type> &point) : glm::vec3(point[0], point[1], (point.size() <= 2 ? 1.0f : point[2])) {}
	constexpr explicit Point2D(const glm::vec2 &point) : glm::vec3(point, 1.0f) {}
	using glm::vec3::vec3;
	static constexpr auto type = GL_POINTS;
};
using Vector2D = Point2D;


struct Line2D {
	Point2D a, b;
	Line2D() = default;
	constexpr Line2D(const Point2D &_a, const Point2D &_b) : a(_a), b(_b) {}
	constexpr explicit Line2D(const Point2D (&points)[2]) : a(points[0]), b(points[1]) {}
	constexpr explicit Line2D(const std::array<Point2D, 2> &points) : a(points[0]), b(points[1]) {}
	explicit Line2D(const std::vector<Point2D> &points) : a(points.at(0)), b(points.at(1)) {}
	static constexpr auto type = GL_LINES;
};

struct Circle2D {
	Point2D center;
	GLfloat radius;
	Circle2D() = default;
	constexpr Circle2D(const Point2D &_center, GLfloat _radius) : center(_center), radius(_radius) {}
};

struct Triangle2D {
	Point2D vertices[3];
	Triangle2D() {}
	constexpr Triangle2D(const Point2D &a, const Point2D &b, const Point2D &c) : vertices{ a, b, c } {}
	constexpr explicit Triangle2D(const Point2D (&points)[3]) : vertices{ points[0], points[1], points[2] } {}
	constexpr explicit Triangle2D(const std::array<Point2D, 3> &points) : vertices{ points[0], points[1], points[2] } {}
	explicit Triangle2D(const std::vector<Point2D> &points) : vertices{ points.at(0), points.at(1), points.at(2) } {}
	static constexpr auto type = GL_TRIANGLES;
};

struct Polygon2D {
	std::vector<Point2D> pts;
	Polygon2D() {}
	explicit Polygon2D(const std::vector<Point2D> &_pts) : pts(_pts) {}
	explicit Polygon2D(std::vector<Point2D> &&_pts) : pts(std::move(_pts)) {}
};
