#include "camera.hpp"
#include <array>
#include <cmath>

CAMERA new_camera(std::array<float, 3> pos, std::array<float, 3> target) {
	CAMERA *result = new CAMERA;
	result->position = glm::make_vec3(pos.data());
	result->target = glm::make_vec3(target.data());
	result->vec_backward = glm::normalize(result->position - result->target);
	result->vec_right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), result->vec_backward));
	result->vec_up = glm::cross(result->vec_backward, result->vec_right);
	result->view = glm::lookAt(result->position, result->target, result->vec_up);
	result->yaw = -90.f;
	return *result;
}


void move_camera(CAMERA *camera, glm::vec3 direction, float speed) {
	direction = glm::normalize(direction) * speed;
	camera->position += direction;
	camera->target += direction;
	camera->view = glm::lookAt(camera->position, camera->target, camera->vec_up);
}

void rotate_camera(CAMERA *camera, float roll, float pitch, float yaw) {
	camera->vec_backward.x = std::cosf(glm::radians(yaw)) * std::cosf(glm::radians(pitch));
	camera->vec_backward.y = std::sinf(glm::radians(pitch));
	camera->vec_backward.z = std::sinf(glm::radians(yaw)) * std::cosf(glm::radians(pitch));
}

void rotate_camera_around_target(CAMERA *camera, glm::vec3 plane_normal, float theta) {
	glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.f), theta, plane_normal);
	glm::vec4 position_vec4 = rotation_matrix * glm::vec4(camera->position, 1.f);
	camera->position = glm::vec3(position_vec4);
	camera->vec_backward = glm::normalize(camera->position - camera->target);
	camera->vec_right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), camera->vec_backward));
	camera->vec_up = glm::cross(camera->vec_backward, camera->vec_right);
	camera->view = glm::lookAt(camera->position, camera->target, camera->vec_up);
}
