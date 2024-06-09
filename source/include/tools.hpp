#pragma once

#include "closed_gl.hpp"

auto read_file_binary(std::filesystem::path& path) -> std::vector<char>;
auto error(int errnum, const char* errmsg) -> void;