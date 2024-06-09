#include "closed_gl.hpp"

class Camera
{
        public:
    Camera(short movement_speed, float spinning_speed)
        : movement_speed(movement_speed), spinning_speed(spinning_speed) {};
    ~Camera() = default;

    

        private:
    short movement_speed;
    float spinning_speed;
};