#include "closed_gl.hpp"

#include "engine.hpp"
#include "input.hpp"
#include "mesh.hpp"
#include "tools.hpp"

#include <chrono>
#include <filesystem>
#include <print>

static int g_width = 1600, g_height = 900;

auto
main() -> int
{
    glfwSetErrorCallback(error);

    if (glfwInit() == 0) {
        return 1;
    }

    // setting the opengl version to 4.6 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // making window unresizeable
    // TODO make window ressizeable again
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    auto* window = glfwCreateWindow(g_width, g_height, "ClosedGL", nullptr, nullptr);
    if (window == nullptr) {
#ifdef _DEBUG
        std::print(stderr, "failed to creat the window");
#endif
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, nullptr);
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

    std::println(stderr, "OPENGL VERSION: {}\nOPENGL VENDOR: {}\nOPENGL RENDERER: {}",
                 glbinding::aux::ContextInfo::version().toString(), glbinding::aux::ContextInfo::vendor(),
                 glbinding::aux::ContextInfo::renderer());

    // engine starts here
    auto engine = Engine(window);
    auto input  = Input(window, engine);

    // initializing engine
    engine.init();

    // core
    while (glfwWindowShouldClose(window) == 0) {
        input.check_for_input();
        engine.frame();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}