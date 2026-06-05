#include <iostream>
#include <cstdlib>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// glm include
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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

class mass_point {
	private:
	//
	public:
	mass_point(std::vector<float, 2> pos, std::vector<float, 2> vel, float m, float siz, bool f) {
		position = vec2(pos.at(0), pos.at(1));
		velocity = vec2(vel.at(0), vel.at(1));
		mass = m;
		size = siz;
		fixed = f;
		vertx_buffer = 0;
		vertex_data = new float[6] {pos.at(0), pos.at(1)};

mass_point fixed_point = {
	.position = {0.f, 0.f}, 
	.velocity = {0.f, 0.f}, 
	.mass = 10.f, 
	.size = 1.f, 
	.vertex_buffer = 0, 
	.fixed = true
};
mass_point moving_point = {
	.position = {0.f, 5.f},
	.velocity = {0.f, 0.f}, 
	.mass = 5.f, 
	.size = 1.f, 
	.vertex_buffer = 0, 
	.fixed = false
};

int mass_point_indices[3] = {0, 1, 1};

float spring_stiffness = 2.f;
float spring_relaxed_length = 2.5f;

float oldtime;
float newtime;
float deltatime;

int width = 600;
int height = 400;

glm::mat4 projection = glm::ortho(0.f, (float)width, 0.f, (float)height, 0.1f, 100.f);
glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(0.1f, 0.1f, 0.1f));

void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	projection = glm::ortho(0.f, (float)width, 0.f, (float)height, 0.1f, 100.f);
}

int main() {
	const char vertex_shader_path[] = "shaders/shader.vert";
	const char fragment_shader_path[] = "shaders/shader.frag";
	char *vertex_shader_source, *fragment_shader_source;
	int shader_compile_success;
	char shader_compile_log[1024];
	GLuint shader_program, vertex_shader, fragment_shader, vertex_array, element_buffer;
	GLint vec2_vertPosition, float_size, mat4_projection, mat4_model;
	// init glfw window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow(width, height, "Hooke's Law Simulation", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Could not initialize GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Could not initialize GLAD\n";
		glfwTerminate();
		return -1;
	}
	// init shaders and shader program
	vertex_shader_source = read_file(vertex_shader_path);
	fragment_shader_source = read_file(fragment_shader_path);
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_compile_success);
	if (!shader_compile_success) {
		glGetShaderInfoLog(vertex_shader, 1024, NULL, shader_compile_log);
		std::cerr << "Could not compile vertex shader\nError Message:\n" << shader_compile_log;
		glfwTerminate();
		return -1;
	}
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_compile_success);
	if (!shader_compile_success) {
		glGetShaderInfoLog(fragment_shader, 1024, NULL, shader_compile_log);
		std::cerr << "Could not compile fragment shader\nError Message:\n" << shader_compile_log;
		glfwTerminate();
		return -1;
	}
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	// set up uniforms, vertex arrays, and vertex buffers
	vec2_vertPosition = glGetAttribLocation(shader_program, "vertPosition");
	float_size = glGetUniformLocation(shader_program, "size");
	mat4_projection = glGetUniformLocation(shader_program, "projection");
	mat4_model = glGetUniformLocation(shader_program, "model");
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &fixed_point.vertex_buffer);
	glGenBuffers(1, &moving_point.vertex_buffer);
	glGenBuffers(1, &element_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, fixed_point.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) << 1, fixed_point.position, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, moving_point.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) << 1, moving_point.position, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 3, mass_point_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(vec2_vertPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(vec2_vertPosition);
	// enable the thing i want to enable (it wont be disabled, so just doing it before the main loop is fine)
	glEnable(GL_PROGRAM_POINT_SIZE);
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		// render fixed point
		glUseProgram(shader_program);
		glUniformMatrix4fv(mat4_projection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(float_size, fixed_point.size);
		glBindVertexArray(vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, fixed_point.vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		// render moving point
		glUseProgram(shader_program);
		glUniformMatrix4fv(mat4_projection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(float_size, fixed_point.size);
		glBindVertexArray(vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, moving_point.vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		// stuff
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
