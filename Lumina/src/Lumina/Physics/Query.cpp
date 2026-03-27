#include "Query.h"
#include "World.h"
#include "Conversions.h"

namespace Lumina
{
    namespace
    {
        struct OverlapContext
        {
            OverlapCallback* Callback;
        };

        bool OverlapCallbackWrapper(b2ShapeId shapeId, void* context)
        {
            auto* ctx = static_cast<OverlapContext*>(context);
            return (*ctx->Callback)(shapeId);
        }

        struct RaycastContext
        {
            RayCastCallback* Callback;
        };

        float RaycastCallbackWrapper(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
        {
            auto* ctx = static_cast<RaycastContext*>(context);
            return (*ctx->Callback)(shapeId, ToGlm(point), ToGlm(normal), fraction);
        }
    }

    void QueryAABB(
        const World& w,
        glm::vec2 min,
        glm::vec2 max,
        OverlapCallback callback,
        QueryFilter filter)
    {
        b2AABB aabb;
        aabb.lowerBound = ToB2(min);
        aabb.upperBound = ToB2(max);

        OverlapContext ctx{ &callback };
        b2World_OverlapAABB(w.GetNativeId(), aabb, filter.ToB2(), OverlapCallbackWrapper, &ctx);
    }

    void QueryCircle(
        const World& w,
        glm::vec2 center,
        float radius,
        OverlapCallback callback,
        QueryFilter filter)
    {
        // Create a shape proxy for a circle (single point with radius)
        b2Vec2 point = ToB2(center);
        b2ShapeProxy proxy = b2MakeProxy(&point, 1, radius);

        OverlapContext ctx{ &callback };
        b2World_OverlapShape(w.GetNativeId(), &proxy, filter.ToB2(), OverlapCallbackWrapper, &ctx);
    }

    void RayCast(
        const World& w,
        glm::vec2 origin,
        glm::vec2 direction,
        float maxDistance,
        RayCastCallback callback,
        QueryFilter filter)
    {
        glm::vec2 translation = direction * maxDistance;

        RaycastContext ctx{ &callback };
        b2World_CastRay(
            w.GetNativeId(),
            ToB2(origin),
            ToB2(translation),
            filter.ToB2(),
            RaycastCallbackWrapper,
            &ctx
        );
    }
}
