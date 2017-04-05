#pragma once
#include <tree/definitions.hpp>
#include <tree/direction.hpp>

template <std::size_t dim, typename node_type=unsigned long int>
struct Node: public definitions<dim>
{
    using definitions<dim>::nlevels;
    using definitions<dim>::levelshift;
    using definitions<dim>::levelmask;
    using definitions<dim>::levelone;
    using definitions<dim>::maskpos;
    using definitions<dim>::voidbit;
    using definitions<dim>::Xbit;
    using definitions<dim>::Ybit;
    using definitions<dim>::Zbit;
    using definitions<dim>::Ones;
    
    node_type value;

    Node() = default;
    Node(const Node&) = default;

    Node(std::size_t i):value{i}{}

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
        return {(*this&Dec)? ((*this-Dec).plus(d,depth-1)) : (*this+Dec)};
    }

    inline Node minus(direction d, int depth) const{
        Node Dec = _get_dec(d, depth);
        return {(*this&Dec)? (*this-Dec) : ((*this+Dec).minus(d,depth-1))};
    }

    inline std::size_t level() const
    {
        return (value>>levelshift)&levelmask;
    }

    inline void set_level(std::size_t lev)
    {
        value = (value&maskpos)+(lev<<levelshift);
    }

    inline Node firstSon() const
    {
        return {value+levelone};
    }

    inline Node operator<<(int i) const
    {
        return {value<<i};
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

        Node<dim> IntOne{1};//!<! 1! 

        for(int i=63;i>=0;i--)
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

template <std::size_t dim>
inline Node<dim> operator+(Node<dim> const& node1, Node<dim> const& node2)
{
    Node<dim> res{node1};
    return res+=node2;
}

template <std::size_t dim>
inline Node<dim> operator-(Node<dim> const& node1, Node<dim> const& node2)
{
    Node<dim> res{node1};
    return res-=node2;
}