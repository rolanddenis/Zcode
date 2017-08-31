#pragma once
#include <cmath>

#include <tree/node/util.hpp>
#include <tree/node/zcurve.hpp>

template <std::size_t Dim, typename value_type=std::size_t>
struct definitions
{
    static constexpr std::size_t dim = Dim;
    using type = value_type;
    static const int size = sizeof(value_type)*8;//!< size of Node in digits.
    static const int nbfreebits = 5;//!< number of freebits used for MR.
    static const int nblevelbits = max_level(dim, nbfreebits, size);//!< number of digit for level.
    static const int nlevels = (size-nbfreebits-nblevelbits)/dim;//!< max number of tree levels.

    static const int treetype = 1<<dim;//!< bin (1d), quad(2d), octo (3d) trees.
    static const int nfaces = 2*dim;//!< number of faces of each element
    static const int nbbef = 1<<(dim-1);//!< number of elements when you refine once on each face (1d: 1, 2d: 2, 3d: 4).
    static const int nbd = ipow(2, dim) << dim;//!< number max of bound. elements (1d: 2, 2d: 12, 3d: 56).
       
    //how many nodes at level 0?
    static const int nlevZero = 1<<dim;
    static const value_type one = 1;
    static const value_type allone = static_cast<std::size_t>(~0);

    // first free bit:
    static const value_type firstfreebit = one<<(dim*nlevels);

    //! amount of right shift needed to get level in the tree. 
    static const int levelshift = size-nblevelbits;
    //! first digit marking levels:
    static constexpr value_type levelone = one<<levelshift;
    //using levelone = std::integral_constant<value_type, one<<levelshift>;
    //! mask for extracting level:
    static const value_type levelmask = AllSet2One<dim, nblevelbits, value_type>::value; 
    //! mask the part used to store level
    static const value_type levelzone = (levelmask)<<levelshift;
    //! mask for  what is used for position:
    static constexpr value_type maskpos = AllSet2One<dim, dim*nlevels, value_type>::value;

    //! how many neighbors for one Node (including itself).
    static const int nbneighb = ipow(3, dim);
    //! bits used in nodes computations (Zbit not used -2d problem!-)
    static const value_type Xbit = one<<(dim*nlevels-1);//!< leftmost digit for x position
    static const value_type Ybit = (dim==1)? 0: (Xbit>>1);//!< leftmost digit for y position
    static const value_type Zbit = (dim==3)? (Xbit>>2): 0;//!<! not used (for compatibility with the 3-d case).
    static const value_type XYZbit=Xbit+Ybit+Zbit;
    static const value_type IntOne = one;//!<! 1! 

    // other free bits:
    static const value_type secondfreebit = (firstfreebit<<1);
    static const value_type thirdfreebit = (firstfreebit<<2);
    static const value_type fourthfreebit = (firstfreebit<<3);
    static const value_type fifthfreebit = (firstfreebit<<4);
    static const value_type FreeBitsPart = firstfreebit+secondfreebit+thirdfreebit+
                            fourthfreebit+fifthfreebit;
    static const value_type partWithoutFreeBits = ~FreeBitsPart;
    
    // digit used to mark node as void.
    static const value_type voidbit = fifthfreebit;
    //!mask for extracting all, but the void bit:
    static const value_type AllExceptVoidbit= allone - voidbit;

    static constexpr std::array<value_type, nlevels> Ones{Ones_array<nlevels>(dim, Xbit)};
    static constexpr std::array<value_type, nlevels> AllOnes{Ones_array<nlevels>(dim, XYZbit)};
    static constexpr std::array<value_type, ipow(2, Dim)> TailGen{zcurve<Dim>(Xbit, Ybit, Zbit)};
    static const value_type XMask = Ones[nlevels-1]; 
    static const value_type YMask = XMask>>1;
    static const value_type ZMask = YMask>>1;
};

template <std::size_t dim, typename value_type>
constexpr std::array<value_type, definitions<dim, value_type>::nlevels> definitions<dim, value_type>::Ones;

template <std::size_t dim, typename value_type>
constexpr std::array<value_type, definitions<dim, value_type>::nlevels> definitions<dim, value_type>::AllOnes;

template <std::size_t dim, typename value_type>
constexpr std::array<value_type, ipow(2, dim)> definitions<dim, value_type>::TailGen;

template <std::size_t dim, typename value_type>
constexpr value_type definitions<dim, value_type>::maskpos;

