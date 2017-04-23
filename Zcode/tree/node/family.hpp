#pragma once

#include <tree/node/node.hpp>
#include <tree/node/neighbor.hpp>

//! return the son of a Node which has the smallest absissa (ie, the 1rst
//! one in the son's brotherhood.
//! \param u Node.
//! \note we return a non hashed Node.
template<std::size_t dim, typename value_type>
inline Node<dim, value_type> firstSon(Node<dim, value_type> const& node)
{
    using node_type = Node<dim, value_type>;
    //return {static_cast<type>(node.value + Node_type::levelone)};
    return node + node_type::levelone;
}

//! return the son of a Node which has the largestlest absissa (ie, the last
//! one in the son's brotherhood.
//! \param u Node.
//! \note we return a non hashed Node.
template<typename Node_type>
inline Node_type lastSon(Node_type const& node)
{
    using type = typename Node_type::type;
    Node_type output{static_cast<type>(node.value + Node_type::levelone)};
    output += Node_type::XYZbit>>(Node_type::dim*output.level());
    return output;
}  

//! compute the father of a node
//! \param  node: node.
template<typename Node_type>
inline Node_type father(Node_type const& node)
{
    using type = typename Node_type::type;
    auto level = node.level() - 1;
    return {static_cast<type>((node.value&Node_type::AllOnes[level])+(level<<Node_type::levelshift))};
}

//! test if a Node A is an ancestor of a Node X.
//! \param A Node 
//! \param X Node
//! \note each Node is its own ancestor, too.
template<typename Node_type>
inline bool isAncestor(Node_type A, Node_type X)
{
    return (A.level()<=X.level())
        && (A.value&Node_type::maskpos) == (X.value&Node_type::AllOnes[A.level()]);
}

//! Do 2 Nodes share the same ancestor of a given level ?
//! \param A Node
//! \param B Node
//! \param lv the level.
template<typename Node_type>
inline bool shareAncestor(Node_type A, Node_type B, std::size_t lv)
{
    if(A.level()<lv||B.level()<lv)
        return false;
    else
    {
        typename Node_type::type msk = Node_type::AllOnes[lv];
        return (msk&A.value)==(msk&B.value);
    }
}


template<typename Node_type, typename Node_array>
void brothers_impl(Node_type const& node, Node_array & Brothers, std::integral_constant<std::size_t, 1>)
{
    std::array<int, 2> stencil{0, 1};
    neighbors(node, Brothers, stencil);
}

template<typename Node_type, typename Node_array>
void brothers_impl(Node_type const& node, Node_array & Brothers, std::integral_constant<std::size_t, 2>)
{
    std::array<std::array<int, 2>, 4> stencil{{ {{0, 0}},
                                                {{0, 1}},
                                                {{1, 0}},
                                                {{1, 1}}
                                             }};
    neighbors(node, Brothers, stencil);
}

template<typename Node_type, typename Node_array>
void brothers_impl(Node_type const& node, Node_array & Brothers, std::integral_constant<std::size_t, 3>)
{
    std::array<std::array<int, 3>, 8> stencil{{ {{0, 0, 0}},
                                                {{0, 1, 0}},
                                                {{1, 0, 0}},
                                                {{1, 1, 0}},
                                                {{0, 0, 1}},
                                                {{0, 1, 1}},
                                                {{1, 0, 1}},
                                                {{1, 1, 1}}
                                             }};
    neighbors(node, Brothers, stencil);
}

//! Make the list of the brothers of a minimal node in a brothers set.
//! \param node the node for which we build the list.
//! \param Brothers the list of brothers.
//! \note node *must* be minimal in his brothers set. *NOT TESTED*, except
//! if DEBUG is set.
//! \note Brothers[0] == node.
//! \note the output array Brothers is ordered. 
template<typename Node_type, typename Node_array>
void brothers(Node_type const& node, Node_array & Brothers)
{
// #ifdef DEBUG
//     if(!node.is_minimal())
//       throw GenericException("brothers",node,"not minimal");
// #endif
    brothers_impl(node, Brothers, std::integral_constant<std::size_t, Node_type::dim>{});
}

