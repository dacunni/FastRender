cmake_minimum_required(VERSION 3.1)

PROJECT (fastrender_unit_tests)

ADD_EXECUTABLE (unit_test_vector    vector.cpp)
ADD_EXECUTABLE (unit_test_transform transform.cpp)
ADD_EXECUTABLE (unit_test_raysphere raysphere.cpp)
ADD_EXECUTABLE (unit_test_bxdf      bxdf.cpp)

set(UNIT_TEST_LIBS gtest)

TARGET_LINK_LIBRARIES (unit_test_vector    FastRender ${LIBS} ${UNIT_TEST_LIBS})
TARGET_LINK_LIBRARIES (unit_test_transform FastRender ${LIBS} ${UNIT_TEST_LIBS})
TARGET_LINK_LIBRARIES (unit_test_raysphere FastRender ${LIBS} ${UNIT_TEST_LIBS})
TARGET_LINK_LIBRARIES (unit_test_bxdf      FastRender ${LIBS} ${UNIT_TEST_LIBS})


