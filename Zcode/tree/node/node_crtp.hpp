#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

#include "tree/node/definitions.hpp"

/// Z-curve tree based class.
/// @param TDerived Derived class type.
/// @param Dim      Space dimension.
/// @param TZValue  Z-curve value type.
template < typename TDerived,
           std::size_t Dim,
           typename TZValue = std::size_t >
class ZNode
{
public:
    static constexpr std::size_t dim = Dim;
    using derived_type  = TDerived;
    using zvalue_type   = TZValue;
    using definition    = definitions<dim, zvalue_type>;

    zvalue_type zvalue = 0;

    //! Returns the hash code of this node.
    inline zvalue_type hash() const
    {
        // TODO: check that | is equiv. to the original +
        return zvalue | ( definition::XYZbit >> (dim*(level()+1)) );
    }

    //! Returns the non-hashed code of this node;
    inline zvalue_type unhash() const
    {
        // TODO: check that &~ is equiv. to the original -
        return zvalue & ~( definition::XYZbit >> (dim*(level()+1)) );
    }

    /// Returns the znode level.
    inline std::size_t level() const
    {
        //return (value>>levelshift)&levelmask; // TODO ??
        return static_cast<std::size_t>( zvalue >> definition::levelshift );
    }

    /// Sets the level of the current znode.
    inline void setLevel( std::size_t l )
    {
        assert( lev < definition::nlevels );
        zvalue = (zvalue & definition::maskpos) + (l << definition::levelshift); //TODO: why resetting tags ?
    }

    /// Display node information
    /// @out    Output stream.
    void printValue( std::ostream & out ) const
    {
        std::string s;

        const std::size_t size       = definition::size;
        const std::size_t nlevels    = definition::nlevels;
        const std::size_t levelshift = definition::levelshift;

        zvalue_type IntOne{1};//!<! 1!

        for( int i = size-1; i >= 0; i-- )
        {
            if( zvalue & (IntOne<<i) )
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
    /// Returns mutable reference to the derived object.
    derived_type & derived()
    {
        return *static_cast<derived_type*>(this);
    }

    /// Returns constant reference to the derived object.
    derived_type const & derived() const
    {
        return *static_cast<derived_type const*>(this);
    }


public:
    /// Default constructor
    /// @param zvalue    Z-curve value.
    ZNode( zvalue_type zvalue )
        : zvalue(zvalue)
    {
    }

protected:
    /// Protected destructor (to avoid direct destruction).
    ~ZNode() = default;
};

/// Mesh cell.
/// @param Dim      Space dimension.
/// @param TValue   Z-curve value type.
template < std::size_t Dim, typename TZValue = std::size_t >
class Cell
    : public ZNode< Cell<Dim, TZValue>, Dim, TZValue >
{
public:
    using self_type  = Cell<Dim, TZValue>;
    using znode_type = ZNode< self_type, Dim, TZValue >;
    using znode_type::zvalue;
    using typename znode_type::zvalue_type;
    using znode_type::dim;


public:
    Cell( zvalue_type v = 0 )
        : znode_type( v )
    {
    }

};

template < std::size_t Dim, typename TZValue = std::size_t >
std::ostream & operator<< ( std::ostream & out, Cell<Dim, TZValue> const& node )
{
    out << "Cell: ";
    node.printValue(out);
    return out;
}

template < typename TChildren>
class Slot
    : public ZNode< Slot<TChildren>, TChildren::dim, typename TChildren::zvalue_type >,
      private std::vector< TChildren >
{
public:
    using children_type = TChildren;
    using self_type  = Slot<children_type>;
    using znode_type = ZNode< self_type, children_type::dim, typename children_type::zvalue_type >;

    using znode_type::zvalue;
    using typename znode_type::zvalue_type;
    using znode_type::dim;

    using container_type = std::vector< children_type >;

    using container_type::push_back;
    using container_type::operator[];
    using container_type::begin;
    using container_type::cbegin;
    using container_type::end;
    using container_type::cend;
    using container_type::reserve;

public:
    /// Constructor with memory reservation
    /// @param s1   Z-curve lower bound.
    /// @param size Reserved space.
    Slot( zvalue_type s1, std::size_t size )
        : znode_type{s1}
    {
        // TODO: checking that s1 is at the lower left corner of the corresponding cell !!!
        reserve(size);
    }

    /// Fuse this slot with his Z-neighboors.
    template < typename TBeginIterator, typename TEndIterator >
    void fuseWith( TBeginIterator it, TEndIterator const& it_end )
    {
        assert( unhash() == zvalue && "The resulting fused slot is not the first in his Z." );

        std::size_t cnt = 0; // TODO: for DEBUG only ...
        for ( ; it != it_end ; ++cnt, ++it )
        {
            assert( it->unhash() == zvalue && "The inserted slot is withing the wrong Z." );
            std::move( it->begin(), it->end(), std::back_inserter(*this) );
            it->clear();
        }

        assert( cnt == std::pow(2, dim)-1 && "This slot should be fused with his 2^dim-1 Z-neighboors." );

        //TODO: decreasing current slot level.
    }

    /// Returns the total number of cells in this slot.
    std::size_t nbCells() const
    {
        std::size_t cnt = 0;

        for ( auto const& children : *this )
            cnt += children.nbCells();

        return cnt;
    }
};

template < typename TChildren>
std::ostream & operator<< ( std::ostream & out, Slot<TChildren> const & slot )
{
    out << "Slot: ";
    slot.printValue( out );
    out << std::endl;

    for ( auto const& children : slot )
        out << "\t" << children << std::endl;

    return out;
}

//template < std::size_t
//using SlotCollection = Slot<2, std::size_t, std::shared< Slot<2, std::size_t, Node<2, std::size_t> > >;
