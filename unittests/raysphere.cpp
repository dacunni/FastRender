#include <gtest/gtest.h>
#include "FastRender.h"

namespace {

TEST(RaySphereIsecTest, OutsideHit) {
    {   // Looking down z at sphere at origin
        auto o = Vector4(0.0, 0.0, 5.0);
        auto d = Vector4(0.0, 0.0, -1.0).normalized();
        Ray ray(o, d);
        Sphere s(Vector4(0.0, 0.0, 0.0), 2.0);
        RayIntersection isect;

        EXPECT_TRUE(s.intersectsAny(ray, EPSILON));
        EXPECT_TRUE(s.intersect(ray, isect));
        EXPECT_FLOAT_EQ(isect.distance, fabs(o.z - s.center.z) - s.radius);
    }
    {   // Looking down x from inside sphere at origin
        auto o = Vector4(-1.0, 0.0, 0.0);
        auto d = Vector4(1.0, 0.0, 0.0).normalized();
        Ray ray(o, d);
        Sphere s(Vector4(0.0, 0.0, 0.0), 3.0);
        RayIntersection isect;

        EXPECT_TRUE(s.intersectsAny(ray, EPSILON));
        EXPECT_TRUE(s.intersect(ray, isect));
        EXPECT_FLOAT_EQ(isect.distance, fabs(o.x - (s.center.z + s.radius)));
    }
}

TEST(RaySphereIsecTest, OutsideMiss) {
    {   // Looking down z at sphere at origin, missing due to y offset 
        auto o = Vector4(0.0, 3.0, 5.0);
        auto d = Vector4(0.0, 0.0, -1.0).normalized();
        Ray ray(o, d);
        Sphere s(Vector4(0.0, 0.0, 0.0), 2.0);
        RayIntersection isect;

        EXPECT_FALSE(s.intersectsAny(ray, EPSILON));
        EXPECT_FALSE(s.intersect(ray, isect));
    }
}

} // namespace

int main(int argc, char **argv) {
      ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

