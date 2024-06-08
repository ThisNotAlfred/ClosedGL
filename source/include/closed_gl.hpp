#pragma once

// glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// gl
#include <glbinding/glbinding.h>
#include <glbinding/Version.h>
#include <glbinding/FunctionCall.h>
#include <glbinding/CallbackMask.h>

#include <glbinding/gl/gl.h>
#include <glbinding/getProcAddress.h>

#include <glbinding-aux/Meta.h>
#include <glbinding-aux/debug.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/ValidVersions.h>
#include <glbinding-aux/types_to_string.h>

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

// imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>