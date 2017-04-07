#pragma once
#include <iostream>

#include <tree/definitions.hpp>
#include <tree/direction.hpp>

template <std::size_t dim, typename node_type=std::size_t>
struct Node: public definitions<dim, node_type>
{
    using definitions<dim, node_type>::size;
    using definitions<dim, node_type>::nlevels;
    using definitions<dim, node_type>::levelshift;
    using definitions<dim, node_type>::levelmask;
    using definitions<dim, node_type>::levelzone;
    using definitions<dim, node_type>::levelone;
    using definitions<dim, node_type>::maskpos;
    using definitions<dim, node_type>::voidbit;
    using definitions<dim, node_type>::FreeBitsPart;
    using definitions<dim, node_type>::Xbit;
    using definitions<dim, node_type>::Ybit;
    using definitions<dim, node_type>::Zbit;
    using definitions<dim, node_type>::XYZbit;
    using definitions<dim, node_type>::XMask;
    using definitions<dim, node_type>::YMask;
    using definitions<dim, node_type>::ZMask;
    using definitions<dim, node_type>::Ones;
    using definitions<dim, node_type>::AllOnes;
    
    node_type value;

    Node() = default;
    Node(const Node&) = default;

    Node(node_type i):value{i}{}

    inline auto _get_dec(direction d) const
    {
        node_type bit = 0;
        node_type mask = XMask;
        switch(d)
        {
            case (direction::x):
                bit = Xbit >> dim*level(); break;
            case (direction::y):
                bit = Ybit >> dim*level(); mask >>= 1; break;
            case (direction::z):
                bit = Zbit >> dim*level(); mask >>= 2; break;
        }
        return std::pair<node_type, node_type>{bit, mask};
    }
    inline Node plus(direction d, std::size_t stencil=1) const
    {
        auto dummy = _get_dec(d);
        auto bit = dummy.first;
        auto mask = dummy.second;
        node_type tmp = (maskpos - mask);
        node_type keep = (value&tmp) + (value&levelzone);

        node_type dec = 0;
        for (std::size_t i=0; i<stencil; ++i)
            dec = (dec|tmp) + bit;

        node_type move = (value&mask) + (dec|tmp);
        // if voidbit is True, keep it !!
        node_type is_void = ((value&voidbit)||(move&(~maskpos)))? voidbit: 0;
        return {static_cast<node_type>(((move&mask)&AllOnes[level()]) + keep + is_void)};
    }

    inline Node minus(direction d, std::size_t stencil=1) const{
        auto dummy = _get_dec(d);
        auto bit = dummy.first;
        auto mask = dummy.second;
        node_type tmp = (maskpos - mask);
        node_type keep = value&tmp + (value&levelzone);

        node_type dec = 0;
        for (std::size_t i=0; i<stencil; ++i)
            dec = (dec|tmp) + bit;

        node_type move = (value&mask) - (dec&mask);
        // if voidbit is True, keep it !!
        node_type is_void = ((value&voidbit)||(move&(~maskpos)))? voidbit: 0;
        return {static_cast<node_type>(((move&mask)&AllOnes[level()]) + keep + is_void)};
    }

    //! test if the node as max coordinate 
    //! \param d: the direction.
    inline bool is_max(direction d) const 
    {
        // is_max: all bits set to 1.
        node_type c = Ones[level()]>>static_cast<node_type>(d);
        return (value&c)==c;
    }
    //! test if the node as min coordinate 
    //! \param  d: direction.
    inline bool is_min(direction d) const
    {
        // is_min: all bits set to 0.
        node_type c = Ones[level()]>>static_cast<node_type>(d);
        return (value&c)==0;
    }

    inline std::size_t level() const
    {
        //return (value>>levelshift)&levelmask;
        return (value>>levelshift);
    }

    inline void set_level(std::size_t lev)
    {
        value = (value&maskpos) + (lev<<levelshift);
    }

    inline Node firstSon() const
    {
        return {value+levelone};
    }

    inline void boxNeighbor_impl(std::array<Node, 25> &P, int stencil, std::integral_constant<std::size_t, 2> const&) const
    {
        std::size_t index = 0;
        for(int j=-stencil; j<=stencil; j++)
        {
            Node ny{*this};

            if (j<0)
                ny = ny.minus(direction::y, -j);
            if (j>0)
                ny = ny.plus(direction::y, j);

            for(int i=-stencil; i<=stencil; i++)
            {
                Node nx{ny};
                if (i<0)
                    nx = ny.minus(direction::x, -i);
                if (i>0)
                    nx = ny.plus(direction::x, i);
    
                P[index++] = nx;
            }
        }
        //P[4].value += voidbit;
    }

    //! find a potential neighbor, depending on the position of u.
    //! \param  u: node.
    //! \param P[] returned list(vector)
    inline void boxNeighbor(std::array<Node, 25> &P, std::size_t stencil) const
    {
        boxNeighbor_impl(P, stencil, std::integral_constant<std::size_t, dim>{});
    }

    inline Node operator<<(std::size_t i) const
    {
        return {static_cast<node_type>(value<<i)};
    }

    inline Node& operator+=(Node<dim, node_type> const& node)
    {
        value += node.value;
        return *this;
    }

    inline std::size_t operator[](std::size_t i) const
    {
        return (value>>i)&1;
    }

    inline Node& operator-=(Node<dim, node_type> const& node)
    {
        value -= node.value;
        return *this;
    }

    inline bool operator&(Node<dim, node_type> const& node) const
    {
        return value&node.value;
    }

    friend std::ostream& operator<<(std::ostream &os, const Node &node)
    {
        std::string s;

        Node<dim, node_type> IntOne{1};//!<! 1! 

        for(int i=size-1;i>=0;i--)
        {
            if(node& (IntOne<<i))
                s+='1';
            else
                s+='0';
            if(i==dim*nlevels)
                s+="|";
            //else if(i==levelshift ||i==size-1)
            else if(i==levelshift)
                s+='.';
            else if(i%dim==0 && i>0 && i<dim*nlevels)
                s+='.';
        }

        os << s;
        return os;
    }
};

template <std::size_t dim, typename node_type>
inline Node<dim, node_type> operator+(Node<dim, node_type> const& node1, Node<dim, node_type> const& node2)
{
    Node<dim, node_type> res{node1};
    return res+=node2;
}

template <std::size_t dim, typename node_type>
inline Node<dim, node_type> operator-(Node<dim, node_type> const& node1, Node<dim, node_type> const& node2)
{
    Node<dim, node_type> res{node1};
    return res-=node2;
}

inline int indice(int i, int j)
{
    return 3*j+i+4;
}

// template<typename node_type> 
// inline void Node<2, node_type>::boxNeighbor(Node<2, node_type> P[], std::size_t stencil) const
// {
//     Node<2, node_type> n;
//     int depth = level();
//     std::size_t index = 0;
//     for(int j=-stencil; j<=stencil; j++)
//     {
//         Node<2> n{*this};
//         if (j<0)
//             n = n.minus(direction::y, j);
//         if (j>0)
//             n = n.plus(direction::y, j);
//         for(int i=stencil; i<=stencil; i++)
//         {
//             if (i<0)
//                 n = n.minus(direction::x, i);
//             if (i>0)
//                 n = n.plus(direction::x, i);
//             P[index++] = n;
//         }
//     }
//     P[4] = voidbit;
// }