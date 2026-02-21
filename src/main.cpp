#include <iostream>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int width, height;
double newTime, oldTime, deltaTime;

// vertex shaders (magic)
const char *vertex_shader_source = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"\tgl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// fragment shaders (calculate the colors of pixels)
const char *fragment_shader_source = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"\tFragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

// shader program (multiple shaders combined)
unsigned int shader_program;

// vertex shader objects
unsigned int vertex_shader;

// fragment shader objects
unsigned int frag_shader;

// vertex buffer object
unsigned int VBO;

// vertex array object
unsigned int VAO;

// of the form x, y, z, x, y, z, x, y, z, ...
float triangle_vertices[] = {
	-0.5f,	-0.5f,	0.f,
	0.5f	-0.5f,	0.f,
	0.f,	0.5f,	0.f
};

void err_callback(int error, const char *desc) {
	std::cerr << "Error: " << desc << "\n";
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_TAB:
				glfwSetWindowShouldClose(window, true);
				break;
			case GLFW_KEY_ESCAPE:
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
	glViewport(0, 0, width, height);
}

int main(int argc, char **argv) {
	GLFWwindow *window;
	// initialize GLFW (glfw functions wont work if i dont do this)
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}
	width = 600;
	height = 400;
	window = glfwCreateWindow(width, height, "Window", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// initialize glad (opengl functions wont work if i dont do this)
	// this needs to be done AFTER glfwMakeContextCurrent(window) or else it will fail
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}
	glfwGetFramebufferSize(window, &width, &height);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// enable vsync (this might be limited by the gpu drivers being locked at a certain fps)
	glfwSwapInterval(1);
	glViewport(0, 0, width, height); // idk if setting the frame buffer callback automatically does this
	glGenBuffers(1, &VBO); // generate buffer with ID stored in VBO
	glGenVertexArrays(1, &VAO);
	// Set up the vertex shader
	vertex_shader = glCreateShader(GL_VERTEX_SHADER); // create a shader in the unsigned int vertex_shader with type GL_VERTEX_SHADER
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL); // glShaderSource(shader object, number of strings, shader source string)
	glCompileShader(vertex_shader); // actually compile the shader
	// Set up the fragment shader (very similar to vertex shader setup, but with fragment shaders instead) 
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(frag_shader);
	// link vertexshader and fragment shader into a shader program
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program); // results inn a program shader that can be activated by glUseProgram
	// since the vertex and frag shaders are no longer needed (because of the shader program beinng activated, we can delete them)
	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);
	// specify how the vertexdata is processed
	while (!glfwWindowShouldClose(window)) {
		// clear the screen
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		// render
		// bind the vertex array
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // 'bind' the buffer with ID VBO to the GL_ARRAY_BUFFER target
						    // GL_ARRAY_BUFFER target is used for vertex buffer objects
		// now any function calls using GL_ARRAY_BUFFER will configure the buffer with ID of VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
		// Copy data into the targeted buffer (in this case targeted by GL_ARRAY_BUFFER)
		// 4th parameter of glBufferData control how the GPU manages the data
		// GL_STREAM_DRAW - data is set once and used by the GPU at most a few times
		// GL_STATID_DRAW - data is set once and used by the GPU many times
		// GL_DYNAMIC_DRAW - data is changed many times and used by the GPU many times
		// We dont want the position to change at all, so we use GL_STATIC_DRAW, if we wanted the position to 
		// change, GL_DYNAMIC_DRAW would be used (GPU stores the data in memory with faster write speeds)
		// set the vertex attribute pointer
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // magic
		// first parameter - which vertex attriubte to use (specified in the shaders by 'layout (location = 0)' (attribute with location 0)
		// second parameter - size of vertex attributes (vec3 is used, so size is 3)
		// third parameter - the type used (vecn uses n number of float types)
		// fourth parameter - controls if data is normalized (important for if integer types are inputted)
		// fifth parameter - the size between consecutive vertex attributes (if 0 is inputted, opengl can determine it, but not easily)
		// sixth parameter - offset
		glEnableVertexAttribArray(0); // takes vertex attribute location as a parameter
		// activate the shader program and draw the object
		glUseProgram(shader_program);
		glBindVertexArray(VAO); // copy vertices array into buffer for opengl to use
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// first parameter - the opengl primitive type to be drawn
		// second parameter - starting index of the vertex array to draw
		// third parameter - the number of vertices to draw
		//glClear(GL_COLOR_BUFFER_BIT);
		// calculate deltaTime
		oldTime = newTime;
		newTime = glfwGetTime();
		deltaTime = newTime - oldTime;
		// swap front and back buffers
		glfwSwapBuffers(window);
		// poll events
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
