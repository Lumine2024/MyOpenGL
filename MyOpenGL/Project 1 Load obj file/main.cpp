#define GLFW_INCLUDE_NONE
#define TINYOBJLOADER_IMPLEMENTATION
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <Windows.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>
#include <filesystem>
#include "ShapeDrawer3D.h"
#include "tiny_obj_loader.h"

using namespace gl;
namespace fs = std::filesystem;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

void load(const fs::path &path, std::vector<Vertex> &vertices, std::vector<unsigned> &indices) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;
	if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.string().c_str())) {
		std::cerr << "Failed to load/parse .obj.\n";
		return;
	}
	for(const auto &shape : shapes) {
		for(const auto &index : shape.mesh.indices) {
			Vertex vertex{};
			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			if(index.normal_index >= 0) {
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}
			if(index.texcoord_index >= 0) {
				vertex.texcoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}
			vertices.push_back(vertex);
			indices.push_back(static_cast<unsigned>(indices.size()));
		}
	}
}

std::vector<std::pair<Triangle3D, Point3D>> convert(const std::vector<Vertex> &vertices, const std::vector<unsigned> &indices) {
	std::vector<std::pair<Triangle3D, Point3D>> triangles;
	for(size_t i = 0; i + 2 < indices.size(); i += 3) {
		Point3D p0(vertices[indices[i + 0]].position);
		Point3D p1(vertices[indices[i + 1]].position);
		Point3D p2(vertices[indices[i + 2]].position);
		Triangle3D triangle(p0, p1, p2);

		glm::vec3 n(0.0f);
		const float eps = 1e-6f;
	
		auto v0n = vertices[indices[i + 0]].normal;
		auto v1n = vertices[indices[i + 1]].normal;
		auto v2n = vertices[indices[i + 2]].normal;
		int count = 0;
		if(glm::length(v0n) > eps) { n += glm::normalize(v0n); ++count; }
		if(glm::length(v1n) > eps) { n += glm::normalize(v1n); ++count; }
		if(glm::length(v2n) > eps) { n += glm::normalize(v2n); ++count; }
		if(count > 0) {
			n = glm::normalize(n / static_cast<float>(count));
		} else {
			std::cout << "didn't find valid per-vertex normals, using geometric normal\n";
			glm::vec3 pu = glm::vec3(p1) - glm::vec3(p0);
			glm::vec3 pv = glm::vec3(p2) - glm::vec3(p0);
			n = glm::normalize(glm::cross(pu, pv));
		}
		
		triangles.push_back({triangle, Point3D(n)});
	}
	return triangles;
}

glm::vec3 center;
float scale = 1.0f;
float angle = 0.0f;

void keybdEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(action == GLFW_PRESS) {
		switch(key) {
			case GLFW_KEY_W: {
				center.y -= 0.1f;
				break;
			}
			case GLFW_KEY_S: {
				center.y += 0.1f;
				break;
			}
			case GLFW_KEY_A: {
				center.x += 0.1f;
				break;
			}
			case GLFW_KEY_D: {
				center.x -= 0.1f;
				break;
			}
			case GLFW_KEY_Z: {
				scale *= 1.1f;
				break;
			}
			case GLFW_KEY_C: {
				scale /= 1.1f;
				break;
			}
			case GLFW_KEY_Q: {
				angle -= glm::radians(15.0f);
				break;
			}
			case GLFW_KEY_E: {
				angle += glm::radians(15.0f);
				break;
			}
		}
	}
}

int main() {
	if(!glfwInit()) {
		std::cout << "init failed\n";
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *wnd = glfwCreateWindow(800, 800, "Rabbit transform", nullptr, nullptr);
	if(!wnd) {
		std::cout << "create window failed\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(wnd);
	glbinding::initialize(glfwGetProcAddress);

	HWND hwnd = glfwGetWin32Window(wnd);
	MessageBoxW(hwnd, L"WASD移动方向，ZC缩放，QE旋转", L"操作提示", MB_OK);

	glfwSetKeyCallback(wnd, keybdEventCallback);

	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
	load(fs::path(__FILE__).parent_path() / "bunny_10k.obj", vertices, indices);
	auto triangles = convert(vertices, indices);

	BatchData3D data;
	for(auto &tri : triangles) {
		data.addTriangle(tri.first, tri.second, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	ShapeDrawer3D drawer;
	drawer.drawInit(data);
	drawer.setLight(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

	while(!glfwWindowShouldClose(wnd)) {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(center));
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scale));
		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, -2.0f),
			glm::vec3(0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		glm::mat4 proj = glm::perspective(
			glm::radians(45.0f),
			800.0f / 800.0f,
			0.1f,
			100.0f
		);
		glm::mat4 mvp = proj * view * model;
		drawer.setMVP(mvp);

		drawer.drawLoop(data);
		
		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	glfwDestroyWindow(wnd);
	glfwTerminate();
	return 0;
}