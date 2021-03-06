#include <gtest/gtest.h>
#include "FastRender.h"

namespace {

TEST(BasicTransformTest, Translation) {
    Transform t;
    Vector4 a, b;

    a.set(5, 49, -45);
    b.set(12, -9, 492);
    t = makeTranslation(b);
    EXPECT_EQ(a + b, mult(t.fwd, a));
    EXPECT_EQ(a - b, mult(t.rev, a));
}

TEST(BasicTransformTest, Rotation) {
    Transform t;
    Vector4 a, b;

    a.set(5, 49, -45);
    t = makeRotation(M_PI / 2.0, Vector4(1, 0, 0));
    EXPECT_FLOAT_EQ(dot(mult(t.fwd, a), Vector4(a.x, -a.z, a.y)), a.magnitude_sq());
    t = makeRotation(M_PI / 2.0, Vector4(-1, 0, 0));
    EXPECT_FLOAT_EQ(dot(mult(t.fwd, a), Vector4(a.x, a.z, -a.y)), a.magnitude_sq());
    t = makeRotation(-M_PI / 2.0, Vector4(1, 0, 0));
    EXPECT_FLOAT_EQ(dot(mult(t.fwd, a), Vector4(a.x, a.z, -a.y)), a.magnitude_sq());

    t = makeRotation(M_PI / 2.0, Vector4(0, 1, 0));
    EXPECT_FLOAT_EQ(dot(mult(t.fwd, a), Vector4(a.z, a.y, -a.x)), a.magnitude_sq());
    t = makeRotation(M_PI / 2.0, Vector4(0, -1, 0));
    EXPECT_FLOAT_EQ(dot(mult(t.fwd, a), Vector4(-a.z, a.y, a.x)), a.magnitude_sq());
    t = makeRotation(-M_PI / 2.0, Vector4(0, 1, 0));
    EXPECT_FLOAT_EQ(dot(mult(t.fwd, a), Vector4(-a.z, a.y, a.x)), a.magnitude_sq());

    t = makeRotation(M_PI / 2.0, Vector4(0, 0, 1));
    EXPECT_FLOAT_EQ(dot(mult(t.fwd, a), Vector4(-a.y, a.x, a.z)), a.magnitude_sq());
    t = makeRotation(M_PI / 2.0, Vector4(0, 0, -1));
    EXPECT_FLOAT_EQ(dot(mult(t.fwd, a), Vector4(a.y, -a.x, a.z)), a.magnitude_sq());
    t = makeRotation(-M_PI / 2.0, Vector4(0, 0, 1));
    EXPECT_FLOAT_EQ(dot(mult(t.fwd, a), Vector4(a.y, -a.x, a.z)), a.magnitude_sq());
}

TEST(BasicTransformTest, Scaling) {
    Transform t;
    Vector4 a, b;

    a.set(5, 49, -45);
    float sx = 3.2, sy = -4.3, sz = 237.0;
    t = makeScaling(sx, sy, sz);
    b.set(a.x * sx, a.y * sy, a.z * sz);
    EXPECT_EQ(b, mult(t.fwd, a));
    b.set(a.x / sx, a.y / sy, a.z / sz);
    EXPECT_EQ(b, mult(t.rev, a));

    float s = 4.5;
    t = makeScaling(s);
    b.set(a.x * s, a.y * s, a.z * s);
    EXPECT_EQ(b, mult(t.fwd, a));
    b.set(a.x / s, a.y / s, a.z / s);
    EXPECT_EQ(b, mult(t.rev, a));
}

} // namespace

int main(int argc, char **argv) {
      ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

