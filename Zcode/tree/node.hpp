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
    using definitions<dim, node_type>::levelone;
    using definitions<dim, node_type>::maskpos;
    using definitions<dim, node_type>::voidbit;
    using definitions<dim, node_type>::Xbit;
    using definitions<dim, node_type>::Ybit;
    using definitions<dim, node_type>::Zbit;
    using definitions<dim, node_type>::Ones;
    
    node_type value;

    Node() = default;
    Node(const Node&) = default;

    Node(node_type i):value{i}{}

    inline Node _get_dec(direction d, int depth) const
    {
        Node Dec;
        // assert(dim==2 && d == direction::z 
        //        && "z direction can't be used for a 2 dimensional Node");
        switch(d)
        {
            case (direction::x):
                Dec = (Xbit>>(dim*depth)); break;
            case (direction::y):
                Dec = (Ybit>>(dim*depth)); break;
            case (direction::z):
                Dec = (Zbit>>(dim*depth)); break;
        }
        return Dec;        
    }
    inline Node plus(direction d, int depth) const{
        Node Dec = _get_dec(d, depth);
        return {(*this&Dec)? ((*this-Dec).plus(d, depth-1)) : (*this+Dec)};
    }

    inline Node minus(direction d, int depth) const{
        Node Dec = _get_dec(d, depth);
        return {(*this&Dec)? (*this-Dec) : ((*this+Dec).minus(d,depth-1))};
    }

    //! test if the node as max coordinate 
    //! \param d: the direction.
    inline bool is_max(direction d) const 
    {
        // is_max: all bits set to 1.
        int depth = level();
        Node c = Ones[depth]>>static_cast<std::size_t>(d);
        return (value&c.value)==c.value;
    }
    //! test if the node as min coordinate 
    //! \param  d: direction.
    inline bool is_min(direction d) const
    {
        // is_min: all bits set to 0.
        int depth = level();
        Node c = Ones[depth]>>static_cast<std::size_t>(d);
        return (value&c.value)==0;
    }

    inline std::size_t level() const
    {
        //return (value>>levelshift)&levelmask;
        return (value>>levelshift);
    }

    inline void set_level(std::size_t lev)
    {
        value = (value&maskpos)+(lev<<levelshift);
    }

    inline Node firstSon() const
    {
        return {value+levelone};
    }

    //! find a potential neighbor, depending on the position of u.
    //! \param  u: node.
    //! \param P[] returned list(vector)
    inline void listNeighbor(Node P[]) const;

    inline Node operator<<(std::size_t i) const
    {
        return {static_cast<node_type>(value<<i)};
    }

    inline Node& operator+=(Node const& node)
    {
        value += node.value;
        return *this;
    }

    inline std::size_t operator[](std::size_t i) const
    {
        return (value>>i)&1;
    }

    inline Node& operator-=(Node const& node)
    {
        value -= node.value;
        return *this;
    }

    inline bool operator&(Node const& node) const
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

        os << s << " " << "\n";
        return os;
    }
};

template <std::size_t dim, typename node_type>
inline Node<dim> operator+(Node<dim, node_type> const& node1, Node<dim, node_type> const& node2)
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

template<> 
inline void Node<2>::listNeighbor(Node P[]) const
{
    // returns possible neighbors of u, at the same level, or
    // a "void" Node, if no neighbor exists.
    int i1,i2,j1,j2;
    
    is_min(direction::x)? i1=0 : i1=-1;
    is_max(direction::x)? i2=0 : i2=1;
    is_min(direction::y)? j1=0 : j1=-1;
    is_max(direction::y)? j2=0 : j2=1;

    for(int i=0; i<nbneighb; i++)
        P[i] = voidbit;

    Node<2> n;
    int depth = level();
    for(int i=i1; i<=i2; i++)
        for(int j=j1; j<=j2; j++)
        {
            switch(i)
            {
                case -1:
                n = minus(direction::x, depth); break;
                case 0:
                n = *this; break;
                case 1:
                n = plus(direction::x, depth); break;
                default:
                n = 0; break;
            }
            Node<2> nr;
            switch(j)
            {
                case -1:
                nr = n.minus(direction::y, depth); break;
                case 0:
                nr = n; break;
                case 1:
                nr = n.plus(direction::y, depth); break;
                default:
                nr = 0; break;
            }
            P[indice(i, j)] = nr;
        }
    P[4] = voidbit;
}