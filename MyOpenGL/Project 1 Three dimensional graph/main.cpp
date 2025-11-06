#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <Windows.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>
#include "ShapeDrawer3D.h"

using namespace gl;
namespace fs = std::filesystem;

glm::vec3 center;
float scale = 1.0f;
float angle = 0.0f;

void keybdEventCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
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
	GLFWwindow *wnd = glfwCreateWindow(800, 800, "Moving cube", nullptr, nullptr);
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

	GLfloat vertices[][3] = {
		{1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f}
	};
	int face_indicies[][3] = {
		{0, 1, 2}, {2, 3, 0},
		{4, 5, 6}, {6, 7, 4},
		{0, 4, 7}, {7, 3, 0},
		{1, 5, 6}, {6, 2, 1},
		{0, 1, 5}, {5, 4, 0},
		{3, 2, 6}, {6, 7, 3}
	};
	int edge_indicies[][4] = {
		{0, 1, 2, 3}, {4, 5, 6, 7},
		{0, 4, 7, 3}, {1, 5, 6, 2},
		{0, 1, 5, 4}, {3, 2, 6, 7}
	};
	BatchData3D data;
	for(int i = 0; i < std::ssize(face_indicies); ++i) {
		data.addTriangle(Triangle3D(
			Point3D(vertices[face_indicies[i][0]]),
			Point3D(vertices[face_indicies[i][1]]),
			Point3D(vertices[face_indicies[i][2]])
		), glm::vec4(0.4f, 0.8f, 1.0f, 1.0f));
	}
	for(int i = 0; i < std::ssize(edge_indicies); ++i) {
		for(int j = 0; j < 4; ++j) {
			data.addLine(Line3D(
				Point3D(vertices[edge_indicies[i][j]]),
				Point3D(vertices[edge_indicies[i][(j + 1) % 4]])
			), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}
	ShapeDrawer3D drawer;
	drawer.drawInit(data);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 proj = glm::perspective(
		glm::radians(45.0f),
		800.0f / 800.0f,
		0.1f,
		100.0f);

	while(!glfwWindowShouldClose(wnd)) {
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(center));
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scale));
		glm::mat4 view = glm::lookAt(
			glm::vec3(3.0f, 4.0f, 5.0f),
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