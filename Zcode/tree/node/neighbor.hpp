#pragma once

#include <tree/node/direction.hpp>
#include <tree/node/node.hpp>

template<typename Node_type, std::size_t nx>
void neighbors(Node_type const& node,
               std::array<Node_type, nx> &node_array, 
               std::array<int, nx> const& stencilx)
{
    std::size_t index = 0;
    for(auto &sx: stencilx)
    {
        node_array[index++] = (sx<0)? node.minus(direction::x, -sx): node.plus(direction::x, sx);
    }
}

template<typename Node_type, std::size_t nx, std::size_t ny>
void neighbors(Node_type const& node,
               std::array<Node_type, nx*ny> &node_array, 
               std::array<int, nx> const& stencilx, 
               std::array<int, ny> const& stencily)
{
    std::size_t index = 0;
    for(auto &sy: stencily)
    {
        Node_type node_y = (sy<0)?node.minus(direction::y, -sy): node.plus(direction::y, sy);
        for(auto &sx: stencilx)
        {
            if (sx==0 && sy==0)
                node_array[index++] = node.value|Node_type::voidbit;
            else
                node_array[index++] = (sx<0)? node_y.minus(direction::x, -sx): node_y.plus(direction::x, sx);
        }
    }
}

template<typename Node_type, std::size_t ns>
void neighbors(Node_type const& node,
               std::array<Node_type, ns> &node_array, 
               std::array<std::array<int, 2>, ns> const& stencil)
{
    std::size_t index = 0;
    for(auto &s: stencil)
    {
        Node_type node_x = (s[0]<0)?node.minus(direction::x, -s[0]): node.plus(direction::x, s[0]);
        node_array[index++] = (s[1]<0)? node_x.minus(direction::y, -s[1]): node_x.plus(direction::y, s[1]);
    }
}

template<typename Node_type, std::size_t ns>
void neighbors(Node_type const& node,
               std::array<Node_type, ns> &node_array, 
               std::array<std::array<int, 3>, ns> const& stencil)
{
    std::size_t index = 0;
    for(auto &s: stencil)
    {
        Node_type node_x = (s[0]<0)?node.minus(direction::x, -s[0]): node.plus(direction::x, s[0]);
        Node_type node_y = (s[1]<0)?node_x.minus(direction::y, -s[1]): node_x.plus(direction::y, s[1]);
        node_array[index++] = (s[2]<0)? node_y.minus(direction::z, -s[2]): node_y.plus(direction::z, s[2]);
    }
}

template<typename Node_type, std::size_t nx, std::size_t ny, std::size_t nz>
void neighbors(Node_type const& node,
               std::array<Node_type, nx*ny*nz> &node_array, 
               std::array<int, nx> const& stencilx, 
               std::array<int, ny> const& stencily, 
               std::array<int, nz> const& stencilz)
{
    std::size_t index = 0;
    for(auto &sz: stencilz)
    {
        Node_type node_z = (sz<0)?node.minus(direction::z, -sz): node.plus(direction::z, sz);
        for(auto &sy: stencily)
        {
            Node_type node_y = (sy<0)?node_z.minus(direction::y, -sy): node_z.plus(direction::y, sy);
            for(auto &sx: stencilx)
            {
                node_array[index++] = (sx<0)? node_y.minus(direction::x, -sx): node_y.plus(direction::x, sx);
            }
        }
    }
}

//////////////////////////////////////////////////////
//
// BOX
//
//////////////////////////////////////////////////////
template<std::size_t stencil, typename Node_type, typename Node_array>
void boxNeighbors_impl(Node_type const& n, Node_array &node_array, std::integral_constant<std::size_t, 1>)
{
    std::array<int, 2*stencil+1> const s{Stencil_array<stencil>()};
    neighbors(n, node_array, s);
}

template<std::size_t stencil, typename Node_type, typename Node_array>
void boxNeighbors_impl(Node_type const& n, Node_array &node_array, std::integral_constant<std::size_t, 2>)
{
    std::array<int, 2*stencil+1> const s{Stencil_array<stencil>()};
    neighbors(n, node_array, s, s);
}

template<std::size_t stencil, typename Node_type, typename Node_array>
void boxNeighbors_impl(Node_type const& n, Node_array &node_array, std::integral_constant<std::size_t, 3>)
{
    std::array<int, 2*stencil+1> const s{Stencil_array<stencil>()};
    neighbors(n, node_array, s, s, s);
}

//! find a potential neighbor, depending on the position of u.
//! \param  u: node.
//! \param P[] returned list(vector)
template<std::size_t stencil, typename Node_type, typename Node_array>
void boxNeighbors(Node_type const& n, Node_array &node_array)
{
    boxNeighbors_impl<stencil>(n, node_array, std::integral_constant<std::size_t, Node_type::dim>{});
}

//////////////////////////////////////////////////////
//
// STAR
//
//////////////////////////////////////////////////////
template<int stencil, typename Node_type, typename Node_array>
void starNeighbors_impl(Node_type const& n, Node_array &node_array, std::integral_constant<std::size_t, 1>)
{
    std::array<int, 2*stencil> s;
    std::size_t index = 0;
    for (int i=-stencil; i<=stencil; ++i)
        if (i!=0)
        {
            s[index++] = {i, 0};
        }
    neighbors(n, node_array, s);
}

template<int stencil, typename Node_type, typename Node_array>
void starNeighbors_impl(Node_type const& n, Node_array &node_array, std::integral_constant<std::size_t, 2>)
{
    std::array<std::array<int, 2>, 4*stencil> s;
    std::size_t index = 0;
    for (int i=-stencil; i<=stencil; ++i)
        if (i!=0)
        {
            s[index] = {i, 0};
            s[2*stencil + index++] = {0, i};
        }
    neighbors(n, node_array, s);
}

template<int stencil, typename Node_type, typename Node_array>
void starNeighbors_impl(Node_type const& n, Node_array &node_array, std::integral_constant<std::size_t, 3>)
{
    std::array<std::array<int, 3>, 6*stencil> s;
    std::size_t index = 0;
    for (int i=-stencil; i<=stencil; ++i)
        if (i!=0)
        {
            s[index] = {i, 0, 0};
            s[2*stencil + index] = {0, i, 0};
            s[4*stencil + index++] = {0, 0, i};
        }
    neighbors(n, node_array, s);
}

//! find a potential neighbor, depending on the position of u.
//! \param  u: node.
//! \param P[] returned list(vector)
template<int stencil, typename Node_type, typename Node_array>
void starNeighbors(Node_type const& n, Node_array &node_array)
{
    starNeighbors_impl<stencil>(n, node_array, std::integral_constant<std::size_t, Node_type::dim>{});
}
