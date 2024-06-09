#include "tools.hpp"

#include <filesystem>
#include <fstream>
#include <print>

auto
read_file_binary(std::filesystem::path& path) -> std::vector<char>
{
    std::ifstream file(path, std::ios::in);

    std::vector<char> file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return file_content;
}

auto
error(int errnum, const char* errmsg) -> void
{
    std::println(stderr, "{}: {}", errnum, errmsg);
}
