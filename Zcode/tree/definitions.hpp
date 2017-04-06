#pragma once
#include <cmath>

#include <tree/AllSet2One.hpp>
#include <tree/zcurve.hpp>

template <std::size_t dim, typename node_type=std::size_t>
struct definitions
{
    static const int size = sizeof(node_type)*8;//!< size of Node in digits
    static const int nbfreebits = 5;
    static const int nblevelbits = max_level(dim, nbfreebits, size);
    static const int nlevels = (size-nbfreebits-nblevelbits)/dim;//!< max number of tree levels.

    static const int treetype = 1<<dim;//! bin (1d), quad(2d), octo (3d) trees.
    static const int nfaces = 2*dim;//!< number of faces of each element
    static const int nbbef = 1<<(dim-1);//!< number of elements when you refine once on each face (1d: 1, 2d: 2, 3d: 4).
    static const int nbd = ipow(2, dim) << dim;//!< number max of bound. elements (1d: 2, 2d: 12, 3d: 56).
       
    //how many nodes at level 0?
    static const int nlevZero = 1<<dim;
    static const node_type one = 1;
    static const node_type allone = static_cast<std::size_t>(~0);

    // first free bit:
    static const node_type firstfreebit = one<<(dim*nlevels);

    //! amount of right shift needed to get level in the tree. 
    static const int levelshift = size-nblevelbits;
    //!first digit marking levels:
    static const node_type levelone = one<<levelshift;
    //! mask for extracting level:
    static const node_type levelmask = AllSet2One<dim, nblevelbits, node_type>::value; 
    //! mask the part used to store level
    static const node_type levelzone = (levelmask)<<levelshift;
    //! mask for  what is used for position:
    static const node_type maskpos = AllSet2One<dim, dim*nlevels, node_type>::value;

    //! how many neighbors for one Node (including itself).
    static const int nbneighb = ipow(3, dim);
    //! bits used in nodes computations (Zbit not used -2d problem!-)
    static const node_type Xbit = one<<(dim*nlevels-1);//!< leftmost digit for x position
    static const node_type Ybit = (dim==1)? 0: (Xbit>>1);//!< leftmost digit for y position
    static const node_type Zbit = (dim==3)? (Xbit>>2): 0;//!<! not used (for compatibility with the 3-d case).
    static const node_type XYZbit=Xbit+Ybit+Zbit;
    static const node_type IntOne = one;//!<! 1! 

    // other free bits:
    static const node_type secondfreebit = (firstfreebit<<1);
    static const node_type thirdfreebit = (firstfreebit<<2);
    static const node_type fourthfreebit = (firstfreebit<<3);
    static const node_type fifthfreebit = (firstfreebit<<4);
    static const node_type FreeBitsPart = firstfreebit+secondfreebit+thirdfreebit+
                            fourthfreebit+fifthfreebit;
    //static const Node partWithoutFreeBits=AllSet2One<2,size>::value
    //            - FreeBitsPart;
    static const node_type partWithoutFreeBits = ~FreeBitsPart;
    
    // digit used to mark node as void.
    static const node_type voidbit = fifthfreebit;
    //!mask for extracting all, but the void bit:
    //static const node_type AllExceptVoidbit=AllSet2One<2,size>::value-voidbit;
    static const node_type AllExceptVoidbit= allone - voidbit;

    static constexpr std::array<std::size_t, nlevels> Ones{Ones_array<nlevels>(dim, Xbit)};
    static constexpr std::array<std::size_t, nlevels> AllOnes{Ones_array<nlevels>(dim, XYZbit)};
    static constexpr std::array<node_type, ipow(2, dim)> TailGen{zcurve<dim, node_type>(Xbit, Ybit, Zbit)};
    static const node_type XMask = Ones[nlevels-1]; 
    static const node_type YMask = XMask>>1;
    static const node_type ZMask = ZMask>>1;
};

template <std::size_t dim, typename node_type>
constexpr std::array<std::size_t, definitions<dim, node_type>::nlevels> definitions<dim, node_type>::Ones;

template <std::size_t dim, typename node_type>
constexpr std::array<std::size_t, definitions<dim, node_type>::nlevels> definitions<dim, node_type>::AllOnes;

template <std::size_t dim, typename node_type>
constexpr std::array<node_type, ipow(2, dim)> definitions<dim, node_type>::TailGen;
