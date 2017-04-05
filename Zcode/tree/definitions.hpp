#pragma once
#include <tree/AllSet2One.hpp>

#include <cassert>
#include <iostream>
#include <string>


template <std::size_t dim>
struct definitions{};

template <>
struct definitions<2>
{
    typedef unsigned long int Node;
    static const int size=64;//!< size of Node in digits
    static const int nlevels=24;//!< max number of tree levels.

    static const int treetype=4;//! say: we use quad trees.
    static const int nfaces=4;//!< number of faces of each element
    static const int nbbef=2;//!< number of boudary elements on each face.
    static const int nbd=12;//!< number max of bound. elements

    //how many nodes at level 0?
    static const int nlevZero=4;

    // first free bit:
    static const Node firstfreebit=Node(1L)<<(2*nlevels);

    //! amount of right shift needed to get level in the tree. 
    static const int levelshift=size-5;
    //!first digit marking levels:
    static const Node levelone=Node(1L)<<levelshift;
    //! mask for extracting level:
    static const Node levelmask=AllSet2One<2,4>::value;
    //! mask the part used to store level
    static const Node levelzone=(AllSet2One<2,4>::value)<<levelshift;
    //! mask for  what is used for position:
    static const Node maskpos=AllSet2One<2,2*nlevels>::value;

    //!right most digit of frees space used for tags.
    static const Node tagspace=Node(1L)<<(2*nlevels);
    //! how many neighbors for one Node (including itself).
    static const int nbneighb=9;
    //! bits used in nodes computations (Zbit not used -2d problem!-)
    static const Node Xbit=Node(1L)<<(2*nlevels-1);//!< leftmost digit for x position
    static const Node Ybit=(Xbit>>1);//!< leftmost digit for y position
    static const Node Zbit=0;//!<! not used (for compatibility with the 3-d case).
    static const Node XYZbit=Xbit+Ybit;
    static const Node IntOne=Node(1L);//!<! 1! 

    // other free bits:
    static const Node secondfreebit= (firstfreebit<<1);
    static const Node thirdfreebit = (firstfreebit<<2);
    static const Node fourthfreebit= (firstfreebit<<3);
    static const Node fifthfreebit= (firstfreebit<<4);
    static const Node partWithoutFreeBits=AllSet2One<2,size>::value
                -firstfreebit-secondfreebit-thirdfreebit-
                            fourthfreebit-fifthfreebit;
    static const Node FreeBitsPart=firstfreebit+secondfreebit+thirdfreebit+
                            fourthfreebit+fifthfreebit;
    
    // digit used to mark node as void.
    static const Node voidbit=fifthfreebit;
    static const int nbfreebits=5;
    //!mask for extracting all, but the void bit:
    static const Node AllExceptVoidbit=AllSet2One<2,size>::value-voidbit;

    static constexpr std::array<std::size_t, nlevels> Ones{Ones_array<nlevels>(2, Xbit)};
};

template <>
struct definitions<3>
{
    typedef unsigned long  int Node;

    static const int size=64;//!< size of Node in digits
    static const int nlevels=16;
    static const int treetype=8; //!<  we use octrees (do not change this).
    static const int nfaces=6;//!< number of faces ot each element
    static const int nbbef=4;//!< number of boudary elements on each face.
    static const int nbd=56;//!< number max of bound. elements

    //how many nodes at level 0?
    static const int nlevZero=8;

    
    // first free bit:
    static const Node firstfreebit=Node(1L)<<(3*nlevels);
    //! amount of right shift needed to get level in the tree. 
    static const int levelshift=size-5;//!< shift to get the level

    //!first digit marking levels:
    static const Node levelone=Node(1L)<<levelshift;

    //! mask for extracting level:
    static const Node levelmask=AllSet2One<3,4>::value;
    //! mask the part used to store level
    static const Node levelzone=(AllSet2One<3,4>::value)<<(levelshift);
    //! mask for  what is used for position:
    static const Node maskpos=AllSet2One<3,3*nlevels>::value;

    //!right most digit of frees space used for tags.
    static const Node tagspace=Node(1L)<<(3*nlevels);

    //! bits used in nodes computations:
    static const Node Xbit=Node(1L)<<(3*nlevels-1);//!< leftmost digit for x position
    static const Node Ybit=(Xbit>>1);//!< leftmost digit for y position
    static const Node Zbit=(Xbit>>2);//!< leftmost digit for z position
    static const Node XYZbit=Xbit+Ybit+Zbit;
    static const Node IntOne=Node(1L);//! 1!

    // other free bits:
    // note: 1 digit  to mark Node as void
    //       5 digits for the level
    //      48 digits for the absissa (3 times 16 levels).
    //      ---
    //      54 digits all together.
    // There remains 8 digits for various informations; here we give
    // a name only to 4 of them:
    static const Node secondfreebit= (firstfreebit<<1);
    static const Node thirdfreebit = (firstfreebit<<2);
    static const Node fourthfreebit= (firstfreebit<<3);
    static const Node fifthfreebit= (firstfreebit<<4);
    static const Node partWithoutFreeBits=AllSet2One<3,size>::value
                -firstfreebit-secondfreebit-thirdfreebit-fourthfreebit
                            -fifthfreebit;
    static const Node FreeBitsPart=firstfreebit+secondfreebit+thirdfreebit+
                            fourthfreebit+fifthfreebit;
    
    static const Node voidbit=fifthfreebit;
    static const int nbfreebits=5;
    
    //!mask for extracting all, but the void bit:
    static const Node AllExceptVoidbit=AllSet2One<3,size>::value-voidbit;
    //----

    //! how many neighbors for one Node (including itself).
    static const int nbneighb=27;

    static constexpr std::array<std::size_t, nlevels> Ones{Ones_array<nlevels>(3, Xbit)};
    
};

//template <std::size_t dim>
constexpr std::array<std::size_t, definitions<2>::nlevels> definitions<2>::Ones;
constexpr std::array<std::size_t, definitions<3>::nlevels> definitions<3>::Ones;
