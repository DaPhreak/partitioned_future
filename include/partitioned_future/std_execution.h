#pragma once

#include "algorithm.h"
#include <algorithm>
#include <numeric>

namespace std {
namespace execution {

    struct sequenced_policy {
        using _execution_policy            = int;
        static constexpr bool _parallelize = false;
    };

    inline constexpr sequenced_policy seq{};

    struct parallel_policy {
        using _execution_policy            = int;
        static constexpr bool _parallelize = true;
    };

    inline constexpr parallel_policy par{};

    struct parallel_unsequenced_policy {
        using _execution_policy            = int;
        static constexpr bool _parallelize = true;
    };

    inline constexpr parallel_unsequenced_policy par_unseq{};

    struct unsequenced_policy {
        using _execution_policy            = int;
        static constexpr bool _parallelize = false;
    };

    inline constexpr unsequenced_policy unseq{};

} // execution

template <class T>
using _enable_if_execution_policy_t = typename remove_reference_t<T>::_execution_policy;


template <class _ExPo, class _FwdIt, class _Pr, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] bool all_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::all_of(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred), partitioned_future::defaultTasks());
    } else {
        return std::all_of(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Pr, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] bool any_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::any_of(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred), partitioned_future::defaultTasks());
    } else {
        return std::any_of(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Pr, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] bool none_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::none_of(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred), partitioned_future::defaultTasks());
    } else {
        return std::none_of(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Ty, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] auto count(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, const _Ty& _Val) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::count(std::move(_First), std::move(_Last), _Val, partitioned_future::defaultTasks());
    } else {
        return std::count(std::move(_First), std::move(_Last), _Val);
    }
}

template <class _ExPo, class _FwdIt, class _Pr, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] auto count_if(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::count_if(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred), partitioned_future::defaultTasks());
    } else {
        return std::count_if(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Ty, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _FwdIt find(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, const _Ty& _Val) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::find(std::move(_First), std::move(_Last), _Val, partitioned_future::defaultTasks());
    } else {
        return std::find(std::move(_First), std::move(_Last), _Val);
    }
}

template <class _ExPo, class _FwdIt, class _Pr, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _FwdIt find_if(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::find_if(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred), partitioned_future::defaultTasks());
    } else {
        return std::find_if(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Pr, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _FwdIt find_if_not(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::find_if_not(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred), partitioned_future::defaultTasks());
    } else {
        return std::find_if_not(std::move(_First), std::move(_Last), std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Fn, _enable_if_execution_policy_t<_ExPo> = 0>
void for_each(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Fn&& _Func) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        partitioned_future::for_each(std::move(_First), std::move(_Last), std::forward<_Fn>(_Func), partitioned_future::defaultTasks());
    } else {
        std::for_each(std::move(_First), std::move(_Last), std::forward<_Fn>(_Func));
    }
}

template <class _ExPo, class _FwdIt, class _Diff, class _Fn, _enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt for_each_n(_ExPo&&, _FwdIt _First, _Diff _Count_raw, _Fn&& _Func) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::for_each_n(std::move(_First), std::move(_Count_raw), std::forward<_Fn>(_Func), partitioned_future::defaultTasks());
    } else {
        return std::for_each_n(std::move(_First), std::move(_Count_raw), std::forward<_Fn>(_Func));
    }
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Fn, _enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 transform(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _Fn&& _Func) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::transform(std::move(_First), std::move(_Last), std::move(_Dest), std::forward<_Fn>(_Func), partitioned_future::defaultTasks());
    } else {
        return std::transform(std::move(_First), std::move(_Last), std::move(_Dest), std::forward<_Fn>(_Func));
    }
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Fn, _enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt3 transform(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _FwdIt3 _Dest, _Fn&& _Func) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::transform(std::move(_First1), std::move(_Last1), std::move(_First2), std::move(_Dest), std::forward<_Fn>(_Func), partitioned_future::defaultTasks());
    } else {
        return std::transform(std::move(_First1), std::move(_Last1), std::move(_First2), std::move(_Dest), std::forward<_Fn>(_Func));
    }
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, _enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 copy(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return std::copy(std::move(_First), std::move(_Last), std::move(_Dest));
    } else {
        return std::copy(std::move(_First), std::move(_Last), std::move(_Dest));
    }
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Pr, _enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 copy_if(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return std::copy_if(std::move(_First), std::move(_Last), std::move(_Dest), std::forward<_Pr>(_Pred));
    } else {
        return std::copy_if(std::move(_First), std::move(_Last), std::move(_Dest), std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt1, class _Diff, class _FwdIt2, _enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 copy_n(_ExPo&&, _FwdIt1 _First, _Diff _Count_raw, _FwdIt2 _Dest) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return std::copy_n(std::move(_First), std::move(_Count_raw), std::move(_Dest));
    } else {
        return std::copy_n(std::move(_First), std::move(_Count_raw), std::move(_Dest));
    }
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, _enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 move(_ExPo&&, _FwdIt1 _First, _FwdIt1 _Last, _FwdIt2 _Dest) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return std::move(std::move(_First), std::move(_Last), std::move(_Dest));
    } else {
        return std::move(std::move(_First), std::move(_Last), std::move(_Dest));
    }
}

