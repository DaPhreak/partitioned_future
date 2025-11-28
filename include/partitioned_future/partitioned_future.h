#pragma once

#include <future>
#include <thread>
#include <vector>

namespace partitioned_future {

template < class It, class Function >
[[nodiscard]] auto make_futures ( It it, const size_t size, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    using ItRes        = decltype( *std::declval<It>() );
    using FuncRes      = std::invoke_result_t<std::decay_t<Function>,ItRes>;
    using FutureResult = std::conditional_t<std::is_void_v<FuncRes>,void,std::vector<FuncRes>>;
    using Result       = std::vector<std::future<FutureResult>>;
    Result res;

    if ( const size_t taskNr{ std::min( std::max( taskCount, size_t{1} ), size ) } ) {
        const size_t loopdist{ size / taskNr };
        size_t       rest{ size % taskNr };

        res.reserve( taskNr );
        for ( size_t i{}; i < taskNr; ++i ) {
            const size_t dist{ loopdist + ( rest ? 1 : 0) };

            res.emplace_back( std::async( std::launch::deferred | ( i ? std::launch::async : std::launch::deferred ),
            [ it, dist, &function ]() mutable
            {
                if constexpr ( !std::is_void_v<FuncRes> ) {
                    FutureResult res;

                    res.reserve( dist );
                    for ( size_t i{}; i < dist; ++i, ++it ) {
                        res.emplace_back( function( *it ) );
                    }
                    return res;
                } else {
                    for ( size_t i{}; i < dist; ++i, ++it ) {
                        function( *it );
                    }
                }
            } ) );
            std::advance( it, dist );
            if ( rest ) {
                --rest;
            }
        }
    }
    return res;
}

template < class It, class Function >
[[nodiscard]] auto make_futures ( It it, It end, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    const size_t size{ static_cast<size_t>( std::distance( it, std::move( end ) ) ) };

    return make_futures( std::move( it ), size, std::forward<Function>( function ), taskCount );
}

template < class It, class Function >
void async_for_each ( It it, It end, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    auto&& futures{ make_futures( std::move( it ), std::move( end ),
    [&function]( auto&& v )
    {
        function( std::forward<decltype(v)>(v) );
    }, taskCount ) };

    for ( auto& future: futures ) {
       future.get();
    }
}

template < class It, class OutputIt, class Function >
OutputIt async_transform ( It it, It end, OutputIt dest, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    auto&& futures{ make_futures( std::move( it ), std::move( end ), std::forward<Function>( function ), taskCount ) };

    for ( auto& future: futures ) {
        for ( auto& v: future.get() ) {
            *dest = std::move( v );
            ++dest;
        }
    }
    return std::move( dest );
}

} // namespace partitioned_future
