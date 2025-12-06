#pragma once

#include "partitioned_future.h"

namespace partitioned_future {

template < class It, class Function >
void for_each( It it, It end, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    auto&& futures{ make_futures(
        std::move( it ),
        std::move( end ),
        [ &function ]( const It& it )
        {
            function( *it );
        },
        taskCount )
    };

    for ( auto& future: futures ) {
       future.get();
    }
}

template < class It, class OutputIt, class Function >
void transform( It it, It end, OutputIt dest, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    auto&& futures{ make_futures(
        std::move( it ),
        std::move( end ),
        [ &function, dest = std::move( dest ) ]( const size_t id, const It& it )
        {
            *std::next( dest, id ) = function( *it );
        },
        taskCount )
    };

    for ( auto& future: futures ) {
       future.get();
    }
}

template < class It, class Function >
[[nodiscard]] auto transform( It it, It end, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    using FuncRes = std::invoke_result_t<std::decay_t<Function>,decltype(*std::declval<const It&>())>;
    using Result  = std::vector<std::decay_t<FuncRes>>;
    const size_t size{ static_cast<size_t>( std::distance( it, std::move( end ) ) ) };
    Result result(size);

    auto&& futures{ make_futures(
        std::move( it ),
        size,
        [ &function, dest = result.begin() ]( const size_t id, const It& it )
        {
            *std::next( dest, id ) = function( *it );
        },
        taskCount )
    };

    for ( auto& future: futures ) {
       future.get();
    }
    return result;
}

} // namespace partitioned_future
