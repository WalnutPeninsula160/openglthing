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


struct Object {
	std::vector<float> vertex_data;
	std::vector<unsigned int> indices;
	MATERIAL material;
};
