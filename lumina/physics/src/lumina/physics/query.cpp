#include "query.h"
#include "world.h"
#include "conversions.h"

namespace lumina::physics::query
{
    namespace
    {
        struct overlap_context
        {
            overlap_callback* callback;
        };

        bool overlap_callback_wrapper(b2ShapeId shape_id, void* context)
        {
            auto* ctx = static_cast<overlap_context*>(context);
            return (*ctx->callback)(shape_id);
        }

        struct raycast_context
        {
            ray_cast_callback* callback;
        };

        float raycast_callback_wrapper(b2ShapeId shape_id, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
        {
            auto* ctx = static_cast<raycast_context*>(context);
            return (*ctx->callback)(shape_id, to_glm(point), to_glm(normal), fraction);
        }
    }

    void query_aabb(
        const world& w,
        glm::vec2 min,
        glm::vec2 max,
        overlap_callback callback,
        query_filter filter)
    {
        b2AABB aabb;
        aabb.lowerBound = to_b2(min);
        aabb.upperBound = to_b2(max);

        overlap_context ctx{ &callback };
        b2World_OverlapAABB(w.get_native_id(), aabb, filter.to_b2(), overlap_callback_wrapper, &ctx);
    }

    void query_circle(
        const world& w,
        glm::vec2 center,
        float radius,
        overlap_callback callback,
        query_filter filter)
    {
        b2Circle circle;
        circle.center = to_b2(center);
        circle.radius = radius;

        b2Transform transform = b2Transform_identity;

        overlap_context ctx{ &callback };
        b2World_OverlapCircle(w.get_native_id(), &circle, transform, filter.to_b2(), overlap_callback_wrapper, &ctx);
    }

    void ray_cast(
        const world& w,
        glm::vec2 origin,
        glm::vec2 direction,
        float max_distance,
        ray_cast_callback callback,
        query_filter filter)
    {
        glm::vec2 translation = direction * max_distance;

        raycast_context ctx{ &callback };
        b2World_CastRay(
            w.get_native_id(),
            to_b2(origin),
            to_b2(translation),
            filter.to_b2(),
            raycast_callback_wrapper,
            &ctx
        );
    }
}