template <class _ExPo, class _FwdIt, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] auto reduce(_ExPo&& _Exec, _FwdIt _First, _FwdIt _Last) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::reduce(std::move(_First), std::move(_Last), partitioned_future::defaultTasks());
    } else {
        return std::reduce(std::move(_First), std::move(_Last));
    }
}

template <class _ExPo, class _FwdIt, class _Ty, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _Ty reduce(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Ty _Val) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::reduce(std::move(_First), std::move(_Last), std::move(_Val), partitioned_future::defaultTasks());
    } else {
        return std::reduce(std::move(_First), std::move(_Last), std::move(_Val));
    }
}

template <class _ExPo, class _FwdIt, class _Ty, class _BinOp, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _Ty reduce(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Ty _Val, _BinOp&& _Reduce_op) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::reduce(std::move(_First), std::move(_Last), std::move(_Val), std::forward<_BinOp>(_Reduce_op), partitioned_future::defaultTasks());
    } else {
        return std::reduce(std::move(_First), std::move(_Last), std::move(_Val), std::forward<_BinOp>(_Reduce_op));
    }
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, _enable_if_execution_policy_t<_ExPo> = 0>
_Ty transform_reduce(_ExPo&& _Exec, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _Ty _Val) noexcept
{
   if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::transform_reduce(std::move(_First1), std::move(_Last1), std::move(_First2), std::move(_Val), partitioned_future::defaultTasks());
    } else {
        return std::transform_reduce(std::move(_First1), std::move(_Last1), std::move(_First2), std::move(_Val));
    }
}

template <class _ExPo, class _FwdIt, class _Ty, class _BinOp, class _UnaryOp, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _Ty transform_reduce(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Ty _Val, _BinOp&& _Reduce_op, _UnaryOp&& _Transform_op) noexcept
{
   if constexpr (true || remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::transform_reduce(std::move(_First), std::move(_Last), std::move(_Val), std::forward<_BinOp>(_Reduce_op), std::forward<_UnaryOp>(_Transform_op), partitioned_future::defaultTasks());
    } else {
        return std::transform_reduce(std::move(_First), std::move(_Last), std::move(_Val), std::forward<_BinOp>(_Reduce_op), std::forward<_UnaryOp>(_Transform_op));
    }
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Ty, class _BinOp1, class _BinOp2, _enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _Ty transform_reduce(_ExPo&&, _FwdIt1 _First1, _FwdIt1 _Last1, _FwdIt2 _First2, _Ty _Val, _BinOp1 _Reduce_op, _BinOp2 _Transform_op) noexcept
{
   if constexpr (remove_reference_t<_ExPo>::_parallelize) {
        return partitioned_future::transform_reduce(std::move(_First1), std::move(_Last1), std::move(_First2), std::move(_Val), std::forward<_BinOp1>(_Reduce_op), std::forward<_BinOp2>(_Transform_op), partitioned_future::defaultTasks() );
    } else {
        return std::transform_reduce(std::move(_First1), std::move(_Last1), std::move(_First2), std::move(_Val), std::forward<_BinOp1>(_Reduce_op), std::forward<_BinOp2>(_Transform_op));
    }
}

} // namespace std
