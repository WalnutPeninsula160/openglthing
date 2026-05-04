

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
	frameBuffer() {
		glGenFramebuffers(1, &buffer);
		glbingFramebuffer(GL_FRAMEBUFFER, buffer);
		glGenVertexArrays(1, &vertex_array);
		glGenBuffer(1, &vertex_buffer);
		glBindVertexArray(vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glGenTextures(1, &color_texture_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, framebuffer_vertex_data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 1, 
		glGenRenderBuffer(1, &depth_stencil_buffer);
	}
};
