#pragma once

#include "algorithm.h"
#include <algorithm>

namespace std {
namespace execution {

    struct sequenced_policy {
        using execution_policy            = int;
        static constexpr bool parallelize = false;
    };

    inline constexpr sequenced_policy seq{};

    struct parallel_policy {
        using execution_policy            = int;
        static constexpr bool parallelize = true;
    };

    inline constexpr parallel_policy par{};

    struct parallel_unsequenced_policy {
        using execution_policy            = int;
        static constexpr bool parallelize = true;
    };

    inline constexpr parallel_unsequenced_policy par_unseq{};

    struct unsequenced_policy {
        using execution_policy            = int;
        static constexpr bool parallelize = false;
    };

    inline constexpr unsequenced_policy unseq{};

} // execution

template <class T>
using enable_if_execution_policy_t = typename remove_reference_t<T>::execution_policy;


template <class _ExPo, class _FwdIt, class _Pr, enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] bool all_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::all_of(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    } else {
        return std::all_of(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Pr, enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] bool any_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::any_of(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    } else {
        return std::any_of(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Pr, enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] bool none_of(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::none_of(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    } else {
        return std::none_of(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Ty, enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] auto count(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, const _Ty& _Val) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::count(std::move(_First),std::move(_Last),_Val);
    } else {
        return std::count(std::move(_First),std::move(_Last),_Val);
    }
}

template <class _ExPo, class _FwdIt, class _Pr, enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] auto count_if(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::count_if(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    } else {
        return std::count_if(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Ty, enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _FwdIt find(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, const _Ty& _Val) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::find(std::move(_First),std::move(_Last),_Val);
    } else {
        return std::find(std::move(_First),std::move(_Last),_Val);
    }
}

template <class _ExPo, class _FwdIt, class _Pr, enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _FwdIt find_if(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::find_if(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    } else {
        return std::find_if(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Pr, enable_if_execution_policy_t<_ExPo> = 0>
[[nodiscard]] _FwdIt find_if_not(_ExPo&& _Exec, const _FwdIt _First, const _FwdIt _Last, _Pr&& _Pred) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::find_if_not(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    } else {
        return std::find_if_not(std::move(_First),std::move(_Last),std::forward<_Pr>(_Pred));
    }
}

template <class _ExPo, class _FwdIt, class _Fn, enable_if_execution_policy_t<_ExPo> = 0>
void for_each(_ExPo&&, _FwdIt _First, _FwdIt _Last, _Fn&& _Func) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        partitioned_future::for_each(std::move(_First),std::move(_Last),std::forward<_Fn>(_Func));
    } else {
        std::for_each(std::move(_First),std::move(_Last),std::forward<_Fn>(_Func));
    }
}

template <class _ExPo, class _FwdIt, class _Diff, class _Fn, enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt for_each_n(_ExPo&&, _FwdIt _First, const _Diff _Count_raw, _Fn&& _Func) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::for_each_n(std::move(_First),_Count_raw,std::forward<_Fn>(_Func));
    } else {
        return std::for_each_n(std::move(_First),_Count_raw,std::forward<_Fn>(_Func));
    }
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _Fn, enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt2 transform(_ExPo&&, const _FwdIt1 _First, const _FwdIt1 _Last, _FwdIt2 _Dest, _Fn&& _Func) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::transform(std::move(_First),std::move(_Last),std::move(_Dest),std::forward<_Fn>(_Func));
    } else {
        return std::transform(std::move(_First),std::move(_Last),std::move(_Dest),std::forward<_Fn>(_Func));
    }
}

template <class _ExPo, class _FwdIt1, class _FwdIt2, class _FwdIt3, class _Fn, enable_if_execution_policy_t<_ExPo> = 0>
_FwdIt3 transform(_ExPo&&, const _FwdIt1 _First1, const _FwdIt1 _Last1, const _FwdIt2 _First2, _FwdIt3 _Dest, _Fn&& _Func) noexcept
{
    if constexpr (remove_reference_t<_ExPo>::parallelize) {
        return partitioned_future::transform(std::move(_First1),std::move(_Last1),std::move(_First2),std::move(_Dest),std::forward<_Fn>(_Func));
    } else {
        return std::transform(std::move(_First1),std::move(_Last1),std::move(_First2),std::move(_Dest),std::forward<_Fn>(_Func));
    }
}

} // namespace std
