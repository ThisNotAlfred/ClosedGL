#include "engine.hpp"

Engine::Engine(GLFWwindow* window)
{
    // getting size of the current window
    this->window = window;
    glfwGetWindowSize(this->window, &this->width, &this->height);

    // initilizing default values
    this->user_interface = UI(window);
    this->camera         = Camera(10, 3, 0.3F);
}

auto
Engine::frame() -> void
{
    // main loop
    while (this->run) {
        auto loop_start = std::chrono::high_resolution_clock::now();

        glfwPollEvents();

        gl::glClearColor(0, 0, 0, 1);
        gl::glClearDepth(1);
        gl::glEnable(gl::GL_DEPTH_TEST);
        gl::glDepthFunc(gl::GL_LEQUAL);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        this->user_interface.draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(this->window);

        auto loop_end                     = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> time = loop_end - loop_start;
        delta_time                        = time.count();
        
#ifdef _DEBUG
        glfwSetWindowTitle(window, std::format("ClosedGL {}fps", static_cast<int>(1.0F / delta_time)).c_str());
#endif
    }
}