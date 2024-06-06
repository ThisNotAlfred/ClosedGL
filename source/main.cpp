#define GLM_ENABLE_EXPERIMENTAL

#include "closed_gl.hpp"

#include "mesh.hpp"
#include "user_interface.hpp"

#include <chrono>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <numbers>
#include <print>
#include <glm/gtx/euler_angles.hpp>

static int g_width = 1600, g_height = 900;

auto
error(int errnum, const char* errmsg)
{
    std::cerr << errnum << ": " << errmsg << '\n';
}


auto
framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    g_width  = width;
    g_height = height;
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

    auto* window = glfwCreateWindow(g_width, g_height, "ClosedGL", nullptr, nullptr);
    if (window == nullptr) {
#ifdef _DEBUG
        std::cerr << "failed to creat the window";
#endif
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPos(window, g_width / 2, g_height / 2);

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

    std::cout << "OpenGL Version:  " << glbinding::aux::ContextInfo::version() << '\n'
              << "OpenGL Vendor:   " << glbinding::aux::ContextInfo::vendor() << '\n'
              << "OpenGL Renderer: " << glbinding::aux::ContextInfo::renderer() << '\n';

    auto user_inter = UI(window);

    // TODO make this value not hard coded!
    auto path = std::filesystem::path("DragonAttenuation.glb");
    auto mesh = Mesh(path);

    if (!mesh.create_buffers()) {
        std::println(stderr, "failed to load the mesh\n");
    }

    // here shaders from `shaders/`

    auto vert_path = std::filesystem::path("shaders/vert.slang.spv");
    auto frag_path = std::filesystem::path("shaders/frag.slang.spv");
    auto vert      = compile_shader(vert_path, gl::GL_VERTEX_SHADER);
    auto frag      = compile_shader(frag_path, gl::GL_FRAGMENT_SHADER);
    auto program   = gl::glCreateProgram();

    gl::glAttachShader(program, vert);
    gl::glAttachShader(program, frag);
    gl::glLinkProgram(program);
    gl::GLint out;
    gl::glGetProgramiv(program, gl::GL_LINK_STATUS, &out);
    assert(out == 1);

    auto mat_proj  = glm::mat4(1);
    auto mat_view  = glm::mat4(1);
    auto mat_model = glm::mat4(1);

    glm::vec3 cam_pos   = glm::vec3(0.0F, 0.0F, 10.0F);
    glm::vec3 cam_up    = glm::vec3(0.0F, 1.0F, 0.0F);
    float cam_speed_zoom = 10;
    float cam_speed_yaw  = 1;
    float cam_speed_pitch = 1;

    // main loop
    float dt = 0;
    while (glfwWindowShouldClose(window) == 0) {
        // setting `loop_start` to calculate `dt` later
        auto loop_start = std::chrono::high_resolution_clock::now();

        // clearing last frame
        gl::glClearColor(0, 0, 0, 0);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

        double mousex, mousey;
        glfwGetCursorPos(window, &mousex, &mousey);
        
        auto xoff      = (float)mousex / ((float)g_width/ 2) - 1.0;
        auto yoff      = (float)mousey / ((float)g_height / 2) - 1.0;

        auto yaw       = -xoff * cam_speed_yaw * std::numbers::pi - std::numbers::pi;
        auto pitch     = yoff * cam_speed_pitch * std::numbers::pi;
        auto mat_rot   = glm::eulerAngleYXZ<float>(yaw, pitch, 0.0f);
        auto cam_front = glm::vec3(mat_rot[2]);
        auto cam_up    = glm::vec3(mat_rot[1]);
        
        glm::vec3 cam_right = glm::normalize(glm::cross(cam_front, cam_up));
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam_pos += dt * cam_speed_zoom * cam_front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam_pos -= dt * cam_speed_zoom * cam_front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam_pos -= dt * cam_speed_zoom * cam_right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam_pos += dt * cam_speed_zoom * cam_right;

        mat_view = glm::lookAt(cam_pos, cam_pos + cam_front, cam_up);
        mat_proj = glm::perspective(glm::radians(30.0F), (float)g_width / (float)g_height, 0.1F, 100.0F);
        // TODO(StaticSaga): there's a DSA version of those too
        gl::glUseProgram(program);
        gl::glUniformMatrix4fv(0, 1, false, glm::value_ptr(mat_proj));
        gl::glUniformMatrix4fv(1, 1, false, glm::value_ptr(mat_view));
        gl::glUniformMatrix4fv(2, 1, false, glm::value_ptr(mat_model));

        mesh.draw();

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

        // setting `time_point` and calculating `dt`
        auto loop_end                     = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> time = loop_end - loop_start;
        dt                                = time.count();

#ifdef _DEBUG
        glfwSetWindowTitle(window, std::format("ClosedGL {}fps", static_cast<int>(1.0F / dt)).c_str());
#endif
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    glfwTerminate();

    return 0;
}