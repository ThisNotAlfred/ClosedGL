#include "closed_gl.hpp"
#include "glbinding/gl/enum.h"
#include "glbinding/gl/functions.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <chrono>
#include <format>
#include <iostream>

auto
error(int errnum, const char* errmsg)
{
    std::cerr << errnum << ": " << errmsg << '\n';
}

auto
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // size of the screen
    // can implement resizing here!
}

auto
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

auto
main() -> int
{
    glfwSetErrorCallback(error);

    if (glfwInit() == 0) {
        return 1;
    }

    // setting the opengl version to 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    auto* window = glfwCreateWindow(1600, 900, "beaviewer", nullptr, nullptr);
    if (window == nullptr) {
#ifdef _DEBUG
        std::cerr << "failed to creat the window";
#endif
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);

    // setting up imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto imgui_io = ImGui::GetIO();
    static_cast<void>(imgui_io);
    // enabling keyboard in imgui
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // dark style imgui
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // initializing opengl
    glbinding::initialize(glfwGetProcAddress, false);
    glbinding::aux::enableGetErrorCallback();

    std::cout << '\n'
              << "OpenGL Version:  " << glbinding::aux::ContextInfo::version() << '\n'
              << "OpenGL Vendor:   " << glbinding::aux::ContextInfo::vendor() << '\n'
              << "OpenGL Renderer: " << glbinding::aux::ContextInfo::renderer() << '\n';

    // main loop
    unsigned int elapsed_time = 0;
    while (glfwWindowShouldClose(window) == 0) {

        // setting `loop_start` to calculate `elapsed_time` later
        auto loop_start = std::chrono::steady_clock::now();

        glfwPollEvents();

        // start imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // current frame time using imgui
        ImGui::Text("%dms", elapsed_time);

        // rendering imgui windows
        ImGui::Render();
        
        // clearing last frame
        gl::glClearColor(0, 0, 0, 0);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT);

        // drawing imgui windows on current frame
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        // setting `time_point` and calculating `elapsed_time`
        auto loop_end = std::chrono::steady_clock::now();
        elapsed_time  = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start).count();
    }

    gl::glDisableVertexAttribArray(0);

    glfwTerminate();
    return 0;
}