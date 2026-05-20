#include "framebuffer.hpp"


frameBuffer::frameBuffer(width, height) {
	glGenFramebuffers(1, &buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);
	// set up vertex array and buffer objects
	glGenVertexArrays(1, &vertex_array);		// vertex array
	glGenBuffer(1, &vertex_buffer);
	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);	// vetex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, framebuffer_vertex_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glGenTextures(1, &color_texture_buffer);	// color texture
	glBindTexture(GL_TEXTURE_2D, color_texture_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, null);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenRenderBuffer(1, &depth_stencil_buffer);	// depth/stencil buffer
	glBindRenderBuffer(GL_RENDERBUFFER, depth_stencil_buffer);
	glRenderBufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderBuffer(GL_RENDERBUFFER, 0);
	// attatch texture buffer and render buffer to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTATCHMENT0, GL_TEXTURE_2D, color_texture_buffer, 0);
	glFramebufferRenderBuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTATCHMENT, GL_RENDERBUFFER, depth_stencil_buffer);
}
