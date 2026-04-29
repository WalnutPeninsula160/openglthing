#pragma once

struct MATERIAL_BO {
	GLint ambient;
	GLint diffuse;
	GLint specular;
	GLint shiny;
};

struct LIGHT_BO {
	GLint position;
	GLint ambient;
	GLint diffuse;
	GLint specular;
	GLint constant;
	GLint linear;
	GLint quadratic;
};

struct MATERIAL {
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float shiny;
	MATERIAL_BO buffers;
};

struct LIGHT {
	float position[3];
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float constant;
	float linear;
	float quadratic;
	LIGHT_BO buffers;
};


template <size_t vertex_data_length, size_t num_of_tris>
struct Object {
	float vertex_data[vertex_data_length];
	float indices[num_of_tris * 3];
	MATERIAL material;
};
