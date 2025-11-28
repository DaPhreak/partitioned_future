#pragma once

#include <future>
#include <thread>
#include <vector>

namespace partitioned_future {

template < class It, class Function >
[[nodiscard]] auto make_futures ( It it, const size_t size, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    using FuncRes      = std::invoke_result_t<std::decay_t<Function>,size_t,std::add_const_t<It>&>;
    using FutureResult = std::conditional_t<std::is_void_v<FuncRes>,void,std::vector<FuncRes>>;
    using Result       = std::vector<std::future<FutureResult>>;
    Result res;

    if ( const size_t taskNr{ std::min( std::max( taskCount, size_t{1} ), size ) } ) {
        const size_t loopdist{ size / taskNr };
        size_t       rest{ size % taskNr };

        res.reserve( taskNr );
        for ( size_t i{}, offset{}; i < taskNr; ++i ) {
            const size_t dist{ loopdist + ( rest ? 1 : 0) };

            res.emplace_back( std::async(
                std::launch::deferred | ( i ? std::launch::async : std::launch::deferred ),
                [ it, offset, dist, function = ( i + 1 == taskNr ? std::forward<Function>( function ) : function ) ]() mutable
                {
                    if constexpr ( !std::is_void_v<FuncRes> ) {
                        FutureResult res;

                        res.reserve( dist );
                        for ( size_t i{}; i < dist; ++i, ++it ) {
                            res.emplace_back( function( offset + i , std::as_const( it ) ) );
                        }
                        return res;
                    } else {
                        for ( size_t i{}; i < dist; ++i, ++it ) {
                            function( offset + i, std::as_const( it ) );
                        }
                    }
                }
            ) );
            std::advance( it, dist );
            offset += dist;
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
void for_each ( It it, It end, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    auto&& futures{ make_futures(
        std::move( it ),
        std::move( end ),
        [ &function ]( const size_t, const It& it )
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
void transform ( It it, It end, OutputIt dest, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
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

} // namespace partitioned_future
