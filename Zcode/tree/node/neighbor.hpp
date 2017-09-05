#pragma once

#include <tree/node/definitions.hpp>
#include <tree/node/direction.hpp>
#include <tree/node/cell.hpp>
#include <tree/node/util.hpp>

#include <type_traits>

template<typename node_type, std::size_t nx, typename zvalue_type>
void neighbors(node_type const& node,
               std::array<zvalue_type, nx> &node_array, 
               std::array<int, nx> const& stencilx)
{
    static_assert(std::is_same<zvalue_type, typename node_type::zvalue_type>::value,
                  "The element type of the output array is not the same as node_type.");
    std::size_t index = 0;
    for(auto &sx: stencilx)
    {
        node_array[index++] = (sx<0)? node.minus(direction::x, -sx): node.plus(direction::x, sx);
    }
}

template<typename node_type, std::size_t nx, std::size_t ny, typename zvalue_type>
void neighbors(node_type const& node,
               std::array<zvalue_type, nx*ny> &node_array, 
               std::array<int, nx> const& stencilx, 
               std::array<int, ny> const& stencily)
{
    static_assert(std::is_same<zvalue_type, typename node_type::zvalue_type>::value,
                  "The element type of the output array is not the same as node_type.");
    using cell_type = Cell<node_type::dim, zvalue_type>;
    using definition = definitions<node_type::dim, zvalue_type>;

    std::size_t index = 0;
    for(auto &sy: stencily)
    {
        cell_type node_y = (sy<0)?node.minus(direction::y, -sy): node.plus(direction::y, sy);
        for(auto &sx: stencilx)
        {
            if (sx==0 && sy==0)
                node_array[index++] = node.value|definition::voidbit;
            else
                node_array[index++] = (sx<0)? node_y.minus(direction::x, -sx): node_y.plus(direction::x, sx);
        }
    }
}

template<typename node_type, std::size_t ns, typename zvalue_type>
void neighbors(node_type const& node,
               std::array<zvalue_type, ns> &node_array, 
               std::array<std::array<int, 2>, ns> const& stencil)
{
    static_assert(std::is_same<zvalue_type, typename node_type::zvalue_type>::value,
                  "The element type of the output array is not the same as node_type.");

    using cell_type = Cell<node_type::dim, zvalue_type>;
    std::size_t index = 0;
    for(auto &s: stencil)
    {
        cell_type node_x = (s[0]<0)?node.minus(direction::x, -s[0]): node.plus(direction::x, s[0]);
        node_array[index++] = (s[1]<0)? node_x.minus(direction::y, -s[1]): node_x.plus(direction::y, s[1]);
    }
}

template<typename node_type, std::size_t ns, typename zvalue_type>
void neighbors(node_type const& node,
               std::array<zvalue_type, ns> &node_array, 
               std::array<std::array<int, 3>, ns> const& stencil)
{
    static_assert(std::is_same<zvalue_type, typename node_type::zvalue_type>::value,
                  "The element type of the output array is not the same as node_type.");
    using cell_type = Cell<node_type::dim, zvalue_type>;
    std::size_t index = 0;
    for(auto &s: stencil)
    {
        cell_type node_x = (s[0]<0)?node.minus(direction::x, -s[0]): node.plus(direction::x, s[0]);
        cell_type node_y = (s[1]<0)?node_x.minus(direction::y, -s[1]): node_x.plus(direction::y, s[1]);
        node_array[index++] = (s[2]<0)? node_y.minus(direction::z, -s[2]): node_y.plus(direction::z, s[2]);
    }
}

