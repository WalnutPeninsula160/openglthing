#include <iostream>
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
}

int main(int argc, char **argv) {
	GLFWwindow *window;
	if (!glfwInit()) {
		return -1;
	}
	width = 600;
	height = 400;
	window = glfwCreateWindow(width, height, "Window", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwGetFramebufferSize(window, &width, &height);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// enable vsync
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		// render
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
