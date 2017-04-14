#pragma once
#include <tree/node/util.hpp>

template<typename node_type>
constexpr std::array<node_type, 2> zcurve_impl(node_type Xbit, node_type Ybit, node_type Zbit, std::integral_constant<int, 1> const&)
{
    return {0, Xbit};
};

template<typename node_type>
constexpr std::array<node_type, 4> zcurve_impl(node_type Xbit, node_type Ybit, node_type Zbit, std::integral_constant<int, 2> const&)
{
    return {0, 
            Ybit, 
            Xbit, 
            static_cast<node_type>(Xbit + Ybit)
            };
};

template<typename node_type>
constexpr std::array<node_type, 8> zcurve_impl(node_type Xbit, node_type Ybit, node_type Zbit, std::integral_constant<int, 3> const&)
{
    return {0, 
            Ybit, 
            Xbit, 
            static_cast<node_type>(Xbit + Ybit),
            Zbit, 
            static_cast<node_type>(Ybit + Zbit),
            static_cast<node_type>(Xbit + Zbit), 
            static_cast<node_type>(Xbit + Ybit + Zbit)
            };
};

template<std::size_t dim, typename node_type>
constexpr auto zcurve(node_type Xbit, node_type Ybit, node_type Zbit)
{
    return zcurve_impl(Xbit, Ybit, Zbit, std::integral_constant<int, dim>{});
}



