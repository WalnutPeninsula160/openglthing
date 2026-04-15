// regular c++ include
#include <iostream>
#include <cstdlib>
// glfw and opengl include
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// stbi include
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// glm include
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
// other things
#include "camera.hpp"

// macro definitions
#define CAMERA_MOVE_SPEED 0.1
#define CAMERA_ROTATE_SPEED 0.1

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

int height = 600;
int width = 600;

bool firstMouse = true;
float cursor_sensitivity = 0.1f;
float deltax = 0.f, deltay = 0.f;

CAMERA camera;

static float lastx = (float)width/2.f;
static float lasty = (float)height/2.f;

// vec3 verts, vec2 texCoords, vec3 normal
static float square_data[] = {
	// close face
	0.5f,	0.5f,	-0.5f,	0.f,	0.f,	0.f,	0.f,	-1.f,	// close top right
	0.5f,	-0.5f,	-0.5f,	1.f,	0.f,	0.f,	0.f,	-1.f,	// close bottom right
	-0.5f,	-0.5f,	-0.5f,	0.f,	0.f,	0.f,	0.f,	-1.f,	// close bottom left
	-0.5f,	0.5f,	-0.5f,	0.f,	1.f,	0.f,	0.f,	-1.f,	// close top left
	// far face
	-0.5f,	-0.5f,	0.5f,	1.f,	1.f,	0.f,	0.f,	1.f,	// far bottom left
	-0.5f,	0.5f,	0.5f,	1.f,	0.f,	0.f,	0.f,	1.f,	// far top left
	0.5f,	0.5f,	0.5f,	0.f,	0.f,	0.f,	0.f,	1.f,	// far top right
	0.5f,	-0.5f,	0.5f,	0.f,	1.f,	0.f,	0.f,	1.f,	// far bottom right
	// top face
	0.5f,	0.5f,	0.5f,	1.f,	1.f,	0.f,	1.f,	0.f,	// far top right
	0.5f,	0.5f,	-0.5f,	1.f,	0.f,	0.f,	1.f,	0.f,	// close top right
	-0.5f,	0.5f,	-0.5f,	0.f,	0.f,	0.f,	1.f,	0.f,	// close top left
	-0.5f,	0.5f,	0.5f,	0.f,	1.f,	0.f,	1.f,	0.f,	// far top left
	// bottom face
	0.5f,	-0.5f,	-0.5f,	1.f,	1.f,	0.f,	-1.f,	0.f,	// close bottom right
	0.5f,	-0.5f,	0.5f,	1.f,	0.f,	0.f,	-1.f,	0.f,	// far bottom right
	-0.5f,	-0.5f,	0.5f,	0.f,	0.f,	0.f,	-1.f,	0.f,	// far bottom left
	-0.5f,	-0.5f,	-0.5f,	0.f,	1.f,	0.f,	-1.f,	0.f,	// close bottom left
	// left face
	-0.5f,	0.5f,	0.5f,	1.f,	1.f,	-1.f,	0.f,	0.f,	// far top left
	-0.5f,	0.5f,	-0.5f,	1.f,	0.f,	-1.f,	0.f,	0.f,	// close top left
	-0.5f,	-0.5f,	-0.5f,	0.f,	0.f,	-1.f,	0.f,	0.f,	// close bottom left
	-0.5f,	-0.5f,	0.5f,	0.f,	1.f,	-1.f,	0.f,	0.f,	// far bottom left
	// right face
	0.5f,	-0.5f,	0.5f,	1.f,	1.f,	1.f,	0.f,	0.f,	// far bottom right
	0.5f,	-0.5f,	-0.5f,	1.f,	0.f,	1.f,	0.f,	0.f,	// close bottom right
	0.5f,	0.5f,	-0.5f,	0.f,	0.f,	1.f,	0.f,	0.f,	// close top right
	0.5f,	0.5f,	0.5f,	0.f,	1.f,	1.f,	0.f,	0.f,	// far top right
};

static unsigned int square_indices[] = {
	// close
	0,	1,	3,
	1,	2,	3,
	// far
	4,	5,	7,
	5,	6,	7,
	// top
	8,	9,	11,
	9,	10,	11,
	// bottom
	12,	13,	15,
	13,	14,	15,
	// left
	16,	17,	19,
	17,	18,	19,
	// right
	20,	21,	23,
	21,	22,	23
};

static float square_color[] = {
	1.f,	0.5f,	0.f
};

// r, g, b, strength
static float ambient_color[] = {
	0.5f,	0.f,	1.f
};

static float light_pos[] = {
	0.25f,	0.25f,	-1.f
};

static float light_color[] = {
	1.f,	1.f,	1.f
};

