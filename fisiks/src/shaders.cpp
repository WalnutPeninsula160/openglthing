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

Program::Program(GLenum buffers, size_t num_buffers) {
	shader_program = glCreateProgram();
	glGenVertexArrays(1, &VAO);
	std::vector<GLuint> buffers
}

Program::~Program() {}

void Program::compile_shader(const char *path, GLenum type) {
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

void Program::buffer_data(GLenum buffer, GLsizeiptr siz, const void *data, GLenum usage) {
	if (!buffers.contains(buffer_type)) {
		buffers.insert({buffer_type, {0,0}});
		glGenBuffers(&buffers[buffer_type].id);
	}
	glBindBuffer(buffer_type, buffers[buffer_type].id);
	glBufferData(buffer_type, siz, data, usage);
	glBindBuffer(buffer_type, 0);
	buffers[buffer_type].offset = siz;
}

void Program::configure_VA_attribs(GLuint locations[], GLenum types[], GLint sizes[], GLboolean normalized[], GLsizei strides[], const void *pointers[], size_t num_attribs) {
	vertex_size = 0;
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	for (size_t i {}; i < num_attribs; i++) {
		vertex_size += sizes[i];
		glVertexAttribPointer(locations[i], types[i], sizes[i], normalized[i], strides[i], pointers[i]);
		glEnableVertexAttribArray(locations[i]);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(VAO);
}

void Program::draw_object(Object *obj, GLenum draw_mode, ) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[6]);
	if (obj->indices) {
		if (obj->instances > 1)
			glDrawElementsInstanced();
		else 
			glDrawElements();
	} else {
		if (obj->instances > 1)
			glDrawArraysInstanced();
		else
			glDrawArrays();
	}
	glBindVertexArray(0);
}
