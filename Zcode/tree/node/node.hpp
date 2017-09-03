#pragma once
#include <algorithm>
#include <iostream>
#include <string>
#include <cassert>

#include <tree/node/node_crtp.hpp>


template < std::size_t Dim, typename TValue = std::size_t >
class Cell
    : public ZNode< Cell<Dim, TValue>, Dim, TValue >
{
public:
    using znode_type = ZNode< Cell<Dim, TValue>, Dim, TValue >;
    using znode_type::value;
    using znode_type::zvalue_type;
    using znode_type::ZNode;
    using znode_type::dim;

public:
    Cell( TValue v = 0 )
        : znode_type( v )
    {
    }

};

// template <std::size_t Dim, typename Value=std::size_t>
// struct Cell: public definitions<Dim, Value>
// {
//     using value_type = Value;
//     using definitions<Dim, value_type>::dim;
//     using typename definitions<Dim, value_type>::type;
//     using definitions<Dim, value_type>::size;
//     using definitions<Dim, value_type>::nlevels;
//     using definitions<Dim, value_type>::levelshift;
//     using definitions<Dim, value_type>::levelmask;
//     using definitions<Dim, value_type>::levelzone;
//     using definitions<Dim, value_type>::levelone;
//     using definitions<Dim, value_type>::maskpos;
//     using definitions<Dim, value_type>::voidbit;
//     using definitions<Dim, value_type>::FreeBitsPart;
//     using definitions<Dim, value_type>::partWithoutFreeBits;
//     using definitions<Dim, value_type>::Xbit;
//     using definitions<Dim, value_type>::Ybit;
//     using definitions<Dim, value_type>::Zbit;
//     using definitions<Dim, value_type>::XYZbit;
//     using definitions<Dim, value_type>::XMask;
//     using definitions<Dim, value_type>::YMask;
//     using definitions<Dim, value_type>::ZMask;
//     using definitions<Dim, value_type>::Ones;
//     using definitions<Dim, value_type>::AllOnes;
//     using definitions<Dim, value_type>::treetype;
//     using definitions<Dim, value_type>::TailGen;

//     value_type value=0;

//     Cell() = default;
//     Cell(const Cell&) = default;
//     //Cell(Cell&& Cell) = default;

//     Cell(value_type i):value{i}{}

//     Cell(std::string const &bin_repr)
//     {
//         assert(bin_repr.size()<=size);
//         std::cout << bin_repr << "\n";
//         value = std::stoi(bin_repr.data(), 0, 2);
//     }

//     inline auto _get_dec(direction d) const
//     {
//         value_type bit = 0;
//         value_type mask = XMask;
//         switch(d)
//         {
//             case (direction::x):
//                 bit = Xbit >> dim*level(); break;
//             case (direction::y):
//                 bit = Ybit >> dim*level(); mask >>= 1; break;
//             case (direction::z):
//                 bit = Zbit >> dim*level(); mask >>= 2; break;
//         }
//         return std::pair<value_type, value_type>{bit, mask};
//     }

//     inline Cell plus(direction d, std::size_t stencil=1) const
//     {
//         if (stencil == 0)
//             return *this;

//         auto dummy = _get_dec(d);
//         auto bit = dummy.first;
//         auto mask = dummy.second;
//         value_type tmp = (maskpos - mask);
//         value_type keep = (value&tmp) + (value&levelzone);

//         value_type dec = 0;
//         for (std::size_t i=0; i<stencil; ++i)
//             dec = (dec|tmp) + bit;

//         value_type move = (value&mask) + (dec|tmp);
//         // if voidbit is True, keep it !!
//         value_type is_void = ((value&voidbit)||(move&(~maskpos)))? voidbit: 0;
//         return ((move&mask)&AllOnes[level()]) + keep + is_void;
//     }

//     inline Cell minus(direction d, std::size_t stencil=1) const{
//         auto dummy = _get_dec(d);
//         auto bit = dummy.first;
//         auto mask = dummy.second;
//         value_type tmp = (maskpos - mask);
//         value_type keep = (value&tmp) + (value&levelzone);

//         value_type dec = 0;
//         for (std::size_t i=0; i<stencil; ++i)
//             dec = (dec|tmp) + bit;

