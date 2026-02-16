#include <catch2/catch_session.hpp>

#include "graphics/graphics_test_fixture.h"

int main(int argc, char* argv[])
{
    // Initialize graphics fixture for GPU tests
    auto& fixture = graphics_fixture::get();
    bool graphics_available = fixture.init();

    if (!graphics_available)
    {
        // This is fine - graphics tests will be skipped
        // Core tests will still run
    }

    // Run all tests
    int result = Catch::Session().run(argc, argv);

    // Cleanup
    if (graphics_available)
    {
        fixture.shutdown();
    }

    return result;
}
