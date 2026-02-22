#include <iostream>
#include <cmath>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static float vertices[] = {
	0.f,	0.5f, 0.f,
	-0.5f,	-0.5f,	0.f,
	0.5f,	-0.5f,	0.f
};

static const char *vertex_shader_source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
"}\0";

static const char *frag_shader_source = "#version 330 code\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0, 0.5, 0.3, 1.0);\n"
"}\0";

void err_callback(int error, const char *desc) {
	std::cerr << "Error: " << desc << "\n";
} 

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (GLFW_PRESS == action) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
			case GLFW_KEY_TAB:
				glfwSetWindowShouldClose(window, true);
				break;
			default:
				break;
		}
	}
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void rotateTriangle2D(float triangle[], float rads) {
	float s = std::sinf(rads);
	float c = std::cosf(rads);
	float x = triangle[0] * c - triangle[1] * s;
	float y = triangle[0] * s + triangle[1] * c;
	triangle[0] = x;
	triangle[1] = y;
	x = triangle[3] * c - triangle[4] * s;
	y = triangle[3] * s + triangle[4] * c;
	triangle[3] = x;
	triangle[4] = y;
	x = triangle[6] * c - triangle[7] * s;
	y = triangle[6] * s + triangle[7] * c;
	triangle[6] = x;
	triangle[7] = y;
}

int main() {
	int width, height;
	float oldTime, newTime, deltaTime;
	GLFWwindow *window;
	GLuint vertex_buffer, vertex_shader, frag_shader, shader_program, vertex_array;
	if (!glfwInit()) {
		std::cerr << "Could not initialize glfw\n";
		return -1;
	}
	glfwSetErrorCallback(err_callback);
	window = glfwCreateWindow(600, 400, "window", NULL, NULL);
	if (!window) {
		std::cerr << "Could not create window\n";
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Could not initialize glad\n";
		return -1;
	}
	glfwSwapInterval(1);
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glGenBuffers(1, &vertex_buffer);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
	glCompileShader(frag_shader);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	glGenVertexArrays(1, &vertex_array);
	
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	newTime = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader_program);
		glBindVertexArray(vertex_array);
		oldTime = newTime;
		newTime = (float)glfwGetTime();
		deltaTime = newTime - oldTime;
		rotateTriangle2D(vertices, 2.f * static_cast<float>(M_PI) * deltaTime / 2.f);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
