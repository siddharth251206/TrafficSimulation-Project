#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

TEST_CASE("Placeholder Test", "[basic]") {
    REQUIRE(0 == 0);
    REQUIRE(2 + 2 == 4);
}

