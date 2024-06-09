#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <numbers>

static constexpr glm::vec3 world_up      = glm::vec3(0, 1, 0);
static constexpr glm::vec3 world_forward = glm::vec3(0, 0, 1);

auto
Camera::transform() -> void
{
    auto quaternion      = glm::quat(glm::vec3(this->pitch, this->yaw, 0));
    auto movement_vector = glm::vec3(this->right_and_left, this->up_and_down, this->forward_and_back);

    // TODO: use local axes instead of tranforming global ones
    this->up     = glm::rotate(quaternion, world_up);
    this->target = glm::normalize(glm::rotate(quaternion, world_forward));

    auto direction          = glm::rotate(quaternion, movement_vector);
    auto translation_matrix = glm::translate(glm::mat4(1.0F), direction);
    this->position          = translation_matrix * glm::vec4(this->position, 1.0F);

    this->forward_and_back = 0;
    this->right_and_left   = 0;
    this->up_and_down      = 0;
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
    this->right_and_left -= delta_time * movement_speed;
}

auto
Camera::go_left(float delta_time) -> void
{
    this->right_and_left += delta_time * movement_speed;
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
Camera::get_projection_matrix(int width, int height) -> glm::mat4
{
    return glm::perspective(glm::radians(30.0F), static_cast<float>(width) / static_cast<float>(height), 0.01F, 100.0F);
}

auto
Camera::get_view_matrix() const -> glm::mat4
{
    return glm::lookAtRH(this->position, this->position + this->target, this->up);
}
