#include "closed_gl.hpp"

#include "user_interface.hpp"

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

    auto* window = glfwCreateWindow(1600, 900, "ClosedGL", nullptr, nullptr);
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

    auto user_inter = UI(window);


    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f };
    uint32_t indices[] = { 0, 1, 2 };

    gl::GLuint vbo, vao, ibo;
    gl::glCreateBuffers(1, &vbo);
    gl::glNamedBufferStorage(vbo, sizeof(vertices), vertices, gl::GL_DYNAMIC_STORAGE_BIT);
    
    gl::glCreateBuffers(1, &ibo);
    gl::glNamedBufferStorage(ibo, sizeof(indices), indices, gl::GL_DYNAMIC_STORAGE_BIT);

    gl::glCreateVertexArrays(1, &vao);

    gl::glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(float) * 3);
    gl::glVertexArrayElementBuffer(vao, ibo);

    gl::glEnableVertexArrayAttrib(vao, 0);
    gl::glVertexArrayAttribFormat(vao, 0, 3, gl::GL_FLOAT, false, 0);
    gl::glVertexArrayAttribBinding(vao, 0, 0);

    // main loop
    unsigned int elapsed_time = 0;
    while (glfwWindowShouldClose(window) == 0) {
        // setting `loop_start` to calculate `elapsed_time` later
        auto loop_start = std::chrono::steady_clock::now();

        // clearing last frame
        gl::glClearColor(0, 0, 0, 0);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT);

        glfwPollEvents();
        
        gl::glBindVertexArray(vao);
        gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, ibo);
        gl::glDrawElements(gl::GL_TRIANGLES, sizeof(indices) / sizeof(uint32_t), gl::GL_UNSIGNED_INT, nullptr);

        // start imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // user interface drawing
        user_inter.draw();

        // rendering imgui windows
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        // setting `time_point` and calculating `elapsed_time`
        auto loop_end = std::chrono::steady_clock::now();
        elapsed_time  = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start).count();

#ifdef _DEBUG
        glfwSetWindowTitle(window, std::format("ClosedGL {}ms", elapsed_time).c_str());
#endif
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    glfwTerminate();

    return 0;
}