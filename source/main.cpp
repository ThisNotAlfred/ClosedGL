
#include "closed_gl.hpp"

#include "mesh.hpp"
#include "user_interface.hpp"

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <numbers>
#include <print>

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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    glfwSetCursorPos(window, g_width / 2.0F, g_height / 2.0F);

    glfwMakeContextCurrent(window);

    // setting up imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& imgui_io = ImGui::GetIO();
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
    auto path = std::filesystem::path("../bin/Avocado.glb");
    auto mesh = Mesh(path);

    if (!mesh.create_buffers()) {
        std::println(stderr, "failed to load the mesh\n");
    }

    // here shaders from `shaders/`
    auto link_shader = [](gl::GLuint program, gl::GLuint vert, gl::GLuint frag){
        gl::glAttachShader(program, vert);
        gl::glAttachShader(program, frag);


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
    };

    auto program = gl::glCreateProgram();
    gl::glProgramParameteri(program, gl::GL_PROGRAM_SEPARABLE, gl::GL_TRUE);
    auto vert_path = std::filesystem::path("../shaders/basic.vert");
    auto vert      = compile_shader(vert_path, gl::GL_VERTEX_SHADER);
    auto frag_path = std::filesystem::path("../shaders/basic.frag");
    auto frag      = compile_shader(frag_path, gl::GL_FRAGMENT_SHADER);
    gl::glBindFragDataLocation(program, 0, "color_output");
    link_shader(program, vert, frag);

    auto shadow_program = gl::glCreateProgram();
    gl::glProgramParameteri(program, gl::GL_PROGRAM_SEPARABLE, gl::GL_TRUE);
    auto shadow_vert_path = std::filesystem::path("../shaders/shadow.vert");
    auto shadow_vert      = compile_shader(vert_path, gl::GL_VERTEX_SHADER);
    auto shadow_frag_path = std::filesystem::path("../shaders/shadow.frag");
    auto shadow_frag      = compile_shader(frag_path, gl::GL_FRAGMENT_SHADER);
    // gl::glBindFragDataLocation(shadow_program, 0, "depth_output");
    link_shader(shadow_program, shadow_vert, shadow_frag);

    auto mat_proj  = glm::mat4(1);
    auto mat_view  = glm::mat4(1);
    auto mat_model = glm::mat4(1);

    glm::vec3 cam_pos     = glm::vec3(0.0F, 0.0F, 1.0F);
    glm::vec3 cam_up      = glm::vec3(0.0F, 1.0F, 0.0F);
    float cam_speed_zoom  = 1;
    float cam_speed_yaw   = 0.1;
    float cam_speed_pitch = 0.1;

    // main loop
    float total_time = 0;
    float delta_time = 0;

    gl::GLuint fbo, depth_texture;
    gl::glCreateFramebuffers(1, &fbo);
    gl::glCreateTextures(gl::GL_TEXTURE_2D, 1, &depth_texture);
    gl::glTextureStorage2D(depth_texture, 1, gl::GL_DEPTH_COMPONENT32F, 1024, 1024);
    gl::glNamedFramebufferTexture(fbo, gl::GL_DEPTH_ATTACHMENT, depth_texture, 0);

    while (glfwWindowShouldClose(window) == 0) {
        // setting `loop_start` to calculate `dt` later
        auto loop_start = std::chrono::high_resolution_clock::now();


        glfwPollEvents();

        mat_model   = glm::eulerAngleYXZ<float>(total_time * 2.0F, 0, 0);

        double mousex;
        double mousey;
        glfwGetCursorPos(window, &mousex, &mousey);

        auto xoff = (float)mousex / ((float)g_width / 2) - 1.0;
        auto yoff = (float)mousey / ((float)g_height / 2) - 1.0;

        auto yaw       = -xoff * cam_speed_yaw * std::numbers::pi - std::numbers::pi;
        auto pitch     = yoff * cam_speed_pitch * std::numbers::pi;
        auto mat_rot   = glm::eulerAngleYXZ<float>(yaw, pitch, 0.0F);
        auto cam_front = glm::vec3(mat_rot[2]);
        auto cam_up    = glm::vec3(mat_rot[1]);

        glm::vec3 cam_right = glm::normalize(glm::cross(cam_front, cam_up));

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cam_pos += delta_time * cam_speed_zoom * cam_front;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cam_pos -= delta_time * cam_speed_zoom * cam_front;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cam_pos -= delta_time * cam_speed_zoom * cam_right;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cam_pos += delta_time * cam_speed_zoom * cam_right;
        }

        mat_view = glm::lookAt(cam_pos, cam_pos + cam_front, cam_up);
        mat_proj = glm::perspective(glm::radians(30.0F), (float)g_width / (float)g_height, 0.1F, 100.0F);


        // render in shadow framebuffer
        gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, fbo);
        gl::glClearColor(0, 0, 0, 1);
        gl::glClearDepth(1);
        gl::glEnable(gl::GL_DEPTH_TEST);
        gl::glDepthFunc(gl::GL_LEQUAL);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

        // TODO: duplicate in fragment shader
        // what should i set this to?
        
        auto light_dir = glm::vec3(0.0F, -1.0F, 0.0F);

        auto shadow_proj = glm::ortho<float>(-0.25, 0.25, -0.25, 0.25, 0.1F, 10.0F);
        auto shadow_view = glm::lookAt(light_dir, glm::vec3(0,0,0), glm::vec3(0,1,0));
        gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, fbo);    
        gl::glUseProgram(shadow_program);
        gl::glProgramUniformMatrix4fv(shadow_program, 0, 1, false, glm::value_ptr(shadow_proj));
        gl::glProgramUniformMatrix4fv(shadow_program, 1, 1, false, glm::value_ptr(shadow_view));
        gl::glProgramUniformMatrix4fv(shadow_program, 2, 1, false, glm::value_ptr(mat_model));
        mesh.draw();
        
        gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0);

        // rendering to frontbuffer
        gl::glClearColor(0, 0, 0, 1);
        gl::glClearDepth(1);
        gl::glEnable(gl::GL_DEPTH_TEST);
        gl::glDepthFunc(gl::GL_LEQUAL);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);

        gl::glUseProgram(program);
        gl::glProgramUniformMatrix4fv(program, 0, 1, false, glm::value_ptr(mat_proj));
        gl::glProgramUniformMatrix4fv(program, 1, 1, false, glm::value_ptr(mat_view));
        gl::glProgramUniformMatrix4fv(program, 2, 1, false, glm::value_ptr(mat_model));
        gl::glProgramUniform3fv(program, 3, 1, glm::value_ptr(cam_pos));
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
        delta_time                        = time.count();
        total_time += delta_time;

#ifdef _DEBUG
        glfwSetWindowTitle(window, std::format("ClosedGL {}fps", static_cast<int>(1.0F / delta_time)).c_str());
#endif
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    glfwTerminate();

    return 0;
}