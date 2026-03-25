#pragma once

#include <Lumina/Core/Base.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include <functional>
#include <vector>

namespace Lumina
{
    class World;

    // Query filter for controlling what shapes are hit
    struct QueryFilter
    {
        uint64_t CategoryBits = UINT64_MAX;
        uint64_t MaskBits = UINT64_MAX;

        [[nodiscard]] b2QueryFilter ToB2() const
        {
            b2QueryFilter f = b2DefaultQueryFilter();
            f.categoryBits = CategoryBits;
            f.maskBits = MaskBits;
            return f;
        }
    };

    // Overlap query callback - return true to continue, false to stop
    using OverlapCallback = std::function<bool(b2ShapeId shapeId)>;

    // Raycast callback - return fraction to continue (1.0 = continue, 0.0 = stop)
    using RayCastCallback = std::function<float(b2ShapeId shapeId, glm::vec2 point, glm::vec2 normal, float fraction)>;

    namespace Query
    {
        // AABB overlap query
        void QueryAABB(
            const World& w,
            glm::vec2 min,
            glm::vec2 max,
            OverlapCallback callback,
            QueryFilter filter = {}
        );

        // Circle overlap query
        void QueryCircle(
            const World& w,
            glm::vec2 center,
            float radius,
            OverlapCallback callback,
            QueryFilter filter = {}
        );

        // Raycast with callback for all hits
        void RayCast(
            const World& w,
            glm::vec2 origin,
            glm::vec2 direction,
            float maxDistance,
            RayCastCallback callback,
            QueryFilter filter = {}
        );
    }
}
