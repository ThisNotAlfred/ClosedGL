#include <tools.hpp>

#include <fstream>
#include <filesystem>

auto
read_file_binary(std::filesystem::path& path) -> std::vector<unsigned char>
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    std::vector<unsigned char> file_chars((std::istreambuf_iterator<char>(file)),
                                          std::istreambuf_iterator<char>());

    return file_chars;
}
