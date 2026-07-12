#pragma once

#include "partitioned_future.h"

#include <atomic>
#include <numeric>
#include <shared_mutex>

namespace partitioned_future {

template < class It, class Function >
void for_each( It it, It end, Function&& function, const size_t taskCount = defaultTasks() );

template < class It, class Pred >
[[nodiscard]] bool all_of( It it, It end, Pred&& pred, const size_t taskCount = defaultTasks() )
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
[[nodiscard]] bool any_of( It it, It end, Pred&& pred, const size_t taskCount = defaultTasks() )
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
[[nodiscard]] bool none_of( It it, It end, Pred&& pred, const size_t taskCount = defaultTasks() )
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
[[nodiscard]] typename std::iterator_traits<It>::difference_type count_if( It it, It end, Pred&& pred, const size_t taskCount = defaultTasks() )
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
[[nodiscard]] typename std::iterator_traits<It>::difference_type count( It it, It end, const T& value, const size_t taskCount = defaultTasks() )
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
[[nodiscard]] It find_if( It it, It end, Pred&& pred, const size_t taskCount = defaultTasks() )
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
[[nodiscard]] It find_if_not( It it, It end, Pred&& pred, const size_t taskCount = defaultTasks() )
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
[[nodiscard]] It find( It it, It end, const T& value, const size_t taskCount = defaultTasks() )
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
It for_each_n( It it, const Diff count, Function&& function, const size_t taskCount = defaultTasks() )
{
    if ( count < 1 ) {
        return it;
    }
    It result{ std::next( it, count ) };

    for_each( std::move( it ), result, std::forward<Function>( function ), taskCount );
    return result;
}

template < class It, class OutputIt, class Function >
OutputIt transform( It it, It end, OutputIt dest, Function&& function, const size_t taskCount = defaultTasks() )
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
OutputIt transform( It it, It end, It2 it2, OutputIt dest, Function&& function, const size_t taskCount = defaultTasks() )
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
[[nodiscard]] auto transform( It it, It end, Function&& function, const size_t taskCount = defaultTasks() )
{
    using FuncRes = std::invoke_result_t<std::decay_t<Function>,decltype( *std::declval<const It&>() )>;
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

template < class It1, class It2, class T, class BinOp1, class BinOp2>
[[nodiscard]] T transform_reduce( It1 it, It1 end, It2 it2, T init, BinOp1&& reduceOp, BinOp2&& transformOp, const size_t taskCount = defaultTasks() )
{
    const size_t size{ static_cast<size_t>( std::distance( it , end ) ) };

    if ( size < 2 || taskCount < 2 ) {
        for ( ; it != end; ++it, ++it2 ) {
            init = reduceOp( std::move( init ), transformOp( *it, *it2 ) );
        }
        return init;
    }
    T* initP{ &init };
    const size_t sizeMid{ ( size / 2) + ( size % 2 ) };
    const bool dummy{};

    auto&& v{ transform( &dummy, &dummy + sizeMid,
        [&]( const bool& curr )
        {
            const auto id{ 2 * std::distance( &dummy, &curr ) };
            const It1 a{ std::next( it, id) };
            const It2 b{ std::next( it2, id) };

            if ( id + 1 < size ) {
                return reduceOp( transformOp( *std::next( a, 1 ), *std::next( b, 1 ) ), transformOp( *a, *b ) );
            }
            return reduceOp( std::move( *std::exchange( initP, nullptr ) ), transformOp( *a, *b ) );
        },
        taskCount
    ) };

    for ( size_t n{ sizeMid }; n > 1; n = ( n / 2 ) + ( n % 2 ) ) {
        const size_t mid{ n / 2 };

        for_each( &dummy, &dummy + mid + ( initP ? ( n % 2 ) :0 ),
            [&]( const bool& curr )
            {
                const auto id{ std::distance( &dummy, &curr ) };

                if ( id < mid ) {
                    v[ id ] = reduceOp( std::move( v[ id ] ), std::move( v[ n - ( id + 1 ) ] ) );
                } else {
                    v[ id ] = reduceOp( std::move( *std::exchange( initP, nullptr ) ), std::move( v[ id ] ) );
                }
            },
            taskCount
        );
    }

    if ( initP ) {
        return reduceOp( std::move( *initP ), std::move( v[ 0 ] ) );
    }
    return std::move( v[ 0 ] );
}

template < class It1, class It2, class T>
[[nodiscard]] T transform_reduce( It1 it, It1 end, It2 it2, T init, const size_t taskCount = defaultTasks() )
{
    return transform_reduce(
        std::move( it ),
        std::move( end ),
        std::move( it2 ),
        std::move( init ),
        std::plus<>(),
        std::multiplies<>(),
        taskCount
    );
}

template < class It, class T, class BinOp, class UnaryOp>
[[nodiscard]] T transform_reduce( It it, It end, T init, BinOp&& reduceOp, UnaryOp&& transformOp, const size_t taskCount = defaultTasks() )
{
    using TransformRes = std::invoke_result_t< std::decay_t<UnaryOp>, decltype( *it ) >;

    return transform_reduce(
        it,
        std::move( end ),
        it,
        std::move( init ),
        std::forward<BinOp>( reduceOp ),
        [&]( auto&& v, auto&& ) -> TransformRes
        {
            return transformOp( v );
        },
        taskCount
    );
}

template < class It, class T, class BinOp >
[[nodiscard]] T reduce( It it, It end, T init, BinOp&& reduceOp, const size_t taskCount = defaultTasks() )
{
    using TransformRes = decltype( *it );

    return transform_reduce(
        it,
        std::move( end ),
        it,
        std::move( init ),
        std::forward<BinOp>( reduceOp ),
        []( auto&& v, auto&& ) -> TransformRes
        {
            return v;
        },
        taskCount
    );
}

template < class It, class T>
[[nodiscard]] T reduce( It it, It end, T init, const size_t taskCount = defaultTasks() )
{
    return reduce( std::move( it ), std::move( end ), std::move( init ), std::plus<>{}, taskCount );
}

template < class It>
[[nodiscard]] auto reduce( It it, It end, const size_t taskCount = defaultTasks() )
{
    using T = std::iterator_traits<It>::value_type;
    return reduce( std::move( it ), std::move( end ), T{}, std::plus<>{}, taskCount );
}

} // namespace partitioned_future
