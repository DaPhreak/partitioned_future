#include "partitioned_future/partitioned_future.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <numeric>

namespace {

TEST_CASE("Test for each", "[partitioned_future]")
{
    std::vector<std::string> stringVector{ std::thread::hardware_concurrency() * 2 + 3 };

    partitioned_future::async_for_each(
        stringVector.begin(),
        stringVector.end(),
        [&]( auto&& s )
        {
            s = std::to_string( std::distance( stringVector.data(), &s ) );
        } );
    for ( size_t i{}; i < stringVector.size(); ++i ) {
        REQUIRE( stringVector[i] ==  std::to_string( i ) );
    }
}

TEST_CASE("Test transform", "[partitioned_future]")
{
    std::vector<size_t> intVector( std::thread::hardware_concurrency() * 2 + 3 );
    std::vector<std::string> stringVector( intVector.size() );

    std::iota( intVector.begin(), intVector.end(), 0 );

    partitioned_future::async_transform(
        intVector.begin(),
        intVector.end(),
        stringVector.begin(),
        []( auto&& i )
        {
            return std::to_string( i );
        } );

    for ( size_t i{}; i < stringVector.size(); ++i ) {
        REQUIRE( stringVector[i] ==  std::to_string( i ) );
    }
}

} // ::