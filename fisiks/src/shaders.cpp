#include "shaders.hpp"

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
	result = new char[file_size + 1];
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

program::program() {
	glCreateProgram(shader_program);
	glGenVertexArrays(1, &VAO);
	buffers = new GLint[14]{};
	buffer_offsets = new size_t[14]{};
	glGenBuffers
};

void program::compile_shader(const char *path, GLenum type) {
	GLint shader_object;
	char *source;
	int success {};
	char info[1024];
	source = read_file(path);
	if (!source) {
		std::cerr << "Could not read file: " << path << '\n';
		return;
	}
	shader_object = glCreateShader(type);
	glShaderSource(shader_object, 1, &source, NULL);
	glCompileShader(shader_object);
	glGetShaderiv(shader_object, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader_object, 1024, NULL, info);
		std::cerr << "Could not copmile shader at " << path << "\nError: \n" << info << '\n';
	}
	glAttachShader(shader_program, shader_object);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 1024, NULL, info);
		std::cerr << "Could not link shader program\nError:\n" << info << '\n';
	}
	glDeleteShader(shader_object);
}

void program::buffer_data_f(GLenum buffer, GLsizeiptr siz, const void *data, GLenum usage, size_t buffer_index) {
	glBindBuffer(buffer, buffers[buffer_index]);
	glBufferData(buffer, siz, data, usage);
	glBindBuffer(buffer, 0);
	buffer_offsets[buffer_index] = siz;
}

void program::buffer_append_data_f(GLenum buffer, GLsizeiptr siz, const void *data, size_t buffer_index) {
	glBindBuffer(buffer, buffers[buffer_index]);
	glBufferSubData(buffer, buffer_offsets[buffer_index], siz, data);
	glBindBuffer(buffer, 0);
	buffer_offsets[buffer_index] += siz;
}

void program::configure_VA_attribs(GLuint locations[], GLenum types[], GLint sizes[], GLboolean normalized[], GLsizei strides[], const void *pointers[], size_t num_attribs) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	for (size_t i {}; i < num_attribs; i++) {
		glVertexAttribPointer(locations[i], types[i], sizes[i], normalized[i], strides[i], pointers[i]);
		glEnableVertexAttribPointer(locations[i]);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(VAO);
}
