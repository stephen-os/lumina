#include <catch2/catch_all.hpp>
#include <Lumina/Core/layer.h>

using namespace lumina::core;

class test_layer : public layer
{
public:
    test_layer(const std::string& name = "test_layer")
        : layer(name) {}

    void on_attach() override { attach_called = true; }
    void on_detach() override { detach_called = true; }
    void on_update(float dt) override { last_dt = dt; update_called = true; }
    void on_render() override { render_called = true; }
    void on_event(event& e) override { event_received = true; }

    bool attach_called = false;
    bool detach_called = false;
    bool update_called = false;
    bool render_called = false;
    bool event_received = false;
    float last_dt = 0.0f;
};

TEST_CASE("Layer default name", "[layer]")
{
    layer l;
    REQUIRE(l.get_name() == "unnamed_layer");
}

TEST_CASE("Layer custom name", "[layer]")
{
    layer l("my_layer");
    REQUIRE(l.get_name() == "my_layer");
}

TEST_CASE("Layer on_attach is callable", "[layer]")
{
    test_layer l;
    REQUIRE(l.attach_called == false);

    l.on_attach();
    REQUIRE(l.attach_called == true);
}

TEST_CASE("Layer on_detach is callable", "[layer]")
{
    test_layer l;
    REQUIRE(l.detach_called == false);

    l.on_detach();
    REQUIRE(l.detach_called == true);
}

TEST_CASE("Layer on_update receives delta time", "[layer]")
{
    test_layer l;

    l.on_update(0.016f);

    REQUIRE(l.update_called == true);
    REQUIRE(l.last_dt == 0.016f);
}

TEST_CASE("Layer on_render is callable", "[layer]")
{
    test_layer l;
    REQUIRE(l.render_called == false);

    l.on_render();
    REQUIRE(l.render_called == true);
}

TEST_CASE("Layer on_event receives events", "[layer]")
{
    test_layer l;
    window_close_event e;

    REQUIRE(l.event_received == false);

    l.on_event(e);
    REQUIRE(l.event_received == true);
}

TEST_CASE("Layer virtual destructor works", "[layer]")
{
    // This test verifies the virtual destructor works correctly
    // by creating a derived layer through a base pointer
    layer* l = new test_layer("derived");
    REQUIRE(l->get_name() == "derived");
    delete l; // Should not leak
}
