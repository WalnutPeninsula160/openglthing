// include standard c++ llibraries
#include <iostream>
#include <cstdlib>
// include GLFW and OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// include stbi
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// include GLM
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

// vec3 verts, vec2 texCoords
static float square_data[] = {
	0.5f,	0.5f,	0.f,	1.f,	1.f,
	0.5f,	-0.5f,	0.f,	1.f,	0.f,
	-0.5f,	-0.5f,	0.f,	0.f,	0.f,
	-0.5f,	0.5f,	0.f,	0.f,	1.f
};

static float square_color[] = {
	1.f,	0.5f,	0.2f
};

static unsigned int square_indices[] = {
	0,	1,	3,
	1,	2,	3
};

// coordinate system matrices
// FOV, aspect ratio, near plane, far plane
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -1.f));
glm::mat4 projection = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 100.f);

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
	switch (key) {
		case GLFW_KEY_LEFT:
			model = glm::rotate(model, glm::radians(1.f), glm::vec3(0.f, 1.f, 0.f));
			break;
		case GLFW_KEY_RIGHT:
			model = glm::rotate(model, glm::radians(-1.f), glm::vec3(0.1, 1.f, 0.f));
		default:
			break;
	}
	switch (key) {
		case GLFW_KEY_LEFT:
			model = glm::rotate(model, glm::radians(1.f), glm::vec3(0.f, 1.f, 0.f));
			break;
		case GLFW_KEY_RIGHT:
			model = glm::rotate(model, glm::radians(-1.f), glm::vec3(0.1, 1.f, 0.f));
		default:
			break;
	}
}

static void framebuffer_siz_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
	projection = glm::perspective(glm::radians(45.f), (float)width/(float)height, 0.1f, 100.f);
}

int main() {
	int width, height;
	const char vert_shader_path[] = "./shaders/shader.vert";
	const char frag_shader_path[] = "./shaders/shader.frag";
	const char tex0_image_path[] = "./textures/Uzumaki-Junji-Ito.jpg";
	const char tex1_image_path[] = "./textures/UZUMAKI_screens_1200x630_8.jpg";
	unsigned char *tex0_data, *tex1_data;
	char *vert_shader_source, *frag_shader_source;
	GLuint vert_shader, frag_shader, shader_program, vertex_buffer, vertex_array, element_buffer, texture1, texture2; // 
	GLint vec3_vertPosition, vec3_Color, vec2_TexCoords, texture1_location, texture2_location, mat4_model, mat4_view, mat4_project; // buffer objects for shaders
	int success;
	char info[512];
	int tex0_width, tex0_height, tex0_nrChannels, tex1_width, tex1_height, tex1_nrChannels;
	glfwSetErrorCallback(err_callback);
	if (!glfwInit()) {
		std::cerr << "Could not initialize glfw\n";
		glfwTerminate();
		return -1;
	}
	GLFWwindow *window = glfwCreateWindow(600, 600, "square", NULL, NULL);
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
	vec3_vertPosition = glGetAttribLocation(shader_program, "vertPosition");
	vec3_Color = glGetUniformLocation(shader_program, "Color");
	vec2_TexCoords = glGetAttribLocation(shader_program, "TexCoords");
	mat4_model = glGetUniformLocation(shader_program, "model");
	mat4_view = glGetUniformLocation(shader_program, "view");
	mat4_project = glGetUniformLocation(shader_program, "project");
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &element_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_data), square_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(vec3_vertPosition, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(vec2_TexCoords, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(vec3_vertPosition);
	glEnableVertexAttribArray(vec2_TexCoords);
	glUniform3fv(vec3_Color, 1, square_color);

	// texture stuff
	stbi_set_flip_vertically_on_load(true);
	tex0_data = stbi_load(tex0_image_path, &tex0_width, &tex0_height, &tex0_nrChannels, 0);
	if (!tex0_data) {
		std::cerr << "Could not load texture 1\n";
		goto main_exit_err;
	}
	tex1_data = stbi_load(tex1_image_path, &tex1_width, &tex1_height, &tex1_nrChannels, 0);
	if (!tex1_data) {
		std::cerr << "Could not load texture 2\n";
		goto main_exit_err;
	}
	glGenTextures(1, &texture1);
	// first texture image
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex0_width, tex0_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex0_data);
	stbi_image_free(tex0_data);
	// second texture image	
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex1_width, tex1_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex1_data);
	stbi_image_free(tex1_data);
	// bind the texture units
	texture1_location = glGetUniformLocation(shader_program, "texture1");
	texture2_location = glGetUniformLocation(shader_program, "texture2");
	glUseProgram(shader_program);
	glUniform1i(texture1_location, 0);
	glUniform1i(texture2_location, 1);

	// apply transformations
	;;
	// pass transformation matrix to the shaders
	glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(mat4_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(mat4_project, 1, GL_FALSE, glm::value_ptr(projection));

	//main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader_program);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(vertex_array);
		// load stuff into the buffer (buffer data then uniform)
		glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(mat4_view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(mat4_project, 1, GL_FALSE, glm::value_ptr(projection));
		// draw things (glDrawElements uses the indices from the bound element buffer object, in this case element_buffer)
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