// coordinate space matrices
glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
//glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -3.f));
glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)width/(float)height, 0.1f, 100.f);

enum keybindCodes {
	W = 0,
	A = 1,
	S = 2,
	D = 3,
	SHIFT = 4,
	SPACE = 5,
	UP = 6,
	LEFT = 7,
	DOWN = 8,
	RIGHT = 9
};

bool keybinds[] = {
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	false
};

void process_keys(CAMERA *camera, float dt){
	if (keybinds[keybindCodes::W])
		move_camera(camera, -dt * 2.f * camera->vec_backward);
	if (keybinds[keybindCodes::A])
		move_camera(camera, -dt * 2.f * camera->vec_right);
	if (keybinds[keybindCodes::S])
		move_camera(camera, dt * 2.f * camera->vec_backward);
	if (keybinds[keybindCodes::D])
		move_camera(camera, dt * 2.f * camera->vec_right);
	if (keybinds[keybindCodes::SHIFT])
		move_camera(camera, glm::vec3(0.f, -dt * 2.f, 0.f));
	if (keybinds[keybindCodes::SPACE])
		move_camera(camera, glm::vec3(0.f, dt * 2.f, 0.f));
}

void err_callback(int err, const char *desc) {
	std::cerr << "Error: " << desc << '\n';
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	bool act = true;
	if (key == GLFW_KEY_TAB || key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
	if (action == GLFW_RELEASE) {
		act = false;
	}
	switch (key) {
		case GLFW_KEY_W:
			keybinds[keybindCodes::W] = act;
			break;
		case GLFW_KEY_A:
			keybinds[keybindCodes::A] = act;
			break;
		case GLFW_KEY_S:
			keybinds[keybindCodes::S] = act;
			break;
		case GLFW_KEY_D:
			keybinds[keybindCodes::D] = act;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			keybinds[keybindCodes::SHIFT] = act;
			break;
		case GLFW_KEY_SPACE:
			keybinds[keybindCodes::SPACE] = act;
			break;
		case GLFW_KEY_UP:
			keybinds[keybindCodes::UP] = act;
			break;
		case GLFW_KEY_LEFT:
			keybinds[keybindCodes::LEFT] = act;
			break;
		case GLFW_KEY_DOWN:
			keybinds[keybindCodes::DOWN] = act;
			break;
		case GLFW_KEY_RIGHT:
			keybinds[keybindCodes::RIGHT] = act;
			break;
		default:
			break;
	}
}

static void framebuffer_siz_callback(GLFWwindow *window, int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, width, height);
	projection = glm::perspective(glm::radians(45.f), (float)width/(float)height, 0.1f, 100.f);

}

static void cursor_callback(GLFWwindow *window, double x, double y) {
	if (firstMouse) {
		lastx = (float)x;
		lasty = (float)y;
		firstMouse = false;
	}
	deltax = (float)x - lastx;
	deltay = lasty - (float)y;
	lastx = (float)x;
	lasty = (float)y;
	deltax *= cursor_sensitivity;
	deltay *= cursor_sensitivity;
	rotate_camera(&camera, 0.f, deltay, deltax);
}


