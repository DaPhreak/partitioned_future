#include "partitioned_future/algorithm.h"
#include "partitioned_future/partitioned_future.h"
#include "partitioned_future/std_execution.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <numeric>
#include <algorithm>

namespace {

TEST_CASE("Test x_of", "[partitioned_future]")
{
    const std::initializer_list list{ 1, 2, 3, 4, 5, 7, 6, 24, 16, 32 };

    REQUIRE(
        std::all_of(
            std::execution::seq,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i > 0 && i <= *( list.end() - 1); }
        )
    );

    REQUIRE(
        std::all_of(
            std::execution::par,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i > 0 && i <= *( list.end() - 1); }
        )
    );

    REQUIRE(
        ! std::all_of(
            std::execution::seq,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i > 0 && i < *( list.end() - 1); }
        )
    );

    REQUIRE(
        ! std::all_of(
            std::execution::par,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i > 0 && i < *( list.end() - 1); }
        )
    );

    REQUIRE(
        ! std::all_of(
            std::execution::par,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i == 0; }
        )
    );

    REQUIRE(
        std::any_of(
            std::execution::seq,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i == *( list.end() - 1); }
        )
    );

    REQUIRE(
        std::any_of(
            std::execution::par,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i == *( list.end() - 1); }
        )
    );

    REQUIRE(
        std::any_of(
            std::execution::par,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i == 1; }
        )
    );

    REQUIRE(
        ! std::any_of(
            std::execution::seq,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i == *( list.end() - 1) + 1; }
        )
    );

    REQUIRE(
        ! std::any_of(
            std::execution::par,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i == *( list.end() - 1) + 1; }
        )
    );


    REQUIRE(
        std::none_of(
            std::execution::seq,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i > *( list.end() - 1); }
        )
    );

    REQUIRE(
        std::none_of(
            std::execution::par,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i > *( list.end() - 1); }
        )
    );

    REQUIRE(
        ! std::none_of(
            std::execution::seq,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i == *( list.end() - 1); }
        )
    );

    REQUIRE(
        ! std::none_of(
            std::execution::par,
            list.begin(),
            list.end(),
            [&]( const auto& i ) { return i == *( list.end() - 1); }
        )
    );
}

TEST_CASE("Test count", "[partitioned_future]")
{
    const std::initializer_list list{ 
                                        1,
                                        2, 2,
                                        7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                        7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                        7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                        3, 3, 3,
                                        7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                        7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                        7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                        7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                        7, 7, 7, 7, 7, 7, 7 };
    const auto sevens{ std::count( list.begin(), list.end(), 7 ) };

    REQUIRE(
        std::count(
            std::execution::seq,
            list.begin(),
            list.end(),
            1
        ) == 1
    );

    REQUIRE(
        std::count(
            std::execution::par,
            list.begin(),
            list.end(),
            1
        ) == 1
    );

    REQUIRE(
        std::count(
            std::execution::seq,
            list.begin(),
            list.end(),
            4711
        ) == 0
    );

    REQUIRE(
        std::count(
            std::execution::par,
            list.begin(),
            list.end(),
            4711
        ) == 0
    );

    REQUIRE(
        std::count(
            std::execution::seq,
            list.begin(),
            list.end(),
            7
        ) == sevens
    );

    REQUIRE(
        std::count(
            std::execution::par,
            list.begin(),
            list.end(),
            7
        ) == sevens
    );

    REQUIRE(
        std::count_if(
            std::execution::seq,
            list.begin(),
            list.end(),
            []( const auto& i ) { return i >= 3; }
        ) == sevens + 3
    );

    REQUIRE(
        std::count_if(
            std::execution::par,
            list.begin(),
            list.end(),
            []( const auto& i ) { return i >= 3; }
        ) == sevens + 3
    );
}

