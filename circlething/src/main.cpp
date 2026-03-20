#include <iostream>
#include <cstdlib>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "objects.hpp"

char *read_file(const char *path) { 
	FILE *file = std::fopen(path, "rb");
	char *result;
	long file_size;
	size_t bytes_read;
	if (!file) {
		std::cerr << "Could not open file: " << path << '\n';
		return NULL;
	}
	std::fseek(file, 0, SEEK_END);
	file_size = std::ftell(file);
	if (-1 == file_size) {
		std::cerr << "Could not get size of file: " << path << "t\n";
		std::fclose(file);
		return NULL;
}
	result = new char[(file_size + 1) * sizeof(char)];
	std::rewind(file);
	bytes_read = std::fread(result, sizeof(char), file_size, file);
	std::fclose(file);
	if (file_size != bytes_read) {
		std::cerr << "Could not read file: " << path << "\n";
		return NULL;
	}
	result[file_size] = '\0';
	return result;
}

#define SEGMENTS 100
circleObject<100> circle_data = circle_vert_data(0, 0, 1, 100);
circleIndices<100> circle_indices = circle_index_data(100);

float circle_color[] = {
	1.f,	0.5f,	0.2f,	1.f
};

void err_callback(int err, const char *desc) {
	std::cerr << "Error: " << desc << '\n';
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_TAB:
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			default:
				break;
		}
	}
}

static void framebuffer_siz_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	std::cout << '\n';
	int width, height;
	const char vert_shader_path[] = "./shaders/shader.vert";
	const char frag_shader_path[] = "./shaders/shader.frag";
	const char tex_image_path[] = "./textures/Uzumaki-Junji-Ito.jpg";
	unsigned char *tex_data;
	char *vert_shader_source, *frag_shader_source;
	GLuint vert_shader, frag_shader, shader_program, vertex_buffer, vertex_array, element_buffer, texture; // 
	GLint vec2_vertPosition, vec4_Color; // buffer objects for shaders
	int success;
	char info[512];
	int tex_width, tex_height, tex_nrChannels;
	glfwSetErrorCallback(err_callback);
	if (!glfwInit()) {
		std::cerr << "Could not initialize glfw\n";
		glfwTerminate();
		return -1;
	}
	GLFWwindow *window = glfwCreateWindow(600, 600, "circle", NULL, NULL);
	if (!window) {
		std::cerr << "Could not create window\n";
		goto main_exit_err;
	}
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_siz_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Coult not initialize glad\n";
		goto main_exit_err;
	}
	glfwSwapInterval(1);
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// compile shaders
	vert_shader_source = read_file(vert_shader_path);
	if (!vert_shader_source)
		goto main_exit_err;
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_shader_source, NULL);
	glCompileShader(vert_shader);
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vert_shader, 512, NULL, info);
		std::cerr << "Could not compile vertex shader\n" << info << '\n';
	}
	
	frag_shader_source = read_file(frag_shader_path);
	if (!frag_shader_source)
		goto main_exit_err;
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, info);
		std::cerr << "Could not compile fragment shader\n" << info << '\n';
	}

	// create shader program
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	// configure vertex attributes (everything will be passed to the vertex shader, and then the vertex shader will pass needed values to the fragment shader)
	// create vertex buffer object and vertex array object (and element buffer object)
	vec2_vertPosition = glGetAttribLocation(shader_program, "vertPosition");
	vec4_Color = glGetUniformLocation(shader_program, "Color");
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &element_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(circle_data), circle_data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_indices), circle_indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(vec2_vertPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(vec2_vertPosition);
	glUniform4fv(vec4_Color, 1, circle_color);

	//main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(shader_program);
		glBindVertexArray(vertex_array);
		// load stuff into the buffer (buffer data then uniform)
		glUniform4fv(vec4_Color, 1, circle_color);
		// draw things (glDrawElements uses the indices from the bound element buffer object, in this case element_buffer)
		glDrawElements(GL_TRIANGLES, 303, GL_UNSIGNED_INT, 0);
		// second parameter - the number of indices specified, since opengl uses triangles, 2 triangles are needed to draw a square resulting in 6 vertices drawn
		// fourth parameter - the offset of the indices
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
main_exit_err:
	glfwTerminate();
	return -1;
}
