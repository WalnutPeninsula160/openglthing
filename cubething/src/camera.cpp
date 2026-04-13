#include "camera.hpp"
#include <array>
#include <cmath>
#include <iostream>

CAMERA new_camera(CAMERA *result, std::array<float, 3> pos, std::array<float, 3> target) {
	result->position = glm::make_vec3(pos.data());
	result->vec_backward = glm::normalize(result->position - glm::make_vec3(target.data()));
	result->vec_right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), result->vec_backward));
	result->vec_up = glm::cross(result->vec_backward, result->vec_right);
	result->view = glm::lookAt(result->position, result->position - result->vec_backward, result->vec_up);
	result->pitch = glm::degrees(std::atan2f(-result->vec_backward.y, std::sqrtf(result->vec_backward.x * result->vec_backward.x + result->vec_backward.z * result->vec_backward.z)));
	result->yaw = glm::degrees(std::atan2f(-result->vec_backward.z, -result->vec_backward.x));
	return *result;
}


void move_camera(CAMERA *camera, glm::vec3 direction) {
	camera->position += direction;
	camera->target += direction;
	camera->view = glm::lookAt(camera->position, camera->position - camera->vec_backward, camera->vec_up);
}

void rotate_camera(CAMERA *camera, float delta_roll, float delta_pitch, float delta_yaw) {
	glm::vec3 front;
	camera->roll += delta_roll;
	camera->pitch += delta_pitch;
	camera->yaw += delta_yaw;
	if (camera->pitch > 89.f)
		camera->pitch = 89.0f;
	if (camera->pitch < -89.f)
		camera->pitch = -89.f;
	front.x = std::cosf(glm::radians(camera->yaw)) * std::cosf(glm::radians(camera->pitch));
	front.y = std::sinf(glm::radians(camera->pitch));
	front.z = std::sinf(glm::radians(camera->yaw)) * std::cosf(glm::radians(camera->pitch));
	camera->vec_backward = glm::normalize(-front);
	camera->vec_right = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), camera->vec_backward));
	camera->vec_up = glm::cross(camera->vec_backward, camera->vec_right);
	camera->view = glm::lookAt(camera->position, camera->position + front, camera->vec_up);
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
