#include <catch2/catch_all.hpp>
#include <Lumina/Core/uuid.h>
#include <set>

using namespace lumina::core;

TEST_CASE("UUID generates non-zero values", "[uuid]")
{
    uint64_t id = uuid::generate();
    REQUIRE(id != 0);
}

TEST_CASE("UUID generates unique values", "[uuid]")
{
    std::set<uint64_t> ids;

    for (int i = 0; i < 1000; ++i)
    {
        uint64_t id = uuid::generate();
        REQUIRE(ids.find(id) == ids.end()); // Should not already exist
        ids.insert(id);
    }

    REQUIRE(ids.size() == 1000);
}

TEST_CASE("UUID consecutive calls produce different values", "[uuid]")
{
    uint64_t id1 = uuid::generate();
    uint64_t id2 = uuid::generate();
    uint64_t id3 = uuid::generate();

    REQUIRE(id1 != id2);
    REQUIRE(id2 != id3);
    REQUIRE(id1 != id3);
}
