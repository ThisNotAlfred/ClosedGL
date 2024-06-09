#include "engine.hpp"

#include <print>

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
Engine::init() -> void
{
    // TODO make this value not hard coded!
    auto path = std::filesystem::path("../bin/DragonAttenuation.glb");
    auto mesh = Mesh(path);

    if (!mesh.create_buffers()) {
        std::println(stderr, "failed to load the mesh\n");
    }

    meshes.push_back(mesh);

    auto program = gl::glCreateProgram();
    gl::glProgramParameteri(program, gl::GL_PROGRAM_SEPARABLE, gl::GL_TRUE);

    auto vert_path = std::filesystem::path("../shaders/basic.vert");
    auto vert      = compile_shader(vert_path, gl::GL_VERTEX_SHADER);

    auto frag_path = std::filesystem::path("../shaders/basic.frag");
    auto frag      = compile_shader(frag_path, gl::GL_FRAGMENT_SHADER);

    gl::glAttachShader(program, vert);
    gl::glAttachShader(program, frag);

    gl::glBindFragDataLocation(program, 0, "color_output");

    gl::glLinkProgram(program);

    gl::GLboolean is_linked;
    gl::glGetProgramiv(program, gl::GL_LINK_STATUS, &is_linked);
    if (is_linked == gl::GL_FALSE) {
        auto max_length = 1024;
        gl::glGetProgramiv(program, gl::GL_INFO_LOG_LENGTH, &max_length);

        std::vector<char> error_log(max_length);
        gl::glGetProgramInfoLog(program, max_length, &max_length, error_log.data());

        std::string_view error(error_log);
        std::print(stderr, "{}", error);

        gl::glDeleteProgram(program);

        exit(EXIT_FAILURE);
    }

    gl::glDetachShader(program, vert);
    gl::glDetachShader(program, frag);

    gl::glDeleteShader(vert);
    gl::glDeleteShader(frag);

    this->shaders.push_back(program);
}

auto
Engine::frame() -> void
{
    while (this->running) {
        auto loop_start = std::chrono::high_resolution_clock::now();

        // updating camera
        camera.transform();

        // clearing last frame
        gl::glClearColor(0, 0, 0, 1);
        gl::glClearDepth(1);
        gl::glEnable(gl::GL_DEPTH_TEST);
        gl::glDepthFunc(gl::GL_LEQUAL);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

        gl::glUseProgram(this->shaders[0]);
        this->meshes[0].draw();

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

auto Engine::quit() -> void {
    this->running = false;
}