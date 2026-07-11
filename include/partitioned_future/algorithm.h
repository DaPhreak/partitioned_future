#pragma once

#include "partitioned_future.h"

#include <atomic>
#include <shared_mutex>

namespace partitioned_future {

template < class It, class Function >
void for_each( It it, It end, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() );

template < class It, class Pred >
[[nodiscard]] bool all_of( It it, It end, Pred&& pred, const size_t taskCount = std::thread::hardware_concurrency() )
{
    if ( it == end ) {
        return true;
    }
    std::atomic<bool> result{ true };

    for_each(
        std::move( it ),
        std::move( end ),
        [ & ]( const auto& v )
        {
            if (
                result.load( std::memory_order_relaxed ) &&
                ! pred( v ) ) {
                result.store( false );
            }
        },
        taskCount
    );
    return result.load();
}

template < class It, class Pred >
[[nodiscard]] bool any_of( It it, It end, Pred&& pred, const size_t taskCount = std::thread::hardware_concurrency() )
{
    if ( it == end ) {
        return false;
    }
    std::atomic<bool> result{ false };

    for_each(
        std::move( it ),
        std::move( end ),
        [ & ]( const auto& v )
        {
            if (
                ! result.load( std::memory_order_relaxed ) &&
                pred( v ) ) {
                result.store( true );
            }
        },
        taskCount
    );
    return result.load();
}

template < class It, class Pred >
[[nodiscard]] bool none_of( It it, It end, Pred&& pred, const size_t taskCount = std::thread::hardware_concurrency() )
{
    if ( it == end ) {
        return true;
    }
    std::atomic<bool> result{ true };

    for_each(
        std::move( it ),
        std::move( end ),
        [ & ]( const auto& v )
        {
            if (
                result.load( std::memory_order_relaxed ) &&
                pred( v ) ) {
                result.store( false );
            }
        },
        taskCount
    );
    return result.load();
}

template < class It, class Pred >
[[nodiscard]] typename std::iterator_traits<It>::difference_type count_if( It it, It end, Pred&& pred, const size_t taskCount = std::thread::hardware_concurrency() )
{
    if ( it == end ) {
        return {};
    }
    std::atomic<std::iterator_traits<It>::difference_type> result{};

    for_each(
        std::move( it ),
        std::move( end ),
        [ & ]( const auto& v )
        {
            if ( pred( v ) ) {
                ++result;
            }
        },
        taskCount
    );
    return result.load();
}

template < class It, class T >
[[nodiscard]] typename std::iterator_traits<It>::difference_type count( It it, It end, const T& value, const size_t taskCount = std::thread::hardware_concurrency() )
{
    return count_if(
        std::move( it ),
        std::move( end ),
        [ & ]( const auto& v )
        {
            return v == value;
        },
        taskCount
    );
}

template < class It, class Pred >
[[nodiscard]] It find_if( It it, It end, Pred&& pred, const size_t taskCount = std::thread::hardware_concurrency() )
{
    if ( it == end ) {
        return it;
    }
    const size_t size{ static_cast<size_t>( std::distance( it, end ) ) };

    if ( size < 2 || taskCount < 2 ) {
        for ( ; it != end; ++it ) {
            if ( pred( *it ) ) {
                return it;
            }
        }
        return end;
    }
    size_t result{ size };
    std::shared_mutex mutex;

    auto&& futures{ make_futures(
        it,
        size,
        [ & ]( const size_t id, const It& it )
        {
            if ( std::shared_lock l{ mutex }; result < id ) {
                return;
            }
            if ( ! pred( *it ) ) {
                return;
            }
            if ( std::lock_guard l{ mutex }; id < result ) {
                result = id;
            }
        },
        taskCount )
    };

    for ( auto& future: futures ) {
       future.get();
    }
    std::advance( it, result );
    return it;
}

template < class It, class Pred >
[[nodiscard]] It find_if_not( It it, It end, Pred&& pred, const size_t taskCount = std::thread::hardware_concurrency() )
{
    return find_if(
        std::move( it ),
        std::move( end ),
        [ & ]( const auto& v )
        {
            return ! pred( v );
        },
        taskCount
    );
}

template < class It, class T >
[[nodiscard]] It find( It it, It end, const T& value, const size_t taskCount = std::thread::hardware_concurrency() )
{
    return find_if(
        std::move( it ),
        std::move( end ),
        [ & ]( const auto& v )
        {
            return v == value;
        },
        taskCount
    );
}

template < class It, class Function >
void for_each( It it, It end, Function&& function, const size_t taskCount )
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

template < class It, class Diff, class Function >
It for_each_n( It it, const Diff count, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    if ( count < 1 ) {
        return it;
    }
    It result{ std::next( it, count ) };

    for_each( std::move( it ), result, std::forward<Function>( function ), taskCount );
    return result;
}

template < class It, class OutputIt, class Function >
OutputIt transform( It it, It end, OutputIt dest, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    const size_t size{ static_cast<size_t>( std::distance( it, std::move( end ) ) ) };
    OutputIt result{ std::next( dest, size ) };

    auto&& futures{ make_futures(
        std::move( it ),
        size,
        [ &function, dest = std::move( dest ) ]( const size_t id, const It& it )
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

template < class It, class It2, class OutputIt, class Function >
OutputIt transform( It it, It end, It2 it2, OutputIt dest, Function&& function, const size_t taskCount = std::thread::hardware_concurrency() )
{
    const size_t size{ static_cast<size_t>( std::distance( it, std::move( end ) ) ) };
    OutputIt result{ std::next( dest, size ) };

    auto&& futures{ make_futures(
        std::move( it ),
        size,
        [ &function, it2 = std::move( it2 ), dest = std::move( dest ) ]( const size_t id, const It& it )
        {
            *std::next( dest, id ) = function( *it, *std::next( it2, id ) );
        },
        taskCount )
    };

    for ( auto& future: futures ) {
       future.get();
    }
    return result;
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
