#pragma once
#include <tree/node/cell.hpp>

template <std::size_t dim, typename value_type>
inline Node<dim, value_type> operator+(Node<dim, value_type> const& node1, Node<dim, value_type> const& node2)
{
    return {static_cast<value_type>(node1.value + node2.value)};
}

template <std::size_t dim, typename value_type>
inline Node<dim, value_type> operator+(Node<dim, value_type> const& node, value_type const& value)
{
    return {static_cast<value_type>(node.value + value)};
}

// template <std::size_t dim, typename value_type>
// inline Node<dim, value_type>&& operator+(Node<dim, value_type> const& node, value_type const& value)
// {
//     Node<dim, value_type> node_new{static_cast<value_type>(node.value + value)};
//     return std::move(node_new);
// }

template <std::size_t dim, typename value_type>
inline Node<dim, value_type> operator+(value_type const& value, Node<dim, value_type> const& node)
{
    return node + value;
}