template<typename node_type, std::size_t nx, std::size_t ny, std::size_t nz, typename zvalue_type>
void neighbors(node_type const& node,
               std::array<zvalue_type, nx*ny*nz> &node_array, 
               std::array<int, nx> const& stencilx, 
               std::array<int, ny> const& stencily, 
               std::array<int, nz> const& stencilz)
{
    static_assert(std::is_same<zvalue_type, typename node_type::zvalue_type>::value,
                  "The element type of the output array is not the same as node_type.");
    using cell_type = Cell<node_type::dim, zvalue_type>;
    std::size_t index = 0;
    for(auto &sz: stencilz)
    {
        cell_type node_z = (sz<0)?node.minus(direction::z, -sz): node.plus(direction::z, sz);
        for(auto &sy: stencily)
        {
            cell_type node_y = (sy<0)?node_z.minus(direction::y, -sy): node_z.plus(direction::y, sy);
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
template<std::size_t stencil, std::size_t array_size, typename node_type, typename zvalue_type>
void boxNeighbors_impl(node_type const& node, 
                       std::array<zvalue_type, array_size> &neighbors_array, 
                       std::integral_constant<std::size_t, 1>)
{
    static_assert(array_size == ipow(2*stencil+1, node_type::dim),
                  "The array size is not good.");
    std::array<int, 2*stencil+1> const s{Stencil_array<stencil>()};
    neighbors(node, neighbors_array, s);
}

template<std::size_t stencil, std::size_t array_size, typename node_type, typename zvalue_type>
void boxNeighbors_impl(node_type const& node, 
                       std::array<zvalue_type, array_size> &neighbors_array, 
                       std::integral_constant<std::size_t, 2>)
{
    static_assert(array_size == ipow(2*stencil+1, node_type::dim),
                  "The array size is not good.");
    std::array<int, 2*stencil+1> const s{Stencil_array<stencil>()};
    neighbors(node, neighbors_array, s, s);
}

template<std::size_t stencil, std::size_t array_size, typename node_type, typename zvalue_type>
void boxNeighbors_impl(node_type const& node,
                       std::array<zvalue_type, array_size> &neighbors_array, 
                       std::integral_constant<std::size_t, 3>)
{
    static_assert(array_size == ipow(2*stencil+1, node_type::dim),
                  "The array size is not good.");
    std::array<int, 2*stencil+1> const s{Stencil_array<stencil>()};
    neighbors(node, neighbors_array, s, s, s);
}

//! find a potential neighbor, depending on the position of u.
//! \param  u: node.
//! \param P[] returned list(vector)
template<std::size_t stencil, typename node_type, typename neighbors_type>
void boxNeighbors(node_type const& node, neighbors_type &neighbors_array)
{
    boxNeighbors_impl<stencil>(node, neighbors_array, std::integral_constant<std::size_t, node_type::dim>{});
}

//////////////////////////////////////////////////////
//
// STAR
//
//////////////////////////////////////////////////////
template<int stencil, std::size_t array_size, typename node_type, typename zvalue_type>
void starNeighbors_impl(node_type const& node,
                        std::array<zvalue_type, array_size> &neighbors_array, 
                        std::integral_constant<std::size_t, 1>)
{
    static_assert(array_size == 2*stencil*node_type::dim,
                  "The array size is not good.");
    std::array<int, 2*stencil> s;
    std::size_t index = 0;
    for (int i=-stencil; i<=stencil; ++i)
        if (i!=0)
        {
            s[index++] = i;
        }
    neighbors(node, neighbors_array, s);
}

template<int stencil, std::size_t array_size, typename node_type, typename zvalue_type>
void starNeighbors_impl(node_type const& node,
                        std::array<zvalue_type, array_size> &neighbors_array, 
                        std::integral_constant<std::size_t, 2>)
{
    static_assert(array_size == 2*stencil*node_type::dim,
                  "The array size is not good.");    
    std::array<std::array<int, 2>, 4*stencil> s;
    std::size_t index = 0;
    for (int i=-stencil; i<=stencil; ++i)
        if (i!=0)
        {
            s[index] = {i, 0};
            s[2*stencil + index++] = {0, i};
        }
    neighbors(node, neighbors_array, s);
}

template<int stencil, std::size_t array_size, typename node_type, typename zvalue_type>
void starNeighbors_impl(node_type const& node,
                        std::array<zvalue_type, array_size> &neighbors_array, 
                        std::integral_constant<std::size_t, 3>)
{
    static_assert(array_size == 2*stencil*node_type::dim,
                  "The array size is not good.");
    std::array<std::array<int, 3>, 6*stencil> s;
    std::size_t index = 0;
    for (int i=-stencil; i<=stencil; ++i)
        if (i!=0)
        {
            s[index] = {i, 0, 0};
            s[2*stencil + index] = {0, i, 0};
            s[4*stencil + index++] = {0, 0, i};
        }
    neighbors(node, neighbors_array, s);
}

//! find a potential neighbor, depending on the position of u.
//! \param  u: node.
//! \param P[] returned list(vector)
template<std::size_t stencil, typename node_type, typename neighbors_type>
void starNeighbors(node_type const& node, neighbors_type &neighbors_array)
{
    starNeighbors_impl<stencil>(node, neighbors_array, std::integral_constant<std::size_t, node_type::dim>{});
}
