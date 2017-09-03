#pragma once

#include <tree/node/definitions.hpp>
#include <tree/node/node.hpp>
#include <tree/node/neighbor.hpp>

//! return the son of a Node which has the smallest absissa (ie, the 1rst
//! one in the son's brotherhood.
//! \param u Node.
//! \note we return a non hashed Node.
template<typename node_type>
inline auto firstSon(node_type const& node)
{
    using zvalue_type = typename node_type::zvalue_type;
    using definition = definitions<node_type::dim, zvalue_type>;
    zvalue_type output = node.value + definition::levelone;
    return output;
}

//! return the son of a Node which has the largestlest absissa (ie, the last
//! one in the son's brotherhood.
//! \param u Node.
//! \note we return a non hashed Node.
template<typename node_type>
inline auto lastSon(node_type const& node)
{
    using zvalue_type = typename node_type::zvalue_type;
    using definition = definitions<node_type::dim, zvalue_type>;
    zvalue_type output{static_cast<zvalue_type>(node.value + definition::levelone)};
    output += definition::XYZbit>>(definition::dim*output.level());
    return output;
}  

//! compute the father of a node
//! \param  node: node.
template<typename node_type>
inline auto father(node_type const& node)
{
    using zvalue_type = typename node_type::zvalue_type;
    using definition = definitions<node_type::dim, zvalue_type>;
    auto level = node.level() - 1;
    return static_cast<zvalue_type>((node.value&definition::AllOnes[level])+(level<<definition::levelshift));
}

//! test if a Node A is an ancestor of a Node X.
//! \param A Node 
//! \param X Node
//! \note each Node is its own ancestor, too.
template<typename node_type>
inline bool isAncestor(node_type A, node_type X)
{
    using zvalue_type = typename node_type::zvalue_type;
    using definition = definitions<node_type::dim, zvalue_type>;
    return ( A.level() <= X.level())
        && ( A.value&definition::maskpos) == (X.value&definition::AllOnes[A.level()]);
}

//! Do 2 Nodes share the same ancestor of a given level ?
//! \param A Node
//! \param B Node
//! \param lv the level.
template<typename node_type>
inline bool shareAncestor(node_type A, node_type B, std::size_t lv)
{
    using zvalue_type = typename node_type::zvalue_type;
    using definition = definitions<node_type::dim, zvalue_type>;
    if( A.level() < lv || B.level() < lv )
        return false;
    else
    {
        auto msk = definition::AllOnes[lv];
        return ( msk & A.value ) == ( msk & B.value );
    }
}


template<typename node_type, typename zvalue_type>
void brothers_impl(node_type const& node,
                   std::array<zvalue_type, 2> &brothers,
                   std::integral_constant<std::size_t, 1>)
{
    std::array<int, 2> stencil{0, 1};
    neighbors(node, brothers, stencil);
}

template<typename node_type, typename zvalue_type>
void brothers_impl(node_type const& node,
                   std::array<zvalue_type, 4> &brothers,
                   std::integral_constant<std::size_t, 2>)
{
    std::array<std::array<int, 2>, 4> stencil{{ {{0, 0}},
                                                {{0, 1}},
                                                {{1, 0}},
                                                {{1, 1}}
                                             }};
    neighbors(node, brothers, stencil);
}

template<typename node_type, typename zvalue_type = typename node_type::zvalue_type>
void brothers_impl(node_type const& node, 
                   std::array<zvalue_type, 8> &brothers,
                   std::integral_constant<std::size_t, 3>)
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
    neighbors(node, brothers, stencil);
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
#ifdef DEBUG
    if(!node.is_minimal())
      throw GenericException("brothers",node,"not minimal");
#endif
    brothers_impl(node, Brothers, std::integral_constant<std::size_t, Node_type::dim>{});
}

