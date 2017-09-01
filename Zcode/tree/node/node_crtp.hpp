#pragma once

#include <tree/node/definitions.hpp>
#include <vector>


template < typename TDerived,
           std::size_t Dim,
           typename TValue = std::size_t >
class ZNode
{
public:
    static constexpr std::size_t dim = Dim;
    using derived_type  = TDerived;
    using value_type    = TValue;
    using definition    = definitions<dim, value_type>;

    value_type value = 0;

    //! return the hash code for nodes.
    //! \param x Node
    //! \note we do not test if x is already hashed, except if DEBUG is set.
    inline value_type hash() const
    {
        return value + ( definition::XYZbit >> (dim*(level()+1)));
    }

    //! Return the znode level.
    inline std::size_t level() const
    {
        //return (value>>levelshift)&levelmask; // TODO ??
        return value >> definition::levelshift;
    }

    //! Display node information
    void print_value( std::ostream & out ) const
    {
        std::string s;

        const std::size_t size       = definition::size;
        const std::size_t nlevels    = definition::nlevels;
        const std::size_t levelshift = definition::levelshift;

        value_type IntOne{1};//!<! 1!

        for( int i = size-1; i >= 0; i-- )
        {
            if( value & (IntOne<<i) )
                s+='1';
            else
                s+='0';
            if(i == dim * nlevels)
                s+="|";
            //else if(i==levelshift ||i==size-1)
            else if(i == levelshift)
                s+='.';
            else if ( i%dim == 0 && i > 0 && static_cast<std::size_t>(i) < dim*nlevels)
                s+='.';
        }

        out << s;
    }


public:
    derived_type & derived()
    {
        return *static_cast<derived_type*>(this);
    }

    derived_type const & derived() const
    {
        return *static_cast<derived_type const*>(this);
    }


public:
    ZNode( value_type v = 0 )
        : value(v)
    {
    }

protected:
    ~ZNode() = default;
};


template < std::size_t Dim, typename TValue = std::size_t >
class Node
    : public ZNode< Node<Dim, TValue>, Dim, TValue >
{
public:
    using znode_type = ZNode< Node<Dim, TValue>, Dim, TValue >;
    using znode_type::value;
    using znode_type::value_type;
    using znode_type::ZNode;

public:
    Node( TValue v = 0 )
        : znode_type( v )
    {
    }

};

template < std::size_t Dim, typename TValue = std::size_t >
std::ostream & operator<< ( std::ostream & out, Node<Dim, TValue> const& node )
{
    out << "Node: ";
    node.print_value(out);
    return out;
}

//template < typename TChildren, std::size_t Dim, typename TValue = std::size_t >
template < typename TChildren>
class Slot
    : public ZNode< Slot<TChildren>, TChildren::dim, typename TChildren::value_type >,
      private std::vector< TChildren >
{
public:
    using znode_type = ZNode< Slot<TChildren>, TChildren::dim, typename TChildren::value_type >;

    using znode_type::value;
    using value_type = typename znode_type::value_type;
    //using znode_type::value_type;

    using container_type = std::vector< TChildren >;

    using container_type::push_back;
    using container_type::operator[];
    using container_type::begin;
    using container_type::cbegin;
    using container_type::end;
    using container_type::cend;
    using container_type::reserve;

    Slot( value_type s1, std::size_t size )
        : znode_type{s1}
    {
        reserve(size);
    }
};

template < typename TChildren>
std::ostream & operator<< ( std::ostream & out, Slot<TChildren> const & slot )
{
    out << "Slot: ";
    slot.print_value( out );
    out << std::endl;

    for ( auto const& children : slot )
        out << "\t" << children << std::endl;
    
    return out;
}

//template < std::size_t 
//using SlotCollection = Slot<2, std::size_t, std::shared< Slot<2, std::size_t, Node<2, std::size_t> > >;
