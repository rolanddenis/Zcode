#pragma once
#include <array>
/////////////////////////////////////////////////////////////////////////
///
/// Generate a number with all digits in range (0,n-1) set to one
///  (metaprogram).
///
/// Use it like: 
///
///  static const int k=AllSet2One<n>:value;
///  with n=3, this gives k=7. 
///
/// \brief generate a number with digits (0,n-1) set to 1.
////////////////////////////////////////////////////////////////////////
template<int dim,int n> struct AllSet2One
{
  typedef std::size_t Node;
  static const Node value= (AllSet2One<dim,n-1>::value<<1)+1;
};
template<int dim> struct AllSet2One<dim,0>
{
  typedef std::size_t Node;
  static const Node value= 0;
};

// Construction of Ones array
// C++ 14 version
template <std::size_t level>
constexpr std::size_t Ones(std::size_t dim, std::size_t bit) {
  return Ones<level-1>(dim, bit) + (bit>>(dim*level));
}

template <>
constexpr std::size_t Ones<0>(std::size_t dim, std::size_t bit) {
    return bit;
}

template<std::size_t... i>
constexpr auto Ones_array(std::size_t dim, std::size_t bit, std::index_sequence<i...>) {
    return std::array<std::size_t, sizeof...(i)>{{Ones<i>(dim, bit)...}};
}

template<std::size_t size>
constexpr auto Ones_array(std::size_t dim, std::size_t bit) {
    return Ones_array(dim, bit, std::make_index_sequence<size>{});
}

// // C++ 17 version
// template <std::size_t levels>
// auto Ones_array(std::size_t dim, std::size_t bit){
//   std::array<std::size_t, levels> a{bit};
//   for (std::size_t i=1; i<levels; i++)
//     a[i] = a[i-1] + bit>>(dim*i);
//   return a;
// }
