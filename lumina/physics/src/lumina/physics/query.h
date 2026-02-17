#pragma once

#include <lumina/core/base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <functional>
#include <vector>

namespace lumina::physics
{
    class world;

    // Query filter for controlling what shapes are hit
    struct query_filter
    {
        uint64_t category_bits = UINT64_MAX;
        uint64_t mask_bits = UINT64_MAX;

        [[nodiscard]] b2QueryFilter to_b2() const
        {
            b2QueryFilter f = b2DefaultQueryFilter();
            f.categoryBits = category_bits;
            f.maskBits = mask_bits;
            return f;
        }
    };

    // Overlap query callback - return true to continue, false to stop
    using overlap_callback = std::function<bool(b2ShapeId shape_id)>;

    // Raycast callback - return fraction to continue (1.0 = continue, 0.0 = stop)
    using ray_cast_callback = std::function<float(b2ShapeId shape_id, glm::vec2 point, glm::vec2 normal, float fraction)>;

    namespace query
    {
        // AABB overlap query
        void query_aabb(
            const world& w,
            glm::vec2 min,
            glm::vec2 max,
            overlap_callback callback,
            query_filter filter = {}
        );

        // Circle overlap query
        void query_circle(
            const world& w,
            glm::vec2 center,
            float radius,
            overlap_callback callback,
            query_filter filter = {}
        );

        // Raycast with callback for all hits
        void ray_cast(
            const world& w,
            glm::vec2 origin,
            glm::vec2 direction,
            float max_distance,
            ray_cast_callback callback,
            query_filter filter = {}
        );
    }
}