TEST_CASE("find", "[partitioned_future]")
{
    std::vector<size_t> intVector( 10'001 );
 
    std::iota( intVector.begin(), intVector.end(), 0 );

    REQUIRE(
        std::find(
            std::execution::seq,
            intVector.begin(),
            intVector.end(),
            1
        ) == intVector.begin() + 1
    );

    REQUIRE(
        std::find(
            std::execution::par,
            intVector.begin(),
            intVector.end(),
            1
        ) == intVector.begin() + 1
    );

    REQUIRE(
        partitioned_future::find(
            intVector.begin(),
            intVector.end(),
            9000,
            0
        ) == intVector.begin() + 9000
    );

    REQUIRE(
        std::find_if_not(
            std::execution::seq,
            intVector.begin(),
            intVector.end(),
            [ & ]( const auto& v )
            {
                return v < 900;
            }
        ) == intVector.begin() + 900
    );

    REQUIRE(
        std::find_if_not(
            std::execution::par,
            intVector.begin(),
            intVector.end(),
            [ & ]( const auto& v )
            {
                return v < 900;
            }
        ) == intVector.begin() + 900
    );

}

TEST_CASE("Test for each", "[partitioned_future]")
{
    constexpr auto make_function{ []( const std::string* begin )
    {
        return [ begin ]( std::string& s )
        {
            s = std::move( std::to_string( &s - begin ) += std::string_view{ ". long string does not fit in SSO" } );
        };
    }};
    std::vector<std::string> stringVector1{ partitioned_future::defaultTasks() * 2 + 3 };

    std::for_each(
        std::execution::seq,
        stringVector1.begin(),
        stringVector1.end(),
        make_function( stringVector1.data() )
    );
    std::vector<std::string> stringVector2( stringVector1.size() );

    std::for_each(
        std::execution::par,
        stringVector2.begin(),
        stringVector2.end(),
        make_function( stringVector2.data() )
    );

    REQUIRE( stringVector1 == stringVector2 );

    stringVector1.assign( stringVector1.size(),{} );

    std::for_each_n(
        std::execution::seq,
        stringVector1.begin(),
        stringVector1.size(),
        make_function( stringVector1.data() )
    );

    stringVector2.assign( stringVector2.size(),{} );

    std::for_each_n(
        std::execution::par,
        stringVector2.begin(),
        stringVector2.size(),
        make_function( stringVector2.data() )
    );

    REQUIRE( stringVector1 == stringVector2 );
}

TEST_CASE("Test reduce", "[partitioned_future]")
{
    std::vector<size_t> intVector( 10'001 );
 
    std::iota( intVector.begin(), intVector.end(), 0 );

    {
        const auto a{ std::reduce(
            std::execution::seq,
            intVector.begin(),
            intVector.end()
        ) };

        const auto b{ std::reduce(
            std::execution::par,
            intVector.begin(),
            intVector.end()
        ) };

        REQUIRE ( a == b );
    }

    {
        const auto a{ std::reduce(
            std::execution::seq,
            intVector.begin(),
            intVector.end(),
            size_t{ 22 }
        ) };

        const auto b{ std::reduce(
            std::execution::par,
            intVector.begin(),
            intVector.end(),
            size_t{ 22 }
        ) };

        REQUIRE ( a == b );
    }

    {
        const auto a{ std::reduce(
            std::execution::seq,
            intVector.begin(),
            intVector.end(),
            size_t{ 22 },
            std::plus<>()
        ) };

        const auto b{ std::reduce(
            std::execution::par,
            intVector.begin(),
            intVector.end(),
            size_t{ 22 },
            std::plus<>()
        ) };

        REQUIRE ( a == b );
    }

    {
        const auto a{ std::transform_reduce(
            std::execution::seq,
            intVector.begin(),
            intVector.end(),
            intVector.begin(),
            size_t{ 22 }
        ) };

        const auto b{ std::transform_reduce(
            std::execution::par,
            intVector.begin(),
            intVector.end(),
            intVector.begin(),
            size_t{ 22 }
        ) };

        REQUIRE ( a == b );
    }

    {
        const auto a{ std::transform_reduce(
            std::execution::seq,
            intVector.begin(),
            intVector.end(),
            size_t{ 22 },
            std::plus<>{},
            []( auto&& v ) { return v; }
        ) };

        const auto b{ std::transform_reduce(
            std::execution::par,
            intVector.begin(),
            intVector.end(),
            size_t{ 22 },
            std::plus<>{},
            []( auto&& v ) { return v; }
        ) };
        
        REQUIRE ( a == b );
    }

    {
        const auto a{ std::transform_reduce(
            std::execution::seq,
            intVector.begin(),
            intVector.end(),
            intVector.begin(),
            size_t{ 22 },
            std::plus<>{},
            std::plus<>{}
        ) };

        const auto b{ std::transform_reduce(
            std::execution::par,
            intVector.begin(),
            intVector.end(),
            intVector.begin(),
            size_t{ 22 },
            std::plus<>{},
            std::plus<>{}
        ) };

        REQUIRE ( a == b );
    }
    {
        const std::string hello{ "Hello!" };
        std::vector<std::string> strings{ 100, hello };

        const auto a{ std::reduce(
            std::execution::par,
            std::make_move_iterator( strings.begin() ),
            std::make_move_iterator( strings.end() ),
            std::string{}
        ) };

        REQUIRE ( a.size() == hello.size() * strings.size() );
        REQUIRE ( a.substr( 0, hello.size() ) == hello );
    }

}

TEST_CASE("Test transform", "[partitioned_future]")
{
    constexpr auto function{ []( const size_t i ) { return std::to_string( i ); } };
    std::vector<size_t> intVector( partitioned_future::defaultTasks() * 2 + 3 );
 
    std::iota( intVector.begin(), intVector.end(), 0 );
    std::vector<std::string> stringVector1( intVector.size() );

    std::transform(
        std::execution::seq,
        intVector.begin(),
        intVector.end(),
        stringVector1.begin(),
        function
    );
    std::vector<std::string> stringVector2( intVector.size() );

    std::transform(
        std::execution::par,
        intVector.begin(),
        intVector.end(),
        stringVector2.begin(),
        function
    );

    REQUIRE( stringVector1 == stringVector2 );

    auto stringVector3{ partitioned_future::transform(
        intVector.begin(),
        intVector.end(),
        function )
    };

    REQUIRE( stringVector1 == stringVector3 );

    constexpr auto plus{ []( const size_t i, const auto& s ) { return std::to_string( i ) + s; } };

    std::transform(
        std::execution::seq,
        intVector.begin(),
        intVector.end(),
        stringVector3.begin(),
        stringVector1.begin(),
        plus
    );

    std::transform(
        std::execution::par,
        intVector.begin(),
        intVector.end(),
        stringVector3.begin(),
        stringVector2.begin(),
        plus
    );

    REQUIRE( stringVector1 == stringVector2 );
}

TEST_CASE("Test make_futures", "[partitioned_future]")
{
    std::vector<std::string> stringVector1( partitioned_future::defaultTasks() * 2 + 3 );

    for ( size_t i{}; i < stringVector1.size(); i++ ) {
        stringVector1[i] = std::to_string( i );
    }

    auto futures{ partitioned_future::make_futures(
        stringVector1.begin(),
        stringVector1.end(),
        []( auto&& it ) -> std::string& { return *it; }
        ) };
    std::vector<std::string> stringVector2;

    stringVector2.reserve( stringVector1.size() );
    for ( auto&& future: futures ) {
        for ( auto&& string: future.get() ) {
            stringVector2.emplace_back( std::move( string ) );
        }
    }

    futures = partitioned_future::make_futures(
        stringVector1.begin(),
        stringVector1.end(),
        []( auto&& it, const size_t offset, const size_t chunkSize )
        {
            std::vector<std::string> res{};

            res.reserve( chunkSize );
            for ( size_t i{}; i < chunkSize; ++i, ++it ) {
                res.emplace_back( *it );
            }
            return res;
        }
    );
    stringVector2.clear();

    for ( auto&& future: futures ) {
        for ( auto&& string: future.get() ) {
            stringVector2.emplace_back( std::move( string ) );
        }
    }

    REQUIRE( stringVector1 == stringVector2 );
}

} // ::