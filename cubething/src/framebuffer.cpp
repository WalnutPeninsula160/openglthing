#include "framebuffer.hpp"


frameBuffer::frameBuffer() {
	glGenFramebuffers(1, &buffer);
	glbingFramebuffer(GL_FRAMEBUFFER, buffer);
	// set up vertex array and buffer objects
	glGenVertexArrays(1, &vertex_array);
	glGenBuffer(1, &vertex_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, framebuffer_vertex_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glGenTextures(1, &color_texture_buffer);
	glGenRenderBuffer(1, &depth_stencil_buffer);
}
