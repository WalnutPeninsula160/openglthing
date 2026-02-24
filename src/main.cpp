#include <iostream>
#include <cmath>
#include <filesystem>
#include <cstdio> // prolly should lean to use the c++ file reading, but im too lazy rn
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// first 9 elements are for positions, last 9 are for colors
static float triangle_data[] = {
	0.f,	0.5f,	0.f,
	-0.5f,	-0.5f,	0.f,
	0.5f,	-0.5f,	0.f,
	1.f,	0.f,	0.f,
	0.f,	1.f,	0.f,
	0.f,	0.f,	1.f
};

char *vertex_shader_source;

char *frag_shader_source;

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

int main() {
	int width, height;
	float oldTime, newTime, deltaTime;
	int shader_compile_success;
	char shader_compile_logInfo[512];
	GLFWwindow *window;
	GLuint vertex_buffer, vertex_shader, frag_shader, shader_program, vertex_array;
	GLint vec_position_location, vec_color_location, scalar_time_location;
	FILE *vertex_shader_file, *fragment_shader_file;
	long file_size; // can just reuse this for both files
	size_t bytes_read;
	if (!glfwInit()) {
		std::cerr << "Could not initialize glfw\n";
		return -1;
	}
	glfwSetErrorCallback(err_callback);
	window = glfwCreateWindow(600, 600, "window", NULL, NULL);
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
	
	// get shader text from source files
	vertex_shader_file = std::fopen("./assets/shaders/shader.vert", "rb");
	if (!vertex_shader_file) {
		std::cerr << "Could not open file: ./assets/shaders/shader.vert\n";
		return -1;
	}
	std::fseek(vertex_shader_file, 0, SEEK_END);
	file_size = std::ftell(vertex_shader_file);
	if (-1 == file_size) {
		std::cerr << "Could not get size of file: ./assets/shaders/shader.vert\n";
		std::fclose(vertex_shader_file);
		return -1;
	}
	vertex_shader_source = new char[(file_size + 1) * sizeof(char)]; // +1 for null terminator
	std::rewind(vertex_shader_file);
	bytes_read = std::fread(vertex_shader_source, sizeof(char), file_size, vertex_shader_file);
	std::fclose(vertex_shader_file);
	if (file_size != bytes_read) {
		std::cerr << "Could not read file: ./assets/shaders/shader.vert\n";
		return -1;
	}
	vertex_shader_source[file_size] = '\0';
	fragment_shader_file = std::fopen("./assets/shaders/shader.frag", "rb");
	if (!fragment_shader_file) {
		std::cerr << "Could not open file: ./assets/shaders/shader.frag\n";
		return -1;
	}
	std::fseek(fragment_shader_file, 0, SEEK_END);
	file_size = std::ftell(fragment_shader_file);
	if (-1 == file_size) {
		std::cerr << "Could not get size of file: ./assets/shaders/shader.frag\n";
		std::fclose(fragment_shader_file);
		return -1;
	}
	frag_shader_source = new char[(file_size + 1) * sizeof(char)]; // +1 for null terminator
	std::rewind(fragment_shader_file);
	bytes_read = std::fread(frag_shader_source, sizeof(char), file_size, fragment_shader_file);
	std::fclose(fragment_shader_file);
	if (file_size != bytes_read) {
		std::cerr << "Could not read file: ./assets/shaders/shader.frag\n";
		return -1;
	}
	frag_shader_source[file_size] = '\0';
	
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_compile_success);
	if (!shader_compile_success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, shader_compile_logInfo);
		std::cerr << "Vertex shader failed to compile: " << shader_compile_logInfo << "\n";
	}

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &shader_compile_success);
	if (!shader_compile_success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, shader_compile_logInfo);
		std::cerr << "Frag shader failed to compile: " << shader_compile_logInfo << "\n";
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	glGenVertexArrays(1, &vertex_array);
	
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_data), triangle_data, GL_DYNAMIC_DRAW);
	//vec_position_location = glGetAttribLocation(shader_program, "aPos");
	vec_color_location = glGetAttribLocation(shader_program, "InColor");
	scalar_time_location = glGetUniformLocation(shader_program, "time");
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glVertexAttribPointer(vec_color_location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(vec_color_location);

	newTime = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader_program);
		glUniform1f(scalar_time_location, newTime);
		glBindVertexArray(vertex_array);
		oldTime = newTime;
		newTime = (float)glfwGetTime();
		deltaTime = newTime - oldTime;
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_data), triangle_data, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}
