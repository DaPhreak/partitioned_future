#include "partitioned_future/partitioned_future.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <numeric>

namespace {

TEST_CASE("Test for each", "[partitioned_future]")
{
    constexpr auto make_function{ []( const std::string* begin )
    {
        return [ begin ]( std::string& s )
        {
            s = std::to_string( &s - begin );
        };
    }};
    std::vector<std::string> stringVector1{ std::thread::hardware_concurrency() * 2 + 3 };

    std::for_each(
        stringVector1.begin(),
        stringVector1.end(),
        make_function( stringVector1.data() )
    );
    std::vector<std::string> stringVector2( stringVector1.size() );

    partitioned_future::for_each(
        stringVector2.begin(),
        stringVector2.end(),
        make_function( stringVector2.data() )
    );

    REQUIRE( stringVector1 ==  stringVector2 );
}

TEST_CASE("Test transform", "[partitioned_future]")
{
    constexpr auto function{ []( const size_t i ) { return std::to_string( i ); } };
    std::vector<size_t> intVector( std::thread::hardware_concurrency() * 2 + 3 );
 
    std::iota( intVector.begin(), intVector.end(), 0 );
    std::vector<std::string> stringVector1( intVector.size() );

    std::transform(
        intVector.begin(),
        intVector.end(),
        stringVector1.begin(),
        function
    );
    std::vector<std::string> stringVector2( intVector.size() );

    partitioned_future::transform(
        intVector.begin(),
        intVector.end(),
        stringVector2.begin(),
        function
    );

    REQUIRE( stringVector1 ==  stringVector2 );
}

} // ::