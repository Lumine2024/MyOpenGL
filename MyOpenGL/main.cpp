#define GLFW_INCLUDE_NONE
#define TINYOBJLOADER_IMPLEMENTATION
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>
#include <filesystem>
#include "tiny_obj_loader.h"

using namespace gl;
namespace fs = std::filesystem;

inline double binomial(int n, int m) {
    if(n < 0 || m < 0 || m > n) return 0.0;
    static std::vector<long double> fac = [] {
        std::vector<long double> v(1);
        v[0] = 1.0L;
        return v;
    }();
    if(fac.size() <= n) {
        int old = fac.size();
        fac.resize(n + 1);
        for(int i = old; i <= n; ++i) {
            fac[i] = fac[i - 1] * i;
        }
    }
    long double ret = fac[n] / (fac[m] * fac[n - m]);
    return static_cast<double>(ret);
}

std::vector<glm::vec3> points; // we have to set it globally

void mouseBtn(GLFWwindow *window, int btn, int action, int) {
	if(btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		points.push_back(glm::vec3(static_cast<float>(x), static_cast<float>(y), 1.0f));
	}
}

void bezier(std::function<void(int, int)> setPixel) {

}

int main() {
	if(!glfwInit()) {
		std::cout << "init failed\n";
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *wnd = glfwCreateWindow(800, 800, "Rotating rabbit", nullptr, nullptr);
	if(!wnd) {
		std::cout << "create window failed\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(wnd);
	glbinding::initialize(glfwGetProcAddress);
	glfwSetMouseButtonCallback(wnd, mouseBtn);

	while(!glfwWindowShouldClose(wnd)) {
		
		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	glfwDestroyWindow(wnd);
	glfwTerminate();
	return 0;
}