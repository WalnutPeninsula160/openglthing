#include <iostream>
//#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
//#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

int width, height;
double newTime, oldTime, deltaTime;

void err_callback(int error, const char *desc) {
	std::cerr << "Error: " << desc << "\n";
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_TAB:
				glfwSetWindowShouldClose(window, true);
				break;
			default:
				break;
		}
	}
}

void framebuffer_size_callback(GLFWwindow *window, int x, int y) {
	width = x;
	height = y;
	glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {
	GLFWwindow *window;
	// initialize GLFW (glfw functions wont work if i dont do this)
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}
	width = 600;
	height = 400;
	window = glfwCreateWindow(width, height, "Window", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// initialize glad (opengl functions wont work if i dont do this)
	// this needs to be done AFTER glfwMakeContextCurrent(window) or else it will fail
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}
	glfwGetFramebufferSize(window, &width, &height);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// enable vsync (this might be limited by the gpu drivers being locked at a certain fps)
	glfwSwapInterval(1);
	glViewport(0, 0, width, height); // idk if setting the frame buffer callback automatically does this
	while (!glfwWindowShouldClose(window)) {
		// render
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		oldTime = newTime;
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		std::cout << (int)(1 / deltaTime) << "\n";
		// swap front and back buffers
		glfwSwapBuffers(window);
		// poll events
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
