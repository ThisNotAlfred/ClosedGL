#include "tools.hpp"

#include <filesystem>
#include <fstream>

auto
read_file_binary(std::filesystem::path& path) -> std::vector<char>
{
    std::ifstream file(path, std::ios::in);

    std::vector<char> file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return file_content;
}