//         value_type move = (value&mask) - (dec&mask);
//         // if voidbit is True, keep it !!
//         value_type is_void = ((value&voidbit)||(move&(~maskpos)))? voidbit: 0;
//         return ((move&mask)&AllOnes[level()]) + keep + is_void;
//     }

//     inline Cell operator<<(std::size_t i) const
//     {
//         return {static_cast<value_type>(value<<i)};
//     }

//     inline Cell& operator+=(Cell<dim, value_type> const& Cell)
//     {
//         value += Cell.value;
//         return *this;
//     }

//     // inline Cell& operator=(Cell<dim, value_type> && Cell)
//     // {
//     //     value = std::move(Cell.value);
//     //     return *this;
//     // }

//     inline std::size_t operator[](std::size_t i) const
//     {
//         return (value>>i)&1;
//     }

//     inline Cell& operator-=(Cell<dim, value_type> const& Cell)
//     {
//         value -= Cell.value;
//         return *this;
//     }

//     inline bool operator&(Cell<dim, value_type> const& Cell) const
//     {
//         return value&Cell.value;
//     }

// };

// template <std::size_t dim, typename value_type=std::size_t>
// std::ostream& operator<<(std::ostream &os, const Cell<dim, value_type> &Cell)
// {
//     std::string s;

//     using Cell_type = Cell<dim, value_type>;

//     const std::size_t size = Cell_type::size;
//     const std::size_t nlevels = Cell_type::nlevels;
//     const std::size_t levelshift = Cell_type::levelshift;

//     Cell_type IntOne{1};//!<! 1!

//     for( int i = size-1; i >= 0; i-- )
//     {
//         if(Cell&(IntOne<<i))
//             s+='1';
//         else
//             s+='0';
//         if(i==dim*nlevels)
//             s+="|";
//         //else if(i==levelshift ||i==size-1)
//         else if(i==levelshift)
//             s+='.';
//         else if ( i%dim == 0 && i > 0 && static_cast<std::size_t>(i) < dim*nlevels)
//             s+='.';
//     }

//     os << s;
//     return os;
// }

template < std::size_t Dim, typename TValue >
std::ostream & operator<< ( std::ostream & out, Cell<Dim, TValue> const& cell )
{
    out << "Cell: ";
    cell.print_value(out);
    return out;
}

template <std::size_t dim, typename TValue>
inline bool operator==(Cell<dim, TValue> const& cell1, Cell<dim, TValue> const& cell2)
{
    using cell_type = Cell<dim, TValue>;
    using definition = definitions<dim, TValue>;
    
    return (cell1.value&definition::partWithoutFreeBits)==(cell2.value&definition::partWithoutFreeBits);
}

// template <std::size_t dim, typename value_type>
// inline Cell<dim, value_type> operator&(Cell<dim, value_type> const& Cell1, Cell<dim, value_type> const& Cell2)
// {
//     return {static_cast<value_type>(Cell1.value&Cell2.value)};
// }

// template <std::size_t dim, typename value_type>
// inline Cell<dim, value_type> operator&(Cell<dim, value_type> const& Cell, value_type const& value)
// {
//     return {static_cast<value_type>(Cell.value&value)};
// }

// template <std::size_t dim, typename value_type>
// inline bool operator<(Cell<dim, value_type> const& Cell1, Cell<dim, value_type> const& Cell2)
// {
//     return (Cell1.value<Cell2.value);
// }

// template <std::size_t dim, typename value_type>
// inline bool operator<=(Cell<dim, value_type> const& Cell1, Cell<dim, value_type> const& Cell2)
// {
//     return (Cell1.value<=Cell2.value);
// }

// template <std::size_t dim, typename value_type>
// inline bool operator>(Cell<dim, value_type> const& Cell1, Cell<dim, value_type> const& Cell2)
// {
//     return (Cell1.value>Cell2.value);
// }

// template <std::size_t dim, typename value_type>
// inline bool operator>=(Cell<dim, value_type> const& Cell1, Cell<dim, value_type> const& Cell2)
// {
//     return (Cell1.value>=Cell2.value);
// }
