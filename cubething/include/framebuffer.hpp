

// store the quad meant to cover the whole screen globally so that it is not repeated for every frame buffer
static framebuffer_vertex_data = {
	-1.f,	1.f,	0.f,	1.f,
	-1.f,	-1.f,	0.f,	0.f,
	1.f,	-1.f,	1.f,	0.f,
	
	-1.f,	1.f,	0.f,	1.f,
	1.f,	-1.f,	1.f,	0.f,
	1.f,	1.f,	1.f,	1.f
};

struct frameBuffer {
	GLuint VAO, VBO, texture;
};

class frameBuffer{
public:
	GLuint vertex_array, vertex_buffer, color_texture_buffer, depth_stencil_buffer, buffer;
	frameBuffer();
};
