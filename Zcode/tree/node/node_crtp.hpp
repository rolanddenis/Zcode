#pragma once

#include <tree/node/definitions.hpp>
#include <tree/node/direction.hpp>

#include <vector>


template < typename TDerived,
           std::size_t Dim,
           typename TValue = std::size_t >
class ZNode
{
public:
    static constexpr std::size_t dim = Dim;
    using derived_type  = TDerived;
    using zvalue_type    = TValue;
    using definition    = definitions<dim, zvalue_type>;

    zvalue_type value = 0;

    inline auto _get_dec(direction d) const
    {
        zvalue_type bit = 0;
        zvalue_type mask = definition::XMask;
        switch(d)
        {
            case (direction::x):
                bit = definition::Xbit >> dim*level(); break;
            case (direction::y):
                bit = definition::Ybit >> dim*level(); mask >>= 1; break;
            case (direction::z):
                bit = definition::Zbit >> dim*level(); mask >>= 2; break;
        }
        return std::pair<zvalue_type, zvalue_type>{bit, mask};
    }

    inline zvalue_type plus(direction d, std::size_t stencil=1) const
    {
        if (stencil == 0)
            return value;

        auto dummy = _get_dec(d);
        auto bit = dummy.first;
        auto mask = dummy.second;
        zvalue_type tmp = (definition::maskpos - mask);
        zvalue_type keep = (value&tmp) + (value&definition::levelzone);

        zvalue_type dec = 0;
        for (std::size_t i=0; i<stencil; ++i)
            dec = (dec|tmp) + bit;

        zvalue_type move = (value&mask) + (dec|tmp);
        // if voidbit is True, keep it !!
        zvalue_type is_void = ((value&definition::voidbit)||(move&(~definition::maskpos)))? definition::voidbit: 0;
        return ((move&mask)&definition::AllOnes[level()]) + keep + is_void;
    }

    inline zvalue_type minus(direction d, std::size_t stencil=1) const{
        auto dummy = _get_dec(d);
        auto bit = dummy.first;
        auto mask = dummy.second;
        zvalue_type tmp = (definition::maskpos - mask);
        zvalue_type keep = (value&tmp) + (value&definition::levelzone);

        zvalue_type dec = 0;
        for (std::size_t i=0; i<stencil; ++i)
            dec = (dec|tmp) + bit;

        zvalue_type move = (value&mask) - (dec&mask);
        // if voidbit is True, keep it !!
        zvalue_type is_void = ((value&definition::voidbit)||(move&(~definition::maskpos)))? definition::voidbit: 0;
        return ((move&mask)&definition::AllOnes[level()]) + keep + is_void;
    }

    inline void setLevel(std::size_t lev)
    {
        assert( lev < definition::nlevels );
        value = (value&definition::maskpos) + (lev<<definition::levelshift);
    }

    // test if the znode is void.
    inline bool isVoid() const
    {
        return value&definition::voidbit;
    }

    //! set the tag part of a znode
    //! \param tags tag value
    inline void setTags(const zvalue_type & tags)
    {
        value |= tags&definition::FreeBitsPart;
    }

    //! suppress given tags
    //! \param tags 
    inline void unsetTags(const zvalue_type & tags)
    {
        value ^= (tags&definition::FreeBitsPart)&(value&definition::FreeBitsPart);
    }

    //! Suppress all the tags
    inline void clearAllTags()
    {
        value &= ~definition::FreeBitsPart;
    }

    //! return the hash code for znode.
    //! \note we do not test if x is already hashed, except if DEBUG is set.
    inline zvalue_type hash() const
    {
        return value + ( definition::XYZbit >> (dim*(level()+1)));
    }

    //! return the non hashed representation.
    inline zvalue_type unhash() const
    {
        return value - (definition::XYZbit>>(dim*(level()+1)));
    }

    //! Is a znode hashed?
    inline bool isHashed() const
    {
        return value&(definition::XYZbit>>(dim*(level()+1)));
    }

    //! test if the znode has max coordinate
    //! \param d: the direction.
    inline bool isMax(direction d) const
    {
        // isMax: all bits set to 1.
        zvalue_type c = definition::Ones[level()]>>static_cast<zvalue_type>(d);
        return (value&c)==c;
    }
    //! test if the znode has min coordinate
    //! \param  d: direction.
    inline bool is_min(direction d) const
    {
        // isMin: all bits set to 0.
        zvalue_type c = definition::Ones[level()]>>static_cast<zvalue_type>(d);
        return (value&c)==0;
    }

    //! get the last level digits of a znode in an int (flushed right).
    //! \param znode
    //! \note this can be applied to hashed and non hashed Cells.
    inline std::size_t lastlevel() const
    {
        auto l = level();
        return (value&(definition::XYZbit>>(dim*l)))>> (dim*(definition::nlevels-l+1));
    }

    //! is a node minimal (ie has minimal abscissa) in his set of Brothers?
    inline bool isMinimal() const
    {
        return !lastlevel();
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

        zvalue_type IntOne{1};//!<! 1!

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

    inline derived_type& operator+=(derived_type const& node)
    {
        value += node.value;
        return derived();
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
    ZNode( zvalue_type v = 0 )
        : value(v)
    {
    }

protected:
    ~ZNode() = default;
};


// template < std::size_t Dim, typename TValue = std::size_t >
// class Node
//     : public ZNode< Node<Dim, TValue>, Dim, TValue >
// {
// public:
//     using znode_type = ZNode< Node<Dim, TValue>, Dim, TValue >;
//     using znode_type::value;
//     using znode_type::zvalue_type;
//     using znode_type::ZNode;

// public:
//     Node( TValue v = 0 )
//         : znode_type( v )
//     {
//     }

// };

// template < std::size_t Dim, typename TValue = std::size_t >
// std::ostream & operator<< ( std::ostream & out, Node<Dim, TValue> const& node )
// {
//     out << "Node: ";
//     node.print_value(out);
//     return out;
// }

// //template < typename TChildren, std::size_t Dim, typename TValue = std::size_t >
// template < typename TChildren>
// class Slot
//     : public ZNode< Slot<TChildren>, TChildren::dim, typename TChildren::zvalue_type >,
//       private std::vector< TChildren >
// {
// public:
//     using znode_type = ZNode< Slot<TChildren>, TChildren::dim, typename TChildren::zvalue_type >;

//     using znode_type::value;
//     using zvalue_type = typename znode_type::zvalue_type;
//     //using znode_type::zvalue_type;

//     using container_type = std::vector< TChildren >;

//     using container_type::push_back;
//     using container_type::operator[];
//     using container_type::begin;
//     using container_type::cbegin;
//     using container_type::end;
//     using container_type::cend;
//     using container_type::reserve;

//     Slot( zvalue_type s1, std::size_t size )
//         : znode_type{s1}
//     {
//         reserve(size);
//     }
// };

// template < typename TChildren>
// std::ostream & operator<< ( std::ostream & out, Slot<TChildren> const & slot )
// {
//     out << "Slot: ";
//     slot.print_value( out );
//     out << std::endl;

//     for ( auto const& children : slot )
//         out << "\t" << children << std::endl;
    
//     return out;
// }

// //template < std::size_t 
// //using SlotCollection = Slot<2, std::size_t, std::shared< Slot<2, std::size_t, Node<2, std::size_t> > >;
