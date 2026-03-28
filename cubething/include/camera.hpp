#pragma once 

// standard C++ includes
#include <array>
// glm includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

struct CAMERA {
	glm::mat4 view;
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 vec_backward;
	glm::vec3 vec_right;
	glm::vec3 vec_up;
};

CAMERA new_camera(std::array<float, 3> pos, std::array<float, 3> target);

void move_camera(CAMERA *camera, glm::vec3 direction, float speed);

//void rotate_camera(CAMERA camera, glm::vec3 plane, float theta);

void rotate_camera_around_target(CAMERA *camera, glm::vec3 plane, float theta);
