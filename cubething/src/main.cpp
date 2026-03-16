#include <iostream>
#include <cstdlib>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
	0.5f,	0.5f,	0.5f,	1.f,	1.f,
	0.5f,	-0.5f,	0.5f,	1.f,	0.f,
	-0.5f,	-0.5f,	0.5f,	0.f,	0.f,
	-0.5f,	0.5f,	0.5f,	0.f,	1.f,
	0.5f,	0.5f,	1.f,	1.f,	1.f,
	0.5f,	-0.5f,	1.f,	1.f,	0.f,
	-0.5f,	-0.5f,	1.f,	0.f,	0.f,
	-0.5f,	0.5f,	1.f,	0.f,	1.f
};

static float square_color[] = {
	1.f,	0.5f,	0.2f
};

static unsigned int square_indices[] = {
	// front
	0,	1,	3,
	1,	2,	3,
	// right
	4,	5,	0,
	5,	1,	0,
	// back
	7,	6,	4,
	6,	5,	4,
	// left
	7,	6,	3,
	6,	2,	3,
	// up
	4,	0,	7,
	0,	3,	7,
	// down
	5,	1,	6,
	1,	2,	6
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
	int width, height;
	const char vert_shader_path[] = "./shaders/shader.vert";
	const char frag_shader_path[] = "./shaders/shader.frag";
	const char tex_image_path[] = "./textures/Uzumaki-Junji-Ito.jpg";
	unsigned char *tex_data;
	char *vert_shader_source, *frag_shader_source;
	GLuint vert_shader, frag_shader, shader_program, vertex_buffer, vertex_array, element_buffer, texture; // 
	GLint vec3_vertPosition, vec3_Color, vec2_TexCoords; // buffer objects for shaders
	int success;
	char info[512];
	int tex_width, tex_height, tex_nrChannels;
	glfwSetErrorCallback(err_callback);
	if (!glfwInit()) {
		std::cerr << "Could not initialize glfw\n";
		glfwTerminate();
		return -1;
	}
	GLFWwindow *window = glfwCreateWindow(600, 400, "square", NULL, NULL);
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
	tex_data = stbi_load(tex_image_path, &tex_width, &tex_height, &tex_nrChannels, 0);
	if (!tex_data) {
		std::cerr << "Could not load texture\n";
		goto main_exit_err;
	}
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
	stbi_image_free(tex_data);

	//main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(shader_program);
		glBindVertexArray(vertex_array);
		// load stuff into the buffer (buffer data then uniform)
		glUniform3fv(vec3_Color, 1, &square_color[0]);
		// draw things (glDrawElements uses the indices from the bound element buffer object, in this case element_buffer)
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
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
