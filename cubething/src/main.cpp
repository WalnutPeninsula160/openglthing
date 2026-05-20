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
#include "objects.hpp"

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

// store the quad meant to cover the whole screen globally so that it is not repeated for every frame buffer
static std::vector<float> framebuffer_vertex_data = {
	-1.f,	1.f,	0.f,	1.f,
	1.f,	-1.f,	1.f,	0.f,
	-1.f,	-1.f,	0.f,	0.f,
	
	-1.f,	1.f,	0.f,	1.f,
	1.f,	1.f,	1.f,	1.f,
	1.f,	-1.f,	1.f,	0.f
};

GLuint frame_buffer, framebuffer_texture, framebuffer_renderbuffer;

static struct Object cube {
	// vec3 verts, vec2 texCoords, vec3 normal
	.vertex_data = {
		// close face
		0.5f,	0.5f,	0.5f,	0.f,	0.f,	0.f,	0.f,	1.f,	// top right
		0.5f,	-0.5f,	0.5f,	0.f,	1.f,	0.f,	0.f,	1.f,	// bottom right
		-0.5f,	-0.5f,	0.5f,	1.f,	1.f,	0.f,	0.f,	1.f,	// bottom left
		-0.5f,	0.5f,	0.5f,	1.f,	0.f,	0.f,	0.f,	1.f,	// top left
		// far face
		0.5f,	0.5f,	-0.5f,	1.f,	1.f,	0.f,	0.f,	-1.f,	// top right
		0.5f,	-0.5f,	-0.5f,	1.f,	0.f,	0.f,	0.f,	-1.f,	// bottom right
		-0.5f,	-0.5f,	-0.5f,	0.f,	0.f,	0.f,	0.f,	-1.f,	// bottom left
		-0.5f,	0.5f,	-0.5f,	0.f,	1.f,	0.f,	0.f,	-1.f,	// top left
		// top face
		0.5f,	0.5f,	-0.5f,	1.f,	0.f,	0.f,	1.f,	0.f,	// top right
		0.5f,	0.5f,	0.5f,	1.f,	1.f,	0.f,	1.f,	0.f,	// bottom right
		-0.5f,	0.5f,	0.5f,	0.f,	1.f,	0.f,	1.f,	0.f,	// bottom left
		-0.5f,	0.5f,	-0.5f,	0.f,	0.f,	0.f,	1.f,	0.f,	// top left
		// bottom face
		0.5f,	-0.5f,	0.5f,	1.f,	0.f,	0.f,	-1.f,	0.f,	// top right
		0.5f,	-0.5f,	-0.5f,	1.f,	1.f,	0.f,	-1.f,	0.f,	// bottom right
		-0.5f,	-0.5f,	-0.5f,	0.f,	1.f,	0.f,	-1.f,	0.f,	// bottom left
		-0.5f,	-0.5f,	0.5f,	0.f,	0.f,	0.f,	-1.f,	0.f,	// bottom left
		// left face
		-0.5f,	0.5f,	0.5f,	1.f,	1.f,	-1.f,	0.f,	0.f,	// top left
		-0.5f,	-0.5f,	0.5f,	0.f,	1.f,	-1.f,	0.f,	0.f,	// bottom left
		-0.5f,	-0.5f,	-0.5f,	0.f,	0.f,	-1.f,	0.f,	0.f,	// bottom left
		-0.5f,	0.5f,	-0.5f,	1.f,	0.f,	-1.f,	0.f,	0.f,	// top left
		// right face
		0.5f,	0.5f,	-0.5f,	0.f,	0.f,	1.f,	0.f,	0.f,	// top right
		0.5f,	-0.5f,	-0.5f,	1.f,	0.f,	1.f,	0.f,	0.f,	// bottom right
		0.5f,	-0.5f,	0.5f,	1.f,	1.f,	1.f,	0.f,	0.f,	// bottom right
		0.5f,	0.5f,	0.5f,	0.f,	1.f,	1.f,	0.f,	0.f,	// top right
	},
	.indices = {
		// close
		0,	1,	2,
		0,	2,	3,
		// far
		4,	5,	6,
		4,	6,	7,
		// top
		8,	9,	10,
		8,	10,	11,
		// bottom
		12,	13,	14,
		12,	14,	15,
		// left
		16,	17,	18,
		16,	18,	19,
		// right
		20,	21,	22,
		20,	22,	23
	},
	.material = {
		.ambient = {1.f, 	1.f,	1.f},
		.diffuse = {1.f,	1.f,	1.f},
		.specular = {1.f,	1.f,	1.f},
		.shiny = 32.f,
	}
};

static struct LIGHT light = {
	.position = {0.7f, 0.7f, 1.f},
	.ambient = {0.3f, 0.0f, 0.3f},
	.diffuse = {0.5f, 0.0f, 0.5f},
	.specular = {0.6f, 0.0f, 0.6f},
	.constant = 0.2f,
	.linear = 0.3f,
	.quadratic = 0.4f,
};

