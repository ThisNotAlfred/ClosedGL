#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <numbers>

static constexpr glm::vec3 world_up      = glm::vec3(0, 1, 0);
static constexpr glm::vec3 world_forward = glm::vec3(0, 0, 1);

auto
Camera::transform() -> void
{
    auto quaternion = glm::quat(glm::vec3(this->yaw, this->pitch, 0));

    this->up     = glm::rotate(quaternion, world_up);
    this->target = glm::normalize(glm::rotate(quaternion, world_forward));

    auto movement_vector    = glm::vec3(this->right_and_left, this->up_and_down, this->forward_and_back);
    auto direction = glm::rotate(quaternion, movement_vector);

    auto translation_matrix = glm::translate(glm::mat4(), movement_vector);
    this->position = translation_matrix * this->position;

    this->forward_and_back = 0;
    this->right_and_left = 0;
    this->up_and_down = 0;
}

auto
Camera::go_forward(float delta_time) -> void
{
    this->forward_and_back += delta_time * movement_speed;
}

auto
Camera::go_back(float delta_time) -> void
{
    this->forward_and_back -= delta_time * movement_speed;
}

auto
Camera::go_right(float delta_time) -> void
{
    this->right_and_left += delta_time * movement_speed;
}

auto
Camera::go_left(float delta_time) -> void
{
    this->right_and_left -= delta_time * movement_speed;
}

auto
Camera::go_up(float delta_time) -> void
{
    this->up_and_down += delta_time * movement_speed;
}

auto
Camera::go_down(float delta_time) -> void
{
    this->up_and_down -= delta_time * movement_speed;
}

auto
Camera::rotate(float mouse_x, float mouse_y) -> void
{
    this->yaw   += ((mouse_x - this->last_mouse_x) * this->spinning_speed);
    this->pitch += ((mouse_y - this->last_mouse_y) * this->spinning_speed);

    this->pitch = std::clamp(this->pitch, -89.0F, 89.0F);

    this->last_mouse_x = mouse_x;
    this->last_mouse_y = mouse_y;
}

auto
Camera::get_projection_matrix() const -> glm::mat4
{
    return this->projection;
}

auto
Camera::get_view_matrix() const -> glm::mat4
{
    return this->view;
}

auto
Camera::get_model_matrix() const -> glm::mat4
{
    return this->model;
}
