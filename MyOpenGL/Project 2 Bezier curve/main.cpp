#define GLFW_INCLUDE_NONE
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>
#include <filesystem>
#include <fstream>

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
		points.push_back(glm::vec3(static_cast<float>(x / 400 - 1), static_cast<float>(1 - y / 400), 0.0f));
	}
}

void bezier(std::function<void(float, float)> setPixel) {
	int n = points.size();
	if(n == 0) return;
	if(n == 1) {
		setPixel(points[0].x, points[0].y);
		return;
	}
	constexpr int pixels = 10000;
	constexpr float inc_per_pixel = 1.0f / static_cast<float>(pixels);
	for(float t = 0.0f; t <= 0.9999f; t += inc_per_pixel) {
		glm::vec3 pt(0.0f);
		for(int i = 0; i < n; ++i) {
			pt += static_cast<float>(binomial(n - 1, i)) * std::powf(1 - t, n - 1 - i) * std::powf(t, i) * points[i];
		}
		setPixel(pt.x, pt.y);
	}
}

GLuint initProgram() {
	static char errorBuffer[1 << 12], codeBuffer[1 << 16];
	auto compileShader = [](GLenum type, const char *src) -> GLuint {
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);
		GLint ok = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
		if(!ok) {
			char errorBuffer[1 << 12];
			glGetShaderInfoLog(shader, 1 << 12, nullptr, errorBuffer);
			std::cout << errorBuffer << std::endl;
		}
		return shader;
	};

	fs::path pa = fs::path(__FILE__).parent_path();
	std::ifstream fVertexShader(pa / "VertexShader2D.glsl");
	fVertexShader.read(codeBuffer, sizeof(codeBuffer) - 1);
	codeBuffer[fVertexShader.gcount()] = '\0';
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, codeBuffer);
	fVertexShader.close();
	std::ifstream fFragmentShader(pa / "FragmentShader2D.glsl");
	fFragmentShader.read(codeBuffer, sizeof(codeBuffer) - 1);
	codeBuffer[fFragmentShader.gcount()] = '\0';
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, codeBuffer);
	fFragmentShader.close();
	GLuint program = glCreateProgram();
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
	return program;
}

int main() {
	if(!glfwInit()) {
		std::cout << "init failed\n";
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *wnd = glfwCreateWindow(800, 800, "Bezier curve", nullptr, nullptr);
	if(!wnd) {
		std::cout << "create window failed\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(wnd);
	glbinding::initialize(glfwGetProcAddress);
	glfwSetMouseButtonCallback(wnd, mouseBtn);

	GLuint program = initProgram();
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	std::vector<glm::vec3> curve(10000);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, curve.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glUseProgram(program);

	while(!glfwWindowShouldClose(wnd)) {
		glClear(GL_COLOR_BUFFER_BIT);
		int i = 0;
		auto setPixel = [&](float x, float y) {
			curve[i++] = glm::vec3(x, y, 0.0f);
		};

		bezier(setPixel);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, curve.size() * sizeof(glm::vec3), curve.data());
		glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(i));
		
		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	glfwDestroyWindow(wnd);
	glfwTerminate();
	return 0;
}