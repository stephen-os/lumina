#include <catch2/catch_all.hpp>
#include <lumina/core/file.h>
#include <filesystem>

using namespace lumina::core;

// Helper to create a unique temp file path
std::string get_temp_path(const std::string& name)
{
    auto temp_dir = std::filesystem::temp_directory_path();
    return (temp_dir / ("lumina_test_" + name)).string();
}

// Helper to clean up temp files
struct temp_file_cleanup
{
    std::string path;
    temp_file_cleanup(const std::string& p) : path(p) {}
    ~temp_file_cleanup() { std::filesystem::remove(path); }
};

TEST_CASE("File exists returns false for non-existent file", "[file]")
{
    REQUIRE(file::exists("this_file_definitely_does_not_exist_12345.txt") == false);
}

TEST_CASE("File write_text and read_text roundtrip", "[file]")
{
    std::string path = get_temp_path("text_test.txt");
    temp_file_cleanup cleanup(path);

    std::string content = "Hello, World!\nThis is a test.\n";
    file::write_text(path, content);

    REQUIRE(file::exists(path) == true);

    std::string read_content = file::read_text(path);
    REQUIRE(read_content == content);
}

TEST_CASE("File write_binary and read_binary roundtrip", "[file]")
{
    std::string path = get_temp_path("binary_test.bin");
    temp_file_cleanup cleanup(path);

    std::vector<uint8_t> data = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0x42};
    file::write_binary(path, data);

    REQUIRE(file::exists(path) == true);

    std::vector<uint8_t> read_data = file::read_binary(path);
    REQUIRE(read_data == data);
}

TEST_CASE("File write_binary with raw pointer", "[file]")
{
    std::string path = get_temp_path("binary_ptr_test.bin");
    temp_file_cleanup cleanup(path);

    uint8_t data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    file::write_binary(path, data, sizeof(data));

    REQUIRE(file::exists(path) == true);

    std::vector<uint8_t> read_data = file::read_binary(path);
    REQUIRE(read_data.size() == 4);
    REQUIRE(read_data[0] == 0xAA);
    REQUIRE(read_data[3] == 0xDD);
}

TEST_CASE("File write_text overwrites existing", "[file]")
{
    std::string path = get_temp_path("overwrite_test.txt");
    temp_file_cleanup cleanup(path);

    file::write_text(path, "First content");
    file::write_text(path, "Second content");

    std::string content = file::read_text(path);
    REQUIRE(content == "Second content");
}

TEST_CASE("File read_text empty file", "[file]")
{
    std::string path = get_temp_path("empty_test.txt");
    temp_file_cleanup cleanup(path);

    file::write_text(path, "");

    std::string content = file::read_text(path);
    REQUIRE(content.empty());
}

TEST_CASE("File read_binary empty file", "[file]")
{
    std::string path = get_temp_path("empty_binary_test.bin");
    temp_file_cleanup cleanup(path);

    file::write_binary(path, std::vector<uint8_t>{});

    std::vector<uint8_t> data = file::read_binary(path);
    REQUIRE(data.empty());
}

TEST_CASE("File exists returns true after write", "[file]")
{
    std::string path = get_temp_path("exists_test.txt");
    temp_file_cleanup cleanup(path);

    REQUIRE(file::exists(path) == false);

    file::write_text(path, "test");

    REQUIRE(file::exists(path) == true);
}