// coordinate space matrices
glm::mat4 model[4] = {
	glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f)),
	glm::translate(glm::mat4(1.f), glm::vec3(1.f, 1.f, 0.f)),
	glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 0.f)),
	glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 1.f))
};
//glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -3.f));
glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)width/(float)height, 0.1f, 100.f);
glm::mat3 Normalize[4] = {
	glm::mat3(glm::transpose(glm::inverse(model[0]))),
	glm::mat3(glm::transpose(glm::inverse(model[1]))),
	glm::mat3(glm::transpose(glm::inverse(model[2]))),
	glm::mat3(glm::transpose(glm::inverse(model[3])))
};

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
	glDeleteTextures(1, &framebuffer_texture);
	glDeleteRenderbuffers(1, &framebuffer_renderbuffer);
	glDeleteFramebuffers(1, &frame_buffer);
	// set up framebuffer
	// create framebuffer
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	// create framebuffer texture
	glGenTextures(1, &framebuffer_texture);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
	glViewport(0, 0, w, h);
	projection = glm::perspective(glm::radians(45.f), (float)w/(float)h, 0.1f, 100.f);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// attach the framebuffer texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_texture, 0);
	// create the framebuffer render buffer
	glGenRenderbuffers(1, &framebuffer_renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, framebuffer_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// attach the framebubffer render buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer_renderbuffer);
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


int main(int argc, char **argv) {
	const char vert_shader_path[] = "./shaders/shader.vert";
	const char frag_shader_path[] = "./shaders/shader.frag";
	const char post_vert_shader_path[] = "./shaders/post_shader.vert";
	const char post_frag_shader_path[] = "./shaders/post_shader.frag";
	const char tex_image_path[] = "./textures/Uzumaki-Junji-Ito.jpg";
	unsigned char *tex_data;
	char *vert_shader_source, *frag_shader_source, *post_vert_shader_source, *post_frag_shader_source;
	GLuint vert_shader, frag_shader, shader_program, vertex_buffer, vertex_array, element_buffer, texture;
	GLuint post_vert_shader, post_frag_shader, post_shader_program, post_vertex_buffer, post_vertex_array;
	// buffer objects for shaders
	GLint vec3_vertPosition, vec2_TexCoords, vec3_vertNormal;
	GLint vec3_CameraPosition, mat4_model[4], mat4_view, mat4_projection, mat3_Normalize[4];
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

	post_vert_shader_source = read_file(post_vert_shader_path);
	if (!post_vert_shader_source) {
		glfwTerminate();
		return -1;
	}
	post_vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(post_vert_shader, 1, &post_vert_shader_source, NULL);
	glCompileShader(post_vert_shader);
	glGetShaderiv(post_vert_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(post_vert_shader, 512, NULL, info);
		std::cerr << "Could not compile post processing vertex shader\n" << info << '\n';
	}

	post_frag_shader_source = read_file(post_frag_shader_path);
	if (!post_frag_shader_source) {
		glfwTerminate();
		return -1;
	}
	post_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(post_frag_shader, 1, &post_frag_shader_source, NULL);
	glCompileShader(post_frag_shader);
	glGetShaderiv(post_frag_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(post_frag_shader, 512, NULL, info);
		std::cerr << "Could not compile post processing fragment shader\n" << info << '\n';
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

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
	cube.material.buffers.ambient = glGetUniformLocation(shader_program, "material.ambient");
	cube.material.buffers.diffuse = glGetUniformLocation(shader_program, "material.diffuse");
	cube.material.buffers.specular = glGetUniformLocation(shader_program, "material.specular");
	cube.material.buffers.shiny = glGetUniformLocation(shader_program, "material.shiny");
	light.buffers.position = glGetUniformLocation(shader_program, "light.position");
	light.buffers.ambient = glGetUniformLocation(shader_program, "light.ambient");
	light.buffers.diffuse = glGetUniformLocation(shader_program, "light.diffuse");
	light.buffers.specular = glGetUniformLocation(shader_program, "light.specular");
	light.buffers.constant = glGetUniformLocation(shader_program, "light.constant");
	light.buffers.linear = glGetUniformLocation(shader_program, "light.linear");
	light.buffers.quadratic = glGetUniformLocation(shader_program, "light.quadratic");
	vec3_CameraPosition = glGetUniformLocation(shader_program, "viewPos");
	for (unsigned int i {}; i < 4; i++) {
		mat4_model[i] = glGetUniformLocation(shader_program, ("mat4_model[" + std::to_string(i) + "]").c_str());
		mat3_Normalize[i] = glGetUniformLocation(shader_program, ("Normalize[" + std::to_string(i) + "]").c_str());
	}
	mat4_view = glGetUniformLocation(shader_program, "view");
	mat4_projection = glGetUniformLocation(shader_program, "projection");
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &element_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cube.vertex_data.size(), cube.vertex_data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * cube.indices.size(), cube.indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(vec3_vertPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(vec2_TexCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(vec3_vertNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(vec3_vertPosition);
	glEnableVertexAttribArray(vec2_TexCoords);
	glEnableVertexAttribArray(vec3_vertNormal);

	// create post processing shader program
	post_shader_program = glCreateProgram();
	glAttachShader(post_shader_program, post_vert_shader);
	glAttachShader(post_shader_program, post_frag_shader);
	glLinkProgram(post_shader_program);

	// configure vertex attributes for the post processing buffer
	glGenVertexArrays(1, &post_vertex_array);
	glGenBuffers(1, &post_vertex_buffer);
	glBindVertexArray(post_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, post_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * framebuffer_vertex_data.size(), framebuffer_vertex_data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// set up framebuffer
	// create framebuffer
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	// create framebuffer texture
	glGenTextures(1, &framebuffer_texture);
	glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// attach the framebuffer texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_texture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glfwTerminate();
		return -1;
	}
	// create the framebuffer render buffer
	glGenRenderbuffers(1, &framebuffer_renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, framebuffer_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// attach the framebubffer render buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer_renderbuffer);

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
	new_camera(&camera, {-1.f, -0.5f, 3.f}, {0, 0, 0});

	// coordinate system matrices
	for (unsigned int i {}; i < 4; i++) {
		glUniformMatrix4fv(mat4_model[i], 1, GL_FALSE, glm::value_ptr(model[i]));
		glUniformMatrix3fv(mat3_Normalize[i], 1, GL_FALSE, glm::value_ptr(Normalize[i]));
	}
	glUniformMatrix4fv(mat4_view, 1, GL_FALSE, glm::value_ptr(camera.view));
	glUniformMatrix4fv(mat4_projection, 1, GL_FALSE, glm::value_ptr(projection));

	// unchanged uniforms
	glUniform3fv(cube.material.buffers.ambient, 1, cube.material.ambient);
	glUniform3fv(cube.material.buffers.diffuse, 1, cube.material.diffuse);
	glUniform3fv(cube.material.buffers.specular, 1, cube.material.specular);
	glUniform1f(cube.material.buffers.shiny, cube.material.buffers.shiny);
	glUniform3fv(light.buffers.position, 1, light.position);
	glUniform3fv(light.buffers.ambient, 1, light.ambient);
	glUniform3fv(light.buffers.diffuse, 1, light.diffuse);
	glUniform3fv(light.buffers.specular, 1, light.specular);
	glUniform1f(light.buffers.constant, light.constant);
	glUniform1f(light.buffers.linear, light.linear);
	glUniform1f(light.buffers.quadratic, light.quadratic);
	glUniform3fv(vec3_CameraPosition, 1, glm::value_ptr(camera.position));

	//main loop
	while (!glfwWindowShouldClose(window)) {
		glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.f, 0.f, 0.f, 0.f);
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
		// compute normalize matrix (better to do it in cpu rather than gpu)
		for (unsigned int i {}; i < 4; i++) 
			Normalize[i] = glm::mat3(glm::transpose(glm::inverse(model[i])));
		// load stuff into the buffer (buffer data then uniform)
		glUniform3fv(cube.material.buffers.ambient, 1, cube.material.ambient);
		glUniform3fv(cube.material.buffers.diffuse, 1, cube.material.diffuse);
		glUniform3fv(cube.material.buffers.specular, 1, cube.material.specular);
		glUniform1f(cube.material.buffers.shiny, cube.material.buffers.shiny);
		glUniform3fv(light.buffers.position, 1, light.position);
		glUniform3fv(light.buffers.ambient, 1, light.ambient);
		glUniform3fv(light.buffers.diffuse, 1, light.diffuse);
		glUniform3fv(light.buffers.specular, 1, light.specular);
		glUniform1f(light.buffers.constant, light.constant);
		glUniform1f(light.buffers.linear, light.linear);
		glUniform1f(light.buffers.quadratic, light.quadratic);
		glUniform3fv(vec3_CameraPosition, 1, glm::value_ptr(camera.position));
		for (unsigned int i {}; i < 4; i++) {
			glUniformMatrix4fv(mat4_model[i], 1, GL_FALSE, glm::value_ptr(model[i]));
			glUniformMatrix3fv(mat3_Normalize[i], 1, GL_FALSE, glm::value_ptr(Normalize[i]));
		}
		glUniformMatrix4fv(mat4_view, 1, GL_FALSE, glm::value_ptr(camera.view));
		glUniformMatrix4fv(mat4_projection, 1, GL_FALSE, glm::value_ptr(projection));
		// draw things (glDrawElements uses the indices from the bound element buffer object, in this case element_buffer)
		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 4);
		// second parameter - the number of indices specified, since opengl uses triangles, 2 triangles are needed to draw a square resulting in 6 vertices drawn
		// fourth parameter - the offset of the indices
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		// use post processing shader
		glUseProgram(post_shader_program);
		glBindTexture(GL_TEXTURE_2D, framebuffer_texture);
		glBindVertexArray(post_vertex_array);
		// draw the things
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glBindBuffer(GL_FRAMEBUFFER, 0); // unbind the created framebuffer
	glDeleteBuffers(1, &frame_buffer); // delete the framebuffer when it is no longer needed
	glfwTerminate();
	return 0;
}
