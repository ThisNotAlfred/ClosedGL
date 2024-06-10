#include "closed_gl.hpp"

class Camera
{
        public:
    explicit Camera() = default;
    Camera(float movement_speed, float scrolling_speed, float spinning_speed)
        : movement_speed(movement_speed), scrolling_speed(scrolling_speed), spinning_speed(spinning_speed) {};
    ~Camera() = default;

    auto transform() -> void;

    // forward and back
    auto go_forward(float delta_time) -> void;
    auto go_back(float delta_time) -> void;
    // right and left
    auto go_right(float delta_time) -> void;
    auto go_left(float delta_time) -> void;
    // up and down
    auto go_up(float delta_time) -> void;
    auto go_down(float delta_time) -> void;
    // mouse
    auto rotate(float mouse_x, float mouse_y) -> void;

    [[nodiscard]] static auto get_projection_matrix(int width, int height) -> glm::mat4;
    [[nodiscard]] auto get_view_matrix() const -> glm::mat4;

        private:
    float movement_speed;
    float scrolling_speed;
    float spinning_speed;

    glm::vec3 position = glm::vec4(0, 0, -1, 0);
    glm::vec3 up       = glm::vec4(0, 1, 0, 0);
    glm::vec3 target   = glm::vec4();

    float pitch;
    float yaw;

    float forward_and_back;
    float right_and_left;
    float up_and_down;

    float last_mouse_x;
    float last_mouse_y;
};