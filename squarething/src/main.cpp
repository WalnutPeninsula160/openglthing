#include <iostream>
#include <
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

char *read_file(const char *path) { 
	FILE *file = std::fopen(path, "rb");
	char *result;
	long file_size;
	size_t bytes_read;
	if (!file) {
		std::cerr << "Could not open file: " << path << '\n';
		return -1;
	}
	std::fseek(file, 0, SEEK_END);
	file_size = std::ftell(file);
	if (-1 == file_size) {
		std::cerr << "Could not get size of file: " << path << "t\n";
		std::fclose(file);
		return -1;
	}
	result = new char[(file_size + 1) * sizeof(char)];
	std::rewind(file);
	bytes_read = std::fread(result, sizeof(char), file_size, file);
	std::fclose(file);
	if (file_size != bytes_read) {
		std::cerr << "Could not read file: " << path << "\n";
		return -1;
	}
	result[file_size] = '\0';
	return result;
}

float square_data[] = {
	//
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
	char *vert_shader_source, *frag_shader_source;
	GLuint vert_shader, frag_shader, shader_program, vertex_buffer; // 
	GLint ; // buffer objects for shaders
	glfwSetErrorCallback(err_callback);
	if (!glfwInit()) {
		std::cerr << "Could not initialize glfw\n";
		goto exit_err;
	}
	GLFWwindow *window = glfwCreateWindow(600, 400, "square", NULL, NULL);
	if (!window) {
		std::cerr << "Could not create window\n";
		goto exit_err;
	}
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_siz_callback);
	glfwMakeContextCurrent(window);
	if (!gladGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Coult not initialize glad\n";
		goto exit_err;
	}
	glfwSwapInterval(1);

	// create vertex buffe object
	glGenBuffers(1, vertex_buffer);
	//the next 2 lines must be repeated in the main loop if the position changes in CPU memory
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_data), square_data, GL_DYNAMIC_DRAW);

	// compile shaders
	vert_shader_source = read_file(vert_shader_path);
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_shader_source, NULL);
	glCompileShader(vert_shader);

	frag_shader_source = read_file(frag_shader_path);
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
	glCompileShader(frag_shader);

	// create shader program
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	// configure vertex attributes (everything will be passed to the vertex shader, and then the vertex shader will pass needed values to the fragment shader)
	vec3_vertPosition = glGetAttribLocation(shader_program, "vertPosition");
	vec3_vertColor = glGetAttribLocation(shader_program, "vertColor");
	glVertexAttribPointer(vec3_vertPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(vec3_vertPosition);
	glVertexAttribPointer(vec3_vertColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(12 * sizeof(float)));
	glEnablevertexAttribArray(vec3_vertColor);

	//main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader_program);
		//
		glfwSwapBuffers(window);
		glfwPollEvents();
	glfwTerminate();
	return 0;
exit_err:
	glfwTerminate();
	return -1;
}
