#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <cassert>

#include <tree/node/definitions.hpp>
#include <tree/node/direction.hpp>

template <std::size_t Dim, typename node_type=std::size_t>
struct Node: public definitions<Dim, node_type>
{
    using definitions<Dim, node_type>::dim;    
    using typename definitions<Dim, node_type>::type;
    using definitions<Dim, node_type>::size;
    using definitions<Dim, node_type>::nlevels;
    using definitions<Dim, node_type>::levelshift;
    using definitions<Dim, node_type>::levelmask;
    using definitions<Dim, node_type>::levelzone;
    using definitions<Dim, node_type>::levelone;
    using definitions<Dim, node_type>::maskpos;
    using definitions<Dim, node_type>::voidbit;
    using definitions<Dim, node_type>::FreeBitsPart;
    using definitions<Dim, node_type>::partWithoutFreeBits;
    using definitions<Dim, node_type>::Xbit;
    using definitions<Dim, node_type>::Ybit;
    using definitions<Dim, node_type>::Zbit;
    using definitions<Dim, node_type>::XYZbit;
    using definitions<Dim, node_type>::XMask;
    using definitions<Dim, node_type>::YMask;
    using definitions<Dim, node_type>::ZMask;
    using definitions<Dim, node_type>::Ones;
    using definitions<Dim, node_type>::AllOnes;
    using definitions<Dim, node_type>::treetype;
    using definitions<Dim, node_type>::TailGen;
    
    node_type value=0;

    Node() = default;
    Node(const Node&) = default;

    Node(node_type i):value{i}{}

    Node(std::string const &bin_repr)
    {
        assert(bin_repr.size()<=size);
        std::cout << bin_repr << "\n";
        value = std::stoi(bin_repr.data(), 0, 2);
    }

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

    //! get the last level digits of a node in an int (flushed right).
    //! \param node 
    //! \note this can be applied to hashed and non hashed Nodes.
    inline std::size_t lastlevel() const
    {
        auto l = level();
        return (value&(XYZbit>>(dim*l)))>> (dim*(nlevels-l+1));
    }

    //! is a Node minimal (ie has minimal abscissa) in his set of Brothers?
    inline bool is_minimal() const
    {
        return !lastlevel();
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

    // test if the node is void.
    inline bool isVoid() const
    {
        return value&voidbit;
    }

    //! set the tag part of a Node
    //! \param N pointer to the Node.
    //! \param V tag value 
    //! \note we do not check V.
    inline void setTags(Node &n) const
    {
        n.value = ((n.value)&partWithoutFreeBits) + value&FreeBitsPart;
    }

    //! return the hash code for nodes.
    //! \param x Node
    //! \note we do not test if x is already hashed, except if DEBUG is set.
    inline Node hash() const
    {
        return value + (XYZbit>>(dim*(level()+1)));
    }
    //! For a given hasehd representation of a Node, we return the non
    //! hashed representation.
    //! \param x Node
    inline Node unhash() const
    {
        return value - (XYZbit>>(dim*(level()+1)));
    }

    //! Is a node hashed?
    inline bool isHashed() const 
    {
        return value&(XYZbit>>(dim*(level()+1)));
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

};

template <std::size_t dim, typename value_type=std::size_t>
std::ostream& operator<<(std::ostream &os, const Node<dim, value_type> &node)
{
    std::string s;

    using node_type = Node<dim, value_type>;

    const std::size_t size = node_type::size;
    const std::size_t nlevels = node_type::nlevels;
    const std::size_t levelshift = node_type::levelshift;

    node_type IntOne{1};//!<! 1! 

    for(int i=size-1;i>=0;i--)
    {
        if(node&(IntOne<<i))
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

template <typename node_type>
inline bool operator==(node_type const& node1, node_type const& node2)
{
    return (node1.value&node_type::partWithoutFreeBits)==(node2.value&node_type::partWithoutFreeBits);
}

template <typename node_type>
inline node_type operator&(node_type const& node1, node_type const& node2)
{
    using type = typename node_type::type;
    return {static_cast<type>(node1.value&node2.value)};
}

template <typename node_type>
inline bool operator<(node_type const& node1, node_type const& node2)
{
    return (node1.value<node2.value);
}

template <typename node_type>
inline bool operator<=(node_type const& node1, node_type const& node2)
{
    return (node1.value<=node2.value);
}

template <typename node_type>
inline bool operator>(node_type const& node1, node_type const& node2)
{
    return (node1.value>node2.value);
}

template <typename node_type>
inline bool operator>=(node_type const& node1, node_type const& node2)
{
    return (node1.value>=node2.value);
}
