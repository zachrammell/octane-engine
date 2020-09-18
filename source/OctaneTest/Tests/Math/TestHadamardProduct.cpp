#include <OctaneTest/Helper/TestFloat.h>
#include <catch2/catch.hpp>

#include <OctaneEngine/Math.h>

using OctaneTest::t_float;

TEST_CASE("Simple Hadamard Product, XMFLOAT3")
{
  SECTION("[1, 2, 3] ⊙ [2, 3, 4]")
  {
    DirectX::XMFLOAT3 a = {1.0f, 2.0f, 3.0f};
    DirectX::XMFLOAT3 b = {2.0f, 3.0f, 4.0f};

    DirectX::XMFLOAT3 c = Octane::HadamardProduct(a, b);

    REQUIRE((t_float(c.x) == 2.0f));
    REQUIRE((t_float(c.y) == 6.0f));
    REQUIRE((t_float(c.z) == 12.0f));
  }
}
