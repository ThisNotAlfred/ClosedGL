#include "closed_gl.hpp"

#include "user_interface.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <print>

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

const auto vert_shader = R"(
#version 460 core
precision highp float;
layout (location = 0) in vec3 a_position;

layout (location = 0) uniform mat4 u_mat_proj;
layout (location = 1) uniform mat4 u_mat_view;
layout (location = 2) uniform mat4 u_mat_model;

void
main()
{
    gl_Position = u_mat_proj * u_mat_view * u_mat_model * vec4(a_position, 1.0);
}
)";

const auto frag_shader = R"(
#version 460 core
precision highp float;
layout (location = 0) out vec4 o_color;

void
main()
{
    o_color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
)";

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

    auto compile_shader = [](const char* str, gl::GLenum type) {
        auto shader = gl::glCreateShader(type);
        gl::glShaderSource(shader, 1, &str, nullptr);
        gl::glCompileShader(shader);
        gl::GLint out = 0;
        gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &out);
        if (out == 1) {
            return shader;
        }
        
        gl::GLint length = 0;
        gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &length);
        char* log = (char*)calloc(1, length);
        gl::glGetShaderInfoLog(shader, length, nullptr, log);
        std::println(stderr, "SHADER ERROR: {}", log);
        assert(false);
    };

    auto vert = compile_shader(vert_shader, gl::GL_VERTEX_SHADER);
    auto frag = compile_shader(frag_shader, gl::GL_FRAGMENT_SHADER);
    auto program = gl::glCreateProgram();
    gl::glAttachShader(program, vert);
    gl::glAttachShader(program, frag);
    gl::glLinkProgram(program);
    gl::GLint out;
    gl::glGetProgramiv(program, gl::GL_LINK_STATUS, &out);
    assert(out == 1);

    auto mat_proj  = glm::mat4(1);
    auto mat_view  = glm::mat4(1);
    auto mat_model = glm::mat4(1);

    // main loop
    unsigned int elapsed_time = 0;
    while (glfwWindowShouldClose(window) == 0) {
        // setting `loop_start` to calculate `elapsed_time` later
        auto loop_start = std::chrono::steady_clock::now();

        // clearing last frame
        gl::glClearColor(0, 0, 0, 0);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

        // TODO(StaticSaga): there's a DSA version of those too
        gl::glUseProgram(program);
        gl::glUniformMatrix4fv(0, 1, false, glm::value_ptr(mat_proj));
        gl::glUniformMatrix4fv(1, 1, false, glm::value_ptr(mat_view));
        gl::glUniformMatrix4fv(2, 1, false, glm::value_ptr(mat_model));
        
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