int main() {
	const char vert_shader_path[] = "./shaders/shader.vert";
	const char frag_shader_path[] = "./shaders/shader.frag";
	const char tex_image_path[] = "./textures/Uzumaki-Junji-Ito.jpg";
	unsigned char *tex_data;
	char *vert_shader_source, *frag_shader_source;
	GLuint vert_shader, frag_shader, shader_program, vertex_buffer, vertex_array, element_buffer, texture; // 
	GLint vec3_vertPosition, vec3_Color, vec2_TexCoords, vec3_AmbientColor, vec3_LightPos, vec3_vertNormal, vec3_LightColor, float_AmbientStrength, float_LightStrength, float_SpecularStrength, vec3_CameraPosition, mat4_model, mat4_view, mat4_projection; // buffer objects for shaders
	int success;
	char info[512];
	int tex_width, tex_height, tex_nrChannels;
	float oldtime, newtime, deltatime;
	glfwSetErrorCallback(err_callback);
	if (!glfwInit()) {
		std::cerr << "Could not initialize glfw\n";
		glfwTerminate();
		return -1;
	}
	GLFWwindow *window = glfwCreateWindow(width, height, "square", NULL, NULL);
	if (!window) {
		std::cerr << "Could not create window\n";
		glfwTerminate();
		return -1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, cursor_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_siz_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Coult not initialize glad\n";
		glfwTerminate();
		return -1;
	}
	glfwSwapInterval(1);
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// compile shaders
	vert_shader_source = read_file(vert_shader_path);
	if (!vert_shader_source) {
		glfwTerminate();
		return -1;
	}
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vert_shader_source, NULL);
	glCompileShader(vert_shader);
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vert_shader, 512, NULL, info);
		std::cerr << "Could not compile vertex shader\n" << info << '\n';
	}
	
	frag_shader_source = read_file(frag_shader_path);
	if (!frag_shader_source) {
		glfwTerminate();
		return -1;
	}
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, info);
		std::cerr << "Could not compile fragment shader\n" << info << '\n';
	}

	glEnable(GL_DEPTH_TEST);

	// create shader program
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vert_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	// configure vertex attributes (everything will be passed to the vertex shader, and then the vertex shader will pass needed values to the fragment shader)
	// create vertex buffer object and vertex array object (and element buffer object)
	vec3_vertPosition = glGetAttribLocation(shader_program, "vertPosition");
	vec2_TexCoords = glGetAttribLocation(shader_program, "TexCoords");
	vec3_vertNormal = glGetAttribLocation(shader_program, "vertNormal");
	vec3_AmbientColor = glGetUniformLocation(shader_program, "AmbientColor");
	float_AmbientStrength = glGetUniformLocation(shader_program, "AmbientStrength");
	vec3_LightPos = glGetUniformLocation(shader_program, "LightPos");
	vec3_LightColor = glGetUniformLocation(shader_program, "LightColor");
	float_LightStrength = glGetUniformLocation(shader_program, "LightStrength");
	float_SpecularStrength = glGetUniformLocation(shader_program, "SpecularStrength");
	vec3_CameraPosition = glGetUniformLocation(shader_program, "CameraPosition");
	mat4_model = glGetUniformLocation(shader_program, "model");
	mat4_view = glGetUniformLocation(shader_program, "view");
	mat4_projection = glGetUniformLocation(shader_program, "projection");
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &element_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_data), square_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);
	glVertexAttribPointer(vec3_vertPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(vec2_TexCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(vec3_vertNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(vec3_vertPosition);
	glEnableVertexAttribArray(vec2_TexCoords);
	glEnableVertexAttribArray(vec3_vertNormal);

	// texture stuff
	stbi_set_flip_vertically_on_load(true);
	tex_data = stbi_load(tex_image_path, &tex_width, &tex_height, &tex_nrChannels, 0);
	if (!tex_data) {
		std::cerr << "Could not load texture\n";
		glfwTerminate();
		return -1;
	}
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
	stbi_image_free(tex_data);

	// set up camera
	new_camera(&camera, {0, 0, -3}, {0, 0, 0});

	// coordinate system matrices
	glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(mat4_view, 1, GL_FALSE, glm::value_ptr(camera.view));
	glUniformMatrix4fv(mat4_projection, 1, GL_FALSE, glm::value_ptr(projection));

	// unchanged uniforms
	glUniform3fv(vec3_Color, 1, square_color);
	glUniform3fv(vec3_AmbientColor, 1, ambient_color);
	glUniform3fv(vec3_LightPos, 1, light_pos);
	glUniform3fv(vec3_LightColor, 1, light_color);
	glUniform1f(float_AmbientStrength, 0.2f);
	glUniform1f(float_LightStrength, 0.1f);
	glUniform1f(float_SpecularStrength, 0.4f);
	glUniform3fv(vec3_CameraPosition, 1, glm::value_ptr(camera.position));

	//main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(shader_program);
		glBindVertexArray(vertex_array);
		// deltatime
		oldtime = newtime;
		newtime = (float)glfwGetTime();
		deltatime = newtime - oldtime;
		// process input
		process_keys(&camera, deltatime);
		// load stuff into the buffer (buffer data then uniform)
		glUniform3fv(vec3_Color, 1, square_color);
		glUniform3fv(vec3_AmbientColor, 1, ambient_color);
		glUniform3fv(vec3_LightPos, 1, light_pos);
		glUniform3fv(vec3_LightColor, 1, light_color);
		glUniform1f(float_AmbientStrength, 0.2f);
		glUniform1f(float_LightStrength, 0.1f);
		glUniform1f(float_SpecularStrength, 0.4f);
		glUniform3fv(vec3_CameraPosition, 1, glm::value_ptr(camera.position));
		glUniformMatrix4fv(mat4_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(mat4_view, 1, GL_FALSE, glm::value_ptr(camera.view));
		glUniformMatrix4fv(mat4_projection, 1, GL_FALSE, glm::value_ptr(projection));
		// draw things (glDrawElements uses the indices from the bound element buffer object, in this case element_buffer)
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		// second parameter - the number of indices specified, since opengl uses triangles, 2 triangles are needed to draw a square resulting in 6 vertices drawn
		// fourth parameter - the offset of the indices
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
