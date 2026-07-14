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
constexpr size_t function_signature_v{ std::is_invocable_v<std::decay_t<Function>,It,size_t,size_t> ? 2 : std::is_invocable_v<std::decay_t<Function>,It,size_t> ? 1 : std::is_invocable_v<std::decay_t<Function>,It> ? 0 : std::numeric_limits<size_t>::max() };

template < class It, class Function >
decltype(auto) __invoke_it( It&& it, Function&& function, const size_t offset, const size_t chunkSizeOrId )
{
    if constexpr ( constexpr auto signature{ function_signature_v<It,Function> }; signature == 0 ) {
        return function( std::forward< It >( it ) );
    } else if constexpr ( signature == 1 ) {
        return function( std::forward< It >( it ), offset + chunkSizeOrId );
    } else if constexpr ( signature == 2 ) {
        return function( std::forward< It >( it ), offset, chunkSizeOrId );
    } else {
        static_assert( false, "Function signature not supported!" );
    }
}

template < class It, class Function >
[[nodiscard]] auto make_futures ( It it, const size_t size, Function&& function, const size_t taskCount )
{
    using FuncRes      = decltype(__invoke_it( it, function, size_t{}, size_t{} ));
    using FutureResult = std::conditional_t<std::is_void_v<FuncRes>,void,std::conditional_t<function_signature_v<It,Function> == 2,std::decay_t<FuncRes>,std::vector<std::decay_t<FuncRes>>>>;
    using Result       = std::vector<std::future<FutureResult>>;
    Result res;

    if ( const size_t taskNr{ std::min( std::max( taskCount, size_t{ 1 } ), size ) } ) {
        const size_t chunkFloor{ size / taskNr };
        const size_t restId{ taskNr - ( size % taskNr ) };

        res.reserve( taskNr );
        for ( size_t i{}, offset{}; i < taskNr; ++i ) {
            const size_t chunkSize{ chunkFloor + ( i >= restId ? 1 : 0 ) };

            res.emplace_back( std::async(
                std::launch::deferred | ( i ? std::launch::async : std::launch::deferred ),
                [ it, function, offset, chunkSize ]() mutable
                {
                    if constexpr ( function_signature_v<It,Function> == 2 ) {
                        return __invoke_it( std::move( it ), std::move( function ), offset, chunkSize );
                    } else {
                        if constexpr ( !std::is_void_v<FutureResult> ) {
                            FutureResult res;

                            res.reserve( chunkSize );
                            for ( size_t i{}; i < chunkSize; ++i, ++it ) {
                                res.emplace_back( __invoke_it( std::as_const( it ), function, offset, i ) );
                            }
                            return res;
                        } else {
                            for ( size_t i{}; i < chunkSize; ++i, ++it ) {
                                __invoke_it( std::as_const( it ), function, offset, i );
                            }
                        }
                    }
                }
            ) );
            std::advance( it, chunkSize );
            offset += chunkSize;
        }
    }
    return res;
}

} // namespace partitioned_future
