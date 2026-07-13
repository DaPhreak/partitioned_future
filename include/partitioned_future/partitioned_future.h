#pragma once

#include <future>
#include <thread>
#include <utility>
#include <vector>

namespace partitioned_future {

[[nodiscard]] inline size_t defaultTasks() noexcept
{
    static const size_t res{ std::thread::hardware_concurrency() };
    return res;
}


template < class It, class Function >
[[nodiscard]] auto make_futures( It it, const size_t size, Function&& function, const size_t taskCount = defaultTasks() );

template < class It, class Function >
[[nodiscard]] auto make_futures( It it, It end, Function&& function, const size_t taskCount = defaultTasks() )
{
    const size_t size{ static_cast<size_t>( std::distance( it, std::move( end ) ) ) };

    return make_futures( std::move( it ), size, std::forward<Function>( function ), taskCount );
}

template < class It, class Function >
decltype(auto) __invoke_it( Function&& function, const size_t id, It&& it )
{
    if constexpr ( std::is_invocable_v<std::decay_t<Function>,size_t,It> ) {
        return function( id, std::forward< It >( it ) );
    } else {
        return function( std::forward< It >( it ) );
    }
}

template < class It, class Function >
[[nodiscard]] auto make_futures ( It it, const size_t size, Function&& function, const size_t taskCount )
{
    using FuncRes      = decltype(__invoke_it( function, size_t{}, it ));
    using FutureResult = std::conditional_t<std::is_void_v<FuncRes>,void,std::vector<std::decay_t<FuncRes>>>;
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
                            res.emplace_back( __invoke_it( function, offset + i , std::as_const( it ) ) );
                        }
                        return res;
                    } else {
                        for ( size_t i{}; i < dist; ++i, ++it ) {
                            __invoke_it( function, offset + i, std::as_const( it ) );
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

} // namespace partitioned_future
