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
    REQUIRE(file::write_text(path, content));

    REQUIRE(file::exists(path) == true);

    auto read_result = file::read_text(path);
    REQUIRE(read_result.has_value());
    REQUIRE(read_result.value() == content);
}

TEST_CASE("File write_binary and read_binary roundtrip", "[file]")
{
    std::string path = get_temp_path("binary_test.bin");
    temp_file_cleanup cleanup(path);

    std::vector<uint8_t> data = {0x00, 0x01, 0x02, 0xFF, 0xFE, 0x42};
    REQUIRE(file::write_binary(path, data));

    REQUIRE(file::exists(path) == true);

    auto read_result = file::read_binary(path);
    REQUIRE(read_result.has_value());
    REQUIRE(read_result.value() == data);
}

TEST_CASE("File write_binary with raw pointer", "[file]")
{
    std::string path = get_temp_path("binary_ptr_test.bin");
    temp_file_cleanup cleanup(path);

    uint8_t data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    REQUIRE(file::write_binary(path, data, sizeof(data)));

    REQUIRE(file::exists(path) == true);

    auto read_result = file::read_binary(path);
    REQUIRE(read_result.has_value());
    REQUIRE(read_result.value().size() == 4);
    REQUIRE(read_result.value()[0] == 0xAA);
    REQUIRE(read_result.value()[3] == 0xDD);
}

TEST_CASE("File write_text overwrites existing", "[file]")
{
    std::string path = get_temp_path("overwrite_test.txt");
    temp_file_cleanup cleanup(path);

    REQUIRE(file::write_text(path, "First content"));
    REQUIRE(file::write_text(path, "Second content"));

    auto read_result = file::read_text(path);
    REQUIRE(read_result.has_value());
    REQUIRE(read_result.value() == "Second content");
}

TEST_CASE("File read_text empty file", "[file]")
{
    std::string path = get_temp_path("empty_test.txt");
    temp_file_cleanup cleanup(path);

    REQUIRE(file::write_text(path, ""));

    auto read_result = file::read_text(path);
    REQUIRE(read_result.has_value());
    REQUIRE(read_result.value().empty());
}

TEST_CASE("File read_binary empty file", "[file]")
{
    std::string path = get_temp_path("empty_binary_test.bin");
    temp_file_cleanup cleanup(path);

    REQUIRE(file::write_binary(path, std::vector<uint8_t>{}));

    auto read_result = file::read_binary(path);
    REQUIRE(read_result.has_value());
    REQUIRE(read_result.value().empty());
}

TEST_CASE("File exists returns true after write", "[file]")
{
    std::string path = get_temp_path("exists_test.txt");
    temp_file_cleanup cleanup(path);

    REQUIRE(file::exists(path) == false);

    REQUIRE(file::write_text(path, "test"));

    REQUIRE(file::exists(path) == true);
}

TEST_CASE("File read_text returns error for non-existent file", "[file]")
{
    auto result = file::read_text("this_file_definitely_does_not_exist_12345.txt");
    REQUIRE_FALSE(result.has_value());
    REQUIRE_FALSE(result.error().empty());
}

TEST_CASE("File read_binary returns error for non-existent file", "[file]")
{
    auto result = file::read_binary("this_file_definitely_does_not_exist_12345.bin");
    REQUIRE_FALSE(result.has_value());
    REQUIRE_FALSE(result.error().empty());
}
