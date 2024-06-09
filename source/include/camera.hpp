#include "closed_gl.hpp"

class Camera
{
        public:
    explicit Camera() = default;
    Camera(short movement_speed, short scrolling_speed, float spinning_speed)
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

    [[nodiscard]] auto get_projection_matrix() const -> glm::mat4;
    [[nodiscard]] auto get_view_matrix() const -> glm::mat4;
    [[nodiscard]] auto get_model_matrix() const -> glm::mat4;

        private:
    float movement_speed;
    float scrolling_speed;
    float spinning_speed;

    glm::vec4 position = glm::vec4(0, 0, 0, 0);
    glm::vec3 up       = glm::vec3(0, 1, 0);
    glm::vec3 target   = glm::vec3();

    glm::mat4 projection = glm::mat4(1);
    glm::mat4 view       = glm::mat4(1);
    glm::mat4 model      = glm::mat4(1);

    float pitch;
    float yaw;

    float forward_and_back;
    float right_and_left;
    float up_and_down;

    float last_mouse_x;
    float last_mouse_y;
};